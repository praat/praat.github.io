/**
 * Test suite for the class-based Praat WASM API.
 */
import { createPraatWasm } from './js/praat-wasm.mjs'

let praat
let passed = 0
let failed = 0

function assert (condition, msg) {
  if (!condition) {
    console.log('  FAIL: ' + msg)
    failed++
  } else {
    console.log('  PASS: ' + msg)
    passed++
  }
}

function assertCloseTo (actual, expected, tolerance, msg) {
  const ok = Math.abs(actual - expected) < tolerance
  if (!ok) {
    console.log('  FAIL: ' + msg + ' (got ' + actual + ', expected ~' + expected + ')')
    failed++
  } else {
    console.log('  PASS: ' + msg)
    passed++
  }
}

async function run () {
  praat = await createPraatWasm()

  console.log('\n=== Sound class ===')

  praat.run('Create Sound as pure tone: "tone", 1, 0, 0.5, 44100, 440, 0.2, 0.01, 0.01')
  const objects = praat.list()
  assert(objects.length > 0, 'list() returns wrapped objects')
  const sound = objects[objects.length - 1]
  assert(sound.constructor.name === 'Sound', 'object type is Sound: ' + sound.constructor.name)
  assert(sound.className === 'Sound', 'className is Sound')
  assertCloseTo(sound.samplingFrequency, 44100, 1, 'samplingFrequency')
  assertCloseTo(sound.nSamples, 22050, 1, 'nSamples')
  assert(sound.nChannels === 1, 'nChannels')
  assertCloseTo(sound.startTime, 0, 0.001, 'startTime')
  assertCloseTo(sound.endTime, 0.5, 0.001, 'endTime')
  assertCloseTo(sound.duration, 0.5, 0.001, 'duration')

  const energy = sound.getEnergy()
  assert(energy > 0, 'getEnergy > 0: ' + energy)
  const rms = sound.getRms()
  assert(rms > 0, 'getRms > 0: ' + rms)

  console.log('\n=== Sound -> Pitch (OOP style) ===')

  const pitch = sound.toPitch()
  assert(pitch !== null, 'toPitch() returned object')
  assert(pitch.constructor.name === 'Pitch', 'type is Pitch: ' + pitch.constructor.name)
  const f0 = pitch.getValueAtTime(0.25)
  assertCloseTo(f0, 440, 5, 'Pitch F0 at 0.25s')
  const voiced = pitch.countVoicedFrames()
  assert(voiced > 0, 'countVoicedFrames > 0: ' + voiced)

  const smoothed = pitch.smooth(10)
  assert(smoothed !== null, 'smooth() returned Pitch')
  assert(smoothed.constructor.name === 'Pitch', 'smooth() type is Pitch')
  smoothed.remove()

  const interp = pitch.interpolate()
  assert(interp !== null, 'interpolate() returned Pitch')
  interp.remove()

  const koj = pitch.killOctaveJumps()
  assert(koj !== null, 'killOctaveJumps() returned Pitch')
  koj.remove()

  console.log('\n=== Sound -> Formant (OOP style) ===')

  const formant = sound.toFormantBurg()
  assert(formant !== null, 'toFormantBurg() returned object')
  assert(formant.constructor.name === 'Formant', 'type is Formant')
  const f1 = formant.getValueAtTime(1, 0.25)
  assert(!isNaN(f1), 'Formant F1 at 0.25s is a number: ' + f1)

  console.log('\n=== Sound -> Intensity ===')

  const intensity = sound.toIntensity()
  assert(intensity !== null, 'toIntensity() returned object')
  assert(intensity.constructor.name === 'Intensity', 'type is Intensity')
  const intVal = intensity.getValue(0.25)
  assert(!isNaN(intVal), 'Intensity value: ' + intVal)
  const intAvg = intensity.getAverage()
  assert(!isNaN(intAvg), 'Intensity average: ' + intAvg)

  console.log('\n=== Sound -> Harmonicity ===')

  const harm = sound.toHarmonicityAc()
  assert(harm !== null, 'toHarmonicityAc() returned object')
  assert(harm.constructor.name === 'Harmonicity', 'type is Harmonicity')
  const hVal = harm.getMean()
  assert(!isNaN(hVal), 'Harmonicity mean: ' + hVal)

  console.log('\n=== Sound -> Spectrum ===')

  const spectrum = sound.toSpectrum()
  assert(spectrum !== null, 'toSpectrum() returned object')
  assert(spectrum.constructor.name === 'Spectrum', 'type is Spectrum')
  assert(spectrum.nBins > 0, 'nBins > 0: ' + spectrum.nBins)
  const cog = spectrum.getCenterOfGravity()
  assert(!isNaN(cog), 'center of gravity: ' + cog)
  const bandE = spectrum.getBandEnergy(400, 500)
  assert(!isNaN(bandE), 'band energy: ' + bandE)

  const sndBack = spectrum.toSound()
  assert(sndBack !== null, 'Spectrum.toSound() returned Sound')
  assert(sndBack.constructor.name === 'Sound', 'round-trip type is Sound')
  sndBack.remove()

  const smoothSpec = spectrum.cepstralSmoothing(500)
  assert(smoothSpec !== null, 'cepstralSmoothing() returned Spectrum')
  smoothSpec.remove()

  console.log('\n=== Sound -> Spectrogram ===')

  const spectrogram = sound.toSpectrogram()
  assert(spectrogram !== null, 'toSpectrogram() returned object')
  assert(spectrogram.constructor.name === 'Spectrogram', 'type is Spectrogram')
  const pw = spectrogram.getPowerAt(0.25, 440)
  assert(!isNaN(pw), 'power at (0.25, 440): ' + pw)

  console.log('\n=== Sound -> MFCC ===')

  const mfcc = sound.toMfcc()
  assert(mfcc !== null, 'toMfcc() returned object')
  assert(mfcc.constructor.name === 'MFCC', 'type is MFCC')
  assert(mfcc.nCoefficients > 0, 'nCoefficients > 0: ' + mfcc.nCoefficients)

  console.log('\n=== Sound -> PointProcess ===')

  const pp = sound.toPointProcess()
  assert(pp !== null, 'toPointProcess() returned object')
  assert(pp.constructor.name === 'PointProcess', 'type is PointProcess')
  const nPts = pp.getNumberOfPoints()
  assert(nPts > 0, 'number of points > 0: ' + nPts)
  const jitter = pp.getJitter()
  assert(!isNaN(jitter), 'jitter: ' + jitter)

  console.log('\n=== praat.call() Parselmouth-style ===')

  const pitch2 = praat.call(sound, 'To Pitch...', 0, 75, 600)
  assert(pitch2 !== null, 'praat.call(sound, "To Pitch...") returned object')
  assert(pitch2.constructor.name === 'Pitch', 'returned Pitch type')

  const f0val = praat.call(pitch2, 'Get value at time...', 0.25, 'Hertz', 'Linear')
  assert(typeof f0val === 'number', 'praat.call() query returns number: ' + f0val)
  assertCloseTo(f0val, 440, 5, 'praat.call() F0 value')

  const nVoiced = praat.call(pitch2, 'Count voiced frames')
  assert(typeof nVoiced === 'number', 'count returns number: ' + nVoiced)

  const newSound = praat.call('Create Sound as pure tone', 'test', 1, 0, 0.3, 44100, 220, 0.2, 0.01, 0.01)
  assert(newSound !== null, 'praat.call() with no object created Sound')
  assert(newSound.constructor.name === 'Sound', 'is Sound type')
  newSound.remove()
  pitch2.remove()

  console.log('\n=== TextGrid ===')

  const tg = praat.createTextGrid(0, 1, 'words phones tones', 'tones')
  assert(tg !== null, 'createTextGrid() returned object')
  assert(tg.constructor.name === 'TextGrid', 'type is TextGrid')
  assert(tg.getNumberOfTiers() === 3, 'has 3 tiers')
  assert(tg.isIntervalTier(1) === true, 'tier 1 is interval')
  assert(tg.isIntervalTier(3) === false, 'tier 3 is point')

  tg.insertBoundary(1, 0.5)
  tg.setIntervalText(1, 1, 'hello')
  tg.setIntervalText(1, 2, 'world')
  assert(tg.getLabelOfInterval(1, 1) === 'hello', 'interval 1 label')
  assert(tg.getLabelOfInterval(1, 2) === 'world', 'interval 2 label')

  tg.insertPoint(3, 0.25, 'H*')
  assert(tg.getNumberOfPoints(3) === 1, 'point count')
  assert(tg.getLabelOfPoint(3, 1) === 'H*', 'point label')

  tg._select()
  praat.saveAsText('/tmp/test.TextGrid')
  const tg2 = praat.readFile('/tmp/test.TextGrid')
  assert(tg2 !== null, 'TextGrid read back')
  assert(tg2.constructor.name === 'TextGrid', 'read-back type is TextGrid')
  tg.remove()
  tg2.remove()

  console.log('\n=== Sound modification ===')

  const resampled = sound.resample(22050)
  assert(resampled !== null, 'resample() returned Sound')
  assertCloseTo(resampled.samplingFrequency, 22050, 1, 'resampled to 22050')
  resampled.remove()

  const part = sound.extractPart(0.1, 0.3)
  assert(part !== null, 'extractPart() returned Sound')
  assertCloseTo(part.duration, 0.2, 0.01, 'extracted part duration')
  part.remove()

  const mono = sound.convertToMono()
  assert(mono !== null, 'convertToMono returned Sound')
  mono.remove()

  console.log('\n=== snake_case aliases ===')

  assert(typeof sound.to_pitch === 'function', 'sound.to_pitch exists')
  assert(typeof sound.to_formant_burg === 'function', 'sound.to_formant_burg exists')
  assert(typeof sound.to_intensity === 'function', 'sound.to_intensity exists')
  assert(typeof sound.to_spectrum === 'function', 'sound.to_spectrum exists')
  assert(typeof sound.to_spectrogram === 'function', 'sound.to_spectrogram exists')
  assert(typeof sound.to_mfcc === 'function', 'sound.to_mfcc exists')
  assert(typeof pitch.get_value_at_time === 'function', 'pitch.get_value_at_time exists')
  assert(typeof pitch.count_voiced_frames === 'function', 'pitch.count_voiced_frames exists')
  assert(typeof pitch.kill_octave_jumps === 'function', 'pitch.kill_octave_jumps exists')
  assert(typeof formant.get_value_at_time === 'function', 'formant.get_value_at_time exists')
  assert(typeof spectrum.get_center_of_gravity === 'function', 'spectrum.get_center_of_gravity exists')
  assert(typeof spectrum.get_centre_of_gravity === 'function', 'spectrum.get_centre_of_gravity exists')

  const f0snake = pitch.get_value_at_time(0.25)
  assertCloseTo(f0snake, 440, 5, 'snake_case alias produces same result')

  console.log('\n=== PraatObject base methods ===')

  assert(typeof sound.info === 'function', 'info() exists')
  assert(sound.fullName.startsWith('Sound'), 'fullName starts with Sound')
  assert(sound.toString() === sound.fullName, 'toString() matches fullName')

  const soundCopy = sound.copy()
  assert(soundCopy !== null, 'copy() returned object')
  assert(soundCopy.id !== sound.id, 'copy has different ID')
  soundCopy.remove()

  console.log('\n=== File I/O ===')

  sound.saveAsWav('/tmp/test.wav')
  const bytes = praat.getFile('/tmp/test.wav')
  assert(bytes.length > 44, 'WAV file has data: ' + bytes.length + ' bytes')

  const loaded = praat.readAudio(bytes.buffer, '/tmp/test2.wav')
  assert(loaded !== null, 'readAudio() returned Sound')
  assertCloseTo(loaded.samplingFrequency, sound.samplingFrequency, 1, 'loaded sampling freq matches')
  loaded.remove()

  console.log('\n=== Cleanup ===')

  pitch.remove()
  formant.remove()
  intensity.remove()
  harm.remove()
  spectrum.remove()
  spectrogram.remove()
  mfcc.remove()
  pp.remove()
  sound.remove()

  const remaining = praat.list()
  assert(remaining.length === 0, 'all objects cleaned up (remaining: ' + remaining.length + ')')

  console.log('\n========================================')
  console.log('Results: ' + passed + ' passed, ' + failed + ' failed')
  console.log('========================================')

  process.exit(failed > 0 ? 1 : 0)
}

run().catch(err => {
  console.error('Test error:', err)
  process.exit(1)
})
