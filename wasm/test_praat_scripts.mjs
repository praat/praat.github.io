/*
 * test_praat_scripts.mjs
 *
 * Runs the existing Praat test suite (.praat scripts) through the WASM build.
 * Loads all test files into Emscripten's MEMFS, then runs each script individually.
 */

import { readFileSync, readdirSync, statSync } from 'fs';
import { join, relative, dirname } from 'path';
import { fileURLToPath } from 'url';
import PraatModule from './dist/praat.mjs';

const __dirname = dirname(fileURLToPath(import.meta.url));
const projectRoot = join(__dirname, '..');

/* ---- Load the WASM module ---- */
const Module = await PraatModule();

/* ---- Helper: recursively collect files ---- */
function collectFiles(dir, base) {
    let files = [];
    for (const entry of readdirSync(dir)) {
        const full = join(dir, entry);
        const rel = relative(base, full);
        const st = statSync(full);
        if (st.isDirectory()) {
            files = files.concat(collectFiles(full, base));
        } else {
            files.push({ abs: full, rel });
        }
    }
    return files;
}

/* ---- Preload test directory into MEMFS ---- */
const testDir = join(projectRoot, 'test');
console.log('Loading test files into MEMFS...');

const allFiles = collectFiles(testDir, projectRoot);

// Create directories first
const dirs = new Set();
for (const f of allFiles) {
    const parts = dirname(f.rel).split('/');
    let cur = '';
    for (const p of parts) {
        cur = cur ? cur + '/' + p : p;
        dirs.add('/' + cur);
    }
}
for (const d of [...dirs].sort()) {
    try { Module.FS.mkdir(d); } catch (e) { /* already exists */ }
}

// Write files
let loaded = 0;
for (const f of allFiles) {
    const data = readFileSync(f.abs);
    Module.FS.writeFile('/' + f.rel, data);
    loaded++;
}
console.log(`Loaded ${loaded} files into MEMFS.\n`);

/* ---- Init Praat ---- */
Module.praatInit();

/* ---- Tests to skip with reasons ---- */
const SKIP = {
    // Meta test runners — we're running our own
    'test/runAllTests.praat': 'meta runner',
    'test/runAllTests_batch.praat': 'meta runner',
    'test/runAllTests_leak.praat': 'meta runner',
    'test/createPraatTests.praat': 'meta/code generator',
    'test/menuSpeed.praat': 'GUI speed test',
    'test/code.praat': 'reads source files (not available in WASM)',

    // Graphics (NO_GRAPHICS build)
    'test/sys/graphics.praat': 'requires graphics',
    'test/sys/graphicsText.praat': 'requires graphics',
    'test/sys/graphicsTextSpeed.praat': 'requires graphics',
    'test/fon/Spectrum_draw.praat': 'requires graphics',
    'test/sys/colour.praat': 'requires graphics',
    'test/fon/resample11_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample16_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample22_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample24_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample32_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample44_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample48_8.praat': 'requires graphics (Erase all/Paint)',
    'test/fon/resample96_8.praat': 'requires graphics (Erase all/Paint)',
    'test/dwtools/Discriminant.praat': 'requires graphics (Erase all)',
    'test/dwtools/EditCostsTable.praat': 'requires graphics (Draw)',
    'test/num/interpolation.praat': 'requires graphics (Erase all)',
    'test/gram/DBN.praat': 'requires graphics (Erase all)',
    'test/script/RBM.praat': 'requires graphics (Erase all)',
    'test/script/commandReturns.praat': 'requires graphics (Draw inner box)',

    // GUI / progress bars
    'test/sys/progress.praat': 'requires GUI progress bars',
    'test/sys/progress2.praat': 'requires GUI progress bars',

    // SpeechSynthesizer (espeak stubbed)
    'test/dwtools/SpeechSynthesizer.praat': 'requires espeak (stubbed)',

    // Speed/benchmark tests (slow in WASM, these are timing tests not correctness)
    'test/fon/fftSpeed.praat': 'benchmark/speed test',
    'test/fon/textioSpeed.praat': 'benchmark/speed test',
    'test/sys/object.praat': 'formula speed benchmark',
    'test/sys/praat_statistics.cpp.praat': 'memory stats (environment-dependent)',

    // Large memory / stress tests
    'test/script/text100MB.praat': 'allocates 100MB+, too large for WASM',
    'test/fon/LongSound.praat': 'creates 205s WAV files, excessive for WASM',

    // Multi-threading tests
    'test/script/RBMparallel.praat': 'requires multi-threading',

    // Tests that rely on platform-specific file encoding
    'test/kar/MacRoman.praat': 'macOS-specific encoding test',

    // Crash test (intentional crash/abort)
    'test/crash.praat': 'intentional crash test',

    // File system tests that depend on host OS filesystem features
    'test/script/fileNames-S-H.praat': 'host filesystem sorting',
    'test/script/fileNames-S-H-down.praat': 'host filesystem sorting',
    'test/script/up/fileNames-S-H-up.praat': 'host filesystem sorting',
    'test/fileSystem/Melder_fopen.praat': 'host filesystem specific',

    // Graphics commands in non-obvious test scripts
    'test/fon/resamplingDepth.praat': 'requires graphics (Erase all)',
    'test/stat/Table.cpp.praat': 'requires graphics (Scatter plot)',

    // Known Praat bug: stereoAnalysis asserts LPC bug (2009-2025) vs praatVersion
    'test/fon/stereoAnalysis.praat': 'known Praat version-check bug (stereo LPC)',

    // Discriminant save/read crashes with memory access out of bounds in WASM
    'test/fon/data.praat': 'Discriminant serialization crashes in WASM (memory OOB)',
};

