/* manual_whatsnew.cpp
 *
 * Copyright (C) 1992-2026 Paul Boersma
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

void manual_whatsnew_init (ManPages me);
void manual_whatsnew_init (ManPages me) {

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"What's new?"
© Paul Boersma 1995–2026

Latest changes in Praat.

##6.4.65# (24 April 2026)
• Fixed a bug that could lead to a “Text is not valid UTF-8” error message in automatic transcription.
• More windows recognize Command-Q (##Quit Praat#).

##6.4.64# (17 April 2026)
• Many small improvements and bug repairs in speech recognition, including automatic alignment and diarization.

##6.4.63# (4 April 2026)
• Speaker diarization in ##Transcribe interval# (if Whisper models installed).
• Mac: Command-mousewheel zooming (as on Windows).

##6.4.62# (13 March 2026)
• @@Sound: To TextGrid (speech activity, Silero)...@: “voice activity detection” by AI.
• TextGrid window: fixed a bug from 2013 or earlier that could cause Praat to crash during interval alignment.
• Fixed a bug that caused Praat to crash if a notebook was run twice.
• Linux: support horizontal scrolling in three ways (two-finger swiping on the touchpad;
  left and right tilting of the mouse’s scroll wheel; turning the mouse’s scroll wheel
  while keeping the Shift key pressed).
• Linux: support zooming by turning the mouse’s scroll wheel
  while keeping the Ctrl key pressed).

##6.4.61# (28 February 2026)
• ##Transcribe interval# (via automatic speech recognition): better word alignment via Whisper’s built-in DTW.
• ##Transcribe interval# (via automatic speech recognition): option for silence removal (via an additionally installed AI model).
• Windows: Ctrl+Alt+Letter menu shortcuts have become Ctrl+Shift+Letter, because that’s the
  way it’s nowadays done in e.g. browsers and NotePad, and because Ctrl+Alt+Letter combinations
  can stand for actual Alt-GR characters on some keyboards. On the Mac we still use Option+Command+Letter.
  Note that on most Windows keyboards the Ctrl and Alt keys are adjacent, as are the Option and Command
  keys on most Mac keyboards.
• Manual: menu shortcuts for ##Previous page# and ##Next page#.

##6.4.60# (10 February 2026)
• Scripting: fixed an interaction between pause windows and @`exitScript`.
• Scripting: turned cases of multiple identical procedure names into
  once-warnings instead of errors.

##6.4.59# (5 February 2026)
• Made nested scripts with pause windows and/or Demo window work again.
• Fixed a bug by which SpeechRecognizer creation could make Praat crash on some platforms.

##6.4.58# (25 January 2026, supersedes versions 6.4.52 through 6.4.57)
• MacOS Tahoe: support making menu choices or turning windows to full screen
  if a script is paused (by the Demo window or the Pause window),
  as on earlier MacOS versions and on Windows and Linux.
• Windows: support horizontal scrolling in three ways (two-finger swiping on the touchpad;
  left and right tilting of the mouse’s scroll wheel; turning the mouse’s scroll wheel
  while keeping the Shift key pressed).
• Windows: better dotted or dashed thick lines. 
• @@Sound: To PowerCepstrogram...@ now averages the power over channels instead of taking only channel 1.
• Opening TIFF files with @@Read from file...@, as well as @@Insert picture from file...@:
  rid a bug that would show monochrome TIFF files in four vertical stripes.
• Mac and Linux: made @@Insert picture from file...@ work for PDF files again (bug since 6.4.28).
• Backslash trigraph for apico-postalveolars: s\.v (`s\.v`), following @@Ladefoged & Maddieson (1996)@, who wanted to
  distinguish them from the more extreme retroflexes, i.e. apico-palatals such as \s. (`\s.`).
• Linux: prevented a crash when a “barren” edition tried to run `runAllTests_batch.praat`.
• Scripting: @`runSubprocess` and @`runSystem` now support @`asynchronous`, with
  which you can spawn a subprocess without waiting for it to finish. 
• Scripting: @`randomImax`.
• Scripting: in pause forms, allow vector expressions as defaults in vector fields.
• Scripting: syntax check against duplicate procedure names.
• Scripting: no longer any limits on the number of labels and procedures.

##6.4.51# (30 December 2025)
• Windows: switch back to the faster version (version 6.4.50 was deliberately slower,
  so as to support Windows 7 and 8.1 for the last time).

##6.4.50# (29 December 2025)
• A bit of speech recognition via Whisper.cpp (adapted to Praat by Anastasia Shchupak),
  such as ##Transcribe interval# in the TextGrid window. This works after you install
  one or more Whisper-cpp models, such as `ggml-base.bin` (from
  `https://huggingface.co/ggerganov/whisper.cpp`) in the `models` subfolder of the @@preferences folder@.

##6.4.49# (23 December 2025)
• Open without error messages or warnings Kay files that have one sample too few or too many.
• Linux: the standard Linux edition now runs equally well on Ubuntu, Fedora, Debian and Centos.
• EditDistanceTable: made readable and writable.

##6.4.48# (9 December 2025)
• Linux audio: recognize JACK devices (next to the already recongized ALSA devices).
• Linux fonts: support for the Charis 7 font, as on macOS and Windows.
• OTGrammar: non-negative maximum entropy learning.
• Debug option at start-up from command line, e.g. `--debug=58`.

##6.4.47# (7 November 2025)
• ##Sound: To LPC...#: channel averaging by default.
• @`runSystem` and @`runSubprocess` should not divert a terminal window’s standard output.
• Repaired a bug from November 2014 by which writing a BarkSpectrogram to a binary file used an incorrect format.

##6.4.46# (20 October 2025)
• ScriptEditor: made ##Run selection# work again on Windows and Linux (bug since 6.4.44).
• Made pause windows (and perhaps Demo window) compatible with macOS 26 (Tahoe).

##6.4.45# (12 October 2025)
• Made @@Picture window@ 60\xx60 inches (instead of 12\xx12).
• Windows: repaired a crashing bug in @`runSystem` introduced in 6.4.44.
• Made progress bar writing compatible with macOS 26 (Tahoe).
• Repaired an old bug that could crash Praat when viewing a Table with 0 rows.

##6.4.44# (25 September 2025)
• Made @`runSubprocess` compatible with the Microsoft runtime (instead of MinGW).
• Made Info window writing compatible with macOS 26 (Tahoe).
• Made @`demoShow` compatible with macOS 26.
• Made @ScriptEditor menus reachable on macOS 26 when the script is paused.

##6.4.43# (14 September 2025)
• Calling Praat from the command line: the new switch `--send-or-form` sends a script
  to a different already running GUI instance of Praat, like `--send` does,
  but takes no arguments (except for the name of the script file)
  and presents a form on the screen if the script has a `form` in it.
• More detailed manual page for @`fileNames$#` ().

##6.4.42# (28 August 2025)
• Recognize more OGG sound files.
• Recognize the pre-2025 `sendpraat` message on Windows again (compatibility with Elan 7.0).

##6.4.41# (21 August 2025)
• Faster computation of some formants (because of multithreading).
• Removed a bug that caused Praat to crash on @@Sound: To Formant (robust)...@.

##6.4.40# (16 August 2025)
• Faster computation of spectrograms and some formants (because of multithreading).
• Full awareness of processors with up to 400 physical threads (i.e. 200 or 400 cores).

##6.4.39# (13 July 2025)
• Calibrated cepstral peak prominence (@@PowerCepstrogram: Get CPPS...@).
• Windows: restored “\@ ”, “~” and “\bs” on Latin-American Spanish keyboards.

##6.4.38# (1 July 2025)
• Macintosh and Windows: put back Charis SIL 6.200 for downloading with older Praat versions.
• PairDistribution: ##Set weight...#.
• OTGrammarEditor: removed a bug by which the window wouldn’t update automatically after learning.

##6.4.37# (28 June 2025)
• Macintosh and Windows: support for the Charis 7 font.

##6.4.36# (25 June 2025)
• Sound window: restore loudspeaker icon (for multi-channel sounds; bug since 6.4.31).
• Scripting: function @`moveAndOrRenameFile` ().
• FLAC 1.5.0.
• Scripting on Windows: more Command-key combinations with @`demoKey$` in Demo window.

##6.4.35# (15 June 2025)
• Pitch analysis: guard against zero pitch floor if logarithmic.
• Scripting: @`fixed$` () for vectors and matrices.
• Linux: removed hard-coded background colour.
• Full website `https://praat.org` now mirrors `https://www.fon.hum.uva.nl/praat` (to prevent down time).

##6.4.34# (20 May 2025)
• Linux: made @`demoShow` () work again on GTK 3.
• Scripting: @`clock` ().

##6.4.33# (18 May 2025)
• H1-minus-H2 in the voice report.
• Repaired an old bug that made spectrogram computation in the Sound window crash under some uncommon combinations of settings.

##6.4.32# (14 May 2025)
• Better continuity of integration of energy (finally corresponds to manual).
• Scripting: more reliable @`random_initializeWithSeedUnsafelyButPredictably` (works even after an odd number of @`randomGauss`).

##6.4.31# (3 May 2025)
• TextGrid window: rubber banding within the selection,
  by dragging a boundary while keeping the Option and Command keys pressed (plus the Shift key, if you want to drag in all tiers).
• Backslash trigraphs: s\vv (`s\vv`), m\LIb (`m\LIb`), \V^ (`\V^`), d\// (`d\//`), d\-/ (`d\-/`), \l.\~/ (`\l.\~/`).
• Unicode 16.0.

##6.4.30# (29 April 2025)
• Made the whole of the Buckeye corpus readable.
• Backslash trigraphs for tone symbols: a\-' (`a\-'`), a\'- (`a\'-`), a\`- (`a\``-`), a\-` (`a\-```), a\rf (`a\rf`), a\fr (`a\fr`).
• Other backslash trigraphs: d\mv (`d\mv`), d\wv (`d\wv`).

##6.4.29# (27 April 2025)
• ##Read Sound and adjacent annotation files (TIMIT)...#: if you open any sound file from the TIMIT corpus,
  the annotation files with the same name in the same folder are interpreted as a TextGrid and opened as well.
• ##Read Sound and adjacent annotation files (Buckeye)...#: if you open any sound file from the Buckeye corpus,
  the annotation files with the same name in the same folder are interpreted as a TextGrid and opened as well.
• Backslash trigraphs for tone symbols: a\'' (`a\''`), a\`` (`a\`````),
  \-5 (`\-5`), \-4 (`\-4`), \-3 (`\-3`), \-2 (`\-2`), \-1 (`\-1`), \|u (`\|u`), \|d (`\|d`).
• Backslash trigraphs for diagonal arrows: \NE (`\NE`), \SE (`\SE`), \SW (`\SW`), \NW (`\NW`);
  the first two of these double as tone symbols.

##6.4.28# (22 April 2025)
• TextGrid window: removed a bug that caused slow scrolling (i.e. computation of whole-sound Pitch and Formant objects)
  if the TextGrid was much longer than the Sound and the Sound was scrolled out of sight.
• EEG window: repaired a bug that caused Praat to crash when clicking ##View & Edit#.
• Windows Demo window: made Escape key available to `demoKey$()`
  (this had been a bug since the beginning of the Demo window, i.e. since May 2009).
• ##Read TextGrid from ESPS label file...#
• Repaired a bug that could cause Praat to crash on pitch analysis when a part of the sound was constant.
• ##Insert picture from file...#: the picture is now embedded into Praat picture files;
  this also solves a problem with file paths that surfaced when a script called another script that inserted a picture.
• ##Photo: Save as PNG file...#: repaired a bug that caused a crash on the Mac.

##6.4.27# (27 January 2025)
• Graphics text: better pipes and slashes.
• Windows for ARM64 processors: removed a bug whereby dynamic menu deletion could crash after a sound was played in the Demo window.

##6.4.26# (8 January 2025)
• ##Rectangle text (maximal fit)...# is perhaps useful in general,
  but will especially facilitate text and button drawing in Demo window.
• Linux for ARM64 processors: sped up some slow computations by a factor of over 100.
• ##OTGrammar & Strings & Strings: Tabulate all correct rankings# lists all full rankings
  that map the given list of inputs to the given list of partial outputs,
  using brute-force permutation of up to 18 constraints.
• ##OTGrammar: Tabulate equivalent pairs# checks to what extent different candidates
  have different sets of constraint violations.

##6.4.25# (8 December 2024)
• Command-line interface: solved a crash that occurred when a menu command was not available.
• Windows: open-source code signing.

##6.4.24# (1 December 2024)
• FormantPath: fixed bug with sampling frequency.
• Formant measurements: fixed bugs in “autocorrelation” and “robust” methods.
• Manual: better support for screens taller than 15 inches.
• Scripting: support for subtraction and unary minus for tensors.
• Scripting: removed `sendsocket`.

##6.4.23# (27 October 2024)
• Audio output: higher-quality playback of sounds with unusual sampling frequencies.
• Scripting: better error messages on #Save commands.
• Better HTML output from manuals (e.g. italicized or bolded backslash sequences).

##6.4.22# (5 October 2024)
• More robustness against trying to run a binary file as a script.
• Windows Sound window: faster zooming with Ctrl-scrollwheel.
• Windows: guarantee Explorer-type file selection windows.

##6.4.21# (21 September 2024)
• SpeechSynthesizer: repaired an old bug whereby ##Get phonemes from text...# could increase the duration
  of a subsequent ##Play# (or ##To Sound...#).
• SpeechSynthesizer and TextGrid alignment: repaired a bug (existing since 6.4.18) whereby #clause and #word tiers would be empty.
• TextGrid window: disambiguated Command-F5 (now Option-F5 for ##Get minimum pitch#; likewise, Option-F8 for ##Get minimum intensity#).
• Calling Praat from command line: reenabled `--pref-dir=` option.

##6.4.20# (18 September 2024)
• Linux: repaired a major bug whereby Praat could crash when trying to draw to a hidden Picture window.
• Scripting: just like normal forms, pause forms now also accept “yes” and “off” (and the like) instead of just 1 and 0.
• Scripting: repaired a minor bug whereby `optionmenu` in forms would show an incorrect default value.

##6.4.19# (4 September 2024)
• Repaired many bugs in the eSpeak part of Praat.

##6.4.18# (21 August 2024)
• SpeechSynthesizer and TextGrid alignment: solved an eSpeak-internal bug that caused some
  rare long phoneme codes to be handled incorrectly.
• Scripting: `padLeft$`(), `truncateLeft$`() and `padOrTruncateLeft$`(), and their `Right` counterparts.
• s390x edition for Linux.

##6.4.17# (8 August 2024)
• Spectrogram: #Paint: solved a bug that could cause Praat to crash if the time domain was smaller than that of the Spectrogram.
• ScriptEditor: ##Run selection# can call procedures outside the selected text.
• Audio files: can open Polyphone (Shorten) files.

##6.4.16# (29 July 2024)
• Windows: make command-line output visible (new on ARM64, was lost in 6.4.14 for Intel64 and Intel32).

##6.4.15# (26 July 2024)
• Distinguish between %%pitch ceiling% (flat) and %%pitch top% (curved ceiling),
  mainly because of their different meanings.

##6.4.14# (22 July 2024)
• Pitch menu (in e.g. Sound or TextGrid window): separate settings for four pitch analysis methods.
• Formant analysis: faster on all systems.
• ARM64 Windows: worked around a compiler bug that caused incorrect window sizes.
• Pitch analysis: faster on Windows (Intel64 and Intel32).
• Windows: better fonts and background colours (perhaps dependent on language settings).
• ##TextGrids: Concatenate#: guard against some incorrect TextGrid files.

##6.4.13# (10 June 2024)
• More manual pages.
• @@Sounds: Concatenate with overlap...@: guard against a crash that would occur
  if the total overlap time was longer than the original sounds together.
• @`number` recognizes hexadecimal numbers again (bug since February 2023).

##6.4.12# (2 May 2024)
• ##Align interval#: fewer error messages.

##6.4.11# (26 April 2024)
• Menu commands that call a script (e.g. those that were added from a plug-in)
  are now visible as such by being marked with a little “+” sign.
• Fixed a bug that could cause the SpeechSynthesizer to mishandle European
  characters with accents on Linux.

##6.4.10# (21 April 2024)
• Function windows: fixed a problem that could cause Praat to crash when doing #all after #Group
  when windows had a different duration.

##6.4.09# (20 April 2024)
• TextGrid window: fixed a rare problem with ##Align interval# that could lead to a zero-length interval.
• Windows ARM64 edition: fixed a bug that would cause ##Align interval# to crash.
• ##Sound & TextGrid: Align interval...# now also available in the Objects window.

##6.4.08# (14 April 2024)
• Windows: fixed a problem with the right and bottom edges of windows (half-visible scroll bars)
	for the Intel editions (the ARM64 edition still has this problem).
• Scripting: fixed a bug that could cause Praat to crash when an unknown choice was given to an option menu.
• MelSpectrogram: fixed a bug that could cause Praat to crash if a mel value was negative.
• DataModeler: improved confidence limits on estimated model parameters.

##6.4.07# (17 March 2024)
• The command @@Sound & Pitch: To PointProcess (cc)@, which is also used when computing the times of pulses
  in the Sound window, could cause Praat to freeze when the pitch ceiling (and the measured pitch)
  were very high; this problem existed since at least 2001.
• eSpeak: new version, with e.g. Tigrinya.
• Support for 119-dpi graphics on Windows.
• Repaired a bug that could cause the Artword windows to crash if the Time field was left empty.

##6.4.06# (25 February 2024)
• Sound files on Windows: corrected a bug (existing since July 2021)
  that prevented reading FLAC files with special characters in their names.
• FormantPathEditor without Sound: corrected several bugs that caused Praat to crash (such as ##Formant listing#, using the Pitch menu...).
• Spectrogram painting: improved painting of very quiet spectrograms at extreme dynamic ranges.
• Scripting: @`folderExists`.
• Scripting: home-relative paths (such as `~/sounds/*.wav`) in more locations,
  such as @`fileNames$#`, @@Create Strings as file list...@, and @`createFolder`.

##6.4.05# (27 January 2024)
• Many typos in manual.
• Made TIMIT label files readable on Windows.
• Prevented 8000 files from being read at start-up.
• Mac and Linux scripting: @`fileReadable` now returns 0 if the argument is a folder (as already on Windows).

##6.4.04# (6 January 2024)
• Typo in example script in manual.
• ARM64 edition for Linux (Ubuntu 20.04 and up).
• ARM64 edition for Chromebook (2020 and later).

##6.4.03# (4 January 2024)
• ARM64 edition for Windows.
• ARM64 edition for Linux (Ubuntu 22.04 and up).
• ARM64 edition for Chromebook (2022 and later).

##6.4.02# (30 December 2023)
• Saving sounds as MP3 files.

##6.4.01# (30 November 2023)
• Removed some visible percent signs before variable names from the scripting tutorial.
• SoundEditor/TextGridEditor logging: when logging 'f0',
  Praat now correctly reports values in Hz (rather than logarithms)
  if Unit is set to “Hertz (logarithmic)”.
  This bug had existed since Praat version 4.3.16 (June 2005).
• SoundEditor/TextGridEditor scripting compatibility: made old versions of “Pitch settings...”
  and “Advanced pitch settings...” available to scripts again, made the old denominations
  “autocorrelation” and “cross-correlation” (as pitch analysis methods) available to scripts again,
  and made obsolete versions of pitch analysis settings (such as “Pitch silence threshold”)
  available again, now under the COMPATIBILITY section of the output of “Editor info”.
  This will allow older editor scripts to continue to run without change.

What used to be new?
====================
• @@What was new in 6.4?
• @@What was new in 6.3?
• @@What was new in 6.2?
• @@What was new in 6.1?
• @@What was new in 6.0?
• @@What was new in 5.4?
• @@What was new in 5.3?
• @@What was new in 5.2?
• @@What was new in 5.1?
• @@What was new in 5.0?
• @@What was new in 4.6?
• @@What was new in 4.5?
• @@What was new in 4.4?
• @@What was new in 4.3?
• @@What was new in 4.2?
• @@What was new in 4.1?
• @@What was new in 4.0?
• @@What was new in 3.9?
• @@What was new in 3.8?
• @@What was new in 3.7?
• @@What was new in 3.6?
• @@What was new in 3.5?
• @@What was new in 3.3?
• @@What was new in 3.2?
• @@What was new in 3.1?

################################################################################
"What was new in 6.4?"
© Paul Boersma 2022,2023

##6.4# (15 November 2023)
• New pitch analysis methods: @@Sound: To Pitch (filtered ac)...@ and
  @@Sound: To Pitch (filtered cc)...@.
• @@Sound: To Pitch (filtered ac)...@ has become the preferred method for
  measuring vocal-fold vibration and intonation (@@Sound: To Pitch (raw cc)...@
  is still the preferred method for voice analysis).
  See @@how to choose a pitch analysis method@.

##6.3.20# (24 October 2023)
• SpeechSynthesizer and automatic alignment in TextGridEditor: worked around a bug
  introduced in 6.3.18 that caused incorrect rendering of phonemes consisting
  of four UTF-8 bytes (such as /ɔː/).
• SpeechSynthesizer and automatic alignment in TextGridEditor: worked around a very
  old bug that deleted word-final /l/ in Dutch and English.

##6.3.19# (20 October 2023)
• MacOS: resolved a crash when Praat started up on macOS 12 or older
  (worked around a bug in the Xcode linker that comes with macOS 14 Sonoma).

##6.3.18# (8 October 2023)
• @SpeechSynthesizer: included the new version of eSpeak (development 1.52), which has 133 languages.
• @@Add to menu...@: you can now add separators, headers and indented submenus to menus in editor windows.
• Scripting: new functions @`index_caseInsensitive`, @`rindex_caseInsensitive`,
  @`startsWith_caseInsensitive`, @`endsWith_caseInsensitive`
  @`fileNames_caseInsensitive$#`, @`folderNames_caseInsensitive$#`.

##6.3.17# (10 September 2023)
• SoundEditor: three Paste options, namely ##Paste before selection# (Shift-Command-V),
  ##Paste over selection# (Option-Command-V), and the already existing ##Paste after selection#
  (still Command-V).
• All editor windows: extended the old single-level Undo and Redo
  to a ten-level Undo and Redo.
• Scripting: the new @`runSystem$` and @`runSubprocess$` return the output
  of the system command or subprocess.
• Scripting: @`runSystem`, @`runSystem$`, @`runSubprocess` and @`runSubprocess$`
  report any error messages generated by the system command or subprocess.
• PointEditor (pulses): settable ##Period floor#, ##Period ceiling#,
  ##Maximum period factor# and ##Maximum amplitude factor#, for jitter and shimmer measurements.

##6.3.16# (29 August 2023)
• Sound analysis areas: better visible contrast in intensity curve (green on lime).
• Windows: repaired a bug introduced in 6.3.10 (as a result of the repair mentioned there)
  by which the Demo window would be insensitive to arrow keys.
• SpeechSynthesizer: repaired a bug that caused ##Get phonemes from text...#
  to return empty text.
• Repaired a bug that caused Praat to crash when you supplied 0
  for the channel number in @@Sound: Get nearest zero crossing...@.

##6.3.15# (23 August 2023)
• SpeechSynthesizer: improvements in ##Get phonemes from text...#
• Scripting: @`part#`, @`part##`, @`selected$#`.

##6.3.14# (4 August 2023)
• Repaired a bug that occurred when saving a @FormantPath to disk.

##6.3.13# (31 July 2023)
• Repaired a bug introduced in 6.3.12 by which a link to a file wasn’t found in external manual pages.

##6.3.12# (24 July 2023)
• Improvements in manual, including corrections on layout problems introduced in version 6.3.11.

##6.3.11# (17 July 2023)
• VowelEditor: repaired a bug by which the VowelEditor wouldn’t start up if an incorrect marks file had previously been specified.
• Mac: use a Cherokee font that distinguishes "ka" and "wo" well.
• OTMulti: vertical constraint names support newlines and disharmonies.
• Text windows are one tab plus 84.5 characters wide.
• Table: ##Get all values in column...#.
• Undocumented alpha support for Praat notebooks.
• Formulas: @`correlation` (%`vec1#`, %`vec2#`), @`vertical$` (for formatting string arrays).
• Scripting: @`readFile#`, @`readFile##`.

##6.3.10# (3 May 2023)
• Windows: repaired a bug by which key strokes would no longer go to the Demo window after a pause window with no text fields was closed.
• Repaired a bug in the VowelEditor by which it would crash when starting up if fewer than 4 numbers had been set in the “Extra formant–bandwidth pairs” field.
• FormantPathEditor improvements.
• Undocumented alpha support for Praat notebooks.
• Scripting: vector literals can be indexed.
• Better interactions between editor scripts and pause windows.

##6.3.09# (2 March 2023)
• SpeechSynthesizer can do eSpeak’s grapheme-to-phoneme conversion in 130 languages.
• FormantPath can now have time stretches that are not aligned to frame boundaries.
• Stabler handling of edge cases in editor scripting.
• Corrected an at least 23-year-old bug by which menu titles could not be greyed out on Windows.

##6.3.08# (10 February 2023)
• Corrected a bug introduced in 6.3.04 by which `word` and `sentence` would not work in new-style script forms.

##6.3.07# (6 February 2023)
• Corrected an old bug that caused ##Matrix: Draw as squares# to crash on a 1\xx1 matrix.
• NoulliGrid window: “NoulliGrid” menu.
• Scripting: faster vectors in arguments to `runScript()`.

##6.3.06# (31 January 2023)
• Corrected a bug introduced in 6.3.04 by which `optionmenu` would not work in new-style script forms.
• Scripting: `min()`, `max()`, `imin()` and `imax()` for vectors.

##6.3.05# (27 January 2023)
• Corrected a bug introduced in 6.3.04 by which some old-style forms could not be read.
• Repaired an old bug by which `chooseReadString$()` could crash on Linux.

##6.3.04# (24 January 2023)
• Playing multiple sounds with #Play now concatenates them without breaks,
and is interruptable by the Escape key.
• Renamed the remaining occurrences of #Preferences to #Settings
	(old scripts will continue to work).
• ##Create Polygon from values...
• ##Sound: To Sound (derivative)...
• Scripting: forms now look more like other commands.
• Scripting: forms can contain `infile`, `outfile`, `folder`, `realvector`,
	`positivevector`, `integervector` and `naturalvector` fields.
• Scripting: in forms and pause forms, you can now set the number of lines in
	`text`, `infile`, `outfile`, `folder`, and vector fields.

##6.3.03# (17 December 2022)
• Sound files: read Kay\re Nasometer\tm files (as separate channels).
• Script window: indent and exdent.
• Scripting: conserve editor environment in `runScript`.

##6.3.02# (29 November 2022)
• Linux: corrected a bug introduced in 6.2.20 by which typing Tab would not start a Sound.
• Sound window: corrected a bug introduced in 6.2.19 by which drawing selection hairs could not be switched off
	(or on) when painting a spectrogram to the Picture window.

##6.3.01# (21 November 2022)
• Fixed a crash introduced in 6.3 when closing an editor window by choosing #Close (or by typing Command-W).

################################################################################
"What was new in 6.3?"
© Paul Boersma 2021,2022

##6.3# (15 November 2022)
• Number-aware sorting for labels when drawing tables.
• Scripting: @`sort$#` (), @`shuffle$#` (), @`sort_numberAware$#` ().
• Fixed a crash when saving (or copying) a DTW object.
• Scripting: fixed a crash when using `Close` after @`editor`.

##6.2.23# (8 October 2022)
• Fix a crash introduced in August 2020 when in the Sound window you would select a part, then click #sel,
  then select a part, then do #Cut, then click #bak.
• #Rename should leave an object’s ID in the title of the editor window.
• Windows graphics should support a resolution of 690 dots per inch.
• Editor scripting: corrected a bug introduced in 6.2.15 whereby an added editor script would lose
  the editor context if it contained a form, after #Apply was clicked.
• Editor windows (such as the Sound window): showing a dialog window that calls a script
  (e.g. ##Log script 3#, ##Log script 4# or an added command) should remove the previous version of that window.
• Mac editor windows: dialog windows that call scripts should disappear
  when the editor goes away (as on Windows and Linux).

##6.2.22# (6 October 2022)
• Playing cursor in Pitch, PitchTier, KlattGrid and PointProcess windows.
• Corrected a bug introduced in 6.2.15 whereby an added editor script would lose
  the editor context if it contained a form, after #OK was clicked.

##6.2.21# (1 October 2022)
• Corrected a bug introduced in 6.2.15 whereby a sub-window in #Inspect would crash upon clicking #Change.

##6.2.20# (24 September 2022)
• More balanced colours.
• Hierarchical menus in editor windows.
• Sound window scripting: understand `editor$` again in log format (bug since 6.2.15).

##6.2.19# (12 September 2022)
• Visible window queries in #Time menu.
• Correct execution of added Objects window script after running a failing editor script (decades-old bug).
• Made ##Erase first# work again (bug introduced in 6.2.15).

##6.2.18# (2 September 2022)
• SoundRecorder: support for up to 20 (instead of 8) input devices.
• FormantPath window: can now be viewed even without a Sound.
• Windows audio: SoundRecorder prevents high CPU usage when not recording.

##6.2.17# (23 August 2022)
• FormantPath window: more information shown.
• Windows audio: compatible with speakers that do not support mono.
• Editor scripts: made two old commands available again (removed in 6.2.15).

##6.2.16# (18 August 2022)
• FormantPath window: non-editable TextGrid.
• Scripting the Demo window on the Mac: better guarantees that `demoShow` () actually drains on current systems.
• TextGrid works again with LongSound (bug introduced in 6.2.15).
• Editor scripts work again from menus (bug introduced in 6.2.15).

##6.2.15# (14 August 2022)
• Legends in editor windows.
• The menus #Time and #Play are new in editor windows, and the menus #Spectrum and #Formant were renamed to #Spectrogram and #Formants.
• More menu commands in editor windows.
• Rid frames around areas in editor window, except if editable.
• The FormantPath window can include a copy of a TextGrid.
• Restored horizontal and vertical dragging in tier areas (had been absent for two years).
• Removed several small bugs (and probably introduced a couple of new ones) in editor windows.
• Restored the original Praat icon.

##6.2.14# (24 May 2022)
• TextGrid window: repaired a bug introduced in 6.2.10 by which a click in the boundary insertion circle
  would copy the contents of a new interval from another tier (if that other tier stood selected).

##6.2.13# (18 May 2022)
• Playing a sound on the Mac: prevented an audible click at the end of audio output (appending a silence at the end is no longer needed).
• FormantPath window: more flexible visualization.
• New versions of PortAudio and Unicode.
• Removed the sendpraat directive.
• Removed several small interface bugs, and improved manual.

##6.2.12# (17 April 2022)
• @@Table: Get sum...@
• Scripting an editor window can now query analyses that are not visible (e.g. after zooming).

##6.2.11# (8 April 2022)
• Windows: Demo window could hang with use of Chinese keyboard (after #Play).

##6.2.10# (17 March 2022)
• TextGrid window: restored the splitting of labels.
• ##Save Demo Window as PDF file...# (#Goodies menu).

##6.2.09# (15 February 2022)
• Windows: support for a screen resolution of 204 dpi.

##6.2.08# (5 February 2022)
• Mac: more checks on audio devices when playing sounds.

##6.2.07# (28 January 2022)
• Prevented a rare crash in the TextGrid window.
• Windows: corrected a bug introduced in 6.2.04 by which some file names were unreadable.

##6.2.06# (20 January 2022)
• Mac: prevent Demo window from hanging on copy or paste.

##6.2.05# (5 January 2022)
• Calling Praat from the command line: the switch `--send` sends a script
  to a different already running GUI instance of Praat; this is more flexible than using %sendpraat,
  because `--send` will start a new instance of Praat if Praat is not yet running.

##6.2.04# (18 December 2021)
• Praat now includes eSpeak 1.51, raising the number of supported languages from 100 to 130.
• Scripting: support for `infile`, `outfile` and `folder` fields in forms.
• Calling Praat from the command line: the switch `--open` now adds files to an already running instance of Praat;
  this was already the case on Windows, but it now works in the same way on the Mac and on Linux.
• Scripting on Mac: multiline text fields in forms have scroll bars and behave like other fields.

##6.2.03# (3 December 2021)
• Mac: made Enter key work as expected (i.e. character selection) on Japanese keyboards in multi-line text fields in dialogs and TextGrid window.

##6.2.02# (2 December 2021)
• Fixed a hang if there were negative pitch values in @@Sound: Change gender...@.
• Fixed a crash if the size of a PCA did not match the size of a TableOfReal.

##6.2.01# (17 November 2021)
• Sound window: corrected a bug that caused a thick waveform during or after playing.

################################################################################
"What was new in 6.2?"
© Paul Boersma 2019–2021

##6.2# (15 November 2021)
• TextGrid window: closing box and opening triangle for IPA chart.
• Linux: compatibility with Wayland (rather than just X11).
• Chromebook: surrogate window title replaces the one that Chromebooks won’t show.
• Much longer list of functions in manual.

##6.1.56# (3 November 2021)
• Scripting: @@Record Sound (fixed time)...@ should return an object ID.
• More phonetic symbols in EPS files.

##6.1.55# (25 October 2021)
• TextGrid window: guard against a rare crash on Windows.
• TextGrid: Extract part: guard against producing an empty interval tier.
• Plug-ins: settings windows should not show the file name of the script.
• Scripting: clicking Cancel when browsing an Infile field should not crash.
• Phonetic symbols: a new backslash trigraph for a mid-centralized diacritic (as in [e\x^]), namely `\x^`.

##6.1.54# (9 October 2021)
• PowerCepstrogram: can tabulate cepstral peak prominence values.
• Mac: repaired global Escape key menu shortcut (stops sounds that play from the Objects window).

##6.1.53# (8 September 2021)
• Linux: corrected a bug that caused PDF files to be misaligned
  if the origin was not in the top left corner.

##6.1.52# (25 August 2021)
• FormantPath window: more formant listings and stress listings.
• Sound window: on #Cut or #Paste, all windows that contain that Sound now get ungrouped.
• Corrected a crash when reading Vorbis files on some 32-bit platforms.

##6.1.51# (22 July 2021)
• @@Spectrum: Tabulate (verbose)@ with:  power spectral density, bin width within domain, bin energy, and much more.
• Sound window: ##Get amplitude(s)#.
• Scripting: @`date_utc$` (), @`date#` (), @`date_utc#` ().

##6.1.50# (20 June 2021)
• RealTier: editing, Formula, conversion from and to Matrix and Table and other tiers such as PitchTier,
  DurationTier, IntensityTier and AmplitudeTier.
• Scripting: can now assign multiple objects to a vector.
• Fix crashes in ##Create letter R example# and ##Create KlattTable example# introduced in 6.1.49.

##6.1.49# (12 June 2021)
• @PitchTier, @IntensityTier, @DurationTier and @AmplitudeTier windows: save preferences.
• @Manipulation window: removed unused semitone options.
• @RealTier object: editing; conversions from and to other tiers and from Table and Matrix.

##6.1.48# (2 June 2021)
• TextGrid window: Shift-drag drags multiple boundaries again even if the Shift key is not released.
• Scripting: publishing (#Extract) an object from an editor window returns its ID again.
• Scripting: ##Select...# in an editor window is now prevented from selecting outside the time domain.

##6.1.47# (21 May 2021)
• The Escape key now means Cancel also in a non-pausing pause window.

##6.1.46# (20 May 2021)
• Fixed a bug with line types in the Picture window.
• TextGrid window: shift-click on a boundary again extends the selection.

##6.1.45# (20 May 2021)
• Praat picture files can be drawn with ##Read from file# again.

##6.1.44# (17 May 2021)
• Scripting: queries in editor windows are scriptable again.

##6.1.43# (13 May 2021)
• TextGrid window: characters in the IPA chart grow with the window.
• TextGrid window: seeing the maroon shadow now always means that the boundary will be moved when the mouse is released (problem since 6.1.20 or so).
• Scripting: pause window on the Mac: the Enter key works as designed (bug in 6.1.42).

##6.1.42# (15 April 2021)
• Settings windows: taller text fields for formulas and vectors.
• Mac TextGrid window: cursor moves to end of text after selecting an interval (as on Windows and Linux).

##6.1.41# (25 March 2021)
• TextGrid window: unselected boundaries can again be drawn to the cursor.
• Windows: faster running cursor.
• Multi-line formula fields.
• Scripting: guard against too deep recursive calls to runScript (depth 20).
• EEG: repaired a bug that could cause Praat to crash when computing an ERPTier.

##6.1.40# (27 February 2021)
• Inspect window: corrected a bug that could cause Praat to crash when opening a string vector.
• Sound window: made channel scrolling work again when there are more than 8 channels.
• Sound window: made channel muting by clicking on the loudspeaker symbols work again.

##6.1.39# (8 February 2021)
• Scripting: made string vectors visible in @`writeFile` (), @`pauseScript` () and @`exitScript` ().
• Mac: corrected a bug that could cause About Praat to crash on macOS 10.10.

##6.1.38# (2 January 2021)
• Scripting: string vectors, @`empty$#` (), @`readLinesFromFile$#` (), @`fileNames$#` (), @`folderNames$#` (), @`splitByWhitespace$#` ().
• Sound files: open Ogg Vorbis files and Opus files.

##6.1.37# (16 December 2020)
• TextGrid window: removed a bug that caused Praat to crash when option-clicking or command-clicking a boundary when another boundary was selected on a point tier.
• @@Record Sound (fixed time)...@: removed a bug that caused Praat to crash with certain combinations of duration and sampling frequency on Windows.
• PowerCepstrogram: more precise integration, so that the cepstral peak prominence is now a continuous function of time.
• TextGrid: Get points (preceded): removed a bug that caused Praat to crash under some edge conditions.
• Scripting: @`tryToWriteFile` () and @`tryToAppendFile` () should not be available in manuals.

##6.1.36# (6 December 2020)
• Scripting: @`tryToWriteFile` (), @`tryToAppendFile` (); formulas can have a size of 1 million.
• Obscured scripts: error messages always disclosed.
• IPA chart: sacrifice laminality symbol for breathiness symbol.

##6.1.35# (29 November 2020)
• Scripting: @`sort#` (), @`shuffle#` (), @`to#` (), @`from_to#` (), @`from_to_by#` (), @`from_to_count#` (), @`between_by#` (), @`between_count#` ().
• Mac: repaired a bug existing since 6.1.04 that caused incorrect line counts in text files with high Unicode characters.

##6.1.34# (25 November 2020)
• Mac: got bold and italic to work on Praat versions compiled with the BigSur API.
• SoundRecorder always a white background, instead of a different background on each platform.

##6.1.33# (19 November 2020)
• Mac: got audio to work on Praat versions compiled with the BigSur API (6.1.32 did not have audio on BigSur).

##6.1.32# (16 November 2020)
• First version for Macs with Apple Silicon chip.
• Mac: corrected a bug that could cause Praat to crash when a Sound window was closed from a script.
• Windows and Linux: made ##Text width...# work correctly from batch.
• Linux: reinstalled Tab shortcut in Sound window.

##6.1.31# (10 November 2020)
• Linux: first GTK-3 version.

##6.1.30# (3 November 2020)
• Windows: prevented a crash that could occur in the Sound window when clicking with the touchpad while dragging with the mouse (or the reverse).

##6.1.29# (27 October 2020)
• Mac: corrected a bug revealed by Xcode 12 since 6.1.22 that could cause Praat to crash when using empty vectors,
  such as when querying a Harmonicity for its mean in regions without frames.

##6.1.28# (20 October 2020)
• Pause forms: text fields can be multi-line.
• Removed a bug that caused Praat to crash when editing a PitchTier.
• Script window: got rid of too many history entries after dragging in the Picture window.

##6.1.27# (13 October 2020)
• Demo window: make sure that the contents of the window become visible
  more often without calling demoShow() or demoWaitForInput().

##6.1.26# (5 October 2020)
• Windows: removed a bug that caused Praat to crash if you clicked in the Sound window,
then dragged the mouse out of that window, then released the mouse button, and then clicked in the Sound window again.
• Linux: removed a bug that caused Praat to crash when clicking Change in the Inspect window.

##6.1.25# (4 October 2020)
• @FormantPath and @FormantPathEditor.
• Windows: implemented vertical scrolling with the mouse wheel in the manual and in the Picture window.
• Linux: removed a bug that caused Praat to crash when double-clicking in the Sound window.
• Linux: removed a bug that caused Praat to crash when raising the About window twice.
• Removed a bug that caused Praat to crash when zooming in
  to a region in the Sound window without samples, when a pitch curve was visible.
• Removed a bug that caused Praat to crash when pasting in
  a region in the Sound window without samples.

##6.1.24# (29 September 2020)
• Linux: got rid of flashing during a running cursor or
  when making a selection in the Sound window or the Picture window.

##6.1.23# (28 September 2020)
• Windows: got rid of flashing during a running cursor or
  when making a selection in the Sound window or the Picture window.
• Linux: corrected a bug that caused the buttons at the top of a manual window
  to be overwritten with text when scrolling.

##6.1.22# (24 September 2020)
• Windows: made the running cursor visible again.
• Corrected a bug introduced in 6.1.17alpha that could cause incorrect line spacing in the Picture window,
  such as in ##TableOfReal: Draw as numbers#.

##6.1.21# (20 September 2020)
• First fully functional version for BigSur.
• Removed a decades-old bug by which an extremum allegedly computed by cubic interpolation
  would actually have been computed by sinc700.

##6.1.20beta# (10 September 2020)
• Second beta version for macOS Big Sur.

##6.1.19beta# (7 September 2020)
• First beta version for macOS Big Sur.

##6.1.18alpha# (1 September 2020)
• Second alpha version for macOS Big Sur.

##6.1.17alpha# (16 August 2020)
• Alpha version for macOS Big Sur.

##6.1.16# (6 June 2020)
• @@Record Sound (fixed time)...@: more reliable choice of input device.
• Mac: notice plugging and unplugging of headphones.

##6.1.15# (20 May 2020)
• Repaired a bug introduced in 6.0.44 that could cause an incorrect (namely, totally constant) ClassificationTable.

##6.1.14# (2 May 2020)
• Repaired a bug in drawing ranges introduced in 6.1.06.

##6.1.13# (19 April 2020)
• Repaired discriminant analysis on i386 processors.
• Scripting: enabled predictability by setting random seeds.
• Automated batch testing on x86_64, i386 and ARMv7 processors.

##6.1.12# (13 April 2020)
• All non-GUI tests can be automated from batch.
• Repaired a bug introduced in 6.0 whereby a KlattGrid-generated Sound would always be peak-scaled.

##6.1.11# (6 April 2020)
• Better messages.
• Overhauled VowelEditor.
• Repaired crashes after creating a Formant from a Spectrum or an Excitation.

##6.1.10# (23 March 2020)
• Faster formant computation.
• More LPC queries.

##6.1.09# (26 January 2020)
• Accept some more kinds of broken WAV files.
• Improved box plot drawing.
• More Network commands.

##6.1.08# (5 December 2019)
• Repaired a bug with automatic alignment for TextGrids (introduced in 6.1.07).

##6.1.07# (26 November 2019)
• Raspberry Pi edition.

##6.1.06# (8 November 2019)
• Chromebook edition.
• Scripting: colour fields now take name, grey value or RGB vector.

##6.1.05# (16 October 2019)
• Repaired a bug introduced in 6.0.44 that could cause rubbish LPC smoothing.
• Repaired a rare crash when dragging a selection on the Mac.

##6.1.04# (28 September 2019)
• @Electroglottography.
• Sound and other windows: ##Widen or shrink selection...#.
• @@Create KlattGrid from vowel...|KlattGrid: Create from vowel...@.
• Fix rectangle playing bug when selection viewer is on.
• Selection viewer separately visible for Sound window and TextGrid window.
• Scripting: allow comments after parameter list.
• Scripting: fix CR/LF pasting from Microsoft Word.

##6.1.03# (1 September 2019)
• Sound window: ##Widen or shrink selection...#.

##6.1.02# (25 August 2019)
• Repaired @@Sounds: Concatenate with overlap...@.
• Mac: Info and script windows: prevent line breaks caused by tab stops.

##6.1.01# (14 August 2019)
• Repaired ##TextGrid: Replace interval texts...#.

################################################################################
"What was new in 6.1?"
© Paul Boersma 2015–2019

##6.1# (13 July 2019)

##6.0.57# (1 July 2019)
• Repaired a crash when an editor window zoomed in to a zero time range.
• Repaired a crash when playing a matrix movie with one row or a constant value.
• Repaired a crash when reconstructing a TableOfReal from a PCA.
• Scripting: allow comments after `else`, `endif`, `endfor`...

##6.0.56# (20 June 2019)
• Windows: file dropping on the Praat icon works for higher-Unicode file names.
• SpellingChecker: Unicode support.

##6.0.55# (13 June 2019)
• Unicode normalization in file names.

##6.0.54# (6 June 2019)
• Removed a bug introduced in 6.0.51 that could cause incorrect axes in Demo window.

##6.0.53# (26 May 2019)
• Much faster playing of short parts of long sounds that need resampling.
• Better handling of broken CSV files.
• 64-bit floating-point WAV files.

##6.0.52# (2 May 2019)
• Removed a bug introduced in 6.0.51 that could cause turning a Discriminant into a Configuration to crash.
• Removed a bug introduced in contour grey drawing in August 2017.

##6.0.51# (29 April 2019)
• Script window: Use Selection for Find.
• Removed a bug introduced in 6.0.41 that could cause Praat to crash after removing an element from a Strings or a row from a TableOfReal.

##6.0.50# (31 March 2019)
• Manual updates, speed, more tests.
• Scripting: @`rowSums#` (), @`columnSums#` (); @`randomGauss##` () finally with four arguments.

##6.0.49# (2 March 2019)
• Removed a bug introduced in 6.0.41 whereby a script could misreport an undefined table value.
• Removed a bug introduced in 6.0.44 whereby an MFCC's maximum frequency could be ignored.
• Pitch: Tabulate candidates.
• SoundSet.

##6.0.48# (17 February 2019)
• Removed a bug introduced in 6.0.44 whereby Praat could crash when drawing a function without any points.
• Removed a bug whereby Praat would not start up on macOS 10.10 (because of required GPU libraries).

##6.0.47# (8 February 2019)
• Removed a bug introduced in 6.0.44 whereby a cepstral peak prominence would be computed incorrectly.

##6.0.46# (3 January 2019)
• Removed a crashing bug in the LongSound window introduced in 6.0.44.
• Removed a bug causing incorrect conversion from Formant to LPC introduced in 6.0.44.
• More space for actions in buttons window.

##6.0.45# (1 January 2019)
• Removed a crashing bug in EditDistanceTable introduced in 6.0.44.
• Removed a crashing bug in KlattGrid drawing introduced in 6.0.44.
• Removed a bug introduced in 6.0.44 whereby old HMM files were read incorrectly.

##6.0.44# (31 December 2018)
• Bug fixes and speed.
• Bug fixes and speed.

##6.0.43# (8 September 2018)
• Removed a very old bug that could create invalid TextGrids during forced alignment.
• Windows: repaired a bug introduced in 6.0.41
  that caused a crash in several places (such as when changing the sample rate in the SoundRecorder)
  when Praat had been started with a double click.
• Windows: repaired a bug introduced in 6.0.41 that caused Praat to crash when exiting.

##6.0.42# (15 August 2018)
• Repaired a bug introduced in 6.0.41 that caused a crash in pause windows.

##6.0.41# (6 August 2018)
• Interoperability: the Windows edition can now communicate with other software in UTF-8.

##6.0.40# (11 May 2018)
• Scripting: $$@@Scripting 4.3. Querying objects|`selected#`@$.
• Table window: cell selection.
• Unicode-savvy regular expressions.

##6.0.39# (3 April 2018)
• More string matching (case-insensitive, word-level).

##6.0.38# (29 March 2018)
• Concordance: ##TextGrid: Tabulate occurrences...#.
• Some @@Independent Component Analysis on EEG@.
• Removed many small bugs.

##6.0.37# (3 February 2018)
• Graphics: fixed crashing bug with \bsun trigraph (\un).

##6.0.36# (11 November 2017)
• Many more languages for SpeechSynthesizer and automatic alignment (new eSpeak).

##6.0.35# (16 October 2017)
• Fixed a bug introduced in 6.0.23 by which ##Remove right boundary# would sometimes incorrectly refuse.

##6.0.34# (10 October 2017)
• Scripting: arrays in menu commands in scripts.
• #Inspect: made a button visible that had disappeared in 6.0.33.

##6.0.33# (26 September 2017)
• ##Pitch: Subtract linear fit...#: correct a bug that prevented computation in the ERB domain.
• Scripting: made `object[xx].xmin` work again.
• Removed a bug in ##Polynomial: Evaluate...# introduced in 6.0.32.

##6.0.32# (16 September 2017)
• Table: allow spaces in column names.
• Settings windows no longer replace your visible expressions with their resulting values.
• Scripting: formulas using the tilde.
• Optimality Theory learning: corrected a bug introduced in 6.0.30 that could cause the editor to crash on the Mac.
• EEG: corrected a bug that could cause an infinite loop when doing principal component analysis.
• Scripting: faster interpreter.

##6.0.31# (21 August 2017)
• Scripting: more vectors and matrices.
• Numerics: faster and more precise sums, means, standard deviations.

##6.0.30# (22 July 2017)
• Removed a bug that caused an incorrect title for a PitchTier or PointProcess window.
• Removed a bug that caused Praat to crash when doing a linear regression on a Table with no rows.
• Scripting: `object[]`, @@Scripting 5.7. Vectors and matrices|vectors@.
• Graphics: better text drawing details.
• Linux: possibility to compile Praat without a GUI but with graphics file output.

##6.0.29# (24 May 2017)
• Sound window: channel muting.
• Linux: support for Chinese, Japanese, Korean, Indic, Arabic and Hebrew characters in TextGrids and elsewhere.

##6.0.28# (23 March 2017)
• Scripting: @`demoPeekInput` () for animations in combination with @`sleep` ().

##6.0.27# (18 March 2017)
• TextGrid: fewer error messages in concatenation of multiple TextGrids.
• Scripting: @`sleep` () to pause Praat temporarily: useful for animations in combination with @`demoShow` ().

##6.0.26# (2 March 2017)
• Mac: more corrections in @`demoShow` () and `blankWhilePlaying`.
• PraatBarren: better error message when an attempt is made to run PraatBarren interactively.

##6.0.25# (11 February 2017)
• Mac: made @`demoShow` () and `blankWhilePlaying` compatible with MacOS 10.12 Sierra.
• Mac SoundRecorder: more sampling frequencies, on behalf of external USB microphones.

##6.0.24# (23 January 2017)
• Fixed a bug introduced in 6.0.23 by which ##Remove right boundary# would choose the wrong tier.
• TextGrid window: click to insert a phonetic symbol from an IPA chart.

##6.0.23# (12 December 2016)
• Linux: fixed a bug that caused Praat to crash when playing a sound of more than 7 channels.
• Change Gender: fixed a bug introduced in 6.0.22 by which the pitch range factor could not be 0.
• Improvements in the manual and in texts.

##6.0.22# (15 November 2016)
• Scripting: correct error messages for expressions like: `5 + “hello”`
• Command line: the --open option works correctly in the GUI if you open multiple files.

##6.0.21# (25 September 2016)
• ExperimentMFC: you can now specify font sizes and response keys for goodness judgments.
• Table: when drawing ellipses, a font size of 0 can now be used to prevent drawing the labels.
• Mac: dragging selections repaired for System 10.11.6 (but we advise to upgrade to 10.12).
• Mac: re-enabled Return key for confirming some dialog boxes.

##6.0.20# (3 September 2016)
• Can open UTF-8 text files with Byte Order Mark.
• Scripting improvement: function names can now be used as names of indexed variables.

##6.0.19# (13 June 2016)
• Mac: dragging selections repaired for System 10.11.5.

##6.0.18# (23 May 2016)
• Windows: better dotted lines.
• TextGrid window: again better automatic alignment.

##6.0.17# (21 April 2016)
• TextGrid window: better automatic alignment.

##6.0.16# (5 April 2016)
• Scripting: “hashes”: variables can now be indexed with strings rather than only with numbers.
• TextGrid window: fewer out-of-order messages in automatic alignment.

##6.0.15# (21 March 2016)
• TextGrid window: removed a bug whereby Praat could do automatic alignment only on sounds sampled at 44.1 kHz.
• TextGrid window: improved the location of the final boundary in automatic alignment.
• Table window: added a preference for the interpretation of graphical style symbols (`%#^_@`).

##6.0.14# (11 February 2016)
• Linux: corrected a bug by which the Tab short-cut for playing a sound would not work if the NumLock key was on.
• Mac 64-bit: corrected a bug that could cause Praat to crash if the Tab key was pressed in a dialog window.

##6.0.13# (31 January 2016)
• Windows: corrected a bug that could cause Praat to crash when closing a script window.
• Mac 64-bit: progress bars with movies.

##6.0.12# (24 January 2016)
• Windows 32-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.
• Mac 64-bit: better @`demoShow` ().
• Mac 64-bit: working Matrix movies and articulatory synthesizer movies.

##6.0.11# (18 January 2016)
• Corrected a bug that could cause Praat to crash when drawing a Spectrum with undefined values.
• Mac 64-bit: corrected a bug by which some text in the Sound window would sometimes not be drawn.
• Mac 64-bit demo window: improved the working of @`demoShow` ().

##6.0.10# (8 January 2016)
• Corrected a bug that would cause Praat to crash when starting an ExperimentMFC window.
• Mac 64-bit: corrected a bug that would cause black selections in the Sound window.

##6.0.09# (1 January 2016)
• Windows 64-bit: corrected a bug that could cause Praat to crash when closing the Sound window after popping up dialog boxes.

##6.0.08# (5 December 2015)
• Windows command line: the `--a` option can be used when redirecting the output to pipes and files.
• Linux command line: the `--run` option is not needed in the Terminal when redirecting output
  (unless you redirect standard output, standard input %and standard error).

##6.0.07# (30 November 2015)
• Mac 64-bit: repaired dragging selections (which got broken in the El Capitan fix in 6.0.06).

##6.0.06# (29 November 2015)
• Mac 64-bit: correct early Info window updates and @`demoShow` () in El Capitan.
• Manual updates.
• Corrected some small memory leaks.

##6.0.05# (8 November 2015)
• A manual page that describes @@TextGrid file formats@.
• Corrected a bug that prevented some KlattGrid tiers from being edited.
• 32-bit Linux: correct use of PulseAudio (note: 6.0.04 on Debian already had this correct).

##6.0.04# (1 November 2015)
• Corrected a bug that caused an incorrect number in FFNet files;
  unreadable FFNet files can be sent to the Praat authors for repair.

##6.0.03# (31 October 2015)
• Corrected a bug that caused Praat to crash when the Soundrecorder window was closed.
• Scripting: when calling Praat from the command line, added the `--run` option to force script execution.

##6.0.02# (30 October 2015)
• Corrected the redrawing of analyses in the Sound window after a change in the sound.

##6.0.01# (29 October 2015)
• Windows: fixed a bug that caused Praat to crash at start-up in some Windows versions.

################################################################################
"What was new in 6.0?"
© Paul Boersma 2014,2015

##6.0# (28 October 2015)
• Linux: use PulseAudio as the server for playing sound.
• Windows: removed `Praatcon.exe`, because Praat.exe can now be used from the command line.
• Praat can now open files from the command line on all platforms.
• The API change that requires raising the major Praat version to 6:
  no longer any need for `Praatcon.exe`, because Praat itself can now run a script from the command line on all platforms.
• See @@Scripting 6.9. Calling from the command line@ for all command line options.
• SpeechSynthesizer: corrected a potentially crashing bug.
• Mac Retina graphics: made function drawing (e.g. in the TextGrid window) twice as fast.
• LongSound: corrected playing of more than 2 channels.

##5.4.22# (8 October 2015)
• Sound files: added support for MP3 files with ID3 version 2 headers.
• @@Table: Line graph where...@: removed a bug that caused nothing to be drawn if the number of columns was larger than the number of rows and
the column index was larger than the number of rows.
• Mac: corrected horizontal spacing in graphical text on El Capitán.
• Mac: corrected a bug that would cause Praat to crash if a PNG file could not be saved (from a script).
• Windows: enabled PNG saving from Praatcon.

##5.4.21# (29 September 2015)
• Corrected a bug introduced in 5.4.20 that could cause the Sound window to crash.
• Repaired a very old hang with pulses in the Sound window.

##5.4.20# (26 September 2015)
• Corrected the graphical representation of numbers with exponents (wrong since 5.4.10).
• Windows: prevented flashing console window in @`runScript` ().

##5.4.19# (16 September 2015)
• LongSound: corrected saving of 8-bit files as 16-bit.

##5.4.18# (7 September 2015)
• Regular expressions: if the pattern is illegal, you'll now get an error message instead of an empty string.
• LongSound: removed a bug introduced in 1999 that could play long stereo sounds much too fast if they had unusual sampling frequencies.
  This bug has become more noticeable on the Mac since January 2015.
• Guard better against sound files that contain 0 samples.
• Scripting: faster look-up of variables.

##5.4.17# (20 August 2015)
• Scripting: removed a bug that could cause Praat to crash when calling `writeFile` () with an empty text.

##5.4.16# (16 August 2015)
• Mac 64-bit: removed a bug by which graphical text passed over the right edge in TextGrid intervals.
• Mac 64-bit: removed a bug that allowed the general #Zoom and #Close commands to be accessible from a script.

##5.4.15# (1 August 2015)
• Introduced several command line options that make it easier to call Praat from a web server.
• 64-bit Mac: removed a bug introduced in 5.4.11 that caused incorrect spacing of vertical text in picture files.

##5.4.14# (24 July 2015)
• Windows and 32-bit Mac: removed a bug introduced in 5.4.12 that could cause weird behavior (even crashing) when
  pressing the Tab key in the script window or in the manual.

##5.4.13# (22 July 2015)
• The Demo window can now save itself to a PNG or PDF file.
• Windows: removed a bug introduced in 5.4.10 whereby Praatcon would write garbled text to the console.

##5.4.12# (10 July 2015)
• Windows: removed a bug introduced in 5.4.10 whereby SpeechSynthesizer would only pronounce the first letter of the text.

##5.4.11# (8 July 2015)
• Windows: removed a bug introduced in 5.4.10 whereby the file selection window could try to open a file with a garbled name.
• Removed a bug whereby the PointProcess window would crash when showing a perfectly silent sound.

##5.4.10# (27 June 2015)
• Removed a bug introduced in version 4.5.09 (January 2007) that caused incorrect
  upsampling of stereo sounds when the upsampling factor was exactly 2. This bug has caused
  incorrect playing of stereo sounds with a sampling frequency of 22050 Hz on the Mac since January 2015.
• Removed a bug introduced in 2011 that could cause Praat to crash when you scrolled the LongSound window.
• ##TextGrid: Count intervals where...# and ##TextGrid: Count points where...#.

##5.4.09# (1 June 2015)
• Linux: the tab key can be used to play sounds on more computers.
• Windows: TextGrid files with non-BMP characters are now read correctly.
• Windows: files with names that contain non-BMP characters are now saved correctly.
• Updated manual.

##5.4.08# (24 March 2015)
• Sound window: removed a bug that caused a crash in ##Editor info#.

##5.4.07# (22 March 2015)
• TextGrid window: ##Add interval# now does the same thing as ##Add boundary# if a time stretch is selected.
• Linux: better redrawing in TextGrid window.

##5.4.06# (21 February 2015)
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 200 dpi.

##5.4.05# (13 February 2015)
• Better support for big integer numbers on Windows XP.
• Sound window: guarded against empty view ranges for the intensity curve.

##5.4.04# (28 December 2014)
• Windows audio playback: if the sound has more channels than the audio hardware, distribute them evenly.

##5.4.03# (18 December 2014)
• TextGrid reading: Praat now corrects some incomplete TextGrid files created by others.
• Better support for text files larger than 2 GB.

##5.4.02# (26 November 2014)
• Mac: repaired a bug by which quote characters typed into the script window could become curly instead of straight.

##5.4.01# (9 November 2014)
• @MelSpectrogram, @BarkSpectrogram.
• Linux: removed a bug that could cause too many flashes when scrolling a Sound window.
• Mac: repaired a bug that could cause a crash in the @VowelEditor window.

################################################################################
"What was new in 5.4?"
© Paul Boersma 2011–2014

##5.4# (4 October 2014)

##5.3.87# (3 October 2014)
• Windows scripting: prevented incorrect handling of relative paths after the use of @`chooseReadFile$`.
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 180 or 1200 dpi.

##5.3.86# (28 September 2014)
• Linux audio: reverted to old version of PortAudio because of race problems in the Alsa–PulseAudio cooperation.

##5.3.85# (19 September 2014)
• Mac audio: circumvented a bug in PortAudio by which playback of sounds with sampling frequencies
  below 44100 Hz could be distorted on some USB headsets.

##5.3.84# (26 August 2014)
• Manipulation: repaired a bug that could cause Praat to crash when playing a manipulation of a Sound created by the @SpeechSynthesizer.

##5.3.83# (16 August 2014)
• TextGrid window: repaired a bug with automatic alignment that could cause an analysis tier to go out of order.
• Linux audio: created a second workaround that reduces even more the chances of a freeze that is due to a potential deadlock
  in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.
• Smoother communication with Phon.
• Windows: repaired a memory leak when saving PNG files.

##5.3.82# (26 July 2014)
• Linux and Mac audio playback: if the sound has more channels than the audio hardware, distribute them evenly.
• Pause forms: more consistent appearance of the Revert button.
• Scripting: @`pauseScript` () function.

##5.3.81# (2 July 2014)
• EEG: can work with status %numbers instead of only with status %bits.
• Windows: repaired a bug that could cause Praat to crash if there was a 96-dpi printer.

##5.3.80# (29 June 2014)
• Praat preferences: choice between Chinese and Japanese style for Han characters.

##5.3.79# (21 June 2014)
• Can now play sounds over more than two channels.
• Asynchronous play in scripts (see @@Demo window@).
• EEG: blue-to-red colour scale for scalp distributions.

##5.3.78# (12 June 2014)
• Multithreading can now speed up pitch analysis by a factor of 4 or so,
  depending on the number of cores in your processor.
• Linux: can now open and save Photo objects (from PNG files)
  and use @@Insert picture from file...@.
• Open WAV files that are in the “extensible” format (previously “unsupported format -2”).
• Windows: support for dropping more than one file on the Praat icon.
• Scripting: can now use the #editor command with an object’s ID instead of only with its name.
• Windows: removed a bug that sometimes disallowed saving more than one JPEG file.
• Linux audio: created a workaround that reduces the chances of a freeze that is due to a potential deadlock
  in the collaboration between Alsa and PulseAudio that can occur when the playback of a sound is cancelled.

##5.3.77# (18 May 2014)
• EEG: more facilities for EDF+ files.

##5.3.76# (8 May 2014)
• One can determine the size of “speckles” (filled circles) with ##Speckle size...# in the #Pen menu.
  Speckles are used in drawing Formant, PitchTier, and several other kinds of objects.

##5.3.75# (30 April 2014)
• Linux Matrix graphics bug fix: corrected working of ##Matrix: Draw cells...#.
• Scripting bug fix: ability to use x and y as indexed variables.
• PowerCepstrogram bug fix: made old version of Paint command available again for scripts.

##5.3.74# (24 April 2014)
• EEG: more interpretation of triggers in EDF+ files.

##5.3.73# (21 April 2014)
• EEG: understand more EGI/NetStation files.

##5.3.72# (17 April 2014)
• Windows: repaired a bug that caused two black edges in PNG files.
• Windows: repaired a bug that could cause Praat to crash if a metafile resolution was 360 dpi.
• Linux: repaired a bug that caused Praat to crash when cutting or pasting a sound in the Sound window.

##5.3.71# (9 April 2014)
• Windows: brought more unity in the style of Chinese characters.

##5.3.70# (2 April 2014)
• Added some query commands for DurationTier objects.
• Repaired a bug that caused Praat not to run as a console app.

##5.3.69# (28 March 2014)
• Picture window: can save to 300-dpi and 600-dpi PNG files.
• Graphics: sub-pixel precision line drawing on Mac and Linux.
• Repaired a bug that could show spurious buttons in the Objects window if a plug-in created objects.

##5.3.68# (20 March 2014)
• Mac: corrected a bug introduced in 5.3.67 that could cause crashes when drawing a spectrogram.
• Mac and Linux: @@Create Strings as file list...@ handles broken symbolic links more leniently.

##5.3.67# (19 March 2014)
• Corrected a bug that would create strange PNG files if the selection did not touch the upper left corner of the Picture window.
• Mac: can save the Picture window to PNG file.
• EEG: understand trigger letters in BDF/EDF files.

##5.3.66# (9 March 2014)
• Windows and Linux: can save the Picture window to PNG file.
• Windows: opening, modifying and saving PNG, TIFF or JPEG files (the Photo object, as on the Mac).

##5.3.65# (27 February 2014)
• Scripting language: removed some bugs from @`runScript`.
• Linux: can save the Picture window to PDF file.

##5.3.64# (12 February 2014)
• Scripting language: @`writeInfo`, @`procedure`, @`exitScript`, @`runScript`: all with colons.
• 64-bit Mac graphics: better highlighting and unhighlighting of selection.
• 64-bit Mac graphics: full screen.

##5.3.63# (24 January 2014)
• Scripting language: easier menu command invocation using the colon “`:`”.
• 64-bit Mac graphics: better handling of any absence of Doulos SIL or Charis SIL.
• Windows scripting: can now use “`~`” in file names to refer to the home directory, as on Mac and Linux.

##5.3.62# (2 January 2014)
• 64-bit Mac: removed a bug introduced in 5.3.61 that could cause text containing “ff” to become invisible.

##5.3.61# (1 January 2014)
• EEG: understand status registers that contain text.
• KlattGrid: removed a bug introduced in May 2009 that could make Praat crash after editing an oral formant grid.

##5.3.60# (8 December 2013)
• Mac 64-bit: implemented swiping (to scroll with the trackpad) and pinching (to zoom with the trackpad).
• Scripting: @`backslashTrigraphsToUnicode$` () and @`unicodeToBackslashTrigraphs$` ().

##5.3.59# (20 November 2013)
• EEG: faster reading of BDF and EDF files.
• Batch scripting: made @`appendInfo` () write to the console in the same way as #`print`.
• Removed a bug introduced in 5.3.57 whereby some Praat text files could not be read.

##5.3.58# (17 November 2013)
• EEG: support for 16-bit (next to 24-bit) BDF files and for 16-bit (next to 8-bit) statuses.
• Mac: 64-bit beta version.

##5.3.57# (27 October 2013)
• Mac: opening, modifying and saving image files (the Photo object).
• Mac 64-bit: some small improvements in the user interface.

##5.3.56# (15 September 2013)
• Mac: 64-bit alpha version.
• Linux: improved selecting in the Picture window.

##5.3.55# (2 September 2013)
• Corrected a bug introduced in 5.3.54 by which you couldn't select a file for saving.

##5.3.54# (1 September 2013)
• Sound window: removed a bug introduced in 5.3.42 by which you couldn't ask for an odd number of poles in Formant Settings
  (by e.g. specifying “5.5” for the number of formants).
• Linux: improved dragging of selections in the Picture window and the Sound window.

##5.3.53# (9 July 2013)
• Table: more drawing commands.

##5.3.52# (12 June 2013)
• Scripting: editor windows understand #`do` and #`do$`.

##5.3.51# (30 May 2013)
• Sound window: ##Extract selected sound for overlap...#.

##5.3.49# (13 May 2013)
• TextGrid window: alignment of the sound and the annotation in an interval, via eSpeak.
• Scripting: repaired a bug introduced in 5.3.32 that could cause very slow running of scripts.

##5.3.48# (1 May 2013)
• Scripting: variable-substitution-free object selection and file writing.
• Scripting: @`selectObject` and @`removeObject` can select or remove multiple objects at a time.

##5.3.47# (23 April 2013)
• OTGrammar: included Giorgio Magri's (2012) update rule (weighted all up, high down).

##5.3.46# (21 April 2013)
• Scripting: variable-substitution-free procedure calls.
• Linux: made the Save menu compatible with Ubuntu 12.04.

##5.3.45# (15 April 2013)
• More parts of the manual reflect variable-substitution-free scripting.

##5.3.44# (7 April 2013)
• @@Create Sound as pure tone...@.
• First steps towards variable-substitution-free scripting: the #`do`, #`do$`, @`writeInfo` and @`appendInfo` functions.

##5.3.43# (27 March 2013)
• Read and write stereo Kay sound files.
• Phonetic symbols \ts, \tS and \ap.
• Network: ##Normalize weights...#, ##List weights...#, ##Weights down to Table...#.

##5.3.42# (2 March 2013)
• Repaired some minor bugs regarding synchronization between multiple windows.

##5.3.41# (9 February 2013)
• Linux: repaired a bug that could cause Praat to crash when closing the Info window or a script window.

##5.3.40# (2 February 2013)
• Windows: better handling (in the LongSound window) of sounds that are too long to play.
• Mac: corrected a bug that caused Praat to crash when closing the Strings window.
• Linux: made audio output work again on 32-bit Linux.

##5.3.39# (6 January 2013)
• @@Formant & Spectrogram: To IntensityTier...@.

##5.3.38# (4 January 2013)
• ExperimentMFC: repaired a bug introduced in 5.3.36 that caused sound to fail to play completely if the screen did not blank.

##5.3.37# (2 January 2013)
• ExperimentMFC: after screen blanking, reaction times count from when the response buttons appear.

##5.3.36# (1 January 2013)
• ExperimentMFC: made it possible to blank the screen while the sound is playing.

##5.3.35# (8 December 2012)
• SpeechSynthesizer: made it possible to have more than one at a time.
• Linux: corrected a bug that caused incorrect alignment of the buttons in the Objects window.

##5.3.34# (21 November 2012)
• Windows: corrected a bug that caused incorrect alignment of the buttons in the Objects window.
• The new @@Sounds: Combine to stereo@ can now work with Sounds of different durations, as the old could.
• Corrected a bug that caused Praat to crash when creating a SpeechSynthesizer.

##5.3.33# (20 November 2012)
• @@Sounds: Combine to stereo@ can now combine any number of Sounds into a new Sound whose number of channels
  is the sum of the numbers of channels of the original Sounds.
• ERP: Down to Sound.

##5.3.32# (17 October 2012)
• Sound window: corrected the working of the ##by window# scaling option.

##5.3.31# (10 October 2012)
• ERP: Down to Table...
• Linux: corrected a bug that could cause Praat to crash after closing a file selector window for saving.

##5.3.30# (6 October 2012)
• Circumvented a rare Windows bug that could cause Praat to start to write 1.5 as 1,5 in some countries
  after opening a strange directory.
• Windows: corrected a bug introduced in 5.3.24 that could cause Praat to crash when quitting.
• Windows: corrected a bug introduced in 5.3.24 that could cause the Objects window to look weird after resizing.

##5.3.29# (30 September 2012)
• @EEG: Draw scalp..., including gray legend.
• Made the new Sound scaling options available for LongSounds.

##5.3.28# (28 September 2012)
• Corrected a bug introduced in 5.3.24 that made Praat crash on OSX 10.5.

##5.3.27# (27 September 2012)
• Corrected a couple of small bugs.

##5.3.26# (26 September 2012)
• Corrected a bug introduced in 5.3.24 that prevented Praat from running in batch mode.

##5.3.25# (26 September 2012)
• EEG: view of scalp distribution takes scaling options into account.
• Linux: swipable Sound window and manual.
• Linux: corrected a bug introduced in 5.3.24 whereby TextGrids could not be edited.

##5.3.24# (24 September 2012)
• More kinds of vertical scaling in e.g. Sound and EEG windows.
• Rewritten user interface (may contain some bugs).

##5.3.23# (7 August 2012)
• Mac: removed a bug introduced in September 2011 that could cause incorrect behaviour of regular expressions since July 2012.

##5.3.22# (21 July 2012)
• Linux: removed a bug introduced in 5.3.21 that could cause Praat to crash when opening files.
• Neural networks: correct writing and reading of learning settings.

##5.3.21# (10 July 2012)
• Linux: better folder choice in file selector window for opening files.
• Repaired a bug that caused Praat to crash when opening a ManPages file on some platforms.

##5.3.20# (5 July 2012)
• @EEG: ##Replace TextGrid#.

##5.3.19# (24 June 2012)

##5.3.18# (15 June 2012)
• Corrected a bug in @@Sound: Change gender...@ that caused a part of the sound not to be changed.

##5.3.17# (12 June 2012)
• @EEG window: extent autoscaling by window.
• ERPTier: Remove events between...

##5.3.16# (23 May 2012)

##5.3.15# (10 May 2012)
• Improvements in clipboards, PDF and EPS files.

##5.3.14# (28 April 2012)
• Linux: Tab shortcut for playing the selection.
• EPS files: higher minimum image resolution for spectrograms (300 instead of 106 dpi).

##5.3.13# (11 April 2012)
• @EEG: Extract part...

##5.3.12# (5 April 2012)
• Praat picture file: allow larger function drawings (up to 10^9 instead of 10^6 points).
• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing LongSounds).

##5.3.11# (27 March 2012)
• @EEG: a selected ERPTier can extract events on the basis of a column of a selected Table.

##5.3.10# (12 March 2012)
• @EEG: Concatenate.

##5.3.09# (10 March 2012)
• Better text-to-speech.

##5.3.08# (5 March 2012)
• Removed a bug introduced in 5.3.07 that could cause Praat to crash when viewing a LongSound.

##5.3.07# (4 March 2012)
• Praat can now save Sounds as 24-bit and 32-bit WAV files.

##5.3.06# (28 February 2012)

##5.3.05# (19 February 2012)
• SpeechSynthesizer (#New menu \-> #Sound): text-to-speech.
• @EEG: better scalp distribution drawing for both 32-channel and 64-channel recordings.

##5.3.04# (12 January 2012)
• EEG: draw scalp distribution.
• Linux: better audio compatibility with Ubuntu 11.10 (support for unusual sampling frequencies when playing Sounds).

##5.3.03# (21 November 2011)
• EEG: filtering, editing, more viewing.

##5.3.02# (7 November 2011)
• Corrected a bug introduced in 5.3.01 that could cause the PitchTier or PointProcess window to crash.
• Corrected a bug that could cause the Info window to freeze after an error message in a script window.

##5.3.01# (1 November 2011)
• Macintosh and Windows: better window positioning if the Dock or Task Bar is on the left or right.
• IPA symbol: you can now use \bs.f for the half-length sign (\.f).
• EEG window.

################################################################################
"What was new in 5.3?"
© Paul Boersma 2010,2011

##5.3# (15 October 2011)

##5.2.46# (7 October 2011)
• Corrected the same very old bug as in 5.2.44, but now also for opening and saving files.
• Many better messages.

##5.2.45# (29 September 2011)
• Spectrum window: ##Move cursor to nearest peak#.
• @@Table: Save as comma-separated file...@.
• Windows: you can now change the font size of the script window.
• Windows scripting: the Info window now updates while you are writing to it.
• Windows: error messages now stay at the front so that you never have to click away
  an invisible message window anymore.

##5.2.44# (23 September 2011)
• Corrected a very old bug in the history mechanism (the button title wouldn't always show up).

##5.2.43# (21 September 2011)
• Linux: you can now change the font size of the script window.
• Corrected a bug that could cause Praat to crash when opening an already open file in the script window.

##5.2.42# (18 September 2011)
• Corrected a bug introduced in 5.2.36 that prevented formulas from working on FormantGrid objects.

##5.2.41# (17 September 2011)
• Improved dashed-dotted lines in EPS files.
• Corrected a bug introduced in 5.2.36 that caused Praat to crash when running an ExperimentMFC.

##5.2.40# (11 September 2011)
• Corrected a bug in the 64-bit Windows edition that led to an incorrect location for the preferences files.

##5.2.39# (10 September 2011)
• 64-bit edition for Windows.
• Corrected a bug that caused Praat to crash instead of saying “undefined” when reporting the shimmer of a sound with fewer than 3 pulses.

##5.2.38# (6 September 2011)
• Corrected several bugs that were introduced in 5.2.36 in the Inspect window.

##5.2.37# (2 September 2011)
• Graphics: added the Chinese phonetic symbols \id and \ir. See @@Phonetic symbols: vowels@.
• Corrected a bug introduced in 5.2.36 that caused Praat to crash when querying formant quantiles.

##5.2.36# (30 August 2011)
• Graphics: added superscript diacritics as single symbols: \^h (\bs\^ h), \^j (\bs\^ j),
and many more; also \_u (\bs\_ u). See @@Phonetic symbols: diacritics@.
• Praat fully converted to C++ (this may initially cause some bugs, but will be more reliable in the end).

##5.2.35# (5 August 2011)
• Corrected the menus of the Sound and TextGrid windows.

##5.2.34# (3 August 2011)
• @@Insert picture from file...@ now works on Windows (as well as on the Mac).
• Corrected a bug that could cause Praat to crash when playing a LongSound.

##5.2.33# (29 July 2011)
• Improved the reliability of sound playing and several other things.
• Improved several error messages.

##5.2.32# (22 July 2011)
• Corrected several problems with error messages.
• Corrected a bug that could cause slow execution of scripts with forms.

##5.2.31# (21 July 2011)
• Corrected a bug that caused an incorrect window size in the VowelEditor.
• Corrected a bug that caused incorrect error messages when opening a FLAC file as a LongSound.
• Sound window: corrected a bug that could cause Praat to crash when zooming in on the left or right edge.

##5.2.30# (18 July 2011)
• Corrected a bug introduced in 5.2.29 whereby the list of object actions could stay empty after an error message in a script.
• Corrected a bug in Klatt synthesis whereby the generation of a sound could be refused.

##5.2.29# (12 July 2011)
• More accurate error checking, due to Praat's conversion to C++ (last C-only version was 5.2.17).

##5.2.28# (28 June 2011)
• Corrected some Polygon bugs.

##5.2.27# (19 June 2011)
• @Polygon: ##Draw (closed)...#.
• @@PointProcess: To Sound (phonation)...@:
corrected a bug that could cause Praat to crash if pulses lay outside the time domain.

##5.2.26# (24 May 2011)
• Corrected a bug that could cause Praat to crash on some platforms when reading a Praat binary file.
• @ExperimentMFC: corrected a bug that caused Praat to crash if an experiment contained zero trials.
• Corrected a bug that caused Praat to crash when merging multiple IntervalTiers.

##5.2.25# (11 May 2011)
• OT learning: corrected a crashing bug from ##Get candidate...#.

##5.2.24# (10 May 2011)
• Ability to open WAV files that contain incorrect information about the number of samples.
• Removed an old bug that could cause Praat to fail to read a chronological TextGrid text file.

##5.2.23# (1 May 2011)
• Removed a bug introduced recently that could cause Praat to crash when working with derivatives of TableOfReal (such as Distributions).

##5.2.22# (14 April 2011)
• @ExperimentMFC: reaction times for key presses.
• Linux: more reliable start-up on German systems.

##5.2.21# (29 March 2011)
• Scripting: removed a crashing bug introduced for colour names in 5.2.20.

##5.2.20# (25 March 2011)
• Scripting: removed a large memory leak for indexed variables.
• Scripting: removed a small memory leak for colour names.
• Support for very long file paths on the Mac.

##5.2.19# (16 March 2011)
• @ExperimentMFC: corrected a bug introduced in 5.2.18 that could cause Praat to crash when extracting results from an incomplete experiment.

##5.2.18# (9 March 2011)
• @ExperimentMFC: incomplete experiments can nevertheless output their incomplete results.

##5.2.17# (2 March 2011)
• Better names for Table opening and saving commands.
• @ExperimentMFC: reaction times for mouse clicks.
• Linux/GTK: corrected triple clicks in ExperimentMFC.

##5.2.16# (20 February 2011)
• Better support for WAV files with special “chunks” in them.
• Manual: documentation of explicit formulas for @@Voice 2. Jitter|jitter measurements@.

##5.2.15# (11 February 2011)
• @@Sounds: Concatenate with overlap...@.

##5.2.14# (8 February 2011)
• Repaired crashing bug in Manipulation window introduced in 5.2.13.

##5.2.13# (7 February 2011)
• Renamed #Write commands to #Save commands.
• Scripting: allow pause forms without #Stop button (see @@Scripting 6.6. Controlling the user@).
• GTK: correct behaviour of default buttons.

##5.2.12# (28 January 2011)
• Renamed #Edit buttons to ##View & Edit#.
• Better visibility of dragged things on Linux.

##5.2.11# (18 January 2011)
• Better visibility of dragged things (sound selection, tier points, TextGrid boundaries) on Windows and Linux.

##5.2.10# (11 January 2011)
• Renamed #Read and #Write menus to #Open and #Save.
• Sound: use of ##Formula (part)...# can speed up formulas appreciably.

##5.2.09# (9 January 2011)
• Much improved scripting tutorial.
• Listening experiments can now show pictures instead of just texts (on the Mac).
• EPS files can now be many miles wide instead of just 55 inches.

##5.2.08# (1 January 2011)
• Improved manual.
• Improved memory allocation on 32-bit platforms:
less probability of crashes when you approach the 2 GB memory limit gradually,
and a new “low on memory; save your work now” warning.
• Removed IntervalTier and TextTier datatypes from Praat (old scripts that use them will continue to work).

##5.2.07# (24 December 2010)
• Support for reading and writing multi-channel sound files (i.e. above two-channel stereo).

##5.2.06# (18 December 2010)
• Picture window: a new (the fourth) line type, namely dashed-dotted.
• Support for analysing and drawing multi-channel sounds (i.e. above two-channel stereo).
• Can read some EEG files (BioSemi 24-bit BDF) as a Sound and a TextGrid object.
• Linux: compatibility with computers without English language support.
• Macintosh: support for high-plane (i.e. very uncommon) Unicode characters in file names (as already existed on Unix and Windows).

##5.2.05# (4 December 2010)
• Regular expressions: better Unicode support.
• Scripting window: command ##Convert to C string#.

##5.2.04# (27 November 2010)
• Scripting: allow directory (folder) selector windows; see @@Scripting 6.6. Controlling the user@.

##5.2.03# (19 November 2010)
• Scripting: support for string arrays.

##5.2.02# (17 November 2010)
• TextGrid window: corrected a bug that caused Praat to crash (instead of doing nothing) when you tried to add boundaries
if the selection ran from the penultimate interval to the end of the TextGrid.
• Scripting: support for arrays with multiple indexes.
• Linux: made spectrogram drawing compatible with Ubuntu 10.10.
• Linux: made sound more easily available on Ubuntu 10.10.

##5.2.01# (4 November 2010)
• Scripting: support for numeric @@Scripting 5.6. Arrays and dictionaries|arrays@.

################################################################################
"What was new in 5.2?"
© Paul Boersma 2009,2010

##5.2# (29 October 2010)

##5.1.45# (26 October 2010)
• Linux/GTK: allow Praat to run without an X display.
• Sounds are played synchronously in scripts run from ManPages with \bsSC.

##5.1.44# (4 October 2010)
• Linux/GTK: visibility of ExperimentMFC window.
• Linux/GTK: keyboard shortcuts.

##5.1.43# (4 August 2010)
• Scripting: support for stand-alone programs; see @@Scripting 9. Turning a script into a stand-alone program@.
• Table: allow drawing of ellipses even if irrelevant columns contain undefined data.
• Linux/GTK: correct resizing of Table window.
• Linux/GTK: prevented multiple storing of Picture window selection in scripting history.

##5.1.42# (26 July 2010)
• Scripting: allow file selector windows; see @@Scripting 6.6. Controlling the user@.
• Linux: multiple file selection.

##5.1.41# (15 July 2010)
• OTGrammar: ##Compare candidates...#
• GTK: support for the Doulos SIL and Charis SIL fonts.
• GTK: working vowel editor.
• Vowel editor: repaired memory leak.
//• GTK: Picture window: front when drawing.
//• GTK: correct scrolling in manuals.

##5.1.40# (13 July 2010)
• GTK: working @@Demo window@ (drawing, clicks, keys).
• GTK: pause forms.
• GTK: manual pages: receive a white background, for compatibility with dark-background themes.
• GTK: in settings windows, better alignment of labels to radio groups.
• GTK: rotated text.

##5.1.39# (10 July 2010)
• GTK beta version.
• Linux: made @@Record Sound (fixed time)...|Sound: Record (fixed time)...@ work correctly (the sampling frequency was wrong).
• GTK: list of objects: multiple selection, working Rename button.
• GTK: running a script no longer deselects all objects at the start.
• GTK: working Buttons editor.
• GTK: correctly laid out settings windows.

##5.1.38# (2 July 2010)
• Linux: made @@Record Sound (fixed time)...|Sound: Record fixed time...@ work correctly (the sampling frequency was wrong).
• Mac: repaired a bug introduced in 5.1.35 that could cause Praat to crash
  if neither Doulos SIL nor Charis SIL were installed.
• Mac: correct live scrolling in Picture window and DataEditor.

##5.1.37# (23 June 2010)
• PitchTier window and similar windows: can drag multiple points simultaneously.
• Table: %t-tests report the number of degrees of freedom.
• GTK: correct progress bar again.
• GTK: correct behaviour of Synth menu in manipulation window.

##5.1.36# (18 June 2010)
• Mac: live scrolling.
• @@Demo window@: the Demo window can now run from the command line.
• Motif: corrected a bug introduced in 5.1.33 whereby things in the Picture window could be placed incorrectly.
• GTK: script window accepts Unicode again.

##5.1.35# (10 June 2010)
• TextGrid window: removed a very old bug that could lead to reversed intervals
  and to crashes when you inserted a boundary after using Shift-arrow and Command-arrow.
• Graphics: Praat now uses Doulos SIL instead of Charis SIL if your font setting is “Times”
  and the font is nonbold and nonitalic, because Doulos SIL matches Times New Roman better.
• kNN: made Ola Söder's k-nearest-neighbours classification compatible with Windows.

##5.1.34# (31 May 2010)
• Sound window: corrected a bug that caused Praat to crash if the analysis window was shorter than 2 samples.
• GTK: scrolling in the Info window and script window.
• GTK: script editor: implemented searching, replacing, and change dialogs.

##5.1.33# (24 May 2010)
• GTK alpha version.
• Abolished resolution independence: the Sound window now looks the same on all platforms,
  and the @@Demo window@ has the same relative font size on your screen and on the video projector.
• GTK: support for asynchronous audio output.
• GTK: sound plays once rather than three times in Sound and other windows.
• GTK: can click more than once in the manual.
• GTK: correct pink selections in Sound and other windows.
• GTK: correct dragging in TextGrid, Manipulation, and tier windows.
• GTK: a working TextGrid window.
• GTK: no automatic triple click in the manual (and many other windows).
• GTK: moving cursor while sound is playing in Sound and other windows.
• GTK: correct colours of the rectangles in Sound and other windows.
• GTK: a working Group button.
• GTK: correct font menus, font size menus, colour menus, line type menus.
• GTK: scrolling in the manual (and many other windows).
• GTK: erase old texts in manual windows.
• GTK: made Picture window come back when drawing.
• GTK: Info window is fronted automatically.
• GTK: support sendpraat.

##5.1.32# (30 April 2010)
• Scripting: command ##Expand include files#.
• Scripting: accept lines that start with non-breaking spaces
  (as may occur in scripts copied from the web manual).
• Sound files: accept MP3 files with extension written in capitals.
• Linux audio recording: corrected input choice (microphone, line).

##5.1.31# (4 April 2010)
• @@Sounds: Convolve...@, @@Sounds: Cross-correlate...@, @@Sound: Autocorrelate...@, with full documentation.
• More query commands for IntensityTier.

##5.1.30# (25 March 2010)
• Scripting: `createFolder` () can now work with absolute paths.
• PointProcess: made it impossible to add a point where there is already a point.

##5.1.29# (11 March 2010)
• Full support for unicode values above 0xFFFF on Macintosh.

##5.1.28# (10 March 2010)
• TextGrid window: removed a recently introduced bug that could cause Praat to crash when inserting a boundary.

##5.1.27# (7 March 2010)
• Table: Wilcoxon rank sum test.
• Logistic regression: corrected a bug by which a boundary would sometimes not be drawn.

##5.1.26# (25 February 2010)
• Experimental GTK version with the help of Franz Brauße.
• Corrected a bug that could cause Praat to crash if sound playing failed twice.

##5.1.25# (20 January 2010)
• Script window: the new command ##Reopen from disk# allows you to edit the script with an external editor.
• Script window: removed a bug that could cause Praat to crash
  if you did ##Find again# before having done any #Find.

##5.1.24# (15 January 2010)
• Formulas run 10 to 20 percent faster.
• Macintosh: support for forward delete key and Command-\` .

##5.1.23# (1 January 2010)
• Allowed multiple files to be selected with e.g. @@Read from file...@.
• @@Demo window@: guarded against handling the Demo window from two scripts at a time.

##5.1.22# (15 December 2009)
• Picture window: millions of @@colour|colours@ instead of just twelve.
• Sound window: ##Move frequency cursor to...#

##5.1.21# (30 November 2009)
• @@Sound: Draw where...@
• ##Matrix: Draw contours...# and ##LogisticRegression: Draw boundary...# support reversed axes.
• Sound window: ##Move frequency cursor to...#.

##5.1.20# (26 October 2009)
• Editor windows: repaired the Synchronized Zoom And Scroll preference.

##5.1.19# (21 October 2009)
• Table: Randomize rows
• Tables: Append (vertically)
• Scripting: corrected a bug that could cause Praat to crash if the name of a field in a form contained a colon.
• Windows: corrected arc drawing.

##5.1.18# (9 October 2009)
• The @@Demo window@ is less often automatically moved to the front
  (in order to allow it to pop up other editor windows).
• @@DTW & TextGrid: To TextGrid (warp times)@: corrected a bug that could
  lead to an incorrect end time of the last interval in new IntervalTiers.

##5.1.17# (22 September 2009)
• Made more stereo movies readable.
• Editor windows now have a ##Zoom Back# button.

##5.1.16# (17 September 2009)
• Macintosh: corrected a bug that caused incorrect phonetic symbols if Charis SIL was available but SIL Doulos IPA93 was not.

##5.1.15# (30 August 2009)
• Corrected a bug in @@Sound: Change gender...@ introduced in 5.1.14.

##5.1.14# (27 August 2009)
• Windows: corrected a bug introduced in 5.1.13 that caused Praat to crash during tab navigation.
• Made @@Sound: Change gender...@ compatible with elephant calls (i.e. very low F0).

##5.1.13# (21 August 2009)
• Script window: #Find and #Replace.
• @@Picture window@ (and therefore @@Demo window@!): @@Insert picture from file...@ (MacOS 10.4 and up).
• @@Demo window@: full screen (on the Mac).
• Scripting: faster object selection (scripts no longer slow down when there are many objects in the list).
• Scripting: @`variableExists` ().
• Macintosh: PDF clipboard (MacOS 10.4 and up).

##5.1.12# (4 August 2009)
• Macintosh: the Picture window can save to PDF file (you need MacOS 10.4 or up).
• Macintosh: corrected a bug that caused Praat to crash at start-up on MacOS 10.3.

##5.1.11# (19 July 2009)

##5.1.10# (8 July 2009)
• Corrected a bug that could cause Praat to crash if the @@Demo window@ was closed after an #execute.
• OTGrammar & PairDistribution: added ##Get minimum number correct...#.

##5.1.09# (28 June 2009)
• Made East-European Roman characters available in EPS files.

##5.1.08# (21 June 2009)
• Removed a bug introduced in 5.1.07 that could cause strange pictures in manual.
• Macintosh: execute @sendpraat messages immediately instead of waiting for the user to click the jumping Praat icon.

##5.1.07# (12 May 2009)
• @@Demo window@: navigation by arrow keys also on Windows.
• @@Demo window@: no longer crashes on Linux.

##5.1.06# (11 May 2009)
• @@Demo window@.

##5.1.05# (7 May 2009)
• KlattGrid update.

##5.1.04# (4 April 2009)
• Corrected a bug that could cause a missing text character in EPS files produced by a version of Praat running in batch.
• Corrected a bug that could cause high values in a Matrix computed from a Pitch.

##5.1.03# (21 March 2009)
//• OT learning: the ##Random up, highest down# update rule.
• ExperimentMFC: corrected a bug introduced in 5.0.36 that caused Praat to crash if a sound file did not exist.
• Articulatory synthesis: corrected a bug that could cause Praat to crash when copying Artwords.
• Macintosh: corrected a bug that could cause poor text alignment in picture.

##5.1.02# (9 March 2009)
• Allow pause forms without fields.
• The value “undefined” is disallowed from all fields in command windows
  except ##Table: Set numeric value...# and @@TableOfReal: Set value...@.
• ##TextGrid: List...# and ##TextGrid: Down to Table...#.
• OT learning: Giorgio Magri’s ##Weighted all up, highest down# update rule.

##5.1.01# (26 February 2009)
• Corrected several bugs in Klatt synthesis.

################################################################################
"What was new in 5.1?"
© Paul Boersma 2007–2009

##5.1# (31 January 2009)
• Editors for Klatt synthesis.
• Corrected many bugs.

##5.0.47# (21 January 2009)
• Extensive pause windows: @@Scripting 6.6. Controlling the user@.

##5.0.46# (7 January 2009)
• More Klatt synthesizer.
• First pause form support.
• Renewed CategoriesEditor.
• Repaired several memory leaks.

##5.0.45# (29 December 2008)
• Bug fixes in Klatt synthesizer.

##5.0.44# (24 December 2008)
• David's Klatt synthesizer: @KlattGrid.

##5.0.43# (9 December 2008)
• Scripting tutorial: local variables in procedures.

##5.0.42# (26 November 2008)
• Removed a bug that could cause Praat to crash when drawing pictures in the manual window.
• Removed a bug that could cause Praat to crash when drawing multi-line text.

##5.0.41# (23 November 2008)
• ExperimentMFC: allow multiple lines in all texts.
• Regular expressions: removed a bug that could cause Praat to hang when using “`.*`”.
• Table: Draw ellipses: removed a bug that could cause Praat to crash if some cells were undefined.

##5.0.40# (10 November 2008)
• Improved reading and writing of text files (faster, bigger).

##5.0.39# (1 November 2008)
• praatcon -a (for sending ANSI encoding when redirected)

##5.0.38# (28 October 2008)
• @FormantGrid: To Formant...

##5.0.36# (20 October 2008)
• @ExperimentMFC: accepts nonstandard sound files.

##5.0.35# (5 October 2008)
• ##Scale times by...# and ##Scale times to...# (Modify menu).

##5.0.34# (22 September 2008)
• ##Shift times by...# and ##Shift times to...# (Modify menu).
• @@Sounds: Combine to stereo@ works even if the two mono sounds have different durations or time domains.

##5.0.33# (9 September 2008)
• Windows: prevented warning messages about “Widget type”.

##5.0.32# (12 August 2008)
• Contributed by Ola Söder: kNN classifiers and k-means clustering.
• Made UTF-16-encoded chronological TextGrid files readable.

##5.0.31# (6 August 2008)
• Macintosh: corrected a bug introduced in 5.0.30 that caused Praat to crash when you pressed the Tab key
in a window without text fields.

##5.0.30# (22 July 2008)
• Macintosh and Windows: tab navigation.

##5.0.29# (8 July 2008)
• OTMulti: can record history.
• Picture window: corrected text in Praat picture files.

##5.0.28# (3 July 2008)
• Windows: audio output uses DirectX (next to Multi-Media Extensions).

##5.0.27# (28 June 2008)
• @@Phonetic symbols@: breve (a\N^).
• Annotation: improved some SpellingChecker commands.
• Table: can now set string values that contain spaces.

##5.0.26# (15 June 2008)
• Windows: sound recording no longer makes your laptop's fan spin.
• Windows: no longer any 64 MB limit on recorded sounds.
• Linux: audio input and output uses Alsa (next to OSS).

##5.0.25# (31 May 2008)
• OT learning: added decision strategy ExponentialMaximumEntropy.

##5.0.24# (14 May 2008)
• Linux: corrected a bug at start-up.

##5.0.23# (9 May 2008)
• Corrected a bug that could cause Praat to crash when you edited an Artword that you had read from a file.

##5.0.22# (26 April 2008)
• Editing formant contours: @FormantGrid.

##5.0.21# (22 April 2008)
• Annotating with Cyrillic, Arabic, Chinese, Korean characters, and many more (on Macintosh and Windows).

##5.0.20# (8 April 2008)
• @ExperimentMFC: prevented the OK key from working if no response (and goodness choice) had been made.
• OT learning: sped up learning from partial outputs by a factor of five or more.

##5.0.19# (4 April 2008)
• TextGrid window: corrected a bug introduced in 5.0.17 that could cause Praat to crash when handling point tiers.

##5.0.18# (31 March 2008)
• Manipulation window: corrected a bug introduced in 5.0.17 that caused Praat to crash when adding the first duration point.
• Sound: added ##Extract all channels#.
• OT learning: added @@OTGrammar & PairDistribution: Find positive weights...@.
• ExperimentMFC: corrected a bug that caused Praat to crash when the second of multiple experiments
  referred to non-existing sound files.

##5.0.17# (29 March 2008)
• Sped up vowel editor by a large factor.
• OT learning: corrected Exponential HG update rule from OT-GLA to HG-GLA.
• OT learning: shift Exponential HG average constraint weight to zero after every learning step.

##5.0.16# (25 March 2008)
• Macintosh: returned to old compiler because of incompatibility with MacOS X 10.4.

##5.0.15# (21 March 2008)
• Windows: more reliable dropping of files on the Praat icon when Praat is already running.

##5.0.14# (20 March 2008)
• David's vowel editor (New \-> Sound).
• Formulas: corrected scaling of sinc function.

##5.0.13# (18 March 2008)
• Corrected drawing of histograms.
• TextGrid window: selected part of the TextGrid can be extracted and saved.
• TextGrid: more complete conversion between backslash trigraphs and Unicode.
• Windows: more reliable dropping of files on the Praat icon when Praat is not running yet.
• Formulas: sinc function.

##5.0.12# (12 March 2008)
• Bigger ligature symbol (k\lip).

##5.0.11# (7 March 2008)
• Corrected saving of new binary Manipulation files (you can send any unreadable Manipulation files to Paul Boersma for correction).

##5.0.10# (27 February 2008)
• Added the characters \d- and \D-.
• Windows: made ##praatcon.exe# compatible with Unicode command lines.

##5.0.09# (16 February 2008)
• Windows: corrected a bug by which Praat would not open files that were dragged on the Praat icon
if the names of these files or their directory paths contained non-ASCII characters.
• Linux: ignore the Mod2 key, because of its unpredictable assignment.

##5.0.08# (10 February 2008)
• Corrected the minus sign (\bs-m = “−”).

##5.0.07# (8 February 2008)
• Object IDs are visible in editor windows.

##5.0.06# (31 January 2008)
• Corrected a bug that caused ##Find again# in the TextGrid window not to work.
• Macintosh: made Praat work correctly on 10.3 computers with missing fonts.

##5.0.05# (19 January 2008)
• All Matrix, Sound, Spectrogram, Pitch, Ltas, and Spectrum objects (and more) are now in 52-bit relative precision (instead of the earlier 23 bits).
• Corrected a bug that could lead to “Unknown opcode (0)” messages when drawing large sounds (more than 16 million samples) in the Picture window.
• Macintosh: solved around a bug in the C library that could cause incorrect representation of non-ASCII characters (in the OTGrammar window).

##5.0.04# (12 January 2008)
• Windows: corrected a bug introduced in 5.0.03 that caused Praat to crash if you pressed a key in the Sound window.
• Macintosh: some cosmetic corrections.

##5.0.03# (9 January 2008)
• Scripting: guard against opening the same file more than once.
• Table: possibility of regarding a column as a distribution (#Generate menu).
• Macintosh: corrected line colours.

##5.0.02# (27 December 2007)
• TextGrid window: corrected the drawing of numbers to the right of the tiers.
• Corrected a bug that caused Praat to crash when doing ##SpectrumTier: List#.

##5.0.01# (18 December 2007)
• Corrected a bug that could cause Praat to crash when redrawing the sound or TextGrid window.

################################################################################
"What was new in 5.0?"
© Paul Boersma 2007

##5.0# (10 December 2007)
• Corrected many bugs.
• Display font sizes in points rather than pixels.

##4.6.41# (9 December 2007)
• Windows: corrected a bug that could cause listening experiments not to run when the directory path included non-ASCII characters;
  the same bug could (under comparable circumstances) cause scripted menu commands not to work.
• Corrected a bug that could cause null bytes in data files when the text output encoding preference was ##try ISO Latin-1, then UTF-16#.

##4.6.40# (3 December 2007)
• Corrected some minor bugs.

##4.6.39# (1 December 2007)
• Manual: corrected a bug that could cause Praat to crash when viewing certain manual pages with pictures.
• Scripting: corrected a bug that could cause Praat to crash when a long string was used as an argument to a procedure.

##4.6.38# (19 November 2007)
• More extensive @@logistic regression@.

##4.6.37# (15 November 2007)
• Object list shows numbers.
• Macintosh: corrected saving of non-ASCII text settings.

##4.6.36# (2 November 2007)
• Sound and TextGrid windows: direct drawing of intensity, formants, pulses, and TextGrid.
• Regular expressions: corrected handling of newlines on Windows.
• Scripting: improved positioning of settings windows for script commands in editors on Windows.

##4.6.35# (22 October 2007)
• Windows and Linux: better positioning of form windows of editor scripts.
• Macintosh: OTMulti learning window more compatible with non-ASCII characters.

##4.6.34# (18 October 2007)
• Corrected a bug introduced in September that could cause Praat to crash when starting up
  if the user had explicitly made action commands visible or invisible in an earlier session.

##4.6.33# (16 October 2007)
• Corrected a bug introduced in September that caused Praat to crash when a PointProcess window was opened without a Sound.
• Macintosh: objects with non-ASCII names show up correctly in the list.

##4.6.32# (14 October 2007)
• Unicode support for names of objects.
• Linux: first Unicode support (in window titles).
• Windows scripting: corrected a bug that caused weird characters in Paste History.

##4.6.31# (8 October 2007)
• TextGrid window: made Save command available again for TextGrid windows without a sound.
• Corrected a bug that caused binary Collection files with objects with names with non-ASCII characters to be unreadable.

##4.6.30# (3 October 2007)
• OTMulti: added an evaluate command in the Objects window, so that paced learning becomes scriptable.
• Macintosh: worked around a feature of a system library that could cause Praat to crash when reading a Collection text file
  that contained objects with non-ASCII names.

##4.6.29# (1 October 2007)
• OT learning: leak and constraint in OTMulti.
• Support for saving Table, TableOfReal, Strings, OTGrammar and OTMulti in Unicode.

##4.6.28# (1 October 2007)
• OT learning: positive constraint satisfactions in OTMulti tableaus.
• Corrected a bug that could cause Praat to crash when reading a non-UTF-8 text file
  when the ##Text reading preference# had been set to UTF-8.

##4.6.27# (29 September 2007)
• Corrected redrawing of text with non-ASCII characters.

##4.6.26# (29 September 2007)
• Corrected reading of high UTF-8 codes.

##4.6.25# (26 September 2007)
• @ExperimentMFC: can set font size for response buttons.

##4.6.24# (24 September 2007)
• Czech, Polish, Croatian, and Hungarian characters such as \c< \uo \t< \e; \s' \l/ \c' \o: (see @@Special symbols@).
• Some support for Hebrew characters such as \?+ \B+ \sU (see @@Special symbols@).

##4.6.23# (22 September 2007)
• Corrected a bug introduced in 4.6.13 that caused crashes in text handling (mainly on Linux).
• Info commands in editors.

##4.6.22# (17 September 2007)
• Phonetic symbols: added the nonsyllabicity diacritic (a\nv).
• Macintosh: worked around a feature of a system library that could cause strange behaviour of forms in scripts with non-ASCII characters.

##4.6.21# (5 September 2007)
• Sound and TextGrid windows: direct drawing of selected sound to the picture window.

##4.6.20# (2 September 2007)
• Introduced direct drawing of spectrogram and pitch to the Praat picture window from Sound windows and TextGrid windows.
• Corrected a bug introduced in 4.6.13 by which Inspect did not show all data in a TextGrid.

##4.6.19# (31 August 2007)
• Macintosh: worked around a bug in a system library that caused Praat to crash (since version 4.6.13)
  when removing a boundary from a TextGrid interval in MacOS X 10.3 or earlier.

##4.6.18# (28 August 2007)
• Sound: Filter (formula): now works in the same way as the other filter commands
  (without adding an empty space at the end), and on stereo sounds.

##4.6.17# (25 August 2007)
• Windows: improved rotated text, also for copy-paste and printing.
• Windows: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.
• Picture settings report (mainly for script writers).
• Corrected a bug that could cause Praat to crash when closing a manual page that had been read from a file.

##4.6.16# (22 August 2007)
• Macintosh: corrected a bug introduced in 4.6.13 that could cause Praat to crash when drawing a spectrogram in MacOS X 10.3 or earlier.

##4.6.15# (21 August 2007)
• Corrected a bug introduced in 4.6.14 that prevented the use of Helvetica in the Picture window.
• Corrected a bug in ##Read Table from table file...#, introduced in 4.6.13.

##4.6.14# (20 August 2007)
• Corrected a bug introduced in 4.6.13 that prevented any other font than Palatino in the Picture window.
• Macintosh: corrected height of subscripts and superscripts (broken in 4.6.13).

##4.6.13# (16 August 2007)
• TextGrid: corrected reading of chronological files.
• Macintosh: text looks better (and rotated text is now readable on Intel Macs).
• Macintosh: phonetic characters on the screen now require the Charis SIL or Doulos SIL font.

##4.6.12# (27 July 2007)
• OTGrammar bug fix: leak and constraint plasticity correctly written into OTGrammar text files.

##4.6.11# (25 July 2007)
• OTGrammar: introduced %%constraint plasticity% for slowing down or halting the speed with which constraints are reranked.
• OTGrammar: introduced %leak for implementing forgetful learning of correlations.
• OTGrammar: positive constraint satisfactions are drawn as “+” in tableaus.

##4.6.10# (22 July 2007)
• Improved reading of UTF-16 data files.
• Improved error messages when reading text files (line numbers are mentioned).
• @@Table: Report group mean (Student t)...|Table: Get group mean (Student t)...@.

##4.6.09# (24 June 2007)
• Corrected a bug introduced in 4.6.07 that caused a crash when reading Collections.
• Corrected a bug introduced in 4.6.07 that caused incorrect Open buttons in Inspect.
• How come 4.6.07 introduced those bugs? Because of large changes in the Praat source code as a result of the transition to Unicode.

##4.6.08# (22 June 2007)
• Windows: worked around a “feature” of the C library that caused 3-byte line-breaks in the buttons file.
• Windows: returned to smaller font in script window.
• OT learning: corrected a bug in PositiveHG.

##4.6.07# (20 June 2007)
• Sound files: MP3 as LongSound (implemented by Erez Volk).
• Scripting: Unicode support for strings and script window (Mac and Windows only).

##4.6.06# (4 June 2007)
• Script window: corrected a bug introduced in 4.6.05 that could cause incorrect symbols in saved files.

##4.6.05# (2 June 2007)
• Sound files: reading MP3 audio files (implemented by Erez Volk).

##4.6.04# (29 May 2007)
• OT learning: added decision strategy PositiveHG.

##4.6.03# (24 May 2007)
• Spectral slices have better names.

##4.6.02# (17 May 2007)
• Sound files: saving FLAC audio files (implemented by Erez Volk).

##4.6.01# (16 May 2007)
• Removed a bug that caused downsampling (and therefore formant measurements)
  to be incorrect for stereo sounds.

################################################################################
"What was new in 4.6?"
© Paul Boersma 2006,2007

##4.6# (12 May 2007)

##4.5.26# (8 May 2007)
• Sound files: reading FLAC audio files (implemented by Erez Volk).

##4.5.25# (7 May 2007)
• @@Table: Rows to columns...@
• @@Table: Collapse rows...@ (renamed from #Pool).
• @@Table: Formula (column range)...@
• OT learning: OTGrammar window shows harmonies.

##4.5.24# (27 April 2007)
• OT learning: added decision strategy MaximumEntropy;
  this has the same harmony determination method as Harmonic Grammar (include the additive constraint noise),
  but there is some more variability, in that every candidate gets a relative probability of exp(harmony).

##4.5.23# (26 April 2007)
• Macintosh: much smaller sizes (in kilobytes) of spectrograms for printing and clipboard;
  this improves the compatibility with other programs such as Microsoft Word for large spectrograms.

##4.5.22# (25 April 2007)
• Macintosh: improved drawing of spectrograms for printing and clipboard
  (this was crippled in 4.5.18, but now it is better than before 4.5.18).

##4.5.21# (24 April 2007)
• OT learning: corrected HarmonicGrammar (and LinearOT) learning procedure
  to the stochastic gradient ascent method applied by @@Jäger (2003)@ to MaxEnt grammars.
• Scripting: removed a bug that could make selection checking (in command windows) unreliable after a script was run.

##4.5.20# (19 April 2007)
• Scripting: allow assignments like `pitch = To Pitch... 0 75 600`.
• PitchTier Formula: guard against undefined values.

##4.5.19# (2 April 2007)
• Scripting: allow comments with “`#`” and “`;`” in forms.
• Windows audio playing: attempt at more compatibility with Vista.

##4.5.18# (30 March 2007)
• Macintosh: better image drawing (more grey values).
• More tabulation commands.
• More SpectrumTier commands.
• Picture window: keyboard shortcut for @@Erase all@.

##4.5.17# (19 March 2007)
• Picture window: can change arrow size.
• Several #List commands.
• ##Spectrum: To SpectrumTier (peaks)#.

##4.5.16# (22 February 2007)
• Sound-to-Intensity: made resistant against undefined settings.
• Windows: made Ctrl-. available as a shortcut.
• Linux: made it more likely to find the correct fonts.

##4.5.15# (12 February 2007)
• Windows XP: worked around a bug in Windows XP that could cause Praat to crash
  when the user moved the mouse pointer over a file in the Desktop in the file selector.
  The workaround is to temporarily disable file info tips when the file selector window is on the screen.

##4.5.14# (5 February 2007)
• Scripting: some new predefined string variables like `preferencesDirectory$`.

##4.5.13# (3 February 2007)
• For stereo sounds, pitch analysis is based on correlations pooled over channels
  (rather than on correlations of the channel average).
• For stereo sounds, spectrogram analysis is based on power density averaged across channels
  (rather than on the power density of the channel average).
• Scripting: removed a bug introduced in 4.5.06 that caused some variables not to be substituted.

##4.5.12# (30 January 2007)
• Made cross-correlation pitch analysis as fast as it used to be before 4.5.11.

##4.5.11# (29 January 2007)
• Sound objects can be stereo, for better playback quality
  (most analyses will work on the averaged mono signal).
• Macintosh: recording a sound now uses CoreAudio instead of SoundManager,
  for more compatibility with modern recording devices,
  and the possibility to record with a sampling frequency of 96 kHz.
• @ManPages allow picture scripts with separate object lists.
• Linux: better scroll bars in object list for Lesstif (Debian).
• Linux: made @@Create Strings as file list...@ work on Reiser.
• @sendpraat scripts correctly wait until sounds have played.

##4.5.08# (20 December 2006)
• ExperimentMFC: can use stereo sounds.

##4.5.07# (16 December 2006)
• Macintosh: playing a sound now uses CoreAudio instead of SoundManager.
• Phonetic symbols: \'1primary stress and \'1secon\'2dary stress.

##4.5.06# (13 December 2006)
• Support for 32-bit floating-point WAV files.
• Scripting: removed several kinds of restrictions on string length.
• SSCP: Draw confidence ellipse: corrected a bug that would sometimes not draw the ellipse when %N was very large.

##4.5.05# (5 December 2006)
• Macintosh scripting: European symbols such as ö and é and ç are now allowed in file names in scripts
  and in MacRoman-encoded file names sent by other programs through the sendpraat subroutine.

##4.5.04# (1 December 2006)
• @@Sound: Change gender...@: corrected a bug that often caused a female-to-male conversion to sound monotonous.

##4.5.03# (29 November 2006)
• Table: added independent-samples t-test.
• Linux: corrected a bug introduced in 4.5.02 that prevented sounds from playing and other weird things.

##4.5.02# (16 November 2006)
• Corrected yet another bug in the new @@Sound: To TextGrid (silences)...@.

##4.5.01# (28 October 2006)
• Sound window: the pitch drawing method is #Curves, #Speckles, or #Automatic.
• Corrected another bug in the new @@Sound: To TextGrid (silences)...@.

################################################################################
"What was new in 4.5?"
© Paul Boersma 2006

##4.5# (26 October 2006)

##4.4.35# (20 October 2006)
• In @ManPages you can now draw pictures.

##4.4.34# (19 October 2006)
• Corrected a bug in the new @@Sound: To TextGrid (silences)...@.

##4.4.33# (4 October 2006)
• Windows: corrected a bug introduced in 4.4.31 that caused Praat to skip the first line of the Buttons file.

##4.4.32# (30 September 2006)
• Scripting: more techniques for @@Scripting 4. Object selection|object selection@.
• Scripting: more support for putting the results of the #Info command into a string variable.

##4.4.31# (23 September 2006)
• Support for @@plug-ins@.
• Split between @@Create Strings as file list...@ and @@Create Strings as folder list...@.

##4.4.30# (28 August 2006)
• Table: Draw ellipse (standard deviation)...

##4.4.29# (21 August 2006)
• Allowed “European” symbols in file names and object names.

##4.4.28# (10 August 2006)
• Windows XP: Praat files can finally again be opened by double-clicking and by dragging them onto the Praat icon.
• Scripting (Windows): removed a bug that caused Praat to crash if the script window was closed when a file selector window was open.

##4.4.27# (4 August 2006)
• Table window: corrected vertical scroll bar (on Windows).
• Formulas: invSigmoid.
• Logging: added `'power'` (and documented the `'freq'` command).
• Removed a bug that caused ##Read two Sounds from stereo file...# not to work in scripts.

##4.4.26# (24 July 2006)
• ##Sound & FormantTier: Filter#: much more accurate.

##4.4.25# (16 July 2006)
• TextGrid reading: don't set first boundary to zero for .wrd label files.

##4.4.24# (19 June 2006)
• Scripting: regular expressions allow replacement with empty string.

##4.4.23# (1 June 2006)
• Table: ignore more white space.

##4.4.22# (30 May 2006)
• Scripting: replacing with regular expression. See @@Formulas 6. String functions@.

##4.4.21# (29 May 2006)
• Made Manipulation objects readable again.

##4.4.20# (3 May 2006)
• Removed limit on number of menus (Praat could crash if the number of open windows was high).

##4.4.19# (28 April 2006)
• @@Table: Get mean...@, @@Table: Get standard deviation...@, @@Table: Get quantile...@.

##4.4.18# (24 April 2006)
• Table: ##View & Edit#: view the contents of a table.
• @@Table: Scatter plot...@.
• Scripting: more warnings against missing or extra spaces.

##4.4.17# (19 April 2006)
• @@Table: Collapse rows...|Table: Pool...@: computing averages and medians of dependent variables
for a selected combination of independent variables.
• @@Table: Formula...@ accepts string expressions as well as numeric expressions.
• @@Table: Sort rows...|Table: Sort...@ can sort by any number of columns.
• @@Create Table with column names...|Table: Create with column names...@.
• @@Table: Report mean (Student t)...|Table: Report mean...@.
• Formulas: @@Formulas 8. Attributes of objects|`row$` and `col$` attributes@.
• Warning when trying to read data files whose format is newer than the Praat version.

##4.4.16# (1 April 2006)
• Spectrum window: dynamic range setting.
• SoundRecorder: corrected a bug in the Intel Mac edition.

##4.4.15# (30 March 2006)
• Source code even more compatible with 64-bit compilers.

##4.4.14# (29 March 2006)
• Source code more compatible with 64-bit compilers.

##4.4.13# (8 March 2006)
• ##Table: To TableOfReal...#: better handling of --undefined-- values (are now numeric).
• MacOS X: TextGrid files can be double-clicked to open.
• @@Create Strings as file list...@: now handles up to 1,000,000 files per directory.

##4.4.12# (24 February 2006)
• TextGrid: removed a bug introduced in 4.4.10 that caused Praat to crash when converting an IntervalTier into a TextGrid.

##4.4.11# (23 February 2006)
• Listening experiments: removed a bug that could cause Praat to crash when an ExperimentMFC object was removed.

##4.4.10# (20 February 2006)
• Intel computers: corrected reading and writing of 24-bit sound files (error introduced in 4.4.09).
• Create TextGrid: guard against zero tiers.
• MacOS X: correct visibility of Praat icon.
• MacOS X: correct dropping of Praat files on Praat icon.

##4.4.09# (19 February 2006)
• Macintosh: first Intel Macintosh version.
• Windows: @@Create Strings as folder list...|Create Strings from folder list...@.

##4.4.08# (6 February 2006)
• Much improved cepstral smoothing.

##4.4.07# (2 February 2006)
• More scripting facilities (local variables in procedures, e.g. `.x` and `.text$`).
• Faster formulas.

##4.4.06# (30 January 2006)
• More scripting facilities (`Object_xxx [ ]`, `Self.nx`, `Table_xxx$ [ ]`, better messages).
• Better reading and writing of Xwaves label files.

##4.4.05# (26 January 2006)
• @ExperimentMFC: removed a bug that caused Praat to crash when the Oops button was pressed after the experiment finished.
• TextGrid: an IntervalTier can be written to an Xwaves label file.

##4.4.04# (6 January 2006)
• Windows: Quicktime support (see at 4.4.03).

##4.4.03# (6 January 2006)
• Macintosh: Quicktime support, i.e., @@Read from file...@ can now read the audio from several kinds of movie files (.mov, .avi).

##4.4.02# (5 January 2006)
• OT learning: allow the decision strategies of Harmonic Grammar and Linear OT.

##4.4.01# (2 January 2006)
• Picture window: ##Logarithmic marks...# allows reversed axes.
• Manipulation window: removed a bug from ##Shift frequencies...# that caused much too small shifts in semitones.
• ##TextGrid: Remove point...#.

################################################################################
"What was new in 4.4?"
© Paul Boersma 2005

##4.4# (19 December 2005)

##4.3.37# (15 December 2005)
• @@Principal component analysis@: now accepts tables with more variables (columns) than cases (rows).
• TIMIT label files: removed a bug that caused Praat to crash for files whose first part was not labelled.

##4.3.36# (11 December 2005)
• Ltas: Average.
• Optimality Theory: compute crucial rankings (select OTGrammar + PairDistribution).

##4.3.35# (8 December 2005)
• @ExperimentMFC: switched off warnings for stereo files.

##4.3.34# (8 December 2005)
• Sound window: the arrow scroll step is settable.
• You can now run multiple listening experiments (@ExperimentMFC) in one #Run.
• ##Formant: Get quantile of bandwidth...#.

##4.3.33# (6 December 2005)
• Removed three bugs introduced in 4.3.32 in @ExperimentMFC.

##4.3.32# (5 December 2005)
• Many more possibilities in @ExperimentMFC.

##4.3.31# (27 November 2005)
• @@Sound: To Ltas (pitch-corrected)...@

##4.3.30# (18 November 2005)
• TableOfReal: Scatter plot: allows reversed axes.

##4.3.29# (11 November 2005)
• Windows: many more font sizes.

##4.3.28# (7 November 2005)
• Fontless EPS files: corrected character width for Symbol font (depended on SILIPA setting).
• Windows: more reliable detection of home directory.

##4.3.27# (7 October 2005)
• TextGrid & Pitch: draw with text alignment.

##4.3.26# (29 September 2005)
• Macintosh: corrected error introduced in 4.3.25.

##4.3.25# (28 September 2005)
• Macintosh: allowed recording with sampling frequencies of 12 and 64 kHz.

##4.3.24# (26 September 2005)
• ##Table: Down to TableOfReal...#: one column of the Table can be used as the row labels for the TableOfReal,
  and the strings in the remaining columns of the Table are replaced with whole numbers assigned in alphabetical order.

##4.3.23# (24 September 2005)
• @@Read Table from comma-separated file...@
• @@Read Table from tab-separated file...@
• Write picture as fontless EPS file: choice between XIPA and SILIPA93.
• Bold IPA symbols in EPS files (fontless SILIPA93 only).

##4.3.22# (8 September 2005)
• Macintosh: variable scroll bar size (finally, 7 years since System 8.5).

##4.3.21# (1 September 2005)
• Macintosh: error message if any of the fonts Times, Helvetica, Courier and Symbol are unavailable at start-up.
• Renamed Control menu to “Praat” on all platforms (as on the Mac),
to reflect the fact that no other programs than Praat have used the Praat shell for five years.
• Script editor: Undo and Redo buttons (only on the Mac for now).
• Manual: corrected a bug that sometimes caused Praat to crash when trying to print.

##4.3.20# (18 August 2005)
• Log files: include name of editor window.

##4.3.19# (20 July 2005)
• Improved buttons in manual.
• @@Read TableOfReal from headerless spreadsheet file...@: allow row and column labels to be 30,000 rather than 100 characters.

##4.3.18# (12 July 2005)
• Glottal source for sound synthesis, corrected and documented.

##4.3.17# (7 July 2005)
• Glottal source for sound synthesis.
• Multi-level Optimality Theory: parallel evaluation and bidirectional learning.

##4.3.16# (22 June 2005)
• Pitch drawing: corrected logarithmic scales.

##4.3.15# (22 June 2005)
• Graphics: better dotted lines in pitch contours; clipped pitch curves in editor windows.
• Pitch analysis: more different units (semitones %re 1 Hz).

##4.3.14# (14 June 2005)
• Scripting: regular expressions.
• Removed a bug that caused Praat to crash if a proposed object name was longer than 200 characters.

##4.3.13# (19 May 2005)
• Macintosh: an option to switch off screen previews in EPS files.
• Sources: compatibility of makefiles with MinGW (Minimalist GNU for Windows).

##4.3.12# (10 May 2005)
• Some more manual tricks.

##4.3.11# (6 May 2005)
• TextGrid editor: show number of intervals.

##4.3.10# (25 April 2005)
• ##Table: Get logistic regression...#.

##4.3.08# (19 April 2005)
• OT learning: store history with ##OTGrammar & Strings: Learn from partial outputs...#.

##4.3.07# (31 March 2005)
• Linux: removed a bug that could cause a sound to stop playing.

##4.3.04# (9 March 2005)
• Use SIL Doulos IPA 1993/1996 instead of 1989.

##4.3.03# (2 March 2005)
• TextGrid window: green colouring of matching text.
• Regular expressions can be used in many places.
• Pitch analysis: switched off formant-pulling.

##4.3.02# (16 February 2005)
• ##TextGrid: Remove boundary at time...#
• Scripting: corrected %nowarn.
• Linux: guard against blocking audio device.
• Macintosh: guard against out-of-range audio level meter.

##4.3.01# (9 February 2005)
• Replaced PostScript font SILDoulosIPA with XIPA (adapted for Praat by Rafael Laboissière).
• ##Sound: Set part to zero...#
• ##Pitch: To Sound (sine)...#
• ##Sound & TextGrid: Clone time domain#.

################################################################################
"What was new in 4.3?"
© Paul Boersma 2005

Praat 4.3, 26 January 2005
==========================

General:
• #Apply button in settings windows for menu commands and in script forms.
• Info window can be saved.
• Removed 30,000-character limit in Info window.

Phonetics:
• Speeded up intensity analysis by a factor of 10
  (by making its time resolution 0.01 ms rather than 0.0001 ms at a sampling frequency of 44 kHz).
• Speeded up pitch analysis and spectrogram analysis by a factor of two.
• Sound: To Spectrum... now has a reasonably fast non-FFT version.
• Calibrated long-term average spectrum (Sound: To Ltas...).
• Pitch-corrected LTAS analysis.
• Sound: Scale intensity.
• PitchTier: To Sound (sine).
• Better warnings against use of the LPC object.

Voice:
• July 9, 2004 (4.2.08): Shimmer measurements: more accurate and less sensitive to additive noise.
• More extensive voice report: pitch statistics; harmonicity.

Audio:
• Reading and opening 24-bit and 32-bit sound files (saving still goes in 16 bits).
• LongSound: save separate channels.
• Macintosh: much faster reading of WAV files.

Listening experiments:
• Subjects can now respond with keyboard presses.

Graphics:
• One can now drag the inner viewport in the Picture window, excluding the margins.
  This is nice e.g. for creating square viewports or for drawing a waveform and a spectrogram in close contact.
• Unix: picture highlighting as on Mac and Windows.
• More drawing methods for Sound and Ltas (curve, bars, poles, speckles).

OT learning:
• Monitor rankings when learning.
• OTGrammar: Remove harmonically bounded candidates...
• OTGrammar: Save as headerless spreadsheet file...
• Metrics grammar: added *Clash, *Lapse, WeightByPosition and *MoraicConsonant.

Scripting:
• nowarn, noprogress, nocheck.
• Line numbers.

################################################################################
"What was new in 4.2?"
© Paul Boersma 2004

Praat 4.2, 4 March 2004
=======================

General:
• July 10, 2003: Open source code (@@General Public License, version 3|General Public License@).

Phonetics:
• Faster computation of spectrum, spectrogram, and pitch.
• More precision in numeric libraries.
• PitchTier: Interpolate quadratically.
• TextGrids can be saved chronologically (and Praat can read that file again).
• Sound editor window @@Time step settings...@: #Automatic, #Fixed, and ##View-dependent#.
• Sound window: distinguish basic from advanced spectrogram and pitch settings.
• Read TableOfReal from headerless spreadsheet file...: cells with strings are considered zero.
• Sound window: introduced time step as advanced setting.
• Sound window: reintroduced view range as advanced setting.
• Ltas: Compute trend line, Subtract trend line.

Audio:
• Sun workstations: support audio servers.

Graphics:
• Better selections in Picture window and editor windows.
• Picture window: better handling of rectangles and ellipses for reversed axes.
• Windows: corrected positioning of pictures on clipboard and in metafiles.
• Windows: EPS files check availability of Times and TimesNewRomanPSMT.
• Polygon: can now also paint in colour instead of only in grey values.
• Unlimited number of points for polygons in PostScript (may not work on very old printers).
• Picture window: line widths on all printers and clipboards are now equal to line widths used on PostScript printers:
  a line with a line width of “1” will be drawn with a width 3/8 points. This improves the looks of pictures printed
  on non-PostScript printers, improves the looks of pictures copied to your wordprocessor when printed,
  and changes the looks of pictures copied to your presentation program.

OT learning:
• Metrics grammar supports “impoverished overt forms”,
  i.e. without secondary stress even if surface structures do have secondary stress.
• Support for crucially tied constraints and tied candidates.
• Support for backtracking in EDCD.
• Queries for testing grammaticality.

Scripting:
• ManPages: script links can receive arguments.
• ManPages: variable duration of recording.
• Support for unlimited size of script files in editor window on Windows XP and MacOS X (the Unix editions already had this).
• Improved the reception of %sendpraat commands on Windows XP.

################################################################################
"What was new in 4.1?"
© Paul Boersma 2003

Praat 4.1, 5 June 2003
======================

General:
• MacOS X edition.
• Removed licensing.
• More than 99 percent of the source code distributed under the @@General Public License, version 3|General Public License@.
• Windows 2000 and XP: put preferences files in home directory.

Phonetics:
• Spectrum: the sign of the Fourier transform has changed, to comply with common use
  in technology and physics. Old Spectrum files are converted when read.
• Spectral moments.
• Many jitter and shimmer measures, also in the Sound editor window.
• PitchTier: shift or multiply frequencies (also in ManipulationEditor).
• TextGrid: shift times, scale times.
• Overlap-add synthesis: reduced buzz in voiceless parts.
• @@Sound: Change gender...
• Editors: @@Intro 3.6. Viewing a spectral slice@.
• Editors: ##Get spectral power at cursor cross#.
• @@Sound: To PointProcess (periodic, peaks)...@
• Ltas: merge.

Listening experiments:
• Goodness judgments.
• Multiple ##ResultsMFC: To Table#, so that the whole experiment can go into a single statistics file.
• Stimulus file path can be relative to directory of experiment file.
• @ExperimentMFC: multiple substimuli for discrimination tests.

Statistics:
• New @Table object for column @statistics: Pearson's %r, Kendall's %\ta-%b, %t-test.
• Table: scatter plot.
• Table: column names as variables.
• @@T-test@.
• TableOfReal: Extract rows where column...
• TableOfReal: Get correlation....
• @@Correlation: Confidence intervals...
• @@SSCP: Get diagonality (bartlett)...

OT learning:
• Tutorial for bidirectional learning.
• Random choice between equally violating candidates.
• More constraints in metrics grammar.
• Learning in editor.

Graphics:
• Printing: hard-coded image interpolation for EPS files and PostScript printers.

Scripting:
• New @Formulas tutorial.
• @Formulas: can use variables without quotes.
• Formulas for PitchTier, IntensityTier, AmplitudeTier, DurationTier.
• Refer to any matrices and tables in formulas, e.g. `Sound_hello (x)` or `Table_everything [row, col]`
  or `Table_tokens [i, “F1”]`.
• Assignment by modification, as with += -= *= /=.
• New functions: @`date$` (), @`extractNumber`, @`extractWord$`, @`extractLine$`. See @@Formulas 6. String functions@.
• @@Scripting 5.8. Including other scripts@.
• String formulas in the calculator.
• Stopped support of things that had been undocumented for the last four years:
  #let, #getnumber, #getstring, #ARGS, #copy, #proc, variables with capitals, and strings in numeric variables;
  there are messages about how to modify your old scripts.
• Disallowed ambiguous expressions like -3\^ 2.

################################################################################
"What was new in 4.0?"
© Paul Boersma 2001

Praat 4.0, 15 October 2001
==========================

Editors:
• Simplified selection and cursor in editor windows.
• Spectrogram, pitch contour, formant contour, and intensity available in the
  Sound, LongSound, and TextGrid editors.
• TextGrid editor: additions and improvements.
• @@Log files@.

Phonetics library:
• @ExperimentMFC: multiple-forced-choice listening experiments.
• @@Sound: To Pitch (ac)...@: pitch contour less dependent on time resolution.
  This improves the constancy of the contours in the editors when zooming.
• TextGrid: additions and improvements.
• Sounds: Concatenate recoverably. Creates a TextGrid whose interval labels are the original
  names of the sounds.
• Sound & TextGrid: Extract all intervals. The reverse of the previous command.
• Filterbank analyses, @MelFilter, @BarkFilter and
  @FormantFilter, by @@band filtering in the frequency domain@." )
• Cepstrum by David Weenink: @MFCC, @LFCC.
  @Cepstrum object is a representation of the %%complex cepstrum%.
• Intensity: To IntensityTier (peaks, valleys).
• Replaced Analysis and AnalysisEditor with @Manipulation and @ManipulationEditor.

Phonology library:
• PairDistribution: Get percentage correct (maximum likelihood, probability matching).
• OTGrammar & PairDistribution: Get percentage correct...

Graphics:
• Improved spectrogram drawing.
• @@Special symbols@: háček.
• Macintosh: improved screen rendition of rotated text.

Audio:
• Macintosh: support for multiple audio input devices (sound cards).

Statistics and numerics library:
• More statistics by David Weenink.
• Improved random numbers and other numerical stuff.
• @@Regular expressions@.

Scripting:
• Formatting in variable substitution, e.g. 'pitch:2' gives two digits after the decimal point.
• Added @`fixed$` () to scripting language for formatting of numbers.

Documentation:
• @@Multidimensional scaling@ tutorial.
• Enabled debugging-at-a-distance.

################################################################################
"What was new in 3.9?"
© Paul Boersma 2000

Praat 3.9, 18 October 2000
==========================

Editors:
• Shift-click and shift-drag extend or shrink selection in editor windows.
• Grouped editors can have separate zooming and scrolling (FunctionEditor preferences).
• Cursor follows playing sound in editors; interruption by Escape key moves the cursor.
• TextGridEditor: optimized for transcribing large corpora: text field, directly movable boundaries,
  more visible text in tiers, @SpellingChecker,
  type while the sound is playing, complete keyboard navigation, control font size, control text alignment,
  shift-click near boundary adds interval to selection.
• Stereo display in LongSound and TextGrid editors.
• LongSoundEditor and TextGridEditor: write selection to audio file.
• SoundEditor: added command ##Extract selection (preserve times)#.
• IntervalTierEditor, DurationTierEditor.
• Added many query commands in editors.

Phonetics library:
• @@Sound: To Formant (burg)...|Sound: To Formant...@: sample-rate-independent formant analysis.
• @@Sound: To Harmonicity (ac)...|Sound: To Harmonicity...@ (glottal-to-noise excitation ratio).
• Pitch: support for ERB units, draw all combinations of line/speckle and linear/logarithmic/semitones/mels/erbs,
  optionally with TextGrid, Subtract linear fit.
• Spectrum: Draw along logarithmic frequency axis.
• TextGrid: modification commands, Extract part, Shift to zero, Scale times (with Sound or LongSound).
• @@Matrix: To TableOfReal@, ##Matrix: Draw contour...#.
• Concatenate Sound and LongSound objects.
• File formats: save PitchTier in spreadsheet format, read CGN syntax files (XML version),
  text files now completely file-server-safe (independent from Windows/Macintosh/Unix line separators).

Statistics and numerics library:
• @@Principal component analysis@.
• @@Discriminant analysis@.
• @Polynomial: drawing, @@Roots|root@ finding etc.
• @@TableOfReal: Draw box plots...@.
• @@Covariance: To TableOfReal (random sampling)...@.
• @@SSCP: Get sigma ellipse area...@.
• Query @DTW for “weighted distance” of time warp.
• ##Distributions: To Strings (exact)...#
• ##Strings: Randomize#

Phonology library:
• ##OTGrammar: To PairDistribution#.

Graphics:
• Full support for colour inkjet printers on Windows and Macintosh.
• Full support for high-resolution colour clipboards and metafiles for
  Windows and Macintosh programs that support them (this include MS Word
  for Windows, but unfortunately not MS Word for Macintosh).
• Colour in EPS files.
• Interpolating grey images, i.e. better zoomed spectrograms.
• Linux: support for 24-bits screens.

Audio:
• Asynchronous sound play.
• Linux: solved problems with /dev/mixer (“Cannot read MIC gain.”) on many computers.
• Added possibility of zero padding for sound playing,
  in order to reduce clicks on some Linux and Sun computers.
• LongSound supports mono and stereo, 8-bit and 16-bit, %\mu-law and A-law,
  big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.
•v##Read two Sounds from stereo file...# supports 8-bit and 16-bit, %\mu-law and A-law,
  big-endian and little-endian, AIFC, WAV, NeXT/Sun, and NIST files.
• SoundRecorder writes to 16-bit AIFC, WAV, NeXT/Sun, and NIST mono and stereo files.
• Sound & LongSound: write part or whole to mono or stereo audio file.
• Read Sound from raw Alaw file.
• Artword & Speaker (& Sound) movie: real time on all platforms.

Scripting:
• @@Formulas 5. Mathematical functions@: added statistical functions: %\ci^2, Student T, Fisher F, binomial,
  and their inverse functions.
• Windows: program #`praatcon` for use as a Unix-style console application.
• Windows and Unix: Praat can be run with a command-line interface without quitting on errors.
• Unix & Windows: can use <stdout> as a file name (supports pipes for binary data).
• @sendpraat now also for Macintosh.
• @@Scripting 6.7. Sending a message to another program|sendsocket@.
• @@Read from file...@ recognizes script files if they begin with “`#!`”.
• Script links in @ManPages.

Documentation
• Tutorials on all subjects available through @Intro.

################################################################################
"What was new in 3.8?"
© Paul Boersma 1999

Praat 3.8, 12 January 1999
==========================

Phonetics library
• New objects: @LongSound (view and label long sound files), with editor; PairDistribution.
• @@Overlap-add@ manipulation of voiceless intervals, version 2: quality much better now;
target duration is exactly as expected from Duration tier or specified lengthening in @@Sound: Lengthen (overlap-add)...@.
• Audio: Escape key stops audio playing (on Mac also Command-period).
• @SoundRecorder: allows multiple recordings without close; Play button; Write buttons; buffer size can be set.
• Reverse a Sound or a selection of a Sound.
• @@Sound: Get nearest zero crossing...@.
• ##Formant: Scatter plot (reversed axes)...#
• ##TextGrid & Pitch: Speckle separately...#
• ##Extract Sound selection (preserve times)# in TextGridEditor.
• More query commands for Matrix, TableOfReal, Spectrum, PointProcess.

Phonology library
• 25-page OT learning tutorial.
• Made the OT learner 14 times as fast.

Platforms
• May 23: Windows beta version.
• April 24: Windows alpha version.

Files
• Read more Kay, Sun (.au), and WAV sound files.
• @@Read Strings from raw text file...@
• @@Create Strings as file list...@
• ##Read IntervalTier from Xwaves...#
• hidden ##Read from old Windows Praat picture file...#

Graphics
• Use colours (instead of only greys) in ##Paint ellipse...# etc.
• More true colours (maroon, lime, navy, teal, purple, olive).
• Direct printing from Macintosh to PostScript printers.
• Hyperpage printing to PostScript printers and PostScript files.
• Phonetic symbols: raising sign, lowering sign, script g, corner, ligature, pointing finger.

Shell
• November 4: all dialogs are modeless (which is new for Unix and Mac).
• September 27: @sendpraat for Windows.

Scripting
• January 7: scriptable editors.
• October 7: file I/O in scripts.
• August 23: script language includes all the important functions for string handling.
• June 24: string variables in scripts.
• June 22: faster look-up of script variables.
• June 22: unlimited number of script variables.
• April 5: suspended chopping of trailing spaces.
• March 29: enabled formulas as arguments to dialogs (also interactive).

################################################################################
"What was new in 3.7?"
© Paul Boersma 1998

Praat 3.7, 24 March 1998
========================

Editors:
• In all FunctionEditors: drag to get a selection.

Phonetics library:
• Many new query (#Get) commands for @Sound, @Intensity, @Harmonicity, @Pitch,
  @Formant, @Ltas, @PitchTier, @IntensityTier, @DurationTier, #FormantTier.
• Many new modification commands.
• Many new interpolations.
• Sound enhancements: @@Sound: Lengthen (overlap-add)...@, @@Sound: Deepen band modulation...@
• @@Source-filter synthesis@ tutorial, @@Sound & IntensityTier: Multiply@,
  ##Sound & FormantTier: Filter#, @@Formant: Formula (frequencies)...@, @@Sound: Pre-emphasize (in-place)...@.

Labelling
• TextGrid queries (#Get times and labels in a script).
• @@TextGrid: Count labels...@.
• @@PointProcess: To TextGrid (vuv)...@: get voiced/unvoiced information from a point process.
• ##IntervalTier: To TableOfReal...#: labels become row labels.
• ##TextTier: To TableOfReal...#.

Numerics and statistics library
• @@Multidimensional scaling@ (Kruskal, INDSCAL, etc).
• @TableOfReal: @@TableOfReal: ||Set value...@, @@Formula...@,
  ##Remove column...#, ##Insert column...#,
  ##Draw as squares...#, ##To Matrix#.

Phonology library
• OT learning: new strategies: weighted symmetric plasticity (uncancelled or all).

Praat shell
• First Linux version.
• Eight new functions like e.g. @`hertzToBark` in @@Formulas 5. Mathematical functions@.
• @@Praat script@: procedure arguments; object names.

Documentation:
• 230 more man pages (now 630).
• Hypertext: increased readability of formulas, navigation with keyboard.

################################################################################
"What was new in 3.6?"
© Paul Boersma 1997

Praat 3.6, 27 October 1997
==========================

Editors:
• Intuitive position of B and E buttons on left-handed mice.
• @SoundEditor: copy %windowed selection to list of objects.
• @SoundEditor: undo Cut, Paste, Zero.
• @SpectrumEditor: copy band-filtered spectrum or sound to list of objects.
• @ManipulationEditor: LPC-based pitch manipulation.

Objects:
• Use “-” and “+” in object names.

Phonetics library
• LPC-based resynthesis in @ManipulationEditor.
• @Sound: direct modification without formulas (addition, multiplication, windowing)
• @Sound: filtering in spectral domain by formula.
• Create a simple @Pitch object from a @PitchTier (for %F__0_) and a @Pitch (for V/U).
• Semitones in @PitchTier tables.
• @PointProcess: transplant time domain from @Sound.
• Much more...

Phonology library
• Computational Optimality Theory. See @@OT learning@.

Hypertext
• You can use @ManPages files for creating your own tutorials.
  These contains buttons for playing and recording sounds, so you can use this for creating
  an interactive IPA sound training course.

Scripting:
• Programmable @@Praat script@ language: variables, expressions, control structures,
  procedures, complete dialog box, exchange of information with Info window, continuation lines.
• Use platform-independent relative file paths in @@Praat script@.
• @ScriptEditor: Run selection.

Graphics:
• Rotation and scaling while printing the @@Picture window@.
• Apart from bold and italic, now also bold-italic (see @@Text styles@).
• Rounded rectangles.
• Conversion of millimetres and world coordinates.
• Measurement of text widths (screen and PostScript).

Unix:
• Use the @sendpraat program for sending messages to running Praat programs.

Mac:
• Praat looks best with the new and beautiful System 8.

################################################################################
"What was new in 3.5?"
© Paul Boersma 1997

Praat 3.5, 27 May 1997
======================

New editors:
• @TextGridEditor replaces and extends LabelEditor: edit points as well as intervals.
• @@ManipulationEditor|AnalysisEditor@ replaces and extends PsolaEditor: view pitch, spectrum, formant, and intensity
analyses in a single window, and allow pitch and duration resynthesis by overlap-add and more (would be undone in 3.9.19).
• @SpectrumEditor allows you to view and edit spectra.

Praat shell:
• @@History mechanism@ remembers all the commands that you have chosen,
and allows you to put them into a script.
• @ScriptEditor allows you to edit and run any Praat script, and to put it under a button.
• All added and removed buttons are remembered across sessions.
• #ButtonEditor allows you to make buttons visible or invisible.

Evaluations:
• In his 1996 doctoral thesis, Henning Reetz
compared five pitch analysis routines; @@Sound: To Pitch (ac)...@ appeared to make the fewest errors.
H. Reetz (1996): %%Pitch Perception in Speech: a Time Domain Approach%, Studies in Language and Language Use #26,
IFOTT, Amsterdam (ICG Printing, Dordrecht).

Documentation:
• 140 more man pages (now 330).
• Tables and pictures in manual.
• Printing the entire manual.
• Logo.

New types:
• Labelling & segmentation: @TextGrid, #IntervalTier, #TextTier.
• Analysis & manipulation: @@Manipulation|Analysis@.
• Statistics: @TableOfReal, @Distributions, #Transition

File formats:
• Read and write rational numbers in text files.
• Read 8-bit .au sound files.
• Read and write raw 8-bit two’s-complement and offset-binary sound files.

Audio:
• 16-bit interactive Sound I/O on Mac.
• Record sounds at 9.8 kHz on SGI.

New commands:
• Two more pitch-analysis routines.
• Sound to PointProcess: collect all maxima, minima, zero crossings.
• PointProcess: set calculus.
• TextGrid: extract time-point information.
• Compute pitch or formants at given time points.
• Put pitch, formants etc. in tables en get statistics.
• Many more...

Macintosh:
• 16-bit interactive sound I/O.
• Fast and interpolating spectrogram drawing.
• Phonetic Mac screen font included in source code (as a fallback to using SIL Doulos IPA).
• Keyboard shortcuts, text editor, help under question mark, etc.

################################################################################
"What was new in 3.3?"
© Paul Boersma 1996

Praat 3.3, 6 October 1996
=========================
• Documentation: hypertext help browser, including the first 190 man pages.
• New editors: type #TextTier for labelling times instead of intervals.
• New actions: #Formant: Viterbi tracker, Statistics menu, Scatter plot.
• Evaluation: For HNR analysis of speech, the cross-correlation method,
  which has a sensitivity of 60 dB and a typical time resolution of 12 milliseconds,
  must be considered better than the autocorrelation method,
  which has a better sensitivity (80 dB), but a much worse time resolution (30 ms).
  For pitch analysis, the autocorrelation method still beats the cross-correlation method
  because of its better resistance against noise and echos,
  and despite its marginally poorer resolution (15 vs. 12 ms).
• User preferences are saved across sessions.
• The phonetic X screen font included in the source code.
• Xwindows resources included in the source code
• Graphics: eight colours, small caps, text rotation.
• File formats: Sun/NexT mu-law files, raw matrix text files, Xwaves mark files.
• Accelerations: keyboard shortcuts, faster dynamic menu, Shift-OK keeps file selector on screen.
• Object type changes: #StylPitch and #MarkTier are now called @PitchTier and #TextTier, respectively.
  Old files can still be read.
• Script warning: all times in dialogs are in seconds now: milliseconds have gone.

################################################################################
"What was new in 3.2?"
© Paul Boersma 1996

Praat 3.2, 29 April 1996
========================
• Sound I/O for HPUX, Sun Sparc 5, and Sun Sparc LX.
• Cross-correlation pitch and HNR analysis.
• Facilities for generating tables from scripts.
• Editing and playing stylized pitch contours and point processes.
• Overlap-add pitch manipulation.
• Spectral smoothing techniques: cepstrum and LPC.
• Time-domain pitch analysis with jitter measurement.
• Read and write Bell-Labs sound files and Kay CSL audio files.
• Replaced IpaTimes font by free SILDoulos-IPA font, and embedded phonetic font in PostScript picture.
• Completed main phonetic characters.

################################################################################
"What was new in 3.1?"
© Paul Boersma 1995

Praat 3.1, 5 December 1995
==========================
• Add and remove buttons dynamically.
• DataEditor (Inspect button).
• Initialization scripts.
• Logarithmic axes.
• Call remote ADDA server directly.

################################################################################
)~~~"
MAN_PAGES_END

}

/*
   BUGBASE

>* The Artword editor would be easier to read if the vertical axis of the
>graphs were squeezed a little.  As it is, the line for a target of 1
>throughout the utterance merges into the top bounding box and is
>invisible.
>

>* In drawing a picture of an Artword for a Speaker, the Pen|Line-width
>option only works for dotted and dashed lines.  It's ignored for plain
>lines (and is WAY too wide).

ENTRY (U"To do")
	• TextGrid & Sound: Extract intervals with margins.")
	• Spectrum: draw power, re, im, phase.")
	• Formant: To Spectrum (slice)... (combines Formant-to-LPC and LPC-to-Spectrum-slice)")
	• Read and/or write Matlab files, MBROLA files, Xwaves files, CHAT files.") // Aix
	• Matrix: draw numbers.")
	• Fractions with \bsf{a|b}.")
	• Move objects up and down list.")
	• Spectrogram cross-correlation.")
	• Labels in AIFC file.") // Theo Veenker 19980323
	• Improve scrolling and add selection in hyperpages.")
	• Segment spectrograph?") // Ton Wempe, Jul 16 1996
	• Phoneme-to-articulation conversion??") // Mirjam Ernestus, Jul 1 1996
ENTRY (U"Known bugs in the Windows version")
	• Cannot stand infinitesimal zooming in SpectrogramEditor.")
*/

