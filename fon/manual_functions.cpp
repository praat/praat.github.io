/* manual_functions.cpp
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

void manual_functions_init (ManPages me);
void manual_functions_init (ManPages me) {

MAN_PAGES_BEGIN R"~~~(
################################################################################
"Functions"
© Paul Boersma 2022-2026

A growing list of functions that you can use in @formulas and @scripting...

, @`abs` (%`x`) – absolute value
, @`abs#` (%`vector#`) – absolute value of each element of %`vector#`
, @`abs##` (%`matrix##`) – absolute value of each cell of %`matrix##`
, @`appDay` ( ) – the month day at which the present version of Praat was built
, @`appendFile` (%`filePath$`, `...`) – write texts, numbers, vectors and so on
	at the end of an existing file (create such a file if it does not exist yet)
, @`appendFileLine` (%`filePath$`, `...`) – write texts, numbers, vectors and so on,
	followed by a newline, at the end of an existing file
	(create such a file if it does not exist yet)
, @`appendInfo` (`...`) – write texts, numbers, vectors and so on,
	to the Info window
, @`appendInfoLine` (`...`) – write texts, numbers, vectors and so on,
	followed by a newline, to the Info window
, @`appMonth` ( ) – the month in which the present version of Praat was built (between 1 and 12)
, @`appMonth$` ( ) – the name of the month (in English) in which the present version of Praat was built
, @`appVersion` ( ) – the number of the present version of Praat
, @`appVersion$` ( ) – the present version of Praat, as text
, @`appYear` ( ) – the year in which the present version of Praat was built
, @`arccos` (%`x`) – inverse cosine
, @`arccos#` (%`vector#`) – inverse cosine of each element of %`vector#`
, @`arccos##` (%`matrix##`) – inverse cosine of each cell of %`matrix##`
, @`arccosh` (%`x`) – inverse hyperbolic cosine
, @`arccosh#` (%`vector#`) – inverse hyperbolic cosine of each element of %`vector#`
, @`arccosh##` (%`matrix##`) – inverse hyperbolic cosine of each cell of %`matrix##`
, @`arcsin` (%`x`) – inverse sine
, @`arcsin#` (%`vector#`) – inverse sine of each element of %`vector#`
, @`arcsin##` (%`matrix##`) – inverse sine of each cell of %`matrix##`
, @`arcsinh` (%`x`) – inverse hyperbolic sine
, @`arcsinh#` (%`vector#`) – inverse hyperbolic sine of each element of %`vector#`
, @`arcsinh##` (%`matrix##`) – inverse hyperbolic sine of each cell of %`matrix##`
, @`arctan` (%`x`) – inverse tangent
, @`arctan#` (%`vector#`) – inverse tangent of each element of %`vector#`
, @`arctan##` (%`matrix##`) – inverse tangent of each cell of %`matrix##`
, @`arctan2` (%`y`, %`x`) – argument angle
, @`arctanh` (%`x`) – inverse hyperbolic tangent
, @`arctanh#` (%`vector#`) – inverse hyperbolic tangent of each element of %`vector#`
, @`arctanh##` (%`matrix##`) – inverse hyperbolic tangent of each cell of %`matrix##`
, @`assert` %`condition` – condition checking
, @`asserterror` %`message$` – testing that a certain error occurs
, @`asynchronous` – let the script continue while the sound is playing or a subprocess is running
, @`backslashTrigraphsToUnicode$` (%`string$`) – convert e.g. \bsct to \ct
, @`barkToHertz` (%`x`) – from Bark-rate to acoustic frequency
, @`besselI` (%`n`, %`x`) – modified Bessel function of the first kind, %I__%n_
, @`besselK` (%`n`, %`x`) – modified Bessel function of the second kind, %K__%n_
, @`beta` (%`x`, %`y`) – \be function
, @`between_by#` (%`low`, %`high`, %`step`) – symmetrically dispersed numbers between %`low` and %`high`, in steps of %`step`
, @`between_count#` (%`low`, %`high`, %`n`) – %`n` symmetrically dispersed numbers between %`low` and %`high` (bin centres)
, @`binomialP` (%`p`, %`k`, %`n`) – probability of Bernoulli event occurring at most %`k` in %`n` times
, @`binomialQ` (%`p`, %`k`, %`n`) – probability of Bernoulli event occurring at least %`k` in %`n` times
, @`ceiling` (%x) – round up to integer
, @`ceiling#` (%`vector#`) – round up each element of %`vector#`
, @`ceiling##` (%`matrix##`) – round up each cell of %`matrix##`
, @`center` (%`v#`) – center (or centre) of gravity
, @`chiSquareP` (%`chiSquare`, %`df`) – area under the %\ci^2 curve up to %`chiSquare`
, @`chiSquareQ` (%`chiSquare`, %`df`) – area under the %\ci^2 curve after %`chiSquare`
, @`chooseFolder$` (%`title$`) – pops up a folder selection window
, @`chooseReadFile$` (%`title$`) – pops up a file selection window for opening (or appending to) an existing file
, @`chooseWriteFile$` (%`title$`, %`defaultFilename$`) – pops up a file selection window for saving to a new file
, @`clearinfo` – clear the Info window
, @`clock` ( ) – typically the number of seconds since system start-up
, @`col` – the columns number (of the current cell) in a Formula command
, @`col$` [%`i`] – the name of column %i of an object
, @`col#` (%`matrix##`, %`columnNumber`) - extract one column from a matrix
, @`columnSums#` (%`matrix##`)
, @`combine#` (...) - combine numbers, vectors and matrices into a new vector
, @`correlation` (%`u#`, %`v#`) – correlation (between \-m1 and +1) of two vectors
, @`cos` (%`x`) – cosine
, @`cos#` (%`vector#`) – cosine of each element of %`vector#`
, @`cos##` (%`matrix##`) – cosine of each cell of %`matrix##`
, @`cosh` (%x) – hyperbolic cosine
, @`cosh#` (%`vector#`) – hyperbolic cosine of each element of %`vector#`
, @`cosh##` (%`matrix##`) – hyperbolic cosine of each cell of %`matrix##`
, @`createFolder` (%`folderPath$`) – create a new folder, or do nothing if it already exists
, @`date$` ( ) – current local date and time in the form "Mon Nov  8 16:32:42 2021"
, @`date_iso$` ( ) – current local date and time in the form "2021-11-08T16:32:42+01:00"
, @`date#` ( ) – current local date and time in the form { 2021, 11, 8, 16, 32, 42 }
, @`date_utc$` ( ) – current standard date and time in the form "Mon Nov  8 15:32:42 2021"
, @`date_utc_iso$` ( ) – current standard date and time in the form "2021-11-08T15:32:42Z"
, @`date_utc#` ( ) – current standard date and time in the form { 2021, 11, 8, 15, 32, 42 }
, @`deleteFile` (%`filePath$`) – delete a file, or do nothing if it does not exist
, @`demo` – execute a graphics command in the Demo window (instead of in the Picture window)
, @`demoClicked` ( ) – determine whether the user clicked in the Demo window
, @`demoClickedIn` (%`xmin`, %`xmax`, %`ymmin`, %`ymax`) – determine whether the user clicked in a rectangular region in the Demo window
, @`demoCommandKeyPressed` ( ) – determine whether the user pressed the Command key when they typed into the Demo window
, @`demoInput` (%`characters$`) – determine whether the user typed (or clicked) the specified characters into the Demo window
, @`demoKey$` ( ) – determine the key that the user typed into the Demo window
, @`demoKeyPressed` ( ) – determine whether the user typed anything into the Demo window
, @`demoOptionKeyPressed` ( ) – determine whether the user pressed the Option key when they typed into the Demo window
, @`demoPeekInput` ( ) – see whether a user event (click or key press) is available in the Demo window
, @`demoShiftKeyPressed` ( ) – determine whether the user pressed the Shift key when they typed into the Demo window
, @`demoShow` ( ) – make all deferred drawing commands immediately visible
, @`demoWaitForInput` ( ) – wait for a user event (click or key press) in the Demo window
, @`demoWindowTitle` (%`newTitle$`) – change the title of the Demo window
, @`demoX` ( ) – the horizontal position of a mouse click in the Demo window
, @`demoY` ( ) – the vertical position of a mouse click in the Demo window
, @`differenceLimensToPhon` (%`x`) – from jnd-scale to perceptual loudness
, @`dx` – the horizontal distance between cells (i.e. the distance between columns) of an object
, @`dy` – the vertical distance between cells (i.e. the distance between rows) of an object
, @`editor` (%`editor($)`) – enter the environment of an editor (by number or by name)
, @`empty$#` (%`numberOfStrings`) – create an array of empty strings
, @`endeditor` – leave the environment of an editor
, @`endproc` – end a procedure definition
, @`endsWith` (%`string$`, %`part$`) – determine whether %`string$` ends in %`part$`
, @`endsWith_caseInsensitive` (%`string$`, %`part$`) – determine whether %`string$` ends in %`part$`
, @`environment$` (%`string$`) – look up a system environment string
, @`erb` (%`f`) – equivalent rectangular bandwidth for frequency %`f`
, @`erbToHertz` (%`x`) – from ERB-rate to acoustic frequency
, @`erf` (%`x`) – error function, the integral of the Gaussian
, @`erfc` (%`x`) – complement of the error function, i.e. 1 − erf (%`x`);
	this is a separate function because erf (%`x`) can be close to 1
, @`exitScript` (`...`) – show a message that tells the user why the script will be interrupted
, @`exp` (%`x`) – exponentiation, i.e. %e^%`x`
, @`exp#` (%`vector#`) – exponentiate each element of %`vector#`
, @`exp##` (%`matrix##`) – exponentiate each cell of %`matrix##`
, @`extractLine$` (%`string$`, %`part$`) – everything that comes after the first occurrence of %`part$` in %`string$`
, @`extractNumber` (%`string$`, %`part$`) – the first number after the first occurrence of %`part$% in %`string$`
, @`extractWord$` (%`string$`, %`part$`) – the first “word” after the first occurrence of %`part$% in %`string$`
, @`fileNames$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the files in a folder
	or that match a pattern with an asterisk
, @`fileNames_caseInsensitive$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the files in a folder
	or that match a pattern with an asterisk
, @`fileReadable` (%`filePath$`) – 1 if the file exists and can be read, 0 otherwise
, @`fisherP` (%`f`, %`df1`, %`df2`) – area under the Fisher %F curve up to %`f`
, @`fisherQ` (%`f`, %`df1`, %`df2`) – area under the Fisher %F curve after %`f`
, @`fixed$` (%`number`, %`precision`) – format a number as a string, with %`precision` digits after the decimal point
, @`fixed$` (%`vector#`, %`precision`) – format a vector as a string, with all elements expressed in %`precision` digits after the decimal point
, @`fixed$` (%`matrix##`, %`precision`) – format a matrix as a string, with all cells expressed in %`precision` digits after the decimal point
, @`floor` (%`x`) – round down to integer
, @`floor#` (%`vector#`) – round down each element of %`vector#`
, @`floor##` (%`matrix##`) – round down each cell of %`matrix##`
, @`folderExists` (%`folderPath$`) – 1 if the folder exists and can be read, 0 otherwise
, @`folderNames$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the subfolders in a folder
	or that match a pattern with an asterisk
, @`folderNames_caseInsensitive$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the subfolders in a folder
	or that match a pattern with an asterisk
, @`from_to#` (%`m`, %`n`) – the integers from %`m` through %`n`
, @`from_to_by#` (%`m`, %`n`, %`step`) – numbers from %`m` through %`n`, in steps of %`step`
, @`from_to_count#` (%`start`, %`end`, %`n`) – %`n` numbers from %`start` through %`end`
, @`gaussP` (%`z`) – area under the normal curve up to %`z`
, @`gaussQ` (%`z`) – area under the normal curve after %`z`
, @`goto` – place in a script to jump from
, @`hertzToBark` (%`x`) – from acoustic frequency to Bark-rate
, @`hertzToErb` (%`x`) – from acoustic frequency to ERB-rate
, @`hertzToMel` (%`x`) – from acoustic frequency to mel
, @`hertzToSemitones` (%`x`) – from acoustic frequency to logarithmic scale %re 100 Hz
, @`imax` (%`x`, `...`) – location of the maximum
, @`imin` (%`x`, `...`) – location of the minimum
, @`index` (%`string$`, %`part$`) – look up a substring, or 0 if not found
, @`index` (%`strings$#`, %`s$`) – look up whether and where %`s$` first occurs in %`strings$`
, @`index_caseInsensitive` (%`string$`, %`part$`) – look up a substring, or 0 if not found
, @`index_caseInsensitive` (%`strings$#`, %`s$`) – look up whether and where %`s$` first occurs in %`strings$`
, @`index_regex` (%`string$`, %`regex$`) – determine whether and where %`string$` first matches %`regex$`
, @`inner` (%`a#`, %`b#`) – inner product
, @`invBinomialP` (%`P`, %`k`, %`n`) – inverse of `binomialP`
, @`invBinomialQ` (%`Q`, %`k`, %`n`) – inverse of `binomialQ`
, @`invChiSquareQ` (%`q`, %`df`) – inverse of `chiSquareQ`
, @`invFisherQ` (%`q`, %`df1`, %`df2`) – inverse of `fisherQ`
, @`invGaussQ` (%`z`) – inverse of `gaussQ`
, @`invSigmoid` (%`x`) – inverse `sigmoid`
, @`invSigmoid#` (%`vector#`) – inverse sigmoid of each element of %`vector#`
, @`invSigmoid##` (%`matrix##`) – inverse sigmoid of each cell of %`matrix##`
, @`invStudentQ` (%`q`, %`df`) – inverse studentQ
, @`label` – place in a script to jump to
, @`left$` (%`string$`, %`n`) – the %n first characters in %`string$`
, @`length` (%`string$`) – number of Unicode characters in %`string$`
, @`ln` (%`x`) – natural logarithm, base %e
, @`ln#` (%`vector#`) – natural logarithm of each element of %`vector#`
, @`ln##` (%`matrix##`) – natural logarithm of each cell of %`matrix##`
, @`lnGamma` (%`x`) – logarithm of the \Ga function
, @`log2` (%`x`) – logarithm, base 2
, @`log2#` (%`vector#`) – base-2 logarithm of each element of %`vector#`
, @`log2##` (%`matrix##`) – base-2 logarithm of each cell of %`matrix##`
, @`log10` (%x) – logarithm, base 10
, @`log10#` (%`vector#`) – base-10 logarithm of each element of %`vector#`
, @`log10##` (%`matrix##`) – base-10 logarithm of each cell of %`matrix##`
, @`lowerCase$` (%`string$`) – turns all capitals into lower case
, @`lowerCaseAppName$` ( ) – the name of the app in lower snake case, e.g. `praat` or `praat_for_hospitals`
, @`max` (%`x`, `...`) – maximum
, @`mean` (%`v#`) – average of the elements of a vector
, @`melToHertz` (%`x`) – from mel to acoustic frequency
, @`mid$` (%`string$`, %`from`, %`n`) – the %`n` characters in %`string$` starting at position %`from`
, @`min` (%`x`, `...`) – minimum
, @`minusObject` (`...`) – shrink the selection of objects in the list
, @`moveAndOrRenameFile` (%`fromFilePath$`, %`toFilePath$`) – move and/or rename a file (note: supply two paths)
, @`mul##` (%`a##`, %`b##`) – matrix multiplication
, @`ncol` – the number of columns of an object
, @`nrow` – the number of rows of an object
, @`number` (%`a$`) – interpret a string as a number
, @`number#` (%`a$#`) – interpret strings as numbers
, @`numberOfColumns` (%`matrix##`)
, @`numberOfRows` (%`matrix##`)
, @`nx` – the number of cells horizontally (i.e. the number of columns) of an object
, @`ny` – the number of cells vertically (i.e. the number of rows) of an object
, @`outer##` (%`a#`, %`b#`) – outer product, i.e. %%result__ij_% = %%a__i_%%%b__j_%
, @`padLeft$` (%`string$`, %`minimumNewWidth` [, %`pad$`]) – left padding
, @`padOrTruncateLeft$` (%`string$`, %`newWidth` [, %`pad$`]) – left padding or truncation
, @`padOrTruncateRight$` (%`string$`, %`newWidth` [, %`pad$`]) – right padding or truncation
, @`padRight$` (%`string$`, %`minimumNewWidth` [, %`pad$`]) – right padding
, @`pauseScript` (`...`) – show a message in a simple @@pause window@
, @`percent$` (%`number`, %`precision`) – format a number as a string,
	with a trailing percent sign and %`precision` digits after the decimal point
, @`phonToDifferenceLimens` (%`x`) – from perceptual loudness to jnd-scale
, @`plusObject` (`...`) – extend the selection of objects in the list
, @`procedure`: %`procedureName`, `...` – start a procedure definition
, @`randomBernoulli` (%`p`) – Bernoulli-distributed random deviate (0 or 1)
, @`randomBernoulli#` (%`n`, %`p`) – %`n` independent Bernoulli-distributed zeroes and ones
, @`randomGamma` (%`shape`, %`rate`) – gamma-distributed random deviate
, @`randomGamma#` (%`n`, %`shape`, %`rate`) – %n independent gamma-distributed random numbers
, @`randomGamma#` (%`vector#`, %`shape`, %`rate`) – duplicate %`vector#`,
	and replace all elements with independent gamma-distributed random numbers
, @`randomGamma##` (%`nrow`, %`ncol`, %`shape`, %`rate`) – %nrow \xx %ncol independent gamma-distributed random numbers
, @`randomGamma##` (%`matrix##`, %`shape`, %`rate`) – duplicate %`matrix##`,
	and replace all cells with independent gamma-distributed random numbers
, @`randomGauss` (%`mu`, %`sigma`) – normally distributed random deviate
, @`randomGauss#` (%`n`, %`mu`, %`sigma`) – %n independent normally distributed random numbers
, @`randomGauss#` (%`vector#`, %`mu`, %`sigma`) – duplicate %`vector#`,
	and replace all elements with independent normally distributed random numbers
, @`randomGauss##` (%`nrow`, %`ncol`, %`mu`, %`sigma`) – %`nrow` \xx %`ncol` independent normally distributed random numbers
, @`randomGauss##` (%`matrix##`, %`mu`, %`sigma`) – duplicate %`matrix##`,
	and replace all cells with independent normally distributed random numbers
, @`randomImax` (%`vector#`) – sample from an array of relative probabilities
, @`randomInteger` (%`min`, %`max`) – uniformly distributed integer random deviate
, @`randomInteger#` (%`n`, %`min`, %`max`) – %`n` independent uniformly distributed random integers
, @`randomInteger#` (%`vector#`, %`min`, %`max`) – duplicate %`vector#`,
	and replace all elements with independent uniformly distributed random integers
, @`randomInteger##` (%`nrow`, %`ncol`, %`min`, %`max`) – %`nrow` \xx %`ncol` independent uniformly distributed random integers
, @`randomInteger##` (%`matrix##`, %`min`, %`max`) – duplicate %`matrix##`,
	and replace all cells with independent uniformly distributed random integers
, @`randomPoisson` (%`mean`) – Poisson-distributed random deviate
, @`randomPoisson#` (%`n`, %`mean`) – %`n` independent Poisson-distributed random numbers
, @`randomPoisson#` (%`vector#`, %`mean`) – duplicate %`vector#`,
	and replace all elements with independent Poisson-distributed random numbers
, @`randomPoisson##` (%nrow, %ncol, %`mean`) – %`nrow` \xx %`ncol` independent Poisson-distributed random numbers
, @`randomPoisson##` (%`matrix##`, %`mean`) – duplicate %`matrix##`,
	and replace all cells with independent Poisson-distributed random numbers
, @`randomUniform` (%`min`, %`max`) – uniformly distributed random deviate
, @`randomUniform#` (%`n`, %`min`, %`max`) – %n independent uniformly distributed random numbers
, @`randomUniform#` (%`vector#`, %min, %max) – duplicate %`vector#`,
	and replace all elements with independent uniformly distributed random numbers
, @`randomUniform##` (%`nrow`, %`ncol`, %`min`, %`max`) – %`nrow` \xx %`ncol` independent uniformly distributed random numbers
, @`randomUniform##` (%`matrix##`, %`min`, %`max`) – duplicate %`matrix##`,
	and replace all cells with independent uniformly distributed random numbers
, @`random_initializeSafelyAndUnpredictably` ( ) – undoes the effects of the following function
, @`random_initializeWithSeedUnsafelyButPredictably` (%`seed`) – reproducible sequence of random numbers
, @`readFile` (%`filePath$`) – read a number from a text file
, @`readFile$` (%`filePath$`) – read a whole text file into a string
, @`readFile#` (%`filePath$`) – read a vector from a text file
, @`readFile##` (%`filePath$`) – read a matrix from a text file
, @`readLinesFromFile$#` (%`filePath$`) – read all lines from a text file
, @`rectify` (%`x`) – set to zero if negative (no change if positive)
, @`rectify#` (%`vector#`) – rectify each element of %`vector#`
, @`rectify##` (%`matrix##`) – rectify each cell of %`matrix##`
, @`removeObject` (`...`) – remove objects from the list, by ID and/or name
, @`repeat#` (%`v#`, %`n`) – repeat the whole sequence of elements of %`v#` %`n` times
, @`replace$` (%`a$`, %`b$`, %`c$`, %`n`) – within %`a$`, replace the first %`n` occurrences of %`b$` with %`c$`, or all if %`n` = 0
, @`replace_regex$` (%`a$`, %`b$`, %`c$`, %`n`) – within %`a$`,
	replace the first %`n` matches of regular expression %`b$` with the regular expression %`c$`, or all if %`n` = 0
, @`right$` (%`string$`, %`n`) – the %n last characters in %`string$`
, @`rindex` (%`string$`, %`part$`) – look up a substring from the end, or 0 if not found
, @`rindex` (%`strings$#`, %`s$`) – look up whether and where %`s$` last occurs in %`strings$#`
, @`rindex_caseInsensitive` (%`string$`, %`part$`) – look up a substring from the end, or 0 if not found
, @`rindex_caseInsensitive` (%`strings$#`, %`s$`) – look up whether and where %`s$` last occurs in %`strings$#`
, @`rindex_regex` (%`string$`, %`regex$`) – determine whether and where %`string$` last matches %`regex$`
, @`round` (%`x`) – nearest integer
, @`round#` (%`vector#`) – nearest integer of each element of %`vector#`
, @`round##` (%`matrix##`) – nearest integer of each cell of %`matrix##`
, @`row` – the row number (of the current cell) in a Formula command
, @`row$` [%`i`] – the name of row %i of an object
, @`row#` (%`matrix##`, %`rowNumber`) - extract one row from a matrix
, @`rowSums#` (%`matrix##`)
, @`runScript` (%`filePath$`, `...`) – run a script with the given arguments
, @`runSubprocess` (%`executableFilePath$`, `...`) – run an external program with the given arguments
, @`runSubprocess$` (%`executableFilePath$`, `...`) – run an external program with the given arguments, and return its output
, @`runSystem` (`...`) – run a system command line with the given arguments concatenated
, @`runSystem$` (`...`) – run a system command line with the given arguments concatenated, and return its output
, @`selected` ( ) – the ID of the topmost selected object
, @`selected` (%`i`) – the ID of the %%i%th selected object (as counted from the top, or from the bottom if negative)
, @`selected` (%`type$`) – the ID of the topmost selected object of type %`type$`
, @`selected` (%`type$`, %`i`) – the ID of the %%i%th selected object of type %`type$` (as counted from the top,
  or from the bottom if negative)
, @`selected$` ( ) – the full name (type + given name) of the topmost selected object
, @`selected$` (%`i`) – the full name (type + given name) of the %%i%th selected object (as counted from the top,
  or from the bottom if negative)
, @`selected$` (%`type$`) – the given name of the topmost selected object of type %`type$`
, @`selected$` (%`type$`, %`i`) – the given name of the %%i%th selected object of type %`type$` (as counted from the top,
  or from the bottom if negative)
, @`selected#` ( ) – the IDs of all selected objects
, @`selected#` (%`type$`) – the IDs of all selected objects of type %`type$`
, @`selected$#` ( ) – the full names of all selected objects
, @`selected$#` (%`type$`) – the given names of all selected objects of type %`type$`
, @`selectObject` (`...`) – select object(s) in the list by ID and/or full name
, @`semitonesToHertz` (%`x`) – from logarithmic scale %re 100 Hz to acoustic frequency
, @`sigmoid` (%`x`) – 1 / (1 + %e^^-%`x`^)
, @`sigmoid#` (%`vector#`) – sigmoid of each element of %`vector#`
, @`sigmoid##` (%`matrix##`) – sigmoid of each cell of %`matrix##`
, @`sin` (%`x`) – sine
, @`sin#` (%`vector#`) – sine of each element of %`vector#`
, @`sin##` (%`matrix##`) – sine of each cell of %`matrix##`
, @`sinc` (%`x`) – sinus cardinalis: sin (%`x`) / %`x`
, @`sincpi` (%`x`) – sin (%\pi%x) / (%\pi%x)
, @`sinh` (%`x`) – hyperbolic sine
, @`sinh#` (%`vector#`) – hyperbolic sine of each element of %`vector#`
, @`sinh##` (%`matrix##`) – hyperbolic sine of each cell of %`matrix##`
, @`size` (%`v#`) – number of elements
, @`shuffle#` (%`vector#`) – randomize order
, @`shuffle$#` (%`stringVector$#`) – randomize order
, @`sleep` (%`duration`) – pause script for %`duration` seconds
, @`softmax#` (%`vector#`)
, @`softmaxPerRow##` (%`matrix##`)
, @`solve#` (%`a##`, %`y#`) – find #x for which #A · #x = #y
, @`solve##` (%`a##`, %`y##`) – find #X for which #A · #X = #Y
, @`solveNonnegative#` (%`a##`, %`y#` [, %`x`], %`maximumNumberOfIterations`, %`tolerance`, %`infoLevel`)
, @`solveSparse#` (%`a##`, %`y#` [, %`x`], %`maximumNumberOfNonzeros`, %`maximumNumberOfIterations`, %`tolerance`, %`infoLevel`)
, @`solveWeaklyConstrained#` (%`a##`, %`y#`, %`alpha`, %`delta`)
, @`sort#` (%`vector#`) – reshuffle in increasing order
, @`sort$#` (%`stringVector$#`) – reshuffle in Unicode-alphabetical order
, @`sort_numberAware$#` (%`stringVector$#`) – reshuffle in Unicode-alphabetical order, with special attention to numbers
, @`splitByWhitespace$#` (%`string$`) – split a string into inks
, @`sqrt` (%`x`) – square root
, @`sqrt#` (%`vector#`) – square root of each element of %`vector#`
, @`sqrt##` (%`matrix##`) – square root of each cell of %`matrix##`
, @`startsWith` (%`string$`, %`part$`) – determine whether %`string$` starts with %`part$`
, @`startsWith_caseInsensitive` (%`string$`, %`part$`) – determine whether %`string$` starts with %`part$`
, @`stdev` (%`v#`) – standard deviation of the elements of a vector
, @`stopwatch` – give the current value of the timer, which is then immediately reset to zero (note: no parentheses)
, @`string$` (%`number`) – format a number as a string
, @`studentP` (%`t`, %`df`) – area under the Student %T curve up to %`t`
, @`studentQ` (%`t`, %`df`) – area under the Student %T curve after %`t`
, @`sum` (%`v#`) – sum the elements
, @`sumOver` (%`loopVariable` `to` %`n`, %`numericExpressionOptionallyUsingLoopVariable`) – \Si
, @`tan` (%`x`) – tangent
, @`tan#` (%`vector#`) – tangent of each element of %`vector#`
, @`tan##` (%`matrix##`) – tangent of each cell of %`matrix##`
, @`tanh` (%`x`) – hyperbolic tangent
, @`tanh#` (%`vector#`) – hyperbolic tangent of each element of %`vector#`
, @`tanh##` (%`matrix##`) – hyperbolic tangent of each cell of %`matrix##`
, @`to#` (%`n`) – the integers 1 through %`n`
, @`transpose##` (%`matrix##`) – flip matrix along its diagonal
, @`truncateLeft$` (%`string$`, %`newMaximumWidth` [, %`pad$`]) – left truncation of %`string$`
, @`truncateRight$` (%`string$`, %`newMaximumWidth` [, %`pad$`]) – right truncation of %`string$`
, @`tryToAppendFile` (%`filePath$`) – check whether a file can be appended to
, @`tryToWriteFile` (%`filePath$`) – check whether a file can be written (destructive!)
, @`unicode` (%`c$`) – the Unicode codepoint number that corresponds to character %`c$`
, @`unicode$` (%`n`) – the character that corresponds to Unicode codepoint %`n`
, @`unicodeToBackslashTrigraphs$` (%`string$`) – convert e.g. \ct to \bsct
, @`upperCase$` (%`string$`) – turns all lower-case characters into capitals
, @`upperCaseAppName$` ( ) – the name of the app in upper camel case, e.g. `Praat` or `PraatForHospitals`
, @`variableExists` (%`variableName$`) – does the variable %`variableName$` exist?
, @`vertical$` (%`stringArray$#`) – format a string array vertically
, @`writeFile` (%`filePath$`, `...`) – create a new text file, and write texts, numbers, vectors and so on into it
, @`writeFileLine` (%`filePath$`, `...`) – create a new text file, write texts, numbers, vectors and so on into it, followed by a newline
, @`writeInfo` (`...`) – write texts, numbers, vectors and so on, to a clear Info window
, @`writeInfoLine` (`...`) – write texts, numbers, vectors and so on, followed by a newline, to a clear Info window
, @`x` – the horizontal place (in world coordinates) of the current cell in a Formula command
, @`xmax` – the top of the horizontal domain of an object
, @`xmin` – the bottom of the horizontal domain of an object
, @`y` – the vertical place (in world coordinates) of the current cell in a Formula command
, @`ymax` – the top of the vertical domain of an object
, @`ymin` – the bottom of the vertical domain of an object
, @`zero#` (%`n`) – vector with %`n` elements that are all 0
, @`zero##` (%`nrow`, %`ncol`) – matrix with %`nrow` \xx %`ncol` elements that are all 0

################################################################################
"`abs`"
© Paul Boersma 2023

A function that can be used in @@Formulas@. The absolute value.

Syntax and semantics
====================
#`abs` (%`x`)
: compute the absolute value of %`x`.

Examples
========
{
	writeInfoLine: \#{abs} (3.14), " ", \#{abs} (0), " ", \#{abs} (-789e-300)
}

################################################################################
"`abs#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`abs#` (%`v#`)
: compute the absolute value (@`abs`) of each element of the vector %`v#`.

Examples
========
{
	a# = \#{abs#} ({ -1, 5.5, 0, -456.789 })
	writeInfoLine: a#
}

################################################################################
"`abs##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`abs##` (%`m##`)
: compute the absolute value (@`abs`) of each cell of the matrix %`m##`.

Examples
========
{
	a## = \#{abs##} ({{ -1, 5.5, 0, -456.789 }, { 34, -45, 18.0, 0 },
	... { 1e-89, -3.2e-307, 6e200, -2345e-2 }})
	writeInfoLine: a##
}

################################################################################
"`appDay`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The month day on which the present version of Praat was built
(a number between 1 and 31).

Syntax and semantics
====================
#`appDay` ( )
: give the month day of the creation of the present version of Praat.

Examples
========
{
	writeInfoLine: \#{appDay} ( )
}

################################################################################
"`appendFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendFile` (%`filePath$`, `...`)
: write texts, numbers, vectors and so on
	at the end of an existing file (create such a file if it does not exist yet)

################################################################################
"`appendFileLine`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendFileLine` (%`filePath$`, `...`)
: write texts, numbers, vectors and so on, followed by a newline,
	at the end of an existing file (create such a file if it does not exist yet)

################################################################################
"`appendInfo`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendInfo` (%`filePath$`, `...`)
: write texts, numbers, vectors and so on
	to the end of the Info window

################################################################################
"`appendInfoLine`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendInfoLine` (%`filePath$`, `...`)
: write texts, numbers, vectors and so on, followed by a newline,
	to the end of the Info window

################################################################################
"`appMonth`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The number of the month in which the present version of Praat was built
(a number between 1 and 12).

Syntax and semantics
====================
#`appMonth` ( )
: give the month of the creation of the present version of Praat, as a number.

Examples
========
{
	writeInfoLine: \#{appMonth} ( )
}

################################################################################
"`appMonth$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The name of the month in which the present version of Praat was built.

Syntax and semantics
====================
#`appMonth$` ( )
: give the month of the creation of the present version of Praat, as text.

Examples
========
{
	writeInfoLine: \#{appMonth$} ( )
}

################################################################################
"`appVersion`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The present version of Praat, as a number.

Syntax and semantics
====================
#`appVersion` ( )
: give the present version of Praat, as a number.

Examples
========
{
	writeInfoLine: \#{appVersion} ( )
}

################################################################################
"`appVersion$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The present version of Praat, as text.

Syntax and semantics
====================
#`appVersion$` ( )
: give the present version of Praat, as text.

Examples
========
{
	writeInfoLine: \#{appVersion$} ( )
}

################################################################################
"`appYear`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The year in which the present version of Praat was built.

Syntax and semantics
====================
#`appYear` ( )
: give the year of the creation of the present version of Praat.

Examples
========
{
	writeInfoLine: \#{appYear} ( )
}

################################################################################
"`arccos`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccos` (%`x`)
: compute the inverse cosine of %`x`.

Domain
======
#`arccos` (%x) is defined for -1 \<_ %x \<_ 1.

Tests
=====
{
	\`{assert} \#{arccos} (-1.1) = undefined
	\`{assert} \`{abs} (\#{arccos} (-1.0) - pi) < 1e-17
	\`{assert} \`{abs} (\#{arccos} (0.0) - pi/2) < 1e-17
	\`{assert} \#{arccos} (1.0) = 0
	\`{assert} \#{arccos} (1.1) = undefined
	\`{assert} \#{arccos} (undefined) = undefined
}

################################################################################
"`arccos#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccos#` (%`v#`)
: compute the inverse cosine (@`arccos`) of each element of the vector %`v#`.

Examples
========
{
	writeInfoLine: \#{arccos#} ({ -1.1, -1.0, 0.0, 1.0, 1.1, undefined })
}

################################################################################
"`arccos##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccos##` (%`m##`)
: compute the inverse cosine (@`arccos`) of each cell of the matrix %`m##`.

################################################################################
"`arccosh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccosh` (%`x`)
: compute the inverse hyperbolic cosine of %`x`.

Definition
==========
~	#`arccosh` (%x) = ln (%x + \Vr(%x^2−1))

Tests
=====
{
	\`{assert} \#{arccosh} (1.0) = 0
	\`{assert} \#{arccosh} (0.9) = undefined
	\`{assert} \#{arccosh} (0.0) = undefined
	\`{assert} \#{arccosh} (-10.0) = undefined
	\`{assert} \#{arccosh} (undefined) = undefined
}

################################################################################
"`arccosh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccosh#` (%`v#`)
: compute the inverse hyperbolic cosine (@`arccosh`) of each element of the vector %`v#`.

################################################################################
"`arccosh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arccosh##` (%`m##`)
: compute the inverse hyperbolic cosine (@`arccosh`) of each cell of the matrix %`m##`.

################################################################################
"`arcsin`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsin` (%`x`)
: compute the inverse sine of %`x`.

Domain
======
#`arcsin` (%x) is defined for -1 \<_ %x \<_ 1.

Tests
=====
{
	\`{assert} \#{arcsin} (-1.1) = undefined
	\`{assert} \`{abs} (\#{arcsin} (-1.0) - -pi/2) < 1e-17
	\`{assert} \#{arcsin} (0.0) = 0
	\`{assert} \`{abs} (\#{arcsin} (1.0) - pi/2) < 1e-17
	\`{assert} \#{arcsin} (1.1) = undefined
	\`{assert} \#{arcsin} (undefined) = undefined
}

################################################################################
"`arcsin#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsin#` (%`v#`)
: compute the inverse sine (@`arcsin`) of each element of the vector %`v#`.

################################################################################
"`arcsin##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsin##` (%`m##`)
: compute the inverse sine (@`arcsin`) of each cell of the matrix %`m##`.

################################################################################
"`arcsinh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsinh` (%`x`)
: compute the inverse hyperbolic sine of %`x`.

Definition
==========
~	#`arcsinh` (%x) = ln (%x + \Vr(1+%x^2))

Tests
=====
{
	\`{assert} \#{arcsinh} (-1.0) < 0
	\`{assert} \#{arcsinh} (0.0) = 0
	\`{assert} \#{arcsinh} (1.0) > 0
	\`{assert} \#{arcsinh} (undefined) = undefined
}

################################################################################
"`arcsinh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsinh#` (%`v#`)
: compute the inverse hyperbolic sine (@`arcsinh`) of each element of the vector %`v#`.

################################################################################
"`arcsinh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arcsin##` (%`m##`)
: compute the inverse hyperbolic sine (@`arcsinh`) of each cell of the matrix %`m##`.

################################################################################
"`arctan`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arctan` (%`x`)
: compute the inverse tangent of (%`x`).

Tests
=====
{
	\`{assert} \#{arctan} (0.0) = 0
}

################################################################################
"`arctan#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arctan#` (%`v#`)
: compute the inverse tangent (@`arctan`) of each element of the vector %`v#`.

################################################################################
"`arctan##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arctan##` (%`m##`)
: compute the inverse tangent (@`arctan`) of each cell of the matrix %`m##`.

################################################################################
"`arctan2`"
© Paul Boersma 2023

A function that can be used in @@Formulas@. The argument angle.

Syntax and semantics
====================
#`arctan2` (%`y`, %`x`)
:	the angle (in radians) from the %X axis to the point (%x, %y) in two-dimensional space;
	positive (i.e. measured counterclockwise) for %y > 0,
	and negative (and measured clockwise) for %y < 0.
	For %y = 0, it’s 0 if %x > 0, and %\pi if %x < 0.
	In the point (0, 0), `arctan2` is undefined.

Tests
=====
{
	\`{assert} \#{arctan2} (0.0, 3.0) = 0
	\`{assert} \`{abs} (\#{arctan2} (3.0, 3.0) - pi/4) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (3.0, 0.0) - pi/2) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (3.0, -3.0) - 3*pi/4) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (0.0, -3.0) - pi) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (-3.0, -3.0) + 3*pi/4) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (-3.0, 0.0) + pi/2) < 1e-17
	\`{assert} \`{abs} (\#{arctan2} (-3.0, 3.0) + pi/4) < 1e-17
	;\`{assert} \#{arctan2} (0.0, 0.0) = undefined
}
Edge cases
==========
In C, and therefore in Numpy, `arctan2` has arbitrary values
where it cannot be defined. Thus `numpy.arctan2 (0.0, +0.0)` is 0,
`numpy.arctan2 (+0.0, -0.0)` is +%\pi,
and `numpy.arctan2 (-0.0, -0.0)` is \-m%\pi.
The function is then even defined for e.g. %x = \-m∞ and %y = +∞,
where it is 3%\pi/4. In all these cases, `arctan` returns `undefined` in Praat.
This doesn’t mean that the function is very sane in Praat, as
it is still discontinuous for small %`y`:
{
	writeInfoLine: \#{arctan2} (1e-308, -1.0), " ", \#{arctan2} (-1e-308, -1.0)
}
which are indistinguishable from +%\pi and \-m%\pi, respectively.

################################################################################
"`arctanh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arctanh` (%`x`)
: compute the inverse hyperbolic tangent of %`x`.

Tests
=====
{
	\`{assert} \#{arctanh} (-1.0) = undefined
	\`{assert} \#{arctanh} (0.0) = 0
	\`{assert} \#{arctanh} (1.0) = undefined
	\`{assert} \#{arctanh} (1000) = undefined
	\`{assert} \#{arctanh} (undefined) = undefined
}

################################################################################
"`arctanh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

#`arctanh#` (%`v#`)
: compute the inverse hyperbolic tangent (@`arctanh`) of each element of the vector %`v#`.

################################################################################
"`arctanh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`arctanh##` (%`m##`)
: compute the inverse hyperbolic tangent (@`arctanh`) of each cell of the matrix %`m##`.

################################################################################
"`assert`"
© Paul Boersma 2023

A keyword that can be used in @Scripting, to test whether an expected condition holds.

Examples
========

This is an assertion that is satisfied:
{
	a = 5*6
	\#{assert} a = 30
	writeInfoLine: a
}
This is an assertion that is violated:
{;
	a = 5*6
	\#{assert} a < 10
	writeInfoLine: a
}
This three-line script stops at the second line with the error message
“`Assertion violated in line 2: a < 10`”.

That is, because %`a` is not less than 10 (it’s 30),
the assertion is false, and the script stops, mentioning the line number
as well as the text of the assertion (i.e. “`a < 10`”).

################################################################################
"`asserterror`"
© Paul Boersma 2023,2026

A keyword that can be used in @Scripting, to test whether an expected error message is indeed generated.

Examples
========

{
	\#{asserterror} Unknown variable
	a = 2 * b + 3
}
The line `a = 2 * b + 3` contains the unknown variable `b`. Therefore, the script would normally issue an error message like
`
	Unknown variable:
	« a = 2 * b

	Script line 2 not performed or completed:
		« a = 2 * b + 3 »
	Menu command “Run” not completed.
`
The line with #`asserterror` prevents the error message from being popped up, and instead checks
that the error message contains the string “Unknown variable”, which it does.

This one works differently:
{;
	b = 4
	\#{asserterror} Unknown variable
	a = 2 * b + 3
}
In this case the expected error message does not occur, and the script exits with the following error message:
`
	Script assertion fails in line 2: error « Unknown variable » not raised.
	Instead: no error.

	Script line 3 not performed or completed:
		« a = 2 * b + 3 »
	Menu command “Run” not completed.
`
Another case:
{;
	\#{asserterror} What is blabla
	a = Create Sound from blabla: 1, 2, 3
}
In this case a different error message than the expected error message occurs, and the script exits with the following error message:
`
	Script assertion fails in line 1: error « What is blabla » not raised.
	Instead: Command “Create Sound from blabla:” not available for current selection.

	Script line 2 not performed or completed:
		« a = Create Sound from blabla: 1, 2, 3 »
	Menu command “Run” not completed.
`

################################################################################
"`asynchronous`"
© Paul Boersma 2023,2026

A keyword that can be used in @Scripting, to let a script continue while a sound is playing:
{;
	\#{asynchronous} \@{Sound: ||Play}
}
or to let a script continue while a subprocess is running:
{;
	\#{asynchronous} \`{runSubprocess}: “C://apps/videoViewer.exe”, “hello.wav”
}
or
{;
	\#{asynchronous} \`{runSystem}: “C://apps/videoViewer.exe hello.wav”
}

################################################################################
"`backslashTrigraphsToUnicode$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`backslashTrigraphsToUnicode$` (%`string$`)
: convert from backslash trigraphs to unicode characters,
e.g. from “\bsct” to “\ct”.

################################################################################
"`barkToHertz`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`barkToHertz` (%`x`)
: convert from an auditory Bark-rate value %`x` to an acoustic frequency in Hz.

Definition
==========
~ #`barkToHertz` (%x) = 650 @`sinh` (%x / 7)

################################################################################
"`besselI`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`besselI` (%`n`, %`x`)
: compute the %`n`th modified Bessel function of the first kind (%I_%`n`) of %`x`.

################################################################################
"`besselK`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`besselK` (%`n`, %`x`)
: compute the %`n`th modified Bessel function of the second kind (%K_%`n`) of %`x`.

################################################################################
"`beta`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`beta` (%`x`, %`y`)
: compute the %\be function.

################################################################################
"`between_by#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`between_by#` (%`low`, %`high`, %`step`)
: compute symmetrically dispersed numbers between %`low` and %`high`, in steps of %`step`.

################################################################################
"`between_count#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`between_count#` (%`low`, %`high`, %`n`)
: compute %`n` symmetrically dispersed numbers between %`low` and %`high` (bin centres).

################################################################################
"`binomialP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@. The cumulative binomial distribution.

Syntax and semantics
====================
#`binomialP` (%`p`, %`k`, %`n`)
: compute the probability that in %`n` trials an event with probability %`p` will occur at most %`k` times.

Related function
================
It is probably more common to use the complement of this function, namely @`binomialQ`.

################################################################################
"`binomialQ`"
© Paul Boersma 2014, 2023

A function that can be used in @@Formulas@. The complement of the cumulative binomial distribution.

Syntax and semantics
====================
#`binomialQ` (%`p`, %`k`, %`n`)
: compute the probability that in %`n` trials an event with probability %`p` will occur at least %`k` times.

Calculator example
==================

A die is suspected to yield more sixes than a perfect die would do. In order to test this suspicion,
you throw it 1,000 times. The result is 211 sixes.

The probability that a perfect die yields at least 211 sixes is, according to @@Calculator...@,
{
	appendInfoLine: \#{binomialQ} (1/6, 211, 1000)
}
which should be 0.000152.

Script example
==============

You convert 1000 values of pitch targets in Hz to the nearest note on the piano keyboard.
597 of those values turn out to be in the A, B, C, D, E, F, or G regions (the white keys), and 403 values turn out
to be in the A\# , C\# , D\# , F\# , or G\#  regions (the black keys).
Do our subjects have a preference for the white keys?
The following script computes the probability that in the case of no preference the subjects
would target the white keys at least 597 times. This is compared with a %\ci^2 test.
{
	a = 597
	b = 403
	p = 7/12   ; no preference
	writeInfoLine: “*** Binomial test ”, a, “, ”, b, “, p = ”, fixed$ (p, 6), “ ***”
	pbin = \#{binomialQ} (p, a, a+b)
	appendInfoLine: “P (binomial) = ”, fixed$ (pbin, 6)
	# Chi-square test with Yates correction:
	x2 = (a - 1/2 -   p   * (a+b)) ^ 2 / (  p   * (a+b)) +
	...  (b + 1/2 - (1-p) * (a+b)) ^ 2 / ((1-p) * (a+b))
	px2 = chiSquareQ (x2, 1)
	appendInfoLine: “P (chi-square) = ”, fixed$ (px2, 6)
}
The result is:
`
	*** Binomial test 597, 403, p = 0.583333 ***
	P (binomial) = 0.199330
	P (chi-square) = 0.398365
`
The %\ci^2 test is two-sided (it signals a preference for the white or for the black keys),
so it has twice the probability of the binomial test.

We cannot conclude from this test that people have a preference for the white keys.
Of course, we cannot conclude either that people %%don’t% have such a preference.

################################################################################
"`ceiling`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ceiling` (%`x`)
: round %`x` up to the nearest integer number.

################################################################################
"`ceiling#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ceiling#` (%`v#`)
: round up (@`ceiling`) each element of the vector %`v#`.

################################################################################
"`ceiling##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ceiling##` (%`m##`)
: round up (@`ceiling`) each cell of the matrix %`m##`.

################################################################################
"`center`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`center` (%`v#`)
: compute the center (or centre) of gravity of the vector %`v#`.

################################################################################
"`chiSquareP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`chiSquareP` (%`chiSquare`, %`df`)
: compute the area under the %\ci^2 distribution between 0 and %`chiSquare`,
for %`df` degrees of freedom.

################################################################################
"`chiSquareQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`chiSquareQ` (%`chiSquare`, %`df`)
: compute the area under the %\ci^2 curve between %`chiSquare` and +\oo,
for %`df` degrees of freedom, i.e. the “statistical significance %p”
of the %\ci^2 difference between two distributions in %`df`+1 dimensions.

################################################################################
"`chooseFolder$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`chooseFolder$` (%`title$`)
: pop up a folder selection window.

################################################################################
"`chooseReadFile$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`chooseReadFile$` (%`title$`)
: pop up a file selection window for opening (or appending to) an existing file.

################################################################################
"`chooseWriteFile$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`chooseWriteFile$` (%`title$`, %`defaultFilename$`)
: pop up a file selection window for saving to a new file.

################################################################################
"`clearinfo`"
© Paul Boersma 2023

A command that can be used in @Scripting.

Syntax and semantics
====================
#`clearinfo`
: clear the @@Info window@.

################################################################################
"`clock`"
© Paul Boersma 2025

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`clock` ( )
: the number of seconds since system start-up, with microsecond precision.

Examples
========
{
	t = \#{clock}()
	for i to 1000
	endfor
	\`{appendInfoLine}: "Time elapsed: ", \#{clock}() - t, " seconds"
}
{
	t = \#{clock}()
	for i to 1000
	endfor
	timeElapsed = \#{clock}() - t
	\`{appendInfoLine}: "Time elapsed: ", \`{fixed$} (1000 * timeElapsed, 3), " ms"
}

################################################################################
"`col`"
© Paul Boersma 2024

The column number (of the current cell) in a Formula command.

See @@Formulas 1.7. Formulas for modification@.

################################################################################
"`col$`"
© Paul Boersma 2024

The name of a column of an object.

Syntax and semantics
====================
#`col$` [%`i`]
: the name of the %%i%th column of an object.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`col#`"
© Paul Boersma 2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`col#` (%`m##`, %`i`)
: extract the %`i`th column from the matrix %`m##`.

Examples
========
{
	mat## = {{ 7, 4 }, { 99, 103 }}
	\`{assert} col# (mat##, 2) = { 4, 103 }
}

################################################################################
"`columnSums#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`columnSums#` (%`m##`)
: compute the sum of the elements of each row of the matrix %`m##`.

################################################################################
"`combine#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`combine#` (...)
: combine numbers, vectors and matrices into a new vector.

Examples
========
{
	\`{assert} \#{combine#} (7, 4, 99) = { 7, 4, 99 }
	\`{assert} \#{combine#} ({ 7, 4 }, 99) = { 7, 4, 99 }
	\`{assert} \#{combine#} ({ 7, 4 }, { 99, 103, 1 }) = { 7, 4, 99, 103, 1 }
	\`{assert} \#{combine#} ({{ 7, 4 }, { 99, 103 }}, 1, { 8, -1, 2 }) =
	... { 7, 4, 99, 103, 1, 8, -1, 2 }
}

################################################################################
"`correlation`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`correlation` (%`u#`, %`v#`)
: compute the correlation between the vectors %`u#` and %`v#`.

Precondition
============
The vectors %`u#` and %`v#` have to have equal numbers of elements.

Definition
==========
If %N is the number of elements of both #u and #v,
~	%m_%u = (1/%N) \su__%i=1_^%N %u_%i
~	%m_%v = (1/%N) \su__%i=1_^%N %v_%i
~	%ssq_%u = \su__%i=1_^%N (%u_%i \-m %m_%u)^2
~	%ssq_%v = \su__%i=1_^%N (%v_%i \-m %m_%v)^2
~	`correlation` (#u, #v) = \su__%i=1_^%N (%u_%i \-m %m_%u) · (%v_%i \-m %m_%v) /
	\Vr(%ssq_%u · %ssq_%v)

################################################################################
"`cos`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cos` (%`x`)
: compute the cosine of %`x`.
{
	Axes: -5, 5, -1.1, 1.1
	Draw inner box
	Draw function: -5, 5, 1000, ~ \#{cos} (x)
	One mark bottom: -3*pi/2, "no", "yes", "yes", "\-m3%\pi/2"
	One mark bottom: -pi, "no", "yes", "yes", "\-m%\pi"
	One mark bottom: -pi/2, "no", "yes", "yes", "\-m%\pi/2"
	One mark bottom: 0, "no", "yes", "yes", "0"
	One mark bottom: pi/2, "no", "yes", "yes", "+%\pi/2"
	One mark bottom: pi, "no", "yes", "yes", "+%\pi"
	One mark bottom: 3*pi/2, "no", "yes", "yes", "+3%\pi/2"
	One mark left: -1, "no", "yes", "yes", "\-m1"
	One mark left: 0, "no", "yes", "yes", "0"
	One mark left: 1, "no", "yes", "yes", "+1"
	Text bottom: "yes", "%x \->"
	Text left: "yes", "##\#{cos}# (%x)"
	Text top: "no", "##just over one and a half periods of a (co)sine wave"
}
This picture illustrates the following properties of the cosine function:

- #`cos` (%x) is zero for all values of %x that are odd multiples of %\pi/2.
- The extrema of #`cos` (%x) are \-m1 and +1.
- #`cos` (%x) is \-m1 or +1 wherever %x is an integer multiple of %\pi.
- The cosine function is %periodic with period 2%\pi, i.e., #`cos` (%x+2%\pi) = #`cos` (%x).
- The cosine function is %symmetric around 0, i.e. #`cos` (\-m%x) = #`cos` (%x).
- The cosine function is %antisymmetric around %\pi/2, i.e. #`cos` (%\pi/2\-m%x) = \-m #`cos` (%x);
  in other words, if you take the cosine curve and rotate it by 180 degrees
  around the point (%\pi/2, 0), then you get the same curve again.

A cosine wave is a special case of the more general %%sine wave% that is given by
%y = @`sin` (%x + %\fi), where %\fi (an additive constant called the %%initial phase%) is %\pi/2.

################################################################################
"`cos#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cos#` (%`v#`)
: compute the cosine (@`cos`) of each element of the vector %`v#`.

################################################################################
"`cos##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cos##` (%`m##`)
: compute the cosine (@`cos`) of each cell of the matrix %`m##`.

################################################################################
"`cosh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cosh` (%`x`)
: compute the hyperbolic cosine of %`x`, i.e. (%e^%x + %e^^-%x^) / 2.

################################################################################
"`cosh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cosh#` (%`v#`)
: compute the hyperbolic cosine (@`cosh`) of each element of the vector %`v#`.

################################################################################
"`cosh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`cosh##` (%`m##`)
: compute the hyperbolic cosine (@`cosh`) of each cell of the matrix %`m##`.

################################################################################
"`createFolder`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`createFolder` (%`folderPath$`)
: create a new folder, or do nothing if the folder already exists.

################################################################################
"`date$`"
© Paul Boersma 2003,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date$` ( )
: format the current local date and time in a human-readable format.
{
	writeInfoLine: date$ ()
}

################################################################################
"`date_iso$`"
© Paul Boersma 2021,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date_iso$` ( )
: format the current local date and time in ISO 8601 format.
{
	writeInfoLine: date_iso$ ()
}

################################################################################
"`date#`"
© Paul Boersma 2021,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date#` ( )
: format the current local date and time as a 6-element vector containing
the year, the month number, the day, the hours, the minutes, and the seconds.
{
	writeInfoLine: date# ()
}

################################################################################
"`date_utc$`"
© Paul Boersma 2021,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date_utc$` ( )
: format the current Coordinated Universal Time in a human-readable format.
{
	writeInfoLine: date_utc$ ()
}

################################################################################
"`date_utc_iso$`"
© Paul Boersma 2021,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date_utc_iso$` ( )
: format the current Coordinated Universal Time in a ISO 8601 format.
{
	writeInfoLine: date_utc_iso$ ()
}

################################################################################
"`date_utc#`"
© Paul Boersma 2021,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`date#` ( )
: format the current Coordinated Universal Time as a 6-element vector containing
the year, the month number, the day, the hours, the minutes, and the seconds.
{
	writeInfoLine: date_utc# ()
}

################################################################################
"`deleteFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`deleteFile` (%`filePath$`)
: delete a file, or do nothing if the file does not exist.

################################################################################
"`demo`"
© Paul Boersma 2023

A keyword that can be used in @Scripting,
for executing a graphics command in the Demo window (instead of in the Picture window).

Example
=======
The following code draws a box into the Picture window:
{
	\@{Erase all}
	\@{Draw inner box}
}
The following code draws a box into the Demo window instead:
{;
	\#{demo} \@{Erase all}
	\#{demo} \@{Draw inner box}
}

More examples
=============

################################################################################
"`demoClicked`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoClicked` ( )
: determine whether the user clicked in the Demo window.

Usage
=====
This function is usually called after a user event has been detected (with @`demoWaitForInput`),
and is typically followed by determing the location of the click
(with @`demoX`) and @`demoY`, or with @`demoClickedIn`).

################################################################################
"`demoClickedIn`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoClickedIn` (%`xmin`, %`xmax`, %`ymmin`, %`ymax`)
: determine whether the user clicked in a rectangular region in the Demo window.

Usage
=====
This function is usually called after it has been determined that the user clicked into the Demo window,
i.e. after @`demoClicked` has returned 1.

################################################################################
"`demoCommandKeyPressed`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoCommandKeyPressed` ( )
: determine whether the user pressed the Command key when they typed into the Demo window.

Usage
=====
This function is usually called after it has been determined that the user pressed a key,
i.e. after @`demoKeyPressed` has returned 1.

################################################################################
"`demoInput`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoInput` (%`characters$`)
: determine whether the user typed (or clicked) the specified characters into the Demo window.

################################################################################
"`demoKey$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoKey$` ( )
: determine the key that the user typed into the Demo window.

################################################################################
"`demoKeyPressed`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoKeyPressed` ( )
: determine whether the user typed anything into the Demo window.

Usage
=====
This function is usually called after a user event has been detected (with @`demoWaitForInput`),
and is typically followed by determing the key that was pressed
(with @`demoKey$`), perhaps while determining which modifier keys were pressed
(with @`demoShiftKeyPressed`, @`demoOptionKeyPressed`, and/or @`demoCommandKeyPressed`).

################################################################################
"`demoShow`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoShow` ( )
: make all deferred drawing commands immediately visible.

Usage
=====
This function is used in animation, where you cannot wait until @`demoWaitForInput`
causes the drawings to become visible.

################################################################################
"`demoOptionKeyPressed`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoOptionKeyPressed` ( )
: determine whether the user pressed the Option key when they typed into the Demo window.

Usage
=====
This function is usually called after it has been determined that the user pressed a key,
i.e. after @`demoKeyPressed` has returned 1.

################################################################################
"`demoPeekInput`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoPeekInput` ( )
: see whether a user event (click or key press) is available in the Demo window.

################################################################################
"`demoShiftKeyPressed`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoShiftKeyPressed` ( )
: determine whether the user pressed the Shift key when they typed into the Demo window.

Usage
=====
This function is usually called after it has been determined that the user pressed a key,
i.e. after @`demoKeyPressed` has returned 1.

################################################################################
"`demoWaitForInput`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoWaitForInput` ( )
: wait for a user event (click or key press) in the Demo window.

Behaviour
=========
This function blocks until the user clicks in the Demo window or types a key into the Demo window.

Return value
============
Once the function finally returns, it returns the value 1 (true).
This makes it possible to use this function in a `while` loop:
{;
	while \#`{demoWaitForInput} ()
		if \`{demoClicked} ())
			# handle the mouse click
		elsif \`{demoKeyPressed} ())
			# handle the key press
		endif
	endwhile
}

################################################################################
"`demoWindowTitle`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoWindowTitle` (%`newTitle$`)
: change the title of the Demo window to %`newTitle$`.

################################################################################
"`demoX`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoX` ( )
: the horizontal position of a mouse click in the Demo window.

################################################################################
"`demoY`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`demoY` ( )
: the vertical position of a mouse click in the Demo window.

################################################################################
"`differenceLimensToPhon`"
© Paul Boersma 2002,2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`differenceLimensToPhon` (%`ndli`)
: convert auditory intensity difference limens into a sensation level in phon units.

Definition
==========
~	#`differenceLimensToPhon` (%ndli) = @`ln` (1 + %ndli / 30) / @`ln` (61 / 60)

Related function
================
#`differenceLimensToPhon` is the inverse of @`phonToDifferenceLimens`.

################################################################################
"`dx`"
© Paul Boersma 2024

The horizontal distance between cells (i.e. the distance between columns) of an object,
in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`dy`"
© Paul Boersma 2024

The vertical distance between cells (i.e. the distance between rows) of an object,
in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`editor`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`editor` (%`editorID`)
: enter the environment of the editor whose ID is %`editorID`.

#`editor` (%`editorName$`)
: enter the environment of the editor whose name is %`editorName$`.

For an introduction, see @@Scripting 7. Scripting the editors@.

################################################################################
"`empty$#`"
© Paul Boersma 2002, 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`empty$#` (%`numberOfStrings`)
: create an array of empty strings.

################################################################################
"`endeditor`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`endeditor`
: leave the environment of the current editor switching back to the @@List of Objects@.

For an introduction, see @@Scripting 7. Scripting the editors@.

################################################################################
"`endproc`"
© Paul Boersma 2023

A keyword that can be used in @Scripting.

Syntax and semantics
====================
#`endproc`
: finish the definition of a procedure.

For an introduction, see @@Scripting 5.5. Procedures@.

################################################################################
"`endsWith`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`endsWith` (%`string$`, %`part$`)
: determine whether %`string$` ends in %`part$`.

See also
========
- @`startsWith` for determining whether %`string$` %starts with %`part$`.
- @`endsWith_caseInsensitive` for %%case-insensitive% matching.

################################################################################
"`endsWith_caseInsensitive`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`endsWith_caseInsensitive` (%`string$`, %`part$`)
: determine whether %`string$` ends in %`part$`.

See also
========
- @`startsWith_caseInsensitive` for determining whether %`string$` %starts with %`part$`.
- @`endsWith` for %%case-sensitive% matching.

################################################################################
"`environment$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`environment$` (%`string$`)
: look up a system environment string.

For details and examples, see @@Scripting 6.5. Calling system commands@.

################################################################################
"`erb`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erb` (%`f`)
: compute the auditory %%equivalent rectangular bandwidth% (ERB) in hertz,
for a specified acoustic frequency %f (also in hertz).

Definition
==========
~	#`erb` (%f) = 6.23\.c10^^\-m6^ %f^2 + 0.09339 %f + 28.52

################################################################################
"`erbToHertz`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erbToHertz` (%`x`)
: convert from an auditory ERB-rate %`x` to an acoustic frequency in Hz.

Definition
==========
~	#`erbToHertz` (%x) (14680 %d - 312) / (1 - %d), where %d = exp ((%x - 43) / 11.17)

################################################################################
"`erf`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erf` (%`x`)
: compute the “error function”, the integral of the Gaussian, i.e.

Definition
==========
~	#`erf` (%x) = 2/\Vr%\pi _0\in^%x exp(-%t^2) %dt

Related function
================
The complement of #`erf` is @`erfc`.

################################################################################
"`erfc`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erfc` (%`x`)
: compute the complement of the error function

Definition
==========
#`erfc` (%x) = 1 − @`erf` (%x)

Why isn’t this function superfluous?
====================================
Because of floating-point rounding errors:
although one might think that @`erf` ic sufficient,
#`erfc` is a separate function because @`erf` (%`x`) can be close to 1.

################################################################################
"`exitScript`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`exitScript` (`...`)
: show a simple window message that tells the user why the script will be interrupted.

When the user clicks OK in this window, the script will have stopped running.

For details and examples, see @@Scripting 5.9. Quitting@
and @@Scripting 6.8. Messages to the user@.

################################################################################
"`exp`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp` (%`x`)
: exponentiate %x, i.e. compute %e^%`x`.

Related function
================
The inverse of #`exp` is @`ln`.

################################################################################
"`exp#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp#` (%`v#`)
: exponentiate (@`exp`) each element of the vector %`v#`.

################################################################################
"`exp##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp##` (%`m##`)
: exponentiate (@`exp`) each cell of the matrix %`m##`.

################################################################################
"`extractLine$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractLine$` (%`string$`, %`part$`)
: return everything that comes after the first occurrence of %`part$` in %`string$`.

################################################################################
"`extractNumber`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractNumber` (%`string$`, %`part$`)
: return the first number after the first occurrence of %`part$% in %`string$`.

################################################################################
"`extractWord$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractWord$` (%`string$`, %`part$`)
: return the first “word” after the first occurrence of %`part$% in %`string$`.

################################################################################
"`fileNames$#`"
© Paul Boersma 2023,2025

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`fileNames$#` (%`folderNameOrPattern$`)
: return the names (not the whole paths) of the files in a folder
	or that match a file-name pattern with up to two asterisks.

Behaviour
=========
The resulting string array will contain an alphabetical list of file names (by naïve Unicode-sorting),
without the preceding path through the folder structures. If there are no files that match the file path,
the string array will contain 0 strings.

Usage
=====
There are two ways to specify the file path.

One way is to specify a folder name only. On Unix, the file path could be
`/usr/people/miep/sounds` or `/usr/people/miep/sounds/`, for instance. On Windows,
`C:\Users\Miep\Sounds` or `C:\Users\Miep\Sounds\`.
On Macintosh, `/Users/miep/Sounds` or `/Users/miep/Sounds/`. Any of these produce
a list of all the files in the specified folder.

The other way is to specify a wildcard (a single asterisk) for the file names.
To get a list of all the files whose names start with “`hal`” and end in “`.wav`”,
use `/usr/people/miep/sounds/hal*.wav`, `C:\Users\Miep\Sounds\hal*.wav`,
or `/Users/miep/Sounds/hal*.wav`.

You can even use %two wildcards: #`fileNames$#` (“/usr/people/miep/sounds/*al*.wav”)
gives you a list of all files whose names contain “al” and end in “wav`.

Usage
=====
In a script, you can use this command to cycle through the files in a folder.
For instance, to read in all the sound files in a specified folder,
you could use the following script:
{;
	folder$ = “/usr/people/miep/sounds”
	list$# = \#`{fileNames$#}: folder$ + “/*.wav”
	for ifile to \`{size} (list$#)
		\@{Read from file:} folder$ + “/” + list$# [ifile]
	endfor
}
If the script has been saved to a script file, you can use file paths that are relative to the folder
where you saved the script. Thus, with
{;
	list$# = \#`{fileNames$#}: “*.wav”
}
you get a list of all the `.wav` files that are in the same folder as the script that contains this line.
And to get a list of all the `.wav` files in the folder `Sounds` that resides in the same folder as your script,
you can do
{;
	list$# = \#`{fileNames$#}: “Sounds/*.wav”
}
As is usual in Praat scripting, the forward slash (“/”) in this example can be used on all platforms, including Windows.
This makes your script portable across platforms.

Note that the above functionality can also be written four lines shorter, using built-in functions:
{;
	folder$ = “/usr/people/miep/sounds”
	list$# = \#`{fileNames$#}: folder$ + “/*.wav”
	for ifile to \`{size} (list$#)
		\@{Read from file:} folder$ + “/” + list$# [ifile]
	endfor
}

See also
========
- @`folderNames$#` for finding %folder names
- @`fileNames_caseInsensitive$#` for %%case-insensitive% matching

################################################################################
"`fileNames_caseInsensitive$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`fileNames_caseInsensitive$#` (%`folderNameOrPattern$`)
: return the names (not the whole paths) of the files in a folder
	or that match a file-name pattern with up to two asterisks.

See also
========
- @`folderNames_caseInsensitive$#` for finding %folder names
- @`fileNames$#` for %%case-sensitive% matching

################################################################################
"`fileReadable`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`fileReadable` (%`filePath$`)
: return 1 if the file exists and can be read, or 0 otherwise
  (e.g. if %`filePath$` does not exist or refers to a folder).

################################################################################
"`fisherP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@: the cumulative Fisher’s F-distribution.

Syntax and semantics
====================
#`fisherP` (%`f`, %`df1`, %`df2`)
: compute the area under Fisher’s F-distribution \-m\oo to %f,
for %`df1` numerator degrees of freedom and %`df2` denominator degrees of freedom.

################################################################################
"`fisherQ`"
© David Weenink 2000, Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`fisherQ` (%`f`, %`df1`, %`df2`)
: compute the area under Fisher’s F-distribution from %f to +\oo,
for %`df1` numerator degrees of freedom and %`df2` denominator degrees of freedom.

Related functions
=================
(1) #`fisherQ` is the inverse of @`invFisherQ`, i.e.
~	fisherQ (invFisherQ (%q, %df_1, %df_2), %df_1, %df_2) = %q

, and
~	invFisherQ (fisherQ (%f, %df_1, %df_2), %df_1, %df_2) = %f

(2) #`fisherQ` is the complement of @`fisherP`, i.e.
~	fisherQ (%f, %df1, %df2) = 1 \-m fisherP (%f, %df1, %df2)

, and
~	fisherP (%f, %df1, %df2) = 1 \-m fisherQ (%f, %df1, %df2)

################################################################################
"`fixed$`"
© Paul Boersma 2023,2025

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`fixed$` (%`number`, %`precision`)
: format a number as a string, with %`precision` digits after the decimal point.

#`fixed$` (%`vector#`, %`precision`)
: format a vector as a string, with all elements expressed in %`precision` digits after the decimal point.

#`fixed$` (%`matrix##`, %`precision`)
: format a matrix as a string, with all cells expressed in %`precision` digits after the decimal point.

Examples
========

{
	assert fixed$ (pi, 3) = "3.142"
	assert fixed$ (-1, 3) = "-1.000"
	assert fixed$ (1e6, 3) = "1000000.000"
	assert fixed$ (0.01, 3) = "0.010"
	assert fixed$ (0.001, 3) = "0.001"
	assert fixed$ (0.0001, 3) = "0.0001"   ; never less than 1 significant digit!
	
	assert fixed$ ({ pi, -1, 1e6, 0.0001 }, 3) =
	... "3.142" + newline$ + "-1.000" + newline$ +
	... "1000000.000" + newline$ + "0.0001" + newline$
}

################################################################################
"`floor`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`floor` (%`x`)
: round %`x` down to the nearest integer number.

################################################################################
"`floor#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`floor#` (%`v#`)
: round down (@`floor`) each element of the vector %`v#`.

################################################################################
"`floor##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`floor##` (%`m##`)
: round down (@`floor`) each cell of the matrix %`m##`.

################################################################################
"`folderExists`"
© Paul Boersma 2024

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`folderExists` (%`folderPath$`)
: return 1 if the folder exists, or 0 otherwise
  (e.g. if %`folderPath$` does not exist or refers to a file).

################################################################################
"`folderNames$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`folderNames$#` (%`folderNameOrPattern$`)
: return the names (not the whole paths) of the subfolders in a folder
	or that match a subfolder-name pattern with up to two asterisks.

See also
========
- @`fileNames$#` for finding %file names
- @`folderNames_caseInsensitive$#` for %%case-insensitive% matching

################################################################################
"`folderNames_caseInsensitive$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`folderNames_caseInsensitive$#` (%`folderNameOrPattern$`)
: return the names (not the whole paths) of the subfolders in a folder
	or that match a subfolder-name pattern with up to two asterisks.

See also
========
- @`fileNames_caseInsensitive$#` for finding %file names
- @`folderNames$#` for %%case-sensitive% matching

################################################################################
"`from_to#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`from_to#` (%`m`, %`n`)
: return the integers from %`m` through %`n`.

################################################################################
"`from_to_by#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`from_to_by#` (%`m`, %`n`, %`step`)
: return the numbers from %`m` through %`n`, in steps of %`step`.

################################################################################
"`from_to_count#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`from_to_count#` (%`start`, %`end`, %`n`)
: return %`n` equally-spaced numbers
starting with %`start` and ending with %`end`.

################################################################################
"`gaussP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@: the cumulative normal (Gauss) distribution.

Syntax and semantics
====================
#``gaussP` (%`z`)
: compute the area under the normal distribution
(with mean 0 and standard deviation 1) from \-m\oo to %`z`.

################################################################################
"`gaussQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`gaussQ` (%`z`)
: compute the area under the normal distribution
(with mean 0 and standard deviation 1) from %z to +\oo.

Related functions
=================
(1) #`gaussQ` is the inverse of @`invGaussQ`, i.e.
~	gaussQ (invGaussQ (%q)=) = %q

, and
~	invGaussQ (gaussQ (%z)) = %z

(2) #`gaussQ` is the complement of @`gaussP`, i.e.
~	gaussQ (%z) = 1 \-m gaussP (%z)

, and
~	gaussP (%z) = 1 \-m gaussQ (%z)

################################################################################
"`goto`"
© Paul Boersma 2023

A keyword that can be used in @Scripting, for control flow.

Syntax and semantics
====================
#`goto` %`LABEL`
: jump from here to the line that contains `label \%{LABEL}`,
where %`LABEL` can be any sequence of letters (usually upper case), underscores and digits.

See also @`label`.

################################################################################
"`hertzToBark`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToBark` (%`x`)
: convert from an acoustic frequency %x in Hz to an auditory Bark-rate
(auditory spectral frequency; place on basilar membrane).

Definition
==========
~	#`hertzToBark` (%x) = 7 ln (%x/650 + \Vr(1 + (%x/650)^2))

################################################################################
"`hertzToErb`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToErb` (%`x`)
: convert from an acoustic frequency %`x` in Hz to an auditory ERB-rate.

Definition
==========
~	#`hertzToErb` (%x) = 11.17 ln ((%x + 312) / (%x + 14680)) + 43

################################################################################
"`hertzToMel`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToMel` (%`x`)
: convert from an acoustic frequency %`x` in Hz to an auditory pitch in mel.

Definition
==========
~	#`hertzToMel` (%x) 550 ln (1 + %x / 550).

################################################################################
"`hertzToSemitones`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToSemitones` (%`x`)
: convert from an acoustic frequency %x in Hz
to a logarithmic musical scale relative to 100 Hz.

Definition
==========
~	#`hertzToSemitones` (%x) = 12 ln (%x / 100) / ln 2

################################################################################
"`imax`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`imax` (%`x`, `...`)
: determine the location of the maximum of a sequence of numbers.

Tests
=====
{
	assert imax (7.2, -5, 3) = 1
}

################################################################################
"`imin`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`imin` (%`x`, `...`)
: determine the location of the minimum of a sequence of numbers.

Tests
=====
{
	assert imin (7.2, -5, 3) = 2
}

################################################################################
"`index`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`index` (%`string$`, %`part$`)
: return the first location of the substring %`part$` inside %`string$`, or 0 if not found.

#`index` (%`strings$#`, %`s$`)
: look up whether and where %`s$` first occurs in %`strings$#`.

See also
========
- @`rindex` for the %last occurrence
- @`index_caseInsensitive` for %%case-insensitive% matching
- @`index_regex` for %%regular-expression% matching

################################################################################
"`index_caseInsensitive`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`index_caseInsensitive` (%`string$`, %`part$`)
: return the first location of the substring %`part$` inside %`string$`, or 0 if not found.

#`index_caseInsensitive` (%`strings$#`, %`s$`)
: look up whether and where %`s$` first occurs in %`strings$#`.

Case insensitivity
==================
For determining where the substring is located,
it doesn’t matter whether %`string$` and/or %`part$` are in lower case or upper case.
Here is a comparison with @`index`:
{
	assert \`{index} ("hello", "L") = 0
	assert \#{index_caseInsensitive} ("hello", "L") = 3
	assert \`{index} ("heLlo", "L") = 3
	assert \#{index_caseInsensitive} ("heLlo", "L") = 3
	assert \`{index} ("helLo", "L") = 4
	assert \#{index_caseInsensitive} ("helLo", "L") = 3
	assert \`{index} ("heLLo", "L") = 3
	assert \#{index_caseInsensitive} ("heLLo", "L") = 3
}
See also
========
- @`rindex_caseInsensitive` for the %last occurrence
- @`index` for %%case-sensitive% matching

################################################################################
"`index_regex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`index_regex` (%`string$`, %`regex$`)
: determine whether and where %`string$` first matches %`regex$`.
  See @@Regular expressions@.

################################################################################
"`inner`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`inner` (%`a#`, %`b#`)
: compute the inner product of the vectors %`a#` and %`b#`.

################################################################################
"`invBinomialP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invBinomialP` (%`p`, %`k`, %`n`)
: return the inverse cumulative binomial distribution,
i.e. the value %prob for which @`binomialP` (%`prob`, %`k`, %`n`) = %`p`.

################################################################################
"`invBinomialQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invBinomialQ` (%`q`, %`k`, %`n`)
: return the inverse cumulative binomial distribution,
i.e. the value %prob for which @`binomialQ` (%`prob`, %`k`, %`n`) = %`q`.

################################################################################
"`invChiSquareQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invChiSquareQ` (%`q`, %`df`)
: return the inverse %\ci^2 (chi-square) distribution,
i.e. the value %chisq for which @`chiSquareQ` (%`chisq`, %`df`) = %`q`.

################################################################################
"`invFisherQ`"
© David Weenink 2000, Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invFisherQ` (%`q`, %`df1`, %`df2`)
: return the inverse cumulative Fisher F-distribution,
i.e. the value %f for which @`fisherQ` (%`f`, %`df1`, %`df2`) = %`q`.

################################################################################
"`invGaussQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invGaussQ` (%`q`)
: return the inverse cumulative normal (Gauss) distribution,
i.e. the value %z for which @`gaussQ` (%`z`) = %`q`.

################################################################################
"`invSigmoid`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invSigmoid` (%`x`)
: compute the inverse @`sigmoid`.

Definition
==========
~	#`invSigmoid` (%x) = ln (%x / (1 − %x)).

Domain and range
================
#`invSigmoid` (%x) is defined for 0 < %`x` < 1.
The possible results are all real numbers.

################################################################################
"`invSigmoid#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invSigmoid#` (%`v#`)
: compute the inverse sigmoid (@`invSigmoid`) of each element of the vector %`v#`.

################################################################################
"`invSigmoid##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invSigmoid##` (%`m##`)
: compute the inverse sigmoid (@`invSigmoid`) of each cell of the matrix %`m##`.

################################################################################
"`invStudentQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`invStudentQ` (%`q`, %`df`)
: return the inverse cumulative Student %t distribution,
i.e. the value %t for which @`studentQ` (%`t`, %`df`) = %`q`.

################################################################################
"`label`"
© Paul Boersma 2023

A keyword that can be used in @Scripting, for control flow.

Syntax and semantics
====================
#`label` %`LABEL`
: proceed from here, perhaps after jumping from a line that contains `goto \%{LABEL}`,
where %`LABEL` can be any sequence of letters (usually upper case), underscores and digits.

See also @`goto`.

################################################################################
"`left$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`left$` (%`string$`, %`n`)
: return the %`n` first characters in %`string$`.

################################################################################
"`length`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`length` (%`string$`)
: return the number of Unicode characters in %`string$`.

################################################################################
"`ln`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ln` (%`x`)
: return the natural logarithm of %`x`, i.e. the logarithm with base %e.

################################################################################
"`ln#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ln#` (%`v#`)
: compute the natural logarithm (@`ln`) of each element of the vector %`v#`.

################################################################################
"`ln##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`ln##` (%`m##`)
: compute the natural logarithm (@`ln`) of each cell of the matrix %`m##`.

################################################################################
"`lnGamma`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`lnGamma` (%`x`)
: return the natural logarithm of the \Ga function of %`x`.

################################################################################
"`log2`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log2` (%`x`)
: return the base-2 logarithm of %`x`.

################################################################################
"`log2#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log2#` (%`v#`)
: compute the base-2 logarithm (@`log2`) of each element of the vector %`v#`.

################################################################################
"`log2##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log2##` (%`m##`)
: compute the base-2 logarithm (@`log2`) of each cell of the matrix %`m##`.

################################################################################
"`log10`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log10` (%`x`)
: return the base-10 logarithm of %`x`.

################################################################################
"`log10#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log10#` (%`v#`)
: compute the base-10 logarithm (@`log10`) of each element of the vector %`v#`.

################################################################################
"`log10##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`log10##` (%`m##`)
: compute the base-10 logarithm (@`log10`) of each cell of the matrix %`m##`.

################################################################################
"`lowerCase$`"
© Paul Boersma 2025

A function that can be used in @@Formulas@. It turns all capitals
into lower-case characters.

Syntax and semantics
====================
#`lowerCase$` (%`string$`)
: give the lower-case version of %`string$`, whereby all upper-case letters
  have been replaced with lower-case letters.

Tests
=====
{
	\`{assert} \#{lowerCase$} ("HOwAREYou") = "howareyou"
	\`{assert} \#{lowerCase$} ("Maria Ressa") = "maria ressa"
	\`{assert} \#{lowerCase$} ("GSL") = "gsl"
	\`{assert} \#{lowerCase$} ("ÖAV") = "öav"
	\`{assert} \#{lowerCase$} ("TOUCHÉ") = "touché"
}

################################################################################
"`lowerCaseAppName$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The name of the present app,
in lower snake case, such as `praat` or `praat_for_hospitals`.

Syntax and semantics
====================
#`lowerCaseAppName$` ( )
: give the name of the present app, in lower snake case.

Examples
========
{
	writeInfoLine: \#{lowerCaseAppName$} ( )
}

################################################################################
"`max`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`max` (%`x`, `...`)
: determine the maximum of a list of numbers.

Tests
=====
{
	\`{assert} \#{max} (7.2, -5, 3) = 7.2
}

################################################################################
"`mean`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`mean` (%`v#`)
: compute the average of the elements of the vector `v#`.

################################################################################
"`melToHertz`"
© Paul Boersma 2023-06-30

A function that can be used in @Formulas.

Syntax and semantics
====================
#`melToHertz` (%`x`)
: convert from an auditory pitch %`x` in mel to an acoustic frequency in Hz.

Definition
==========
~	#`melToHertz` (%x) 550 (exp (%x / 550) - 1).

################################################################################
"`mid$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`mid$` (%`string$`, %`from`, %`n`)
: return the %`n` characters in %`string$` starting at position %`from`.

################################################################################
"`min`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`min` (%`x`, `...`)
: determine the minimum of a list of numbers.

Tests
=====
{
	assert min (7.2, -5, 3) = -5
}

################################################################################
"`minusObject`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`minusObject` (`...`)
: deselect the objects given by IDs and/or full names.

################################################################################
"`moveAndOrRenameFile`"
© Paul Boersma 2025

A function that can be used in @Scripting.

Syntax and semantics
====================
#`moveAndOrRenameFile` (%`fromFilePath$`, %`toFilePath$`)
: move and/or rename an existing file to a new location and/or name.

Example: rename
===============
{;
	\#{moveAndOrRenameFile} (“/Users/me/Desktop/hello.wav”,
	... “/Users/me/Desktop/goodbye.wav”)
}

Example: move
===============
{;
	\#{moveAndOrRenameFile} (“/Users/me/Desktop/hello.wav”,
	... “/Users/me/sound files/hello.wav”)
}

Example: move and rename
========================
{;
	\#{moveAndOrRenameFile} (“/Users/me/Desktop/hello.wav”,
	... “/Users/me/sound files/goodbye.wav”)
}

################################################################################
"`mul##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`mul##` (%`a##`, %`b##`)
: perform a matrix multiplication of the matrices %`a##` and %`b##`.

################################################################################
"`ncol`"
© Paul Boersma 2024

The number of columns of an object.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`nrow`"
© Paul Boersma 2024

The number of rows of an object.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`number`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`number` (%`a$`)
: interpret a string as a number.

{
	\`{assert} \#{number} ("0.0") = 0
	\`{assert} \#{number} ("0.00123") = 0.00123   ; decimal point
	\`{assert} \#{number} ("12e34") = 1.2e+35   ; exponent introduced by "e" or "E"
	\`{assert} \#{number} ("junk") = undefined   ; i.e. not zero as in C++ !
	\`{assert} \#{number} ("  -0.0000000123junk") = -1.23e-08   ; maximize digits, then ignore
	\`{assert} \#{number} ("1.0e+309") = undefined
	\`{assert} \#{number} ("1.0e-309") <> 0   ; denormalized
	\`{assert} \#{number} ("1.0e-319") <> 0   ; denormalized
	\`{assert} \#{number} ("1.0e-329") = 0   ; underflow
	\`{assert} \#{number} ("32278") = 32278
	\`{assert} \#{number} ("-32278") = -32278
	\`{assert} \#{number} ("32278.64785") = 32278.64785
	\`{assert} \#{number} ("-32278.64785") = -32278.64785
	\`{assert} \#{number} ("32278.647e85") = 32278.647e85
	\`{assert} \#{number} ("-32278.647e85") = -32278.647e85
	\`{assert} \#{number} ("32278.647e-85") = 32278.647e-85
	\`{assert} \#{number} ("-32278.647e-85") = -32278.647e-85
	\`{assert} \#{number} ("32278.647e-305") = 32278.647e-305
	\`{assert} \#{number} ("-32278.647e-305") = -32278.647e-305
	\`{assert} \#{number} ("32278.647e-315") = 32278.647e-315
	\`{assert} \#{number} ("-32278.647e-315") = -32278.647e-315
	\`{assert} \#{number} ("32278.647e-325") = 32278.647e-325
	\`{assert} \#{number} ("-32278.647e-325") = -32278.647e-325
	\`{assert} \#{number} ("32278.647e305") = 32278.647e305
	\`{assert} \#{number} ("-32278.647e305") = -32278.647e305
	\`{assert} \#{number} ("32278.647e315") = 32278.647e315
	\`{assert} \#{number} ("-32278.647e315") = -32278.647e315
	\`{assert} \#{number} ("32278.647e325") = 32278.647e325
	\`{assert} \#{number} ("-32278.647e325") = -32278.647e325
}
Hexadecimal:
{
	\`{assert} \#{number} ("0x32278") = 0x32278
	\`{assert} \#{number} ("0x123") = 291
	\`{assert} \#{number} ("0x000001ABCDEFGH") = 0x00001abcdef   ; maximize digits, then ignore
	\`{assert} \#{number} ("0x123.456") = 291.27099609375
	\`{assert} \#{number} ("0x123.abc") = 291.6708984375   ; hexadecimal point
	\`{assert} \#{number} ("0x123.abcp-1") = 145.83544921875   ; exponent introduced by "p" or "P"
	\`{assert} \#{number} ("0x123.abcP-2") = 72.917724609375
	\`{assert} \#{number} ("0x123.abcp-12") = 0.07120871543884277   ; the exponent is decimal !
	\`{assert} \#{number} ("0x123.abcp-1A2") = 145.83544921875   ; the exponent is decimal !
	\`{assert} \#{number} ("-0x1afp-2") = -107.75   ; from `cppreference.com`
}

################################################################################
"`number#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`number#` (%`a$#`)
: interpret strings as numbers.

################################################################################
"`numberOfColumns`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`numberOfColumns` (%`m##`)
: return the number of columns of the matrix %`m##`.

################################################################################
"`numberOfRows`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`numberOfRows` (%`m##`)
: return the number of rows of the matrix %`m##`.

################################################################################
"`nx`"
© Paul Boersma 2024

The number of cells horizontally (i.e. the number of columns) of an object.
This is the same as @`ncol`.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`ny`"
© Paul Boersma 2024

The number of cells vertically (i.e. the number of rows) of an object.
This is the same as @`nrow`.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`outer##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`outer##` (%`a#`, %`b#`)
: compute the outer product of the vectors %`a#` and %`b#`.

Definition
==========
~	%result__%i%j_ = %a_%i %b_%j

################################################################################
"`padLeft$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`padLeft$` (%`string$`, %`minimumNewWidth` [, %`pad$`])
: add characters (selected from %`pad$`) to the left of %`string$` until the result is at least as long as %`minimumNewWidth`:
{
	\`{assert} \#{padLeft$} ("hello", 12, ".") = ".......hello"
	\`{assert} \#{padLeft$} ("hello", 5, ".") = "hello"
}
If %`pad$` is not given, the padding will be done with spaces:
{
	\`{assert} \#{padLeft$} ("hello", 12) = "       hello"
}
No truncation takes place, so the resulting string may be longer than %`minimumNewWidth`:
{
	\`{assert} \#{padLeft$} ("hello", 3) = "hello"
	\`{assert} \#{padLeft$} ("hello", -100) = "hello"
}
If %`pad$` contains more than one character, the left edge of %`pad$` is left-aligned to the resulting string,
so that the result will look good if padding is used for visualizing columns:
{
	\`{assert} \#{padLeft$} ("hello", 12, ".-;)")   = ".-;).-;hello"
	\`{assert} \#{padLeft$} ("goodbye", 12, ".-;)") = ".-;).goodbye"
}
There will be an error message if padding is required but no characters are to be found in %`pad$`:
{
	\`{asserterror} Empty pad string.
	a$ = \#{padLeft$} ("hello", 12, "")
}

################################################################################
"`padOrTruncateLeft$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`padOrTruncateLeft$` (%`string$`, %`newWidth` [, %`pad$`])
: add characters (selected from %`pad$`) to the left of %`string$`, or remove characters from the left of %`string$`,
  until the result is precisely %`newWidth` characters long:
{
	\`{assert} \#{padOrTruncateLeft$} ("hello", 12, ".") = ".......hello"
	\`{assert} \#{padOrTruncateLeft$} ("hello", 3, ".") = "llo"
	\`{assert} \#{padOrTruncateLeft$} ("hello", 0) = ""
}
If %`pad$` is not given, the padding will be done with spaces:
{
	\`{assert} \#{padOrTruncateLeft$} ("hello", 12) = "       hello"
	\`{assert} \#{padOrTruncateLeft$} ("hello", 3) = "llo"
}
If characters have to be added, and %`pad$` contains more than one character,
the left edge of %`pad$` is left-aligned to the resulting string,
so that the result will look good if padding is used for visualizing columns:
{
	\`{assert} \#{padOrTruncateLeft$} ("hello", 12, ".-;)")   = ".-;).-;hello"
	\`{assert} \#{padOrTruncateLeft$} ("goodbye", 12, ".-;)") = ".-;).goodbye"
}
Note that %`newWidth` cannot be negative:
{
	\`{asserterror} Can never truncate a string down to -3 characters.
	a$ = \#{padOrTruncateLeft$} ("hello", -3)
}
There will be an error message if padding is required but no characters are to be found in %`pad$`:
{
	\`{asserterror} Empty pad string.
	a$ = \#{padOrTruncateLeft$} ("hello", 12, "")
}

################################################################################
"`padOrTruncateRight$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`padOrTruncateRight$` (%`string$`, %`newWidth` [, %`pad$`])
: add characters (selected from %`pad$`) to the right of %`string$`, or remove characters from the right of %`string$`,
  until the result is precisely %`newWidth` characters long:
{
	\`{assert} \#{padOrTruncateRight$} ("hello", 12, ".") = "hello......."
	\`{assert} \#{padOrTruncateRight$} ("hello", 3, ".") = "hel"
	\`{assert} \#{padOrTruncateRight$} ("hello", 0) = ""
}
If %`pad$` is not given, the padding will be done with spaces:
{
	\`{assert} \#{padOrTruncateRight$} ("hello", 12) = "hello       "
	\`{assert} \#{padOrTruncateRight$} ("hello", 3) = "hel"
}
If characters have to be added, and %`pad$` contains more than one character,
the right edge of %`pad$` is right-aligned to the resulting string,
so that the result will look good if padding is used for visualizing columns:
{
	\`{assert} \#{padOrTruncateRight$} ("hello", 12, ".-;)")   = "hello-;).-;)"
	\`{assert} \#{padOrTruncateRight$} ("goodbye", 12, ".-;)") = "goodbye).-;)"
}
Note that %`newWidth` cannot be negative:
{
	\`{asserterror} Can never truncate a string down to -3 characters.
	a$ = \#{padOrTruncateRight$} ("hello", -3)
}
There will be an error message if padding is required but no characters are to be found in %`pad$`:
{
	\`{asserterror} Empty pad string.
	a$ = \#{padOrTruncateRight$} ("hello", 12, "")
}

################################################################################
"`padRight$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`padRight$` (%`string$`, %`minimumNewWidth` [, %`pad$`])
: add characters (selected from %`pad$`) to the right of %`string$` until the result is at least as long as %`minimumNewWidth`:
{
	\`{assert} \#{padRight$} ("hello", 12, ".") = "hello......."
	\`{assert} \#{padRight$} ("hello", 5, ".") = "hello"
}
If %`pad$` is not given, the padding will be done with spaces:
{
	\`{assert} \#{padRight$} ("hello", 12) = "hello       "
}
No truncation takes place, so the resulting string may be longer than %`minimumNewWidth`:
{
	\`{assert} \#{padRight$} ("hello", 3) = "hello"
	\`{assert} \#{padRight$} ("hello", -100) = "hello"
}
If %`pad$` contains more than one character, the right edge of %`pad$` is right-aligned to the resulting string,
so that the result will look good if padding is used for visualizing columns:
{
	\`{assert} \#{padRight$} ("hello", 12, ".-;)")   = "hello-;).-;)"
	\`{assert} \#{padRight$} ("goodbye", 12, ".-;)") = "goodbye).-;)"
}
There will be an error message if padding is required but no characters are to be found in %`pad$`:
{
	\`{asserterror} Empty pad string.
	a$ = \#{padRight$} ("hello", 12, "")
}

################################################################################
"`part#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`part#` (%`vector#`, %`from`, %`to`)
: compute a subsequence of elements of a vector.

Examples
========
{
	\`{assert} part# ({ 7, 4, 99, 103, 1 }, 3, 4) = { 99, 103 }
}

Examples of error messages
==========================
Checks on the number of arguments (always has to be 3):
{
	asserterror The function “part#” requires exactly three arguments
	... (namely a vector, a starting index, and an end index),
	... not the 0 given.
	a# = part# ()

	asserterror The function “part#” requires exactly three arguments
	... (namely a vector, a starting index, and an end index),
	... not the 5 given.
	a# = part# (7, 8, 9, "hello", "world")
}
Checks on the types of the arguments (always has to be vector, number, number):
{
	asserterror The first argument of the function “part#” should be
	... a numeric vector, not a number.
	a# = part# (2, 4, { 5, 6, 7, 9, 8 })

	asserterror The first argument of the function “part#” should be
	... a numeric vector, not a string.
	a# = part# ("hello", 4, { 5, 6, 7, 9, 8 })

	asserterror The second argument of the function “part#” should be
	... a number (the starting index), not a string.
	a# = part# ({ 5, 6, 7, 9, 8 }, "hello", 4)

	asserterror The second argument of the function “part#” should be
	... a number (the starting index), not a numeric vector.
	a# = part# ({ 5, 6, 7, 9, 8 }, { 0 }, 4)

	asserterror The third argument of the function “part#” should be
	... a number (the end index), not a string.
	a# = part# ({ 5, 6, 7, 9, 8 }, 4, "hello")

	asserterror The third argument of the function “part#” should be
	... a number (the end index), not a numeric vector.
	a# = part# ({ 5, 6, 7, 9, 8 }, 4, { 0 })
}
Finally the checks on the preconditions of the arguments:
both element numbers should be within bounds:
{
	asserterror The second argument of the function “part#” (the starting index)
	... should (after rounding) be a positive whole number, not -3.
	a# = part# ({ 5, 6, 7, 9, 8 }, -2.98, 0)

	asserterror The second argument of the function “part#” (the starting index)
	... should (after rounding) be a positive whole number, not 0.
	a# = part# ({ 5, 6, 7, 9, 8 }, 0, 0)

	asserterror The second argument of the function “part#” (the starting index)
	... should (after rounding) be at most the number of elements (5), not 99.
	a# = part# ({ 10, 6, 7, 9, 8 }, 99, 0)

	asserterror The second argument of the function “part#” (the starting index)
	... should (after rounding) be at most the number of elements (5), not 6.
	a# = part# ({ 10, 6, 7, 9, 8 }, 5.5, 0)

	asserterror The third argument of the function “part#” (the end index)
	... should (after rounding) be a positive whole number, not -3.
	a# = part# ({ 10, 6, 7, 9, 8 }, 3, -2.98)

	asserterror The third argument of the function “part#” (the end index)
	... should (after rounding) be a positive whole number, not 0.
	a# = part# ({ 10, 6, 7, 9, 8 }, 3, 0)

	asserterror The third argument of the function “part#” (the end index)
	... should (after rounding) be at most the number of elements (5), not 99.
	a# = part# ({ 10, 6, 7, 9, 8 }, 3, 99)

	asserterror The third argument of the function “part#” (the end index)
	... should (after rounding) be at most the number of elements (5), not 6.
	a# = part# ({ 10, 6, 7, 9, 8 }, 3, 5.5)
}

################################################################################
"`part##`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`part##` (%`matrix#`, %`fromRow`, %`toRow`, %`fromColumn`, %`toColumn`)
: compute a subwindow of cells of a matrix.

Examples
========
{
	\`{assert} part## ({{ 7, 4, 9, 3 }, { 1, 4, 3, 22 }, { 0, 8, 6, 5 }},
	... 2, 3, 1, 3) = {{ 1, 4, 3 }, { 0, 8, 6 }}
}

Examples of error messages
==========================
Checks on the number of arguments (always has to be 5):
{
	asserterror The function “part##” requires exactly five arguments
	... (namely a matrix, a starting row, an end row, a starting column, and an end column),
	... not the 0 given.
	a## = part## ()

	asserterror The function “part##” requires exactly five arguments
	... (namely a matrix, a starting row, an end row, a starting column, and an end column),
	... not the 7 given.
	a## = part## (7, 8, 9, 10, 11, "hello", "world")
}
Checks on the types of the arguments (always has to be matrix, number, number, number, number):
{
	asserterror The first argument of the function “part##” should be
	... a numeric matrix, not a number.
	a## = part## (2, 4, { 5, 6, 7, 9, 8 }, 66, 99)

	asserterror The first argument of the function “part##” should be
	... a numeric matrix, not a string.
	a## = part## ("hello", 4, { 5, 6, 7, 9, 8 }, 66, 99)

	asserterror The first argument of the function “part##” should be
	... a numeric matrix, not a numeric vector.
	a## = part## ({ 5, 6, 7, 9, 8 }, 4, { 5, 6, 7, 9, 8 }, 66, 99)

	m## = {{ 7, 4, 9, 3 }, { 1, 4, 3, 22 }, { 0, 8, 6, 5 }}   ; three rows, four columns

	asserterror The second argument of the function “part##” should be
	... a number (the starting row), not a string.
	a## = part## (m##, "hello", 4, 66, 99)

	asserterror The second argument of the function “part##” should be
	... a number (the starting row), not a numeric vector.
	a## = part## (m##, { 0 }, 4, 66, 99)

	asserterror The third argument of the function “part##” should be
	... a number (the end row), not a string.
	a## = part## (m##, 4, "hello", 66, 99)

	asserterror The third argument of the function “part##” should be
	... a number (the end row), not a numeric vector.
	a## = part## (m##, 4, { 0 }, 66, 99)

	asserterror The fourth argument of the function “part##” should be
	... a number (the starting column), not a string.
	a## = part## (m##, 66, 99, "hello", 4)

	asserterror The fourth argument of the function “part##” should be
	... a number (the starting column), not a numeric vector.
	a## = part## (m##, 66, 99, { 0 }, 4)

	asserterror The fifth argument of the function “part##” should be
	... a number (the end column), not a string.
	a## = part## (m##, 66, 99, 4, "hello")

	asserterror The fifth argument of the function “part##” should be
	... a number (the end column), not a numeric vector.
	a## = part## (m##, 66, 99, 4, { 0 })
}
Finally the checks on the preconditions of the arguments:
both row numbers and both column numbers should be within bounds:
{
	asserterror The second argument of the function “part##” (the starting row)
	... should (after rounding) be a positive whole number, not -3.
	a## = part## (m##, -2.98, 0, 66, 99)

	asserterror The second argument of the function “part##” (the starting row)
	... should (after rounding) be a positive whole number, not 0.
	a## = part## (m##, 0, 0, 66, 99)

	asserterror The second argument of the function “part##” (the starting row)
	... should (after rounding) be at most the number of rows (3), not 99.
	a## = part## (m##, 99, 0, 66, 99)

	asserterror The second argument of the function “part##” (the starting row)
	... should (after rounding) be at most the number of rows (3), not 4.
	a## = part## (m##, 3.5, 0, 66, 99)

	asserterror The third argument of the function “part##” (the end row)
	... should (after rounding) be a positive whole number, not -3.
	a## = part## (m##, 2, -2.98, 66, 99)

	asserterror The third argument of the function “part##” (the end row)
	... should (after rounding) be a positive whole number, not 0.
	a## = part## (m##, 2, 0, 66, 99)

	asserterror The third argument of the function “part##” (the end row)
	... should (after rounding) be at most the number of rows (3), not 99.
	a## = part## (m##, 2, 99, 66, 99)

	asserterror The third argument of the function “part##” (the end row)
	... should (after rounding) be at most the number of rows (3), not 4.
	a## = part## (m##, 2, 3.5, 66, 99)

	asserterror The fourth argument of the function “part##” (the starting column)
	... should (after rounding) be a positive whole number, not -3.
	a## = part## (m##, 2, 3, -2.98, 0)

	asserterror The fourth argument of the function “part##” (the starting column)
	... should (after rounding) be a positive whole number, not 0.
	a## = part## (m##, 2, 3, 0, 0)

	asserterror The fourth argument of the function “part##” (the starting column)
	... should (after rounding) be at most the number of columns (4), not 99.
	a## = part## (m##, 2, 3, 99, 0)

	asserterror The fourth argument of the function “part##” (the starting column)
	... should (after rounding) be at most the number of columns (4), not 5.
	a## = part## (m##, 2, 3, 4.5, 0)

	asserterror The fifth argument of the function “part##” (the end column)
	... should (after rounding) be a positive whole number, not -3.
	a## = part## (m##, 2, 3, 1, -2.98)

	asserterror The fifth argument of the function “part##” (the end column)
	... should (after rounding) be a positive whole number, not 0.
	a## = part## (m##, 2, 3, 1, 0)

	asserterror The fifth argument of the function “part##” (the end column)
	... should (after rounding) be at most the number of columns (4), not 99.
	a## = part## (m##, 2, 3, 1, 99)

	asserterror The fifth argument of the function “part##” (the end column)
	... should (after rounding) be at most the number of columns (4), not 5.
	a## = part## (m##, 2, 3, 1, 4.5)
}

################################################################################
"`pauseScript`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`pauseScript` (`...`)
: show a message in a simple @@pause window@.

For details and examples, see @@Scripting 6.6. Controlling the user@.

################################################################################
"`percent$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`percent$` (%`number`, %`precision`)
: format a number as a string,
with a trailing percent sign and %`precision` digits after the decimal point.

################################################################################
"`phonToDifferenceLimens`"
© Paul Boersma 2002, 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`phonToDifferenceLimens` (%`x`)
: convert a sensation level %`x` in phon units into an intensity difference limen level.

Definition
==========
~	`phonToDifferenceLimens` (%phon) = 30 · ((61/60)^^ %phon^ − 1)

Related function
================
#`phonToDifferenceLimens` is the inverse of @`differenceLimensToPhon`.

Derivation
==========

In first approximation, humans can detect an intensity difference of 1 phon, i.e.
if two sounds that differ only in intensity are played a short time after each other,
people can generally detect their intensity difference if it is greater than 1 phon.

But the sensitivity is somewhat better for louder sounds.
According to @@Jesteadt, Wier & Green (1977)@, the relative difference limen
of intensity is given by
~	DLI = \De%I / %I = 0.463 · (%I / %I_0)^^−0.072^

In this formula, %I is the intensity of the sound in Watt/m^2, %I_0 is the intensity of
the auditory threshold (i.e. 10^^−12^ Watt/m^2 at 1000 Hz),
and \De%I is the just noticeable difference.

@@Boersma (1998)|Boersma (1998: 109)@ calculates a difference-limen scale from this.
Given an intensity %I, the number of difference limens above threshold is
~	\in__%%I%0_^%I %dx \De%I(%x)
			= (1 / 0.463) \in__%%I%0_^%I %dx %I_0^^−0.072^ %x^^0.072−1^
~	= (1 / (0.463 · 0.072)) ((%I / %I_0)^^0.072^ − 1)

The sensation level in phon is defined as
~	SL = 10 log__10_ (%I / %I_0)

so that the number of difference limens above threshold is
~	(1 / (0.463 · 0.072)) (10^^(0.072 / 10) (10 log (%I / %I_0))^ − 1)
			= 30 · (1.0167^^SL^ − 1)

################################################################################
"`plusObject`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`plusObject` (`...`)
: select the objects given by IDs and/or full names,
without deselecting any objects that are currently selected.

################################################################################
"`procedure`"
© Paul Boersma 2023

A keyword that can be used in @Scripting.

Syntax and semantics
====================
#`procedure`: %`procedureName$`, `...`
: start the definition of the procedure %`procedureName$`, with optional parameters.

For an introduction, see @@Scripting 5.5. Procedures@.

################################################################################
"`randomBernoulli`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomBernoulli` (%`p`)
: generate a Bernoulli-distributed random deviate,
which is 1 with probability %`p`, and 0 with probability 1 \-m %`p`.

################################################################################
"`randomBernoulli#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomBernoulli#` (%`n`, %`p`)
: generate %`n` independent Bernoulli-distributed (@`randomBernoulli`) zeroes and ones.

#`randomBernoulli#` (%`model#`, %`p`)
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent Bernoulli-distributed zeroes and ones.
This is shorthand for doing
`
	\#{randomBernoulli#} (\`{size} (\%{model#}), \%{p})
`
################################################################################
"`randomBernoulli##`"
© Paul Boersma 2023,2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomBernoulli##` (%`nrow`, %`ncol`, %`p`)
: generate %`nrow` \xx %`ncol` independent Bernoulli-distributed (@`randomBernoulli`) zeroes and ones.

#`randomBernoulli##` (%`model##`, %`p`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent Bernoulli-distributed zeroes and ones.
This is shorthand for doing
`
	\#{randomBernoulli##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{p})
`
################################################################################
"`randomGamma`"
© David Weenink 2019, Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGamma` (%`shape`, %`rate`)
: generate a random number from a Gamma distribution
with shape parameter %`shape` and rate parameter %`rate`, which is defined as
the probability density function %f(%x; %`shape`, %`rate`) =
(1 / \Ga (%`shape`)) %`rate`^^%`shape`^ %x^^%`shape`−1^ %e^^−%`rate` %`x`^,
for %`x` > 0, %`shape` > 0 and %`rate` > 0,
according to the method by @@Marsaglia & Tsang (2000)@.

################################################################################
"`randomGamma#`"
© Paul Boersma 2023,2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGamma#` (%`n`, %`shape`, %`rate`)
: generate %`n` independent \Ga-distributed (@`randomGamma`) numbers.

#`randomGamma#` (%`model#`, %`shape`, %`rate`)
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent \Ga-distributed numbers.
This is shorthand for doing
{;
	\#{randomGamma#} (\`{size} (model#), \%{shape}, \%{rate})
}
################################################################################
"`randomGamma##`"
© Paul Boersma 2023,2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGamma##` (%`nrow`, %`ncol`, %`shape`, %`rate`)
: generate %`nrow` \xx %`ncol` independent \Ga-distributed (@`randomGamma`) numbers.

#`randomGamma##` (%`model##`, %`shape`, %`rate`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent \Ga-distributed numbers.
This is shorthand for doing
{;
	\#{randomGammma##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{shape}, \%{rate})
}
################################################################################
"`randomGauss`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGauss` (%`mu`, %`sigma`)
: generate a normally distributed random deviate,
with true population mean %`mu` and true population standard deviation %`sigma`.

################################################################################
"`randomGauss#`"
© Paul Boersma 2023,2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGauss#` (%`n`, %`mu`, %`sigma`)
: generate %`n` independent normally distributed (@`randomGauss`) numbers.

#`randomGauss#` (%`model#`, %`mu`, %`sigma`)
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent normally distributed numbers.
This is shorthand for doing
{;
	\#{randomGauss#} (\`{size} (\%{model#}), \%{mu}, \%{sigma})
}
################################################################################
"`randomGauss##`"
© Paul Boersma 2023,2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`randomGauss##` (%`nrow`, %`ncol`, %`mu`, %`sigma`)
: generate %`nrow` \xx %`ncol` independent normally distributed (@`randomGauss`) numbers.

#`randomGauss##` (%`model##`, %`mu`, %`sigma`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent normally distributed numbers.
This is shorthand for doing
{;
	\#{randomGauss##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{mu}, \%{sigma})
}
################################################################################
"`randomImax`"
© Paul Boersma 2026

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomImax` (%`distribution#`)
: sample from an array of relative probabilities.

Example
=======
The following draws the outcome 1 one half of the time, the outcome 2 one sixth of the time, and the outcome 3 one third of the time:
{
	for i to 20
		outcome = \#{randomImax} ({ 3.0, 1.0, 2.0 })
		\`{appendInfoLine}: outcome
	endfor
}
All elements of the vector have to be non-negative:
{
	\`{asserterror} Element 2 is less than zero; cannot interpret as a probability.
	a = \#{randomImax} ({ 3.0, -1.0, 2.0 })
}
Some elements of the vector may be zero. The following draws the outcome 1 60 percent of the time, and the outcome 3 40 percent of the time:
{
	for i to 20
		outcome = \#{randomImax} ({ 3.0, 0.0, 2.0 })
		\`{appendInfoLine}: outcome
	endfor
}
Not all elements of the vector can be zero:
{
	\`{asserterror} Cannot interpret a zero vector as probabilities.
	a = \#{randomImax} ({ 0.0, 0.0, 0.0 })
}

################################################################################
"`randomInteger`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomInteger` (%`min`, %`max`)
: generate a uniformly distributed integer random deviate,
with minimum %`min` and maximum %`max`.

################################################################################
"`randomInteger#`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomInteger#` (%`n`, %`min`, %`max`)
: generates %`n` independent uniformly distributed (@`randomInteger`) integer numbers.

#`randomInteger#` (%`model#`, %`min`, %`max`):
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent uniformly distributed integer numbers.
This is shorthand for doing
{;
	\#{randomInteger#} (\`{size} (\%{model#}), \%{min}, \%{max})
}
################################################################################
"`randomInteger##`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomInteger##` (%`nrow`, %`ncol`, %`min`, %`max`)
: generate %`nrow` \xx %`ncol` independent normally distributed (@`randomInteger`) numbers.

#`randomInteger##` (%`model##`, %`min`, %`max`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent uniformly distributed integer numbers.
This is shorthand for doing
{;
	\#{randomInteger##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{min}, \%{max})
}
################################################################################
"`randomPoisson`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomPoisson` (%`mean`)
: generate a Poisson-distributed random deviate, with true mean %`mean`.

################################################################################
"`randomPoisson#`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomPoisson#` (%`n`, %`mean`)
: generate %`n` independent Poisson-distributed (@`randomPoisson`) numbers.

#`randomPoisson#` (%`model#`, %`mean`)
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent Poisson-distributed numbers.
This is shorthand for doing
{;
	\#{randomPoisson#} (\`{size} (\%{model#}), \%{mean})
}
################################################################################
"`randomPoisson##`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomPoisson##` (%`nrow`, %`ncol`, %`mean`)
: generate %`nrow` \xx %`ncol` independent Poisson-distributed (@`randomPoisson`) numbers.

#`randomPoisson##` (%`model##`, %`mean`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent Poisson-distributed numbers.
This is shorthand for doing
{;
	\#{randomPoisson##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{mean})
}
################################################################################
"`randomUniform`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomUniform` (%`min`, %`max`)
: generate a uniformly distributed random deviate,
with true minimum %`min` (included) and true maximum %`max` (excluded).

################################################################################
"`randomUniform#`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomUniform#` (%`n`, %`min`, %`max`)
: generate %`n` independent uniformly distributed (@`randomUniform`) numbers.

#`randomUniform#` (%`model#`, %`min`, %`max`)
: create a vector with the same number of elements as %`model#`,
and fill all elements with independent uniformly distributed numbers.
This is shorthand for doing
{;
	\#{randomUniform#} (\`{size} (\%{model#}), \%{min}, \%{max})
}
################################################################################
"`randomUniform##`"
© Paul Boersma 2023,2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`randomUniform##` (%`nrow`, %`ncol`, %`min`, %`max`)
: generate %`nrow` \xx %`ncol` independent uniformly distributed (@`randomUniform`) numbers.

#`randomUniform##` (%`model##`, %`min`, %`max`)
: create a matrix with the same number of rows and columns as %`model##`,
and fill all cells with independent uniformly distributed numbers.
This is shorthand for doing
{;
	\#{randomUniform##} (\`{numberOfRows} (\%{model##}), \`{numberOfColumns} (\%{model##}), \%{min}, \%{max})
}
################################################################################
"`random_initializeSafelyAndUnpredictably`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`random_initializeSafelyAndUnpredictably` ( )
: undo the effects of @`random_initializeWithSeedUnsafelyButPredictably`.

################################################################################
"`random_initializeWithSeedUnsafelyButPredictably`"
© Paul Boersma 2023,2025

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`random_initializeWithSeedUnsafelyButPredictably` (%`seed`)
: produce (from now on) a reproducible sequence of random numbers.

The effects of this function can be undone by @`random_initializeSafelyAndUnpredictably`.

################################################################################
"`readFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`readFile` (%`filePath$`)
: read a number from a text file.

################################################################################
"`readFile$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`readFile$` (%`filePath$`)
: read a whole text file into a string.

################################################################################
"`readFile#`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`readFile#` (%`filePath$`)
: read the elements of a vector from a text file.

The elements can be separated by horizontal and/or vertical space.

################################################################################
"`readFile##`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`readFile##` (%`filePath$`)
: read the cells of a matrix from a text file.

Each line of the text file is read as a row of the matrix;
the elements of the row are separated by horizontal space (i.e. spaces and/or tabs).

The matrix obtains as many rows as there are lines in the text file.

Precondition
============
Each line of the text file should contain the same number of elements.

################################################################################
"`readLinesFromFile$#`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`readLinesFromFile$#` (%`filePath$`)
: read all lines from a text file.

################################################################################
"`rectify`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`rectify` (%`x`)
: return zero if %`x` is negative, and %`x` if %`x` is positive.

################################################################################
"`rectify#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`rectify#` (%`v#`)
: rectifies (@`rectify`) each element of the vector %`v#`.

################################################################################
"`rectify##`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`rectify##` (%`m##`)
: rectifies (@`rectify`) each cell of the matrix %`m##`.

################################################################################
"`removeObject`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`removeObject` (`...`)
: remove the objects given by IDs and/or full names,
without (further) changing the selection.

################################################################################
"`repeat#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`repeat#` (%`v#`, %`n`)
: repeats the whole sequence of elements of %`v#` %`n` times.

################################################################################
"`replace$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`replace$` (%`a$`, %`b$`, %`c$`, %`n`)
: within %`a$`, replace the first %`n` occurrences of %`b$`
with %`c$`, or all if %`n` = 0.

################################################################################
"`replace_regex$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`replace_regex$` (%`a$`, %`b$`, %`c$`, %`n`)
: within %`a$`, replaces the first %`n` matches of regular expression %`b$`
with the regular expression %`c$`, or all if %`n` = 0.
  See @@Regular expressions@.

################################################################################
"`right$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`right$` (%`string$`, %`n`)
: return the %`n` last characters in %`string$`.

################################################################################
"`rindex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`rindex` (%`string$`, %`part$`)
: return the last location of the substring %`part$` inside %`string$`, or 0 if not found.

#`rindex` (%`strings$#`, %`s$`)
: look up whether and where %`s$` last occurs in %`strings$`.

See also
========
- @`index` for the %first occurrence
- @`rindex_caseInsensitive` for %%case-insensitive% matching
- @`rindex_regex` for %%regular-expression% matching

################################################################################
"`rindex_caseInsensitive`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`rindex_caseInsensitive` (%`string$`, %`part$`)
: return the last location of the substring %`part$` inside %`string$`, or 0 if not found.

#`rindex_caseInsensitive` (%`strings$#`, %`s$`)
: look up whether and where %`s$` last occurs in %`strings$#`.

Case insensitivity
==================
For determining where the substring is located,
it doesn’t matter whether %`string$` and/or %`part$` are in lower case or upper case.
Here is a comparison with @`rindex`:
{
	assert \`{rindex} ("hello", "L") = 0
	assert \#{rindex_caseInsensitive} ("hello", "L") = 4
	assert \`{rindex} ("heLlo", "L") = 3
	assert \#{rindex_caseInsensitive} ("heLlo", "L") = 4
	assert \`{rindex} ("helLo", "L") = 4
	assert \#{rindex_caseInsensitive} ("helLo", "L") = 4
	assert \`{rindex} ("heLLo", "L") = 4
	assert \#{rindex_caseInsensitive} ("heLLo", "L") = 4
}
See also
========
- @`index_caseInsensitive` for the %first occurrence
- @`rindex` for %%case-sensitive% matching

################################################################################
"`rindex_regex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`rindex_regex` (%`string$`, %`regex$`)
: return the last match of the regular expression %`regex$` inside %`string$`, or 0 if not found.
  See @@Regular expressions@.

################################################################################
"`round`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`round` (%`x`)
: round %`x` to the nearest integer number.

{
	assert round (1.3) = 1
	assert round (1.7) = 2
	assert round (-1.3) = -1
	assert round (-1.7) = -2
}
Halves are rounded up:
{
	assert round (-2.5) = -2
	assert round (-1.5) = -1
	assert round (-0.5) = 0
	assert round (0.5) = 1
	assert round (1.5) = 2
	assert round (2.5) = 3
}

################################################################################
"`round#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`round#` (%`v#`)
: round (@`round`) each element of the vector %`v#`.

################################################################################
"`round##`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`round##` (%`m##`)
: round (@`round`) each cell of the matrix %`m##`.

################################################################################
"`row`"
© Paul Boersma 2024

The row number (of the current cell) in a Formula command.

See @@Formulas 1.7. Formulas for modification@.

################################################################################
"`row$`"
© Paul Boersma 2024

The name of a row of an object.

Syntax and semantics
====================
#`row$` [%`i`]
: the name of the %%i%th row of an object.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`row#`"
© Paul Boersma 2024

A function that can be used in @Formulas.

Syntax and semantics
====================
#`row#` (%`m##`, %`i`)
: extract the %`i`th row from the matrix %`m##`.

Examples
========
{
	\`{assert} row# ({{ 7, 4 }, { 99, 103 }}, 2) = { 99, 103 }
}

################################################################################
"`rowSums#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`rowSums#` (%`m##`)
: compute the sum of the elements of each row of the matrix %`m##`.

################################################################################
"`runScript`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`runScript` (%`filePath$`, `...`)
: run the script given by the (relative or absolute) %`filePath$`,
with optional arguments given in “`...`”.

If there are any arguments, they will be consumed by the `form` in the script.
For details and examples, see @@Scripting 6.1. Arguments to the script@.

################################################################################
"`runSubprocess`"
© Paul Boersma 2023,2026

A function that can be used in @Scripting.

Syntax and semantics
====================
#`runSubprocess` (%`executableFilePath$`, `...`)
: run the program given by the (relative or absolute) %`executableFilePath$`,
with optional arguments given in “`...`”.

If the subprocess fails in its execution, #`runSubprocess` will report
the subprocess’ error message and stop the script.

Examples
========
{;
	\`{writeInfoLine}: “start”
	\#{runSubprocess}: “C:\\Windows\Notepad.exe”
	\`{appendInfoLine}: “end”
}
This script will write “start” into the Info window, then start up `Notepad.exe`,
wait until you close `Notepad`, and only then write “end” into the Info window.
That is, the subprocess is run %synchronously.

You can use @`asynchronous` in combination with #`runSubprocess`:
{;
	\`{writeInfoLine}: “start”
	\#`{asynchronous} \#{runSubprocess}: “C:\\Windows\Notepad.exe”
	\`{appendInfoLine}: “end”
}
This script will write “start” into the Info window, then start up `Notepad.exe`,
and then immediately write “end” into the Info window.
Whatever you subsequently do in `Notepad` (including closing that app) will have no influence on your script
or on Praat in general.

For some more details and examples, see @@Scripting 6.5. Calling system commands@.

################################################################################
"`runSubprocess$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`runSubprocess$` (%`executableFilePath$`, `...`)
: run the program given by the (relative or absolute) %`executableFilePath$`,
with optional arguments given in “`...`”, and return the output of that program.

For details and examples, see @@Scripting 6.5. Calling system commands@.

################################################################################
"`runSystem`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`runSystem` (`...`)
: run the command line given by the arguments in “`...`”,
which are plainly concatenated (in the same way as in `writeInfo`).

For details and examples, see @@Scripting 6.5. Calling system commands@.

################################################################################
"`runSystem$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`runSystem$` (`...`)
: run the command line given by the arguments in “`...`”,
which are plainly concatenated (in the same way as in `writeInfo`),
and return the output of that command line.

For details and examples, see @@Scripting 6.5. Calling system commands@.

################################################################################
"`selected`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`selected` ( )
: return the ID of the topmost selected object.

#`selected` (%`i`)
: return the ID of the %%i%th selected object (as counted from the top, or from the bottom if %`i` is negative).

#`selected` (%`type$`)
: return the ID of the topmost selected object of type %`type$`.

#`selected` (%`type$`, %`i`)
: return the ID of the %%i%th selected object of type %`type$` (as counted from the top, or from the bottom if %`i` is negative).

Usage
=====
For examples, see @@Scripting 4.3. Querying objects@.

################################################################################
"`selected$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`selected$` ( )
: return the full name (type + given name) of the topmost selected object.

#`selected$` (%`i`)
: return the full name (type + given name) of the %%i%th selected object (as counted from the top, or from the bottom if %`i` is negative).

#`selected$` (%`type$`)
: return the given name of the topmost selected object of type %`type$`.

#`selected$` (%`type$`, %`i`)
: return the given name of the %%i%th selected object of type %`type$` (as counted from the top, or from the bottom if %`i` is negative).

Usage
=====
This function is useful if you want to write or draw the name of the object.
For selecting an object, using @`selected` is safer,
as explained in @@Scripting 4.3. Querying objects@.

################################################################################
"`selected#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`selected#` ( )
: return a list of the IDs of all currently selected objects.

#`selected#` (%`type$`)
: return a list of the IDs of all currently selected objects of type %`type$`.

Example
=======
Assume that at the start of the following script, the list of objects is empty:
{
	sound = Create Sound from formula: "sine377", 1, 0, 0.1, 44100,
	... ~ sin (2*pi*377*x)
	pitch = To Pitch: 0.01, 75, 600
	plusObject: sound
	pulses = To PointProcess (cc)
	plusObject: sound
}
At this point, the list of objects will contain three objects,
which will look as follows (ID, type name, given name),
where the two that stand selected are given in bold:
`
	\#{1. Sound sine377}
	2. Pitch sine377
	\#{3. PointProcess sine377_sine377}
`
The IDs of the two selected objects are 1 and 3, respectively,
and this is what `selected#` ( ) will show:
{
	writeInfoLine: \#{selected#} ()
}
We can also just list the selected Sound objects:
{
	writeInfoLine: \#{selected#} ("Sound")
}
or the selected Pitch objects (there should be none):
{
	writeInfoLine: \#{selected#} ("Pitch")
}
or the selected PointProcess objects:
{
	writeInfoLine: \#{selected#} ("PointProcess")
}
Test
====
An automated test:
{
	\`{assert} \#{selected#} () = { 1, 3 }
	\`{assert} \#{selected#} ("Sound") = { 1 }
	\`{assert} \#{selected#} ("Pitch") = zero# (0)
	\`{assert} \#{selected#} ("PointProcess") = { 3 }
}

################################################################################
"`selected$#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`selected$#` ( )
: return a list of the full names of all currently selected objects.

#`selected$#` (%`type$`)
: return a list of the given names of all currently selected objects of type %`type$`.

Pitfall
=======
You cannot normally use this function to cycle through all selected objects,
because if two objects happen to have the same full name,
you will select the same object twice. To cycle through all selected objects,
use @`selected#` instead, because the IDs are guaranteed to be unique.

Example
=======
Assume that at the start of the following script, the list of objects is empty:
{
	sound = Create Sound from formula: "sine377", 1, 0, 0.1, 44100,
	... ~ sin (2*pi*377*x)
	pitch = To Pitch: 0.01, 75, 600
	plusObject: sound
	pulses = To PointProcess (cc)
	plusObject: sound
}
At this point, the list of objects will contain three objects,
which will look as follows (ID, type name, given name),
where the two that stand selected are given in bold:
`
	\#{1. Sound sine377}
	2. Pitch sine377
	\#{3. PointProcess sine377_sine377}
`
The full names of the two selected objects are `Sound sine377`
and `PointProcess sine377_sine377`, respectively,
and this is what `selected$#` ( ) will show:
{
	writeInfoLine: \#{selected$#} ()
}
or
{
	writeInfoLine: vertical$: \#{selected$#} ()
}
We can also just list the given names of the selected Sound objects:
{
	writeInfoLine: \#{selected$#} ("Sound")
}
or of the selected Pitch objects (there should be none):
{
	writeInfoLine: \#{selected$#} ("Pitch")
}
or of the selected PointProcess objects:
{
	writeInfoLine: \#{selected$#} ("PointProcess")
}
Note that here we see only the given names (not the full names),
because the type names are already known.

Test
====
An automated test:
{
	\`{assert} \#{selected$#} () = { “Sound sine377”,
	... “PointProcess sine377_sine377” }
	\`{assert} \#{selected$#} ("Sound") = { “sine377” }
	\`{assert} \#{selected$#} ("Pitch") = empty$# (0)
	\`{assert} \#{selected$#} ("PointProcess") = { “sine377_sine377” }
}

################################################################################
"`selectObject`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`selectObject` (`...`)
: select the object(s) given by IDs and/or full names (full name = type + given name),
after deselecting any objects that are currently selected.

################################################################################
"`semitonesToHertz`"
© Paul Boersma 2023-06-30

A function that can be used in @Formulas.

Syntax and semantics
====================
#`semitonesToHertz` (%`x`)
: convert a value %`x` along a logarithmic acoustic scale
in semitones (relative to 100 Hz) to an acoustic frequency in Hz.

Definition
==========
~ #`semitonesToHertz` (%x) = 100 exp (%x ln 2 / 12).

################################################################################
"`sigmoid`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sigmoid` (%`x`)
: compute 1 / (1 + %e^^-%`x`^) or 1 − 1 / (1 + %e^%`x`)

Domain and range
================
#`sigmoid` (%x) is defined for all real numbers %x.
The range are real values between 0 (not included) and 1 (not included).

################################################################################
"`sigmoid#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sigmoid#` (%`v#`):
: compute the @`sigmoid` of each element of the vector %`v#`.

################################################################################
"`sigmoid##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sigmoid##` (%`m##`)
: compute the @`sigmoid` of each cell of the matrix %`m##`.

################################################################################
"`sin`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sin` (%`x`)
: compute the sine of %`x`.
{
	Axes: -5, 5, -1.1, 1.1
	Draw inner box
	Draw function: -5, 5, 1000, ~ \#{sin} (x)
	One mark bottom: -3*pi/2, "no", "yes", "yes", "\-m3%\pi/2"
	One mark bottom: -pi, "no", "yes", "yes", "\-m%\pi"
	One mark bottom: -pi/2, "no", "yes", "yes", "\-m%\pi/2"
	One mark bottom: 0, "no", "yes", "yes", "0"
	One mark bottom: pi/2, "no", "yes", "yes", "+%\pi/2"
	One mark bottom: pi, "no", "yes", "yes", "+%\pi"
	One mark bottom: 3*pi/2, "no", "yes", "yes", "+3%\pi/2"
	One mark left: -1, "no", "yes", "yes", "\-m1"
	One mark left: 0, "no", "yes", "yes", "0"
	One mark left: 1, "no", "yes", "yes", "+1"
	Text bottom: "yes", "%x \->"
	Text left: "yes", "##\#{sin}# (%x)"
	Text top: "no", "##just over one and a half periods of a sine wave"
}
This picture illustrates the following properties of the sine function:

- #`sin` (%x) is zero for all values of %x that are integer multiples of %\pi.
- The extrema of #`sin` (%x) are \-m1 and +1.
- #`sin` (%x) is \-m1 or +1 wherever %x is an odd multiple of %\pi/2.
- The function is %periodic with period 2%\pi, i.e., #`sin` (%x+2%\pi) = #`sin` (%x).
- The function is %symmetric around %\pi/2, i.e. #`sin` (%\pi/2\-m%x) = #`sin` (%x).
- The function is %antisymmetric around 0, i.e. #`sin` (\-m%x) = \-m #`sin` (%x);
  in other words, if you take the sine curve and rotate it by 180 degrees
  around the origin (i.e. the point (0, 0)), then you get the same curve again.

################################################################################
"`sin#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
`sin#` (%`v#`)
: compute the sine (@`sin`) of each element of the vector %`v#`.

################################################################################
"`sin##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
`sin##` (%`m##`)
: compute the sine (@`sin`) of each cell of the matrix %`m##`.

################################################################################
"`sinc`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sinc` (%`x`)
: compute the sinus cardinalis of %`x`.

Definition
==========
~	#`sinc` (%x) = sin (%x) / %x

################################################################################
"`sincpi`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sinc` (%`x`)
: compute the sinus cardinalis (@`sinc`) of %\pi%`x`.

Definition
==========
~	#`sincpi` (%x) = sin (%\pi%x) / (%\pi%x)

################################################################################
"`sinh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sinh` (%`x`)
: compute the hyperbolic sine of %`x`.

Definition
==========
~	#`sinh` (%x) = (%e^%x \-m %e^^\-m%x^) / 2

################################################################################
"`sinh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sinh#` (%`v#`)
: compute the hyperbolic sine (@`sinh`) of each element of the vector %`v#`.

################################################################################
"`sinh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sinh##` (%`m##`)
: compute the hyperbolic sine (@`sinh`) of each cell of the matrix %`m##`.

################################################################################
"`size`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`size` (%`v#`)
: return the number of elements of the vector %`v#`.

################################################################################
"`shuffle#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`shuffle#` (%`v#`)
: return a vector with the same elements as %`v#`, but in randomized order.

################################################################################
"`shuffle$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`shuffle$#` (%`stringVector$#`)
: return a vector with the same elements as %`stringVector$#`, but in randomized order.

################################################################################
"`sleep`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`sleep` (%`duration`)
: pause the script for %`duration` seconds.

This can be used in animations, e.g. in the @@Demo window@.

################################################################################
"`softmax#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`softmax#` (%`v#`)
: convert the elements of the vector %`v` into their softmax values.

################################################################################
"`softmaxPerRow##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`softmaxPerRow##` (%`m##`)
: convert the elements of each row of the matrix %`m`
into their softmax (@`softmax#`) values (within that row).

################################################################################
"`solve#`"
© Paul Boersma 2023

See @@solving matrix equations@.

################################################################################
"`solve##`"
© Paul Boersma 2023

See @@solving matrix equations@.

################################################################################
"`solveNonnegative#`"
© Paul Boersma 2023

See @@solving matrix equations@.

################################################################################
"`solveSparse#`"
© Paul Boersma 2023

See @@solving matrix equations@.

################################################################################
"`solveWeaklyConstrained#`"
© Paul Boersma 2023

See @@solving matrix equations@.

################################################################################
"`sort#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sort#` (%`v#`)
: return a vector with the same elements as %`v`, but in sorted order (from low to high).

################################################################################
"`sort$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sort$#` (%`stringVector$#`)
: return a string array with the same elements as %`stringVector$#`,
but in sorted (Unicode-alphabetical) order.

################################################################################
"`sort_numberAware$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sort_numberAware$#` (%`stringVector$#`)
: return a string array with the same elements as %`stringVector$#`,
but in sorted (Unicode-alphabetical) order, with special attention to numbers.

################################################################################
"`splitByWhitespace$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`splitByWhitespace$#` (%`string$`) splits a string into inks.

################################################################################
"`sqrt`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sqrt` (%`x`)
: take the square root of %`x`.

{
	assert sqrt (0) = 0
	assert sqrt (1) = 1
	assert sqrt (4) = 2
	assert sqrt (9) = 3
	assert (sqrt (2) - 1.41421356237309504880168872420969807856967187537694807317667973799) < 1e-17
}

################################################################################
"`sqrt#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#sqrt#` (%`v#`)
: take the square root (@`sqrt`) of each element of the vector %`v#`.

################################################################################
"`sqrt##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#sqrt##` (%`m##`)
: take the square root (@`sqrt`) of each cell of the matrix %`m##`.

################################################################################
"`startsWith`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`startsWith` (%`string$`, %`part$`)
: determine whether %`string$` starts with %`part$`.

See also
========
- @`endsWith` for determining whether %`string$` %ends in %`part$`.
- @`startsWith_caseInsensitive` for %%case-insensitive% matching.

################################################################################
"`startsWith_caseInsensitive`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`startsWith_caseInsensitive` (%`string$`, %`part$`)
: determine whether %`string$` starts with %`part$`.

See also
========
- @`endsWith_caseInsensitive` for determining whether %`string$` %ends in %`part$`.
- @`startsWith` for %%case-sensitive% matching.

################################################################################
"`stdev`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`stdev` (%`v#`)
: compute the standard deviation of the elements of the vector %`v#`.

################################################################################
"`stopwatch`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`stopwatch`
: give the current value of the timer, which is then immediately reset to zero (note: no parentheses).

################################################################################
"`string$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`string$` (%`number`)
: format a number as a string.

################################################################################
"`studentP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`studentP` (%`t`, %`df`)
: compute the area under Student’s %T distribution from \-m\oo to %`t`.

################################################################################
"`studentQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`studentQ` (%`t`, %`df`)
: compute the area under Student’s %T distribution from %`t` to +\oo.

################################################################################
"`sum`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sum` (%`v#`)
: compute the sum of the elements of the vector %`v`.

################################################################################
"`sumOver`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sumOver` (%`loopVariable` `to` %`n`, %`numericExpressionOptionallyUsingLoopVariable`)
: compute \Si_1^%`n` %`numericExpressionOptionallyUsingLoopVariable`.

################################################################################
"`tan`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tan` (%`x`)
: compute the tangent of %`x`.

Definition
==========
~	#`tan` (%x) = sin (%x) / cos (%x)

################################################################################
"`tan#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tan#` (%`v#`)
: compute the tangent (@`tan`) of each element of the vector %`v#`.

################################################################################
"`tan##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tan##` (%`m##`)
: compute the tangent (@`tan`) of each cell of the matrix %`m##`.

################################################################################
"`tanh`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tanh` (%`x`)
: compute the hyperbolic tangent of %`x`.

Definition
==========
~	#`tanh` (%x) = @`sinh` (%x) / @`cosh` (%x)

################################################################################
"`tanh#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tanh#` (%`v#`)
: compute the hyperbolic tangent (@`tanh`) of each element of the vector %`v#`.

################################################################################
"`tanh##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`tanh##` (%`m##`)
: compute the hyperbolic tangent (@`tanh`) of each cell of the matrix %`m##`.

################################################################################
"`to#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`to#` (%`n`)
: lists the integers 1 through %`n`.

################################################################################
"`transpose##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`transpose##` (%`m##`)
: flip the matrix %`m##` along its diagonal.

################################################################################
"`truncateLeft$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`truncateLeft$` (%`string$`, %`maximumNewWidth`)
: remove characters from the left of %`string$` until the result is at least as short as %`maximumNewWidth`:
{
	\`{assert} \#{truncateLeft$} ("hello", 3) = "llo"
	\`{assert} \#{truncateLeft$} ("hello", 5) = "hello"
}
No padding takes place, so the resulting string may be shorter than %`maximumNewWidth`:
{
	\`{assert} \#{truncateLeft$} ("hello", 12) = "hello"
}
Note that %`maximumNewWidth` cannot be negative:
{
	\`{asserterror} Can never truncate a string down to -3 characters.
	a$ = \#{truncateLeft$} ("hello", -3)
}

################################################################################
"`truncateRight$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`truncateRight$` (%`string$`, %`maximumNewWidth`)
: remove characters from the left of %`string$` until the result is at least as short as %`maximumNewWidth`:
{
	\`{assert} \#{truncateRight$} ("hello", 3) = "hel"
	\`{assert} \#{truncateRight$} ("hello", 5) = "hello"
}
No padding takes place, so the resulting string may be shorter than %`maximumNewWidth`:
{
	\`{assert} \#{truncateRight$} ("hello", 12) = "hello"
}
Note that %`maximumNewWidth` cannot be negative:
{
	\`{asserterror} Can never truncate a string down to -3 characters.
	a$ = \#{truncateRight$} ("hello", -3)
}

################################################################################
"`tryToAppendFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`tryToAppendFile` (%`filePath$`)
: check whether this file can be appended to.

The file is opened for appending, then closed
(if the file already existed, it is not changed).

Return value
============
Upon success (i.e. the file already existed and can be appended to,
or the file didn’t exist yet but was successfully created), this function returns 1 (true).
Otherwise, the function returns 0 (false); this can happen
if the folder in the path does not exist,
or if the file is (or would be) on a read-only device.

For instance, if my TIMIT database is in a read-only disk image called `TIMIT.dmg`,
and I mounted that disk image on my Mac (by double-clicking `TIMIT.dmg`), then trying
{;
	\#{tryToAppendFile} ("/Volumes/TIMIT/readme.doc")
}
would return 0.

Related function
================
For a non-destructive version of this function, see `tryToWriteFile`.

################################################################################
"`tryToWriteFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`tryToWriteFile` (%`filePath$`)
: check whether this file can be written to.

The file is opened for writing, then closed
(if the file already existed, it is emptied!).

Return value
============
Upon success (i.e. the file already existed and was successfully emptied,
or the file didn’t exist yet but was successfully created), this function returns 1 (true).
Otherwise, the function returns 0 (false); this can happen
if the folder in the path does not exist,
or if the file is (or would be) on a read-only device.

For instance, if my TIMIT database is in a read-only disk image called `TIMIT.dmg`,
and I mounted that disk image on my Mac (by double-clicking `TIMIT.dmg`), then trying
{;
	\#{tryToWriteFile} ("/Volumes/TIMIT/hello.txt")
}
would return 0.

Related function
================
For a non-destructive version of this function, see `tryToAppendFile`.

################################################################################
"`unicode`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`unicode` (%`c$`)
: return the Unicode codepoint number that corresponds to the character %`c$`.

################################################################################
"`unicode$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`unicode$` (%`n`)
: return the character that corresponds to the Unicode codepoint %`n`.

################################################################################
"`unicodeToBackslashTrigraphs$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`unicodeToBackslashTrigraphs$` (%`string$`)
: convert from unicode characters to backslash trigraphs,
e.g. from “\ct” to “\bsct”.

################################################################################
"`upperCase$`"
© Paul Boersma 2025

A function that can be used in @@Formulas@. It turns all lower-case characters
into capitals.

Syntax and semantics
====================
#`upperCase$` (%`string$`)
: give the upper-case version of %`string$`, whereby all lower-case letters
  have been replaced with upper-case letters.

Tests
=====
{
	\`{assert} \#{upperCase$} ("HOwAREYou") = "HOWAREYOU"
	\`{assert} \#{upperCase$} ("Maria Ressa") = "MARIA RESSA"
	\`{assert} \#{upperCase$} ("e.g.") = "E.G."
	\`{assert} \#{upperCase$} ("öav") = "ÖAV"
	\`{assert} \#{upperCase$} ("touché") = "TOUCHÉ"
}

################################################################################
"`upperCaseAppName$`"
© Paul Boersma 2024

A function that can be used in @@Formulas@. The name of the present app,
in upper camel case, such as `Praat` or `PraatForHospitals`.

Syntax and semantics
====================
#`upperCaseAppName$` ( )
: give the name of the present app, in upper camel case.

Examples
========
{
	writeInfoLine: \#{upperCaseAppName$} ( )
}

################################################################################
"`variableExists`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Syntax and semantics
====================
#`variableExists` (%`variableName$`)
: return 1 (true) if the variable %`variableName$` exists, else 0 (false).

################################################################################
"`vertical$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`vertical$` (%`stringArray$#`)
: format a string array as a vertical column of strings,
by concatenating the elements while separating them by newlines.

################################################################################
"`writeFile`"
© Paul Boersma 2023

A function that writes its arguments (texts, numbers, vectors and so on) to a new file.
If the file already existed, anything that was previously present in the file is overwritten.

Examples of use:
================

################################################################################
"`writeFileLine`"
© Paul Boersma 2023

A function that writes its arguments (texts, numbers, vectors and so on, plus a newline symbol, to a new file.
If the file alreasdy existed, anything that was previously present in the file is overwritten.

Examples of use:
================

################################################################################
"`writeInfo`"
© Paul Boersma 2023

A function that writes its arguments to the Info window,
overwriting what was previously present in the Info window.

Examples of use:
================

################################################################################
"`writeInfoLine`"
© Paul Boersma 2023

A function that writes its arguments to the Info window,
overwriting what was previously present in the Info window,
and moving the output cursor to the next line.

Examples of use:
================

################################################################################
"`x`"
© Paul Boersma 2024

The horizontal place (in world coordinates) of the current cell in a Formula command.

See @@Formulas 1.7. Formulas for modification@.

################################################################################
"`xmax`"
© Paul Boersma 2024

The top of the horizontal domain of an object, in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`xmin`"
© Paul Boersma 2024

The bottom of the horizontal domain of an object, in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`y`"
© Paul Boersma 2024

The vertical place (in world coordinates) of the current cell in a Formula command.

See @@Formulas 1.7. Formulas for modification@.

################################################################################
"`ymax`"
© Paul Boersma 2024

The top of the vertical domain of an object, in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`ymin`"
© Paul Boersma 2024

The bottom of the vertical domain of an object, in world coordinates.

See @@Formulas 8. Attributes of objects@.

################################################################################
"`zero#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`zero#` (%`n`)
: return a vector with %`n` elements that are all 0.0.

################################################################################
"`zero##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`zero##` (%`nrow`, %`ncol`)
: return a matrix with %`nrow` \xx %`ncol` cells that are all 0.0.

################################################################################
)~~~"
MAN_PAGES_END

}

/* End of file manual_functions.cpp */
