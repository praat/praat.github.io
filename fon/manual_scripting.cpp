/* manual_scripting.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
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

void manual_scripting_init (ManPages me);
void manual_scripting_init (ManPages me) {

MAN_BEGIN (U"Paste history", U"ppgb", 20050822)
INTRO (U"A command in the #Edit menu of a @ScriptEditor, for inserting the history of commands. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN (U"Clear history", U"ppgb", 20000927)
INTRO (U"A command in the Edit menu of the @ScriptEditor for clearing the remembered history. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN (U"History mechanism", U"ppgb", 20040414)
INTRO (U"The easiest way to do @@scripting@. "
	"The %history is the sequence of all menu commands "
	"(in the Objects or Picture window or in the editors), "
	"action commands (in the dynamic menu), "
	"or mouse clicks on objects (in the list of objects), that you performed during your Praat session, "
	"together with the settings that you specified in the settings windows "
	"that popped up as a result of those commands.")
ENTRY (U"Viewing the history")
NORMAL (U"To view your history, you first open a @ScriptEditor with @@New Praat script@ or @@Open Praat script...@. "
	"You then choose @@Paste history@ from the #Edit menu.")
ENTRY (U"Recording a macro")
NORMAL (U"To record a sequence of mouse clicks for later re-use, "
	"perform the following steps:")
LIST_ITEM (U"1. Choose @@Clear history@ from the #Edit menu. "
	"This makes the history mechanism forget all previous clicks.")
LIST_ITEM (U"2. Perform the actions that you want to record.")
LIST_ITEM (U"3. Choose @@Paste history@ from the #Edit menu. Because you cleared the history "
	"before you started, the resulting script contains only the actions "
	"that you performed in step 2. "
	"You can now already re-run the actions that you performed in step 2.")
LIST_ITEM (U"4. You can save the recorded actions to a script file by choosing #Save from the #File menu.")
LIST_ITEM (U"5. You can put this script file under a button in the @@dynamic menu@ "
	"by choosing @@Add to dynamic menu...@ from the File menu, "
	"or under a button in a fixed menu by choosing @@Add to fixed menu...@. "
	"This button will be preserved across Praat sessions.")
NORMAL (U"This macro mechanism is much more flexible than the usual opaque macro mechanism "
	"used by most programs, because you can edit the script and make some "
	"of the arguments variable by putting them in the #form clause at the top of the script. "
	"In this way, the script will prompt the user for these arguments, "
	"just as with all the menu and action commands that end in the three dots (...). "
	"See the @Scripting tutorial for all the things that you can do in scripts.")
MAN_END

MAN_BEGIN (U"New Praat script", U"ppgb", 20050822)
INTRO (U"A command in the @@Praat menu@ for creating a new Praat script. "
	"It creates a @ScriptEditor with an empty script that you can edit, run, and save.") 
MAN_END

MAN_BEGIN (U"New Praat notebook", U"ppgb", 20230325)
INTRO (U"A command in the @@Praat menu@ for creating a new Praat notebook. "
	"It creates a @NotebookEditor with an empty notebook that you can edit, run, and save.")
MAN_END

MAN_BEGIN (U"Open Praat script...", U"ppgb", 20050822)
INTRO (U"A command in the @@Praat menu@ for editing an existing @@Praat script@. "
	"It creates a @ScriptEditor and asks "
	"you to select a file. If you click #%OK, the file is read into the ScriptEditor window, "
	"and you can run and edit it; if you click #%Cancel, you get an empty script, as with @@New Praat script@.")
MAN_END

MAN_BEGIN (U"Open Praat notebook...", U"ppgb", 20230325)
INTRO (U"A command in the @@Praat menu@ for editing an existing @@Praat notebook@. "
	"It creates a @NotebookEditor and asks "
	"you to select a file. If you click #%OK, the file is read into the NotebookEditor window, "
	"and you can run and edit it; if you click #%Cancel, you get an empty notebook, as with @@New Praat notebook@.")
MAN_END

MAN_BEGIN (U"Praat script", U"ppgb", 20230325)
INTRO (U"An executable program text in the Praat scripting language, which consists of menu commands, action commands, computations and control flow.")
NORMAL (U"See the @Scripting tutorial.")
MAN_END

MAN_BEGIN (U"Praat notebook", U"ppgb", 20230325)
INTRO (U" A text document with which you can create things that look like Praat’s manual pages, "
	"with capabilities of graphics and @scripting.")
MAN_END

MAN_BEGIN (U"NotebookEditor", U"ppgb", 20230325)
INTRO (U"An aid to documented @@scripting@ as well as to creating manual pages.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Scripting"
© Paul Boersma 2000,2002–2004,2008,2010,2011,2013,2014,2017,2020,2024

This is one of the tutorials of the Praat program. It assumes you are familiar with the @Intro.

A %script is a text that consists of menu commands and action commands.
If you %run the script (perhaps from a @ScriptEditor),
the commands are executed as if you clicked on them.
You can read this tutorial sequentially with the help of the “##< 1#” and “##1 >#” buttons.

, @@Scripting 1. Your first scripts@ (how to create, how to run, how to save)
, @@Scripting 2. How to script settings windows@ (numeric, boolean, multiple-choice, text, file)
, @@Scripting 3. Simple language elements
	, @@Scripting 3.1. Hello world@ (`writeInfoLine`, `appendInfoLine`)
	, @@Scripting 3.2. Numeric variables@ (assignments)
	, @@Scripting 3.3. Numeric queries
	, @@Scripting 3.4. String variables@ (assignments)
	, @@Scripting 3.5. String queries
	, @@Scripting 3.6. “For” loops@ (`for`, `endfor`)
	, @@Scripting 3.7. Layout@ (white space, comments, continuation lines)
, @@Scripting 4. Object selection@
	, @@Scripting 4.1. Selecting objects
	, @@Scripting 4.2. Removing objects
	, @@Scripting 4.3. Querying objects
, @@Scripting 5. Language elements reference@
	, @@Scripting 5.1. Variables@ (numeric, string)
	, @@Scripting 5.2. Expressions@ (numeric, string)
	, @@Scripting 5.3. Jumps@ (`if`, `then`, `elsif`, `else`, `endif`)
	, @@Scripting 5.4. Loops@ (`for`/`endfor`, `while`/`endwhile`, `repeat`/`until`)
	, @@Scripting 5.5. Procedures@ (`@`, `procedure`)
	, @@Scripting 5.6. Arrays and dictionaries
	, @@Scripting 5.7. Vectors and matrices
	, @@Scripting 5.8. Including other scripts
	, @@Scripting 5.9. Quitting@ (`exitScript`)
, @@Scripting 6. Communication outside the script
	, @@Scripting 6.1. Arguments to the script@ (`form`/`endform`, `runScript`)
	, @@Scripting 6.2. Writing to the Info window@ (`writeInfoLine`, `appendInfoLine`, `appendInfo`, `tab$`)
	, @@Scripting 6.3. Query commands@ (#Get, #Count)
	, @@Scripting 6.4. Files@ (`fileReadable`, `readFile`, `writeFile`, `deleteFile`, `createFolder`)
	, @@Scripting 6.5. Calling system commands@ (`runSystem`, `environment$`, `stopwatch`)
	, @@Scripting 6.6. Controlling the user@ (`pause`, `beginPause`/`endPause`, `chooseReadFile$`)
	, @@Scripting 6.7. Sending a message to another program@
	, @@Scripting 6.8. Messages to the user@ (`exitScript`, `assert`, `nowarn`, `nocheck`)
	, @@Scripting 6.9. Calling from the command line
, @@Scripting 7. Scripting the editors
	, @@Scripting 7.1. Scripting an editor from a shell script@ (`editor`/`endeditor`)
	, @@Scripting 7.2. Scripting an editor from within
, @@Scripting 8. Controlling Praat from another program
	, @@Scripting 8.1. The sendpraat subroutine
	, @@Scripting 8.2. The sendpraat program
, @@Scripting 9. Turning a script into a stand-alone program
, @@Scripting 10. Old functions

Also see the @@scripting examples@.

################################################################################
"Scripting 1. Your first scripts"
© Paul Boersma 2000,2002,2004,2005,2008(“My first script”),2011,2013,2014,2020,2023

This page tells you how to create, run and save a script.
To get a feel for how it works, you are advised to try out all the steps.

1. A minimal script
===================
Suppose that you want to create a script that allows you to play a selected Sound object twice.
You first create an empty script, by choosing @@New Praat script@ from the Praat menu in the Objects window.
A @ScriptEditor window will appear on your screen:
{- 6x4
	)~~~"
		Manual_DRAW_WINDOW (4, "untitled script", "File   Edit   Search   Convert   Font   Run   Help")
	R"~~~(
	Draw rectangle: 0, 560, 0, 360
	info$ = Picture info
	fontSize = extractNumber (info$, "Font size: ")
	;Text: 50, "centre", 50, "half", string$ (fontSize)
}
In this window, you type
{;
	Play
	Play
}
Now select a Sound in the Objects window. As you expect from selecting a Sound, a #Play button will
appear in the dynamic menu. If you now choose #Run from the Run menu in the ScriptEditor,
Praat will play the sound twice.
This works because #Play is a command that becomes available in the dynamic menu when you select a Sound.

2. Some more commands
=====================
In the above example, you could use $Play in your script because the #Play command was available,
i.e. because “Play” was the text on a button currently available in the dynamic menu.
Apart from these selection-dependent (dynamic) commands,
you can also use all fixed commands from the menus of the @@Objects window@
and the @@Picture window@. For instance, try the following script:
{;
	Erase all
	Draw inner box
	Play
	Play
	Erase all
}
When you run this script, you’ll see a rectangle appear in the Picture window
(that’s what the command ##Draw inner box# in the #Margins menu does),
then you’ll hear the Sound play twice, then you’ll see the rectangle disappear from the Picture window
(that’s what the command ##Erase all# from the Edit menu does).

Here we see that the Praat scripting language is an example of a %%procedural programming language%,
which means that the five %statements are executed in the order in which they appear in the script,
i.e. first ##Erase all#, then ##Draw inner box#, then #Play twice, and finally ##Erase all#.

3. Experimenting with your script
=================================
You don't have to be afraid of making mistakes. Here are a couple that you can try to make.

First, try to run the script when a Sound is not selected
(e.g. you create a Pitch object from it and keep that selected, or you throw away the Sound).
You will notice that Praat gives you an error message saying
“The command “Play” is not available for the current selection”.
Indeed, if you select a Pitch or if you select nothing, then no command #Play appears in the dynamic menu,
so the script cannot execute it. Note that the commands ##Erase all# and ##Draw inner box# are still available,
because they continue to be present in the menus of the Picture window;
therefore, the script will execute the first two lines (`Erase all` and `Draw inner box`)
and stop running at the third line, i.e. at your first `Play`.
The result is that the “box” will stay visible in the Picture window, because the fifth line of the script,
which should erase the box, is never executed.

Second, try to mistype a command (there’s a good chance you already did it by accident),
e.g. write `PLay` instead of `Play`, or `Draw inner bocks` or whatever.
Again, you are likely to get a message saying that that command is not available.
Such messages are the most common messages that you’ll see when writing scripts;
now you know that they mean either that you mistyped something or that you made the wrong selection.

4. Saving your script
=====================
The File menu of the ScriptEditor has a command #Save,
with which you can save your script as a file on disk, for instance under the name “test.praat”.

Please try this with the five-line script you just typed.
After saving the script, the name of the script file will appear in the window title:
{- 6x4
	)~~~"
		Manual_DRAW_WINDOW (4, "Script “/Users/Rose/Desktop/test.praat”", "File   Edit   Search   Convert   Font   Run   Help")
	R"~~~(
	Courier
	Text: 0, “left”,  75, “half”, “\s{Erase all}”
	Text: 0, “left”,  90, “half”, “\s{Draw inner box}”
	Text: 0, “left”, 105, “half”, “\s{Play}”
	Text: 0, “left”, 120, “half”, “\s{Play}”
	Text: 0, “left”, 135, “half”, “\s{Erase all}”
	Draw rectangle: 0, 560, 0, 360
}
After you save your script, you can close the ScriptEditor window without losing the script:
you can reopen the script file by using @@Open Praat script...@ from the Praat menu,
or by choosing ##New Praat script# again, followed by ##Open...# from the ScriptEditor’s File menu.

It advisable to use “.praat” as the extension for script file names.
On the Mac, if you double-click a “.praat” file, Praat will automatically start up and show the script.
On the Mac and on Windows, if you drag a “.praat” file on the Praat icon, Praat will also start up and show the script.

################################################################################
"Scripting 2. How to script settings windows"
Paul Boersma 1999,2004,(“Arguments to commands”),2011,2013,2014,2017,2019,2020,2023

Not all menu commands are as simple as those on the @@Scripting 1. Your first scripts|previous page@,
which act immediately once you choose them from a menu (e.g. ##Play#, ##Erase all#).
Most commands in Praat require the user to supply additional information;
these are the commands whose title ends in “...”.

For instance, when you select a Sound, the command ##Draw...# will appear in the #Draw menu,
and when you click it, Praat will present you with a %%settings window%,
which asks you to supply six pieces of additional information, i.e. six so-called %settings (or in programming jargon: %arguments):

// 4 fields, of which 0 texts and 0 additional radio buttons,
// hence lines = 4 + 0 * 0.6 - 0 * 0.3 = 4.0,
// hence height = 1.4 + 4.0 * 0.4 = 3.0
{- 5.4x3.0,
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 4.0)
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= auto)")
		Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 1)
		Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU ("Drawing method", "Curve")
	R"~~~(
}
In this example, all the settings have their standard values: you want to draw the whole time domain of the Sound,
you want to have autoscaling vertically, you want to see garnishings around the picture (a box, labelled axes, and numbers),
and you want the waveform to be drawn as a curve. Pressing the OK button in the above window
is equivalent to executing the following script line:
{;
	Draw: 0, 0, 0, 0, “yes”, “Curve”
}
You see that in a script, all of the arguments are supplied after the command, preceded by a colon and separated by commas,
in the same order as in the settings window, counted from top to bottom (and, within a line, from left to right).
The texts “(= all)” and “(= auto)” above are just Praat’s explanations of what it means to type a zero in those fields
(namely “draw all times” and “use vertical autoscaling”, respectively); in a script they are superfluous and you shouldn’t write them.

If you want to draw the sound with different settings, say from 1 to 3.2 seconds, scaled between -1 and +1 instead of automatically,
with garnishings off, and with the waveform drawn as poles, you would have the following settings window:

// 4 fields, of which 0 texts and 0 additional radio buttons,
// hence lines = 4 + 0 * 0.6 - 0 * 0.3 = 4.0,
// hence height = 1.4 + 4.0 * 0.4 = 3.0
{- 5.4x3.0
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 4.0)
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "1.0", "3.2")
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "-1", "1")
		Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 0)
		Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU ("Drawing method", "Poles")
	R"~~~(
}
In a script this would look like
{;
	Draw: 1.0, 3.2, -1, 1, “no”, “Poles”
}
1. Numeric arguments
====================
The first four arguments in the above examples are %%numeric arguments%: they are (real or integer) numbers.
You just write them in the script as you would write them into the settings window.

2. Boolean (yes/no) arguments
=============================
The fifth argument in the above examples (#Garnish) is a %%boolean argument% (yes/no choice) and is represented by a %%check button%.
In the script you write it as `"yes"` (including the quotes) or `"no"` (or as 1 or 0).

3. Multiple-choice arguments
============================
The sixth argument in the above examples (##Drawing method#) is a %%multiple-choice argument% and is represented by an %%option menu%.
In the script you write the text of the choice, i.e. `"Curve"` or `"Poles"` in the examples.

A multiple-choice argument is sometimes represented by a %%choice box% instead of by an option menu.
For instance, the last example above could equally well have looked like

// 7 fields, of which 0 texts and 3 additional radio buttons,
// hence lines = 7 + 0 * 0.6 - 3 * 0.3 = 6.1,
// hence height = 1.4 + 6.1 * 0.4 = 3.84
{- 5.4x3.84,
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 6.1)
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "1.0", "3.2")
		Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "-1", "1")
		Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 0)
		Manual_DRAW_SETTINGS_WINDOW_CHOICE ("Drawing method", "Curve", 0)
		"y -= 12\n"
		Manual_DRAW_SETTINGS_WINDOW_CHOICE ("", "Bars", 0)
		"y -= 12\n"
		Manual_DRAW_SETTINGS_WINDOW_CHOICE ("", "Poles", 1)
		"y -= 12\n"
		Manual_DRAW_SETTINGS_WINDOW_CHOICE ("", "Speckles", 0)
	R"~~~(
}
In supplying arguments to a command in a script, there is no difference between an option menu and a radio box.
This last example will therefore again look like the following in a script:
{;
	Draw: 1.0, 3.2, -1, 1, “no”, “Poles”
}
4. Text arguments
=================
Consider another frequently used menu command, namely ##Create Sound from formula...# in the #New menu:

// 6 fields, of which 1 text and 0 additional radio buttons,
// hence lines = 6 + 1 * 0.6 - 0 * 0.3 = 6.6,
// hence height = 1.4 + 6.6 * 0.4 = 4.04
{- 5.4x4.04,
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Create Sound from formula", 6.6)
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Name", "sine")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Number of channels", "1")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Start time (s)", "0.0")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("End time (s)", "1.0")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Sampling frequency (Hz)", "44100")
		Manual_DRAW_SETTINGS_WINDOW_TEXT ("Formula", "1/2 * sin(2*pi*377*x)")
	R"~~~(
}
In a script this would look like:
{;
	Create Sound from formula: “sine”, 1, 0.0, 1.0, 44100, “1/2 * sin(2*pi*377*x)”
}
Both the first argument (#Name) and the sixth argument (#Formula) are %%text arguments%.
In a script they are written within quotes.

5. Formula arguments
====================
Sometimes it would be a bit awkward to write a formula that contains double quotes.
Imagine you have a @Table object with a column “stimulus”, a column “response”, and a column “correct”,
and you want to have a 1 in column “correct” if the texts in the columns “stimulus” and “response”
are the same, and a 0 otherwise. You would do this by selecting the Table and choosing the ##Formula...# command:

// 2 fields, of which 1 text and 0 additional radio buttons,
// hence lines = 2 + 1 * 0.6 - 0 * 0.3 = 2.6,
// hence height = 1.4 + 2.6 * 0.4 = 2.44
{- 5.4x2.44
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Table: Formula", 2.6)   // 0.6 extra for the text
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Column (label)", "correct")
		Manual_DRAW_SETTINGS_WINDOW_TEXT ("Formula", "self\\$  [\\\" response\\\" ] = self\\$  [\\\" stimulus\\\" ]")
	R"~~~(
}
According to section 4 above, and according to @@Formulas 2.2. Representation of strings@,
you would have to write this in the following way in a script:
{;
	Formula: "correct", "self$ [""response""] = self$ [""stimulus""]"
}
The required doubling of string-internal double quotes is awkward.
Therefore, there exists a special way for typing formula arguments, namely with the tilde ("~"):
{;
	Formula: "correct", ~ self$ ["response"] = self$ ["stimulus"]
}
This means that you can write the example of section 4 in an analogous way:
{;
	Create Sound from formula: “sine”, 1, 0.0, 1.0, 44100, ~ 1/2 * sin(2*pi*377*x)
}
The tilde is probably the preferred way to write formula arguments.
You should remember, however, that the result is still a string (i.e. text),
and you can treat it as a string with the string methods described later on in this tutorial.

Another way is to use curly quotes, for instance:
{;
	Formula: "correct", “self$ ["response"] = self$ ["stimulus"]”
}
or
{;
	Formula: "correct", "self$ [“response”] = self$ [“stimulus”]"
}
6. File arguments
=================
The commands from the Open and Save menus, and several other commands whose names
start with #Read, #Open, or #Save, present a %%file selector window% instead of a typical Praat
settings window. File selector windows ask the user to supply a single argument: the file name.

In a script you can either supply the %%complete path% to the file, or supply a %%relative path%.

A complete path includes both the folder (directory) hierarchy and the name of the file.
This goes slightly differently on the Windows platform on the one hand,
and on the Mac and Linux platforms on the other. If your user name is Miep,
and your home folder contains a folder `Sounds`,
and this folder contains a folder `Animals`, and this contains the file `miauw.wav`,
you can open that file as follows:
{;
	Read from file: “C:/Users/Miep/Sounds/Animals/miauw.wav”   ; Windows
	Read from file: “/Users/Miep/Sounds/Animals/miauw.wav”   ; Mac
	Read from file: “/home/miep/Sounds/Animals/miauw.wav”   ; Linux
}
(the part before your user name may be slightly different on your computer;
use your command or terminal window to find out)

In these examples, “C” is the Windows %%drive letter% and
`/Users/Miep` or `/home/Miep` is your %%home folder%. Both the home folder and the drive letter
can be abbreviated away by using the tilde (“~”) in the path
(this tilde has nothing to do with the tilde used in formula arguments):
{;
	Read from file: “~/Sounds/Animals/miauw.wav”
}
If your `Sounds` folder is not in your home folder but on your desktop, you do
{;
	Read from file: “~/Desktop/Sounds/Animals/miauw.wav”
}
(this works because on all three platforms, the desktop folder is a subfolder of your home folder)

If your Sounds folder is on a USB drive called PORCH, it would be something like:
{;
	Read from file: “G:/Sounds/Animals/miauw.wav”   ; Windows
	Read from file: “/Volumes/PORCH/Sounds/Animals/miauw.wav”   ; Mac
	Read from file: “/media/PORCH/Sounds/Animals/miauw.wav”   ; Linux
}
Instead of all these complete path names, you can use %relative path names.
These are taken as relative to the folder in which your script resides,
and help to make your script portable if you move the script along with your data.

Thus, if your script (after you have saved it!) is in the `Animals` folder mentioned above,
i.e. in the same folder as `miauw.wav`, you would simply open the file with
{;
	Read from file: “miauw.wav”
}
If your script is in the `Sounds` folder mentioned above,
i.e. in the same folder as where the `Animals` folder is, you would open the file with
{;
	Read from file: “Animals/miauw.aifc”
}
If your script is in the folder `Scripts` that is inside the `Sounds` folder,
i.e. if your script is a sister folder of the `Animals` folder, you would open the file with
{;
	Read from file: “../Animals/miauw.aifc”
}
where “..” is the general way on all platforms to go one folder up in the hierarchy.

Note that on Windows you could use the backslash (“\”) instead of the forward slash (“/”),
but with the forward slash your script will work on all three platforms.

7. Colour arguments
===================
Several commands in the World and Pen menus of the Picture window,
as well as many object drawing commands, ask for a colour in the following way:

// 5 fields, of which 0 texts and 0 additional radio buttons,
// hence lines = 5 + 0 * 0.6 - 0 * 0.3 = 5.0,
// hence height = 1.4 + 5.0 * 0.4 = 3.4
{- 5.4x3.4
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Praat picture: Paint rectangle", 5.0)
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colour (0-1, name, or {r,g,b})", "0.5")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("From x", "0.0")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("To x", "1.0")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("From y", "0.0")
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("To y", "1.0")
	R"~~~(
}
For the colour, you can supply, as text, one of the 16 explicit names that also appear in the Pen menu:
{
	Paint rectangle: “maroon”, 0.0, 1.0, 0.0, 1.0
}
Alternatively, you can supply a grey value, as a number between 0.0 (black) and 1.0 (white). The following produces a very light grey:
{
	Paint rectangle: 0.9, 0.0, 1.0, 0.0, 1.0
}
Finally, you can supply an explicit RGB (red–green–blue) value, as a “vector” of three numbers between 0.0 and 1.0.
The following produces dark pink:
{
	Paint rectangle: {0.8,0.2,0.4}, 0.0, 1.0, 0.0, 1.0
}
8. How to supply arguments automatically
========================================
Now you know all the ways to write the arguments of commands in a script line.
If you dislike manually copying arguments from settings windows into your script,
or if you are not sure whether something is a numeric or a string argument,
you can use the @@history mechanism@:
choose @@Clear history@ from the Edit menu in your @ScriptEditor,
click your command button, edit the arguments, and click #OK. The command will be executed.
Then choose @@Paste history@, and the command line, including the arguments,
will appear in the ScriptEditor at the position of the text cursor. You can build whole new scripts on the basis of this mechanism.

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Scripting 3. Simple language elements", U"ppgb", 20130421)
INTRO (U"The Praat scripting language doesn't only call the menu commands "
	"discussed in the @@Scripting 1. Your first scripts|first@ and @@Scripting 2. How to script settings windows|second@ chapters of this tutorial, "
	"it is also a general procedural programming language that allows you to compute numbers, handle texts, and make custom analyses.")
NORMAL (U"This chapter focuses on the things you need most. It is designed in such a way that you can work through it even if you haven't written computer programs before.")
LIST_ITEM (U"@@Scripting 3.1. Hello world@ (writeInfoLine, appendInfoLine)")
LIST_ITEM (U"@@Scripting 3.2. Numeric variables@ (assignments)")
LIST_ITEM (U"@@Scripting 3.3. Numeric queries")
LIST_ITEM (U"@@Scripting 3.4. String variables@ (assignments)")
LIST_ITEM (U"@@Scripting 3.5. String queries")
LIST_ITEM (U"@@Scripting 3.6. “For” loops@ (for, endfor)")
LIST_ITEM (U"@@Scripting 3.7. Layout@ (white space, comments, continuation lines)")
MAN_END

#define Manual_DRAW_PICTURE_WINDOW(height,vpLeft,vpRight,vpTop,vpBottom) \
	Manual_DRAW_WINDOW (height, "Praat Picture", "File   Edit   Margins   World   Select   Pen   Font   Help") \
	"worldHeight = " #height " - 1\n" \
	"Select inner viewport: 0.2, 5.8, 0.8, 0.8+worldHeight\n" \
	"Axes: 0, 5.6, worldHeight, 0\n" \
	"vpLeft = " #vpLeft "\nvpRight = " #vpRight "\nvpTop = " #vpTop "\nvpBottom = " #vpBottom "\n" \
	"Paint rectangle: “Pink”, vpLeft, vpRight, vpTop, vpBottom\n" \
	"Paint rectangle: “White”, vpLeft+0.69, vpRight-0.69, vpTop+0.46, vpBottom-0.46\n" \
	"Yellow\n" \
	"Draw line: 3, 0, 3, worldHeight\n" \
	"for i to worldHeight/3\n" \
	"   Draw line: 0, i*3, 5.6, i*3\n" \
	"Red\n" \
	"for i to 5\n" \
	"   Text special: i, “centre”, 0, “top”, “Helvetica”, fontSize/1.2, “0”, string$(i)\n" \
	"endfor\n" \
	"for i to worldHeight\n" \
	"   Text special: 0, “left”, i, “half”, “Helvetica”, fontSize/1.2, “0”, string$(i)\n" \
	"endfor\n" \
	"Black\n" \
	"Draw line: 0, 0, 5.6, 0\n" \

MAN_BEGIN (U"Scripting 3.1. Hello world", U"ppgb", 20140111)
INTRO (U"Many manuals of computer programming languages start with their answer on the following question:")
NORMAL (U"%%How do I write the text “Hello world” on the screen?")
NORMAL (U"For the Praat scripting language, there are two answers.")
ENTRY (U"1. “Hello world” in the Info window")
NORMAL (U"The simplest answer is that you open the ScriptEditor window with ##New Praat script# from the #Praat menu, "
	"then type the following line into the ScriptEditor window:")
CODE (U"writeInfoLine: “Hello world”")
NORMAL (U"and finally choose #Run from the #Run menu.")
NORMAL (U"When you try this, the result should be that the Info window comes to the front, and that it shows the text `Hello world`:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{Hello world}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"Now suppose that you want to write two lines instead of just one, so you try a script with two lines:")
CODE (U"writeInfoLine: “Hello world”")
CODE (U"writeInfoLine: “How do you do?”")
NORMAL (U"This turns out not to do what you want: it seems to write only the text “How do you do?”. "
	"This happens because the #writeInfoLine function first erases the Info window, then writes the line of text. "
	"So the first line of the script did write the text “Hello world”, but the second line wiped it out "
	"and wrote “How do you do?” instead. The script that does what you want is")
CODE (U"writeInfoLine: “Hello world”")
CODE (U"appendInfoLine: “How do you do?”")
NORMAL (U"Now the result will be")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{Hello world}”\n"
	"Text: 0, “left”, 90, “half”, “\\s{How do you do?}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This works because @`appendInfoLine` writes a line without erasing the Info window first.")
NORMAL (U"Finally, try the following script:")
CODE (U"appendInfoLine: “Another try”")
CODE (U"appendInfoLine: “Goodbye”")
NORMAL (U"The result could be")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{Hello world}”\n"
	"Text: 0, “left”, 90, “half”, “\\s{How do you do?}”\n"
	"Text: 0, “left”, 105, “half”, “\\s{Another try}”\n"
	"Text: 0, “left”, 120, “half”, “\\s{Goodbye}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"In other words, @`appendInfoLine` writes lines into the Info window without erasing it, even if you run a script anew. "
	"This is why many Praat scripts that write into the Info window do a #writeInfoLine first, and follow it with a series of @`appendInfoLine` calls.")
NORMAL (U"For more information on these commands, see @@Scripting 6.2. Writing to the Info window@.")
ENTRY (U"2. “Hello world” in the Picture window.")
NORMAL (U"You can also show text in the Picture window. If you are an experienced Praat user, you have probably used the comamnd ##Text top...# before. "
	"You can use it do draw a text at the top of the current %viewport, which is the part of the Picture window where the next drawing will occur "
	"and which is marked by the pink %margins. Thus, when you select the top 4\\xx3 inches of the Picture window (with the mouse), "
	"set the font size to 12 (with the Pen menu), and run the script")
CODE (U"Text top: “yes”, “Hello world”")
NORMAL (U"then you’ll see")
SCRIPT (6, 4.5, U""
	Manual_DRAW_PICTURE_WINDOW (4.5, 0,4,0,3)
	"Select outer viewport: 0.2, 4.2, 0.8, 12\n" \
	"Times\n" \
	"Text top: “yes”, “Hello world”\n" \
	"Select inner viewport: 0.2, 5.8, 0.2, 4.3\n"\
	"Axes: 0, 1, 0, 1\n" \
	"Draw rectangle: 0, 1, 0, 1\n"
)
NORMAL (U"So this works the same as when you choose ##Text top...# from the #Margins menu by hand, with #Far switched on.")
NORMAL (U"If you want your script to always show the same text at the same position, with nothing else in the picture, "
	"then you can make your script a bit more extensive:")
CODE (U"Erase all")
CODE (U"Times")
CODE (U"Font size: 12")
CODE (U"Select outer viewport: 0, 4, 0, 3")
CODE (U"Text top: “yes”, “Hello world”")
NORMAL (U"In this script, line 1 erases the Picture window, so that nothing besides your text can appear in the Picture window.")
NORMAL (U"Line 2 executes the command #Times from the #Font menu, so that the script will always draw the text in Times, "
	"even if you choose #Helvetica in the Font menu with the mouse before you run the script "
	"(after the script has run, you’ll see that #Times is chosen in the Font menu).")
NORMAL (U"Line 3 executes the command ##Font size...# from the #Font menu, setting the font size to 12 and setting the width of the pink margins "
	"accordingly.")
NORMAL (U"Line 4 executes the command @@Select outer viewport...@ from the Select menu. "
	"This performs an action that you would normally do by dragging the mouse, "
	"namely selecting the part of the Picture window that runs from 0 to 4 inches horizontally "
	"and from 0 to 3 inches vertically. After running the script, "
	"the %viewport is indeed [0, 4] \\xx [0, 3], as you can clearly see "
	"from the pink margins above.")
NORMAL (U"Line 5 finally writes the text.")
NORMAL (U"For more information on these commands, see @@Picture window@.")
MAN_END

MAN_BEGIN (U"Scripting 3.2. Numeric variables", U"ppgb", 20230201)
INTRO (U"In any general procedural programming language you can work with %variables, "
	"which are places in your computer's memory where you can store a number or anything else.")
NORMAL (U"For instance, you could put the number 3.1 into the variable $%b in the following way:")
CODE (U"b = 3.1")
NORMAL (U"This statement is called as %assignment, i.e., you %assign the %value 3.1 to the %variable %`b`. "
	"We read this statement aloud as “%`b` becomes 3.1” (or “%`b` gets 3.1”, but not “%`b` is 3.1”). "
	"What this means is that after this statement, the memory location %`b` %contains the numeric value (number) 3.1.")
NORMAL (U"You can regard a variable as a box: you put the value 3.1 into the box named %`b`. "
	"Or you can regard a variable as a house: the house is called %`b` and now the family “3.1” is living there. "
	"Or you can regard it as any other storage location.")
NORMAL (U"To see what value a variable contains (what’s in the box, or who lives in the house), "
	"you can use the #`writeInfoLine` function:")
CODE (U"b = 3.1")
CODE (U"writeInfoLine: “The value is ”, b, “.”")
NORMAL (U"This will put the text “`The value is 3.1.`” into the Info window, as you are invited to verify.")
NORMAL (U"A variable is called a variable because it is %variable, i.e. its value can change. Try the script")
CODE (U"b = 3.1")
CODE (U"b = 5.8")
CODE (U"writeInfoLine: “The value is ”, b, “.”")
NORMAL (U"You will see that %`b` ends up having the value 5.8. The first line puts the value 3.1 there, but the second line "
	"replaces it with 5.8. It’s like taking the 3.1 out of the box and putting the 5.8 in its stead. "
	"Or the family 3.1 moves from the house, and the family called 5.8 moves in.")
NORMAL (U"In an assignment, the part to the right of the “becomes” sign (the “=” sign) doesn’t have to be a number; "
	"it can be any %formula that %evaluates to a number. For instance, the script")
CODE (U"b = 3.1 * 2")
CODE (U"writeInfoLine: “The value is ”, b, “.”")
NORMAL (U"puts the text “`The value is 6.2.`” into the Info window. This works because Praat handles the first line "
	"in the following way:")
LIST_ITEM (U"1. the formula `3.1 * 2` is %evaluated (i.e. its value is computed), and the result is 6.2.")
LIST_ITEM (U"2. the value 6.2 is subsequently stored in the variable `b`.")
NORMAL (U"After line 1 has been executed, the variable %`b` just contains the value 6.2, nothing more; "
	"the variable %`b` doesn’t remember that that value has been computed by multiplying 3.1 with 2.")
NORMAL (U"Formulas can contain more things than numbers: they can also contain other variables:")
CODE (U"b = 3.1")
CODE (U"c = b * 2")
CODE (U"writeInfoLine: “The value of b is ”, b, “, and the value of c is ”, c, “.”")
NORMAL (U"In the first line, %`b` gets the value 3.1. In the second line, the formula `b * 2` first has to be evaluated. "
	"Praat looks up the value of %`b` (which is 3.1), so that it knows that the formula actually means `3.1 * 2`. "
	"Praat evaluates this formula and stores the result (namely the value 6.2) "
	"into the variable %`c`, which will then contain nothing else than the value 6.2. "
	"The Info window thus reports “`The value of b is 3.1, and the value of c is 6.2.`”.")
NORMAL (U"After these explanations, consider the following script:")
CODE (U"b = 3.1")
CODE (U"c = b * 2")
CODE (U"b = 5.8")
CODE (U"writeInfoLine: “The value of c is ”, c, “.”")
NORMAL (U"Can you figure out what the Info will report? If you think it will report "
	"“`The value of c is 6.2.`”, then you are correct: after the first line, %`b` contains the value 3.1; "
	"after the second line, the value of %`c` is therefore 6.2, and nothing more; "
	"after line 3, the value of %`b` has changed to 5.8, but the value of %`c` hasn’t changed and is still 6.2.")
NORMAL (U"If you thought that %`c` would end up having the value 11.6, then you’re thinking in terms "
	"of a non-procedural language such as Prolog; you may have thought that the thing assigned to %`c` in the second line "
	"is the whole %formula `b * 2`, so that %`c` changes when %`b` changes. But this is not the case: "
	"the thing stored in %`c` is just the %value of the formula `b * 2` at that moment, which is 6.2, "
	"and %`c` doesn’t remember how it got that value. If you have trouble understanding this, "
	"consult anybody who writes programs.")
MAN_END

MAN_BEGIN (U"Scripting 3.3. Numeric queries", U"ppgb", 20221202)
INTRO (U"Now that you know how to script a menu command, and you know how variables work, "
	"you are ready to combine the two.")
NORMAL (U"Suppose you have selected a Sound in the object list. One of the commands available in the @@Query submenu@ "
	"is ##Get power...#. When you choose it, you will see the following settings window:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (1), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Get power", 1)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
)
NORMAL (U"When you click OK, something like the following will appear in the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{0.1350605005239421 Pa2}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This is the mean power of the whole Sound.")
NORMAL (U"In a script, you want to use the value of this power in the script itself, not in the Info window, "
	"perhaps because you want to do computations with it or because you want to report the value with a nice text around it. "
	"This is how you do the latter:")
CODE (U"power = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: “The power of this sound is ”, power, “ Pascal-squared.”")
NORMAL (U"The first line of this script executes the menu command ##Get power...#, "
	"but puts the value 0.1350605005239421 into the variable %`power` instead of into the Info window "
	"(the variable can have any name you like, as long as it starts with a lower-case letter "
	"and consists of letters and digits; see @@Scripting 5.1. Variables@).")
NORMAL (U"The second line then reports the value in the Info window, this time with a nice text around it:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{The power of this sound is 0.1350605005239421 Pascal-squared.}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
MAN_END

MAN_BEGIN (U"Scripting 3.4. String variables", U"ppgb", 20130411)
INTRO (U"Just as you can store @@Scripting 3.2. Numeric variables|numeric variables@, "
	"you can store %%string variables%, which contain text instead of numbers. Here is an example:")
CODE (U"word1$ = “Hello”")
CODE (U"word2$ = “world”")
CODE (U"sentence$ = word1$ + “ ” + word2$")
CODE (U"writeInfoLine: “The whole sentence is: ”, sentence$")
NORMAL (U"Yes, this is another way to get the sentence `Hello world` into the Info window. "
	"It's a more linguistically valid way to do it, and here is how it works:")
LIST_ITEM (U"1. In line 1, the value “Hello”, which is a text (as we can see by its use of quotes), "
	"is stored into the variable %`word1$`, which is a string variable (as we can see because its name ends in a dollar sign).")
LIST_ITEM (U"2. In line 2, the text value “world” is stored into the string variable %`word2$`.")
LIST_ITEM (U"3. In line 3, we have the formula `word1$ + “ ” + word2$`, which contains two variables, "
	"namely %`word1$` and %`word2$`.")
LIST_ITEM (U"4. The values of the two variables are “Hello” and “world”, respectively, "
	"so what the formula actually says is “Hello” + “ ” + “world”.")
LIST_ITEM (U"5. The pluses in the formula mean “concatenate”, so we concatenate the three strings "
	"“Hello”, “ ”, and “world”, giving the longer string “Hello world”.")
LIST_ITEM (U"6. Still in line 3, the string value “Hello world” is assigned to the string variable %`sentence$`.")
LIST_ITEM (U"7. Line 4 reports in the Info window: `The whole sentence is: Hello world`")
MAN_END

MAN_BEGIN (U"Scripting 3.5. String queries", U"ppgb", 20221202)
INTRO (U"Just as you can use menu commands (usually in a @@Query submenu@) to query @@Scripting 3.3. Numeric queries|numbers@, "
	"you can query texts as well.")
NORMAL (U"For instance, when you select a Textgrid, the @@Query submenu@ will contain the command ##Get label of interval...#, "
	"which takes two numeric arguments, namely ##Tier number# and ##Interval number#:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (2), U""
	Manual_DRAW_SETTINGS_WINDOW ("TextGrid: Get label of interval", 2)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Tier number", "1")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Interval number", "3")
)
NORMAL (U"When you click OK, and interval 3 of tier 1 happens to contain the text `hello`, the following appears in the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{hello}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"In a script, you will want to put the result of the query in a string variable instead of in the Info window, "
	"because you want to manipulate it further:")
CODE (U"text$ = Get label of interval: 1, 3")
CODE (U"writeInfoLine: “The text in interval 3 of tier 1 is: ”, text$")
NORMAL (U"The script first stores the text of the interval, i.e. `hello`, into the variable %`text$`, "
	"then writes it, preceded by some informative text, into the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{The text in interval 3 of tier 1 is: hello}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"Hey, yet another way to implement “Hello world” with the Praat scripting language!")
ENTRY (U"The difference between numeric queries and string queries")
NORMAL (U"A string query stores in a string variable the whole text that would appear in the Info window. "
	"For instance, the script")
CODE (U"power$ = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: power$")
NORMAL (U"could give you the following result:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{0.1350605005239421 Pa2}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"A numeric query stores in a numeric variable only the first number that it can find in the text that would appear in the Info window. "
	"For instance, the script")
CODE (U"power = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: power")
NORMAL (U"could give you the following result:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{0.1350605005239421}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
MAN_END

MAN_BEGIN (U"Scripting 3.6. “For” loops", U"ppgb", 20140111)
INTRO (U"The power of a procedural programming language is most easily illustrated with the %%for-loop%.")
NORMAL (U"Take the example of the @@Scripting 3.5. String queries|previous page@, "
	"whereas you wanted to know the text in the third interval of the first tier of a selected TextGrid. "
	"It’s easy to imagine that you actually want the texts of %%all the first five% intervals. "
	"With knowledge from the previous sections, you could write it like this:")
CODE (U"writeInfoLine: “The texts in the first five intervals:”")
CODE (U"text$ = Get label of interval: 1, 1")
CODE (U"appendInfoLine: “Interval 1: ”, text$")
CODE (U"text$ = Get label of interval: 1, 2")
CODE (U"appendInfoLine: “Interval 2: ”, text$")
CODE (U"text$ = Get label of interval: 1, 3")
CODE (U"appendInfoLine: “Interval 3: ”, text$")
CODE (U"text$ = Get label of interval: 1, 4")
CODE (U"appendInfoLine: “Interval 4: ”, text$")
CODE (U"text$ = Get label of interval: 1, 5")
CODE (U"appendInfoLine: “Interval 5: ”, text$")
NORMAL (U"The result will be something like")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{The texts in the first five intervals:}”\n"
	"Text: 0, “left”, 90, “half”, “\\s{Interval 1: I}”\n"
	"Text: 0, “left”, 105, “half”, “\\s{Interval 2: say}”\n"
	"Text: 0, “left”, 120, “half”, “\\s{Interval 3: hello}”\n"
	"Text: 0, “left”, 135, “half”, “\\s{Interval 4: and}”\n"
	"Text: 0, “left”, 150, “half”, “\\s{Interval 5: you}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This can be done more nicely. The first step is to realize that the sentences starting with `text$` are similar to each other, "
	"and the sentence starting with `appendInfoLine` are also similar to each other. They only differ in the interval number, "
	"and can therefore be made %identical by using a variable for the interval number, like this:")
CODE (U"writeInfoLine: “The texts in the first five intervals:”")
CODE (U"intervalNumber = 1")
CODE (U"text$ = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"intervalNumber = 2")
CODE (U"text$ = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"intervalNumber = 3")
CODE (U"text$ = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"intervalNumber = 4")
CODE (U"text$ = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"intervalNumber = 5")
CODE (U"text$ = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
NORMAL (U"A new trick that you see here is that as a numeric argument (##Interval number#, "
	"the second argument to ##Get label of interval...#), you can use not only a number "
	"(as in all previous examples), but also a variable (%`intervalNumber`). "
	"The rest of the script should be known stuff by now.")
NORMAL (U"The script above is long, but it can be made much shorter with the use of a %%for-loop%:")
CODE (U"writeInfoLine: “The texts in the first five intervals:”")
CODE (U"for intervalNumber from 1 to 5")
	CODE1 (U"text$ = Get label of interval: 1, intervalNumber")
	CODE1 (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"endfor")
NORMAL (U"The two lines that were repeated five times in the previous version now show up with indentation "
	"between a `for` line and its corresponding `endfor`. Those two lines (the `text$` and the `appendInfoLine` line) "
	"are executed five times: for %`intervalNumber` equal to 1, for %`intervalNumber` equal to 2, for %`intervalNumber` equal to 3, "
	"for %`intervalNumber` equal to 4, and for %`intervalNumber` equal to 5, in that order.")
NORMAL (U"In the above example, using a loop does not do much more than save eight lines, at the cost of adding two new lines. "
	"But imagine the case in which you want to list %all the texts in the intervals: "
	"the version without the loop is no longer possible. By contrast, the version %with the loop is still possible, "
	"because we have the command ##Get number of intervals...#, which gives us the number of intervals in the specified tier "
	"(here, tier 1). So you do:")
CODE (U"numberOfIntervals = Get number of intervals: 1")
CODE (U"writeInfoLine: “The texts in all ”, numberOfIntervals, “ intervals:”")
CODE (U"for intervalNumber from 1 to numberOfIntervals")
	CODE1 (U"text$ = Get label of interval: 1, intervalNumber")
	CODE1 (U"appendInfoLine: “Interval ”, intervalNumber, “: ”, text$")
CODE (U"endfor")
NORMAL (U"This may yield something like")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, “left”, 75, “half”, “\\s{The texts in all 7 intervals:}”\n"
	"Text: 0, “left”, 90, “half”, “\\s{Interval 1: I}”\n"
	"Text: 0, “left”, 105, “half”, “\\s{Interval 2: say}”\n"
	"Text: 0, “left”, 120, “half”, “\\s{Interval 3: hello}”\n"
	"Text: 0, “left”, 135, “half”, “\\s{Interval 4: and}”\n"
	"Text: 0, “left”, 150, “half”, “\\s{Interval 5: you}”\n"
	"Text: 0, “left”, 165, “half”, “\\s{Interval 6: say}”\n"
	"Text: 0, “left”, 180, “half”, “\\s{Interval 7: goodbye}”\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This is the first script in this tutorial that is useful in itself. On the basis of it "
	"you can create all kinds of ways to list the texts in intervals. Here is how you would also list the durations "
	"of those intervals:")
CODE (U"numberOfIntervals = Get number of intervals: 1")
CODE (U"writeInfoLine: “The durations and texts in all ”, numberOfIntervals, “ intervals:”")
CODE (U"for intervalNumber from 1 to numberOfIntervals")
	CODE1 (U"startTime = Get start point: 1, intervalNumber")
	CODE1 (U"endTime = Get end point: 1, intervalNumber")
	CODE1 (U"duration = endTime - startTime")
	CODE1 (U"text$ = Get label of interval: 1, intervalNumber")
	CODE1 (U"appendInfoLine: “Interval ”, intervalNumber, “ is ”, "
			"duration, “ seconds long and contains the text: ”, text$")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Scripting 3.7. Layout", U"ppgb", 20170904)
INTRO (U"This chapter handles the way you use white space, comments, and continuation lines in a Praat script.")
ENTRY (U"White space")
NORMAL (U"Praat ignores all white space (spaces and tabs) that you put at the beginning of lines. The indentation "
	"that you saw on the @@Scripting 3.6. “For” loops|previous page@ was therefore used solely for readability. "
	"You are advised to use indenting, though, with three or four spaces for each level, "
	"as in the following example, which loops over all tiers and intervals of a TextGrid:")
CODE (U"writeInfoLine: “The texts in all tiers and intervals:”")
CODE (U"numberOfTiers = Get number of tiers")
CODE (U"for tierNumber from 1 to numberOfTiers")
	CODE1 (U"numberOfIntervals = Get number of intervals: tierNumber")
	CODE1 (U"for intervalNumber from 1 to numberOfIntervals")
		CODE2 (U"text$ = Get label of interval: tierNumber, intervalNumber")
		CODE2 (U"appendInfoLine: “Tier ”, tierNumber, “, interval ”, intervalNumber, “: ”, text$")
	CODE1 (U"endfor")
CODE (U"endfor")
NORMAL (U"Praat also ignores lines that are empty or consist solely of white space, "
	"so you use those to structure your script visually.")
ENTRY (U"Comments")
NORMAL (U"Comments are lines that start with “`#`” or “`;`”. Praat ignores these lines when your script is running:")
CODE (U"# Create 1 second of a sine wave with a frequency of 100 Hertz,")
CODE (U"# sampled at 44100 Hz:")
CODE (U"Create Sound from formula: “sine”, 1, 0, 1, 44100, ~ sin (2*pi*100*x)")
NORMAL (U"Because of its visibility, you are advised to use “`#`” for comments that structure your script, "
	"and “;” perhaps only for “commenting out” a statement, i.e. to temporarily put it before a line "
	"that you don’t want to execute.")
ENTRY (U"Continuation lines")
NORMAL (U"There is normally one line per statement, and one statement per line. But some statements are very long, "
	"such as this one on a previous page:")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “ is ”, duration, “ seconds long and contains the text: ”, text$")
NORMAL (U"By making the current window wider, you can see that I really put this whole statement on a single line. "
	"I could have distributed it over two lines in the following way, by using three dots (an %ellipsis):")
CODE (U"appendInfoLine: “Interval ”, intervalNumber, “ is ”, duration, “ seconds long")
CODE (U"... and contains the text: ”, text$")
NORMAL (U"Here is another common type of example:")
CODE (U"Create Sound from formula: “windowedSine”, 1, 0, 1, 44100,")
CODE (U"... ~ 0.5 * sin(2*pi*1000*x) * exp(-0.5*((x-0.5)/0.1)^2)")
NORMAL (U"You will normally want to follow such an ellipsis with a space, unless you want to concatenate "
	"the parts of a long word:")
CODE (U"Select outer viewport: 0, 10, 0, 4")
CODE (U"Text top: “yes”, “It’s a long way to Llanfairpwllgwyngyll")
CODE (U"...gogerychwyrndrobwllllantysiliogogogoch,")
CODE (U"... unless you start from Tyddyn-y-felin.”")
MAN_END

MAN_BEGIN (U"Scripting 4. Object selection", U"ppgb", 20130501)
INTRO (U"This chapter is about how to select objects from your script, "
	"and how to find out what objects are currently selected.")
LIST_ITEM (U"@@Scripting 4.1. Selecting objects")
LIST_ITEM (U"@@Scripting 4.2. Removing objects")
LIST_ITEM (U"@@Scripting 4.3. Querying objects")
MAN_END

MAN_BEGIN (U"Scripting 4.1. Selecting objects", U"ppgb", 20180428)
NORMAL (U"To simulate the mouse-clicked and dragged selection in the list of objects, "
	"you have the functions @`selectObject`, @`plusObject` and @`minusObject`.")
NORMAL (U"Suppose you start Praat and use ##Create Sound as tone...# to create a Sound called %tone. "
	"In the object list it looks like “1. Sound tone”. "
	"Suppose you then do ##To Spectrum...# from the ##Analyse Spectrum# menu. "
	"A second object, called “2. Spectrum tone” appears in the list and is selected. "
	"To select and play the Sound, you can do either")
CODE (U"\\#`{selectObject}: 1")
CODE (U"Play")
NORMAL (U"or")
CODE (U"\\#`{selectObject}: “Sound tone”")
CODE (U"Play")
NORMAL (U"So you can select an object either by its unique ID (identifier: the unique number by which it appears in the list) "
	"or by name.")
NORMAL (U"The function @`selectObject` works by first deselecting all objects, and then selecting the one you mention. "
	"If you don’t want to deselect the existing selection, you can use @`plusObject` or @`minusObject`. "
	"When the Sound is selected, you can select the Spectrum as well by doing")
CODE (U"\\#`{plusObject}: 2")
NORMAL (U"or")
CODE (U"\\#`{plusObject}: “Spectrum tone”")
NORMAL (U"If you then want to deselect the Sound, and keep the Spectrum selected, you can do")
CODE (U"\\#`{minusObject}: 1")
NORMAL (U"or")
CODE (U"\\#`{minusObject}: “Sound tone”")
NORMAL (U"All these functions can take more than one argument. To select the Sound and the Spectrum together, you can do")
CODE (U"\\#`{selectObject}: 1, 2")
NORMAL (U"or")
CODE (U"\\#`{selectObject}: “Sound tone”, “Spectrum tone”")
NORMAL (U"or even")
CODE (U"\\#`{selectObject}: 1, “Spectrum tone”")
NORMAL (U"or, using a numeric vector:")
CODE (U"myObjects# = { 1, 2 }")
CODE (U"\\#`{selectObject}: myObjects#")
ENTRY (U"How to refer to objects created in your script")
NORMAL (U"In a script, you typically don't know whether the IDs of the objects are 1 and 2, or much higher numbers. "
	"Fortunately, commands that create a new object give you the ID of the object that is created, "
	"so that you can refer to the object later on. For instance, suppose you want to generate a sine wave, play it, "
	"draw its spectrum, and then throw away both the Sound and the Spectrum. Here is how you do it:")
CODE (U"sound = Create Sound as pure tone: “sine377”,")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"To Spectrum: “yes”")
CODE (U"Draw: 0, 5000, 20, 80, “yes”   ; the Spectrum is selected, so it is drawn")
CODE (U"# Remove the created Spectrum and Sound:")
CODE (U"\\#`{plusObject}: sound   ; the Spectrum was already selected")
CODE (U"Remove")
NORMAL (U"You could also select the objects by name:")
CODE (U"Create Sound as pure tone: “sine377”,")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; no need to remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"To Spectrum: “yes”")
CODE (U"Draw: 0, 5000, 20, 80, “yes”   ; the Spectrum is selected, so it is drawn")
CODE (U"# Remove the created Spectrum and Sound:")
CODE (U"\\#`{plusObject}: “Sound sine377”   ; the Spectrum was already selected")
CODE (U"Remove")
NORMAL (U"This works even if there are multiple objects called “Sound sine377”, "
	"because if there are more objects with the same name, @`selectObject` and @`plusObject` select the most recently created one, "
	"i.e., the one nearest to the bottom of the list of objects.")
MAN_END


MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Scripting 4.2. Removing objects"
© Paul Boersma 1999,2004,2006–2008,2011,2013,2014

In @@Scripting 4.1. Selecting objects|\SS4.1@ we saw that objects could be removed by
selecting them first and then calling the #Remove command.
A faster way is the #removeObject function, which can also remove unselected objects:
{;
	sound = Create Sound as pure tone: “sine377”,
	... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound
	Play   ; the Sound is selected, so it plays
	spectrum = To Spectrum: “yes”   ; remember the ID of the Spectrum
	Draw: 0, 5000, 20, 80, “yes”   ; the Spectrum is selected, so it is drawn
	# Remove the created Spectrum and Sound:
	\#`{removeObject}: sound, spectrum   ; remove one selected and one unselected object
}
The #removeObject function keeps the objects selected that were selected before
(except of course the ones it throws away).
This allows you to easily throw away objects as soon as you no longer need them:
{;
	sound = Create Sound as pure tone: “sine377”,
	... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound
	Play   ; the Sound is selected, so it plays
	spectrum = To Spectrum: “yes”
	\#`{removeObject}: sound   ; we no longer need the Sound, so we remove it
	Draw: 0, 5000, 20, 80, “yes”   ; the Spectrum is still selected, so it is drawn
	\#`{removeObject}: spectrum   ; remove the last object created by the script
}
Selecting and removing all objects from the list (don’t)
========================================================
A very strange command, which you should not normally use, is `select all`:
{;
	\#{select all}
	Remove
}
This selects all objects in the list and then removes them.
Please try not to use this, because it will remove even the objects that your script did not create!
After all, you don’t want the users of your script to lose the objects they created!
So please try to remove in your script only the objects that your script created,
even if the script is for your own use (because if it is a nice script, others will want to use it).

################################################################################
"Scripting 4.3. Querying objects"
© Paul Boersma 1999,2004,2006–2008,2011,2013,2014,2018,2023

You can get the name of a selected object into a string variable.
For instance, the following reads the name of the second selected Sound
(as counted from the top of the list of objects) into the variable %`name$`:
{;
	name$ = \#`{selected$} (“Sound”, 2)
}
If the Sound was called “Sound hallo”, the variable %`name$` will contain the string “hallo”.
To get the name of the topmost selected Sound object, you can leave out the number:
{;
	name$ = selected$ (“Sound”)
}
To get the full name (type + name) of the third selected object, you do:
{;
	fullName$ = selected$ (3)
}
To get the full name of the topmost selected object, you do:
{;
	fullName$ = selected$ ()
}
To get the type and name out of the full name, you do:
{;
	type$ = \`{extractWord$} (fullName$, “”)
	name$ = \`{extractLine$} (fullName$, “ ”)
}
Negative numbers count from the bottom. Thus, to get the name of the bottom-most selected Sound
object, you say
{;
	name$ = selected$ (“Sound”, -1)
}
You would use `selected$` () for drawing the object name in a picture:
{;
	Draw: 0, 0, 0, 0, “yes”
	name$ = selected$ (“Sound”)
	Text top: “no”, “This is sound ” + name$
}
For identifying previously selected objects, this method is not very suitable, since
there may be multiple objects with the same name:
{;
	# The following two lines are OK:
	soundName$ = selected$ (“Sound”, -1)
	pitchName$ = selected$ (“Pitch”)
	# But the following line is questionable, since it doesn’t
	# necessarily select the previously selected Pitch again:
	selectObject: “Pitch ” + pitchName$
}
Instead of this error-prone approach, you should get the object’s unique ID.
The correct version of our example becomes:
{;
	sound = selected (“Sound”, -1)
	pitch = selected (“Pitch”)
	# Correct:
	selectObject: pitch
}
To get the number of selected Sound objects into a variable, use
{;
	numberOfSelectedSounds = numberOfSelected (“Sound”)
}
To get the number of selected objects into a variable, use
{;
	numberOfSelectedObjects = numberOfSelected ()
}
Example: doing something to every selected Sound
================================================
{;
	sounds# = \#`{selected#} (“Sound”)
	# Median pitches of all selected sounds:
	for i to size (sounds#)
		selectObject: sounds# [i]
		To Pitch (filtered ac): 0.0, 50, 800, 15, “no”, 0.03, 0.09, 0.50, 0.055, 0.35, 0.14
		f0 = Get quantile: 0, 0, 0.50, “Hertz”
		appendInfoLine: f0
		Remove
	endfor
	# Restore selection:
	selectObject (sounds#)
}

################################################################################
"Scripting 5. Language elements reference"
© Paul Boersma 2017-07-18

In a Praat script, you can use variables, expressions, and functions, of numeric as well as string type,
and most of the control structures known from other procedural computer languages.
The way the distinction between numbers and strings is made, may remind you of the programming language Basic.

,	@@Scripting 5.1. Variables@ (numeric, string)
,	@@Scripting 5.2. Expressions@ (numeric, string)
,	@@Scripting 5.3. Jumps@ (if, then, elsif, else, endif)
,	@@Scripting 5.4. Loops@ (for/endfor, while/endwhile, repeat/until)
,	@@Scripting 5.5. Procedures@ (\@ , procedure)
,	@@Scripting 5.6. Arrays and dictionaries@
,	@@Scripting 5.7. Vectors and matrices
,	@@Scripting 5.8. Including other scripts@
,	@@Scripting 5.9. Quitting@ (exit)

################################################################################
"Scripting 5.1. Variables"
© Paul Boersma 2020-12-29,2024,2025

A %variable is a location in your computer’s memory that has a name and where you can store something,
as explained in @@Scripting 3.2. Numeric variables|\\SS3.2@ and @@Scripting 3.4. String variables|\\SS3.4@.
In a Praat script, you can store numbers and texts, i.e. you can use %%numeric variables% and %%string variables%.

Numeric variables
=================

Numeric variables can hold integer numbers between -1,000,000,000,000,000 and +1,000,000,000,000,000
or real numbers between -10^^308^ and +10^^308^. The smallest numbers lie near -10^^-308^ and +10^^-308^.

You use numeric variables in your script like this:
{ 4x2
	length = 10
	Axes: 0, 100, 0, 100
	Draw line: 0, 50, length, 50
	Draw inner box
}
This draws a line in the Picture window from position (0, 50) to position (%`length`, 50).
In the first line, you assign the value 10 to the variable called %`length`,
and in the third line you use the value of %`length` as the third argument to the command ##Draw line...#.

Names of numeric variables must start with a lower-case letter, optionally followed by a sequence
of letters, digits, and underscores.

String variables
================

You use string variables, which contain text, as follows:
{ 4x2
	title$ = “Dutch nasal place assimilation”
	Text top: “no”, title$
	Draw inner box
}
This writes the text “Dutch nasal place assimilation” above your drawing.

As in the programming language Basic, the names of string variables end in a dollar sign.

Making numeric variables visible
================================

You can write the content of numeric variables directly to the info window:
{;
	x = 2.0
	root = sqrt (x)
	\#`{writeInfoLine}: “The square root of ”, x, “ is ”, root, “.”
}
When you try this out, this will write the following text to the Info window:
`
	The square root of 2 is 1.4142135623730951.
`
{-;
	assert string$ (sqrt (2)) = "1.4142135623730951"
}

You can fix the number of digits after the decimal point by use of the @`fixed$` function:
{
	x = 2.0
	root = sqrt (x)
	\`{writeInfoLine}: “The square root of ”, \#`{fixed$} (x, 3), “ is approximately ”, \#`{fixed$} (root, 3), “.”
}
{-;
	assert fixed$ (sqrt (2), 3) = "1.414"
}

By using 0 decimal digits, you round to whole values:
{
	root = sqrt (2)
	\`{writeInfoLine}: “The square root of 2 is very approximately ”, fixed$ (root, 0), “.”
}
{-;
	assert fixed$ (sqrt (2), 0) = "1"
}

By using the `percent$` function, you give the result in a percent format:
{
	jitter = 0.0156789
	\`{writeInfoLine}: “The jitter is ”, \#`{percent$} (jitter, 3), “.”
}
{-;
	jitter = 0.0156789
	assert percent$ (jitter, 3) = "1.568%"
	jitter = -0.0156789
	assert percent$ (jitter, 3) = "-1.568%"
}

The number 0, however, will always be written as 0, and for small numbers the number of
significant digits will never be less than 1:
{
	jitter = 0.000000156789
	\`{writeInfoLine}: “The jitter is ”, percent$ (jitter, 3), “.”
}
{-;
	assert percent$ (0, 3) = "0"
	jitter = 0.000000156789
	assert percent$ (jitter, 3) = "0.00002%"
	jitter *= -1
	assert percent$ (jitter, 3) = "-0.00002%"
}

Predefined variables
====================

All of the variables you saw earlier in this tutorial were defined at the first moment a value was assigned to them.
Some variables, however, are already defined implicitly at the start of your script.

Some predefined ##numeric variables# are `macintosh`, `windows`, and `unix`, which are 1 if your edition of Praat
was built for the Macintosh, Windows, or Unix platform (respectively), and which are otherwise zero.
Likewise, we have `praat_32bit` and `praat_64bit`, of which one is 1 and the other 0,
depending on whether your edition of Praat was built for 32-bit or 64-bit computers.
More precisely, we have `praat_intel32`, `praat_intel64`, `praat_arm64`, `praat_s390x`, and `praat_armv7`,
one of which is 1 (and the others 0)
depending on whether the type of processor chip that your edition of Praat was made for is
32-bit Intel (= x86, i386, i686), or 64-bit Intel (= AMD64), or ARM64 (= Aarch64),
or s390x (LinuxONE), or ARMv7 (smaller Raspberry Pi).
Another predefined numeric variable is `praatVersion`, which is something like 6443.

Some ##predefined string variables# are `newline$`, `tab$`, and `shellDirectory$`.
The last one specifies the folder that was the default folder when Praat started up;
you can use it in scripts that run from the Unix or Windows command line.
Likewise, there exist the predefined string variables `homeDirectory$`,
`preferencesDirectory$`, and `temporaryDirectory$`. These three refer to your home folder
(which is where you log in), the Praat @@preferences folder@, and a folder for saving temporary files;
if you want to know what they are on your computer, try to write them into a script window.
The variable `defaultDirectory$` is available for formulas in scripts; it is the folder that contains the script file.
Finally, we have `praatVersion$`, which is something like “6.4.43”."

A disadvantage of predefined variables is that they can be changed, in which case they take on a different meaning.
For this reason you are advised to use functions instead (if they exist), because these always have the same meaning:
- @`appVersion`() instead of `praatVersion`
- @`appVersion$`() instead of `praatVersion$`

Functions that handle variables
===============================

To check whether a variable exists, you can use the function
{;
	variableExists (variableName$)
}
as in
{
	fgh = 567
	assert variableExists ("fgh")
	assert not variableExists ("jhfwbfejfgcds")
}

################################################################################
)~~~"
MAN_PAGES_END

/*
form Convert from WAV to AIFF
   text fileName hello.wav
endform
fileName$ = fileName$ - ".wav"
Read from file: shellDirectory$ + \"/\" + fileName$ + \".wav\"
Save as AIFF file: shellDirectory$ + \"/\" + fileName$ + \".aiff\"

if left$ (fileName$) <> "/"
   fileName$ = shellDirectory$ + \"/\" + fileName$
endif
*/