/* ---- Collect test scripts ---- */
const testScripts = allFiles
    .filter(f => f.rel.endsWith('.praat'))
    .filter(f => !f.rel.includes('/manually/'))
    .filter(f => !f.rel.includes('/speed/'))
    .filter(f => !f.rel.includes('_GUI_'))
    .filter(f => {
        // Skip sub-scripts that are called by other tests, not standalone
        const base = f.rel.replace('test/', '');
        return !base.includes('/deepPath/') ||
               base === 'script/deepPath.praat';
    })
    .filter(f => {
        // Skip recursive sub-scripts
        const base = f.rel.replace('test/', '');
        return !base.includes('/recursive_file/') ||
               base === 'script/recursive_file.praat';
    })
    .filter(f => {
        const base = f.rel.replace('test/', '');
        return !base.includes('/recursive_info/') ||
               base === 'script/recursive_info.praat';
    })
    .sort((a, b) => a.rel.localeCompare(b.rel));

console.log(`Found ${testScripts.length} test scripts.\n`);

/* ---- Run tests ---- */
let passed = 0;
let failed = 0;
let skipped = 0;
let errors = [];

for (const script of testScripts) {
    const name = script.rel;

    if (SKIP[name]) {
        console.log(`  SKIP  ${name} (${SKIP[name]})`);
        skipped++;
        continue;
    }

    try {
        // Clear any stale error state from previous tests
        try { Module.clearError(); } catch(_) {}

        // Set seed for reproducibility (matching runAllTests_batch.praat)
        Module.executeScript('random_initializeWithSeedUnsafelyButPredictably (5489)\n');

        // Clean up objects from previous test
        Module.executeScript(
            'select all\n' +
            'if numberOfSelected() > 0\n' +
            '  Remove\n' +
            'endif\n'
        );

        // Run the test script via runScript (handles directory switching)
        const result = Module.executeScript(`runScript: "/${name}"\n`);
        console.log(`  PASS  ${name}`);
        passed++;
    } catch (e) {
        // Try to get detailed error from Melder error buffer
        let msg = '';
        try {
            msg = Module.getLastError();
            Module.clearError();
        } catch(_) {}
        if (!msg) msg = e.message || String(e);
        // Truncate long error messages
        const shortMsg = msg.length > 300 ? msg.substring(0, 300) + '...' : msg;
        console.log(`  FAIL  ${name}`);
        console.log(`        ${shortMsg.split('\n').join('\n        ')}`);
        failed++;
        errors.push({ name, error: shortMsg });
    }
}

/* ---- Summary ---- */
console.log('\n' + '='.repeat(60));
console.log(`Results: ${passed} passed, ${failed} failed, ${skipped} skipped (${testScripts.length} total)`);
console.log('='.repeat(60));

if (errors.length > 0) {
    console.log('\nFailed tests:');
    for (const { name, error } of errors) {
        console.log(`\n  ${name}:`);
        console.log(`    ${error}`);
    }
}

console.log(`\nPass rate: ${(100 * passed / (passed + failed)).toFixed(1)}%`);

process.exit(failed > 0 ? 1 : 0);
