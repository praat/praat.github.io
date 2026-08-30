/**
 * Type declarations for praat-wasm.
 * Generated from JSDoc annotations. Can be regenerated with:
 *   tsc --declaration --allowJs --emitDeclarationOnly --outDir types wasm/js/praat-wasm.mjs
 */

/** Package version (Praat patch × 100 + JS revision). */
export declare const version: string;

/* ------------------------------------------------------------------ */
/*  Base class                                                         */
/* ------------------------------------------------------------------ */

export declare class PraatObject {
  /** Praat object ID. */
  readonly id: number;
  /** Class name (e.g. "Sound", "Pitch"). */
  readonly className: string;
  /** Alias for className. */
  readonly type: string;
  /** Object name. */
  readonly name: string;
  /** Full name, e.g. "Sound hello". */
  readonly fullName: string;

  /** Get object info text. */
  info(): string;
  /** Copy this object. */
  copy(): PraatObject;
  /** Save as Praat text file. */
  saveAsTextFile(path: string): void;
  /** Save as Praat binary file. */
  saveAsBinaryFile(path: string): void;
  /** Remove this object from the Praat object list. */
  remove(): void;
  toString(): string;
}

/* ------------------------------------------------------------------ */
/*  Function/Sampled properties (mixin interfaces)                     */
/* ------------------------------------------------------------------ */

interface FunctionProperties {
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;
  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;
}

interface SampledProperties extends FunctionProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
}

/* ------------------------------------------------------------------ */
/*  Sound                                                              */
/* ------------------------------------------------------------------ */

export declare class Sound extends PraatObject implements FunctionProperties {
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;
  readonly samplingFrequency: number;
  readonly samplingPeriod: number;
  readonly nSamples: number;
  readonly nChannels: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getSamplingFrequency(): number;
  getSamplingPeriod(): number;
  getNumberOfSamples(): number;
  getNumberOfChannels(): number;

  getEnergy(fromTime?: number, toTime?: number): number;
  getEnergyInAir(): number;
  getIntensity(): number;
  getPower(fromTime?: number, toTime?: number): number;
  getPowerInAir(): number;
  getRms(fromTime?: number, toTime?: number): number;
  getRootMeanSquare(fromTime?: number, toTime?: number): number;
  getIndexFromTime(time: number): number;
  getTimeFromIndex(sample: number): number;
  getNearestZeroCrossing(time: number, channel?: number): number;

  /** Convert to Pitch (autocorrelation method). */
  toPitch(timeStep?: number, pitchFloor?: number, pitchCeiling?: number): Pitch;
  /** Convert to Pitch (autocorrelation method, full params). */
  toPitchAc(timeStep?: number, pitchFloor?: number, maxCandidates?: number,
    veryAccurate?: boolean, silenceThreshold?: number, voicingThreshold?: number,
    octaveCost?: number, octaveJumpCost?: number, voicedUnvoicedCost?: number,
    pitchCeiling?: number): Pitch;
  /** Convert to Pitch (cross-correlation method). */
  toPitchCc(timeStep?: number, pitchFloor?: number, maxCandidates?: number,
    veryAccurate?: boolean, silenceThreshold?: number, voicingThreshold?: number,
    octaveCost?: number, octaveJumpCost?: number, voicedUnvoicedCost?: number,
    pitchCeiling?: number): Pitch;

  toFormantBurg(timeStep?: number, maxFormants?: number, maxFormantHz?: number,
    windowLength?: number, preEmphasis?: number): Formant;
  toIntensity(minPitch?: number, timeStep?: number, subtractMean?: boolean): Intensity;
  toHarmonicityAc(timeStep?: number, minPitch?: number,
    silenceThreshold?: number, periodsPerWindow?: number): Harmonicity;
  toHarmonicityCc(timeStep?: number, minPitch?: number,
    silenceThreshold?: number, periodsPerWindow?: number): Harmonicity;
  toHarmonicity(method?: string, ...args: any[]): Harmonicity;
  toSpectrum(fast?: boolean): Spectrum;
  toSpectrogram(windowLength?: number, maxFrequency?: number,
    timeStep?: number, frequencyStep?: number, windowShape?: string): Spectrogram;
  toMfcc(nCoefficients?: number, windowLength?: number,
    timeStep?: number, firstFilterFreq?: number, distanceBetweenFilters?: number,
    maxFilterFreq?: number): MFCC;
  toPointProcess(method?: string, ...args: any[]): PointProcess;