MAN_BEGIN (U"Scripting 5.2. Expressions", U"ppgb", 20180721)
INTRO (U"In a Praat script, you can use numeric expressions as well as string expressions.")
ENTRY (U"Numeric expressions")
NORMAL (U"You can use a large variety of @@Formulas@ in your script:")
CODE (U"length = 10")
CODE (U"height = length/2")
CODE (U"area = length * height")
CODE (U"writeInfoLine: “The area is ”, area, “.”")
NORMAL (U"You can use numeric variables and formulas in numeric arguments to commands:")
CODE (U"Draw line: 0, 0, length / 2, 2 * height")
NORMAL (U"You can use numeric expressions in assignments (as above), or after "
	"#`if`, #`elsif`, #`while`, #`until`, and twice after #`for`.")
ENTRY (U"String expressions")
NORMAL (U"You can use a large variety of @@Formulas@ in your script:")
CODE (U"addressee$ = “Silke”")
CODE (U"greeting$ = “Hi ” + addressee$ + “!”")
CODE (U"writeInfoLine: “The greeting is: ”, greeting$")
NORMAL (U"You can use string variables and formulas in numeric arguments to commands:")
CODE (U"Draw line: 0, length (greeting$), 0, 100")
CODE (U"Draw line: 0, if answer$ = “yes” then 20 else 30 fi, 0, 100")
NORMAL (U"You can use numeric and string variables and formulas in string arguments to commands:")
CODE (U"Text top: “yes”, “Hi ” + addressee$ + “!”")
CODE (U"Text top: “yes”, left$ (fileName$, index (fileName$, “.”) - 1)")
ENTRY (U"Assignments from query commands")
NORMAL (U"On how to get information from commands that normally write to the Info window, "
	"see @@Scripting 6.3. Query commands@.")
