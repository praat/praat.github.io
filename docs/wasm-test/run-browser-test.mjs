/**
 * Headless Chrome browser test runner for praat-wasm.
 * Runs wasm-test.html in Chrome headless and captures results.
 *
 * Usage: node docs/run-browser-test.mjs
 * Requires: Chrome installed, docs/serve-test.mjs running on port 8000
 */

import { execFile } from 'node:child_process'

const CHROME = '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome'
const URL = 'http://localhost:8000/docs/wasm-test/'
const TIMEOUT = 120_000

/**
 * Evaluate a JS expression in Chrome via CDP WebSocket.
 * @param {string} wsUrl
 * @param {string} expression
 * @returns {Promise<string>}
 */
function cdpEval (wsUrl, expression) {
  return new Promise((resolve, reject) => {
    /* Use built-in WebSocket (Node 22+) */
    const ws = new WebSocket(wsUrl)
    ws.onopen = () => {
      ws.send(JSON.stringify({
        id: 1,
        method: 'Runtime.evaluate',
        params: { expression, returnByValue: true }
      }))
    }
    ws.onmessage = (evt) => {
      try {
        const msg = JSON.parse(evt.data)
        if (msg.id === 1) {
          ws.close()
          if (msg.result && msg.result.result) {
            resolve(msg.result.result.value || '')
          } else {
            resolve('')
          }
        }
      } catch (e) {
        ws.close()
        reject(e)
      }
    }
    ws.onerror = (e) => reject(new Error('WebSocket error'))
    setTimeout(() => { try { ws.close() } catch (_) {} reject(new Error('CDP timeout')) }, 5000)
  })
}

async function runTest () {
  console.log('Running browser tests via Chrome headless...')
  console.log('URL: ' + URL)

  /*
    Use Chrome DevTools Protocol (CDP) via --remote-debugging-port so that
    Web Workers are properly scheduled (unlike --virtual-time-budget which
    only advances time on the main thread).
  */
  const debugPort = 9222 + Math.floor(Math.random() * 1000)

  const args = [
    '--headless=new',
    '--disable-gpu',
    '--no-sandbox',
    `--remote-debugging-port=${debugPort}`,
    URL
  ]

  return new Promise((resolve, reject) => {
    const timer = setTimeout(() => {
      child.kill()
      reject(new Error('Browser test timed out after ' + TIMEOUT + 'ms'))
    }, TIMEOUT)

    const child = execFile(CHROME, args, { timeout: TIMEOUT })

    /* Poll the page title via CDP to detect completion */
    async function pollForCompletion () {
      /* Wait for Chrome to start */
      await new Promise(r => setTimeout(r, 3000))

      for (let attempt = 0; attempt < 200; attempt++) {
        try {
          const resp = await fetch(`http://127.0.0.1:${debugPort}/json`)
          const tabs = await resp.json()
          const tab = tabs.find(t => t.url.includes('wasm-test')  || t.url.includes('docs/wasm-test'))
          if (!tab) { await new Promise(r => setTimeout(r, 1000)); continue }

          /* Use WebSocket to send CDP command */
          const wsUrl = tab.webSocketDebuggerUrl
          const result = await cdpEval(wsUrl, 'document.getElementById("summary").textContent')

          if (result && result.match(/\d+ passed/)) {
            /* Get the full log */
            const logText = await cdpEval(wsUrl, 'document.getElementById("log").textContent')
            clearTimeout(timer)
            child.kill()
            console.log('=== Browser Output ===')
            console.log(logText)
            resolve(logText)
            return
          }
        } catch (_) { /* Chrome not ready yet */ }
        await new Promise(r => setTimeout(r, 1000))
      }
      clearTimeout(timer)
      child.kill()
      reject(new Error('Timed out waiting for results'))
    }

    pollForCompletion().catch(reject)
  })
}

runTest().then(output => {
  /* Parse pass/fail from the output */
  const match = output.match(/(\d+)\s+passed,\s+(\d+)\s+failed/)
  if (match) {
    const [, p, f] = match
    console.log('\n=== Browser Test Results ===')
    console.log(p + ' passed, ' + f + ' failed')
    process.exit(parseInt(f) > 0 ? 1 : 0)
  } else {
    console.log('\nCould not parse results from browser output')
    /* Check for loading state - might need more time */
    if (output.includes('Loading WASM')) {
      console.log('WASM module was still loading. Try increasing --virtual-time-budget')
    }
    process.exit(1)
  }
}).catch(e => {
  console.error('Test runner error:', e.message)
  process.exit(1)
})