  resample(newFrequency: number, precision?: number): Sound;
  extractPart(fromTime: number, toTime: number, windowShape?: string,
    relativeWidth?: number, preserveTimes?: boolean): Sound;
  extractChannel(channel: number): Sound;
  extractLeftChannel(): Sound;
  extractRightChannel(): Sound;
  convertToMono(): Sound;
  convertToStereo(): Sound;
  reverse(fromTime?: number, toTime?: number): void;
  scaleIntensity(newAverage: number): void;
  preEmphasize(fromFrequency?: number, normalize?: boolean): void;
  deEmphasize(fromFrequency?: number, normalize?: boolean): void;
  multiplyByWindow(windowShape?: string): void;
  setToZero(fromTime?: number, toTime?: number, roundToZeroCrossing?: boolean): void;
  overrideSamplingFrequency(newFreq: number): void;
  lengthen(minPitch?: number, maxPitch?: number, factor?: number): Sound;
  deepenBandModulation(enhancement?: number, fromFreq?: number, toFreq?: number,
    slowMod?: number, fastMod?: number, bandSmoothing?: number): Sound;
  autocorrelate(scaling?: string, signalOutside?: string): Sound;
  convolve(other: Sound, scaling?: string, signalOutside?: string): Sound;
  crossCorrelate(other: Sound, scaling?: string, signalOutside?: string): Sound;
  formula(formulaStr: string): void;
  saveAsWav(path: string): void;
  saveAsFlac(path: string): void;
  saveAsAiff(path: string): void;

  /* snake_case aliases */
  to_pitch: Sound['toPitch'];
  to_pitch_ac: Sound['toPitchAc'];
  to_pitch_cc: Sound['toPitchCc'];
  to_formant_burg: Sound['toFormantBurg'];
  to_intensity: Sound['toIntensity'];
  to_harmonicity: Sound['toHarmonicity'];
  to_spectrum: Sound['toSpectrum'];
  to_spectrogram: Sound['toSpectrogram'];
  to_mfcc: Sound['toMfcc'];
  to_point_process: Sound['toPointProcess'];
}

/* ------------------------------------------------------------------ */
/*  Pitch                                                              */
/* ------------------------------------------------------------------ */

export declare class Pitch extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;
  readonly ceiling: number;
  readonly maxNCandidates: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getValueAtTime(time: number, unit?: string, interpolation?: string): number;
  getValueInFrame(frame: number, unit?: string): number;
  getMeanAbsoluteSlope(unit?: string): number;
  getSlopeWithoutOctaveJumps(): number;
  countVoicedFrames(): number;
  interpolate(): Pitch;
  smooth(bandwidth?: number): Pitch;
  killOctaveJumps(): Pitch;
  subtractLinearFit(unit?: string): Pitch;
  step(stepVal?: number, precision?: number, fromTime?: number, toTime?: number): void;
  octaveUp(fromTime?: number, toTime?: number): void;
  octaveDown(fromTime?: number, toTime?: number): void;
  fifthUp(fromTime?: number, toTime?: number): void;
  fifthDown(fromTime?: number, toTime?: number): void;
  unvoice(fromTime?: number, toTime?: number): void;
  formula(formulaStr: string): void;
  toMatrix(): Matrix;
  toSoundPulses(fromTime?: number, toTime?: number): Sound;
  toSoundHum(fromTime?: number, toTime?: number): Sound;
  toSoundSine(fromTime?: number, toTime?: number, samplingFreq?: number, roundToZero?: boolean): Sound;

  /* snake_case aliases */
  get_value_at_time: Pitch['getValueAtTime'];
  count_voiced_frames: Pitch['countVoicedFrames'];
  kill_octave_jumps: Pitch['killOctaveJumps'];
}

/* ------------------------------------------------------------------ */
/*  Formant                                                            */
/* ------------------------------------------------------------------ */

