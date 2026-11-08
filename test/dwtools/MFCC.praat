# MFCC.praat
# Paul Boersma, 8 November 2026

sound = Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)"

mfcc1 = To MFCC: 12, 0.015, 0.005, 100, 100, 0
mfcc2 = Copy: "mfcc2"
selectObject: mfcc1, mfcc2
cc = Cross-correlate: "peak 0.99", "zero"

removeObject: sound, mfcc1, mfcc2, cc