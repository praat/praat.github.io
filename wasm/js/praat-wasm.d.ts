/**
 * Type declarations for praat-wasm.
 * Generated from JSDoc annotations. Can be regenerated with:
 *   tsc --declaration --allowJs --emitDeclarationOnly --outDir types wasm/js/praat-wasm.mjs
 */

export interface PraatObject {
  /** Praat object ID. */
  id: number;
  /** Class name (e.g. "Sound", "Pitch", "TextGrid"). */
  type: string;
  /** Object name. */
  name: string;
}

export interface PraatWasmInstance {
  /**
   * Execute a Praat script and return the Info window contents.
   * Equivalent to Parselmouth's `parselmouth.praat.run()`.
   */
  run(scriptText: string): string;

  /**
   * Execute a Praat command on selected object(s).
   * Equivalent to Parselmouth's `parselmouth.praat.call()`.
   *
   * @example
   * praat.call("To Pitch...", 0, 75, 600);
   * // or with new syntax:
   * praat.call("To Pitch", 0, 75, 600);
   */
  call(command: string, ...args: (string | number)[]): string;

  /** List all objects currently in the Praat object list. */
  list(): PraatObject[];

  /** Select an object by its ID. */
  select(id: number): void;

  /**
   * Load audio from an ArrayBuffer (WAV, AIFF, FLAC, MP3, OGG).
   * Returns the created Praat object.
   *
   * @param arrayBuffer - Raw audio file bytes.
   * @param filename - Optional filename hint (determines format detection).
   */
  readAudio(arrayBuffer: ArrayBuffer, filename?: string): PraatObject | null;

  /** Read a file from the virtual filesystem into the Praat object list. */
  readFile(path: string): PraatObject | null;

  /** Save the selected object as a Praat text file to the virtual filesystem. */
  saveAsText(path: string): void;

  /** Save the selected object as a Praat binary file to the virtual filesystem. */
  saveAsBinary(path: string): void;

  /** Read a file from the virtual filesystem as raw bytes. */
  getFile(path: string): Uint8Array;

  /** Write raw bytes to the virtual filesystem. */
  writeFile(path: string, data: Uint8Array): void;

  /** Remove all objects from the Praat object list. */
  removeAll(): void;

  /** Remove the currently selected objects. */
  removeSelected(): void;

  /** Direct access to Emscripten's virtual filesystem. */
  FS: any;

  /** Clean up resources. */
  destroy(): void;
}

/**
 * Create a Praat WASM instance (runs synchronously on the calling thread).
 * For browser use, prefer `createPraatWorker` to run off the main thread.
 *
 * @param wasmUrl - Optional URL to the praat.wasm binary.
 */
export function createPraatWasm(wasmUrl?: string | URL): Promise<PraatWasmInstance>;
export default createPraatWasm;

export interface PraatWorkerInstance {
  run(scriptText: string): Promise<string>;
  call(command: string, ...args: (string | number)[]): Promise<string>;
  list(): Promise<PraatObject[]>;
  select(id: number): Promise<void>;
  readAudio(arrayBuffer: ArrayBuffer, filename?: string): Promise<PraatObject | null>;
  readFile(path: string): Promise<PraatObject | null>;
  saveAsText(path: string): Promise<void>;
  saveAsBinary(path: string): Promise<void>;
  getFile(path: string): Promise<Uint8Array>;
  writeFile(path: string, data: Uint8Array): Promise<void>;
  removeAll(): Promise<void>;
  removeSelected(): Promise<void>;
  destroy(): void;
}

/**
 * Create a Praat instance running in a Web Worker (off main thread).
 * Best for browser environments to avoid blocking the UI.
 *
 * @param workerUrl - URL to the worker.mjs script.
 * @param wasmUrl - Optional URL to the praat.wasm binary.
 */
export function createPraatWorker(workerUrl?: string | URL, wasmUrl?: string | URL): Promise<PraatWorkerInstance>;