export declare class Formant extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getValueAtTime(formantNumber: number, time: number, unit?: string): number;
  getBandwidthAtTime(formantNumber: number, time: number, unit?: string): number;
  getNumberOfFormants(frame: number): number;
  getQuantile(formantNumber: number, fromTime?: number, toTime?: number,
    units?: string, quantile?: number): number;
  getMean(formantNumber: number, fromTime?: number, toTime?: number, unit?: string): number;
  getStandardDeviation(formantNumber: number, fromTime?: number, toTime?: number, unit?: string): number;
  formula(frequencies: string, bandwidths?: string): void;
  downToTable(includeFrameNumber?: boolean, includeTime?: boolean, numDecimals?: number,
    includeBandwidth?: boolean, includeIntensity?: boolean): Table;

  /* snake_case aliases */
  get_value_at_time: Formant['getValueAtTime'];
  get_bandwidth_at_time: Formant['getBandwidthAtTime'];
}

/* ------------------------------------------------------------------ */
/*  Intensity                                                          */
/* ------------------------------------------------------------------ */

export declare class Intensity extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getValue(time: number, interpolation?: string): number;
  getAverage(fromTime?: number, toTime?: number, averagingMethod?: string): number;
  getMinimum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getMaximum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getTimeOfMinimum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getTimeOfMaximum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getStandardDeviation(fromTime?: number, toTime?: number): number;
  getMean(fromTime?: number, toTime?: number, averagingMethod?: string): number;
  toMatrix(): Matrix;
}

/* ------------------------------------------------------------------ */
/*  Harmonicity                                                        */
/* ------------------------------------------------------------------ */

export declare class Harmonicity extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getValue(time: number, interpolation?: string): number;
  getMean(fromTime?: number, toTime?: number): number;
  getMinimum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getMaximum(fromTime?: number, toTime?: number, interpolation?: string): number;
  getStandardDeviation(fromTime?: number, toTime?: number): number;
  toMatrix(): Matrix;
  formula(formulaStr: string): void;
}

/* ------------------------------------------------------------------ */
/*  Spectrum                                                           */
/* ------------------------------------------------------------------ */

export declare class Spectrum extends PraatObject {
  readonly lowestFrequency: number;
  readonly highestFrequency: number;
  readonly nBins: number;
  readonly numberOfBins: number;
  readonly frequencyResolution: number;

  getRealValueInBin(bin: number): number;
  getImaginaryValueInBin(bin: number): number;
  getCenterOfGravity(power?: number): number;
  getCentreOfGravity(power?: number): number;
  getStandardDeviation(power?: number): number;
  getSkewness(power?: number): number;
  getKurtosis(power?: number): number;
  getBandEnergy(fromFreq?: number, toFreq?: number): number;
  getBandEnergyDifference(fromFreq1?: number, toFreq1?: number,
    fromFreq2?: number, toFreq2?: number): number;
  cepstralSmoothing(bandwidth?: number): Spectrum;
  passHannBand(fromFreq: number, toFreq: number, smoothing?: number): void;
  stopHannBand(fromFreq: number, toFreq: number, smoothing?: number): void;
  toSound(): Sound;
  toSpectrogram(): Spectrogram;
  formula(formulaStr: string): void;

  /* snake_case aliases */
  get_center_of_gravity: Spectrum['getCenterOfGravity'];
  get_centre_of_gravity: Spectrum['getCentreOfGravity'];
}

/* ------------------------------------------------------------------ */
/*  Spectrogram                                                        */
/* ------------------------------------------------------------------ */

export declare class Spectrogram extends PraatObject {
  getPowerAt(time: number, frequency: number): number;
  toSound(samplingFrequency?: number): Sound;
  toSpectrumSlice(time: number): Spectrum;
  formula(formulaStr: string): void;
}

/* ------------------------------------------------------------------ */
/*  MFCC                                                               */
/* ------------------------------------------------------------------ */

export declare class MFCC extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;
  readonly fmin: number;
  readonly fmax: number;
  readonly nCoefficients: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getValueInFrame(frame: number, index: number): number;
  getC0ValueInFrame(frame: number): number;
  getNumberOfCoefficients(frame: number): number;
  toSound(): Sound;
  toMatrix(): Matrix;
  toMelSpectrogram(fromCoeff?: number, toCoeff?: number, includeC0?: boolean): PraatObject;
}

/* ------------------------------------------------------------------ */
/*  TextGrid                                                           */
/* ------------------------------------------------------------------ */