MAN_END

MAN_BEGIN (U"Scripting 5.3. Jumps", U"ppgb", 19991112)
NORMAL (U"You can use conditional jumps in your script:")
TERM (U"#if %expression")
TERM (U"#elsif %expression")
DEFINITION (U"if the expression evaluates to zero or %false, the execution of the script jumps to the next "
	"#`elsif` or after the next #`else` or #`endif` at the same depth.")
NORMAL (U"The following script computes the preferred length of a bed for a person `age` years of age:")
CODE (U"if age <= 3")
	CODE1 (U"length = 1.20")
CODE (U"elsif age <= 8")
	CODE1 (U"length = 1.60")
CODE (U"else")
	CODE1 (U"length = 2.00")
CODE (U"endif")
NORMAL (U"A variant spelling for #`elsif` is #`elif`.")
MAN_END

MAN_BEGIN (U"Scripting 5.4. Loops", U"ppgb", 20140111)
ENTRY (U"\"For\" loops")
TERM (U"#for %variable #from %expression__1_ #to %expression__2_")
TERM (U"#for %variable #to %expression")
DEFINITION (U"the statements between the #for line and the matching #endfor will be executed "
	"while a variable takes on values between two expressions, with an increment (raise) of 1 "
	"on each turn of the loop. If there is no #from, the loop variable starts at 1.")
