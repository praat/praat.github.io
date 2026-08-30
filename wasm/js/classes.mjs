/**
 * praat-wasm class wrappers.
 *
 * Provides an object-oriented API mirroring Parselmouth's Python classes.
 * Each class wraps a Praat object ID and dispatches commands through the
 * script engine. Methods use camelCase (JS convention) with snake_case
 * aliases (Parselmouth convention).
 *
 * @module praat-wasm/classes
 */

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                   */
/* ------------------------------------------------------------------ */

/**
 * Parse a numeric result from a Praat info string.
 * Handles values like "123.456 Hz", "0.5 seconds", "undefined", "--undefined--".
 * @param {string} s
 * @returns {number}
 */
function parseNumber (s) {
  if (!s) return NaN
  const trimmed = s.trim()
  if (trimmed === '--undefined--' || trimmed === 'undefined') return NaN
  const match = trimmed.match(/^(-?[\d.]+(?:e[+-]?\d+)?)/)
  if (match) return parseFloat(match[1])
  return NaN
}

/**
 * Parse an integer result from Praat info string.
 * @param {string} s
 * @returns {number}
 */
function parseInt_ (s) {
  if (!s) return NaN
  const trimmed = s.trim()
  const match = trimmed.match(/^(-?\d+)/)
  if (match) return parseInt(match[1], 10)
  return NaN
}

/**
 * Parse a boolean from Praat info output ("yes"/"no", "1"/"0").
 * @param {string} s
 * @returns {boolean}
 */
function parseBool (s) {
  if (!s) return false
  const t = s.trim().toLowerCase()
  return t === 'yes' || t === '1' || t === 'true' ||
    t.startsWith('1 ') || t.startsWith('yes ')
}

/**
 * Escape a string argument for Praat scripting syntax.
 * @param {string|number|boolean} val
 * @returns {string}
 */
