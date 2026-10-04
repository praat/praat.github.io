#!/usr/bin/env node
/**
 * Reads the Praat version from main/main_Praat.h and the JS revision
 * from wasm/JS_REVISION, then writes the combined semver version
 * into wasm/package.json.
 *
 * Version scheme: Praat 6.4.62 + JS rev 3 → npm 6.4.6203
 * (Praat patch × 100 + JS revision, 0–99)
 *
 * Usage:
 *   node wasm/sync-version.mjs          # update package.json
 *   node wasm/sync-version.mjs --check  # exit 1 if out of sync
 *   node wasm/sync-version.mjs --bump   # increment JS_REVISION then update
 */
import { readFileSync, writeFileSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const root = resolve(__dirname, '..');

// Parse Praat version from main/main_Praat.h
const praatH = readFileSync(resolve(root, 'main/main_Praat.h'), 'utf8');
const match = praatH.match(/#define\s+PRAAT_VERSION_STR\s+(\d+)\.(\d+)\.(\d+)/);
if (!match) {
  console.error('ERROR: Could not parse PRAAT_VERSION_STR from main/main_Praat.h');
  process.exit(1);
}
const [, major, minor, patch] = match;

// Parse JS revision
const revPath = resolve(__dirname, 'JS_REVISION');
let jsRev = parseInt(readFileSync(revPath, 'utf8').trim(), 10);
if (isNaN(jsRev) || jsRev < 0 || jsRev > 99) {
  console.error(`ERROR: JS_REVISION must be 0–99, got: ${jsRev}`);
  process.exit(1);
}

// Handle --bump
if (process.argv.includes('--bump')) {
  jsRev += 1;
  if (jsRev > 99) {
    console.error('ERROR: JS_REVISION would exceed 99. Update the Praat version first.');
    process.exit(1);
  }
  writeFileSync(revPath, jsRev + '\n');
  console.log(`Bumped JS_REVISION to ${jsRev}`);
}

// Compute npm version
const npmPatch = parseInt(patch, 10) * 100 + jsRev;
const npmVersion = `${major}.${minor}.${npmPatch}`;

// Read package.json
const pkgPath = resolve(__dirname, 'package.json');
const pkgText = readFileSync(pkgPath, 'utf8');
const pkg = JSON.parse(pkgText);

if (process.argv.includes('--check')) {
  let ok = true;
  if (pkg.version !== npmVersion) {
    console.error(`MISMATCH: package.json has ${pkg.version}, expected ${npmVersion} (Praat ${major}.${minor}.${patch} + JS rev ${jsRev})`);
    ok = false;
  }
  // Also check praat-wasm.mjs version constant
  const mjsPath = resolve(__dirname, 'js/praat-wasm.mjs');
  const mjsText = readFileSync(mjsPath, 'utf8');
  if (!mjsText.includes(`export const version = '${npmVersion}'`)) {
    console.error(`MISMATCH: praat-wasm.mjs version constant does not match ${npmVersion}`);
    ok = false;
  }
  if (ok) {
    console.log(`OK: version ${npmVersion} matches Praat ${major}.${minor}.${patch} + JS rev ${jsRev}`);
    process.exit(0);
  } else {
    process.exit(1);
  }
}

// Update package.json
pkg.version = npmVersion;
writeFileSync(pkgPath, JSON.stringify(pkg, null, 2) + '\n');

// Update version constant in praat-wasm.mjs
const mjsPath = resolve(__dirname, 'js/praat-wasm.mjs');
const mjsText = readFileSync(mjsPath, 'utf8');
const updatedMjs = mjsText.replace(
  /export const version = '[^']*'/,
  `export const version = '${npmVersion}'`
);
writeFileSync(mjsPath, updatedMjs);

console.log(`Updated version to ${npmVersion} (Praat ${major}.${minor}.${patch} + JS rev ${jsRev})`);
