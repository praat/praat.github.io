modelsFolder$ = preferencesDirectory$ + "/models"
if not folderExists (modelsFolder$)
	exitScript ()
endif
;modelFile$ = modelsFolder$ / "whispercpp" / "ggml-base.bin"
modelFile$ = modelsFolder$ + "/whispercpp/ggml-base.bin"
if not fileReadable (modelFile$)
	;warning: "No file called “ggml-base.bin” in Praat’s settings folder."   ; TODO
	exit
endif
sound = Read from file: "examples/example.wav"
textGrid = To TextGrid: "tier", ""
selectObject: "Sound example", "TextGrid example"
#
# TODO: warnings.
# TODO: exit without error  ; document at exitScript!
#
Transcribe interval: 1, 1, "yes", "no", "yes", 0.5, 0.1, 0.25, 0.03, "ggml-base.bin", "Autodetect language"
selectObject: "TextGrid example"
text$ = Get label of interval: 1, 1
writeInfoLine: text$
assert text$ = "Estimados..."
removeObject: "Sound example", "TextGrid example"
appendInfoLine: "OK"