function formatArg (val) {
  if (typeof val === 'boolean') return val ? '"yes"' : '"no"'
  if (typeof val === 'string') return '"' + val.replace(/"/g, '""') + '"'
  return String(val)
}

/**
 * Build a Praat command string: "CommandName: arg1, arg2, ..."
 * @param {string} command
 * @param {Array} args
 * @returns {string}
 */
function buildCommand (command, args) {
  const clean = command.replace(/\.\.\.$/g, '').trim()
  if (args.length === 0) return clean + '\n'
  const parts = args.map(formatArg)
  return clean + ': ' + parts.join(', ') + '\n'
}

/**
 * Add snake_case aliases for all camelCase methods on a prototype.
 * @param {Function} cls
 */
function addSnakeCaseAliases (cls) {
  const proto = cls.prototype
  const names = Object.getOwnPropertyNames(proto)
  for (const name of names) {
    if (name === 'constructor') continue
    const snake = name.replace(/([A-Z])/g, '_$1').toLowerCase()
    if (snake !== name && !(snake in proto)) {
      const desc = Object.getOwnPropertyDescriptor(proto, name)
      if (desc) Object.defineProperty(proto, snake, desc)
    }
  }
}

/* ------------------------------------------------------------------ */
/*  Base class                                                         */
/* ------------------------------------------------------------------ */

/**
 * Base class for all Praat object wrappers.
 * Wraps a Praat object ID and provides common operations.
 */
export class PraatObject {
  /** @type {import('./praat-wasm.mjs').PraatWasmInstance} */
  #praat
  /** @type {number} */
  #id
  /** @type {string} */
  #type
  /** @type {string} */
  #name

  /**
   * @param {import('./praat-wasm.mjs').PraatWasmInstance} praat
   * @param {number} id
   * @param {string} type
   * @param {string} name
   */
  constructor (praat, id, type, name) {
    this.#praat = praat
    this.#id = id
    this.#type = type
    this.#name = name
  }

  /** @returns {number} Praat object ID */
  get id () { return this.#id }
  /** @returns {string} Class name (e.g. "Sound", "Pitch") */
  get className () { return this.#type }
  /** @returns {string} Alias for className (backward compat). */
  get type () { return this.#type }
  /** @returns {string} Object name */
  get name () { return this.#name }
  /** @returns {string} Full name, e.g. "Sound hello" */
  get fullName () { return this.#type + ' ' + this.#name }
  /** @type {import('./praat-wasm.mjs').PraatWasmInstance} */
  get _praat () { return this.#praat }

  /** Select this object in the Praat object list. */
  _select () {
    this.#praat.select(this.#id)
  }

  /**
   * Execute a Praat command on this object and return raw info string.
   * @param {string} command
   * @param {...(string|number|boolean)} args
   * @returns {string}
   */
  _call (command, ...args) {
    this._select()
    return this.#praat.run(buildCommand(command, args))
  }

  /**
   * Execute a command that creates a new object. Returns wrapped instance.
   * @param {string} command
   * @param {Array} args
   * @returns {PraatObject}
   */
  _callNew (command, ...args) {
    const before = this.#praat.list()
    this._select()
    this.#praat.run(buildCommand(command, args))
    const after = this.#praat.list()
    return after.find(a => !before.some(b => b.id === a.id)) || null
  }

  /**
   * Execute a query command and return a numeric result.
   * @param {string} command
   * @param {...(string|number|boolean)} args
   * @returns {number}
   */
  _queryNumber (command, ...args) {
    return parseNumber(this._call(command, ...args))
  }

  /**
   * Execute a query command and return an integer result.
   * @param {string} command
   * @param {...(string|number|boolean)} args
   * @returns {number}
   */
  _queryInt (command, ...args) {
    return parseInt_(this._call(command, ...args))
  }

  /**
   * Execute a query command and return a string result.
   * @param {string} command
   * @param {...(string|number|boolean)} args
   * @returns {string}
   */
  _queryString (command, ...args) {
    return (this._call(command, ...args) || '').trim()
  }

  /** Get object info text. */
  info () {
    this._select()
    return this.#praat.run('Info\n')
  }

  /** Copy this object. @returns {PraatObject} */
  copy () {
    return this._callNew('Copy', this.#name + '_copy')
  }

  /** Save as Praat text file. @param {string} path */
  saveAsTextFile (path) {
    this._select()
    this.#praat.saveAsText(path)
  }

  /** Save as Praat binary file. @param {string} path */
  saveAsBinaryFile (path) {
    this._select()
    this.#praat.saveAsBinary(path)
  }

  /** Remove this object from the Praat object list. */
  remove () {
    this._select()
    this.#praat.removeSelected()
  }

  toString () {
    return this.fullName
  }
}

/* ------------------------------------------------------------------ */
/*  Function (base for time-domain objects)                            */
/* ------------------------------------------------------------------ */

/**
 * Mixin for Function properties (xmin, xmax).
 * Parselmouth: parselmouth.Function
 * @param {typeof PraatObject} Base
 */
function FunctionMixin (Base) {
  return class extends Base {
    get xmin () { return this._queryNumber('Get start time') }
    get xmax () { return this._queryNumber('Get end time') }
    get xrange () { return [this.xmin, this.xmax] }

    get startTime () { return this.xmin }
    get endTime () { return this.xmax }
    get totalDuration () { return this._queryNumber('Get total duration') }
    get duration () { return this.totalDuration }
    get centreTime () { return (this.xmin + this.xmax) / 2 }

    getStartTime () { return this.startTime }
    getEndTime () { return this.endTime }
    getTotalDuration () { return this.totalDuration }

    shiftXBy (shift) { this._call('Shift times by', shift) }
    shiftXTo (x, newX) { this._call('Shift times to', x, newX) }
    scaleXBy (scale) { this._call('Scale times by', scale) }
    scaleXTo (newXmin, newXmax) { this._call('Scale times to', newXmin, newXmax) }

    /** Aliases matching Parselmouth's TimeFunction */
    shiftTimesBy (seconds) { this.shiftXBy(seconds) }
    shiftTimesTo (time, newTime) { this.shiftXTo(time, newTime) }
    scaleTimesBy (scale) { this.scaleXBy(scale) }
    scaleTimesTo (newStart, newEnd) { this.scaleXTo(newStart, newEnd) }
  }
}

/**
 * Mixin for Sampled properties (dx, nx, x1).
 * @param {typeof PraatObject} Base
 */
function SampledMixin (Base) {
  return class extends FunctionMixin(Base) {
    get dx () { return this._queryNumber('Get time step') }
    get nx () { return this._queryInt('Get number of frames') }
    get x1 () { return this._queryNumber('Get time from frame number', 1) }
    get timeStep () { return this.dx }
    get nFrames () { return this.nx }

    getTimeStep () { return this.dx }
    getNumberOfFrames () { return this.nx }
    getTimeFromFrameNumber (frame) { return this._queryNumber('Get time from frame number', frame) }
    getFrameNumberFromTime (time) { return this._queryNumber('Get frame number from time', time) }
    frameNumberToTime (frame) { return this.getTimeFromFrameNumber(frame) }
    timeToFrameNumber (time) { return this.getFrameNumberFromTime(time) }
  }
}

/* ------------------------------------------------------------------ */
/*  Sound                                                              */
/* ------------------------------------------------------------------ */

export class Sound extends FunctionMixin(PraatObject) {
  /* -- Properties -- */
  get samplingFrequency () { return this._queryNumber('Get sampling frequency') }
  get samplingPeriod () { return this._queryNumber('Get sampling period') }
  get nSamples () { return this._queryInt('Get number of samples') }
  get nChannels () { return this._queryInt('Get number of channels') }

  getSamplingFrequency () { return this.samplingFrequency }
  getSamplingPeriod () { return this.samplingPeriod }
  getNumberOfSamples () { return this.nSamples }
  getNumberOfChannels () { return this.nChannels }

  /* -- Query -- */
  getEnergy (fromTime, toTime) {
    return this._queryNumber('Get energy', fromTime ?? 0, toTime ?? 0)
  }
  getEnergyInAir () { return this._queryNumber('Get energy in air') }
  getIntensity () { return this._queryNumber('Get intensity (dB)') }
  getPower (fromTime, toTime) {
    return this._queryNumber('Get power', fromTime ?? 0, toTime ?? 0)
  }
  getPowerInAir () { return this._queryNumber('Get power in air') }
  getRms (fromTime, toTime) {
    return this._queryNumber('Get root-mean-square', fromTime ?? 0, toTime ?? 0)
  }
  getRootMeanSquare (fromTime, toTime) { return this.getRms(fromTime, toTime) }
  getIndexFromTime (time) { return this._queryNumber('Get sample number from time', time) }
  getTimeFromIndex (sample) { return this._queryNumber('Get time from sample number', sample) }
  getNearestZeroCrossing (time, channel) {
    return this._queryNumber('Get nearest zero crossing', time, channel ?? 1)
  }

  /* -- Analysis: create new objects -- */

  /**
   * @param {number} [timeStep] - 0 = auto
   * @param {number} [pitchFloor] - default 75
   * @param {number} [pitchCeiling] - default 600
   * @returns {Pitch}
   */
  toPitch (timeStep, pitchFloor, pitchCeiling) {
    return /** @type {Pitch} */ (this._callNew('To Pitch',
      timeStep ?? 0, pitchFloor ?? 75, pitchCeiling ?? 600))
  }

  toPitchAc (timeStep, pitchFloor, maxCandidates, veryAccurate,
    silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost,
    voicedUnvoicedCost, pitchCeiling) {
    return /** @type {Pitch} */ (this._callNew('To Pitch (ac)',
      timeStep ?? 0, pitchFloor ?? 75, maxCandidates ?? 15,
      veryAccurate ?? false, silenceThreshold ?? 0.03,
      voicingThreshold ?? 0.45, octaveCost ?? 0.01,
      octaveJumpCost ?? 0.35, voicedUnvoicedCost ?? 0.14,
      pitchCeiling ?? 600))
  }

  toPitchCc (timeStep, pitchFloor, maxCandidates, veryAccurate,
    silenceThreshold, voicingThreshold, octaveCost, octaveJumpCost,
    voicedUnvoicedCost, pitchCeiling) {
    return /** @type {Pitch} */ (this._callNew('To Pitch (cc)',
      timeStep ?? 0, pitchFloor ?? 75, maxCandidates ?? 15,
      veryAccurate ?? false, silenceThreshold ?? 0.03,
      voicingThreshold ?? 0.45, octaveCost ?? 0.01,
      octaveJumpCost ?? 0.35, voicedUnvoicedCost ?? 0.14,
      pitchCeiling ?? 600))
  }

  toPitchShs (timeStep, minPitch, maxCandidates, maxFreqComponent,
    maxSubharmonics, compressionFactor, ceiling, pointsPerOctave) {
    return /** @type {Pitch} */ (this._callNew('To Pitch (shs)',
      timeStep ?? 0.01, minPitch ?? 50, maxCandidates ?? 15,
      maxFreqComponent ?? 1250, maxSubharmonics ?? 15,
      compressionFactor ?? 0.84, ceiling ?? 600,
      pointsPerOctave ?? 48))
  }

  toPitchSpinet (timeStep, windowLength, minFilterFreq, maxFilterFreq,
    nFilters, ceiling, maxCandidates) {
    return /** @type {Pitch} */ (this._callNew('To Pitch (SPINET)',
      timeStep ?? 0.005, windowLength ?? 0.04,
      minFilterFreq ?? 70, maxFilterFreq ?? 5000,
      nFilters ?? 250, ceiling ?? 500, maxCandidates ?? 15))
  }

  /**
   * @param {number} [timeStep]
   * @param {number} [maxFormants] - default 5
   * @param {number} [maxFormantHz] - default 5500
   * @param {number} [windowLength] - default 0.025
   * @param {number} [preEmphasis] - default 50
   * @returns {Formant}
   */
  toFormantBurg (timeStep, maxFormants, maxFormantHz, windowLength, preEmphasis) {
    return /** @type {Formant} */ (this._callNew('To Formant (burg)',
      timeStep ?? 0, maxFormants ?? 5, maxFormantHz ?? 5500,
      windowLength ?? 0.025, preEmphasis ?? 50))
  }

  /**
   * @param {number} [minPitch] - default 100
   * @param {number} [timeStep]
   * @param {boolean} [subtractMean] - default true
   * @returns {Intensity}
   */
  toIntensity (minPitch, timeStep, subtractMean) {
    return /** @type {Intensity} */ (this._callNew('To Intensity',
      minPitch ?? 100, timeStep ?? 0, subtractMean ?? true))
  }

  /**
   * @param {number} [timeStep] - default 0.01
   * @param {number} [minPitch] - default 75
   * @param {number} [silenceThreshold] - default 0.1
   * @param {number} [periodsPerWindow] - default 1.0
   * @returns {Harmonicity}
   */
  toHarmonicityAc (timeStep, minPitch, silenceThreshold, periodsPerWindow) {
    return /** @type {Harmonicity} */ (this._callNew('To Harmonicity (ac)',
      timeStep ?? 0.01, minPitch ?? 75,
      silenceThreshold ?? 0.1, periodsPerWindow ?? 4.5))
  }

  toHarmonicityCc (timeStep, minPitch, silenceThreshold, periodsPerWindow) {
    return /** @type {Harmonicity} */ (this._callNew('To Harmonicity (cc)',
      timeStep ?? 0.01, minPitch ?? 75,
      silenceThreshold ?? 0.1, periodsPerWindow ?? 4.5))
  }

  /** @param {string} [method] - "cc" (default) or "ac" */
  toHarmonicity (method, ...args) {
    if (!method || method === 'cc') return this.toHarmonicityCc(...args)
    if (method === 'ac') return this.toHarmonicityAc(...args)
    return this.toHarmonicityCc(...args)
  }

  /** @param {boolean} [fast] - default true; @returns {Spectrum} */
  toSpectrum (fast) {
    return /** @type {Spectrum} */ (this._callNew('To Spectrum',
      fast ?? true))
  }

  /**
   * @param {number} [windowLength] - default 0.005
   * @param {number} [maxFrequency] - default 5000
   * @param {number} [timeStep] - default 0.002
   * @param {number} [frequencyStep] - default 20
   * @param {string} [windowShape] - default "Gaussian"
   * @returns {Spectrogram}
   */
  toSpectrogram (windowLength, maxFrequency, timeStep, frequencyStep, windowShape) {
    return /** @type {Spectrogram} */ (this._callNew('To Spectrogram',
      windowLength ?? 0.005, maxFrequency ?? 5000,
      timeStep ?? 0.002, frequencyStep ?? 20,
      windowShape ?? 'Gaussian'))
  }

  /**
   * @param {number} [nCoefficients] - default 12
   * @param {number} [windowLength] - default 0.015
   * @param {number} [timeStep] - default 0.005
   * @param {number} [firstFilterFreq] - default 100
   * @param {number} [distBetweenFilters] - default 100
   * @param {number} [maxFrequency]
   * @returns {MFCC}
   */
  toMfcc (nCoefficients, windowLength, timeStep, firstFilterFreq,
    distBetweenFilters, maxFrequency) {
    return /** @type {MFCC} */ (this._callNew('To MFCC',
      nCoefficients ?? 12, windowLength ?? 0.015,
      timeStep ?? 0.005, firstFilterFreq ?? 100,
      distBetweenFilters ?? 100, maxFrequency ?? 0))
  }

  toPointProcess (method, ...args) {
    if (!method || method === 'periodic_cc') {
      return this._callNew('To PointProcess (periodic, cc)',
        args[0] ?? 75, args[1] ?? 600)
    }
    if (method === 'periodic_peaks') {
      return this._callNew('To PointProcess (periodic, peaks)',
        args[0] ?? 75, args[1] ?? 600, args[2] ?? true)
    }
    if (method === 'zeroes') {
      return this._callNew('To PointProcess (zeroes)',
        args[0] ?? 1, args[1] ?? true, args[2] ?? true)
    }
    return this._callNew('To PointProcess (periodic, cc)',
      args[0] ?? 75, args[1] ?? 600)
  }

  /* -- Modification -- */

  resample (newFrequency, precision) {
    return /** @type {Sound} */ (this._callNew('Resample',
      newFrequency, precision ?? 50))
  }

  extractPart (fromTime, toTime, windowShape, relativeWidth, preserveTimes) {
    return /** @type {Sound} */ (this._callNew('Extract part',
      fromTime ?? 0, toTime ?? 0,
      windowShape ?? 'rectangular',
      relativeWidth ?? 1.0,
      preserveTimes ?? false))
  }

  extractChannel (channel) {
    return /** @type {Sound} */ (this._callNew('Extract one channel', channel))
  }

  extractLeftChannel () { return this.extractChannel(1) }
  extractRightChannel () { return this.extractChannel(2) }

  convertToMono () {
    return /** @type {Sound} */ (this._callNew('Convert to mono'))
  }

  convertToStereo () {
    return /** @type {Sound} */ (this._callNew('Convert to stereo'))
  }

  reverse (fromTime, toTime) {
    this._call('Reverse', fromTime ?? 0, toTime ?? 0)
  }

  scaleIntensity (newAverage) {
    this._call('Scale intensity', newAverage)
  }

  preEmphasize (fromFrequency, normalize) {
    this._call('Pre-emphasize (in-place)', fromFrequency ?? 50, normalize ?? true)
  }

  deEmphasize (fromFrequency, normalize) {
    this._call('De-emphasize (in-place)', fromFrequency ?? 50, normalize ?? true)
  }

  multiplyByWindow (windowShape) {
    this._call('Multiply by window', windowShape)
  }

  setToZero (fromTime, toTime, roundToZeroCrossing) {
    this._call('Set part to zero', fromTime ?? 0, toTime ?? 0,
      roundToZeroCrossing ?? true)
  }

  overrideSamplingFrequency (newFreq) {
    this._call('Override sampling frequency', newFreq)
  }

  lengthen (minPitch, maxPitch, factor) {
    return /** @type {Sound} */ (this._callNew('Lengthen (overlap-add)',
      minPitch ?? 75, maxPitch ?? 600, factor))
  }

  deepenBandModulation (enhancement, fromFreq, toFreq, slowMod, fastMod, bandSmoothing) {
    return /** @type {Sound} */ (this._callNew('Deepen band modulation',
      enhancement ?? 20, fromFreq ?? 300, toFreq ?? 8000,
      slowMod ?? 3, fastMod ?? 30, bandSmoothing ?? 100))
  }

  autocorrelate (scaling, signalOutside) {
    return /** @type {Sound} */ (this._callNew('Autocorrelate',
      scaling ?? 'peak 0.99', signalOutside ?? 'zero'))
  }

  convolve (other, scaling, signalOutside) {
    const before = this._praat.list()
    this._praat.run(
      'selectObject: ' + this.id + '\n' +
      'plusObject: ' + other.id + '\n' +
      'Convolve: "' + (scaling ?? 'peak 0.99') + '", "' + (signalOutside ?? 'zero') + '"\n'
    )
    const after = this._praat.list()
    const newObj = after.find(a => !before.some(b => b.id === a.id))
    return newObj ? wrapObject(this._praat, newObj) : null
  }

  crossCorrelate (other, scaling, signalOutside) {
    const before = this._praat.list()
    this._praat.run(
      'selectObject: ' + this.id + '\n' +
      'plusObject: ' + other.id + '\n' +
      'Cross-correlate: "' + (scaling ?? 'peak 0.99') + '", "' + (signalOutside ?? 'zero') + '"\n'
    )
    const after = this._praat.list()
    const newObj = after.find(a => !before.some(b => b.id === a.id))
    return newObj ? wrapObject(this._praat, newObj) : null
  }

  formula (formulaStr) {
    this._call('Formula', formulaStr)
  }

  /* -- File format saves -- */

  saveAsWav (path) {
    this._select()
    this._praat.run('Save as WAV file: "' + path + '"\n')
  }
  saveAsFlac (path) {
    this._select()
    this._praat.run('Save as FLAC file: "' + path + '"\n')
  }
  saveAsAiff (path) {
    this._select()
    this._praat.run('Save as AIFF file: "' + path + '"\n')
  }
}

/* ------------------------------------------------------------------ */
/*  Pitch                                                              */
/* ------------------------------------------------------------------ */

export class Pitch extends SampledMixin(PraatObject) {
  get ceiling () { return this._queryNumber('Get ceiling') }
  get maxNCandidates () { return this._queryInt('Get max number of candidates') }

  getValueAtTime (time, unit, interpolation) {
    return this._queryNumber('Get value at time',
      time, unit ?? 'Hertz', interpolation ?? 'Linear')
  }

  getValueInFrame (frame, unit) {
    return this._queryNumber('Get value in frame', frame, unit ?? 'Hertz')
  }

  getMeanAbsoluteSlope (unit) {
    return parseNumber(this._call('Get mean absolute slope', unit ?? 'Hertz'))
  }

  getSlopeWithoutOctaveJumps () {
    return this._queryNumber('Get slope without octave jumps')
  }

  countVoicedFrames () {
    return this._queryInt('Count voiced frames')
  }

  /** @returns {Pitch} */
  interpolate () {
    return /** @type {Pitch} */ (this._callNew('Interpolate'))
  }

  /** @returns {Pitch} */
  smooth (bandwidth) {
    return /** @type {Pitch} */ (this._callNew('Smooth', bandwidth ?? 10))
  }

  /** @returns {Pitch} */
  killOctaveJumps () {
    return /** @type {Pitch} */ (this._callNew('Kill octave jumps'))
  }

  /** @returns {Pitch} */
  subtractLinearFit (unit) {
    return /** @type {Pitch} */ (this._callNew('Subtract linear fit', unit ?? 'Hertz'))
  }

  pathFinder (silenceThreshold, voicingThreshold, octaveCost,
    octaveJumpCost, voicedUnvoicedCost, ceiling, pullFormants) {
    this._call('Path finder', silenceThreshold ?? 0.03,
      voicingThreshold ?? 0.45, octaveCost ?? 0.01,
      octaveJumpCost ?? 0.35, voicedUnvoicedCost ?? 0.14,
      ceiling ?? 600, pullFormants ?? false)
  }

  step (stepVal, precision, fromTime, toTime) {
    this._call('Step', stepVal, precision ?? 0.1, fromTime ?? 0, toTime ?? 0)
  }

  octaveUp (fromTime, toTime) { this._call('Octave up', fromTime ?? 0, toTime ?? 0) }
  octaveDown (fromTime, toTime) { this._call('Octave down', fromTime ?? 0, toTime ?? 0) }
  fifthUp (fromTime, toTime) { this._call('Fifth up', fromTime ?? 0, toTime ?? 0) }
  fifthDown (fromTime, toTime) { this._call('Fifth down', fromTime ?? 0, toTime ?? 0) }
  unvoice (fromTime, toTime) { this._call('Unvoice', fromTime ?? 0, toTime ?? 0) }

  formula (formulaStr) { this._call('Formula', formulaStr) }

  toMatrix () { return this._callNew('Down to Matrix') }

  toSoundPulses (fromTime, toTime) {
    return /** @type {Sound} */ (this._callNew('To Sound (pulses)', fromTime ?? 0, toTime ?? 0))
  }
  toSoundHum (fromTime, toTime) {
    return /** @type {Sound} */ (this._callNew('To Sound (hum)', fromTime ?? 0, toTime ?? 0))
  }
  toSoundSine (fromTime, toTime, samplingFreq, roundToZero) {
    return /** @type {Sound} */ (this._callNew('To Sound (sine)',
      fromTime ?? 0, toTime ?? 0, samplingFreq ?? 44100,
      roundToZero ?? true))
  }
}

/* ------------------------------------------------------------------ */
/*  Formant                                                            */
/* ------------------------------------------------------------------ */

export class Formant extends SampledMixin(PraatObject) {
  getValueAtTime (formantNumber, time, unit) {
    return this._queryNumber('Get value at time',
      formantNumber, time, unit ?? 'hertz', 'Linear')
  }

  getBandwidthAtTime (formantNumber, time, unit) {
    return this._queryNumber('Get bandwidth at time',
      formantNumber, time, unit ?? 'hertz', 'Linear')
  }

  getNumberOfFormants (frame) {
    return this._queryInt('Get number of formants', frame)
  }

  getQuantile (formantNumber, fromTime, toTime, units, quantile) {
    return this._queryNumber('Get quantile',
      formantNumber, fromTime ?? 0, toTime ?? 0,
      units ?? 'hertz', quantile ?? 0.5)
  }

  getMean (formantNumber, fromTime, toTime, unit) {
    return this._queryNumber('Get mean',
      formantNumber, fromTime ?? 0, toTime ?? 0, unit ?? 'hertz')
  }

  getStandardDeviation (formantNumber, fromTime, toTime, unit) {
    return this._queryNumber('Get standard deviation',
      formantNumber, fromTime ?? 0, toTime ?? 0, unit ?? 'hertz')
  }

  formula (frequencies, bandwidths) {
    if (frequencies) this._call('Formula (frequencies)', frequencies)
    if (bandwidths) this._call('Formula (bandwidths)', bandwidths)
  }

  downToTable (includeFrameNumber, includeTime, numDecimals, includeBandwidth, includeIntensity) {
    return this._callNew('Down to Table',
      includeFrameNumber ?? false, includeTime ?? true,
      numDecimals ?? 6, includeBandwidth ?? false,
      includeIntensity ?? false)
  }
}

/* ------------------------------------------------------------------ */
/*  Intensity                                                          */
/* ------------------------------------------------------------------ */

export class Intensity extends SampledMixin(PraatObject) {
  getValue (time, interpolation) {
    return this._queryNumber('Get value at time',
      time, interpolation ?? 'cubic')
  }

  getAverage (fromTime, toTime, averagingMethod) {
    return this._queryNumber('Get mean',
      fromTime ?? 0, toTime ?? 0, averagingMethod ?? 'energy')
  }

  getMinimum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get minimum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getMaximum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get maximum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getTimeOfMinimum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get time of minimum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getTimeOfMaximum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get time of maximum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getStandardDeviation (fromTime, toTime) {
    return this._queryNumber('Get standard deviation',
      fromTime ?? 0, toTime ?? 0)
  }
}

/* ------------------------------------------------------------------ */
/*  Harmonicity                                                        */
/* ------------------------------------------------------------------ */

export class Harmonicity extends SampledMixin(PraatObject) {
  getValue (time, interpolation) {
    return this._queryNumber('Get value at time',
      time, interpolation ?? 'cubic')
  }

  getMinimum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get minimum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getMaximum (fromTime, toTime, interpolation) {
    return this._queryNumber('Get maximum',
      fromTime ?? 0, toTime ?? 0, interpolation ?? 'parabolic')
  }

  getMean (fromTime, toTime) {
    return this._queryNumber('Get mean', fromTime ?? 0, toTime ?? 0)
  }

  getStandardDeviation (fromTime, toTime) {
    return this._queryNumber('Get standard deviation',
      fromTime ?? 0, toTime ?? 0)
  }
}

/* ------------------------------------------------------------------ */
/*  Spectrum                                                           */
/* ------------------------------------------------------------------ */

export class Spectrum extends PraatObject {
  get nBins () { return this._queryInt('Get number of bins') }
  get df () { return this._queryNumber('Get bin width') }
  get binWidth () { return this.df }
  get fmin () { return this._queryNumber('Get lowest frequency') }
  get fmax () { return this._queryNumber('Get highest frequency') }
  get lowestFrequency () { return this.fmin }
  get highestFrequency () { return this.fmax }

  getNumberOfBins () { return this.nBins }
  getBinWidth () { return this.df }
  getLowestFrequency () { return this.fmin }
  getHighestFrequency () { return this.fmax }

  getBinNumberFromFrequency (freq) {
    return this._queryNumber('Get bin number from frequency', freq)
  }

  getFrequencyFromBinNumber (bin) {
    return this._queryNumber('Get frequency from bin number', bin)
  }

  getRealValueInBin (bin) { return this._queryNumber('Get real value in bin', bin) }
  getImaginaryValueInBin (bin) { return this._queryNumber('Get imaginary value in bin', bin) }

  setRealValueInBin (bin, value) { this._call('Formula', 'if col = ' + bin + ' then self.re := ' + value + ' else self fi') }
  setImaginaryValueInBin (bin, value) { this._call('Formula', 'if col = ' + bin + ' then self.im := ' + value + ' else self fi') }

  getBandEnergy (bandFloor, bandCeiling) {
    return this._queryNumber('Get band energy', bandFloor ?? 0, bandCeiling ?? 0)
  }

  getBandDensity (bandFloor, bandCeiling) {
    return this._queryNumber('Get band density', bandFloor ?? 0, bandCeiling ?? 0)
  }

  getBandEnergyDifference (lowFloor, lowCeiling, highFloor, highCeiling) {
    return this._queryNumber('Get band energy difference',
      lowFloor ?? 0, lowCeiling ?? 0, highFloor ?? 0, highCeiling ?? 0)
  }

  getBandDensityDifference (lowFloor, lowCeiling, highFloor, highCeiling) {
    return this._queryNumber('Get band density difference',
      lowFloor ?? 0, lowCeiling ?? 0, highFloor ?? 0, highCeiling ?? 0)
  }

  getCenterOfGravity (power) {
    return this._queryNumber('Get centre of gravity', power ?? 2)
  }
  getCentreOfGravity (power) { return this.getCenterOfGravity(power) }

  getStandardDeviation (power) {
    return this._queryNumber('Get standard deviation', power ?? 2)
  }

  getSkewness (power) {
    return this._queryNumber('Get skewness', power ?? 2)
  }

  getKurtosis (power) {
    return this._queryNumber('Get kurtosis', power ?? 2)
  }

  getCentralMoment (moment, power) {
    return this._queryNumber('Get central moment', moment, power ?? 2)
  }

  /** @returns {Spectrum} */
  cepstralSmoothing (bandwidth) {
    return /** @type {Spectrum} */ (this._callNew('Cepstral smoothing', bandwidth ?? 500))
  }

  lpcSmoothing (numPeaks, preEmphasis) {
    return /** @type {Spectrum} */ (this._callNew('LPC smoothing', numPeaks ?? 5, preEmphasis ?? 50))
  }

  /** @returns {Sound} */
  toSound () {
    return /** @type {Sound} */ (this._callNew('To Sound'))
  }

  toSpectrogram () {
    return /** @type {Spectrogram} */ (this._callNew('To Spectrogram'))
  }

  formula (formulaStr) { this._call('Formula', formulaStr) }
}

/* ------------------------------------------------------------------ */
/*  Spectrogram                                                        */
/* ------------------------------------------------------------------ */

export class Spectrogram extends PraatObject {
  getPowerAt (time, frequency) {
    return this._queryNumber('Get power at', time, frequency)
  }

  /** @returns {Sound} */
  toSound (samplingFrequency) {
    return /** @type {Sound} */ (this._callNew('To Sound', samplingFrequency ?? 44100))
  }

  toSpectrumSlice (time) {
    return /** @type {Spectrum} */ (this._callNew('To Spectrum (slice)', time))
  }

  formula (formulaStr) { this._call('Formula', formulaStr) }
}

/* ------------------------------------------------------------------ */
/*  MFCC                                                               */
/* ------------------------------------------------------------------ */

export class MFCC extends SampledMixin(PraatObject) {
  get fmin () { return this._queryNumber('Get lowest frequency') }
  get fmax () { return this._queryNumber('Get highest frequency') }
  get nCoefficients () { return this._queryInt('Get number of coefficients', 1) }

  getValueInFrame (frame, index) {
    return this._queryNumber('Get value in frame', frame, index)
  }

  getC0ValueInFrame (frame) {
    return this._queryNumber('Get c0 value in frame', frame)
  }

  getNumberOfCoefficients (frame) {
    return this._queryInt('Get number of coefficients', frame)
  }

  /** @returns {Sound} */
  toSound () {
    return /** @type {Sound} */ (this._callNew('To Sound'))
  }

  toMatrix () { return this._callNew('To Matrix') }

  toMelSpectrogram (fromCoeff, toCoeff, includeC0) {
    return this._callNew('To MelSpectrogram',
      fromCoeff ?? 0, toCoeff ?? 0, includeC0 ?? false)
  }
}

/* ------------------------------------------------------------------ */
/*  TextGrid                                                           */
/* ------------------------------------------------------------------ */

export class TextGrid extends FunctionMixin(PraatObject) {
  getNumberOfTiers () { return this._queryInt('Get number of tiers') }
  getTierName (tier) { return this._queryString('Get tier name', tier) }
  isIntervalTier (tier) { return parseBool(this._call('Is interval tier', tier)) }

  getNumberOfIntervals (tier) { return this._queryInt('Get number of intervals', tier) }
  getNumberOfPoints (tier) { return this._queryInt('Get number of points', tier) }

  getLabelOfInterval (tier, interval) {
    return this._queryString('Get label of interval', tier, interval)
  }

  getStartTimeOfInterval (tier, interval) {
    return this._queryNumber('Get start time of interval', tier, interval)
  }

  getEndTimeOfInterval (tier, interval) {
    return this._queryNumber('Get end time of interval', tier, interval)
  }

  getIntervalAtTime (tier, time) {
    return this._queryInt('Get interval at time', tier, time)
  }

  getLabelOfPoint (tier, point) {
    return this._queryString('Get label of point', tier, point)
  }

  getTimeOfPoint (tier, point) {
    return this._queryNumber('Get time of point', tier, point)
  }

  getLowIntervalAtTime (tier, time) {
    return this._queryInt('Get low interval at time', tier, time)
  }

  getHighIntervalAtTime (tier, time) {
    return this._queryInt('Get high interval at time', tier, time)
  }

  getNearestIndexFromTime (tier, time) {
    return this._queryInt('Get nearest index from time', tier, time)
  }

  /* -- Modification -- */

  setIntervalText (tier, interval, text) {
    this._call('Set interval text', tier, interval, text)
  }

  setPointText (tier, point, text) {
    this._call('Set point text', tier, point, text)
  }

  insertBoundary (tier, time) {
    this._call('Insert boundary', tier, time)
  }

  removeBoundaryAtTime (tier, time) {
    this._call('Remove boundary at time', tier, time)
  }

  insertPoint (tier, time, text) {
    this._call('Insert point', tier, time, text ?? '')
  }

  removePoint (tier, point) {
    this._call('Remove point', tier, point)
  }

  insertIntervalTier (position, name) {
    this._call('Insert interval tier', position, name)
  }

  insertPointTier (position, name) {
    this._call('Insert point tier', position, name)
  }

  removeTier (position) {
    this._call('Remove tier', position)
  }

  /* -- Extract -- */

  extractOneTier (tier) {
    return /** @type {TextGrid} */ (this._callNew('Extract one tier', tier))
  }

  extractPart (fromTime, toTime, preserveTimes) {
    return /** @type {TextGrid} */ (this._callNew('Extract part',
      fromTime, toTime, preserveTimes ?? false))
  }
}

/* ------------------------------------------------------------------ */
/*  PointProcess                                                       */
/* ------------------------------------------------------------------ */

export class PointProcess extends FunctionMixin(PraatObject) {
  getNumberOfPoints () { return this._queryInt('Get number of points') }

  getTimeFromIndex (index) {
    return this._queryNumber('Get time from index', index)
  }

  getNearestIndex (time) {
    return this._queryInt('Get nearest index', time)
  }

  getInterval (time) {
    return this._queryNumber('Get interval', time)
  }

  getNumberOfPeriods (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryInt('Get number of periods',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  getMeanPeriod (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryNumber('Get mean period',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  getJitter (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryNumber('Get jitter (local)',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  getJitterRap (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryNumber('Get jitter (rap)',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  getJitterPpq5 (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryNumber('Get jitter (ppq5)',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  getJitterDdp (fromTime, toTime, minPeriod, maxPeriod, maxPeriodFactor) {
    return this._queryNumber('Get jitter (ddp)',
      fromTime ?? 0, toTime ?? 0,
      minPeriod ?? 0.0001, maxPeriod ?? 0.02, maxPeriodFactor ?? 1.3)
  }

  /** Add a point. */
  addPoint (time) { this._call('Add point', time) }

  /** Remove point by index. */
  removePoint (index) { this._call('Remove point', index) }

  /** Remove points between times. */
  removePointsBetween (fromTime, toTime) {
    this._call('Remove points between', fromTime, toTime)
  }

  toSound (samplingFrequency) {
    return /** @type {Sound} */ (this._callNew('To Sound (phonation)',
      samplingFrequency ?? 44100, 1, 0.05, 0.7, 0.03, 3, 4))
  }
}

/* ------------------------------------------------------------------ */
/*  LPC                                                                */
/* ------------------------------------------------------------------ */

export class LPC extends SampledMixin(PraatObject) {
  getSamplingInterval () { return this._queryNumber('Get sampling interval') }

  getNumberOfCoefficients (frame) {
    return this._queryInt('Get number of coefficients', frame)
  }

  /* -- Conversion -- */
  toFormant () { return /** @type {Formant} */ (this._callNew('To Formant')) }
  toSpectrumSlice (time, minFreqHz, maxFreqHz, nFreq) {
    return /** @type {Spectrum} */ (this._callNew('To Spectrum (slice)',
      time, minFreqHz ?? 20, maxFreqHz ?? 0, nFreq ?? 0))
  }
  toMatrix () { return this._callNew('To Matrix') }
}

/* ------------------------------------------------------------------ */
/*  Matrix (generic)                                                   */
/* ------------------------------------------------------------------ */

export class Matrix extends PraatObject {
  get nRows () { return this._queryInt('Get number of rows') }
  get nColumns () { return this._queryInt('Get number of columns') }
  get xmin () { return this._queryNumber('Get lowest x') }
  get xmax () { return this._queryNumber('Get highest x') }
  get ymin () { return this._queryNumber('Get lowest y') }
  get ymax () { return this._queryNumber('Get highest y') }
  get dx () { return this._queryNumber('Get column distance') }
  get dy () { return this._queryNumber('Get row distance') }

  getValueInCell (row, column) {
    return this._queryNumber('Get value in cell', row, column)
  }

  getValueAtXy (x, y) {
    return this._queryNumber('Get value at xy', x, y)
  }

  getMinimum () { return this._queryNumber('Get minimum') }
  getMaximum () { return this._queryNumber('Get maximum') }
  getSum () { return this._queryNumber('Get sum') }

  setValue (row, column, value) {
    this._call('Set value', row, column, value)
  }

  formula (formulaStr) { this._call('Formula', formulaStr) }

  getXOfColumn (col) { return this._queryNumber('Get x of column', col) }
  getYOfRow (row) { return this._queryNumber('Get y of row', row) }
}

/* ------------------------------------------------------------------ */
/*  Table                                                              */
/* ------------------------------------------------------------------ */

export class Table extends PraatObject {
  getNumberOfRows () { return this._queryInt('Get number of rows') }
  getNumberOfColumns () { return this._queryInt('Get number of columns') }
  getColumnLabel (column) { return this._queryString('Get column label', column) }

  getValue (row, column) {
    if (typeof column === 'string') {
      return this._queryString('Get value', row, column)
    }
    return this._queryString('Get value', row, column)
  }

  getNumericValue (row, column) {
    if (typeof column === 'string') {
      return this._queryNumber('Get value', row, column)
    }
    return this._queryNumber('Get value', row, column)
  }

  setValue (row, column, value) { this._call('Set string value', row, column, String(value)) }
  setNumericValue (row, column, value) { this._call('Set numeric value', row, column, value) }

  appendRow () { this._call('Append row') }
  appendColumn (label) { this._call('Append column', label) }
  removeRow (row) { this._call('Remove row', row) }
  removeColumn (column) { this._call('Remove column', column) }

  insertRow (row) { this._call('Insert row', row) }
  insertColumn (position, label) { this._call('Insert column', position, label) }

  getMean (column) { return this._queryNumber('Get mean', column) }
  getStdev (column) { return this._queryNumber('Get standard deviation', column) }
  getQuantile (column, quantile) { return this._queryNumber('Get quantile', column, quantile) }
  getMinimum (column) { return this._queryNumber('Get minimum', column) }
  getMaximum (column) { return this._queryNumber('Get maximum', column) }

  sortRows (column) { this._call('Sort rows', column) }
}

/* ------------------------------------------------------------------ */
/*  Object wrapping factory                                            */
/* ------------------------------------------------------------------ */

/** @type {Object<string, typeof PraatObject>} */
const CLASS_MAP = {
  Sound,
  Pitch,
  Formant,
  Intensity,
  Harmonicity,
  Spectrum,
  Spectrogram,
  MFCC,
  TextGrid,
  PointProcess,
  LPC,
  Matrix,
  Table
}

/**
 * Wrap a raw {id, type, name} object into the appropriate class.
 * @param {import('./praat-wasm.mjs').PraatWasmInstance} praat
 * @param {{id: number, type: string, name: string}} obj
 * @returns {PraatObject}
 */
export function wrapObject (praat, obj) {
  const Cls = CLASS_MAP[obj.type] || PraatObject
  return new Cls(praat, obj.id, obj.type, obj.name)
}

/* ------------------------------------------------------------------ */
/*  Apply snake_case aliases to all classes                            */
/* ------------------------------------------------------------------ */

;[PraatObject, Sound, Pitch, Formant, Intensity, Harmonicity,
  Spectrum, Spectrogram, MFCC, TextGrid, PointProcess, LPC,
  Matrix, Table].forEach(addSnakeCaseAliases)