NORMAL (U"The following script plays nine sine waves, with frequencies of 200, 300, ..., 1000 Hz:")
CODE (U"\\#{for} i \\#{from} 2 \\#{to} 10")
	CODE1 (U"Create Sound as pure tone: \"tone\", 1, 0, 0.3, 44100, i * 100, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"\\#{endfor}")
NORMAL (U"The stop value of the #for loop is evaluated on each turn. If the second expression "
	"is already less than the first expression to begin with, the statements between #for and #endfor "
	"are not executed even once.")
ENTRY (U"“Repeat” loops")
TERM (U"#until %expression")
DEFINITION (U"the statements between the matching preceding #repeat and the #until line "
	"will be executed again if the expression evaluates to zero or %false.")
NORMAL (U"The following script measures the number of trials it takes me to throw 12 with two dice:")
CODE (U"throws = 0")
CODE (U"\\#{repeat}")
	CODE1 (U"eyes = randomInteger (1, 6) + randomInteger (1, 6)")
	CODE1 (U"throws = throws + 1")
CODE (U"\\#{until} eyes = 12")
CODE (U"writeInfoLine: \"It took me \", throws, \" trials to throw 12 with two dice.\"")
NORMAL (U"The statements in the #repeat/#until loop are executed at least once.")
ENTRY (U"\"While\" loops")
TERM (U"#while %expression")
DEFINITION (U"if the expression evaluates to zero or %false, the execution of the script jumps "
	"after the matching #endwhile.")
TERM (U"#endwhile")
DEFINITION (U"execution jumps back to the matching preceding #while line, which is then evaluated again.")
NORMAL (U"The following script forces the number %x into the range [0; 2\\pi):")
CODE (U"\\#{while} x < 0")
	CODE1 (U"x = x + 2 * pi")
CODE (U"\\#{endwhile}")
CODE (U"\\#{while} x >= 2 * pi")
	CODE1 (U"x = x - 2 * pi")
CODE (U"\\#{endwhile}")
NORMAL (U"If the expression evaluates to zero or %false to begin with, the statements between #while and #endwhile "
	"are not executed even once.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Scripting 5.5. Procedures"
© Paul Boersma 2017-09-04,2025

Sometimes in a Praat script, you will want to perform the same thing more than once.
In @@Scripting 5.4. Loops|\SS5.4@ we saw how %loops can help there.
In this section we will see how %procedures (also called %subroutines) can help us.

Imagine that you want to play a musical note with a frequency of 440 Hz (an “A”)
followed by a note that is one octave higher, i.e. has a frequency of 880 Hz (an “a”).
You could achieve this with the following script:
{;
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 440, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 880, 0.2, 0.01, 0.01
	Play
	Remove
}
This script creates a sound with a sine wave with an amplitude of 0.4 and a frequency of 440 Hz,
then plays this sound, then changes the sound into a sine wave with a frequency of 880 Hz,
then plays this changed sound, and then removes the Sound object from the object list.

This script is perfect if all you want to do is to play those two notes and nothing more.
But now imagine that you want to play such an octave jump not only for a note of 440 Hz,
but also for a note of 400 Hz and for a note of 500 Hz. You could use the following script:
{;
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 440, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 880, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 400, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 800, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 500, 0.2, 0.01, 0.01
	Play
	Remove
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, 1000, 0.2, 0.01, 0.01
	Play
	Remove
}
This script works but is no longer perfect. It contains many similar lines, and is difficult to read.

Here is where %procedures come in handy. With procedures, you can re-use similar pieces of code.
To make the three parts of the above script more similar, I’ll rewrite it using two variables
(%`frequency` and %`octaveHigher`):
{;
	frequency = 440
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
	Play
	Remove
	octaveHigher = 2 * frequency
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
	Play
	Remove
	frequency = 400
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
	Play
	Remove
	octaveHigher = 2 * frequency
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
	Play
	Remove
	frequency = 500
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
	Play
	Remove
	octaveHigher = 2 * frequency
	Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
	Play
	Remove
}
You can now see that seven lines of the script appear identically three times.
I'll put those seven lines into a %procedure that I name #`playOctave`:
{;
	\#{procedure} playOctave
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
		Play
		Remove
		octaveHigher = 2 * frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc}
}
As you see, a %%procedure definition% in Praat consists of three parts:

1. a line with the word #`procedure`, followed by the name of the procedure, followed by a pair of parentheses;
2. the %body of the procedure (here: seven lines);
3. a line with the word #`endproc`.

You can put a procedure definition anywhere in your script;
the beginning or end of the script are common places.

The bodies of procedures are executed only if you %call the procedure explicitly,
which you can do anywhere in the rest of your script:
{;
	frequency = 440
	@playOctave
	frequency = 400
	@playOctave
	frequency = 500
	@playOctave
	\#{procedure} playOctave
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
		Play
		Remove
		octaveHigher = 2 * frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc}
}
This script works as follows. First, the number 440 is assigned to the variable %`frequency` in line 1.
Then, execution of the script arrives at the #`@` (“call”) statement of line 2.
Praat then knows that it has to jump to the procedure called %`playOctave`,
which is found on line 7. The execution of the script then proceeds with the first line of the procedure body,
where a Sound is created. Then, the other lines of the procedure body are also executed, ending with the removal of the Sound.
Then, the execution of the script arrives at the #`endproc` statement. Here, Praat knows that it has to jump back
to the line after the line that the procedure was called from; since the procedure was called from line 2,
the execution proceeds at line 3 of the script. There, the number 400 is assigned to the variable %`frequency`.
In line 4, execution will jump to the procedure again, and with the next #`endproc` the execution will jump back to line 5.
There, 500 is assigned to %`frequency`, followed by the third jump to the procedure.
the third #`endproc` jumps back to the line after the third #`@`, i.e. to line 7.
Here the execution of the script will stop, because there are no more executable commands
(the procedure definition at the end is not executed again).

Arguments
=========

The above example contains something awkward. The procedure %`playOctave` requires that the variable %`frequency`
is set to an appropriate value, so before calling %`playOctave` you always have to insert a line like
{;
	frequency = 440
}
This can be improved upon. In the following version of the script, the procedure %`playOctave` requires an explicit %argument:
{;
	@playOctave: 440
	@playOctave: 400
	@playOctave: 500
	\#{procedure} playOctave: frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
		Play
		Remove
		octaveHigher = 2 * frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc}
}
This works as follows. The first line of the procedure now not only contains the name (%`playOctave`),
but also a list of variables (here only one: %`frequency`). In the first line of the script,
the procedure %`playOctave` is called with the %%argument% 440.
Execution then jumps to the procedure, where the argument 440 is assigned to the variable %`frequency`,
which is then used in the body of the procedure.

Encapsulation and local variables
=================================

