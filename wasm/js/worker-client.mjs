/**
 * praat-wasm Web Worker client.
 *
 * Creates a Worker running the WASM module and provides a Promise-based
 * API that mirrors the direct PraatWasmInstance interface.
 *
 * All heavy computation runs off the main thread.
 *
 * @module praat-wasm/worker-client
 */

/**
 * @typedef {import('./praat-wasm.mjs').PraatObject} PraatObject
 */

/**
 * @typedef {Object} PraatWorkerInstance
 * @property {function(string): Promise<string>} run
 * @property {function(string, ...any): Promise<string>} call
 * @property {function(): Promise<PraatObject[]>} list
 * @property {function(number): Promise<void>} select
 * @property {function(ArrayBuffer, string=): Promise<PraatObject>} readAudio
 * @property {function(string): Promise<PraatObject>} readFile
 * @property {function(string): Promise<void>} saveAsText
 * @property {function(string): Promise<void>} saveAsBinary
 * @property {function(string): Promise<Uint8Array>} getFile
 * @property {function(string, Uint8Array): Promise<void>} writeFile
 * @property {function(): Promise<void>} removeAll
 * @property {function(): Promise<void>} removeSelected
 * @property {function(): void} destroy
 */

/**
 * Create a Praat instance running in a Web Worker.
 *
 * @param {string|URL} [workerUrl] - URL to the worker.mjs script.
 *   Defaults to './worker.mjs' relative to this module.
 * @param {string|URL} [wasmUrl] - Optional URL to the praat.wasm binary,
 *   passed through to the worker.
 * @returns {Promise<PraatWorkerInstance>}
 */
export async function createPraatWorker (workerUrl, wasmUrl) {
  const url = workerUrl || new URL('./worker.mjs', import.meta.url)
  const worker = new Worker(url, { type: 'module' })

  let nextId = 1
  /** @type {Map<number, {resolve: function, reject: function}>} */
  const pending = new Map()

  worker.onmessage = (/** @type {MessageEvent} */ e) => {
    const { id, result, error } = e.data
    const p = pending.get(id)
    if (!p) return
    pending.delete(id)
    if (error) {
      p.reject(new Error(error))
    } else {
      p.resolve(result)
    }
  }

  worker.onerror = (/** @type {ErrorEvent} */ e) => {
    /* Reject all pending requests on worker error */
    const err = new Error(e.message || 'Worker error')
    for (const [, p] of pending) {
      p.reject(err)
    }
    pending.clear()
  }

  /**
   * Send an RPC call to the worker.
   * @param {string} method
   * @param {any[]} args
   * @param {Transferable[]} [transfer]
   * @returns {Promise<any>}
   */
  function rpc (method, args, transfer) {
    return new Promise((resolve, reject) => {
      const id = nextId++
      pending.set(id, { resolve, reject })
      if (transfer) {
        worker.postMessage({ id, method, args }, transfer)
      } else {
        worker.postMessage({ id, method, args })
      }
    })
  }

  /* Initialize the WASM module in the worker */
  const initArgs = wasmUrl ? [{ __wasmUrl: wasmUrl.toString() }] : []
  await rpc('init', initArgs)

  /** @type {PraatWorkerInstance} */
  const instance = {
    run (scriptText) {
      return rpc('run', [scriptText])
    },

    call (command, ...args) {
      return rpc('call', [command, ...args])
    },

    list () {
      return rpc('list', [])
    },

    select (id) {
      return rpc('select', [id])
    },

    readAudio (arrayBuffer, filename) {
      /*
        Transfer the ArrayBuffer to the worker (zero-copy).
      */
      return rpc('readAudio', [arrayBuffer, filename], [arrayBuffer])
    },

    readFile (path) {
      return rpc('readFile', [path])
    },

    saveAsText (path) {
      return rpc('saveAsText', [path])
    },

    saveAsBinary (path) {
      return rpc('saveAsBinary', [path])
    },

    getFile (path) {
      return rpc('getFile', [path]).then(buf => new Uint8Array(buf))
    },

    writeFile (path, data) {
      const buffer = data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength)
      return rpc('writeFile', [path, buffer], [buffer])
    },

    removeAll () {
      return rpc('removeAll', [])
    },

    removeSelected () {
      return rpc('removeSelected', [])
    },

    destroy () {
      rpc('destroy', []).catch(() => {})
      worker.terminate()
    }
  }

  return instance
}

export default createPraatWorker
