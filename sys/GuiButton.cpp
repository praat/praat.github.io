/* GuiButton.cpp
 *
 * Copyright (C) 1993-2008,2010-2020,2024,2026 Paul Boersma,
 *               2007-2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

#include "GuiP.h"

Thing_implement (GuiButton, GuiControl, 0);

#if gtk
	#define iam_button  GuiButton me = (GuiButton) userData
#elif motif
	#define iam_button  GuiButton me = (GuiButton) widget -> userData
#elif cocoa
	#define iam_button  GuiButton me = (GuiButton) self -> d_userData
#endif

#if gtk
	static void _GuiGtkButton_destroyCallback (GuiObject /* widget */, gpointer userData) {
		GuiButton me = (GuiButton) userData;
		trace (U"destroying GuiButton ", Melder_pointer (me));
		forget (me);
	}
	static void _GuiGtkButton_activateCallback (GuiObject widget, gpointer userData) {
		GuiButton me = (GuiButton) userData;
		structGuiButtonEvent event { me, false, false, false };
		if (my d_activateCallback) {
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your click on button \"", Melder_peek8to32_u (gtk_widget_get_name (GTK_WIDGET (widget))), U"\" was not completely handled.");
			}
		}
	}
#elif motif
	void _GuiWinButton_destroy (GuiObject widget) {
		iam_button;
		if (widget == widget -> shell -> defaultButton)
			widget -> shell -> defaultButton = nullptr;   // remove dangling reference
		if (widget == widget -> shell -> cancelButton)
			widget -> shell -> cancelButton = nullptr;   // remove dangling reference
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinButton_handleClick (GuiObject widget) {
		iam_button;
		if (my d_activateCallback) {
			structGuiButtonEvent event { me, false, false, false };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your click on button \"", widget -> name.get(), U"\" was not completely handled.");
			}
		}
	}
	bool _GuiWinButton_tryToHandleShortcutKey (GuiObject widget) {
		iam_button;
		if (my d_activateCallback) {
			structGuiButtonEvent event { me, false, false, false };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your key click on button \"", widget -> name.get(), U"\" was not completely handled.");
			}
			return true;
		}
		return false;
	}
#elif cocoa
	@implementation GuiCocoaButton {
		GuiButton d_userData;
	}
	- (void) dealloc {   // override
		GuiButton me = d_userData;
		forget (me);
		trace (U"deleting a button");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiButton));
		d_userData = static_cast <GuiButton> (userData);
	}
	- (void) _guiCocoaButton_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiButton me = d_userData;
		if (my d_activateCallback) {
			structGuiButtonEvent event { me, false, false, false };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Your click on button \"", my name.get(), U"\" was not completely handled.");
			}
		}
	}
	@end
#endif

static void gui_blocking_dialog_cb_ok (GuiDialog me, GuiButtonEvent event) {
	my clickedButtonId = event -> button -> d_sequentalIdInDialog;
	#if gtk
		gtk_dialog_response (GTK_DIALOG (my d_gtkWindow), 0);   // or `my clickedButtonId`, but return value of gtk_dialog_run is ignored
	#elif cocoa
		[NSApp stopModal];
	#endif
}
GuiButton GuiButton_create (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiButton_ActivateCallback activateCallback, Thing activateBoss, uint32 flags)
{
	autoGuiButton me = Thing_new (GuiButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_activateCallback = activateCallback;
	my d_activateBoss = activateBoss;
	if (! my d_activateCallback) {
		/*
			This must be a button in a blocking dialog.
		*/
		Melder_assert (Thing_isa (my d_shell, classGuiDialog));
		GuiDialog dialog = (GuiDialog) my d_shell;
		my d_activateCallback = gui_blocking_dialog_cb_ok;
		my d_activateBoss = dialog;
		my d_sequentalIdInDialog = ++ dialog -> latestCreatedButtonId;
		if (flags & GuiButton_DEFAULT) {
			dialog -> defaultButtonId = my d_sequentalIdInDialog;
			dialog -> defaultButton = me.get();
		}
	}
	#if gtk
		my d_widget = gtk_button_new_with_label (Melder_peek32to8 (buttonText));
		gtk_button_set_relief (GTK_BUTTON (my d_widget), GTK_RELIEF_NORMAL);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiButton_DEFAULT || flags & GuiButton_ATTRACTIVE) {
			gtk_widget_set_can_default (GTK_WIDGET (my d_widget), true);
			GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (my d_widget));
			Melder_assert (shell);
			gtk_window_set_default (GTK_WINDOW (shell), GTK_WIDGET (my d_widget));
		} else if (1) {
			gtk_button_set_focus_on_click (GTK_BUTTON (my d_widget), false);
			gtk_widget_set_can_default (GTK_WIDGET (my d_widget), false);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkButton_destroyCallback), me.get());
		g_signal_connect (GTK_BUTTON (my d_widget), "clicked", G_CALLBACK (_GuiGtkButton_activateCallback), me.get());
		GtkWidget *internalLabel = gtk_bin_get_child (GTK_BIN (my d_widget));
		if (GTK_IS_LABEL (internalLabel))
			gtk_label_set_justify (GTK_LABEL (internalLabel), GTK_JUSTIFY_CENTER);
