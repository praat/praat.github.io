/**
 * praat-wasm — Praat phonetic analysis library for JavaScript.
 *
 * Main entry point. Loads the WASM module and provides an ergonomic API
 * that mirrors Parselmouth's Python interface.
 *
 * @module praat-wasm
 */

/* global globalThis */

import { Sound, Pitch, Formant, Intensity, Harmonicity, Spectrum, Spectrogram,
  MFCC, TextGrid, PointProcess, LPC, Matrix, Table,
  PraatObject, wrapObject } from './classes.mjs'

export { Sound, Pitch, Formant, Intensity, Harmonicity, Spectrum, Spectrogram,
  MFCC, TextGrid, PointProcess, LPC, Matrix, Table, PraatObject }

/**
 * Package version (Praat patch × 100 + JS revision).
 * @type {string}
 */
export const version = '6.4.6201'

/**
 * @typedef {Object} PraatWasmInstance
 * @property {function(string): string} run - Execute a Praat script, return Info window contents.
 * @property {function(PraatObject|PraatObject[]|string, ...any): *} call - Execute a Praat command.
 * @property {function(): PraatObject[]} list - List all objects.
 * @property {function(number): void} select - Select an object by ID.
 * @property {function(ArrayBuffer, string=): Sound} readAudio - Load audio from an ArrayBuffer.
 * @property {function(string): PraatObject} readFile - Read a file from MEMFS.
 * @property {function(number, number, string=, string=): TextGrid} createTextGrid
 * @property {function(string): void} saveAsText - Save selected to MEMFS path.
 * @property {function(string): void} saveAsBinary - Save selected to MEMFS path.
 * @property {function(string): Uint8Array} getFile - Read from MEMFS.
 * @property {function(string, Uint8Array): void} writeFile - Write to MEMFS.
 * @property {function(): void} removeAll - Remove all objects.
 * @property {function(): void} removeSelected - Remove selected objects.
 * @property {Object} FS - Direct Emscripten MEMFS access.
 * @property {function(): void} destroy - Clean up.
 */

/**
 * Create a Praat WASM instance.
 *
 * @param {string|URL} [wasmUrl] - Optional URL to the praat.wasm file.
 * @returns {Promise<PraatWasmInstance>}
 *
 * @example
 * import { createPraatWasm } from 'praat-wasm';
 *
 * const praat = await createPraatWasm();
 *
 * // Load audio
 * const sound = praat.readAudio(wavBytes);
 *
 * // OOP-style (mirrors Parselmouth)
 * const pitch = sound.toPitch();
 * const f0 = pitch.getValueAtTime(0.5);
 *
 * // praat.call()-style (mirrors Parselmouth's praat.call())
 * const formant = praat.call(sound, "To Formant (burg)...", 0, 5, 5500, 0.025, 50);
 * const f1 = praat.call(formant, "Get value at time...", 1, 0.5, "hertz", "Linear");
 *
 * // Script-style
 * praat.run('Create Sound as pure tone: "tone", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
 */
