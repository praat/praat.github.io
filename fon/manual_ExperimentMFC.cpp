/* manual_ExperimentMFC.cpp
 *
 * Copyright (C) 2001-2011,2013,2015-2017,2019-2021,2023-2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ManPagesM.h"

void manual_ExperimentMFC_init (ManPages me);
void manual_ExperimentMFC_init (ManPages me) {

MAN_BEGIN (U"ExperimentMFC", U"ppgb", 20130101)
INTRO (U"One of the @@types of objects@ in Praat, "
	"for running a Multiple Forced Choice listening experiment.")
LIST_ITEM (U"@@ExperimentMFC 1. When to use Praat")
LIST_ITEM (U"@@ExperimentMFC 2. The first example")
LIST_ITEM (U"@@ExperimentMFC 2.1. The experiment file")
LIST_ITEM (U"@@ExperimentMFC 2.2. The stimuli")
LIST_ITEM (U"@@ExperimentMFC 2.3. The carrier phrase")
LIST_ITEM (U"@@ExperimentMFC 2.4. Breaks")
LIST_ITEM (U"@@ExperimentMFC 2.5. Randomization strategies")
LIST_ITEM (U"@@ExperimentMFC 2.6. Instructions")
LIST_ITEM (U"@@ExperimentMFC 2.7. Response categories")
LIST_ITEM (U"@@ExperimentMFC 2.8. Goodness judgments")
LIST_ITEM (U"@@ExperimentMFC 2.9. How an experiment proceeds")
LIST_ITEM (U"@@ExperimentMFC 3. More examples")
LIST_ITEM (U"@@ExperimentMFC 3.1. A simple discrimination experiment")
LIST_ITEM (U"@@ExperimentMFC 3.2. An AXB discrimination experiment")
LIST_ITEM (U"@@ExperimentMFC 3.3. A 4I-oddity experiment")
LIST_ITEM (U"@@ExperimentMFC 3.4. Variable inter-stimulus intervals")
LIST_ITEM (U"@@ExperimentMFC 4. Special buttons")
LIST_ITEM (U"@@ExperimentMFC 4.1. The replay button")
LIST_ITEM (U"@@ExperimentMFC 4.2. The OK button")
LIST_ITEM (U"@@ExperimentMFC 4.3. The oops button")
LIST_ITEM (U"@@ExperimentMFC 5. Stimulus-dependent texts")
LIST_ITEM (U"@@ExperimentMFC 5.1. The stimulus-dependent run text")
LIST_ITEM (U"@@ExperimentMFC 5.2. Stimulus-dependent response buttons")
LIST_ITEM (U"@@ExperimentMFC 6. Responses are sounds")
LIST_ITEM (U"@@ExperimentMFC 7. Blanking the screen")
LIST_ITEM (U"@@ExperimentMFC 8. Running multiple experiments")
MAN_END

MAN_BEGIN (U"ExperimentMFC 1. When to use Praat", U"ppgb", 20160925)  // 2023-06-08
NORMAL (U"With Praat's ExperimentMFC, you can do simple experiments on identification and discrimination. "
	"“Simple” means that for identification, the subject hears a sound and has to click in one of a set of "
	"labelled rectangles (optionally, you can have the subject give a goodness-of-fit judgment). "
	"For discrimination, you can have simple same-different choices, or more intricate things like AXB, 4I-oddity, and so on.")
NORMAL (U"The advantage of using Praat's ExperimentMFC for this is that it is free, it works on Windows, Unix, and Macintosh, "
	"and the whole experiment (experiment file plus sound files) is portable across computers "
	"(you can run it from a USB stick, for instance). Because of the limited possibilities, "
	"it is also quite easy to set up the experiment. Just read the description below.")
NORMAL (U"If you require more from your experiment design, you can use Praat's @@Demo window@; "
	"with that less simple method you could for instance let the stimulus depend on the subject's previous responses. "
	"Alternatively, you could use a dedicated program like Presentation or E-prime instead of Praat; "
	"with these programs, you can also sometimes measure reaction times more accurately.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2. The first example", U"ppgb", 20051205)
INTRO (U"The following pages give an example of an experiment definition, "
	"and explain the main features of an identification task.")
LIST_ITEM (U"@@ExperimentMFC 2.1. The experiment file")
LIST_ITEM (U"@@ExperimentMFC 2.2. The stimuli")
LIST_ITEM (U"@@ExperimentMFC 2.3. The carrier phrase")
LIST_ITEM (U"@@ExperimentMFC 2.4. Breaks")
LIST_ITEM (U"@@ExperimentMFC 2.5. Randomization strategies")
LIST_ITEM (U"@@ExperimentMFC 2.6. Instructions")
LIST_ITEM (U"@@ExperimentMFC 2.7. Response categories")
LIST_ITEM (U"@@ExperimentMFC 2.8. Goodness judgments")
LIST_ITEM (U"@@ExperimentMFC 2.9. How an experiment proceeds")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.1. The experiment file", U"ppgb", 20160925)
INTRO (U"An experiment is defined in a simple text file, which we call an %%experiment file%. "
	"The following is an example of such an experiment file. The first two lines have to be typed "
	"exactly as in this example, the rest depends on your stimuli, on your response categories, "
	"and on the way the experiment is to be presented to the listener. "
	"The order of the elements in this file cannot be changed, and nothing can be skipped. "
	"The first two lines make the file recognizable for Praat as an ExperimentMFC file; "
	"the number 7 is the version number of this type of ExperimentMFC file "
	"(Praat can still read older ExperimentMFC files with a lower version number).")
CODE (U"\"ooTextFile\"")
CODE (U"\"ExperimentMFC 7\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <yes>")
CODE (U"stimulusFileNameHead = \"Sounds/\"")
CODE (U"stimulusFileNameTail = \".wav\"")
CODE (U"stimulusCarrierBefore = \"weSayTheWord\"")
CODE (U"stimulusCarrierAfter = \"again\"")
CODE (U"stimulusInitialSilenceDuration = 0.5 seconds")
CODE (U"stimulusMedialSilenceDuration = 0")
CODE (U"stimulusFinalSilenceDuration = 0.5 seconds")
CODE (U"numberOfDifferentStimuli = 4")
	CODE1 (U"\"heed\"  \"\"")
	CODE1 (U"\"hid\"   \"\"")
	CODE1 (U"\"hood\"  \"\"")
	CODE1 (U"\"hud\"   \"\"")
CODE (U"numberOfReplicationsPerStimulus = 3")
CODE (U"breakAfterEvery = 0")
CODE (U"randomize = <PermuteBalancedNoDoublets>")
CODE (U"startText = \"This is a listening experiment.")
CODE (U"After hearing a sound, choose the vowel that is most similar to what you heard.")
CODE (U"")
CODE (U"Click to start.\"")
CODE (U"runText = \"Choose the vowel that you heard.\"")
CODE (U"pauseText = \"You can have a short break if you like. Click to proceed.\"")
CODE (U"endText = \"The experiment has finished.\"")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
CODE (U"okButton = 0 0 0 0 \"\" \"\"")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
CODE (U"responsesAreSounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"numberOfDifferentResponses = 5")
	CODE1 (U"0.2 0.3 0.7 0.8 \"h I d\" 40 \"\" \"i\"")
	CODE1 (U"0.3 0.4 0.5 0.6 \"h E d\" 40 \"\" \"e\"")
	CODE1 (U"0.4 0.5 0.3 0.4 \"h A d\" 40 \"\" \"a\"")
	CODE1 (U"0.5 0.6 0.5 0.6 \"h O d\" 40 \"\" \"o\"")
	CODE1 (U"0.6 0.7 0.7 0.8 \"h U d\" 40 \"\" \"u\"")
CODE (U"numberOfGoodnessCategories = 5")
	CODE1 (U"0.25 0.35 0.10 0.20 \"1 (poor)\" 24 \"\"")
	CODE1 (U"0.35 0.45 0.10 0.20 \"2\" 24 \"\"")
	CODE1 (U"0.45 0.55 0.10 0.20 \"3\" 24 \"\"")
	CODE1 (U"0.55 0.65 0.10 0.20 \"4\" 24 \"\"")
	CODE1 (U"0.65 0.75 0.10 0.20 \"5 (good)\" 24 \"\"")
NORMAL (U"This experiment will play 4 different stimuli to the listener, each 3 times. "
	"Thus, the listener is confronted with 12 trials.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.2. The stimuli", U"ppgb", 20201229)
INTRO (U"You can see that the @@ExperimentMFC 2.1. The experiment file|example experiment@ "
	"contains four different stimuli: %heed, %hid, %hood, and %hud. "
	"These are the %names of the four stimuli.")
NORMAL (U"Because in the example experiment stimuli are sounds, "
	"each of the four stimuli should be in a sound file. "
	"The names of these sound files must be identical to the names "
	"of the stimuli, bracketed with %stimulusFileNamehead and %stimulusFileNameTail. "
	"Hence, the stimuli are expected in the following four files:")
LIST_ITEM (U"Sounds/heed.wav")
LIST_ITEM (U"Sounds/hid.wav")
LIST_ITEM (U"Sounds/hood.wav")
LIST_ITEM (U"Sounds/hud.wav")
NORMAL (U"You need not use WAV files. You can also use AIFF files, "
	"in which case $%stimulusFileNameTail would probably be “.aiff”, or any other "
	"type of sound file that Praat supports. But all sound files must have the same number of channels "
	"(i.e. all mono or all stereo) and the same sampling frequency.")
NORMAL (U"In this example, the experiment will look for the sound files in the folder #Sounds, "
	"which has to be in the same folder as your experiment file. "
	"In other words, “Sounds/heed.wav” is a %%relative file path%.")
NORMAL (U"Instead of a relative path, you can also supply a %%full file path%. "
	"Such a path depends on your computer and on your operating system. "
	"For instance, if you have a Windows computer and the stimuli are in the folder ##D:\\bsCorpus\\bsAutumn\\bsSpeaker23#, "
	"you can write")
CODE (U"fileNameHead = \"D:\\Corpus\\Autumn\\Speaker23\\\"")
NORMAL (U"If you have a Macintosh (OS X) or Unix computer and the stimuli are in ##/Users/mietta/Sounds/Dutch#, you write")
CODE (U"fileNameHead = \"/Users/mietta/Sounds/Dutch/\"")
NORMAL (U"But relative file paths will usually be preferred: they are more %portable. "
	"The advantage of using relative file paths is that you can move your whole experiment (experiment file plus sounds) "
	"from one computer to another without changing the experiment file, "
	"as long as you put the experiment file in the same folder as where you put the folder #Sounds. "
	"Or you can put the whole experiment on a USB stick and run the experiment directly from the stick. "
	"Since Praat supports the forward slash “/” as a folder separator on all computers, "
	"you can run the exact same experiment on Macintosh, Windows and Unix computers, "
	"independent of the type of computer where you have created your experiment.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.3. The carrier phrase", U"ppgb", 20201229)
NORMAL (U"The stimuli can be embedded in a %%carrier phrase%. "
	"In the @@ExperimentMFC 2.1. The experiment file|example experiment@, we see that the stimulus "
	"will be inserted between the sounds in the files ##weSayTheWord.wav# "
	"and ##again.wav#, both of which are expected to be in the folder #Sounds. "
	"If you do not want a carrier phrase, you do")
CODE (U"stimulusCarrierBefore = \"\"")
CODE (U"stimulusCarrierAfter = \"\"")
NORMAL (U"If you want only an introductory phrase before the stimulus, and nothing after the stimulus, "
	"you do something like")
CODE (U"stimulusCarrierBefore = \"listenTo\"")
CODE (U"stimulusCarrierAfter = \"\"")
NORMAL (U"and of course you supply the file ##listenTo.wav# in the folder #Sounds.")
NORMAL (U"If you want to have a short silence before every stimulus (and before the carrier phrase), "
	"you supply a non-zero %stimulusInitialSilenceDuration, as in the example.")
NORMAL (U"Since the carrier phrase is concatenated with the stimulus before it is played, it should have the same "
	"sampling frequency as the stimulus files.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.4. Breaks", U"ppgb", 20051205)
NORMAL (U"A new stimulus will arrive as soon as the listener makes her choice. To allow her some breathing "
	"time, you can insert a break after every so many trials. In the example, %breakAfterEvery is 0, "
	"because there are only 12 trials. A typical experiment has perhaps 180 trials, and you may want to "
	"insert a break after every 40 trials. In that case, you do")
CODE (U"breakAfterEvery = 40")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.5. Randomization strategies", U"ppgb", 20051205)
NORMAL (U"The 3 times 4 stimuli will have to be played in a certain order. For the least random order, you say")
CODE (U"randomize = <CyclicNonRandom>")
NORMAL (U"In this case, the stimuli will be played in the order in which they were specified in the file, 3 times:")
EQUATION (U"heed hid hood hud heed hid hood hud heed hid hood hud")
NORMAL (U"The most likely case in which you would want to use this randomization strategy, is if you have, say, 120 "
	"different stimuli and you want to play them only once (%numberOfReplicationsPerStimulus = 1) in a fixed order.")
NORMAL (U"The other extreme, the most random order, is")
CODE (U"randomize = <WithReplacement>")
NORMAL (U"In this case, a stimulus will be chosen at random 12 times without memory, for instance")
EQUATION (U"hid hood hood heed hid hood hud hud hid hood heed hid")
NORMAL (U"The order will probably be different for each listener. "
	"In this example, %hood and %hid occur four times each, %heed and %hud only twice each. "
	"This strategy is too random for most experiments. Usually, you will want to have the same number "
	"of replications of each stimulus. The most random way to do this is")
CODE (U"randomize = <PermuteAll>")
NORMAL (U"In this case, all stimuli will be played exactly 3 times, for instance")
EQUATION (U"heed hood hud hud hid heed heed hud hood hid hid hood")
NORMAL (U"Quite often, you will want a less random order, namely one in which the 12 trials are divided into "
	"3 blocks of 4 stimuli. Within each block, all 4 different stimuli occur in a random order:")
CODE (U"randomize = <PermuteBalanced>")
NORMAL (U"In this case, each stimulus occurs exactly once within each block:")
EQUATION (U"heed hood hud hid hood hud hid heed heed hud hood hid")
NORMAL (U"This strategy ensures a certain spreading of the stimuli over the sequence of 12 trials. "
	"As we see here, it is still possible that the same stimulus (%heed) occurs twice in a row, "
	"namely as the last stimulus of the second block and the first stimulus of the third. "
	"If you want to prevent that situation, you use")
CODE (U"randomize = <PermuteBalancedNoDoublets>")
NORMAL (U"This will ensure that the same stimulus is never applied twice in a row:")
EQUATION (U"heed hood hud hid hood hud hid heed hud heed hood hid")
NORMAL (U"This randomization strategy is used in our example, and advised for most listening experiments "
	"in which you want to minimize effects of stimulus order.")
NORMAL (U"The randomization procedure does not interfere in any way with the breaks. The order is determined "
	"before any breaks are inserted.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.6. Instructions", U"ppgb", 20081123)
NORMAL (U"Before the experiment begins, the listener will see the %startText in the centre of the screen. "
	"During each trial, she will see the %runText at the top of the screen. "
	"During breaks, she will see the %pauseText in the centre of the screen. "
	"After all the trials have been performed, she will see the %endText. "
	"As you can see in the example, all these texts can consist of multiple lines.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.7. Response categories", U"ppgb", 20201229)
NORMAL (U"Every trial comes with the same set of response categories. "
	"The @@ExperimentMFC 2.1. The experiment file|example experiment@ has five of them. "
	"For each response category, you supply the area of the screen where a rectangle will be drawn. "
	"The whole screen measures from 0.0 (left) to 1.0 (right) and from 0.0 (bottom) to 1.0 (top). "
	"Thus, “0.2 0.3 0.7 0.8” means that a rectangle will be drawn somewhere in the top left quadrant "
	"of the screen. You also supply the text that will be drawn in this rectangle, for instance the text “h I d”. "
	"After this you supply the font size for this text, for instance 40.")
NORMAL (U"The second text that you supply for every response is a response key on the keyboard. "
	"In the above example this is “”, i.e. the subject cannot press a key as a response. "
	"If you want the user to be able to press the “m” key instead of clicking in the “h I d” rectangle, "
	"the line in the experiment file would be:")
CODE1 (U"0.2 0.3 0.7 0.8 \"h I d\" 40 \"m\" \"i\"")
NORMAL (U"The third text that you supply for each rectangle is the response category as it will be reported by Praat to you when the user clicks it, "
	"e.g. the text “i”. If you want Praat to ignore mouse clicks on this rectangle, specify an empty response "
	"category, i.e. \"\".")
NORMAL (U"The border of the rectangles will be maroon, the background of the screen will be light grey. "
	"The colour of clickable rectangles will be yellow, that of non-clickable rectangles (those with "
	"empty category specifications) light grey.")
NORMAL (U"You can have a picture instead of a text on a response button, by using \\bsFI:")
CODE1 (U"0.2 0.3 0.7 0.8 \"\\FIpictures/hello.jpg\" 40 \"m\" \"i\"")
NORMAL (U"In this example, the picture ##hello.jpg# from the subfolder #pictures "
	"(i.e. a subfolder of the folder where your experiment file is) "
	"will be drawn into the rectangle [0.2, 0.3] \\xx [0.7, 0.8]. "
	"This currently (September 2011) works only on the Mac and Windows.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.8. Goodness judgments", U"ppgb", 20160925)
NORMAL (U"If %numberOfGoodnessCategories is not 0, some more rectangles will be drawn, "
	"as in the @@ExperimentMFC 2.1. The experiment file|example experiment@. "
	"You specify again the locations of these rectangles (in the example, they touch each other), "
	"the texts on them, the font sizes, and the response keys. "
	"Praat will record the number of the button when the listener clicks on it. "
	"Thus, if she clicks on the button labelled \"1 (poor)\", Praat will record a goodness judgment of 1, "
	"because this is the first button in the list. If she clicks on \"5 (good)\", Praat will record a "
	"goodness judgment of 5.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 2.9. How an experiment proceeds", U"ppgb", 20110317)
NORMAL (U"A text file with an ExperimentMFC object can be read into Praat with @@Read from file...@ "
	"(it is not a script but a data file, so do not try to read it with ##Open Praat script...#). "
	"You can then choose #Run. After the experiment finishes, you can close the experiment window "
	"and choose ##Extract results#. The resulting ResultsMFC object contains for each trial the stimulus "
	"name (e.g. “hood”), the response category (e.g. “u”), and the goodness judgment (e.g. 4). "
	"You will want to save this ResultsMFC object to a text file with @@Save as text file...@. "
	"You may want to call these text files by the names of the subjects, e.g. ##ts.ResultsMFC# "
	"and ##mj.ResultsMFC#. Once you have collected the results of all your subjects, you can read "
	"all the results files into Praat with @@Read from file...@, then select all the resulting "
	"ResultsMFC objects (which will have automatically been named #ts, #mj, and so on), then choose "
	"##Collect to table#. This will result in a table whose first column contains the names of the subjects, "
	"the second column contains the stimulus names, the third column contains the responses, "
	"and the last column contains the approximate reaction times (measured from the start of the stimulus sound, i.e. after the initial silence duration). "
	"If there are goodness judgments, these will go into the fourth column. The table can be saved "
	"as a table file (with ##Save as tab-separated file...#), which can be read by programs like Excel and SPSS.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 3. More examples", U"ppgb", 20051205)
INTRO (U"You can do many more kinds of experiments than simple identification experiments.")
LIST_ITEM (U"@@ExperimentMFC 3.1. A simple discrimination experiment")
LIST_ITEM (U"@@ExperimentMFC 3.2. An AXB discrimination experiment")
LIST_ITEM (U"@@ExperimentMFC 3.3. A 4I-oddity experiment")
LIST_ITEM (U"@@ExperimentMFC 3.4. Variable inter-stimulus intervals")
MAN_END

MAN_BEGIN (U"ExperimentMFC 3.1. A simple discrimination experiment", U"ppgb", 20160925)
NORMAL (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ was an %identification experiment: "
	"the subject had identify a single sound as one element of a set of categories. "
	"Phoneticians will often do %discrimination experiments, which are experiments in which "
	"a stimulus consists of multiple sub-stimuli played in sequence, and the subject has to judge the similarity "
	"between these sub-stimuli.")
NORMAL (U"The simplest discrimination task has only two sub-stimuli, and the subject has to say whether these are "
	"the %same or %different. Suppose you have vowel-like sounds along an F1 continuum with seven steps, "
	"say 300, 320, 340, 360, 380, 400, and 420 hertz, and you are interested in knowing how well the listeners "
	"can distinguish these. As your stimuli, you create pairs of these sounds, separated by 0.8 seconds of silence. "
	"It is important to include stimuli in which the sounds are identical, e.g. stimuli in which both sounds have an F1 "
	"of 340 Hz (see the literature on signal detection theory). Since sounds that are very different acoustically "
	"will always be heard as different, you do not include pairs in which the distance is larger than 60 Hz. "
	"The experiment file will look like this:")
CODE (U"\"ooTextFile\"")
CODE (U"\"ExperimentMFC 7\"")
CODE (U"blank while playing? <no>")
CODE (U"stimuli are sounds? <yes>")
CODE (U"\"stimuli/\"  \".wav\"")
CODE (U"carrier phrase \"\"  \"\"")
CODE (U"initial silence duration 0.5 seconds")
CODE (U"medial silence duration 0.8 seconds  ! inter-stimulus interval")
CODE (U"final silence duration 0 seconds")
CODE (U"37 different stimuli")
CODE1 (U"\"300,300\"  \"\"  \"300,320\"  \"\"  \"300,340\"  \"\"  \"300,360\"  \"\"")
CODE1 (U"\"320,300\"  \"\"  \"320,320\"  \"\"  \"320,340\"  \"\"  \"320,360\"  \"\"  \"320,380\"  \"\"")
CODE1 (U"\"340,300\"  \"\"  \"340,320\"  \"\"  \"340,340\"  \"\"  \"340,360\"  \"\"  \"340,380\"  \"\"  \"340,400\"  \"\"")
CODE1 (U"\"360,300\"  \"\"  \"360,320\"  \"\"  \"360,340\"  \"\"  \"360,360\"  \"\"  \"360,380\"  \"\"  \"360,400\"  \"\"  \"360,420\"  \"\"")
CODE1 (U"\"380,320\"  \"\"  \"380,340\"  \"\"  \"380,360\"  \"\"  \"380,380\"  \"\"  \"380,400\"  \"\"  \"380,420\"  \"\"")
CODE1 (U"\"400,340\"  \"\"  \"400,360\"  \"\"  \"400,380\"  \"\"  \"400,400\"  \"\"  \"400,420\"  \"\"")
CODE1 (U"\"420,360\"  \"\"  \"420,380\"  \"\"  \"420,400\"  \"\"  \"420,420\"  \"\"")
CODE (U"10 replications per stimulus")
CODE (U"break after every 50 stimuli")
CODE (U"<PermuteBalancedNoDoublets>")
CODE (U"\"Click to start.\"")
CODE (U"\"Say whether these sounds were the same or different.\"")
CODE (U"\"You can have a short break if you like. Click to proceed.\"")
CODE (U"\"The experiment has finished. Call the experimenter.\"")
CODE (U"0 replays")
CODE (U"replay button 0 0 0 0 \"\" \"\"")
CODE (U"ok button 0 0 0 0 \"\" \"\"")
CODE (U"oops button 0 0 0 0 \"\" \"\"")
CODE (U"responses are sounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"2 response categories")
CODE1 (U"0.1 0.4 0.35 0.65 \"same\" 24 \"\" \"same\"")
CODE1 (U"0.6 0.9 0.35 0.65 \"different\" 24 \"\" \"different\"")
CODE (U"0 goodness categories")
NORMAL (U"In this example, the subject will have to click 370 times. After every 50 times, she will have the "
	"opportunity to sip her tea. A 0.5-seconds silence is played before every stimulus, so that the listener "
	"will not hear the stimulus immediately after her mouse click.")
NORMAL (U"The experimenter does not have to create the stimulus pairs as sound files. "
	"You can specify multiple sound files by separating them with commas. Thus, \"320,300\" means that "
	"Praat will play the files ##320.wav# and ##300.wav#. These two substimuli will be separated here by a silence "
	"of 0.8 seconds, called the %%inter-stimulus interval% (or %stimulusMedialSilenceDuration).")
NORMAL (U"Note that the text in this file is rather different from the previous example. "
	"It does not matter whether you write %`numberOfDifferentStimuli`, or %`different stimuli`, or anything else; "
	"Praat ignores these texts as long as they do not contain numbers, quoted strings, or things between <>.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 3.2. An AXB discrimination experiment", U"ppgb", 20160925)
INTRO (U"In the AXB task, the subject will hear three stimuli in sequence, and has to say "
	"whether the second (X) is more similar to the first (A) or to the second (B). "
	"An experiment file could look like follows:")
CODE (U"\"ooTextFile\"")
CODE (U"\"ExperimentMFC 7\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <yes>")
CODE (U"\"stimuli/\"  \".wav\"")
CODE (U"carrier \"\"  \"\"")
CODE (U"initial silence 0.5")
CODE (U"inter-stimulus interval 0.3")
CODE (U"final silence 0")
CODE (U"100 stimuli")
	CODE1 (U"\"300,300,320\"  \"\"  \"300,320,340\"  \"\"  \"300,340,340\"  \"\"  \"300,340,360\"  \"\"")
	CODE1 (U"...")
	CODE1 (U"(and 96 more triplets of substimuli)")
	CODE1 (U"...")
CODE (U"4 replications")
CODE (U"break every 50")
CODE (U"<PermuteBalancedNoDoublets>")
CODE (U"\"Click to start.\"")
CODE (U"\"Say whether the second sound is more similar to the first or to the third.\"")
CODE (U"\"You can have a short break if you like. Click to proceed.\"")
CODE (U"\"The experiment has finished.\"")
CODE (U"0 replays")
CODE (U"replay button 0 0 0 0 \"\" \"\"")
CODE (U"ok button 0 0 0 0 \"\" \"\"")
CODE (U"oops button 0 0 0 0 \"\" \"\"")
CODE (U"responses are sounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"3 response categories")
	CODE1 (U"0.1 0.3 0.4 0.6 \"first\" 30 \"\" \"A\"")
	CODE1 (U"0.4 0.6 0.4 0.6 \"second\" 30 \"\" \"\"")
	CODE1 (U"0.7 0.9 0.4 0.6 \"third\" 30 \"\" \"B\"")
CODE (U"0 goodness categories")
NORMAL (U"In this example, the subject has to click 400 times. She sees three buttons, "
	"labelled %first, %second, and %third, but the second button (the one with the empty response category) "
	"is not clickable: it has a light grey rather than a yellow interior and cannot be chosen by the subject. "
	"In your ResultsMFC object, you will only see %A and %B responses.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 3.3. A 4I-oddity experiment", U"ppgb", 20160925)
NORMAL (U"In the four-items-oddity task, the subject will hear four stimuli in sequence, and has to say "
	"whether the second or the third is the odd one out. The other three substimuli are identical. "
	"An experiment file could look as follows:")
CODE (U"\"ooTextFile\"")
CODE (U"\"ExperimentMFC 7\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <yes>")
CODE (U"stimulusFileNameHead = \"stimuli/\"")
CODE (U"stimulusFileNameTail = \".wav\"")
CODE (U"stimulusCarrierBefore = \"\"")
CODE (U"stimulusCarrierAfter = \"\"")
CODE (U"stimulusInitialSilenceDuration = 0.5 seconds")
CODE (U"stimulusMedialSilenceDuration = 0.3 seconds")
CODE (U"numberOfDifferentStimuli = 60")
	CODE1 (U"\"300,300,320,300\"  \"\"  \"300,320,300,300\"  \"\"")
	CODE1 (U"\"300,300,340,300\"  \"\"  \"300,340,300,300\"  \"\"")
	CODE1 (U"\"300,300,360,300\"  \"\"  \"300,360,300,300\"  \"\"")
	CODE1 (U"\"320,320,300,320\"  \"\"  \"320,300,320,320\"  \"\"")
	CODE1 (U"\"320,320,340,320\"  \"\"  \"320,340,320,320\"  \"\"")
	CODE1 (U"\"320,320,360,320\"  \"\"  \"320,360,320,320\"  \"\"")
	CODE1 (U"\"320,320,380,320\"  \"\"  \"320,380,320,320\"  \"\"")
	CODE1 (U"\"340,340,300,340\"  \"\"  \"340,300,340,340\"  \"\"")
	CODE1 (U"\"340,340,320,340\"  \"\"  \"340,320,340,340\"  \"\"")
	CODE1 (U"\"340,340,360,340\"  \"\"  \"340,360,340,340\"  \"\"")
	CODE1 (U"\"340,340,380,340\"  \"\"  \"340,380,340,340\"  \"\"")
	CODE1 (U"\"340,340,400,340\"  \"\"  \"340,400,340,340\"  \"\"")
	CODE1 (U"\"360,360,300,360\"  \"\"  \"360,300,360,360\"  \"\"")
	CODE1 (U"\"360,360,320,360\"  \"\"  \"360,320,360,360\"  \"\"")
	CODE1 (U"\"360,360,340,360\"  \"\"  \"360,340,360,360\"  \"\"")
	CODE1 (U"\"360,360,380,360\"  \"\"  \"360,380,360,360\"  \"\"")
	CODE1 (U"\"360,360,400,360\"  \"\"  \"360,400,360,360\"  \"\"")
	CODE1 (U"\"360,360,420,360\"  \"\"  \"360,420,360,360\"  \"\"")
	CODE1 (U"\"380,380,320,380\"  \"\"  \"380,320,380,380\"  \"\"")
	CODE1 (U"\"380,380,340,380\"  \"\"  \"380,340,380,380\"  \"\"")
	CODE1 (U"\"380,380,360,380\"  \"\"  \"380,360,380,380\"  \"\"")
	CODE1 (U"\"380,380,400,380\"  \"\"  \"380,400,380,380\"  \"\"")
	CODE1 (U"\"380,380,420,380\"  \"\"  \"380,420,380,380\"  \"\"")
	CODE1 (U"\"400,400,340,400\"  \"\"  \"400,340,400,400\"  \"\"")
	CODE1 (U"\"400,400,360,400\"  \"\"  \"400,360,400,400\"  \"\"")
	CODE1 (U"\"400,400,380,400\"  \"\"  \"400,380,400,400\"  \"\"")
	CODE1 (U"\"400,400,420,400\"  \"\"  \"400,420,400,400\"  \"\"")
	CODE1 (U"\"420,420,360,420\"  \"\"  \"420,360,420,420\"  \"\"")
	CODE1 (U"\"420,420,380,420\"  \"\"  \"420,380,420,420\"  \"\"")
	CODE1 (U"\"420,420,400,420\"  \"\"  \"420,400,420,420\"  \"\"")
CODE (U"numberOfReplicationsPerStimulus = 5")
CODE (U"breakAfterEvery = 40")
CODE (U"randomize = <PermuteBalancedNoDoublets>")
CODE (U"startText = \"Click to start.\"")
CODE (U"runText = \"Say whether the second or the third sound is different from the rest.\"")
CODE (U"pauseText = \"You can have a short break if you like. Click to proceed.\"")
CODE (U"endText = \"The experiment has finished.\"")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
CODE (U"okButton = 0 0 0 0 \"\" \"\"")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
CODE (U"responsesAreSounds? <no>")
CODE (U"responseFileNameHead = \"\"")
CODE (U"responseFileNameTail = \"\"")
CODE (U"responseCarrierBefore = \"\"")
CODE (U"responseCarrierAfter = \"\"")
CODE (U"responseInitialSilenceDuration = 0")
CODE (U"responseMedialSilenceDuration = 0")
CODE (U"responseFinalSilenceDuration = 0")
CODE (U"numberOfResponseCategories = 4")
	CODE1 (U"0.04 0.24 0.4 0.6 \"first\" 30 \"\" \"\"")
	CODE1 (U"0.28 0.48 0.4 0.6 \"second\" 30 \"\" \"2\"")
	CODE1 (U"0.52 0.72 0.4 0.6 \"third\" 30 \"\" \"3\"")
	CODE1 (U"0.76 0.96 0.4 0.6 \"fourth\" 30 \"\" \"\"")
CODE (U"numberOfGoodnessCategories = 0")
NORMAL (U"In this example, the subject has to click 300 times. She sees four buttons, "
	"but the first and fourth buttons cannot be chosen. "
	"In your ResultsMFC object, you will only see the responses %2 and %3.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 3.4. Variable inter-stimulus intervals", U"ppgb", 20130101)
NORMAL (U"Praat only supports a fixed inter-stimulus interval, but sometimes you may want to test "
	"discrimination as a function of the inter-stimulus interval itself. You can achieve this by "
	"supplying an %stimulusMedialSilenceDuration of 0 and using sound files with various silences:")
CODE1 (U"\"300,silence0.5,320\"  \"300,silence1.5,320\"  \"300,silence4.5,320\"")
NORMAL (U"In this example, you have to supply the sound files ##silence0.5.wav# and so on. You can "
	"create them with the help of @@Create Sound from formula...@ (supply a %formula of 0).")
MAN_END

MAN_BEGIN (U"ExperimentMFC 4. Special buttons", U"ppgb", 20051205)
INTRO (U"You can include up to three special buttons on the screen that the participant sees. "
	"It is probably inadvisable to use all three at the same time.")
LIST_ITEM (U"@@ExperimentMFC 4.1. The replay button")
LIST_ITEM (U"@@ExperimentMFC 4.2. The OK button")
LIST_ITEM (U"@@ExperimentMFC 4.3. The oops button")
MAN_END

MAN_BEGIN (U"ExperimentMFC 4.1. The replay button", U"ppgb", 20051205)
INTRO (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ contained the following lines:")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
NORMAL (U"This means that that experiment did not have a #replay button. "
	"To add a replay button along the lower edge of the screen, you do something like")
CODE (U"maximumNumberOfReplays = 1000")
CODE (U"replayButton = 0.3 0.7 0.01 0.07 \"Click here to play the last sound again\" \"\"")
NORMAL (U"If you supply a right edge (here 0.7) that is greater than the left edge (here 0.3), "
	"Praat will know that you want to show a #replay button.")
NORMAL (U"When the participant clicks this button, Praat will play the current stimulus again. "
	"In this example, the button will be visible until the partipant has clicked it 1000 times.")
NORMAL (U"To assign a keyboard shortcut to the #replay button, do something like")
CODE (U"maximumNumberOfReplays = 1000")
CODE (U"replayButton = 0.1 0.9 0.01 0.07 \"Click here or press the space bar to play the last sound again\" \" \"")
MAN_END

MAN_BEGIN (U"ExperimentMFC 4.2. The OK button", U"ppgb", 20051205)
INTRO (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ contained the following lines:")
CODE (U"okButton = 0 0 0 0 \"\" \"\"")
NORMAL (U"This means that that experiment did not have an #OK button. "
	"To add an OK button in the lower right corner of the screen, you do something like")
CODE (U"okButton = 0.8 0.95 0.05 0.15 \"OK\" \"\"")
NORMAL (U"If you supply a right edge (here 0.95) that is greater than the left edge (here 0.8), "
	"Praat will know that you want to show an #OK button.")
NORMAL (U"The behaviour of the experiment changes appreciably if you include an #OK button. "
	"If you do not include an #OK button, Praat will present the next stimulus as soon as the participant "
	"has clicked a response category (and a goodness category, if there are such). "
	"If you do include an #OK button, it will become visible to the participant as soon as she has chosen "
	"a response category (and a goodness category, if there are such). "
	"The participant can then click the #OK button, but she can also choose to click the response "
	"(and goodness buttons) a bit more first.")
NORMAL (U"The OK button seems to be useful only if there is also a replay button, "
	"or if the response categories are sounds (see @@ExperimentMFC 6. Responses are sounds@).")
NORMAL (U"To assign a keyboard shortcut (here, the space bar) to the OK button, do something like")
CODE (U"okButton = 0.8 0.95 0.05 0.15 \"OK\" \" \"")
MAN_END

MAN_BEGIN (U"ExperimentMFC 4.3. The oops button", U"ppgb", 20051205)
INTRO (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ contained the following lines:")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
NORMAL (U"This means that that experiment did not have an #oops button. "
	"To add an #oops button in the lower left corner of the screen, you do something like")
CODE (U"oopsButton = 0.05 0.2 0.05 0.15 \"oops\" \"\"")
NORMAL (U"If you supply a right edge (here 0.2) that is greater than the left edge (here 0.05), "
	"Praat will know that you want to show an #oops button.")
NORMAL (U"If you include an #oops button, it will become visible to the participant for every stimulus except the first, "
	"and it will also be visible on the pause (break) screens and on the final screen.")
NORMAL (U"If the participant clicks the #oops button, Praat will forget everything the participant did "
	"with the current stimulus and the previous stimulus. The experiment will continue with playing "
	"the previous stimulus again and waiting for the participant’s choice.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 5. Stimulus-dependent texts", U"ppgb", 20051205)
INTRO (U"In the @@ExperimentMFC 2.1. The experiment file|example experiment@, the text at the top of the screen "
	"stayed the same throughout the experiment, and so did the texts on the response buttons. "
	"There are ways to have these texts depend on the stimulus at hand.")
LIST_ITEM (U"@@ExperimentMFC 5.1. The stimulus-dependent run text")
LIST_ITEM (U"@@ExperimentMFC 5.2. Stimulus-dependent response buttons")
MAN_END

MAN_BEGIN (U"ExperimentMFC 5.1. The stimulus-dependent run text", U"ppgb", 20051205)
INTRO (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ contained the following lines:")
CODE (U"numberOfDifferentStimuli = 4")
	CODE1 (U"\"heed\"  \"\"")
	CODE1 (U"\"hid\"   \"\"")
	CODE1 (U"\"hood\"  \"\"")
	CODE1 (U"\"hud\"   \"\"")
CODE (U"...")
CODE (U"...")
CODE (U"runText = \"Choose the vowel that you heard.\"")
NORMAL (U"For every stimulus, the same “run text” was written at the top of the screen. "
	"But suppose you want to make that text dependent on the stimulus. You would do:")
	CODE1 (U"\"heed\"  \"Choose the vowel you heard.\"")
	CODE1 (U"\"hid\"   \"Click the vowel you heard.\"")
	CODE1 (U"\"hood\"  \"Select the vowel you heard.\"")
	CODE1 (U"\"hud\"   \"What's the vowel you heard?\"")
CODE (U"...")
CODE (U"...")
CODE (U"runText = \"\"")
NORMAL (U"In this case, each stimulus comes with its own text. The %runText will only show up for stimuli "
	"for which you do not supply a separate text.")
NORMAL (U"This feature is useful mainly in cases where the responses are sounds but the stimulus is not "
	"(see @@ExperimentMFC 6. Responses are sounds@) or if you want to cause some lexical priming.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 5.2. Stimulus-dependent response buttons", U"ppgb", 20070926)
INTRO (U"The @@ExperimentMFC 2.1. The experiment file|example experiment@ contained the following lines:")
CODE (U"numberOfDifferentStimuli = 4")
	CODE1 (U"\"heed\"  \"\"")
	CODE1 (U"\"hid\"   \"\"")
	CODE1 (U"\"hood\"  \"\"")
	CODE1 (U"\"hud\"   \"\"")
CODE (U"...")
CODE (U"...")
CODE (U"numberOfDifferentResponses = 5")
	CODE1 (U"0.2 0.3 0.7 0.8 \"h I d\" 40 \"\" \"i\"")
	CODE1 (U"0.3 0.4 0.5 0.6 \"h E d\" 40 \"\" \"e\"")
	CODE1 (U"0.4 0.5 0.3 0.4 \"h A d\" 40 \"\" \"a\"")
	CODE1 (U"0.5 0.6 0.5 0.6 \"h O d\" 40 \"\" \"o\"")
	CODE1 (U"0.6 0.7 0.7 0.8 \"h U d\" 40 \"\" \"u\"")
NORMAL (U"For every stimulus, the buttons contained the same visible texts, such as \"h I d\" and \"h A d\".")
NORMAL (U"But suppose you have an experiment about the perception of voicing in plosives. "
	"The first stimulus starts with an ambiguous /b/ or /p/, and you want the participant "
	"to choose between \"bath\" and \"path\". The next stimulus starts with an ambiguous /d/ or /t/, "
	"and you want the participant to choose between \"dart\" and \"tart\". "
	"You would go about like this:")
CODE (U"numberOfDifferentStimuli = 6")
	CODE1 (U"\"bpath1\"  \"|bath|path\"")
	CODE1 (U"\"bpath2\"  \"|bath|path\"")
	CODE1 (U"\"bpath3\"  \"|bath|path\"")
	CODE1 (U"\"dtart1\"  \"|dart|tart\"")
	CODE1 (U"\"dtart2\"  \"|dart|tart\"")
	CODE1 (U"\"dtart3\"  \"|dart|tart\"")
CODE (U"...")
CODE (U"...")
CODE (U"numberOfDifferentResponses = 2")
	CODE1 (U"0.2 0.4 0.7 0.8 \"\" 40 \"\" \"left\"")
	CODE1 (U"0.6 0.8 0.7 0.8 \"\" 40 \"\" \"right\"")
NORMAL (U"In this case, the two response buttons show either \"path\" and \"path\", or \"dart\" and \"tart\".")
NORMAL (U"In the ResultsMFC (see @@ExperimentMFC 2.9. How an experiment proceeds@), "
	"the stimuli will be recorded as “bpath1|bath|path” and the like, not just as “bpath1”. "
	"Praat does this in order to be able to cope with balanced designs such as")
CODE1 (U"\"bpath1\"  \"|bath|path\"")
CODE1 (U"\"bpath1\"  \"|path|bath\"")
NORMAL (U"In other words, the button ordering is considered part of the stimulus.")
NORMAL (U"This trick can be combined with a stimulus-dependent run text:")
CODE (U"numberOfDifferentStimuli = 32")
	CODE1 (U"\"bpath1\"  \"Throw a...|bath|path\"")
	CODE1 (U"\"bpath1\"  \"Walk a...|bath|path\"")
	CODE1 (U"\"bpath2\"  \"Walk a...|bath|path\"")
	CODE1 (U"\"dtart1\"  \"Throw a...|dart|tart\"")
	CODE1 (U"\"dtart1\"  \"Carry a...|dart|tart\"")
CODE (U"...")
CODE (U"runText = \"\"")
CODE (U"...")
CODE (U"numberOfDifferentResponses = 2")
	CODE1 (U"0.2 0.4 0.7 0.8 \"\" 40 \"\" \"left\"")
	CODE1 (U"0.6 0.8 0.7 0.8 \"\" 40 \"\" \"right\"")
MAN_END

MAN_BEGIN (U"ExperimentMFC 6. Responses are sounds", U"ppgb", 20160925)
INTRO (U"In the @@ExperimentMFC 2.1. The experiment file|example experiment@, "
	"the stimuli were sounds, and the responses were categories whose labels appeared on buttons. "
	"Sometimes you want it the other way around.")
NORMAL (U"An example is the %%/i/ prototype% task: the top of the screen just says “Please choose the best %ee”, "
	"and no stimulus sound is played. Instead, the participant can click repeatedly on an array of 40 buttons, "
	"each of which contains a different [i]-like sound. That is, if the participant clicks on a response button, "
	"an [i]-like sound is played, and every response button has its own sound.")
NORMAL (U"Such a task can be regarded as reversing the task of the example experiment, "
	"in which the stimulus was a sound and the response was a phonological category. "
	"In the /i/ prototype task, the stimulus is a phonological category, and the response is a sound.")
NORMAL (U"This is what the experiment file could look like:")
CODE (U"\"ooTextFile\"")
CODE (U"\"ExperimentMFC 7\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"numberOfDifferentStimuli = 2")
	CODE1 (U"\"i\"  \"Choose the best %%ee%.\"")
	CODE1 (U"\"I\"  \"Choose the best %%i%.\"")
CODE (U"numberOfReplicationsPerStimulus = 1")
CODE (U"breakAfterEvery = 1")
CODE (U"randomize = <CyclicNonRandom>")
CODE (U"startText = \"Click to start.\"")
CODE (U"runText = \"\"")
CODE (U"pauseText = \"You can have a short break if you like. Click to proceed.\"")
CODE (U"endText = \"The experiment has finished.\"")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
CODE (U"okButton = 0.8 0.95 0.45 0.55 \"OK\" \"\"")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
CODE (U"responsesAreSounds? <yes>")
CODE (U"responseFileNameHead = \"Sounds/\"")
CODE (U"responseFileNameTail = \".wav\"")
CODE (U"responseCarrierBefore = \"\"")
CODE (U"responseCarrierAfter = \"\"")
CODE (U"responseInitialSilenceDuration = 0.3")
CODE (U"responseMedialSilenceDuration = 0")
CODE (U"responseFinalSilenceDuration = 0")
CODE (U"numberOfDifferentResponses = 16")
	CODE1 (U"0.2 0.3 0.7 0.8 \"\" 10 \"\" \"i11\"")
	CODE1 (U"0.3 0.4 0.7 0.8 \"\" 10 \"\" \"i12\"")
	CODE1 (U"0.4 0.5 0.7 0.8 \"\" 10 \"\" \"i13\"")
	CODE1 (U"0.5 0.6 0.7 0.8 \"\" 10 \"\" \"i14\"")
	CODE1 (U"0.2 0.3 0.6 0.7 \"\" 10 \"\" \"i21\"")
	CODE1 (U"0.3 0.4 0.6 0.7 \"\" 10 \"\" \"i22\"")
	CODE1 (U"0.4 0.5 0.6 0.7 \"\" 10 \"\" \"i23\"")
	CODE1 (U"0.5 0.6 0.6 0.7 \"\" 10 \"\" \"i24\"")
	CODE1 (U"0.2 0.3 0.5 0.6 \"\" 10 \"\" \"i31\"")
	CODE1 (U"0.3 0.4 0.5 0.6 \"\" 10 \"\" \"i32\"")
	CODE1 (U"0.4 0.5 0.5 0.6 \"\" 10 \"\" \"i33\"")
	CODE1 (U"0.5 0.6 0.5 0.6 \"\" 10 \"\" \"i34\"")
	CODE1 (U"0.2 0.3 0.4 0.5 \"\" 10 \"\" \"i41\"")
	CODE1 (U"0.3 0.4 0.4 0.5 \"\" 10 \"\" \"i42\"")
	CODE1 (U"0.4 0.5 0.4 0.5 \"\" 10 \"\" \"i43\"")
	CODE1 (U"0.5 0.6 0.4 0.5 \"\" 10 \"\" \"i44\"")
CODE (U"numberOfGoodnessCategories = 5")
	CODE1 (U"0.25 0.35 0.10 0.20 \"1 (poor)\" 24 \"\"")
	CODE1 (U"0.35 0.45 0.10 0.20 \"2\" 24 \"\"")
	CODE1 (U"0.45 0.55 0.10 0.20 \"3\" 24 \"\"")
	CODE1 (U"0.55 0.65 0.10 0.20 \"4\" 24 \"\"")
	CODE1 (U"0.65 0.75 0.10 0.20 \"5 (good)\" 24 \"\"")
NORMAL (U"The participant will see 16 squares on the screen. First she will have to find the best /i/, "
	"then the best /\\ic/. The sound files “Sounds/i11.wav” and so on must exist and have the same sampling frequency. "
	"A silence of 0.3 seconds is played just before each response sound.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 7. Blanking the screen", U"ppgb", 20130102)
INTRO (U"In all the earlier examples, the flag $%blankWhilePlaying was set to `<no>`. This means that in all those cases "
	"the participant will immediately see the response categories when the sound starts playing "
	"(or even earlier, if there is an initial silence).")
NORMAL (U"This can be changed by setting $%blankWhilePlaying to `<yes>`. When you do that, the participant will see a blank screen "
	"while the stimulus is playing, and the response buttons will come up only after the sound has finished. "
	"This is useful if you want to prevent the participant from clicking before the sound has finished, "
	"or for priming experiments. Reaction times are measured from when the response buttons appear.")
NORMAL (U"If you want the response buttons to come up 0.5 seconds after the sound finishes playing, "
	"you set the $%stimulusFinalSilenceDuration to 0.5.")
MAN_END

MAN_BEGIN (U"ExperimentMFC 8. Running multiple experiments", U"ppgb", 20160925)
INTRO (U"In all the earlier examples, either the set of stimulus sounds or the set of response sounds stayed "
	"the same throughout the experiment. If you want more than one set of stimuli, or more than one set of responses, "
	"you can run several experiments after each other, simply by selecting more than one experiment, then clicking #Run.")
NORMAL (U"You can put all these ExperimentMFC objects in one text file. The following example contains two experiments. "
	"The second line has to contain the text `\"Collection\"`, followed by the number of experiments:")
CODE (U"\"ooTextFile\"")
CODE (U"\"Collection\" 2")
CODE (U"")
CODE (U"\"ExperimentMFC 7\" \"i\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"numberOfDifferentStimuli = 1")
	CODE1 (U"\"i\"  \"Choose the best %%ee%.\"")
CODE (U"numberOfReplicationsPerStimulus = 1")
CODE (U"breakAfterEvery = 0")
CODE (U"randomize = <CyclicNonRandom>")
CODE (U"startText = \"You are going to choose the best %%ee%. Click to start.\"")
CODE (U"runText = \"\"")
CODE (U"pauseText = \"\"")
CODE (U"endText = \"Thank you for choosing the best %%ee%. Click to proceed.\"")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
CODE (U"okButton = 0.8 0.95 0.45 0.55 \"OK\" \"\"")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
CODE (U"responsesAreSounds? <yes>")
CODE (U"responseFileNameHead = \"Sounds/\"")
CODE (U"responseFileNameTail = \".wav\"")
CODE (U"responseCarrierBefore = \"\"")
CODE (U"responseCarrierAfter = \"\"")
CODE (U"responseInitialSilenceDuration = 0.3")
CODE (U"responseMedialSilenceDuration = 0")
CODE (U"responseFinalSilenceDuration = 0")
CODE (U"numberOfDifferentResponses = 6")
	CODE1 (U"0.2 0.3 0.7 0.8 \"\" 10 \"\" \"i1\"")
	CODE1 (U"0.3 0.4 0.7 0.8 \"\" 10 \"\" \"i2\"")
	CODE1 (U"0.4 0.5 0.7 0.8 \"\" 10 \"\" \"i3\"")
	CODE1 (U"0.5 0.6 0.7 0.8 \"\" 10 \"\" \"i4\"")
	CODE1 (U"0.6 0.7 0.7 0.8 \"\" 10 \"\" \"i5\"")
	CODE1 (U"0.7 0.8 0.7 0.8 \"\" 10 \"\" \"i6\"")
CODE (U"numberOfGoodnessCategories = 0")
CODE (U"")
CODE (U"\"ExperimentMFC 7\" \"u\"")
CODE (U"blankWhilePlaying? <no>")
CODE (U"stimuliAreSounds? <no> \"\" \"\" \"\" \"\" 0 0 0")
CODE (U"numberOfDifferentStimuli = 1")
	CODE1 (U"\"u\"  \"Choose the best %%oo%.\"")
CODE (U"numberOfReplicationsPerStimulus = 1")
CODE (U"breakAfterEvery = 0")
CODE (U"randomize = <CyclicNonRandom>")
CODE (U"startText = \"You are going to choose the best %%oo%. Click to start.\"")
CODE (U"runText = \"\"")
CODE (U"pauseText = \"\"")
CODE (U"endText = \"All the experiments have finished. You can call the experimenter.\"")
CODE (U"maximumNumberOfReplays = 0")
CODE (U"replayButton = 0 0 0 0 \"\" \"\"")
CODE (U"okButton = 0.8 0.95 0.45 0.55 \"OK\" \"\"")
CODE (U"oopsButton = 0 0 0 0 \"\" \"\"")
CODE (U"responsesAreSounds? <yes>")
CODE (U"responseFileNameHead = \"Sounds/\"")
CODE (U"responseFileNameTail = \".wav\"")
CODE (U"responseCarrierBefore = \"\"")
CODE (U"responseCarrierAfter = \"\"")
CODE (U"responseInitialSilenceDuration = 0.3")
CODE (U"responseMedialSilenceDuration = 0")
CODE (U"responseFinalSilenceDuration = 0")
CODE (U"numberOfDifferentResponses = 6")
	CODE1 (U"0.2 0.3 0.7 0.8 \"\" 10 \"\" \"u1\"")
	CODE1 (U"0.3 0.4 0.7 0.8 \"\" 10 \"\" \"u2\"")
	CODE1 (U"0.4 0.5 0.7 0.8 \"\" 10 \"\" \"u3\"")
	CODE1 (U"0.5 0.6 0.7 0.8 \"\" 10 \"\" \"u4\"")
	CODE1 (U"0.6 0.7 0.7 0.8 \"\" 10 \"\" \"u5\"")
	CODE1 (U"0.7 0.8 0.7 0.8 \"\" 10 \"\" \"u6\"")
CODE (U"numberOfGoodnessCategories = 0")
NORMAL (U"In this example, the participant first has to choose the best /i/ from among six [i]-like sounds, "
	"which are in the sound files `i1.wav` through `i6.wav`. After that, she has to choose the best /u/ "
	"from among six [u]-like sounds, which are in the sound files `u1.wav` through `u6.wav`. "
	"The percent signs in `%%ee%` mean that %ee will be italicized.")
NORMAL (U"If you read this file with ##Read from file...#, you will see two ExperimentMFC objects, "
	"named `i` and `u`. They both stand selected. You then click #Run, and after the participant finishes, "
	"you select both ExperimentMFC objects again (probably they still stand selected), and click ##Extract results#. "
	"You will then get two #ResultMFC objects.")
MAN_END
}

/* End of file manual_ExperimentMFC.cpp */
