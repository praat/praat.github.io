// Quick smoke test for praat-wasm
import PraatModule from './dist/praat.mjs';

const m = await PraatModule();
console.log('Module loaded successfully');
console.log('Has praatInit:', typeof m.praatInit);
console.log('Has executeScript:', typeof m.executeScript);
console.log('Has listObjects:', typeof m.listObjects);
console.log('Has call:', typeof m.call);
console.log('Has readFile:', typeof m.readFile);

m.praatInit();
console.log('\npraatInit() succeeded');

// Test 1: Simple script
const result = m.executeScript('writeInfoLine: "Hello from Praat WASM!"');
console.log('Script output:', JSON.stringify(result));

// Test 2: Create a Sound object
m.executeScript('Create Sound as pure tone: "test", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
const objects = m.listObjects();
console.log('Object list:', objects.trim());

// Test 3: Query the Sound
m.executeScript('selectObject: 1');
const info = m.executeScript(
  'duration = Get total duration\n' +
  'sampleRate = Get sample rate\n' +
  'writeInfoLine: "Duration: ", duration, " Rate: ", sampleRate\n'
);
console.log('Sound info:', info.trim());

// Test 4: Convert to Pitch
m.call('To Pitch: 0, 75, 600\n');
const objects2 = m.listObjects();
console.log('After To Pitch:', objects2.trim());

console.log('\nALL TESTS PASSED');