export async function createPraatWasm (wasmUrl) {
  /** @type {any} */
  let PraatModule

  const isNode = typeof globalThis.process !== 'undefined' &&
    typeof globalThis.process.versions !== 'undefined' &&
    typeof globalThis.process.versions.node !== 'undefined'

  if (isNode) {
    const { default: createModule } = await import('../dist/praat.mjs')
    PraatModule = await createModule(wasmUrl ? { locateFile: () => wasmUrl } : undefined)
  } else {
    const { default: createModule } = await import('../dist/praat.mjs')
    const moduleConfig = {}
    if (wasmUrl) {
      moduleConfig.locateFile = (/** @type {string} */ _path) => wasmUrl.toString()
    }
    PraatModule = await createModule(moduleConfig)
  }

  PraatModule.praatInit()

  let nextFileId = 1

  /**
   * Parse the object list string from C++ into raw descriptors.
   * @param {string} listStr
   * @returns {{id: number, type: string, name: string}[]}
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
   * Get the most recently added object.
   * @returns {{id: number, type: string, name: string}|null}
   */
  function getLastObject () {
    const objects = parseObjectList(PraatModule.listObjects())
    if (objects.length === 0) return null
    return objects[objects.length - 1]
  }

  /**
   * Format a single argument for Praat command syntax.
   * @param {*} a
   * @returns {string}
   */
  function formatArg (a) {
    if (typeof a === 'boolean') return a ? '"yes"' : '"no"'
    if (typeof a === 'string') return '"' + a.replace(/"/g, '""') + '"'
    return String(a)
  }

  /** @type {PraatWasmInstance} */
  const instance = {
    /**
     * Execute a Praat script and return the Info window contents.
     * @param {string} scriptText
     * @returns {string}
     */
    run (scriptText) {
      return PraatModule.executeScript(scriptText)
    },

    /**
     * Execute a Praat command, with Parselmouth-compatible signatures.
     *
     * Signatures:
     *   call(object, "Command...", arg1, arg2, ...)
     *   call([obj1, obj2], "Command...", arg1, arg2, ...)
     *   call("Command...", arg1, arg2, ...)
     *
     * Returns the appropriate type based on command result:
     *   - New object(s): wrapped PraatObject (or array)
     *   - Query result: parsed number or string
     *   - Void command: undefined
     *
     * @param {PraatObject|PraatObject[]|string} objectOrCommand
     * @param {...(string|number|boolean)} args
     * @returns {*}
     */
    call (objectOrCommand, ...args) {
      let command, commandArgs

      if (typeof objectOrCommand === 'string') {
        command = objectOrCommand
        commandArgs = args
      } else {
        const objects = Array.isArray(objectOrCommand)
          ? objectOrCommand
          : [objectOrCommand]

        command = /** @type {string} */ (args[0])
        commandArgs = args.slice(1)

        /* Select the objects */
        if (objects.length > 0) {
          const first = objects[0]
          const fid = first instanceof PraatObject ? first.id : first.id
          PraatModule.selectObject(fid)
          for (let i = 1; i < objects.length; i++) {
            const obj = objects[i]
            const oid = obj instanceof PraatObject ? obj.id : obj.id
            PraatModule.executeScript('plusObject: ' + oid + '\n')
          }
        }
      }

      /* Snapshot before */
      const before = parseObjectList(PraatModule.listObjects())

      /* Build command string */
      const cleanCmd = command.replace(/\.\.\.$/g, '').trim()
      let fullCommand
      if (commandArgs.length > 0) {
        fullCommand = cleanCmd + ': ' + commandArgs.map(formatArg).join(', ') + '\n'
      } else {
        fullCommand = cleanCmd + '\n'
      }

      /* Execute */
      const infoResult = PraatModule.call(fullCommand)

      /* Check for new objects */
      const after = parseObjectList(PraatModule.listObjects())
      const newObjects = after.filter(a => !before.some(b => b.id === a.id))

      if (newObjects.length === 1) {
        return wrapObject(instance, newObjects[0])
      }
      if (newObjects.length > 1) {
        return newObjects.map(o => wrapObject(instance, o))
      }

      /* No new objects — parse info result */
      if (infoResult && infoResult.trim()) {
        const trimmed = infoResult.trim()
        const numMatch = trimmed.match(/^(-?[\d.]+(?:e[+-]?\d+)?)\b/)
        if (numMatch) {
          const parsed = parseFloat(numMatch[1])
          if (!isNaN(parsed)) return parsed
        }
        return trimmed
      }

      return undefined
    },

    /**
     * List all objects.
     * @returns {PraatObject[]}
     */
    list () {
      return parseObjectList(PraatModule.listObjects())
        .map(o => wrapObject(instance, o))
    },

    /**
     * Select an object by its ID.
     * @param {number} id
     */
    select (id) {
      PraatModule.selectObject(id)
    },

    /**
     * Load audio from an ArrayBuffer (WAV, AIFF, FLAC, MP3, OGG).
     * @param {ArrayBuffer} arrayBuffer
     * @param {string} [filename]
     * @returns {Sound}
     */
    readAudio (arrayBuffer, filename) {
      const fname = filename || ('/tmp/input_' + (nextFileId++) + '.wav')
      const data = new Uint8Array(arrayBuffer)
      PraatModule.FS.writeFile(fname, data)
      PraatModule.readFile(fname)
      const obj = getLastObject()
      try { PraatModule.FS.unlink(fname) } catch (_e) { /* ignore */ }
      if (!obj) return null
      return /** @type {Sound} */ (wrapObject(instance, obj))
    },

    /**
     * Read a file from the virtual filesystem.
     * @param {string} path
     * @returns {PraatObject}
     */
    readFile (path) {
      PraatModule.readFile(path)
      const obj = getLastObject()
      if (!obj) return null
      return wrapObject(instance, obj)
    },

    /**
     * Create a new TextGrid.
     * @param {number} startTime
     * @param {number} endTime
     * @param {string|string[]} [tierNames]
     * @param {string|string[]} [pointTierNames]
     * @returns {TextGrid}
     */
    createTextGrid (startTime, endTime, tierNames, pointTierNames) {
      const tNames = Array.isArray(tierNames) ? tierNames.join(' ') : (tierNames ?? '')
      const ptNames = Array.isArray(pointTierNames) ? pointTierNames.join(' ') : (pointTierNames ?? '')
      const before = parseObjectList(PraatModule.listObjects())
      PraatModule.executeScript(
        'Create TextGrid: ' + startTime + ', ' + endTime +
        ', "' + tNames + '", "' + ptNames + '"\n')
      const after = parseObjectList(PraatModule.listObjects())
      const newObj = after.find(a => !before.some(b => b.id === a.id))
      if (!newObj) return null
      return /** @type {TextGrid} */ (wrapObject(instance, newObj))
    },

    saveAsText (path) { PraatModule.saveAsText(path) },
    saveAsBinary (path) { PraatModule.saveAsBinary(path) },
    getFile (path) { return PraatModule.FS.readFile(path) },
    writeFile (path, data) { PraatModule.FS.writeFile(path, data) },
    removeAll () { PraatModule.removeAllObjects() },
    removeSelected () { PraatModule.removeSelectedObjects() },
    get FS () { return PraatModule.FS },
    destroy () { PraatModule.removeAllObjects() }
  }

  return instance
}

export default createPraatWasm
