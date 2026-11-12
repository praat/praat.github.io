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

      case 'call': {
        /*
          args can be:
            [command, ...commandArgs]                 -- string command
            [{id, type, name}, command, ...commandArgs]  -- object + command
            [[{id, type, name}, ...], command, ...]      -- array of objects + command
        */
        const first = args[0]
        if (typeof first === 'object' && first !== null && !Array.isArray(first) && 'id' in first) {
          /* Single object reference — select it first */
          praat.select(first.id)
          result = praat.call(args[1], ...args.slice(2))
        } else if (Array.isArray(first) && first.length > 0 && first[0].id != null) {
          /* Array of object references */
          praat.select(first[0].id)
          for (let i = 1; i < first.length; i++) {
            praat.run('plusObject: ' + first[i].id + '\n')
          }
          result = praat.call(args[1], ...args.slice(2))
        } else {
          result = praat.call(args[0], ...args.slice(1))
        }
        /* Serialize PraatObject instances back to plain descriptors */
        if (result && typeof result === 'object' && result.id != null && result.className != null) {
          result = { id: result.id, type: result.type || result.className, name: result.name }
        } else if (Array.isArray(result)) {
          result = result.map(r =>
            r && typeof r === 'object' && r.id != null && r.className != null
              ? { id: r.id, type: r.type || r.className, name: r.name }
              : r
          )
        }
        break
      }

      case 'list':
        result = praat.list().map(o => ({ id: o.id, type: o.type || o.className, name: o.name }))
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
        const obj = praat.readAudio(args[0], args[1])
        result = obj ? { id: obj.id, type: obj.type || obj.className, name: obj.name } : null
        break
      }

      case 'readFile': {
        const obj = praat.readFile(args[0])
        result = obj ? { id: obj.id, type: obj.type || obj.className, name: obj.name } : null
        break
      }

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

      case 'createTextGrid': {
        const tg = praat.createTextGrid(args[0], args[1], args[2], args[3])
        result = tg ? { id: tg.id, type: tg.type || tg.className, name: tg.name } : null
        break
      }

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
