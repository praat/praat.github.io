// Test the JS wrapper API (praat-wasm.mjs)
import { createPraatWasm } from './js/praat-wasm.mjs';

let passed = 0;
let failed = 0;

function assert(condition, msg) {
  if (condition) {
    passed++;
    console.log(`  PASS: ${msg}`);
  } else {
    failed++;
    console.error(`  FAIL: ${msg}`);
  }
}

console.log('Creating PraatWasm instance...');
const praat = await createPraatWasm();
console.log('PraatWasm ready\n');

// Test 1: Run a script
console.log('=== 1. run() ===');
const hello = praat.run('writeInfoLine: "Hello from praat-wasm wrapper!"');
assert(hello.trim() === 'Hello from praat-wasm wrapper!', 'run() returns script output');

// Test 2: Create Sound and list objects
console.log('\n=== 2. Sound creation + list() ===');
praat.run('Create Sound as pure tone: "test", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
const objects = praat.list();
assert(objects.length === 1, `list() returns 1 object (got ${objects.length})`);
assert(objects[0].type === 'Sound', `First object is Sound (got ${objects[0].type})`);
assert(objects[0].name === 'Sound test', `Object name is "Sound test" (got ${objects[0].name})`);

// Test 3: call() with arguments
console.log('\n=== 3. call() with Parselmouth-style args ===');
praat.select(objects[0].id);
praat.call('To Pitch...', 0, 75, 600);
const objects2 = praat.list();
assert(objects2.length === 2, 'call() created Pitch object');
assert(objects2[1].type === 'Pitch', `Second object is Pitch (got ${objects2[1].type})`);

// Test 4: readAudio with WAV bytes
console.log('\n=== 4. readAudio() from ArrayBuffer ===');
praat.removeAll();
// Generate a simple WAV
praat.run('Create Sound as pure tone: "wavtest", 1, 0, 0.3, 22050, 300, 0.2, 0.01, 0.01');
praat.run('Save as WAV file: "/tmp/test_api.wav"');
const wavBytes = praat.getFile('/tmp/test_api.wav');
praat.removeAll();
assert(praat.list().length === 0, 'Objects cleared');

const audioObj = praat.readAudio(wavBytes.buffer, '/tmp/roundtrip.wav');
assert(audioObj !== null, 'readAudio returned an object');
assert(audioObj.type === 'Sound', `readAudio returned Sound (got ${audioObj?.type})`);

// Test 5: File I/O
console.log('\n=== 5. File I/O ===');
praat.removeAll();
praat.run('Create TextGrid: 0, 2, "words", ""');
praat.run('Insert boundary: 1, 1.0');
praat.run('Set interval text: 1, 1, "hello"');
praat.run('Set interval text: 1, 2, "world"');
praat.saveAsText('/tmp/test.TextGrid');
const tgBytes = praat.getFile('/tmp/test.TextGrid');
assert(tgBytes.length > 0, `TextGrid saved (${tgBytes.length} bytes)`);

praat.removeAll();
praat.writeFile('/tmp/test2.TextGrid', tgBytes);
praat.readFile('/tmp/test2.TextGrid');
const label = praat.run('text$ = Get label of interval: 1, 1\nwriteInfoLine: text$');
assert(label.trim() === 'hello', `TextGrid round-trip: got "${label.trim()}"`);

// Test 6: Error handling
console.log('\n=== 6. Error handling ===');
praat.removeAll();
let errorThrown = false;
try {
  praat.call('Nonexistent Command');
} catch (e) {
  errorThrown = true;
  assert(e.message.length > 0, `Error has message: ${e.message.substring(0, 50)}...`);
}
assert(errorThrown, 'Error thrown for invalid command');

// Test 7: destroy()
console.log('\n=== 7. destroy() ===');
praat.destroy();
assert(praat.list().length === 0, 'destroy() clears all objects');

// Summary
console.log(`\n${'='.repeat(50)}`);
console.log(`Results: ${passed} passed, ${failed} failed, ${passed + failed} total`);
if (failed === 0) {
  console.log('ALL WRAPPER TESTS PASSED');
} else {
  console.log('SOME TESTS FAILED');
  process.exit(1);
}
