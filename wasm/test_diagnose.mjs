// Diagnose "memory access out of bounds" errors
import PraatModule from './dist/praat.mjs';

const m = await PraatModule();
m.praatInit();

// Test 1: Direct script execution (known to work from smoke tests)
try {
    const r = m.executeScript(
        'Create Sound from formula: "test", 1, 0, 1, 44100, "sin(2*pi*377*x)"\n' +
        'To Pitch: 0.0, 75.0, 600.0\n' +
        'info$ = Get mean: 0, 0, "Hertz"\n' +
        'writeInfoLine: info$\n' +
        'select all\nRemove\n'
    );
    console.log('Test 1 (direct): PASS -', r.trim());
} catch(e) {
    console.log('Test 1 (direct): FAIL -', e.message || e);
}

// Test 2: Same script but via runScript + MEMFS file
const script2 = [
    'Create Sound from formula: "test", 1, 0, 1, 44100, "sin(2*pi*377*x)"',
    'To Pitch: 0.0, 75.0, 600.0',
    'info$ = Get mean: 0, 0, "Hertz"',
    'writeInfoLine: info$',
    'select all',
    'Remove',
].join('\n') + '\n';

m.FS.writeFile('/tmp/test2.praat', script2);
try {
    const r = m.executeScript('runScript: "/tmp/test2.praat"\n');
    console.log('Test 2 (runScript MEMFS): PASS -', r.trim());
} catch(e) {
    console.log('Test 2 (runScript MEMFS): FAIL -', e.message || e);
}

// Test 3: The actual Sound_to_Pitch.praat test from the test suite
// Read it from disk and write to MEMFS
import { readFileSync } from 'fs';
const pitchTest = readFileSync('test/fon/Sound_to_Pitch.praat');
m.FS.writeFile('/test_Sound_to_Pitch.praat', pitchTest);
try {
    m.executeScript('random_initializeWithSeedUnsafelyButPredictably (5489)\n');
    const r = m.executeScript('runScript: "/test_Sound_to_Pitch.praat"\n');
    console.log('Test 3 (Sound_to_Pitch.praat): PASS -', r.trim());
} catch(e) {
    console.log('Test 3 (Sound_to_Pitch.praat): FAIL -', e.message || e);
}

// Test 4: A simple num test that was failing
const numTest = readFileSync('test/num/mean.praat');
m.FS.writeFile('/test_mean.praat', numTest);
try {
    const r = m.executeScript('runScript: "/test_mean.praat"\n');
    console.log('Test 4 (mean.praat): PASS -', r.trim());
} catch(e) {
    console.log('Test 4 (mean.praat): FAIL -', e.message || e);
}

// Test 5: A pure script test that was failing
const calcTest = readFileSync('test/script/calculator.praat');
m.FS.writeFile('/test_calculator.praat', calcTest);
try {
    const r = m.executeScript('runScript: "/test_calculator.praat"\n');
    console.log('Test 5 (calculator.praat): PASS -', r.trim());
} catch(e) {
    console.log('Test 5 (calculator.praat): FAIL -', e.message || e);
}

// Test 6: A pure script test that was passing
const arithTest = readFileSync('test/script/arithmetic.praat');
m.FS.writeFile('/test_arithmetic.praat', arithTest);
try {
    const r = m.executeScript('runScript: "/test_arithmetic.praat"\n');
    console.log('Test 6 (arithmetic.praat): PASS -', r.trim());
} catch(e) {
    console.log('Test 6 (arithmetic.praat): FAIL -', e.message || e);
}

// Test 7: The stdev test (numeric, was failing)
const stdevTest = readFileSync('test/num/stdev.praat');
m.FS.writeFile('/test_stdev.praat', stdevTest);
try {
    const r = m.executeScript('runScript: "/test_stdev.praat"\n');
    console.log('Test 7 (stdev.praat): PASS -', r.trim());
} catch(e) {
    console.log('Test 7 (stdev.praat): FAIL -', e.message || e);
}

console.log('\nDone. If tests 3-7 fail with memory OOB but 1-2 pass,\nthe issue is in script complexity or interpreter stack depth.');
