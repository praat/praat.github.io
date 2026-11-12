// Test ONLY the runScript path in isolation
import PraatModule from './dist/praat.mjs';

const m = await PraatModule();
m.praatInit();

// Write a trivial script to MEMFS
m.FS.writeFile('/tmp/trivial.praat', 'writeInfoLine: "hello"\n');
try {
    const r = m.executeScript('runScript: "/tmp/trivial.praat"\n');
    console.log('Trivial runScript: PASS -', r.trim());
} catch(e) {
    console.log('Trivial runScript: FAIL -', e.message || e);
}

// Write the Sound+Pitch script
const script = [
    'Create Sound from formula: "test", 1, 0, 1, 44100, "sin(2*pi*377*x)"',
    'To Pitch: 0.0, 75.0, 600.0',
    'info$ = Get mean: 0, 0, "Hertz"',
    'writeInfoLine: info$',
    'select all',
    'Remove',
].join('\n') + '\n';

m.FS.writeFile('/tmp/sound_pitch.praat', script);
try {
    const r = m.executeScript('runScript: "/tmp/sound_pitch.praat"\n');
    console.log('Sound+Pitch runScript: PASS -', r.trim());
} catch(e) {
    console.log('Sound+Pitch runScript: FAIL -', e.message || e);
}

// Try the same but with the script content read back to verify MEMFS
const written = m.FS.readFile('/tmp/sound_pitch.praat', { encoding: 'utf8' });
console.log('MEMFS readback matches:', written === script);

// Now try executing the content directly (should work)
try {
    const r = m.executeScript(script);
    console.log('Direct execute of same script: PASS -', r.trim());
} catch(e) {
    console.log('Direct execute of same script: FAIL -', e.message || e);
}
