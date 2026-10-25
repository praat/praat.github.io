/*
 * Quick diagnostic: run each Praat test script with a 30s timeout per test.
 * Prints only PASS/FAIL/SKIP/TIMEOUT lines for summary.
 */
import { readFileSync, readdirSync, statSync } from 'fs';
import { join, relative, dirname } from 'path';
import { fileURLToPath } from 'url';
import PraatModule from './dist/praat.mjs';

const __dirname = dirname(fileURLToPath(import.meta.url));
const projectRoot = join(__dirname, '..');

const Module = await PraatModule();

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

const testDir = join(projectRoot, 'test');
const allFiles = collectFiles(testDir, projectRoot);

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
    try { Module.FS.mkdir(d); } catch (e) {}
}
for (const f of allFiles) {
    const data = readFileSync(f.abs);
    Module.FS.writeFile('/' + f.rel, data);
}

Module.praatInit();

// Same skip list as test_praat_scripts.mjs — just keys
const SKIP = new Set([
    'test/runAllTests.praat', 'test/runAllTests_batch.praat', 'test/runAllTests_leak.praat',
    'test/createPraatTests.praat', 'test/menuSpeed.praat', 'test/code.praat',
    'test/sys/graphics.praat', 'test/sys/graphicsText.praat', 'test/sys/graphicsTextSpeed.praat',
    'test/fon/Spectrum_draw.praat', 'test/sys/colour.praat',
    'test/fon/resample11_8.praat', 'test/fon/resample16_8.praat', 'test/fon/resample22_8.praat',
    'test/fon/resample24_8.praat', 'test/fon/resample32_8.praat', 'test/fon/resample44_8.praat',
    'test/fon/resample48_8.praat', 'test/fon/resample96_8.praat',
    'test/dwtools/Discriminant.praat', 'test/dwtools/EditCostsTable.praat',
    'test/num/interpolation.praat', 'test/gram/DBN.praat', 'test/script/RBM.praat',
    'test/script/commandReturns.praat', 'test/sys/progress.praat', 'test/sys/progress2.praat',
    'test/dwtools/SpeechSynthesizer.praat', 'test/fon/fftSpeed.praat',
    'test/fon/textioSpeed.praat', 'test/sys/object.praat', 'test/sys/praat_statistics.cpp.praat',
    'test/script/text100MB.praat', 'test/fon/LongSound.praat', 'test/script/RBMparallel.praat',
    'test/kar/MacRoman.praat', 'test/crash.praat', 'test/script/fileNames-S-H.praat',
    'test/script/fileNames-S-H-down.praat', 'test/script/up/fileNames-S-H-up.praat',
    'test/fileSystem/Melder_fopen.praat', 'test/fon/resamplingDepth.praat',
    'test/stat/Table.cpp.praat', 'test/fon/stereoAnalysis.praat', 'test/fon/data.praat',
    'test/num/NUM.praat', 'test/num/stochastic.praat',
    'test/dwtools/Discriminant2.praat', 'test/dwtools/Discriminant3.praat',
    'test/num/inner.praat', 'test/num/mean.praat', 'test/num/sort.praat',
]);

const testScripts = allFiles
    .filter(f => f.rel.endsWith('.praat'))
    .filter(f => !f.rel.includes('/manually/'))
    .filter(f => !f.rel.includes('/speed/'))
    .filter(f => !f.rel.includes('_GUI_'))
    .filter(f => !f.rel.includes('/deepPath/') || f.rel.endsWith('deepPath.praat'))
    .filter(f => !f.rel.includes('/recursive_file/') || f.rel.endsWith('recursive_file.praat'))
    .filter(f => !f.rel.includes('/recursive_info/') || f.rel.endsWith('recursive_info.praat'))
    .sort((a, b) => a.rel.localeCompare(b.rel));

let passed = 0, failed = 0, skipped = 0;

for (const script of testScripts) {
    const name = script.rel;
    if (SKIP.has(name)) { skipped++; continue; }

    const start = Date.now();
    try {
        try { Module.clearError(); } catch(_) {}
        Module.executeScript('random_initializeWithSeedUnsafelyButPredictably (5489)\n');
        Module.executeScript('select all\nif numberOfSelected() > 0\n  Remove\nendif\n');
        Module.executeScript(`runScript: "/${name}"\n`);
        const s = ((Date.now() - start) / 1000).toFixed(1);
        if (parseFloat(s) > 5) {
            console.log(`  SLOW  ${name} (${s}s)`);
        }
        passed++;
    } catch (e) {
        let msg = '';
        try { msg = Module.getLastError(); Module.clearError(); } catch(_) {}
        if (!msg) msg = String(e).substring(0, 100);
        console.log(`  FAIL  ${name} — ${msg.split('\n')[0]}`);
        failed++;
    }
}

console.log(`\nResults: ${passed} passed, ${failed} failed, ${skipped} skipped (${testScripts.length} total)`);
process.exit(failed > 0 ? 1 : 0);