Although the size of the script has now been reduced to 12 lines, which cannot be further improved upon,
there is still something wrong with it. Imagine the following script:
{;
	frequency = 300
	@playOctave: 440
	@playOctave: 400
	@playOctave: 500
	\`{writeInfoLine}: frequency
	\#{procedure} playOctave: frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01
		Play
		Remove
		octaveHigher = 2 * frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc
}
You might have thought that this script will write “300” to the Info window,
because that is what you expect if you look at the first five lines.
However, the procedure will assign the values 440, 400, and 500 to the variable %`frequency`,
so that the script will actually write “500” to the Info window, because 500
is the last (fourth!) value that was assigned to the variable %`frequency`.

What you would want is that variables that are used inside procedures, such as %`frequency` and %`octaveHigher` here,
could somehow be made not to “clash” with variable names used outside the procedure.
A trick that works would be to include the procedure name into the names of these variables:
{;
	frequency = 300
	@playOctave: 440
	@playOctave: 400
	@playOctave: 500
	\`{writeInfoLine}: frequency
	\#{procedure} playOctave: playOctave.frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, playOctave.frequency, 0.2, 0.01, 0.01
		Play
		Remove
		playOctave.octaveHigher = 2 * playOctave.frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, playOctave.octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc
}
This works. The six tones will be played, and \"300\" will be written to the Info window.
But the formulation is a bit wordy, isn’t it?

Fortunately, Praat allows an abbreviated version of these long names:
just leave “playOctave” off from the names of the variables, but keep the period (`.`):
{;
	frequency = 300
	@playOctave: 440
	@playOctave: 400
	@playOctave: 500
	\`{writeInfoLine}: frequency
	\#{procedure} playOctave: .frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, .frequency, 0.2, 0.01, 0.01
		Play
		Remove
		.octaveHigher = 2 * .frequency
		Create Sound as pure tone: “note”, 1, 0, 0.3, 44100, .octaveHigher, 0.2, 0.01, 0.01
		Play
		Remove
	\#{endproc
}
This is the final version of the script. It works because Praat knows that
you are using the variable names %`.frequency` and %`.octaveHigher` in the context of the procedure %`playOctave`,
so that Praat knows that by these variable names you actually mean to refer to %`playOctave.frequency` and %`playOctave.octaveHigher`.

It is advisable that you use such “local” variable names for all %parameters of a procedure,
i.e. for the variables listed after the #`procedure` word (e.g. %`.frequency`),
as well as for all variables that you create in the procedure body (e.g. %`.octaveHigher`).
In this way, you make sure that you don't inadvertently use a variable name that is also used outside the procedure
and thereby perhaps inadvertently change the value of a variable that you expect to be constant across a procedure call.

A list of numeric and string arguments
======================================

You can use multiple arguments, separated by commas, and string arguments (with a dollar sign in the variable name):
{
	@listSpeaker: “Bart”, 38
	@listSpeaker: “Katja”, 24
	\#{procedure} listSpeaker: .name$, .age
		appendInfoLine: “Speaker ”, .name$, “ is ”, .age, “ years old.”
	\#{endproc
}
or
{
	@conjugateVerb: “be”, “I am”, “you are”, “she is”
	\#{procedure} conjugateVerb: .verb$, .first$, .second$, .third$
		writeInfoLine: “Conjugation of 'to ”, .verb$, “':”
		appendInfoLine: “1sg ”, .first$
		appendInfoLine: “2sg ”, .second$
		appendInfoLine: “3sg ”, .third$
	\#{endproc}
}
For the arguments you can use expressions:
{;
	@playOctave: 400 + 100
}
As with all string literals, the double quotes in literal string arguments should be doubled:
{;
	\#{procedure} texts: .top$, .bottom$
		Text top: “yes”, .top$
		Text bottom: “yes”, .bottom$
	\#{endproc}
	@texts: """ hello"" at the top", """goodbye"" at the bottom"
}
unless you use curly quotes:
{;
	@texts: “" hello" at the top”, “"goodbye" at the bottom”
}
or even
{;
	@texts: ““ hello” at the top”, ““goodbye” at the bottom”
}

Restrictions
============

You can have any number of procedures in your script. The name of a procedure can have any number of characters (at least 1, of course).
A procedure name can consist of letters (“word characters”), underscores (“_”), periods (“.”) and digits,
but it cannot start with a digit; this is the same restriction as for variable names.

Functions
=========

The Praat scripting language does not have the concept of a “function” like some other scripting languages do.
A function is a procedure that returns a number, a string, a vector, a matrix, or a string array.
For instance, you can imagine the function `squareNumber` (),
which takes a number (e.g. 5) as an argument and returns the square of that number (e.g. 25).
Here is an example of how you can do that, using the global availability of local variables:
{
	@squareNumber: 5
	\`{writeInfoLine}: “The square of 5 is ”, squareNumber.result, “.”
	\#{procedure} squareNumber: .number
		.result = .number ^ 2
	\#{endproc
}
Another way to emulate functions is to use a variable name as an argument:
{
	@squareNumber2: 5, “square5”
	\`{writeInfoLine}: “The square of 5 is ”, square5, “.”
	\#{procedure} squareNumber2: .number, .squareVariableName$
		'.squareVariableName$' = .number ^ 2
	\#{endproc
}
However, this uses variable substitution, a trick better avoided.

################################################################################
"Scripting 5.6. Arrays and dictionaries"
© Paul Boersma 2017-07-18,2025

You can use arrays of numeric and string variables:
{
	\#{for} i \#{from} 1 \#{to} 5
		square [i] = i * i
		text$ [i] = \`{mid$} (“hello”, i)
	\#{endfor}
}
After this, the variables `square[1]`, `square[2]`, `square[3]`, `square[4]`, `square[5]`,
`text$[1]`, `text$[2]`, `text$[3]`, `text$[4]`, and `text$[5]` contain
the values 1, 4, 9, 16, 25, “h”, “e”, “l”, “l”, and “o”, respectively:
{
	\`{writeInfoLine}: “Some squares:”
	\#{for} i \#{from} 1 \#{to} 5
		\`{appendInfoLine}: “The square of ”, i, “ is ”, square [i]
	\#{endfor}
}

In the examples above, the %index into the array was always a number.
A %hash or %dictionary is an array variable where the index is a string:
{
	age [“John”] = 36
	age [“Babs”] = 39
	\`{writeInfoLine}: “John is ”, age [“John”], “ years old.”
}

See also
========

You can use any number of array and dictionary variables in a script,
but for many applications, namely whenever it were useful to look at a numeric array as a single object,
it may be better to use vectors and matrices (see @@Scripting 5.7. Vectors and matrices@)
or to use Matrix or Sound objects.

################################################################################
"Scripting 5.7. Vectors and matrices"
© Paul Boersma 2023-01-30

1. What is a vector?
====================

A ##numeric vector# is an array of numbers, regarded as a single object.
For instance, the squares of the first five integers can be collected in the vector { 1, 4, 9, 16, 25 }.
In a Praat script, you can put a vector into a variable whose name ends in a number sign (“`#`”):
{
	squares# = { 1, 4, 9, 16, 25 }
}
After this, the variable %`squares#` contains the value { 1, 4, 9, 16, 25 }.
We say that the vector %`squares#` has five %dimensions, i.e. it contains five numbers.

Whereas in @@Scripting 3.2. Numeric variables@ we talked about a numeric variable as being analogous to a house
where somebody (the numeric %value) could live, a numeric vector with five dimensions
can be seen as a %street that contains five houses, which are numbered with the indexes 1, 2, 3, 4 and 5,
each house containing a numeric value. Thus, the street %`squares#` contains the following five houses:
%`squares#` [1], %`squares#` [2], %`squares#` [3], %`squares#` [4] and %`squares#` [5].
Their values (the numbers that currently live in these houses) are 1, 4, 9, 16 and 25, respectively.

To list the five values with a loop, you could do:
{
	\`{writeInfoLine}: “Some squares:”
	\#{for} i \#{from} 1 \#{to} size (squares#)
		\`{appendInfoLine}: “The square of ”, i, “ is ”, squares# [i]
	\#{endfor}
}

Instead of the above procedure to get the vector %`squares#`, with a pre-computed list of five squares,
you could compute the five values with a formula, as in the example of @@Scripting 5.6. Arrays and dictionaries@.
However, in order to put a value into an element of the vector, you have to create the vector first
(i.e., you have to build the whole street before you can put something in a house),
so we start by creating a vector with five zeroes in it:
{
	squares# = zero# (5)
}
After this, %`squares#` is the vector { 0, 0, 0, 0, 0 }, i.e., the value of each element is zero.
Now that the vector (street) exists, we can put values into (populate) the five elements (houses):
{
	\#{for} i \#{from} 1 \#{to} size (squares#)
		squares# [i] = i * i
	\#{endfor}
}
After this, the variable %`squares#` has the value { 1, 4, 9, 16, 25 }, as before,
but now we had the computer compute the squares.

2. Creating a vector
====================

You can create a vector in many ways. The first way we saw was with a ##vector literal#,
i.e. a series of numbers (or numeric formulas) between braces:
{
	lengths# = { 1.83, 1.795, 1.76 }
}

The second way we saw was to create a series of #zeroes. To create a vector consisting of 10,000 zeroes, you do
{
	longEmptyArray# = \#`{zero#} (10000)
}

Another important type of vector is a series of random numbers.
To create a vector consisting of 10,000 values drawn from a ##Gaussian distribution#
with true mean 0.0 and true standard deviation 1.0, you could do
{
	noise# = \#`{randomGauss#} (10000, 0.0, 1.0)
}
To create a vector consisting of 10,000 values drawn from a ##uniform distribution of real numbers#
with true minimum 0.0 and true maximum 1.0, you use
{
	a# = \#`{randomUniform#} (10000, 0.0, 1.0)
}
To create a vector consisting of 10,000 values drawn from a ##uniform distribution of integer numbers#
with true minimum 1 and true maximum 10, you use
{
	a# = \#`{randomInteger#} (10000, 1, 10)
}
To create a vector containing the integer numbers 1 through 64, you use
{
	a# = \#`{to#} (64)
}
To create a vector containing the integer numbers 10 through 20, you use
{
	a# = \#`{from_to#} (10, 20)
}
To create a vector containing linearly increasing (not necessarily integer) numbers from 10 through 20 in steps of 2, you use
{
	a# = \#`{from_to_by#} (10, 20, 2)
}
To create five linearly increasing numbers between 0 and 10 (i.e. { 0, 2.5, 5, 7.5, 10 }), you use
{
	a# = \#`{from_to_count#} (0, 10, 5)
}
To divide the range between 0 and 12 symmetrically with step 5 (i.e. { 1, 6, 11 }), you use
{
	a# = \#`{between_by#} (0, 12, 5)
}
To divide the range between 0 and 10 into five equal parts and list their centres (i.e. { 1, 3, 5, 7, 9 }), you use
{
	a# = \#`{between_count#} (0, 10, 5)
}
To sort the numbers in a vector (e.g. { 7.4, 1.3, 3.6 }), you use
{
	a# = \#`{sort#} ({ 7.4, 1.3, 3.6 })
	\`{writeInfoLine} (a#)
}
To randomly shuffle the numbers in a vector (e.g. { 7.4, 1.3, 3.6 }), you use
{
	a# = \#`{shuffle#} ({ 7.4, 1.3, 3.6 })
}
which can yield { 1.3, 7.4, 3.6 } or any of the five other orders of the elements.

Vectors can also be created by some menu commands. For instance, to get vectors representing
the times and pitch frequencies of the frames in a Pitch object, you can do
{;
	\`{selectObject}: myPitch
	times# = List all frame times
	pitches# = List values in all frames: “hertz”
}

3. Turning a vector into a number
=================================

For the vector defined above, you can compute the #sum of the five values as
{
	\#`{sum} (squares#)
}
which gives 55. You compute the #average of the five values as
{
	\#`{mean} (squares#)
}
which gives 11. You compute the ##standard deviation# of the values as 
{
	\#`{stdev} (squares#)
}
which gives 9.669539802906858 (the standard deviation is undefined for vectors with fewer than 2 elements).
The ##center of gravity# of the distribution defined by regarding
the five values as relative frequencies as a function of the index from 1 to 5 is computed by
{
	\#`{center} (squares#)
}
which gives 4.090909090909091 (for a vector with five elements, the result will always be
a number between 1.0 and 5.0). You compute the ##inner product# of two equally long vectors as follows:
{
	other# = { 2, 1.5, 1, 0.5, 0 }
	result = \#`{inner} (squares#, other#)
}
which gives 1*2 + 4*1.5 + 9*1 + 16*0.5 + 25*0 = 25.
The formula for this is \su__%i=1_^5 %squares[%i] * %other[%i], so that an alternative piece of code could be
{
	result = \#`{sumOver} (i to 5, squares# [i] * other# [i])
}

4. Converting vectors to vectors
================================
{
	a# = squares# + 5   ; adding a number to each element of a vector
	assert a# =	{ 6, 9, 14, 21, 30 }
}
causes %`a#` to become the vector { 6, 9, 14, 21, 30 }.
{
	b# = a# + { 3.14, 2.72, 3.16, -1, 7.5 }   ; adding two vectors of the same length
}
causes %`b#` to become the vector { 9.14, 11.72, 17.16, 20, 37.5 }.
{
	c# = b# / 2   ; dividing each element of a vector
}
causes %`c#` to become the vector { 4.57, 5.86, 8.58, 10, 18.75 }.
{
	d# = b# * c#   ; elementwise multiplication
}
causes %`d#` to become the vector { 41.7698, 68.6792, 147.2328, 200, 703.125 }.

A vector can also be given to a ##menu command# that returns another vector.
For instance, to get a vector representing the pitch frequencies at 0.01-second intervals in a Pitch object,
you can do
{;
	\`{selectObject}: myPitch
	tmin = Get start time
	tmax = Get end time
	times# = \#`{between_by#} (tmin, tmax, 0.01)
	pitches# = List values at times: times#, “hertz”, “linear”
}

5. What is a matrix?
====================

A ##numeric matrix# is a two-indexed array of numbers, regarded as a single object.
In a Praat script, you can put a matrix into a variable whose name ends in two number signs (“`##`”):
{
	confusion## = {{ 3, 6, 2 }, { 8, 2, 1 }}
}
After this, the variable %`confusion##` contains the value {{ 3, 6, 2 }, { 8, 2, 1 }}.
We say that the matrix %`confusion##` has two %rows and three %columns, i.e. it contains six numbers.

Whereas a numeric vector with five dimensions could be seen (see above) as a street that contains five houses,
the matrix %`confusion##` can be seen as a city district with two avenues crossed by three streets,
where everybody lives on an intersection (the analogies start to get less realistic).

6. Creating a matrix
====================

You can create a matrix in many ways. The first way we saw was with a ##matrix literal#,
i.e. a series of series of numbers (or numeric formulas) between nested braces.

The second way is as a matrix of #zeroes. To create a matrix consisting of 100 rows of 10,000 zeroes, you do
{
	a## = \#`{zero##} (100, 10000)
}
After this,
{
	\#`{numberOfRows} (a##)
}
is 100, and
{
	\#`{numberOfColumns} (a##)
}
is 10000.

Another important type of matrix is one filled with random numbers.
To create a matrix consisting of 100 rows of 10,000 values drawn from a ##Gaussian distribution#
with true mean 0.0 and true standard deviation 1.0, you can do
{
	noise## = \#`{randomGauss##} (100, 10000, 0.0, 1.0)
}

You can create a matrix as the outer product of two vectors:
{;
	m## = \#`{outer##} (u#, v#)
}
which is the same as
{;
	m## = \#`{zero##} (\`{size} (u#), \`{size} (v#))
	\#{for} irow \#{to} \`{size} (u#)
		\#{for} icol \#{to} \`{size} (v#)
			m## [irow, icol] = u# [irow] * v# [icol]
		\#{endfor}
	\#{endfor}
}
or in mathematical notation
~	%m__%ij_ = %u_%i %v_%j   (%i = 1..%M, %j = 1..%N)

where %M is the number of rows and %N is the number of columns.

7. Computations with matrices
=============================

You can add matrices:
{;
	c## = a## + b##
}

Elementwise multiplication:
{;
	c## = a## * b##
}
which does
~	%c__%ij_ = %a__%ij_ %b__%ij_   (%i = 1..%M, %j = 1..%N)

Matrix multiplication:
{;
	c## = mul## (a##, b##)
}
which does
~	%m__%ij_ = \\su__%k=1_^K  %a__%ik_ %b__%kj_   (%i = 1..%M, %j = 1..%N)

where %M is the number of rows of %a, %N is the number of columns of %b,
and %K is the number of columns of %a, which has to be equal to the number if rows of %b.

Matrix-by-vector multiplication:
{;
	v# = mul# (m##, u#)
}
which does
~	%v_%i = \\su__%j=1_^N  %m__%ij_ %u_%j   (%i = 1..%M)

where %M is the number of rows of %m, and %N is the number of columns of %m,
which has to be equal to the dimension of %u. Also
{;
	v# = mul# (u#, m##)
}
which does
~	%v_%j = \\su__%i=1_^M  %u_%i %m__%ij_   (%j = 1..%N)

where %M is the number of rows of %m, which has to be equal to the dimension of %u,
and %N is the number of columns of %m.

8. String vectors
=================

You can create string vectors (also called string arrays) in the following ways:
{
	a$# = { “hello”, “goodbye” }
}
creates a vector with two strings, which you can access as `a$# [1]`, which is “hello”, and `a$# [2]`, which is “goodbye”.

{
	a$# = \#`{empty$#} (10)
}
creates a vector with 10 empty strings, which you can access as `a$# [1]` through `a$# [10]`.

{;
	text$# = \#`{readLinesFromFile$#} (“hello.txt”)
}
creates a vector with 100 strings if the file `hello.text` contains 100 lines of text.
{;
	fileNames$# = \#`{fileNames$#} (“sound/*.wav”)
}
creates a vector containing the names of all WAV files in the folder `sound`.
{;
	folderNames$# = \#`{folderNames$#} (“.”)
}
creates a vector containing the names of all folders in the folder where the script resides.
{
	inks$# = \#`{splitByWhitespace$#} (“Hello, how are you?”)
}
creates a vector containing the strings “Hello,” (including the comma), “how”, “are”, and “you?”.

################################################################################
"Scripting 5.8. Including other scripts"
© Paul Boersma 2017-07-18,2025

You can include other scripts within your script:
{;
	a = 5
	include square.praat
	writeInfoLine: a
}
The Info window will show the result 25 if the file square.praat contains the following:
{;
	a = a * a
}
The inclusion is done before any other part of the script is considered, so you can use the #form statement
and all variables in it. Usually, however, you will put some procedure definitions in the include file, that is
what it seems to be most useful for. Watch out, however, for using variable names in the include file:
the example above shows that there is no such thing as a separate name space.

Note that you do not put quotes around the name of the include file.
This is because the name of the include file has to be given explicitly; you cannot put it into a variable, for instance.

You can use full or relative file names. For instance, the file `square.praat` above is expected to be in the same
folder as the script that says `include square.praat`. If you use the ScriptEditor, you will first have to save
the script that you are editing before any relative file names become meaningful (this is the same as with other
uses of relative file names in scripts).

You can “nest” include files, i.e., included scripts can include other scripts. However, relative file names
are always evaluated relative to the folder of the outermost script.

The #include statement can only be at the start of a line: you cannot put any spaces in front of it.

################################################################################
"Scripting 5.9. Quitting"
© Paul Boersma 2019-07-13

Usually, the execution of your script ends when the interpreter has executed the last line
that is not within a procedure definition. However, you can also explicitly stop the script:

#exitScript ( )
:	stops the execution of the script in the normal way, i.e. without any messages to the user.
Any settings (form) window is removed from the screen (unless Apply was clicked instead of OK).

#exitScript: %%error-message%
:	stops the execution of the script while sending an error message to the user.
You can use the same argument list as with #writeInfoLine.
Any settings (form) window will stay on the screen.

For an example, see @@Scripting 6.8. Messages to the user@.

################################################################################
"Scripting 6. Communication outside the script"
© Paul Boersma 2024-11-16

,	@@Scripting 6.1. Arguments to the script@ (form/endform, runScript)
,	@@Scripting 6.2. Writing to the Info window@ (writeInfoLine, appendInfoLine, appendInfo, tab\\$ )
,	@@Scripting 6.3. Query commands@ (Get, Count)
,	@@Scripting 6.4. Files@ (fileReadable, readFile, writeFile, deleteFile, createFolder)
,	@@Scripting 6.5. Calling system commands@ (runSystem, environment\\$ , stopwatch)
,	@@Scripting 6.6. Controlling the user@ (pause, beginPause/endPause, chooseReadFile\\$ )
,	@@Scripting 6.7. Sending a message to another program@
,	@@Scripting 6.8. Messages to the user@ (exitScript, assert, nowarn, nocheck)
,	@@Scripting 6.9. Calling from the command line

################################################################################
"Scripting 6.1. Arguments to the script"
© Paul Boersma xx,2023,2024

You can cause a Praat script to prompt for arguments. The file `playSine.praat` may contain the following:
{;
	\#{form}: “Play a sine wave”
		\#{positive}: “Sine frequency (Hz)”, “377.0”
		\#{positive}: “Gain (0..1)\", “0.3 (= not too loud)”
	\#{endform}
	Create Sound as pure tone: “sine” + string$ (sine_frequency),
	... 1, 0, 1, 44100, sine_frequency, gain, 0.01, 0.01
	Play
	Remove
}

When running this script, the interpreter puts a settings window (%form) on your screen,
entitled “Play a sine wave”,
with two fields, titled “Sine frequency (Hz)” and “Gain”, that have been provided
with the standard values “377.0” and “0.3 (= not too loud)”, which you can change before clicking #OK.

Inside the script, the field names can be accessed as variables: these have underscores
instead of spaces, and the parentheses (Hz) have been chopped off. Note that the first
letter of these variables is converted to lower case, so that you can assign to them in your script.

Inside the script, the value “0.3 (= not too loud)” will be known as 0.3,
because this is a numeric field.

You can use the following field types in your forms:

#`real`: %`variable$`, %`initialValue$`
:	for real numbers.

#`positive`: %`variable$`, %`initialValue$`
:	for positive real numbers: the form issues an error message if the number
that you enter is negative or zero (further on in the script,
you can freely change it to any real number, including negatives ones such as -1.5, or zero).

#`integer`: %`variable$`, %`initialValue$`
:	for whole numbers: the form reads the number as an integer
(further on in the script, you can give it any real value, e.g. 3.14).

#`natural`: %`variable$`, %`initialValue$`
:	for positive whole numbers: the form issues an error message if the number
that you enter is negative or zero (further on in the script, you can give it any real value).

#`word`: %`variable$`, %`initialValue$`
:	for a string without spaces: the form only reads up to the first space (“oh yes” becomes “oh”;
further on in the script, you can give the string any value, perhaps with spaces in it).

#`sentence`: %`variable$`, %`initialValue$`
:	for any short string.

#`text`: %`variable$`, %`initialValue$`
:	for any possibly long string (the variable name will show up above the field).

#`text`: %`numberOfLines`, %`variable$`, %`initialValue$`
:	for any possibly very long string (multiple lines), %`numberOfLines` can be between 1 and 33.

#`boolean`: %`variable$`, %`initialValue`
:	a check box will be shown; the initial value is 1 (on) or 0 (off).

#`boolean`: %`variable$`, %`initialValue$`
:	a check box will be shown; to switch it on,
set the initial value to `“on”`, `“yes”`, `“ON”`, `“YES”`, `“On”` or `“Yes”`;
to switch it off, set it to `“off”`, `“no”`, `“OFF”`, `“NO”`, `“Off”` or `“No”`.

#`choice`: %`variable$`, %`initialValue`
:	a multiple-choice box (or “radio box”) will be shown; the value is 1 or higher. This is followed by a series of:

#`option`: %`text$`
:	an option button in a multiple-choice box (see example below).

#`optionmenu`: %`variable$`, %`initialValue`
:	a multiple-choice menu header will be shown; the value is 1 or higher.
Just as `choice`, this is followed by:

#`option`: %`text$`
:	an option in a multiple-choice menu (see example below).

#`comment`: %`text$`
:	a line with any text.

#`infile`: %`variable$`, %`initialValue$`
:	for a full path to an existing file, usually for reading.

#`outfile`: %`variable$`, %`initialValue$`
:	for a full path to a new file, usually for saving.

#`folder`: %`variable$`, %`initialValue$`
:	for a full path to a folder.

#`realvector`: %`variable$`, %`format$`, %`initialValue$`
:	for a vector with real values. The format can be `“(whitespace-separated)”` or `“(formula)”`;
the initial value should then probably be something like `“10 -9 80”` or `“{ 10, -9, 80 }”`, respectively.

#`realvector`: %`numberOfLines`, %`variable$`, %`format$`, %`initialValue$`
:	use this if you want a field with less or more than the standard 7 lines.

#`positivevector`: %`variable$`, %`format$`, %`initialValue$`

#`positivevector`: %`numberOfLines`, %`variable$`, %`format$`, %`initialValue$`
:	use either of these if you want to check that all initial elements are positive real numbers.

#`integervector`: %`variable$`, %`format$`, %`initialValue$`

#`integervector`: %`numberOfLines`, %%variable\\$ %, %%format$ %, %%initialValue$ %
:	use either of these if you want to check that all initial elements are whole numbers.

#`naturalvector`: %`variable$`, %`format$`, %`initialValue$`

#`naturalvector`: %`numberOfLines`, %`variable$`, %`format$`, %`initialValue$`
:	use either of these if you want to check that all initial elements are positive whole numbers.

Inside the script, strings are known as string variables, numbers as numeric variables. Consider the following form:
{;
	\#{form}: “Sink it”
		\#{sentence}: “Name of the ship”, “Titanic”
		\#{real}: “Distance to the iceberg (m)”, “500.0”
		\#{natural}: “Number of people”, “1800”
		\#{natural}: “Number of boats”, “10”
	\#{endform}
}
In the script following this form, the variables will be known as `name_of_the_ship$`, `distance_to_the_iceberg`,
`number_of_people`, and `number_of_boats`.

The variable associated with a radio box will get a numeric as well as a string value:
{;
	\#{form}: “Fill attributes”
		\#{comment}: “Choose any colour and texture for your paintings”
		\#{choice}: “Colour”, 5
				\#{option}: “Dark red”
				\#{option}: “Sea green”
				\#{option}: “Navy blue”
				\#{option}: “Canary yellow”
				\#{option}: “Black”
				\#{option}: “White”
		\#{choice}: \"Texture”, 1
				\#{option}: “Smooth”
				\#{option}: “Rough”
				\#{option}: “With holes”
	\#{endform}
	\`{writeInfoLine}: “You chose the colour ”, colour$, “ and the texture ”, texture$, “.”
}
This shows two multiple-choice boxes. In the Colour box, the fifth button (Black) is the standard value here.
If you click “Navy blue” and then #%OK, the variable %`colour` will have the value 3,
and the variable %`colour$` will have the value “Navy blue”.
So you can test the value of the Colour box in either of the following ways:
{;
	if colour = 4
}
or
{;
	if colour$ = “Canary yellow\"
}

The field type #`optionmenu` is completely analogous to #`choice`,
but uses up much less space on the screen:
{;
	\#{form}: “Fill attributes”
		\#{comment}: “Choose any colour and texture for your paintings”
		\#{optionmenu}: “Colour”, 5
			\#{option}: “Dark red”
			\#{option}: “Sea green”
			\#{option}: “Navy blue”
			\#{option}: “Canary yellow”
			\#{option}: “Black”
			\#{option}: “White”
		\#{optionmenu}: “Texture”, 1
			\#{option}: “Smooth”
			\#{option}: “Rough”
			\#{option}: “With holes”
	\#{endform}
	\`{writeInfoLine}: “You chose the colour ”, colour$, “ and the texture ”, texture$, “.”
}

For the vector types #`realvector`, #`positivevector`, #`integervector` and #`naturalvector`
you have to specify an initial format, which will be shown to the user:
{;
	\#{form}: “Interesting times”
		\#{comment}: “List the times that you are interested in”
		\#{realvector}: “Fixed times (s)”, “(whitespace-separated)”, “0.5 1.7 2.8”
		\#{realvector}: “Random times (s)”, “(formula)”, “randomUniform# (5, 0.0, 1.0)”
	\#{endform}
	\`{writeInfoLine}: “Your fixed times are ”, fixed_times#, “ and your random times are ”, random_times#, “.”
}
Clicking OK without editing the two fields may print
`
	Your fixed times are 0.5 1.7 2.8 and your random times are 0.754675 0.121393653 0.39856 0.8376572 0.387537.
`

The field types #infile, #outfile and #folder always yield a full path.
Consider the script `playFile.praat`, which contains the following:
{;
	\#{form}: “Play file”
		\#{infile}: “File to play”, “hello.wav”
	\#{endform}
	\`{writeInfoLine}: “You chose the file ”, file_to_play$, “.”
	Read from file: file_to_play$
	Play
	Remove
}
If you just click OK and `playFile.praat` is in the folder `/Users/miep/research/usefulScripts`,
then this will print
`
	You chose the file /Users/miep/research/usefulScripts/hello.wav.
`
into the Info window, and play the sound in that file.

You can combine two short fields into one by using #`left` and #`right`:
{;
	\#{form}: “Get duration”
		\#{natural}: “left Year range”, “1940”
		\#{natural}: “right Year range”, “1945”
	\#{endform}
	duration = right_Year_range - left_Year_range
	\`{writeInfoLine}: “The duration is ”, duration, “ years.”
}
The interpreter will only show the single text “Year range”, followed by two small text fields.

Calling a script from another script
====================================

Scripts can be nested: the file `doremi.praat` may contain the following:
{;
	\#{runScript}: “playSine.praat”, 550, 0.9
	\#{runScript}: “playSine.praat”, 615, 0.9
	\#{runScript}: “playSine.praat”, 687, 0.9
}

With #`runScript`, Praat will not display a form window, but simply execute the script
with the two arguments that you supply on the same line (e.g. 550 and 0.9).

Values for #`choice` must be passed as strings:
{;
	\#{runScript}: “fill attributes.praat”, “Navy blue”, “With holes”
}

Values for vectors can be passed either as a vector expression or as a white-space separated string,
independently of the initial format:
{;
	\#{runScript}: “interesting times.praat”, { 0.3, 0.5, 0.7, 2.0 }, “18000 0.3”
}

You can pass values for #boolean either as the quoted strings “yes” and “no” (or their variants) or as the unquoted numbers 1 and 0.

In #`runScript`, the path to the external script, as well as the paths to #`infile`, #`outfile` and #`folder` parameters
are taken relative to the folder of the current script. For instance, suppose that the current script is
`/Users/miep/research/project19/analyse.praat` and contains:
{;
	\#{runScript}: “../usefulScripts/playFile.praat”, “sounds/sound3.wav”
}
then running the current script will run the above-mentioned script `/Users/miep/research/usefulScripts/playFile.praat`,
which will play the file `/Users/miep/research/project19/sounds/sound3.wav`.

################################################################################
"Scripting 6.2. Writing to the Info window"
© Paul Boersma 2022-12-02,2025

With the @Info button and several commands in the @@Query submenu@ (or with query commands in menus in the editors),
you write to the @@Info window@ (if your program is run from the command line,
the text goes to the console window or to %stdout instead; see @@Scripting 6.9. Calling from the command line|\\SS6.9).

The commands @`writeInfo`, @`writeInfoLine`, @`appendInfo` and @`appendInfoLine`
allow you to write to the Info window from a script. Those with #`write` in their name clear the Info window
before they write to it, those with #`append` in their name do not. Those with #`Line` in their name make sure
that a following #`appendInfo` or #`appendInfoLine` will write on the next line.

These four functions take a variable number of numeric and/or string arguments, separated by commas.
Here is an example:
{
	\`{writeInfoLine}: “Pitch extrema:”
	Create Sound as pure tone: “sine”, 1, 0, 0.1, 44100, 377, 0.2, 0.01, 0.01
	To Pitch: 0.01, 75, 600
	minimum = Get minimum: 0, 0, “hertz”, “parabolic”
	\`{appendInfoLine}: "Minimum: ", minimum, " Hz"
	maximum = Get maximum: 0, 0, “hertz”, “parabolic”
	\`{appendInfoLine}: "Maximum: ", maximum, " Hz"
}

The following script builds a table with statistics about a pitch contour:
{
	\`{writeInfoLine}: “Minimum”, tab$, “Maximum”
	\`{appendInfo}: fixed$ (minimum, 3)
	\`{appendInfo}: tab$
	\`{appendInfo}: fixed$ (maximum, 3)
	\`{appendInfoLine}: “”
}
The little string #`tab$` is a %tab character; it allows you to create
table files that can be read by some spreadsheet programs. The little string #`newline$` is a %newline character;
it moves the following text to the next line.

You could combine the last four print statements into:
{
	\`{appendInfoLine}: fixed$ (minimum, 3), tab$, fixed$ (maximum, 3)
}
which is the same as:
{
	\`{appendInfo}: fixed$ (minimum, 3), tab$, fixed$ (maximum, 3), newline$
}

To clear the Info window, you can do
{
	\`{writeInfo}: “”
}
or
{
	\`{clearinfo}
}

################################################################################
"Scripting 6.3. Query commands"
© Paul Boersma 2014-01-07

If you click the ##Get mean...# command for a Pitch object,
the Info window will contain a text like “150 Hz” as a result. In a script, you would rather
have this result in a variable instead of in the Info window. The solution is simple:
{;
	mean = Get mean: 0, 0, “Hertz”, “Parabolic”
}
The numeric variable %mean now contains the number 150. When assigning to a numeric variable,
the interpreter converts the part of the text before the first space into a number.

You can also assign to string variables:
{;
	mean$ = Get mean: 0, 0, “Hertz\", “Parabolic”
}
The string variable %`mean$` now contains the entire string “150 Hz”.

This works for every command that would otherwise write into the Info window.

################################################################################
"Scripting 6.4. Files"
© Paul Boersma 2020-12-29

You can read from and write to text files from a Praat script.

Reading a file
==============

You can check the availability of a file for reading with the function
{;
	\#`{fileReadable} (\%{fileName$})
}
which returns 1 (true) if the file exists and can be read, and 0 (false) otherwise.
Note that %`fileName$` is taken relatively to the folder where the script is saved;
for instance, if your script is in the folder `Paolo/project1`, then the file name
“hello.wav” refers to `Paolo/project1/hello.wav`, the file name “yesterday/hello.wav”
refers to `Paolo/project1/yesterday/hello.wav`, and the file name “../project2/hello.wav”
refers to `Paolo/project2/hello.wav` (“..” goes one folder up).
You can also use full path names such as `C:/Users/Paolo/project1/hello.wav`
on Windows and `/Users/Paolo/project1/hello.wav` on the Mac.

To read the contents of an existing text file into a string variable or into a numeric variable, you use
{;
	text$ = \#`{readFile$} (“myFile.txt”)
}
or
{;
	number = \#`{readFile} (“myFile.txt”)
}
If the file does not exist, the script terminates with an error message.

Example: reading a settings file
================================

Suppose that the file `height.inf` may contain an appropriate value for a numeric variable
called `height`, which we need to use in our script. We would like to read it with
{;
	height = \#`{readFile} (“height.inf”)
}

However, this script will fail if the file `height.inf` does not exist. To guard
against this situation, we could check the existence of the file, and supply a default
value in case the file does not exist:
{;
	fileName$ = “height.inf”
	\#{if} \#`{fileReadable} (fileName$)
		height = \#`{readFile} (fileName$)
	\#{else}
		height = 180
	\#{endif}
}

Writing a file
==============

You write into a new text file just as you write into the Info window:
{;
	\#`{writeFileLine}: “myFile.txt”, “The present year is ”, 2000 + 13, “.”
}
and likewise you use @`writeFile` if you don't want a newline symbol at the end of the file.
If the file cannot be created, the script terminates with an error message.

To append text at the end of an existing file, you use
{;
	\#`{appendFileLine}: “myFile.txt”, “Next year it will be ”, 2000 + 14, “.”
}

With @`appendFileLine` (and @`appendFile`, which does not add the newline),
we follow the rule that if the file does not yet exist, it is created first.

You can create a folder (directory) with
{;
	\#`{createFolder}: \%{folderPath$}
}
where, as with file names, %`folderPath$` can be relative to the folder of the script
(e.g. “`data`”, or “`yesterday/data`”, or “`../project2/yesterday/data`”)
or an absolute path (e.g. `C:/Users/Paolo/project1/yesterday/data` on Windows
or “/Users/Paolo/project1/yesterday/data” on the Mac).
If the folder already exists, this command does nothing.

You can delete an existing file with the function
{;
	\#`{deleteFile}: \%{fileName$}
}
If the file does not exist, this command does nothing.

Example: writing a table of squares
===================================

Suppose that we want to create a file with the following 100 lines of text:
`
	The square of 1 is 1
	The square of 2 is 4
	The square of 3 is 9
	...")
	The square of 100 is 10000
`

We can do this by appending 100 lines:
{;
	\#`{deleteFile}: “squares.txt”
	\#{for} i \#{to} 100
		\#`{appendFileLine}: “squares.txt”, “The square of ”, i, “ is ”, i * i
	\#{endfor}
}
Note that we delete the file before appending to it,
in order that we do not append to an already existing file.

You can append the contents of the Info window to a file with
{;
	\#`{appendFile}: “out.txt”, info$ ( )")
}

Folder listings
===============

To get the names of the files if a certain type in a certain folder, use
{;
	fileNames$# = \#`{fileNames$#}: \%{path$}
}

For instance, to read in all the sound files in a specified folder,
you could use the following script:
{;
	folder$ = “/usr/people/miep/sounds”
	soundFileNames$# = \#`{fileNames$#} (folder$ + “/*.wav”)
	\#{for} ifile \#{to} \`{size} (soundFileNames$#)
		\@{Read from file:} folder$ + “/” + soundFileNames$# [ifile]
	\#{endfor}
}

Alternative syntax
==================

If, on the basis of the syntax of commands and functions in earlier sections you expected that
{;
	text$ = \#`{readFile$} (“myFile.txt”)
	number = \#`{readFile} (“myFile.txt”)
}
could be written as
{;
	text$ = \#`{readFile$}: “myFile.txt”
	number = \#`{readFile}: “myFile.txt”
}
then you are right. The syntax with the colon is equivalent to the syntax with the two parentheses. Conversely, instead of
{;
	\#`{deleteFile}: \%{fileName$}
}
you can also write
{;
	\#`{deleteFile} (\%{fileName$})
}

################################################################################
"Scripting 6.5. Calling system commands"
© Paul Boersma 2020,2023,2025

Running a command-line-like command
===================================
From a Praat script you can call system commands.
These are the same commands that you would normally type into a terminal window or into the Windows command line prompt.
The syntax is the same as that of the @`writeInfo` command.

Most system commands are different on different platforms.
For instance, to throw away all WAV files in the folder whose path (relative to the script’s folder) is
in the variable `folder$` (and you are VERY SURE that this name contains NO SPACES OR SPECIAL CHARACTERS),
you could write
{;
	\#`{runSystem}: "del ", folder$, "\*.wav"   ; DANGEROUS
}
on Windows, but
{;
	\#`{runSystem}: "rm ", folder$, "/*.wav"   ; DANGEROUS
}
on Macintosh and Linux.

The script will stop running if a system command returns an error. For instance,
{;
	\#`{runSystem}: "rm ", folder$, "/*.wav"   ; DANGEROUS
}
will stop the script if there are no WAV files in the folder,
with a message like “No such file or directory”.

In order to prevent this, you can tell Praat to ignore the return value of \@{runSystem}.

Thus, to make sure that the folder contains no WAV files, you would write
{;
	\#{runSystem_nocheck}: "rm ", folder$, "/*.wav"   ; DANGEROUS
}

Typically, however, the use of @`runSystem` or #`runSystem_nocheck` is DANGEROUS:
if the string `folder$` contains spaces and semicolons and slashes, for instance,
you may lose all files on your computer or install malware without noticing it;
you really have to have full control over your files and know exactly what you are doing
before you use @`runSystem` or #`runSystem_nocheck`.

A safer way to call other programs
==================================
The above is dangerous because `runSystem` has no concept of what an argument to a program is.
For instance, what if `folder$` above is somehow “-rf .*; ls”? Will that remove my whole folder structure?
A much safer way is to use @`runSubprocess`, in which you supply the arguments directly:
{;
	\#`{runSubprocess}: "/usr/local/sbin/theOtherApp", folder$, "*.wav"
}
This will hand to `theOtherApp`: the contents of the string `folder$`
(without fear of executing any commands that are inside the foldername),
and the string `*.wav`. What is done with the asterisk (“*”) depends on your platform:
on macOS and Linux, the string is handed verbatim as “*.wav” to `theOtherApp`,
whereas on Windows the string is handed verbatim if there are no WAV files (in the folder of the script)
but as e.g. “hello.wav goodbye.wav” if the script folder contains those two files.

In general, then, @`runSubprocess` is quite predictable on macOS and Linux, but less so on Windows.
 
Getting the values of system variables
======================================
#`environment$` (%`symbol-string`)
: returns the value of an environment variable, e.g.
{;
		homeFolder$ = \#`{environment$} ("HOME")
}

Getting system duration
=======================
#`stopwatch`
: returns the time that has elapsed since the previous #stopwatch.

Here is a Praat script that measures how long it takes to do a hundred thousand assignments on your computer
(if you are reading this in Praat’s own Help, not on the web):
{
	\#`{stopwatch}
	for i to 100000
		a = 1.23456789e123
	endfor
	time = \#`{stopwatch}
	\`{writeInfoLine}: a, " ", fixed$ (time, 3)
}
How many nanoseconds is that per assignment?
{
	\`{writeInfoLine}: round (time / 100000 * 1e9)
}
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"Scripting 6.6. Controlling the user", U"ppgb", 20230130)
INTRO (U"You can temporarily halt a Praat script:")
TERM (U"#`pauseScript`: %message")
DEFINITION (U"suspends execution of the script, and allows the user to interrupt it. "
	"A message window will appear with the %message (you can use the same argument list as with @`writeInfoLine`) and the buttons #Stop and #Continue:")
CODE (U"\\#{pauseScript}: “The next file will be ”, fileName$")
NORMAL (U"The #`pauseScript` function is useful if you want to send a simple message to the user, "
	"and you only want to ask the user whether she wants to proceed or not. "
	"More interesting interactions between your script and the user are possible with the ##%%pause window%#. "
	"In a pause window you can include the same kinds of arguments as in a @@Scripting 6.1. Arguments to the script|form@. "
	"Here is an extensive example:")
CODE (U"writeInfoLine: “script”")
CODE (U"compression = 1")
CODE (U"number_of_channels = 2")
CODE (U"worth = 3")
CODE (U"for i to 5")
	CODE1 (U"\\#{beginPause}: “Hi”")
		CODE2 (U"\\#{comment}: “Type a lot of nonsense below.”")
		CODE2 (U"\\#{natural}: “Number of people”, 10")
		CODE2 (U"\\#{real}: “Worth”, worth + 1")
		CODE2 (U"\\#{positive}: “Sampling frequency (Hz)”, “44100.0 (= CD quality)”")
		CODE2 (U"\\#{word}: “hi”, “hhh”")
		CODE2 (U"\\#{sentence}: “lo”, “two words”")
		CODE2 (U"\\#{text}: “shortText”, “some one-line text here”")
		CODE2 (U"\\#{text}: 2, “longText”, “some scrollable text here, within a height of 2 lines”")
		CODE2 (U"\\#{boolean}: “You like it?”, 1")
		CODE2 (U"\\#{infile}: “Input file”, “hello.wav”")
		CODE2 (U"\\#{outfile}: 2, “Output file”, “../out.txt”")
		CODE2 (U"\\#{realvector}: 2, “Array of reals”, “(whitespace-separated)”, “20 80 60”")
		CODE2 (U"if worth < 6")
			CODE3 (U"\\#{choice}: “Compression”, compression")
				CODE4 (U"\\#{option}: “lossless (FLAC)”")
				CODE4 (U"\\#{option}: “MP3”")
				CODE4 (U"\\#{option}: “Ogg”")
		CODE2 (U"endif")
		CODE2 (U"\\#{optionmenu}: “Number of channels”, number_of_channels")
			CODE3 (U"\\#{option}: “mono”")
			CODE3 (U"\\#{option}: “stereo”")
			CODE3 (U"\\#{option}: “quadro”")
		CODE2 (U"\\#{comment}: “Then click Stop or one of the continuation buttons.”")
	CODE1 (U"clicked = \\#{endPause}: “Continue”, “Next”, “Proceed”, 2")
	CODE1 (U"appendInfoLine: number_of_people, “ ”, worth, “ ”, sampling_frequency, “ ”, clicked")
	CODE1 (U"appendInfoLine: “Compression: ”, compression, “ (”, compression$, “)”")
	CODE1 (U"appendInfoLine: “Number of channels: ”, number_of_channels$")
	CODE1 (U"appendInfoLine: “Short text: ”, shortText$")
	CODE1 (U"appendInfoLine: “Input file: ”, input_file$")
	CODE1 (U"appendInfoLine: “Array: ”, array_of_reals#")
CODE (U"endfor")
NORMAL (U"This example uses several tricks. A useful one is seen with `number_of_channels`: "
	"this is at the same time the value that is passed to #`optionmenu` (and therefore determines the setting of "
	"the ##Number of channels# menu when the window appears) and the name of the variable in which the user’s "
	"chosen value of ##Number of channels# is stored (because the text “number\\_ of \\_ channels” is what you get "
	"by replacing the spaces in “Number of channels” with underscores and turning its first letter to lower case).")
NORMAL (U"Your own pause windows are not likely to be as rich as the above example. "
	"For instance, the example has three continuation buttons (the second of these is the default button, "
	"i.e. the button that you can “click” by pressing the Enter or Return key). "
	"You will often use only one continuation button, for instance")
CODE (U"\\#{endPause}: “Continue”, 1")
NORMAL (U"or")
CODE (U"\\#{endPause}: “Finish”, 1")
NORMAL (U"or")
CODE (U"\\#{endPause}: “OK”, 1")
NORMAL (U"If your script shows multiple different pause windows, then it is in fact a %wizard, "
	"and it becomes useful to have")
CODE (U"\\#{endPause}: “Next”, 1")
NORMAL (U"for most of them, and")
CODE (U"\\#{endPause}: “Finish”, 1")
NORMAL (U"for the last one.")
NORMAL (U"The possibility of multiple continuation buttons can save the user a mouse click. "
	"The following script, for instance, requires two mouse clicks per sound:")
CODE (U"for %i to 20")
	CODE1 (U"Read from file: “sound“ + string$ (i) + “.wav”")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"#beginPause: “Rate the quality”")
		CODE2 (U"#comment: “How good is the sound on a scale from 1 to 7?”")
		CODE2 (U"#choice: “Quality”, 4")
			CODE3 (U"#option: “1”")
			CODE3 (U"#option: “2”")
			CODE3 (U"#option: “3”")
			CODE3 (U"#option: “4”")
			CODE3 (U"#option: “5”")
			CODE3 (U"#option: “6”")
			CODE3 (U"#option: “7”")
	CODE1 (U"\\#{endPause}: if i = 20 then “Finish” else “Next” fi, 1")
	CODE1 (U"appendInfoLine: quality")
CODE (U"endfor")
NORMAL (U"The following script works faster:")
CODE (U"for i to 20")
	CODE1 (U"Read from file: “sound” + string$ (i) + “.wav”")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"#beginPause: “Rate the quality\"")
		CODE2 (U"#comment: “How good is the sound on a scale from 1 to 7?”")
	CODE1 (U"quality = \\#{endPause}: “1”, “2”, “3”, “4”, “5”, “6”, “7”, 0")
	CODE1 (U"appendInfoLine: quality")
CODE (U"endfor")
NORMAL (U"In this example, the 0 at the end of #endPause means that there is no default button.")
ENTRY (U"File selection")
NORMAL (U"If you want the user to choose a file name for reading (opening), do")
CODE (U"fileName$ = \\#{chooseReadFile$}: “Open a table file\"")
CODE (U"if fileName$ <> “”")
	CODE1 (U"table = Read Table from tab-separated file: fileName$")
CODE (U"endif")
NORMAL (U"A file selector window will appear, with (in this example) ##Open a table file# as the title. "
	"If the user clicks #OK, the variable %`fileName$` will contain the name of the file that the user selected; "
	"if the user clicks #Cancel, the variable %`fileName$` will contain the empty string (“”).")
NORMAL (U"If you want the user to choose a file name for writing (saving), do")
CODE (U"selectObject: mySound")
CODE (U"fileName$ = \\#{chooseWriteFile$}: “Save as a WAV file”, “mySound.wav”")
CODE (U"if fileName$ <> “”")
	CODE1 (U"Save as WAV file: fileName$")
CODE (U"endif")
NORMAL (U"A file selector window will appear, with (in this example) ##Save as a WAV file# as the title "
	"and “mySound.wav” as the suggested file name (which the user can change). "
	"If the user clicks #OK, the form will ask for confirmation if the file name that the user typed already exists. "
	"If the user clicks #OK with a new file name, or clicks #OK in the confirmation window, "
	"the variable %`fileName$` will contain the file name that the user typed; "
	"if the user clicks #Cancel at any point, the variable %`fileName$` will contain the empty string (“”).")
NORMAL (U"If you want the user to choose a folder (directory) name, do")
CODE (U"folderName$ = \\#{chooseFolder$}: “Choose a folder to save all the new files in”")
CODE (U"if folderName$ <> “”")
	CODE1 (U"for i to numberOfSelectedSounds")
		CODE2 (U"selectObject: sound [i]")
		CODE2 (U"Save as WAV file: folderName$ + “/sound” + string$ (i) + “.wav”")
	CODE1 (U"endfor")
CODE (U"endif")
NORMAL (U"A folder selector window will appear, with (in this example) ##Choose a folder to save all the new files in# as the title. "
	"If the user clicks #OK, the variable %`folderName$` will contain the name of the folder that the user selected; "
	"if the user clicks #Cancel, the variable %`folderName$` will contain the empty string (“”).")
ENTRY (U"A non-pausing pause window without a #Stop button")
NORMAL (U"Especially if you use the pause window within the @@Demo window@, you may not want to give the user the capability of "
	"ending the script by hitting #Stop or closing the pause window. "
	"In that case, you can add an extra numeric argument to #`endPause` (at the end) "
	"that denotes the cancel button:")
CODE (U"\\#{beginPause}: “Learning settings”")
	CODE1 (U"\\#{positive}: “Learning rate”, “0.01”")
	CODE1 (U"\\#{choice}: “Directions”, 3")
		CODE2 (U"\\#{option}: “Forward”")
		CODE2 (U"\\#{option}: “Backward”")
		CODE2 (U"\\#{option}: “Bidirectional”")
CODE (U"clicked = \\#{endPause}: “Cancel”, “OK”, 2, 1")
CODE (U"if clicked = 2")
	CODE1 (U"learningRate = learning_rate")
	CODE1 (U"includeForward = ( directions = 1 or directions = 3 )")
	CODE1 (U"includeBackward = ( directions = 2 or directions = 3 )")
CODE (U"endif")
NORMAL (U"In this example, the default button is 2 (i.e. #OK), and the cancel button is 1 (i.e. #Cancel). "
	"The form will now contain no #Stop button, and if the user closes the window, "
	"this will be the same as clicking #Cancel, namely that %`clicked` will be 1 (because the #Cancel button is the first button) "
	"and the variables %`learning_rate`, %`directions` and %`directions$` will not be changed (i.e. they might remain undefined).")
ENTRY (U"Pausing for a fixed time without a window")
NORMAL (U"You can pause Praat for 1.3 seconds by saying")
CODE (U"\\#{sleep} (1.3)")
NORMAL (U"This is of course not about controlling the user, "
	"but it is mentioned here because this section is about pausing.")
MAN_END

MAN_BEGIN (U"Scripting 6.7. Sending a message to another program", U"ppgb", 20241116)
NORMAL (U"To send messages to running programs that use the Praat shell, "
	"use $sendpraat (see @@Scripting 8. Controlling Praat from another program@).")
ENTRY (U"Security measure")
NORMAL (U"One some platforms it used to be possible to send a message to another running program that listens to a socket, "
	"using the $sendsocket directive. This is no longer possible in Praat version 7.")
MAN_END

MAN_BEGIN (U"Scripting 6.8. Messages to the user", U"ppgb", 20230122)
NORMAL (U"If the user makes a mistake (e.g. types conflicting settings into your form window), "
	"you can use the #exitScript function (@@Scripting 5.9. Quitting|\\SS5.9@) "
	"to stop the execution of the script with an error message:")
CODE (U"form: “My analysis”")
	CODE1 (U"real: “Starting time (s)”, “0.0”")
	CODE1 (U"real: “Finishing time (s)”, “1.0”")
CODE (U"endform")
CODE (U"if finishing_time <= starting_time")
	CODE1 (U"\\#{exitScript}: “The finishing time should exceed ”, starting_time, “ seconds.”")
CODE (U"endif")
CODE (U"# Proceed with the analysis...")
NORMAL (U"For things that should not normally go wrong, you can use the #`assert` directive:")
CODE (U"power = Get power")
CODE (U"assert power > 0")
NORMAL (U"This is the same as:")
CODE (U"if (power > 0) = undefined")
	CODE1 (U"exitScript: “Assertion failed in line ”, lineNumber, “ (undefined): power > 0”")
CODE (U"elsif not (power > 0)")
	CODE1 (U"exitScript: “Assertion failed in line ”, lineNumber, “ (false): power > 0”")
CODE (U"endif")
NORMAL (U"You can prevent Praat from issuing warning messages:")
CODE (U"nowarn Save as WAV file: “hello.wav”")
NORMAL (U"This prevents warning messages about clipped samples, for instance.")
NORMAL (U"You can also prevent Praat from showing a progress window:")
CODE (U"pitch = noprogress To Pitch: 0, 75, 500")
NORMAL (U"This prevents the progress window from popping up during lengthy operations. "
	"Use this only if you want to prevent the user from stopping the execution of the script.")
NORMAL (U"Finally, you can make Praat ignore error messages:")
CODE (U"nocheck Remove")
NORMAL (U"This would cause the script to continue even if there is nothing to remove.")
MAN_END

MAN_BEGIN (U"Scripting 6.9. Calling from the command line", U"ppgb", 20220122)   // 2023 2025
INTRO (U"Previous sections of this tutorial have shown you how to run a Praat script from the Script window. "
	"However, you can also call a Praat script from the command line (text console) instead. "
	"Information that would normally show up in the Info window, then goes to %stdout, "
	"and error messages go to %stderr. "
	"You cannot use commands in your script that create windows, such as ##View & Edit#. "
	"Before describing how to achieve this (from section 4 below on), we first describe "
	"how the normal Praat, with its usual Objects and Picture (and perhaps Info) window, "
	"can be started from the command line.")

ENTRY (U"1. Starting Praat from the command line")
NORMAL (U"Before seeing how a Praat script can be called from the command line, "
	"you should first know that just calling Praat from the command line just starts up Praat "
	"with its usual GUI (Graphical User Interface), i.e. with its two windows. "
	"For instance, on Windows you can start the Command Prompt window (the “Console”), and type")
CODE (U"\"C:\\Program Files\\Praat.exe\"")
NORMAL (U"(including the quotes) if Praat.exe is indeed in the folder `C:\\Program Files`.")
NORMAL (U"On the Mac, the executable is hidden inside the `app` file, so you open a Terminal window "
	"and type something like")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat")
NORMAL (U"On Linux, you type into the Terminal something like")
CODE (U"/usr/bin/praat")

ENTRY (U"2. Calling Praat to open data files")
NORMAL (U"On Windows, you can open Praat with a sound file and a TextGrid file by typing")
CODE (U"\"C:\\Program Files\\Praat.exe\" --open data\\hello.wav data\\hello.TextGrid")
NORMAL (U"or")
CODE (U"\"C:\\Program Files\\Praat.exe\" --open data/hello.wav data/hello.TextGrid")
NORMAL (U"at least if your current folder (see the Console’s `cd` and `dir` commands) "
	"contains the folder `data` and that folder contains those two files. "
	"Praat will start up, and shows the two files as a Sound and a TextGrid object in the list. "
	"If Praat was already running when you typed the command, "
	"the two files are added as objects to the existing list in Praat.")
NORMAL (U"On the Mac, you do")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --open data/hello.wav data/hello.TextGrid")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --open data/hello.wav data/hello.TextGrid")
NORMAL (U"Again, if Praat was already running when you typed the command, "
	"the two files are added as objects to the existing list in Praat.")
NORMAL (U"Note that if you want to send messages or files to a running Praat, "
	"the best way (on all platforms) is to use `praat --send` (see below) "
	"or %sendpraat (see @@Scripting 8. Controlling Praat from another program@).")
NORMAL (U"To always start up a new instance of Praat, use #`--new-open` instead of `--open`.")

ENTRY (U"3. Calling Praat to open a script")
NORMAL (U"On Windows, when you type")
CODE (U"\"C:\\Program Files\\Praat.exe\" --open \"my script.praat\"")
NORMAL (U"Praat will start up, opening the script `my script.praat` in a script window. "
	"If Praat was already running when you typed the command, "
	"the script window will appear within the already running instantiation of Praat.")
NORMAL (U"On the Mac, you do")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --open \"my script.praat\"")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --open \"my script.praat\"")
NORMAL (U"Note that on all three platforms, you have to supply quotes around the file name "
	"if that file name contains one or more spaces, as here between `my` and `script` "
	"or above between `Program` and `Files`. This is because the script languages of "
	"the Console or Terminal use spaces for separating commands and arguments.")

ENTRY (U"4. Calling Praat to run a script in the background")
NORMAL (U"Now we are ready to discuss how to run Praat without a GUI.")
NORMAL (U"On Windows, when you type")
CODE (U"\"C:\\Program Files\\Praat.exe\" --run \"my script.praat\"")
NORMAL (U"Praat will execute the script `my script.praat` without showing Praat's GUI, "
	"i.e. without showing its usual two windows. "
	"In fact, any output that would normally go to the Info window, "
	"will now go directly to the Console window in which you typed the command. "
	"If Praat was already running when you typed the command, "
	"its windows will not be affected. In fact, the GUI-instantiation of Praat and the Console-instantiation "
	"can run simultaneously without them noticing each other’s existence; "
	"moreover, multiple Console-instantiations of Praat can run simultaneously, each in their own Console.")
NORMAL (U"On the Mac, you type")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --run \"my script.praat\"")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --run \"my script.praat\"")
NORMAL (U"What happens on all platforms is that the Console or Terminal starts up Praat, "
	"then Praat executes the script, and then Praat closes itself.")

ENTRY (U"5. Calling Praat to run a script with arguments")
NORMAL (U"Consider the following script:")
CODE (U"form: \"Test command line calls\"")
CODE1 (U"sentence: \"First text\", \"I love you\"")
CODE1 (U"real: \"Beep duration\", \"0.4\"")
CODE1 (U"sentence: \"Second text\", \"Me too\"")
CODE (U"endform")
CODE (U"")
CODE (U"writeInfoLine: \"She: \"\"\", first_text$, \"\"\"\"")
CODE (U"appendInfoLine: \"He: \"\"\", second_text$, \"\"\"\"")
CODE (U"")
CODE (U"synth1 = Create SpeechSynthesizer: \"English (Great Britain)\", \"Female1\"")
CODE (U"Play text: first_text$")
CODE (U"Create Sound as pure tone: \"beep\", 1, 0.0, beep_duration,")
CODE (U"... 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"synth2 = Create SpeechSynthesizer: \"English (America)\", \"Male1\"")
CODE (U"Play text: second_text$")
NORMAL (U"When you run this script from within Praat, it writes two lines to the Info window "
	"and plays first a female voice speaking the first sentence, then a beep, and then a male voice "
	"speaking the second sentence. To make this happen from the Windows command line instead, you type")
CODE (U"\"C:\\Program Files\\Praat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"In the Mac terminal, you type")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"and in the Linux terminal, you do")
CODE (U"/usr/bin/praat --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"Note that each argument that contains one or more spaces has to be put within quotes, "
	"on all three platforms. As with #runScript, Praat will not present a form window, "
	"but simply run the script with the arguments given on the command line "
	"(see @@Scripting 6.1. Arguments to the script@). What then happens on all three platforms is that a console instantiation of Praat writes "
	"the two lines to the Console window and plays the three sounds.")
NORMAL (U"The path to the script file as well as to `infile`, `outfile` and `folder` "
	"arguments will be taken relative to the current working directory of the terminal window. For instance, "
	"the following example from @@Scripting 6.1. Arguments to the script@ will run the script "
	"`/Users/miep/research/usefulScripts/playFile.praat`, which will play the sound file "
	"`/Users/miep/research/project19/sounds/sound3.wav`:")
CODE (U"cd /Users/miep/research/project19")
CODE (U"/usr/bin/praat --run ../usefulScripts/playFile.praat sounds/sound3.wav")

ENTRY (U"6. Calling Praat to run a script in the GUI")
NORMAL (U"You can send a script to a running Praat. Praat will then execute it:")
CODE (U"\"C:\\Program Files\\Praat.exe\" --send testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --send testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
CODE (U"/usr/bin/praat --send testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"This works the same way as `--run`, except that `--send` runs in Praat’s Graphical User Interface. "
	"If Praat is already running, then that instance of Praat will execute your script. "
	"If Praat is not running yet, then a new GUI instance of Praat will start up and execute your script. "
	"To always start up a new instance of Praat, use `--new-send` instead of `--send`.")
NORMAL (U"If you use")
CODE (U"\"C:\\Program Files\\Praat.exe\" --send-or-form testCommandLineCalls.praat")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --send-or-form testCommandLineCalls.praat")
CODE (U"/usr/bin/praat --send-or-form testCommandLineCalls.praat")
NORMAL (U"then Praat will present the above form that asks for the three arguments "
	"(if you use `--send-or-form` with a script that has no `form` in it, it will work the same as `--send`). "
	"This switch is useful if you want to implement a Praat connection for interfacing with an IDE such as Microsoft\\re Visual Studio Code\\tm. "
	"To always start up a new instance of Praat, use `--new-send-or-form` instead of `--send-or-form`.")
NORMAL (U"See also %sendpraat (see @@Scripting 8. Controlling Praat from another program@).")

ENTRY (U"7. Calling Praat from other programs such as Python")
NORMAL (U"You can run the above script from several programming languages, not just from a Console or Terminal. "
	"In Python, for instance, you can do it using the same syntax as you would use in the Console or Terminal:")
CODE (U"import os")
CODE (U"os.system ('\"C:\\\\Program Files\\\\Praat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"')")
NORMAL (U"Note that you have to double the backslashes!")
NORMAL (U"A disadvantage of the `os.system` method is that you have to use quotes within quotes. "
	"A somewhat cleaner approach is:")
CODE (U"import subprocess")
CODE (U"subprocess.call(['C:\\\\Program Files\\\\Praat.exe', '--run', 'testCommandLineCalls.praat', 'I love you', '0.4', 'Me too'])")
NORMAL (U"This way you specify the arguments directly, with quotes only because they are all strings, "
	"but without having to worry about spaces. And perhaps even more importantly, this syntax "
	"makes it easy to use variables as arguments, as in:")
CODE (U"first_line = 'I love you'")
CODE (U"second_line = 'me too'")
CODE (U"subprocess.call(['C:\\\\Program Files\\\\Praat.exe', '--run', 'testCommandLineCalls.praat', first_line, '0.4', second_line])")
NORMAL (U"Many other programs beside Python have a `system`-like command, so that you can run a command like")
CODE (U"system ('\"C:\\\\Program Files\\\\Praat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"')")

ENTRY (U"8. What happens if I specify neither --open nor --run nor --send?")
NORMAL (U"If you specify neither `--open` nor `--run` nor `--send`, Praat's behaviour is not guaranteed. "
	"If you type something like")
CODE (U"praat testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"into a Console or Terminal window by hand, Praat will typically run the script. "
	"Also, the `--run` option can probably be left out from the Python call above. "
	"However, if you redirect the output of Praat to a file or pipe, you cannot typically leave out the `--run` option; "
	"if you do, Praat may start its GUI and %open the file rather than run it.")
NORMAL (U"“Why this strange behaviour?”, you may ask. The reason is that several platforms (e.g. Windows) "
	"use this same mechanismm when you double-click a file, or drop a file on an application icon. "
	"For instance, when you double-click `hello.wav` or `doSomething.praat`, "
	"or drop `hello.wav` or `doSomething.praat` on the Praat icon, "
	"you expect Praat to %open that sound file or script, not to %run it. "
	"However, when you double-click or drop `doSomething.praat`, "
	"Windows will send Praat a message %%as if% you had typed `praat doSomething.praat` into a Console window. "
	"This means that in this case Praat will have to interpret `praat doSomething.praat` "
	"as an indication that you want to %open that file, not %run it. "
	"Praat will try to be a bit smart, though: when receiving the message `praat doSomething.praat` from the operating system, "
	"Praat will try to figure out whether you meant to %run or %open the file. "
	"If Praat detects that you typed `praat doSomething.praat` into the Console "
	"by hand, Praat will %run this script; if not, then Praat will %open this script; "
	"this includes double-clicking and dropping (appropriately), but may also include some indirect invocations "
	"such as through a shell script or redirection or pipe. "
	"After all, if Praat is in doubt, wrongly assuming that you want to %open the script is safer than wrongly "
	"assuming that you want to %run the script.")

ENTRY (U"9. Running Praat interactively from the command line")
NORMAL (U"On the Mac and Linux, you have the possibility of running the program interactively from the command line:")
CODE (U"> /usr/bin/praat -")
NORMAL (U"You can then type in any of the fixed and dynamic commands, and commands that handle object selection, "
	"such as #selectObject. This method also works in pipes:")
CODE (U"> echo \"Report memory use\" | /usr/bin/praat -")

ENTRY (U"10. Calling Praat from a web server")
NORMAL (U"If you call Praat from a web server, you typically do not want to read and write its preferences and buttons files. "
	"To achieve this, you use the #`--no-pref-files` command line option before the script name:")
CODE (U"system ('/users/apache/praat --run --no-pref-files /user/apache/scripts/computeAnalysis.praat 1234 blibla')")
NORMAL (U"On Windows, you will often want to specify #`--utf8` as well, because otherwise "
	"Praat will write its output to BOM-less UTF-16 files, which many programs do not understand.")

ENTRY (U"11. All command line switches and options")
NORMAL (U"Switches:")
TERM (U"##--open")
DEFINITION (U"Interpret the command line arguments as files to be opened in an existing or new GUI.")
TERM (U"##--new-open")
DEFINITION (U"Start a new GUI and interpret the command line arguments as files to be opened.")
TERM (U"##--run")
DEFINITION (U"Interpret the command line arguments as a script file name and its arguments, to run without a GUI.")
TERM (U"##--send")
DEFINITION (U"Interpret the command line arguments as a script file name and its arguments, to run in an existing or new GUI.")
TERM (U"##--new-send")
DEFINITION (U"Start a new GUI and interpret the command line arguments as a script file name and its arguments.")
TERM (U"##--version")
DEFINITION (U"Print the Praat version.")
TERM (U"##--help")
DEFINITION (U"Print this list of command line options.")
NORMAL (U"Options:")
TERM (U"##--no-pref-files#")
DEFINITION (U"Ignore the preferences file and the buttons file at start-up, and don't write them when quitting (see above).")
TERM (U"##--no-plugins#")
DEFINITION (U"Don't activate the plugins at start-up.")
TERM (U"##--pref-dir=#`/var/www/praat_plugins`")
DEFINITION (U"Set the preferences folder to `/var/www/praat_plugins` (for instance). "
	"This can come in handy if you require access to preference files and/or plugins that are not in your home folder.")
TERM (U"##-8#, ##--utf8#")
DEFINITION (U"Write the output (e.g. of $writeInfo$) in UTF-8 encoding. This is the default encoding on MacOS and Linux, "
	"but on Windows the default is the Console’s native UTF-16 Little Endian (i.e. the Console understands UTF-16 always, "
	"whereas it understands UTF-8 only if you type `chcp 65001` first). "
	"If you pipe to Windows programs that understand UTF-8 rather than UTF-16, "
	"or if you want to redirect the output to a UTF-8 file, use this option.")
TERM (U"##-a#, ##--ansi#")
DEFINITION (U"Write the output (e.g. of `writeInfo`) in ISO-Latin 1 (\"ANSI\") encoding. "
	"This is not recommended, because it potentially loses information (characters above U+00FF will show up as \"?\"), "
	"but it might be necessary if you want to use Praat in a pipe with programs "
	"that do understand ANSI but do not understand UTF-8 or UTF-16, "
	"or if you want to redirect the output to an ANSI-encoded file.")
TERM (U"##-u#, ##--utf16#")
DEFINITION (U"Write the output (e.g. of `writeInfo`) in UTF-16 Little Endian encoding, without Byte Order Mark. "
	"This format is the default on Windows, "
	"but you can use it to write the output to a UTF-16LE-encoded file on any platform.")
TERM (U"##--trace#")
DEFINITION (U"Switch tracing on at start-up.")
TERM (U"##--hide-picture#")
DEFINITION (U"Hide the Picture window at start-up.")
MAN_END

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Scripting 7. Scripting the editors"
© Paul Boersma 2004,2023

With a Praat script, you can automate your work in the editors.

#Warning: if the purpose of your script is to get information about
analyses (pitch, formants, intensity, spectrogram) from the Sound,
we do %not advise to script the Sound editor window.
It is much simpler, faster, and more reproducible to create the analyses
with the commands of the @@dynamic menu@, then use the Query commands of the dynamic menu
to extract information from the analyses. This also applies if you want to use a TextGrid
to determine the times at which you want to query the analyses.
See @@Scripting examples@.

, @@Scripting 7.1. Scripting an editor from a shell script@ (editor/endeditor)
, @@Scripting 7.2. Scripting an editor from within@

################################################################################
"Scripting 7.1. Scripting an editor from a shell script"
© Paul Boersma 20140526

From a Praat shell script, you can switch to an editor and back again:
{;
	sound$ = "hallo"
	start = 0.3
	finish = 0.7
	sound = \@{Read from file:} sound$ + ".aifc"
	\@{View & Edit}
	\#`{editor}: sound
		Zoom: start, finish
		\#`{endeditor}
	Play
}
This script reads a sound file from disk, pops up an editor for the resulting object,
makes this editor zoom in on the part between 0.3 and 0.7 seconds,
and returns to the Praat shell to play the entire sound.

After @`editor` you can either give the unique id of the object, as above, or its name:
{;
	\#`{editor}: "Sound " + sound$
}
################################################################################
"Scripting 7.2. Scripting an editor from within"
© Paul Boersma 2023

This section shows how you can permanently extend the functionality of an editor.

As an example, consider the following problem: you want to see a graphic representation
of the spectrum of the sound around the cursor position in the SoundEditor. To achieve this,
follow these steps:

1. Create a Sound.
2. View it in a SoundEditor by clicking @@View & Edit@.
3. Choose ##New editor script# from the @@File menu@ in the SoundEditor. The resulting @ScriptEditor
	will have a name like “untitled script [Sound hallo]”.
4. Type the following lines into the ScriptEditor:
{;
			cursor = Get cursor
			Select: cursor - 0.02, cursor + 0.02
			Extract selected sound (windowed): "slice", "Kaiser2", 2, "no"
		\#`{endeditor}
		To Spectrum: "yes"
		\@{View & Edit}
}
If you choose #Run from the #Run menu in the ScriptEditor, a region of 40 milliseconds around the
current cursor position in the SoundEditor will become selected. This piece will be copied to the list of objects,
after applying a double Kaiser window (total length 80 ms). Thus, a Sound named “slice” will appear in the list.
Subsequently, a Spectrum object also called “slice” will appear in the list, and a SpectrumEditor titled
“Spectrum slice” will finally appear on your screen.

5. Save the script to disk, e.g. as `/us/miep/spectrum.praat`. The title of the ScriptEditor will change accordingly.
6. Since you will want this script to be available in all future SoundEditors, you choose ##Add to menu...# from the #File menu.
	For the %Window, you specify “SoundEditor” (this is preset). For the %Menu, you may want to choose “Spectrum”
	instead of the preset value (“File”). For the name of the %Command, you type something like “Show spectrum at cursor”
	(instead of “Do it...”). Then you click #OK.

The command will be visible in every SoundEditor that you create from now on.
To see this, close the one visible SoundEditor, select the original Sound, choose ##View & Edit# again, and inspect the #Spectrogram menu.
You can now view the spectrum around the cursor just by choosing this menu command.

After you leave Praat and start it again, the command will continue to appear in the SoundEditor.
If you don't like the command any longer, you can remove it with the @ButtonEditor, which you can start
by choosing #Buttons from the #Settings submenu of the @@Praat menu@.

Improving your script
=====================
The above spectrum-viewing example has a number of disadvantages. It clutters the object list with a number
of indiscriminable Sounds and Spectra called "slice", and the spectrum is shown up to the Nyquist frequency
while we may just be interested in the lower 5000 Hz. Furthermore, the original selection in the SoundEditor is lost.

To improve the script, we open it again with ##Open editor script...# from the #File menu in the SoundEditor. After every change,
we can run it with #Run from the #Run menu again; alternatively, we could save it (with #Save from the #File menu) and choose our new
##Show spectrum at cursor# button (this button will always run the version on disk, never the one viewed in a ScriptEditor).

To zoom in on the first 5000 Hz, we add the following code at the end of our script:
{;
	\#`{editor}: "Spectrum slice"
		Zoom: 0, 5000
}
To get rid of the “Sound slice”, we can add:
{;
	\#`{endeditor}
	\`{removeObject}: "Sound slice"
}
Note that @`endeditor` is needed to change from the environment of a SpectrumEditor to the environment of the object & picture windows.

If you now choose the ##Show spectrum at cursor# button for several cursor positions, you will notice that all those editors have the same name.
To remedy the ambiguity of the line `editor Spectrum slice`, we give each slice a better name. For example, if the cursor was at
635 milliseconds, the slice could be named “635ms”. We can achieve this by changing the extraction in the following way:
{;
	milliseconds = round (cursor*1000)
	Extract selection sound (windowed): string$ (milliseconds) + "ms", "Kaiser2", 2, "no"
}
The names of the Sound and Spectrum objects will now have more chance of being unique. Two lines will have to be edited trivially.

Finally, we will reset the selection to the original. At the top of the script,
we add two lines to remember the positions of the selection markers:
{;
	start = Get start of selection
	end = Get end of selection
}
At the bottom, we reset the selection:
{;
	\#`{editor}
		Select: start, end
}
Note that the #`editor` directive if not followed by the name of an editor,
returns the script to the original environment.

The complete script is:
{;
		start = Get start of selection
		end = Get end of selection
		cursor = Get cursor
		Select: cursor - 0.02, cursor + 0.02
		# Create a name. E.g. "670ms" means at 670 milliseconds.
		milliseconds = round (cursor*1000)
		Extract windowed selection: string$ (milliseconds) + "ms", "Kaiser2", 2, "no"
	\#`{endeditor}
	To Spectrum: "yes"
	View & Edit
	\#`{editor}: "Spectrum " + string$ (milliseconds) + "ms"
		Zoom: 0, 5000
	\#`{endeditor}
	\`{removeObject}: "Sound " + string$ (milliseconds) + "ms"
	\#`{editor}
		Select: start, end
}
This script is useful as it stands. It is good enough for safe use. For instance, if the created Sound object has the same name
as an already existing Sound object, it will be the newly created Sound object that will be removed by @`removeObject`,
because in case of ambiguity @`removeObject` always removes the most recently created object of that name.

################################################################################
"sendpraat"
© Paul Boersma #1997,2000

See @@Scripting 8. Controlling Praat from another program@.

################################################################################
"Scripting 8. Controlling Praat from another program"
© Paul Boersma 2000,2002,2021,2022

Sendpraat is a function for sending messages to a %running Praat.
It is also a Windows, MacOS, or Linux console program with the same purpose.

As sendpraat cannot start up a new instance of Praat, you may often want to use
`praat --send` instead (see @@Scripting 6.9. Calling from the command line@).

, @@Scripting 8.1. The sendpraat subroutine@
, @@Scripting 8.2. The sendpraat program@

################################################################################
"Scripting 8.1. The sendpraat subroutine"
© Paul Boersma 2002,2003,2005,2009,2014,2015,2021,2023,2026

Sendpraat can be a subroutine for sending messages to a %running Praat program.

C syntax
========
{; C
	\#{sendpraat7} (long \%{timeOut}, int fullTrust, const char *\%{program}, char *\%{text});
}
Arguments
=========
%`timeOut` (MacOS and Linux only)
: the number of seconds that `sendpraat` will wait for an answer
  before writing an error message. A %`timeOut` of 0 means that
  the message will be sent asynchronously, i.e., that sendpraat
  will return immediately without issuing any error message.

%`fullTrust`
: a boolean (0 or 1) that determines whether the script should be allowed
  to save files and call system commands (1), or not (0).

%`program`
: the name of a running program that uses the Praat shell, e.g. "Praat".
  The first letter may be specified as lower or upper case; it will be converted
  to upper case for Windows or MacOS and to lower case for Linux.

%`text`
: the script text to be sent. Sendpraat may alter this text!

Example 1: killing a program
============================
{; C
	char message [100], *errorMessage;
	strcpy (message, "Quit");
	errorMessage = \#{sendpraat7} (0, 0, "praat", message);
	if (errorMessage) fprintf (stderr, "%s", errorMessage);
}
This causes the program #Praat to quit (gracefully), because #Quit is a fixed
command in one of the menus of that program.
On MacOS and Linux, sendpraat returns immediately; on Windows, the %timeOut argument is ignored.
The return value %errorMessage is a statically allocated string internal to sendpraat,
and is overwritten by the next call to sendpraat.

Example 2: playing a sound file in reverse
==========================================
Suppose you have a sound file whose name is in the variable %`fileName`,
and you want the program #Praat, which can play sounds,
to play this sound backwards.
{; C
	char message [1000], *errorMessage;
	snprintf (message,1000, "Read from file: ~%s\nPlay reverse\nRemove", fileName);
	errorMessage = \#{sendpraat7} (3000, 0, "praat", message);
}
This will work because ##Play reverse# is an action command
that becomes available in the dynamic menu when a Sound is selected.
On Linux, sendpraat will allow #Praat at most 3000 seconds to perform this.

Example 3: executing a large script file
========================================
Sometimes, it may be unpractical to send a large script directly to #sendpraat.
Fortunately, the receiving program knows #runScript:
{; C
	char message [100], *errorMessage;
	strcpy (message, "runScript: \"doAll.praat\", 20");
	errorMessage = \#{sendpraat7} (0, 1, "praat", message);
}
This causes the program #Praat to run the script `doAll.praat` with an argument of "20".

How to download
===============
You can download the source code of the sendpraat subroutine
via ##https://praat.org/sendpraat.html# or from ##https://www.fon.hum.uva.nl/praat/sendpraat.html#.

Instead
=======
Instead of using `sendpraat`, you can also just take the following simple steps in your program:
1. on Linux, write the Praat script that you want to run, and save it as `~/.config/Message.txt`;
2. get Praat's process id from `~/.config/pid`;
3. if Praat's process id is e.g. 1178, send it a SIGUSR1 signal: `kill -USR1 1178`

If the first line of your script is the comment “`# 999`”, where 999 stands for the process id of your program,
Praat will send your program a SIGUSR2 signal back when it finishes handling the script.
If you do not want to receive such a message (if your program has no handler for it, the SIGUSR2 signal will kill your program),
then do not include such a line.

If the second line of your script is “`# --FULL-TRUST`”

See also
========
To start a program from the command line instead and sending it a message,
you would not use #sendpraat, but instead run the program with a script file as an argument.
See @@Scripting 6.9. Calling from the command line@.

################################################################################
"Scripting 8.2. The sendpraat program"
© Paul Boersma #1997,2000,2003,2005,2014,2015,2021,2023,2026

Sendpraat can be a Windows console or Unix (MacOS, Linux) terminal program for sending messages to a %running Praat program.

Command line syntax
===================
{; sh
	\#{sendpraat7} [\%{timeOut}] [--FULL-TRUST] \%{program} \%{message} ...
}
For the meaning of the arguments, see @@Scripting 8.1. The sendpraat subroutine|the sendpraat subroutine@.

Example 1: killing a program
============================
{; sh
	\#{sendpraat7} 0 praat Quit
}
Causes the program #Praat to quit (gracefully), because #Quit is a fixed command in one of its menus.
On Unix, `sendpraat` returns immediately; on Windows, you leave out the %`timeOut` argument.

Example 2: playing a sound file in reverse
==========================================
{; sh
	\#{sendpraat7} 1000 praat "Read from file... hello.wav" "Play reverse" "Remove"
}
This works because ##Play reverse# is an action command
that becomes available in the dynamic menu of the #Praat program when a Sound is selected.
On Unix, `sendpraat` will allow #Praat at most 1000 seconds to perform this.

Each line is a separate argument. Lines that contain spaces should be put inside double quotes.

Example 3: drawing
==================
{; sh
	\#{sendpraat7} als "for i from 1 to 5" "Draw circle: 0.5, 0.5, i" "endfor"
}
This causes the program #Als to draw five concentric circles into the Picture window.

Example 4: running a large script
{; sh
	\#{sendpraat7} praat "runScript: \"doAll.praat\", 20"
}
This causes the program #Praat to execute the script ##doAll.praat# with an argument of "20".

How to download
===============
You can download the sendpraat program
via `https://praat.org/sendpraat.html` or from `https://www.fon.hum.uva.nl/praat/sendpraat.html`.

################################################################################
)~~~"
MAN_PAGES_END


/*
ENTRY (U"How to run a script")
NORMAL (U"You can run scripts from the @ScriptEditor. If you will have to use the script very often, "
	"it is advisable to create a button for it in the fixed menu or in a dynamic menu. See the "
	"@ScriptEditor manual page.")
NORMAL (U"(You can also run scripts from the command line. See @@Scripting 6.9. Calling from the command line|\\SS6.9@)")
*/

MAN_BEGIN (U"Scripting 9. Turning a script into a stand-alone program", U"ppgb", 20201229)  // 2025
INTRO (U"You can turn your script into a double-clickable stand-alone program by including it into Praat's #main procedure. "
	"If you want to try this, you should already know how to compile and link the Praat program on your computer.")
NORMAL (U"These stand-alone programs do not show the Objects window and the Picture window; "
	"therefore, you will usually want to use @@Demo window@ commands in your script. Here is an example:")
CODE (U"#include \"praat.h\"")
CODE (U"")
CODE (U"const char32 myScript [ ] = U\"\"")
	CODE1 (U"\"demo Text: 0.5, \\\"centre\\\", 0.5, \\\"half\\\", \\\"Hello world\\\"\\n\"")
	CODE1 (U"\"demoWaitForInput ( )\\n\"")
CODE (U";")
CODE (U"")
CODE (U"int main (int argc, char *argv [ ]) {")
	CODE1 (U"praat_setStandAloneScriptText (myScript);")
	CODE1 (U"praat_init (U\"Hello\", argc, argv);")
	CODE1 (U"INCLUDE_LIBRARY (praat_uvafon_init)")
	CODE1 (U"praat_run ();")
	CODE1 (U"return 0;   // obligatory")
CODE (U"}")
NORMAL (U"The script in this example raises the Demo window, writes “Hello world” in the middle of the window, "
	"waits until the user clicks the mouse or presses a key, and then closes.")
NORMAL (U"Note that Praat is distributed under the General Public License (GPL). This means that if you distribute "
	"a Praat-based stand-alone program, you have to make it open source under the GPL as well (version 3.0 or later).")
NORMAL (U"See also @@Programming with Praat@.")
ENTRY (U"Details")
NORMAL (U"Your program can save its preferences in a folder of its choice, "
	"e.g. in #`preferencesDirectory$ + “/../GuineaPigAnalyzer”` if your program is called GuineaPigAnalyzer. "
	"If you want to be less conspicuous and like to use the Praat preferences folder instead, "
	"please use the #`apps` subfolder, in this way:")
CODE (U"\\`{createFolder}: preferencesDirectory$ + “/apps”")
CODE (U"\\`{createFolder}: preferencesDirectory$ + “/apps/GuineaPigAnalyzer”")
MAN_END

MAN_BEGIN (U"Scripting 10. Old functions", U"ppgb", 20140112)
INTRO (U"The Praat scripting language improves and changes, but old scripts should continue to work correctly. "
	"Here are some examples of what you can see in old scripts, and what they mean:")
NORMAL (U"The meaning of")
CODE (U"echo Hello, my name is 'name$' and I am 'age' years old.")
NORMAL (U"is")
CODE (U"writeInfoLine: \"Hello, my name is \", name$, \" and I am \", age, \" years old.\"")
NORMAL (U"The meaning of")
CODE (U"Draw... 0 0 0 0 yes Curve")
NORMAL (U"is")
CODE (U"Draw: 0, 0, 0, 0, \"yes\", \"Curve\"")
NORMAL (U"The meaning of")
CODE (U"Read from file... 'fileName$'")
NORMAL (U"is")
CODE (U"Read from file: fileName$")
MAN_END

MAN_BEGIN (U"ScriptEditor", U"ppgb", 20140107)
INTRO (U"An aid to @@scripting@.")
NORMAL (U"The ScriptEditor is a text editor that allows you to edit, save, and run "
	"any @@Praat script@. You can type such a script from scratch, "
	"but it is sometimes easier to use the @@History mechanism@, which automatically records "
	"all your commands and mouse clicks, and which can paste these directly "
	"into a ScriptEditor.")
NORMAL (U"To add a script as a button to a fixed or dynamic menu, "
	"use @@Add to fixed menu...@ or @@Add to dynamic menu...@ from the @@File menu@.")
ENTRY (U"Example 1")
NORMAL (U"In this example, we create a fixed button that will play a 0.4-second sine wave with a specified frequency.")
NORMAL (U"First, we create a ScriptEditor by choosing @@New Praat script@ from the @@Praat menu@. "
	"Then, we choose @@Clear history@ from the #Edit menu in the ScriptEditor. "
	"We then perform some actions that will create a sine wave, play it, and remove it:")
LIST_ITEM (U"1. Choose ##Create Sound as pure tone...# from the @@New menu@ and click OK.")
LIST_ITEM (U"2. Click #Play in the dynamic menu.")
LIST_ITEM (U"3. Click the fixed #Remove button.")
NORMAL (U"We then choose @@Paste history@ from the #Edit menu in the ScriptEditor (or type @@Keyboard shortcuts|Command-H@). "
	"The text will now contain at least the following lines (delete any other lines):")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, 0.4, 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"We can run this script again by choosing #Run from the #Run menu (or typing @@Keyboard shortcuts|Command-R@). "
	"However, this always plays a sine with a frequency of 440 Hz, so we will add the variable \"Frequency\" "
	"to the script, which then looks like:")
CODE (U"#form Play a sine wave")
	CODE1 (U"#positive Frequency")
CODE (U"#endform")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, 0.4, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"When we choose #Run, the ScriptEditor will ask us to supply a value for the \"Frequency\" variable. "
	"We can now play 1-second sine waves with any frequency.")
NORMAL (U"It is advisable to supply a standard value for each argument in your script. "
	"If the duration should be variable, too, the final script could look like:")
CODE (U"#form Play a sine wave")
	CODE1 (U"#positive Frequency 440")
	CODE1 (U"#positive Duration 1.0")
CODE (U"#endform")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, duration, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"When you run this script, the ScriptEditor will ask you to supply values for the two variables, "
	"but the values \"440\" and \"1.0\" are already visible in the form window, "
	"so that you will get a sensible result if you just click #OK.")
NORMAL (U"If this script is useful to you, you may want to put a button for it in the @@New menu@, "
	"in the ##Sound# submenu:")
LIST_ITEM (U"1. Save the script to a file, with #Save from the #File menu. The file name that you supply, will "
	"be shown in the title bar of the ScriptEditor window.")
LIST_ITEM (U"2. Choose @@Add to fixed menu...@ from the #File menu. Supply #Objects for the %window, "
	"#New for the %menu, \"Play sine wave...\" for the %command, "
	"##Create Sound from formula...# for %%after command%, and \"1\" for the depth (because it is supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM (U"3. Click #OK and ensure that the button has been added in the @@New menu@. This button will still be there "
	"after you leave the program and enter it again; to remove it from the menu, use the @ButtonEditor.")
ENTRY (U"Example 2")
NORMAL (U"In this example, we will create a shortcut for the usual complex pitch-analysis command.")
NORMAL (U"First, we perform the required actions:")
LIST_ITEM (U"1. Select a Sound object.")
LIST_ITEM (U"2. Click ##To Pitch...# and set the arguments to your personal standard values.")
LIST_ITEM (U"3. Click #OK. A new #Pitch object will appear.")
NORMAL (U"We then paste the history into the ScriptEditor, after which this will contain at least a line like (delete all the other lines):")
CODE (U"To Pitch: 0.01, 150, 900")
NORMAL (U"You can run this script only after selecting one or more Sound objects.")
NORMAL (U"If this script is useful to you, you may want to put a button for it in the dynamic menu:")
LIST_ITEM (U"1. Save the script to a file, with #Save from the #File menu.")
LIST_ITEM (U"2. Choose @@Add to dynamic menu...@ from the #File menu. Supply \"Sound\" for %class1 "
	"(because the button is supposed to be available only if a Sound is selected), \"0\" for %number1 "
	"(because the command is supposed to work for any number of selected Sound objects), "
	"\"To Pitch (child)\" for the %command, "
	"\"To Spectrum\" for %%after command%, and \"0\" for the depth (because it is not supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM (U"3. Click #OK and ensure that the button is clickable if you select one or more Sound objects. "
	"This button will still be available after you leave the program and enter it again; "
	"to remove it from the dynamic menus, use the @ButtonEditor.")
MAN_END

MAN_BEGIN (U"Scripting examples", U"ppgb", 20040222)
INTRO (U"Here is a number of examples of how to use scripting in the Praat program. "
	"Refer to the @scripting tutorial when necessary.")
LIST_ITEM (U"@@Script for listing time–F0 pairs")
LIST_ITEM (U"@@Script for listing time–F0–intensity")
LIST_ITEM (U"@@Script for listing F0 statistics")
LIST_ITEM (U"@@Script for creating a frequency sweep")
LIST_ITEM (U"@@Script for onset detection")
LIST_ITEM (U"@@Script for TextGrid boundary drawing")
LIST_ITEM (U"@@Script for analysing pitch with a TextGrid")
MAN_END

MAN_BEGIN (U"Script for listing time–F0 pairs", U"ppgb", 20140223)
INTRO (U"“I wish to have a list of time markers in one column and F0 in the other. "
	"Those times that have no voiced data should be represented as “.” in the F0 column.”")
CODE (U"writeInfoLine: \"Time:    Pitch:\"")
CODE (U"numberOfFrames = Get number of frames")
CODE (U"for iframe to numberOfFrames")
	CODE1 (U"time = Get time from frame: iframe")
	CODE1 (U"pitch = Get value in frame: iframe, \"Hertz\"")
	CODE1 (U"if pitch = undefined")
		CODE2 (U"appendInfoLine: fixed$ (time, 6)")
	CODE1 (U"else")
		CODE2 (U"appendInfoLine: fixed$ (time, 6), \" \", fixed$ (pitch, 3)")
	CODE1 (U"endif")
CODE (U"endfor")
NORMAL (U"If you want to see this in a text file, you can copy and paste from the Info window, or save the Info window, "
	"or add a line to the script like")
CODE (U"appendFile: \"out.txt\", info$ ( )")
MAN_END

MAN_BEGIN (U"Script for listing time–F0–intensity", U"ppgb", 20200912)
INTRO (U"\"I want a list of pitch and intensity values at the same times.\"")
NORMAL (U"Since @@Sound: To Pitch...@ and @@Sound: To Intensity...@ do not give values at the same times, "
	"you create separate pitch and intensity contours with high time resolution, then interpolate. "
	"In the following example, you get pitch and intensity values at steps of 0.01 seconds "
	"by interpolating curves that have a time resolution of 0.001 seconds.")
CODE (U"sound = selected (\"Sound\")")
CODE (U"tmin = Get start time")
CODE (U"tmax = Get end time")
CODE (U"To Pitch: 0.001, 75, 300")
CODE (U"Rename: \"pitch\"")
CODE (U"selectObject: sound")
CODE (U"To Intensity: 75, 0.001")
CODE (U"Rename: \"intensity\"")
CODE (U"writeInfoLine: \"Here are the results:\"")
CODE (U"for i to (tmax-tmin)/0.01")
	CODE1 (U"time = tmin + i * 0.01")
	CODE1 (U"selectObject: \"Pitch pitch\"")
	CODE1 (U"pitch = Get value at time: time, \"Hertz\", \"linear\"")
	CODE1 (U"selectObject: \"Intensity intensity\"")
	CODE1 (U"intensity = Get value at time: time, \"cubic\"")
	CODE1 (U"appendInfoLine: fixed$ (time, 2), \" \", fixed$ (pitch, 3), \" \", fixed$ (intensity, 3)")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Script for listing F0 statistics", U"ppgb", 20221202)
INTRO (U"\"I need to split the wave into 50 msec sections, and then for each of those sections "
	"get the F0 statistics. That is, for each 50 msec section of speech I want to get the average F0, "
	"min, max, and standard deviation.\"")
NORMAL (U"First you create the complete pitch contour, i.e., you select the Sound and choose "
	"@@Sound: To Pitch...|To Pitch...@. You can then use the commands from the @@Query submenu@ in a loop:")
CODE (U"startTime = Get start time")
CODE (U"endTime = Get end time")
CODE (U"numberOfTimeSteps = (endTime - startTime) / 0.05")
CODE (U"writeInfoLine: \"   tmin     tmax    mean   fmin   fmax  stdev\"")
CODE (U"for step to numberOfTimeSteps")
	CODE1 (U"tmin = startTime + (step - 1) * 0.05")
	CODE1 (U"tmax = tmin + 0.05")
	CODE1 (U"mean = Get mean: tmin, tmax, \"Hertz\"")
	CODE1 (U"minimum = Get minimum: tmin, tmax, \"Hertz\", \"Parabolic\"")
	CODE1 (U"maximum = Get maximum: tmin, tmax, \"Hertz\", \"Parabolic\"")
	CODE1 (U"stdev = Get standard deviation: tmin, tmax, \"Hertz\"")
	CODE1 (U"appendInfoLine: fixed$ (tmin, 6), \" \", fixed$ (tmax, 6), \" \", fixed$ (mean, 2),")
	CODE1 (U"... \" \", fixed$ (minimum, 2), \" \", fixed$ (maximum, 2), \" \", fixed$ (stdev, 2)")
CODE (U"endfor")
ENTRY (U"Notes")
NORMAL (U"One should not cut the sound up into pieces of 50 ms and then do ##To Pitch...# on each of them, "
	"because Praat will not compute F0 values in the first or last 20 ms (or so) of each piece. "
	"This is because the analysis requires a window of 40 ms (or so) for every pitch frame. "
	"Instead, one typically does the analysis on the whole sound, then queries the resulting large Pitch object. "
	"In that way, the information loss of windowing only affects the two 20 ms edges of the whole sound.")
NORMAL (U"The example writes lines to the #Info window. If you want to write to a file instead, "
	"you start with something like")
	CODE1 (U"deleteFile: \"~/results/out.txt\"")
NORMAL (U"and add lines in the following way:")
	CODE1 (U"appendFileLine: \"~/results/out.txt \", fixed$ (tmin, 6), \" \", fixed$ (tmax, 6), \" \",")
	CODE1 (U"... fixed$ (mean, 2), \" \", fixed$ (minimum, 2), \" \", fixed$ (maximum, 2), \" \",")
	CODE1 (U"... fixed$ (stdev, 2)")
MAN_END

MAN_BEGIN (U"Script for creating a frequency sweep", U"ppgb", 20170904)
INTRO (U"\"I have to find a formula for a sinewave that sweeps from 1 kHz to 12 kHz in "
	"60 seconds while ramping the amplitude from 1 to 12 volts in the same amount of time.\"")
NORMAL (U"The absolute amplitude in volts cannot be handled, of course, but linear crescendo is easy:")
CODE (U"Create Sound from formula: \"sweep\", 1, 0, 60, 44100,")
CODE (U"... ~ 0.05 * (1 + 11 * x/60) * sin (2*pi * (1000 + 11000/2 * x/60) * x)")
NORMAL (U"Note the \"/2\" in this formula. Here is the derivation of the formula:")
EQUATION (U"%frequency (%t) = 1000 + 11000 %t / 60")
EQUATION (U"%phase (%t) = \\in %frequency (%t) %dt = 1000 %t + 11000 (%t^2/2) / 60")
EQUATION (U"%signal (%t) = sin (%phase (%t))")
MAN_END

MAN_BEGIN (U"Script for onset detection", U"ppgb", 20140112)
INTRO (U"\"Can anybody provide me with a script that detects the onset of sound (i.e. the end of silence).\"")
NORMAL (U"You can create an Intensity contour and look for the first frame that is above some predefined threshold:")
CODE (U"To Intensity: 100, 0")
CODE (U"n = Get number of frames")
CODE (U"for i to n")
	CODE1 (U"intensity = Get value in frame: i")
	CODE1 (U"if intensity > 40")
		CODE2 (U"time = Get time from frame: i")
		CODE2 (U"writeInfoLine: \"Onset of sound at: \", fixed$ (time, 3), \" seconds.\"")
		CODE2 (U"exit")
	CODE1 (U"endif")
CODE (U"endfor")
NORMAL (U"Since the intensity is computed with rather long windows, the result may be 0.01 or 0.02 seconds "
	"before the actual start of sound.")
MAN_END

MAN_BEGIN (U"Script for TextGrid boundary drawing", U"ppgb", 20140107)
INTRO (U"\"I want only the dotted lines of the textgrid marked on top of another analysis (e.g. pitch, intensity or so) "
	"without the labels being shown below it.\"")
CODE (U"n = Get number of intervals: 1")
CODE (U"for i to n-1")
    CODE1 (U"t = Get end point: 1, i")
    CODE1 (U"One mark bottom: t, \"no\", \"no\", \"yes\"")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Script for analysing pitch with a TextGrid", U"ppgb", 20201229)
INTRO (U"\"I want the mean pitch of every interval that has a non-empty label on tier 5.\"")
CODE (U"if numberOfSelected (\"Sound\") <> 1 or numberOfSelected (\"TextGrid\") <> 1")
	CODE1 (U"exitScript: \"Please select a Sound and a TextGrid first.\"")
CODE (U"endif")
CODE (U"sound = selected (\"Sound\")")
CODE (U"textgrid = selected (\"TextGrid\")")
CODE (U"writeInfoLine: \"Result:\"")
CODE (U"selectObject: sound")
CODE (U"pitch = To Pitch: 0.0, 75, 600")
CODE (U"selectObject: textgrid")
CODE (U"n = Get number of intervals: 5")
CODE (U"for i to n")
	CODE1 (U"tekst$ = Get label of interval: 5, i")
	CODE1 (U"if tekst$ <> \"\"")
		CODE2 (U"t1 = Get starting point: 5, i")
		CODE2 (U"t2 = Get end point: 5, i")
		CODE2 (U"selectObject: pitch")
		CODE2 (U"f0 = Get mean: t1, t2, \"Hertz\"")
		CODE2 (U"appendInfoLine: fixed$ (t1, 3), \" \", fixed$ (t2, 3), \" \", round (f0), \" \", tekst$")
		CODE2 (U"selectObject: textgrid")
	CODE1 (U"endif")
CODE (U"endfor")
CODE (U"selectObject: sound, textgrid")
MAN_END

}

/* End of file manual_scripting.cpp */