//		if (flags & GuiButton_CANCEL) {
//			parent -> shell -> cancelButton = parent -> cancelButton = my widget;
//		}
	#elif motif
		my d_widget = _Gui_initializeWidget (xmPushButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindow (L"button", Melder_peek32toW (_GuiWin_expandAmpersands (my d_widget -> name.get())),
			WS_CHILD
			| ( flags & (GuiButton_DEFAULT | GuiButton_ATTRACTIVE) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON )
			| ( flags & GuiButton_MULTILINE ? BS_MULTILINE : 0 )
			| WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, nullptr
		);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, flags & GuiButton_DEFAULT ? theWinGuiBoldLabelFont () : theWinGuiNormalLabelFont (), false);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiButton_DEFAULT || flags & GuiButton_ATTRACTIVE)
			parent -> d_widget -> shell -> defaultButton = parent -> d_widget -> defaultButton = my d_widget;
		if (flags & GuiButton_CANCEL)
			parent -> d_widget -> shell -> cancelButton = parent -> d_widget -> cancelButton = my d_widget;
	#elif cocoa
		GuiCocoaButton *button = [[GuiCocoaButton alloc] init];
		my name = Melder_dup_f (buttonText);
		my d_widget = (GuiObject) button;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[button setUserData: me.get()];
		[button setButtonType: NSMomentaryPushInButton];
		if (bottom - top < 30)
			[button setBezelStyle: NSBezelStylePush];   // fixed height, ...
		else
			[button setBezelStyle: NSBezelStyleFlexiblePush];   // ... custom height
		[button setImagePosition: NSNoImage];
		[button setBordered: YES];
		static NSFont *theButtonFont;
		if (! theButtonFont)
			theButtonFont = [NSFont systemFontOfSize: 13.0];
		[button setFont: theButtonFont];
		[button setTitle: (NSString *) Melder_peek32toCfstring (buttonText)];
		[button setTarget: (id) my d_widget];
		[button setAction: @selector (_guiCocoaButton_activateCallback:)];
		//[button setAutoresizingMask: NSViewNotSizable];
		if (flags & GuiButton_DEFAULT) {
			[button setKeyEquivalent: @"\r"];
			[[button window] setDefaultButtonCell: [button cell]];   // does nothing (and doesn't help flexible push buttons)
		}
		if (flags & GuiButton_CANCEL)
			[button setKeyEquivalent: [NSString stringWithFormat: @"%c", 27]];   // Escape key
		if (flags & GuiButton_ATTRACTIVE) {
			//[button setKeyEquivalent: @"\r"];   // slow!
			[button highlight: YES];   // lasts only till it's clicked!
			//[button setBezelStyle: NSThickerSquareBezelStyle];
			//[button setFont: [NSFont boldSystemFontOfSize: 14.0]];
		}
	#endif
	if (flags & GuiButton_INSENSITIVE)
		GuiThing_setSensitive (me.get(), false);
	return me.releaseToAmbiguousOwner();
}

GuiButton GuiButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiButton_ActivateCallback activateCallback, Thing activateBoss, uint32 flags)
{
	GuiButton me = GuiButton_create (parent, left, right, top, bottom, buttonText, activateCallback, activateBoss, flags);
	GuiThing_show (me);
	return me;
}

void GuiButton_setText (GuiButton me, conststring32 text /* cattable */) {
	#if gtk
		gtk_button_set_label (GTK_BUTTON (my d_widget), Melder_peek32to8 (text));
	#elif motif
		my d_widget -> name = Melder_dup_f (text);
		_GuiNativeControl_setTitle (my d_widget);
	#elif cocoa
		[(NSButton *) my d_widget setTitle: (NSString *) Melder_peek32toCfstring (text)];
	#endif
}

/* End of file GuiButton.cpp */
