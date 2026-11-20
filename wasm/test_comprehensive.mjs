// Comprehensive integration test for praat-wasm
// Tests all major phonetic analysis functions
import PraatModule from './dist/praat.mjs';

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

function section(name) {
  console.log(`\n=== ${name} ===`);
}

const m = await PraatModule();
m.praatInit();
console.log('Praat WASM initialized');

// Helper: clean up all objects between tests
function cleanup() {
  try {
    m.executeScript('select all\nif numberOfSelected() > 0\n  Remove\nendif\n');
  } catch (_e) { /* ok */ }
}

// =============================================
section('1. Sound creation and basic queries');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "tone", 1, 0, 1.0, 44100, 440, 0.2, 0.01, 0.01');
let info = m.executeScript('Get total duration');
assert(parseFloat(info.trim()) === 1, 'Sound duration is 1.0s');

info = m.executeScript('Get sample rate');
assert(parseFloat(info.trim()) === 44100, 'Sound sample rate is 44100');

info = m.executeScript('Get number of channels');
assert(parseInt(info.trim()) === 1, 'Sound has 1 channel');

info = m.executeScript('Get number of samples');
assert(parseInt(info.trim()) === 44100, 'Sound has 44100 samples');

// =============================================
section('2. Pitch analysis');
// =============================================
m.executeScript('selectObject: 1');
m.executeScript('To Pitch: 0, 75, 600');
info = m.executeScript(
  'selectObject: 2\n' +
  'f0 = Get mean: 0, 0, "Hertz"\n' +
  'writeInfoLine: fixed$(f0, 1)\n'
);
const meanF0 = parseFloat(info.trim());
assert(meanF0 > 430 && meanF0 < 450, `Mean F0 of 440Hz tone is ${meanF0} Hz (expected ~440)`);

info = m.executeScript(
  'voiced = Count voiced frames\n' +
  'writeInfoLine: voiced\n'
);
const voicedFrames = parseInt(info.trim());
assert(voicedFrames > 0, `Has ${voicedFrames} voiced frames`);

// =============================================
section('3. Formant analysis');
// =============================================
cleanup();
m.executeScript('Create Sound from formula: "vowel", 1, 0, 0.5, 44100, "0.5 * sin(2*pi*500*x) + 0.3 * sin(2*pi*1500*x) + 0.2 * sin(2*pi*2500*x)"');
m.executeScript('To Formant (burg): 0, 5, 5500, 0.025, 50');
info = m.executeScript(
  'nFrames = Get number of frames\n' +
  'writeInfoLine: nFrames\n'
);
assert(parseInt(info.trim()) > 0, `Formant has ${info.trim()} frames`);

// =============================================
section('4. Intensity analysis');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "loud", 1, 0, 0.5, 44100, 440, 0.5, 0.01, 0.01');
m.executeScript('To Intensity: 100, 0, "yes"');
info = m.executeScript(
  'meanInt = Get mean: 0, 0, "dB"\n' +
  'writeInfoLine: fixed$(meanInt, 1)\n'
);
const meanIntensity = parseFloat(info.trim());
assert(meanIntensity > 50 && meanIntensity < 100, `Mean intensity is ${meanIntensity} dB`);

// =============================================
section('5. Spectrum analysis');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "spec", 1, 0, 1.0, 44100, 1000, 0.2, 0.01, 0.01');
m.executeScript('To Spectrum: "yes"');
info = m.executeScript(
  'cog = Get centre of gravity: 2\n' +
  'writeInfoLine: fixed$(cog, 1)\n'
);
const cog = parseFloat(info.trim());
assert(cog > 900 && cog < 1100, `Spectrum center of gravity is ${cog} Hz (expected ~1000)`);

// =============================================
section('6. Spectrogram analysis');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "sg", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('To Spectrogram: 0.005, 5000, 0.002, 20, "Gaussian"');
info = m.executeScript(
  'nFrames = Get number of frames\n' +
  'writeInfoLine: nFrames\n'
);
assert(parseInt(info.trim()) > 0, `Spectrogram has ${info.trim()} frames`);

// =============================================
section('7. TextGrid creation and manipulation');
// =============================================
cleanup();
m.executeScript(
  'Create TextGrid: 0, 1, "words phones", "phones"\n' +
  'Insert boundary: 1, 0.5\n' +
  'Set interval text: 1, 1, "hello"\n' +
  'Set interval text: 1, 2, "world"\n'
);
info = m.executeScript(
  'nIntervals = Get number of intervals: 1\n' +
  'text1$ = Get label of interval: 1, 1\n' +
  'text2$ = Get label of interval: 1, 2\n' +
  'writeInfoLine: nIntervals, " ", text1$, " ", text2$\n'
);
assert(info.trim() === '2 hello world', `TextGrid intervals: ${info.trim()}`);

// =============================================
section('8. PointProcess');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "pp", 1, 0, 0.5, 44100, 200, 0.2, 0.01, 0.01');
m.executeScript('To PointProcess (periodic, cc): 75, 600');
info = m.executeScript(
  'nPoints = Get number of points\n' +
  'writeInfoLine: nPoints\n'
);
const nPoints = parseInt(info.trim());
assert(nPoints > 50, `PointProcess has ${nPoints} points (expected ~100 for 200Hz * 0.5s)`);

