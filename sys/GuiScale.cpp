/* GuiScale.cpp
 *
 * Copyright (C) 1993-2011,2012,2015,2016,2017,2026 Paul Boersma, 2020 David Weenink
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

Thing_implement (GuiScale, GuiControl, 0);

#if motif
	#define iam_scale \
		Melder_assert (widget -> widgetClass == xmScaleWidgetClass); \
		GuiScale me = (GuiScale) widget -> userData
#else
	#define iam_scale \
		GuiScale me = (GuiScale) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkScale_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiScale);
		forget (me);
	}
#elif motif
	void _GuiWinScale_destroy (GuiObject widget) {
		iam_scale;
		DestroyWindow (widget -> window);
		trace (U"forgetting a scale or a progress bar");
		forget (me);   // NOTE: my widget is not destroyed here
	}
#elif cocoa
	@implementation GuiCocoaScale {
		GuiScale d_userData;
	}
	- (void) dealloc {   // override
		GuiScale me = d_userData;
		forget (me);
		trace (U"deleting a progress bar");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiScale));
		d_userData = static_cast <GuiScale> (userData);
	}
	@end
#endif

GuiScale GuiScale_create (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, uint32 /* flags */)
{
	autoGuiScale me = Thing_new (GuiScale);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_hscrollbar_new (nullptr);
		gtk_range_set_range (GTK_RANGE (my d_widget), 0, 1000);
		GtkAdjustment *adj = gtk_range_get_adjustment (GTK_RANGE (my d_widget));
		gtk_adjustment_set_page_size (adj, 150.0);
		gtk_adjustment_changed (adj);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkScale_destroyCallback), me.get());
	#elif motif
		my d_widget = XmCreateScale (parent -> d_widget, "scale", nullptr, 0);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtVaSetValues (my d_widget, XmNorientation, XmHORIZONTAL,
			XmNminimum, minimum, XmNmaximum, maximum, XmNvalue, value, //XmNy, 300,
			nullptr
		);
	#elif cocoa
		my d_cocoaScale = [[GuiCocoaScale alloc] init];
		my d_widget = my d_cocoaScale;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[my d_cocoaScale   setUserData: me.get()];
		[my d_cocoaScale   setIndeterminate: false];
		[my d_cocoaScale   setMinValue: minimum];
		[my d_cocoaScale   setMaxValue: maximum];
		[my d_cocoaScale   setDoubleValue: value];
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiScale GuiScale_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, uint32 flags)
{
	GuiScale me = GuiScale_create (parent, left, right, top, bottom, minimum, maximum, value, flags);
	GuiThing_show (me);
	return me;
}

void GuiScale_setValue (GuiScale me, int value) {
	#if gtk
		gtk_range_set_value (GTK_RANGE (my d_widget), value);
	#elif motif
		XmScaleSetValue (my d_widget, value);
	#elif cocoa
		[my d_cocoaScale   setDoubleValue: value];
	#endif
}

/* End of file GuiScale.cpp */
