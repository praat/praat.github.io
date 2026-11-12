// Run ONLY the failing tests and show error messages
import PraatModule from './dist/praat.mjs';
import { readFileSync, readdirSync, statSync } from 'fs';
import { join, relative, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const projectRoot = join(__dirname, '..');
const Module = await PraatModule();

// Load test directory into MEMFS
function collectFiles(dir, base) {
    let files = [];
    for (const entry of readdirSync(dir)) {
        const full = join(dir, entry);
        const rel = relative(base, full);
        const st = statSync(full);
        if (st.isDirectory()) files = files.concat(collectFiles(full, base));
        else files.push({ abs: full, rel });
    }
    return files;
}
const testDir = join(projectRoot, 'test');
const allFiles = collectFiles(testDir, projectRoot);
const dirs = new Set();
for (const f of allFiles) {
    const parts = dirname(f.rel).split('/');
    let cur = '';
    for (const p of parts) { cur = cur ? cur + '/' + p : p; dirs.add('/' + cur); }
}
for (const d of [...dirs].sort()) { try { Module.FS.mkdir(d); } catch(e) {} }
for (const f of allFiles) { Module.FS.writeFile('/' + f.rel, readFileSync(f.abs)); }

Module.praatInit();

const FAILURES = [
    'test/dwtools/Discriminant.praat',
    'test/dwtools/EditCostsTable.praat',
    'test/fon/data.praat',
    'test/fon/resamplingDepth.praat',
    'test/fon/Sound_to_Spectrogram.praat',
    'test/fon/Spectrum.praat',
    'test/fon/stereoAnalysis.praat',
    'test/fon/texio.praat',
    'test/gram/DBN.praat',
    'test/kar/unicode.praat',
    'test/kar/unicode16.praat',
    'test/LPC/Sound_to_LPC.praat',
    'test/num/interpolation.praat',
    'test/script/commandReturns.praat',
    'test/script/fileReadable.praat',
    'test/script/indexedVariables.praat',
    'test/script/options.praat',
    'test/script/RBM.praat',
    'test/script/script.praat',
    'test/script/tensor.praat',
    'test/stat/Table.cpp.praat',
    'test/sys/Formula.cpp.praat',
    'test/sys/Interpreter.cpp.praat',
];

for (const name of FAILURES) {
    try {
        Module.executeScript('random_initializeWithSeedUnsafelyButPredictably (5489)\n');
        Module.executeScript('select all\nif numberOfSelected() > 0\n  Remove\nendif\n');
        Module.executeScript(`runScript: "/${name}"\n`);
        console.log(`  NOW-PASS  ${name}`);
    } catch(e) {
        const msg = (e.message || String(e)).split('\n').slice(0, 5).join('\n    ');
        console.log(`  FAIL  ${name}`);
        console.log(`    ${msg}\n`);
    }
}