export declare class TextGrid extends PraatObject implements FunctionProperties {
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getNumberOfTiers(): number;
  getTierName(tier: number): string;
  isIntervalTier(tier: number): boolean;
  getNumberOfIntervals(tier: number): number;
  getNumberOfPoints(tier: number): number;
  getLabelOfInterval(tier: number, interval: number): string;
  getStartTimeOfInterval(tier: number, interval: number): number;
  getEndTimeOfInterval(tier: number, interval: number): number;
  getIntervalAtTime(tier: number, time: number): number;
  getLabelOfPoint(tier: number, point: number): string;
  getTimeOfPoint(tier: number, point: number): number;
  getLowIntervalAtTime(tier: number, time: number): number;
  getHighIntervalAtTime(tier: number, time: number): number;
  getNearestIndexFromTime(tier: number, time: number): number;
  setIntervalText(tier: number, interval: number, text: string): void;
  setPointText(tier: number, point: number, text: string): void;
  insertBoundary(tier: number, time: number): void;
  removeBoundaryAtTime(tier: number, time: number): void;
  insertPoint(tier: number, time: number, text?: string): void;
  removePoint(tier: number, point: number): void;
  insertIntervalTier(position: number, name: string): void;
  insertPointTier(position: number, name: string): void;
  removeTier(position: number): void;
  extractOneTier(tier: number): TextGrid;
  extractPart(fromTime: number, toTime: number, preserveTimes?: boolean): TextGrid;
}

/* ------------------------------------------------------------------ */
/*  PointProcess                                                       */
/* ------------------------------------------------------------------ */

