# praat-wasm

Praat phonetic analysis library compiled to WebAssembly.

`praat-wasm` provides a JavaScript API modeled after Parselmouth, including:

- direct script execution (`run`)
- command-style calls (`call`)
- object wrappers (`Sound`, `Pitch`, `Formant`, `TextGrid`, and more)
- in-memory file I/O (Emscripten FS)
- worker support for off-main-thread processing

## Installation

```bash
npm install praat-wasm
```

## Quick Start (Direct API)

```js
import { createPraatWasm } from 'praat-wasm'

const praat = await createPraatWasm()

// Run a Praat script.
const result = praat.run('writeInfoLine: 42 + 58')
console.log(result.trim()) // 100

// Create and inspect a sound.
praat.run('Create Sound as pure tone: "tone", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01')
const sound = praat.list()[0]

const pitch = praat.call(sound, 'To Pitch...', 0, 75, 600)
const duration = praat.call(sound, 'Get total duration')

console.log(pitch.type, duration)
```

## Load Audio from ArrayBuffer

```js
import { createPraatWasm } from 'praat-wasm'

const praat = await createPraatWasm()
const audioBytes = await fetch('/example.wav').then(r => r.arrayBuffer())

const sound = praat.readAudio(audioBytes, '/tmp/example.wav')
const sampleRate = praat.call(sound, 'Get sampling frequency')

console.log(sampleRate)
```

## TextGrid Example

```js
import { createPraatWasm } from 'praat-wasm'

const praat = await createPraatWasm()

const tg = praat.createTextGrid(0, 1.0, 'words phones', 'phones')
praat.call(tg, 'Insert boundary...', 1, 0.5)
praat.call(tg, 'Set interval text...', 1, 1, 'hello')

praat.select(tg.id)
praat.saveAsText('/tmp/example.TextGrid')

const bytes = praat.getFile('/tmp/example.TextGrid')
const text = new TextDecoder().decode(bytes)
console.log(text)
```

## Worker API

Use the worker client to run Praat operations off the main thread.

```js
import { createPraatWorker } from 'praat-wasm/worker-client'

// In your app, provide a same-origin URL to worker.mjs.
const worker = await createPraatWorker(new URL('/worker.mjs', window.location.origin))

const value = await worker.run('writeInfoLine: 2 * 21')
console.log(value.trim()) // 42

worker.destroy()
```

### Important Browser Note

`Worker()` requires a same-origin script URL. If you load the worker module from a CDN,
create a same-origin bootstrap blob that imports the CDN module:

```js
import { createPraatWorker } from 'https://cdn.jsdelivr.net/npm/praat-wasm@6.4.6201/js/worker-client.mjs'

const workerModuleUrl = 'https://cdn.jsdelivr.net/npm/praat-wasm@6.4.6201/js/worker.mjs'
const bootstrapSource = `import ${JSON.stringify(workerModuleUrl)}\n`
const bootstrapUrl = URL.createObjectURL(new Blob([bootstrapSource], { type: 'text/javascript' }))

const worker = await createPraatWorker(bootstrapUrl)
URL.revokeObjectURL(bootstrapUrl)
```

## API Surface (High Level)

`createPraatWasm()` returns an instance with:

- `run(scriptText)`
- `call(objectOrCommand, ...args)`
- `list()`, `select(id)`, `removeAll()`, `removeSelected()`
- `readAudio(arrayBuffer, filename?)`
- `readFile(path)`, `writeFile(path, data)`, `getFile(path)`
- `createTextGrid(start, end, tierNames?, pointTierNames?)`
- `saveAsText(path)`, `saveAsBinary(path)`

Wrapped object classes include:

- `Sound`, `Pitch`, `Formant`, `Intensity`, `Harmonicity`
- `Spectrum`, `Spectrogram`, `MFCC`, `TextGrid`
- `PointProcess`, `LPC`, `Matrix`, `Table`

See type declarations in `js/praat-wasm.d.ts` for complete method signatures.

## Build and Test (Repository)

From the repository root:

```bash
make -f Makefile.wasm
node wasm/test_smoke.mjs
node wasm/test_wrapper.mjs
node wasm/test_classes.mjs
```

Browser integration tests:

```bash
node docs/wasm-test/serve-test.mjs
node docs/wasm-test/run-browser-test.mjs
```

## License

GPL-3.0-or-later.