// =============================================
section('9. Harmonicity');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "harm", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('To Harmonicity (cc): 0.01, 75, 0.1, 1');
info = m.executeScript(
  'meanHNR = Get mean: 0, 0\n' +
  'writeInfoLine: fixed$(meanHNR, 1)\n'
);
const meanHNR = parseFloat(info.trim());
assert(meanHNR > 10, `Mean HNR is ${meanHNR} dB (expected high for pure tone)`);

// =============================================
section('10. MFCC');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "mfcc", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('To MFCC: 12, 0.015, 0.005, 100, 100, 0');
info = m.executeScript(
  'nFrames = Get number of frames\n' +
  'writeInfoLine: nFrames\n'
);
assert(parseInt(info.trim()) > 0, `MFCC has ${info.trim()} frames`);

// =============================================
section('11. LPC analysis');
// =============================================
cleanup();
m.executeScript('Create Sound from formula: "lpc", 1, 0, 0.5, 44100, "0.5 * sin(2*pi*500*x) + 0.3 * sin(2*pi*1500*x)"');
m.executeScript('To LPC (autocorrelation): 16, 0.025, 0.005, 50');
info = m.executeScript(
  'nFrames = Get number of frames\n' +
  'writeInfoLine: nFrames\n'
);
assert(parseInt(info.trim()) > 0, `LPC has ${info.trim()} frames`);

// =============================================
section('12. MEMFS file I/O');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "io", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('Save as WAV file: "/tmp/test_output.wav"');

// Check the file exists in MEMFS
let fileData;
try {
  fileData = m.FS.readFile('/tmp/test_output.wav');
  assert(fileData.length > 0, `WAV file written to MEMFS (${fileData.length} bytes)`);
} catch (e) {
  assert(false, `WAV file write failed: ${e.message}`);
}

// Read it back
cleanup();
m.executeScript('Read from file: "/tmp/test_output.wav"');
info = m.executeScript(
  'dur = Get total duration\n' +
  'writeInfoLine: fixed$(dur, 2)\n'
);
assert(info.trim() === '0.50', `Re-read WAV duration: ${info.trim()}`);

// TextGrid save/load
cleanup();
m.executeScript(
  'Create TextGrid: 0, 1, "tier1", ""\n' +
  'Insert boundary: 1, 0.3\n' +
  'Set interval text: 1, 1, "a"\n' +
  'Set interval text: 1, 2, "b"\n' +
  'Save as text file: "/tmp/test.TextGrid"\n'
);
cleanup();
m.executeScript('Read from file: "/tmp/test.TextGrid"');
info = m.executeScript(
  'text1$ = Get label of interval: 1, 1\n' +
  'text2$ = Get label of interval: 1, 2\n' +
  'writeInfoLine: text1$, " ", text2$\n'
);
assert(info.trim() === 'a b', `TextGrid round-trip: ${info.trim()}`);

// =============================================
section('13. Sound manipulation (resample, extract)');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "manip", 1, 0, 1.0, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('Resample: 22050, 50');
info = m.executeScript(
  'sr = Get sample rate\n' +
  'writeInfoLine: sr\n'
);
assert(info.trim() === '22050', `Resampled to 22050 Hz`);

cleanup();
m.executeScript('Create Sound as pure tone: "ext", 1, 0, 2.0, 44100, 440, 0.2, 0.01, 0.01');
m.executeScript('Extract part: 0.5, 1.5, "rectangular", 1, "no"');
info = m.executeScript(
  'dur = Get total duration\n' +
  'writeInfoLine: fixed$(dur, 1)\n'
);
assert(info.trim() === '1.0', `Extracted part duration: ${info.trim()}`);

// =============================================
section('14. Multiple objects and selection');
// =============================================
cleanup();
m.executeScript('Create Sound as pure tone: "a", 1, 0, 0.5, 44100, 300, 0.2, 0.01, 0.01');
m.executeScript('Create Sound as pure tone: "b", 1, 0, 0.5, 44100, 500, 0.2, 0.01, 0.01');
const objects = m.listObjects().trim().split('\n');
assert(objects.length === 2, `Two objects created`);

// =============================================
section('15. Error handling');
// =============================================
cleanup();
let caught = false;
try {
  m.executeScript('Get total duration');  // no object selected
} catch (e) {
  caught = true;
  assert(typeof e.message === 'string' && e.message.length > 0, `Error thrown with message: ${e.message.substring(0, 60)}...`);
}
assert(caught, 'Error thrown for invalid operation');

// =============================================
// Summary
// =============================================
console.log(`\n${'='.repeat(50)}`);
console.log(`Results: ${passed} passed, ${failed} failed, ${passed + failed} total`);
if (failed === 0) {
  console.log('ALL TESTS PASSED');
} else {
  console.log('SOME TESTS FAILED');
  process.exit(1);
}