export declare class PointProcess extends PraatObject implements FunctionProperties {
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getNumberOfPoints(): number;
  getTimeFromIndex(index: number): number;
  getNearestIndex(time: number): number;
  getInterval(time: number): number;
  getNumberOfPeriods(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  getMeanPeriod(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  getJitter(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  getJitterRap(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  getJitterPpq5(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  getJitterDdp(fromTime?: number, toTime?: number, minPeriod?: number,
    maxPeriod?: number, maxPeriodFactor?: number): number;
  addPoint(time: number): void;
  removePoint(index: number): void;
  removePointsBetween(fromTime: number, toTime: number): void;
  toSound(samplingFrequency?: number): Sound;
}

/* ------------------------------------------------------------------ */
/*  LPC                                                                */
/* ------------------------------------------------------------------ */

export declare class LPC extends PraatObject implements SampledProperties {
  readonly dx: number;
  readonly nx: number;
  readonly x1: number;
  readonly timeStep: number;
  readonly nFrames: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly xrange: [number, number];
  readonly startTime: number;
  readonly endTime: number;
  readonly totalDuration: number;
  readonly duration: number;
  readonly centreTime: number;

  getStartTime(): number;
  getEndTime(): number;
  getTotalDuration(): number;
  getTimeStep(): number;
  getNumberOfFrames(): number;
  getTimeFromFrameNumber(frame: number): number;
  getFrameNumberFromTime(time: number): number;
  frameNumberToTime(frame: number): number;
  timeToFrameNumber(time: number): number;
  shiftXBy(shift: number): void;
  shiftXTo(x: number, newX: number): void;
  scaleXBy(scale: number): void;
  scaleXTo(newXmin: number, newXmax: number): void;
  shiftTimesBy(seconds: number): void;
  shiftTimesTo(time: number, newTime: number): void;
  scaleTimesBy(scale: number): void;
  scaleTimesTo(newStart: number, newEnd: number): void;

  getSamplingInterval(): number;
  getNumberOfCoefficients(frame: number): number;
  toFormant(): Formant;
  toSpectrumSlice(time: number, minFreqHz?: number, maxFreqHz?: number, nFreq?: number): Spectrum;
  toMatrix(): Matrix;
}

/* ------------------------------------------------------------------ */
/*  Matrix                                                             */
/* ------------------------------------------------------------------ */

export declare class Matrix extends PraatObject {
  readonly nRows: number;
  readonly nColumns: number;
  readonly xmin: number;
  readonly xmax: number;
  readonly ymin: number;
  readonly ymax: number;
  readonly dx: number;
  readonly dy: number;

  getValueInCell(row: number, column: number): number;
  getValueAtXy(x: number, y: number): number;
  getMinimum(): number;
  getMaximum(): number;
  getSum(): number;
  setValue(row: number, column: number, value: number): void;
  formula(formulaStr: string): void;
  getXOfColumn(col: number): number;
  getYOfRow(row: number): number;
}

/* ------------------------------------------------------------------ */
/*  Table                                                              */
/* ------------------------------------------------------------------ */

export declare class Table extends PraatObject {
  getNumberOfRows(): number;
  getNumberOfColumns(): number;
  getColumnLabel(column: number): string;
  getValue(row: number, column: string | number): string;
  getNumericValue(row: number, column: string | number): number;
  setValue(row: number, column: string | number, value: string | number): void;
  setNumericValue(row: number, column: string | number, value: number): void;
  appendRow(): void;
  appendColumn(label: string): void;
  removeRow(row: number): void;
  removeColumn(column: string | number): void;
  insertRow(row: number): void;
  insertColumn(position: number, label: string): void;
  getMean(column: string | number): number;
  getStdev(column: string | number): number;
  getQuantile(column: string | number, quantile: number): number;
  getMinimum(column: string | number): number;
  getMaximum(column: string | number): number;
  sortRows(column: string | number): void;
}

/* ------------------------------------------------------------------ */
/*  Factory / Instance                                                 */
/* ------------------------------------------------------------------ */

export interface PraatWasmInstance {
  /**
   * Execute a Praat script and return the Info window contents.
   * Equivalent to Parselmouth's `parselmouth.praat.run()`.
   */
  run(scriptText: string): string;

  /**
   * Execute a Praat command, with Parselmouth-compatible signatures.
   *
   * Signatures:
   *   call(object, "Command...", arg1, arg2, ...)
   *   call([obj1, obj2], "Command...", arg1, arg2, ...)
   *   call("Command...", arg1, arg2, ...)
   *
   * @returns New object, parsed number, string, or undefined.
   */
  call(objectOrCommand: PraatObject | PraatObject[] | string, ...args: any[]): any;

  /** List all objects. */
  list(): PraatObject[];

  /** Select an object by its ID. */
  select(id: number): void;

  /** Load audio from an ArrayBuffer (WAV, AIFF, FLAC, MP3, OGG). */
  readAudio(arrayBuffer: ArrayBuffer, filename?: string): Sound | null;

  /** Read a file from the virtual filesystem into the Praat object list. */
  readFile(path: string): PraatObject | null;

  /** Create a new TextGrid. */
  createTextGrid(startTime: number, endTime: number,
    tierNames?: string | string[], pointTierNames?: string | string[]): TextGrid | null;

  /** Save the selected object as a Praat text file. */
  saveAsText(path: string): void;

  /** Save the selected object as a Praat binary file. */
  saveAsBinary(path: string): void;

  /** Read a file from the virtual filesystem as raw bytes. */
  getFile(path: string): Uint8Array;

  /** Write raw bytes to the virtual filesystem. */
  writeFile(path: string, data: Uint8Array): void;

  /** Remove all objects from the Praat object list. */
  removeAll(): void;

  /** Remove the currently selected objects. */
  removeSelected(): void;

  /** Direct access to Emscripten's virtual filesystem. */
  FS: any;

  /** Clean up resources. */
  destroy(): void;
}

/**
 * Create a Praat WASM instance.
 * @param wasmUrl - Optional URL to the praat.wasm binary.
 */
export function createPraatWasm(wasmUrl?: string | URL): Promise<PraatWasmInstance>;
export default createPraatWasm;

export interface PraatWorkerInstance {
  run(scriptText: string): Promise<string>;
  call(objectOrCommand: PraatObject | PraatObject[] | string, ...args: any[]): Promise<any>;
  list(): Promise<PraatObject[]>;
  select(id: number): Promise<void>;
  readAudio(arrayBuffer: ArrayBuffer, filename?: string): Promise<Sound | null>;
  readFile(path: string): Promise<PraatObject | null>;
  createTextGrid(startTime: number, endTime: number,
    tierNames?: string | string[], pointTierNames?: string | string[]): Promise<TextGrid | null>;
  saveAsText(path: string): Promise<void>;
  saveAsBinary(path: string): Promise<void>;
  getFile(path: string): Promise<Uint8Array>;
  writeFile(path: string, data: Uint8Array): Promise<void>;
  removeAll(): Promise<void>;
  removeSelected(): Promise<void>;
  destroy(): void;
}

/**
 * Create a Praat instance running in a Web Worker.
 * @param workerUrl - URL to the worker.mjs script.
 * @param wasmUrl - Optional URL to the praat.wasm binary.
 */
export function createPraatWorker(workerUrl?: string | URL, wasmUrl?: string | URL): Promise<PraatWorkerInstance>;
