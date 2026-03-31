/**
 * praat-wasm Web Worker.
 *
 * Runs the WASM module in a dedicated Worker thread.
 * The main thread communicates via postMessage with a simple RPC protocol.
 *
 * Message format (main -> worker):
 *   { id: number, method: string, args: any[] }
 *
 * Message format (worker -> main):
 *   { id: number, result?: any, error?: string }
 *
 * For audio data, ArrayBuffers are transferred (zero-copy) via Transferable.
 *
 * @module praat-wasm/worker
 */

/* global self, importScripts */

import { createPraatWasm } from './praat-wasm.mjs'

/** @type {import('./praat-wasm.mjs').PraatWasmInstance|null} */
let instance = null

/** @type {Promise<import('./praat-wasm.mjs').PraatWasmInstance>|null} */
let initPromise = null

/**
 * Initialize the Praat instance (lazy, once).
 * @param {string} [wasmUrl]
 * @returns {Promise<import('./praat-wasm.mjs').PraatWasmInstance>}
 */
function ensureInit (wasmUrl) {
  if (instance) return Promise.resolve(instance)
  if (!initPromise) {
    initPromise = createPraatWasm(wasmUrl).then(inst => {
      instance = inst
      return inst
    })
  }
  return initPromise
}

self.onmessage = async function (/** @type {MessageEvent} */ e) {
  const { id, method, args } = e.data

  try {
    const praat = await ensureInit(args && args[0] && args[0].__wasmUrl)

    /** @type {any} */
    let result

    switch (method) {
      case 'init':
        result = true
        break

      case 'run':
        result = praat.run(args[0])
        break

      case 'call':
        result = praat.call(args[0], ...args.slice(1))
        break

      case 'list':
        result = praat.list()
        break

      case 'select':
        praat.select(args[0])
        result = undefined
        break

      case 'readAudio': {
        /*
          args[0] is an ArrayBuffer (transferred from main thread).
          args[1] is an optional filename.
        */
        result = praat.readAudio(args[0], args[1])
        break
      }

      case 'readFile':
        result = praat.readFile(args[0])
        break

      case 'writeFile':
        praat.writeFile(args[0], new Uint8Array(args[1]))
        result = undefined
        break

      case 'getFile': {
        const data = praat.getFile(args[0])
        const buffer = data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength)
        self.postMessage({ id, result: buffer }, [buffer])
        return
      }

      case 'saveAsText':
        praat.saveAsText(args[0])
        result = undefined
        break

      case 'saveAsBinary':
        praat.saveAsBinary(args[0])
        result = undefined
        break

      case 'removeAll':
        praat.removeAll()
        result = undefined
        break

      case 'removeSelected':
        praat.removeSelected()
        result = undefined
        break

      case 'destroy':
        praat.destroy()
        instance = null
        initPromise = null
        result = undefined
        break

      default:
        throw new Error('Unknown method: ' + method)
    }

    self.postMessage({ id, result })
  } catch (err) {
    self.postMessage({ id, error: err.message || String(err) })
  }
}
