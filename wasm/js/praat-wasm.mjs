/**
 * praat-wasm — Praat phonetic analysis library for JavaScript.
 *
 * Main entry point. Loads the WASM module and provides an ergonomic API
 * that mirrors Parselmouth's Python interface.
 *
 * @module praat-wasm
 */

/* global globalThis */

/**
 * @typedef {Object} PraatObject
 * @property {number} id - Praat object ID.
 * @property {string} type - Class name (e.g. "Sound", "Pitch", "TextGrid").
 * @property {string} name - Object name.
 */

/**
 * @typedef {Object} PraatWasmInstance
 * @property {function(string): string} run - Execute a Praat script, return Info window contents.
 * @property {function(string, ...any): string} call - Execute a Praat command on selected object(s).
 * @property {function(): PraatObject[]} list - List all objects in the Praat object list.
 * @property {function(number): void} select - Select an object by ID.
 * @property {function(ArrayBuffer, string=): PraatObject} readAudio - Load audio from an ArrayBuffer.
 * @property {function(string): PraatObject} readFile - Read a file previously written to MEMFS.
 * @property {function(string): void} saveAsText - Save selected object as text to MEMFS path.
 * @property {function(string): void} saveAsBinary - Save selected object as binary to MEMFS path.
 * @property {function(string): Uint8Array} getFile - Read a file from MEMFS as bytes.
 * @property {function(string, Uint8Array): void} writeFile - Write bytes to a MEMFS path.
 * @property {function(): void} removeAll - Remove all objects.
 * @property {function(): void} removeSelected - Remove selected objects.
 * @property {Object} FS - Direct access to the Emscripten MEMFS filesystem.
 * @property {function(): void} destroy - Clean up and free resources.
 */

/**
 * @param {string|URL} [wasmUrl] - Optional URL to the praat.wasm file.
 *   Defaults to loading from the same directory as this module.
 * @returns {Promise<PraatWasmInstance>}
 */
export async function createPraatWasm (wasmUrl) {
  /** @type {any} */
  let PraatModule

  /*
    Determine the correct way to load the WASM glue script.
    In Node.js we use dynamic import; in browsers we use the module directly.
  */
  const isNode = typeof globalThis.process !== 'undefined' &&
    typeof globalThis.process.versions !== 'undefined' &&
    typeof globalThis.process.versions.node !== 'undefined'

  if (isNode) {
    const { default: createModule } = await import('../dist/praat.mjs')
    PraatModule = await createModule(wasmUrl ? { locateFile: () => wasmUrl } : undefined)
  } else {
    /*
      In browsers, import the Emscripten glue.
      The WASM binary is loaded automatically from the same directory.
    */
    const { default: createModule } = await import('../dist/praat.mjs')
    const moduleConfig = {}
    if (wasmUrl) {
      moduleConfig.locateFile = (/** @type {string} */ _path) => wasmUrl.toString()
    }
    PraatModule = await createModule(moduleConfig)
  }

  /*
    Initialize Praat's command system.
  */
  PraatModule.praatInit()

  let nextFileId = 1

  /**
   * Parse the object list string from C++ into structured objects.
   * @param {string} listStr
   * @returns {PraatObject[]}
   */
  function parseObjectList (listStr) {
    if (!listStr || !listStr.trim()) return []
    return listStr.trim().split('\n').map(line => {
      const parts = line.split(' ')
      const id = parseInt(parts[0], 10)
      const type = parts[1] || ''
      const name = parts.slice(2).join(' ')
      return { id, type, name }
    })
  }

  /**
   * Get the most recently added object (highest ID).
   * @returns {PraatObject|null}
   */
  function getLastObject () {
    const objects = parseObjectList(PraatModule.listObjects())
    if (objects.length === 0) return null
    return objects[objects.length - 1]
  }

  /** @type {PraatWasmInstance} */
  const instance = {
    run (scriptText) {
      return PraatModule.executeScript(scriptText)
    },

    call (command, ...args) {
      /*
        Build a Praat command line from the command and arguments.
        If arguments are provided, format them with colon syntax:
          "To Pitch...", 0, 75, 600  =>  "To Pitch: 0, 75, 600"
      */
      let fullCommand
      if (args.length > 0) {
        const cleanCommand = command.replace(/\.\.\.$/g, '').trim()
        const formattedArgs = args.map(a => {
          if (typeof a === 'string') return '"' + a + '"'
          return String(a)
        }).join(', ')
        fullCommand = cleanCommand + ': ' + formattedArgs + '\n'
      } else {
        fullCommand = command + '\n'
      }
      return PraatModule.call(fullCommand)
    },

    list () {
      return parseObjectList(PraatModule.listObjects())
    },

    select (id) {
      PraatModule.selectObject(id)
    },

    readAudio (arrayBuffer, filename) {
      const fname = filename || ('/tmp/input_' + (nextFileId++) + '.wav')
      const data = new Uint8Array(arrayBuffer)
      PraatModule.FS.writeFile(fname, data)
      PraatModule.readFile(fname)
      const obj = getLastObject()
      try { PraatModule.FS.unlink(fname) } catch (_e) { /* ignore */ }
      return obj
    },

    readFile (path) {
      PraatModule.readFile(path)
      return getLastObject()
    },

    saveAsText (path) {
      PraatModule.saveAsText(path)
    },

    saveAsBinary (path) {
      PraatModule.saveAsBinary(path)
    },

    getFile (path) {
      return PraatModule.FS.readFile(path)
    },

    writeFile (path, data) {
      PraatModule.FS.writeFile(path, data)
    },

    removeAll () {
      PraatModule.removeAllObjects()
    },

    removeSelected () {
      PraatModule.removeSelectedObjects()
    },

    get FS () {
      return PraatModule.FS
    },

    destroy () {
      PraatModule.removeAllObjects()
    }
  }

  return instance
}

export default createPraatWasm