/*
> I would like to be able to extract the duration
> and pitch measurements of several words in each utterance

# A Sound and a TextGrid have to be selected first.
textgrid = selected ("TextGrid")
sound = selected ("Sound")
select 'sound'
To Pitch... 0.001 75 600
pitch = selected ("Pitch")
select 'textgrid'
numberOfIntervals = Get number of intervals... 1
filedelete out.txt
for interval to numberOfIntervals
   select 'textgrid'
   tmin = Get starting point... 1 interval
   tmax = Get end point... 1 interval
   duration = tmax - tmin
   select 'pitch'
   f0 = Get quantile... tmin tmax 0.50 Hertz
   fileappend out.txt 'duration:6' 'f0:3''newline$'
endfor
0.225953 377.677
0.170586 376.039
0.328049 376.521
0.275413 378.538
*/

/*
echo Band powers:
binWidth = Get bin width
approximateDuration = 1 / binWidth
call band 125 160
call band 160 200
call band 200 250
call band 5000 6300

procedure band fmin fmax
   bandEnergy = Get band energy... fmin fmax
   rmsPressure_Pascal = sqrt (bandEnergy / approximateDuration)
   intensity_dB = 20 * log10 (rmsPressure_Pascal / 2e-5)
   printline 'fmin'...'fmax': 'intensity_dB:2' dB
endproc
*/

/* drawing overwrites until Erase all */
/* pitch in readable format */

/* End of file manual_whatsnew.cpp */
