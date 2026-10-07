# Makefile of the library "sys"
# Paul Boersma, 23 May 2026

# -I ../sys is there because e.g. Graphics.cpp include fon/Function.h, which again includes something from sys
CPPFLAGS += -I ../kar -I ../melder -I ../sys -I ../dwsys

OBJECTS = Thing.o Data.o Simple.o Collection.o Strings.o \
   Graphics.o Graphics_linesAndAreas.o Graphics_text.o Graphics_colour.o \
   Graphics_image.o Graphics_record.o \
   Graphics_utils.o Graphics_grey.o Graphics_altitude.o \
   GraphicsPostscript.o Graphics_surface.o \
   ManPage.o ManPages.o ManPages_toHtml.o Script.o Notebook.o machine.o \
   GraphicsScreen.o Printer.o \
   Preferences.o site.o \
   Picture.o Ui.o UiFile.o UiPause.o DataGui.o Editor.o DataEditor.o HyperPage.o Manual.o TextEditor.o \
   praat.o praat_actions.o praat_menuCommands.o praat_picture.o \
   praat_script.o praat_statistics.o praat_logo.o praat_library.o \
   praat_objectMenus.o InfoEditor.o ScriptEditor.o NotebookEditor.o ButtonEditor.o \
   Interpreter.o Formula.o \
   StringsEditor.o DemoEditor.o \
   motifEmulator.o GuiText.o GuiWindow.o Gui.o GuiObject.o GuiDrawingArea.o \
   GuiMenu.o GuiMenuItem.o GuiButton.o GuiLabel.o GuiCheckButton.o GuiRadioButton.o \
   GuiDialog.o GuiList.o GuiTrust.o GuiFileSelect.o GuiScale.o GuiScrollBar.o GuiScrolledWindow.o \
   GuiControl.o GuiForm.o GuiOptionMenu.o GuiProgressBar.o GuiShell.o GuiThing.o Gui_messages.o

.PHONY: all clean

all: libsys.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libsys.a

libsys.a: $(OBJECTS)
	ar rus libsys.a $(OBJECTS)

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../dwsys/*.h
