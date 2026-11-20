// Test runScript as the VERY FIRST operation
import PraatModule from './dist/praat.mjs';

const m = await PraatModule();
m.praatInit();

// Test 1: runScript to /hello.praat before anything else
m.FS.writeFile('/hello.praat', 'writeInfoLine: "hello from runScript"\n');
try {
    const r = m.executeScript('runScript: "/hello.praat"\n');
    console.log('Test 1 (first-op runScript): PASS -', JSON.stringify(r.trim()));
} catch(e) {
    console.log('Test 1 (first-op runScript): FAIL -', e.message || e);
}

// Test 2: Try include instead
m.FS.writeFile('/hello2.praat', 'writeInfoLine: "hello from include"\n');
try {
    const r = m.executeScript('include /hello2.praat\n');
    console.log('Test 2 (include): PASS -', JSON.stringify(r.trim()));
} catch(e) {
    console.log('Test 2 (include): FAIL -', e.message || e);
}

// Test 3: Try executing the Praat command to read the file
try {
    const r = m.executeScript('text$ = readFile$ ("/hello.praat")\nwriteInfoLine: text$\n');
    console.log('Test 3 (readFile$): PASS -', JSON.stringify(r.trim()));
} catch(e) {
    console.log('Test 3 (readFile$): FAIL -', e.message || e);
}

// Test 4: Check if fileReadable works
try {
    const r = m.executeScript('x = fileReadable("/hello.praat")\nwriteInfoLine: x\n');
    console.log('Test 4 (fileReadable): PASS -', JSON.stringify(r.trim()));
} catch(e) {
    console.log('Test 4 (fileReadable): FAIL -', e.message || e);
}
