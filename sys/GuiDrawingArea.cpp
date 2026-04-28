/* GuiDrawingArea.cpp
 *
 * Copyright (C) 1993-2018,2020-2023,2025,2026 Paul Boersma,
 *               2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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
#if gtk
	#include "gdk/gdkkeysyms.h"
	#include <locale.h>
#endif
#include "GraphicsP.h"

Thing_implement (GuiDrawingArea, GuiControl, 0);

#if motif
	#define iam_drawingarea \
		Melder_assert (widget -> widgetClass == xmDrawingAreaWidgetClass); \
		GuiDrawingArea me = (GuiDrawingArea) widget -> userData
#else
	#define iam_drawingarea \
		GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget)
#endif

#if gtk
#pragma mark - GTK CALLBACKS (WITH CAIRO)
	static void _guiGtkDrawingArea_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiDrawingArea);
		forget (me);
	}
	static gboolean _guiGtkDrawingArea_drawCallback (GuiObject widget, cairo_t *cairoGraphicsContext, gpointer void_me) {
		trace (U"begin");
		iam (GuiDrawingArea);
		Melder_assert (me);
		if (my d_exposeCallback) {
			Melder_assert (my numberOfGraphicses > 0);
			structGuiDrawingArea_ExposeEvent event { me, 0 };
			event. x = 0;
			event. y = 0;
			event. width = gtk_widget_get_allocated_width (GTK_WIDGET (widget));
			event. height = gtk_widget_get_allocated_height (GTK_WIDGET (widget));
			try {
				for (int igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++)
					((GraphicsScreen) my graphicses [igraphics]) -> d_cairoGraphicsContext = cairoGraphicsContext;
				my d_exposeCallback (my d_exposeBoss, & event);
				for (int igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++)
					((GraphicsScreen) my graphicses [igraphics]) -> d_cairoGraphicsContext = nullptr;
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
			trace (U"the draw callback handled drawing");
			return true;
		}
		trace (U"GTK will handle redrawing");
		return false;
	}
	static structGuiDrawingArea_MouseEvent::Phase previousPhase = structGuiDrawingArea_MouseEvent::Phase::DROP;
	static gboolean _guiGtkDrawingArea_mouseDownCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my mouseCallback) {
			structGuiDrawingArea_MouseEvent event { me, 0 };
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (((GdkEventButton *) e) -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (((GdkEventButton *) e) -> state & GDK_MOD1_MASK) != 0;
			if (previousPhase == structGuiDrawingArea_MouseEvent::Phase::CLICK) {
				/*
					Apparently a double-click.
					On other platforms, a mouse-up event is always generated, even within a double-click.
					On Linux, we generate it ourselves.
				*/
				try {
					previousPhase = event. phase = structGuiDrawingArea_MouseEvent::Phase::DROP;
					my mouseCallback (my mouseBoss, & event);
				} catch (MelderError) {
					Melder_flushError (U"Mouse drop not completely handled.");
				}
			}
			try {
				previousPhase = event. phase = structGuiDrawingArea_MouseEvent::Phase::CLICK;
				my mouseCallback (my mouseBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse click not completely handled.");
			}
			return true;
		}
		return false;
	}
	static gboolean _guiGtkDrawingArea_mouseDraggedCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my mouseCallback) {
			structGuiDrawingArea_MouseEvent event { me, 0 };
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (((GdkEventButton *) e) -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (((GdkEventButton *) e) -> state & GDK_MOD1_MASK) != 0;
			try {
				previousPhase = event. phase = structGuiDrawingArea_MouseEvent::Phase::DRAG;
				my mouseCallback (my mouseBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse drag not completely handled.");
			}
			return true;
		}
		return false;
	}
	static gboolean _guiGtkDrawingArea_mouseUpCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my mouseCallback) {
			structGuiDrawingArea_MouseEvent event { me, 0 };
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (((GdkEventButton *) e) -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (((GdkEventButton *) e) -> state & GDK_MOD1_MASK) != 0;
			try {
				previousPhase = event. phase = structGuiDrawingArea_MouseEvent::Phase::DROP;
				my mouseCallback (my mouseBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse drop not completely handled.");
			}
			return true;
		}
		return false;
	}
	static gboolean _guiGtkDrawingArea_keyCallback (GuiObject widget, GdkEvent *gevent, gpointer void_me) {
		iam (GuiDrawingArea);
		trace (U"begin");
		if (my d_keyCallback && gevent -> type == GDK_KEY_PRESS) {
			structGuiDrawingArea_KeyEvent event { me, 0 };
			GdkEventKey *gkeyEvent = (GdkEventKey *) gevent;
			event. key = gkeyEvent -> keyval;
			/*
			 * Translate with the help of /usr/include/gtk-2.0/gdk/gdkkeysyms.h
			 */
			if (event. key == GDK_KEY_Escape) event. key = 27;
			if (event. key == GDK_KEY_Left)   event. key = 0x2190;
			if (event. key == GDK_KEY_Up)     event. key = 0x2191;
			if (event. key == GDK_KEY_Right)  event. key = 0x2192;
			if (event. key == GDK_KEY_Down)   event. key = 0x2193;
			event. shiftKeyPressed = (gkeyEvent -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (gkeyEvent -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (gkeyEvent -> state & GDK_MOD1_MASK) != 0;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
			/*
			 * FIXME: here we should empty the type-ahead buffer
			 */
			return true;
		}
		return false;   // if the drawing area has no keyCallback, the system will send the key press to a text field.
	}
	static gboolean _guiGtkDrawingArea_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my d_resizeCallback) {
			structGuiDrawingArea_ResizeEvent event { me, 0 };
			trace (U"drawingArea resized to ", allocation -> width, U" x ", allocation -> height, U".");
			event. width = allocation -> width;
			event. height = allocation -> height;
			//g_debug("%d %d", allocation->width, allocation->height);
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
			return true;
		}
		return false;
	}
	static void _guiGtkDrawingArea_handleZoom (GuiObject widget, double delta) {
		iam_drawingarea;
		if (my d_zoomCallback) {
			struct structGuiDrawingArea_ZoomEvent event { me };
			event. delta = delta;
			try {
				my d_zoomCallback (my d_zoomBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Zoom not completely handled.");
			}
		}
	}
	#if 0   // GTK 3
		static void _guiGtkDrawingArea_zoomCallback (GtkGestureZoom *w, gdouble scale, gpointer void_me) {
			iam (GuiDrawingArea);
			TRACE
			trace (1);
			if (! my d_zoomCallback)
				return;
			trace (3);
			const double zoomDelta = (scale - 1.0) * 100.0;
			if (zoomDelta != 0.0) {
				trace (5);
				_guiGtkDrawingArea_handleZoom (w, zoomDelta);
				trace (6);
			}
		}
	#elif 0   // GTK 4
		static gboolean _guiGtkDrawingArea_pinchCallback (GuiObject w, GdkEventTouchpadPinch *event, gpointer void_me) {
			iam (GuiDrawingArea);
			TRACE
			trace (1);
			if (my d_zoomCallback) {
				if (event -> phase == GDK_TOUCHPAD_GESTURE_PHASE_UPDATE) {
					const double zoomDelta = (event -> scale - 1.0) * 100.0;
					if (zoomDelta != 0.0)
						_guiGtkDrawingArea_handleZoom (w, zoomDelta);
				}
			}
		}
	#endif
	static gboolean _guiGtkDrawingArea_swipeCallback (GuiObject w, GdkEventScroll *event, gpointer void_me) {
		iam (GuiDrawingArea);
		trace (U"_guiGtkDrawingArea_swipeCallback ", Melder_pointer (my d_horizontalScrollBar), Melder_pointer (my d_verticalScrollBar));
		constexpr double zoomStepSize = 12.0;
		const bool shiftKeyPressed = (event -> state & GDK_SHIFT_MASK) != 0;
		const bool controlKeyPressed = (event -> state & GDK_CONTROL_MASK) != 0;
		if (controlKeyPressed && my d_zoomCallback) {
			double zoomDelta = 0.0;
			switch (event -> direction) {
				case GDK_SCROLL_UP:
					zoomDelta = zoomStepSize;
					break;
				case GDK_SCROLL_DOWN:
					zoomDelta = - zoomStepSize;
					break;
				case GDK_SCROLL_LEFT:
					zoomDelta = - zoomStepSize;
					break;
				case GDK_SCROLL_RIGHT:
					zoomDelta = zoomStepSize;
					break;
				#ifdef GDK_SCROLL_SMOOTH
				case GDK_SCROLL_SMOOTH:
					if (event -> delta_y != 0.0)
						zoomDelta = - event -> delta_y * zoomStepSize;
					else if (event -> delta_x != 0.0)
						zoomDelta = - event -> delta_x * zoomStepSize;
					break;
				#endif
			}
			if (zoomDelta != 0.0)
				_guiGtkDrawingArea_handleZoom (w, zoomDelta);
			return true;
		}
		const bool mapVerticalToHorizontal = (my d_horizontalScrollBar && (! my d_verticalScrollBar || shiftKeyPressed));
		double hSteps = 0.0;
		double vSteps = 0.0;
		switch (event -> direction) {
			case GDK_SCROLL_LEFT:
				hSteps -= 1.0;
				break;
			case GDK_SCROLL_RIGHT:
				hSteps += 1.0;
				break;
			case GDK_SCROLL_UP:
				if (mapVerticalToHorizontal)
					hSteps -= 1.0;
				else
					vSteps -= 1.0;
				break;
			case GDK_SCROLL_DOWN:
				if (mapVerticalToHorizontal)
					hSteps += 1.0;
				else
					vSteps += 1.0;
				break;
			#ifdef GDK_SCROLL_SMOOTH
			case GDK_SCROLL_SMOOTH:
				if (event -> delta_x != 0.0)
					hSteps += event -> delta_x;
				if (event -> delta_y != 0.0) {
					if (mapVerticalToHorizontal)
						hSteps += event -> delta_y;
					else
						vSteps += event -> delta_y;
				}
				break;
			#endif
		}
		if (my d_horizontalScrollBar && hSteps != 0.0) {
			double hv = gtk_range_get_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget));
			GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (my d_horizontalScrollBar -> d_widget));
			gdouble hi;
			g_object_get (adjustment, "step_increment", & hi, nullptr);
			gtk_range_set_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget), hv + hSteps * hi);
		}
		if (my d_verticalScrollBar && vSteps != 0.0) {
			double vv = gtk_range_get_value (GTK_RANGE (my d_verticalScrollBar -> d_widget));
			GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (my d_verticalScrollBar -> d_widget));
			gdouble vi;
			g_object_get (adjustment, "step_increment", & vi, nullptr);
			gtk_range_set_value (GTK_RANGE (my d_verticalScrollBar -> d_widget), vv + vSteps * vi);
		}
		return true;
	}
#elif motif
#pragma mark - MOTIF CALLBACKS (WITH GDI)
	void _GuiWinDrawingArea_destroy (GuiObject widget) {
		iam_drawingarea;
		DestroyWindow (widget -> window);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinDrawingArea_update (GuiObject widget) {
		iam_drawingarea;
		Melder_assert (my numberOfGraphicses > 0);
		GraphicsScreen graphics = (GraphicsScreen) my graphicses [1];
		Melder_assert (Thing_isa (graphics, classGraphicsScreen));
		HDC memoryDC = CreateCompatibleDC (graphics -> d_gdiGraphicsContext);
		HBITMAP memoryBitmap = CreateCompatibleBitmap (graphics -> d_gdiGraphicsContext, widget -> width, widget -> height);
		SelectObject (memoryDC, memoryBitmap);
		SetBkMode (memoryDC, TRANSPARENT);   // not the default!
		SelectPen (memoryDC, GetStockPen (BLACK_PEN));
		SelectBrush (memoryDC, GetStockBrush (BLACK_BRUSH));
		SetTextAlign (memoryDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);   // baseline is not the default!
		HDC saveContext = graphics -> d_gdiGraphicsContext;
		for (int igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++)
			((GraphicsScreen) my graphicses [igraphics]) -> d_gdiGraphicsContext = memoryDC;
		if (my d_exposeCallback) {
			structGuiDrawingArea_ExposeEvent event { me };
			try {
				my d_exposeCallback (my d_exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
		}
		for (int igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++)
			((GraphicsScreen) my graphicses [igraphics]) -> d_gdiGraphicsContext = saveContext;
		BitBlt (graphics -> d_gdiGraphicsContext, 0, 0, widget -> width, widget -> height, memoryDC, 0, 0, SRCCOPY);
		DeleteObject (memoryBitmap);
		DeleteDC (memoryDC);
		ValidateRect (widget -> window, nullptr);
	}
	void _GuiWinDrawingArea_handleMouse (GuiObject widget, structGuiDrawingArea_MouseEvent::Phase phase, int x, int y) {
		iam_drawingarea;
		if (my mouseCallback) {
			structGuiDrawingArea_MouseEvent event { me, 0 };
			event. x = x;
			event. y = y;
			event. phase = phase;
			//Melder_casual (U": phase ", (int) phase);
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
			event. optionKeyPressed = GetKeyState (VK_MENU) < 0;
			event. commandKeyPressed = GetKeyState (VK_CONTROL) < 0;
			try {
				my mouseCallback (my mouseBoss, & event);
			} catch (MelderError) {
				switch (phase) {
					case structGuiDrawingArea_MouseEvent::Phase::CLICK:
						Melder_flushError (U"Mouse click not completely handled.");
					break; case structGuiDrawingArea_MouseEvent::Phase::DRAG:
						Melder_flushError (U"Mouse drag not completely handled.");
					break; case structGuiDrawingArea_MouseEvent::Phase::DROP:
						Melder_flushError (U"Mouse drop not completely handled.");
					break;
				}
			}
		}
	}
	void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar) {   // TODO: event?
		iam_drawingarea;
		if (my d_keyCallback) {
			structGuiDrawingArea_KeyEvent event { me, 0 };
			event. key = kar;
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;   // TODO: event -> key?
			event. optionKeyPressed = GetKeyState (VK_MENU) < 0;
			event. commandKeyPressed = GetKeyState (VK_CONTROL) < 0;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_shellResize (GuiObject widget) {
		iam_drawingarea;
		if (my d_resizeCallback) {
			structGuiDrawingArea_ResizeEvent event { me };
			event. width = widget -> width;
			event. height = widget -> height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_handleZoom (GuiObject widget, double delta) {
		iam_drawingarea;
		//TRACE
		trace (delta);
		if (my d_zoomCallback) {
			struct structGuiDrawingArea_ZoomEvent event { me };
			event. delta = delta;
			try {
				my d_zoomCallback (my d_zoomBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Zoom not completely handled.");
			}
		}
	}
#elif cocoa
#pragma mark - COCOA CALLBACKS (WITH QUARTZ)
	@implementation GuiCocoaDrawingArea {
		GuiDrawingArea userData;
		bool inited;
		NSTrackingArea *trackingArea;
	}
	- (id) initWithFrame: (NSRect) frame {
		self = [super initWithFrame: frame];
		if (self) {
			self -> trackingArea = [[NSTrackingArea alloc]
				initWithRect: [self visibleRect]
				options: NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingInVisibleRect | NSTrackingActiveAlways
				owner: self
				userInfo: nil
			];
			trace (U"Retain count of tracking area after creation: ", [self -> trackingArea   retainCount]);   // probably 1
			[self addTrackingArea: self -> trackingArea];   // this retains the tracking area
			trace (U"Retain count of tracking area after inclusion: ", [self -> trackingArea   retainCount]);   // probably 2
			[self -> trackingArea   release];
			trace (U"Retain count of tracking area after release: ", [self -> trackingArea   retainCount]);   // probably 1
		}
		return self;
	}
	- (void) dealloc {   // override
		GuiDrawingArea me = self -> userData;
		if (Melder_debug == 55)
			Melder_casual (U"\t\tGuiCocoaDrawingArea-", Melder_pointer (self), U" dealloc for ", Melder_pointer (me));
		forget (me);
		trace (U"Retain count of tracking area before exclusion: ", [self -> trackingArea   retainCount]);   // probably 1
		[self removeTrackingArea: self -> trackingArea];   // this releases the tracking area
		//trace (U"Retain count of tracking area before destruction: ", [self -> trackingArea   retainCount]);   // probably 0 (a crash)
		trace (U"deleting a drawing area");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return self -> userData;
	}
	- (void) setUserData: (GuiThing) newUserData {
		self -> userData = static_cast <GuiDrawingArea> (newUserData);
	}
	- (void) resizeCallback: (NSRect) rect {
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (me && my d_resizeCallback) {
			structGuiDrawingArea_ResizeEvent event = { me, 0, 0 };
			event. width = rect. size. width;
			event. height = rect. size. height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
		}
	}
	- (void) drawRect: (NSRect) dirtyRect {
		trace (U"dirtyRect: ", dirtyRect.origin.x, U", ", dirtyRect.origin.y, U", ", dirtyRect.size.width, U", ", dirtyRect.size.height);
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (Melder_debug == 55)
			Melder_casual (U"\t\tGuiCocoaDrawingArea-", Melder_pointer (self), U" draw to ", Melder_pointer (me));
		if (! self -> inited) {
			// Last chance to do this. Is there a better place?
			[self resizeCallback: [self frame]];
			self -> inited = true;
		}
		if (me && my d_exposeCallback && my numberOfGraphicses > 0) {   // asserting my numberOfGraphicses could crash if Editor_init gives a warning
			structGuiDrawingArea_ExposeEvent event = { me, 0, 0, 0, 0 };
			if (Melder_debug == 55)
				Melder_casual (U"\t", Thing_messageNameAndAddress (me), U" draw for ", Melder_pointer (my d_exposeBoss));
			try {
				for (integer igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++) {
					GraphicsScreen graphics = static_cast <GraphicsScreen> (my graphicses [igraphics]);
					if (graphics -> d_macView) {
						graphics -> d_macGraphicsContext = [[NSGraphicsContext currentContext] CGContext];
						Melder_assert (!! graphics -> d_macGraphicsContext);
					}
				}
				my d_exposeCallback (my d_exposeBoss, & event);
				for (integer igraphics = 1; igraphics <= my numberOfGraphicses; igraphics ++) {
					GraphicsScreen graphics = static_cast <GraphicsScreen> (my graphicses [igraphics]);
					if (graphics -> d_macView)
						graphics -> d_macGraphicsContext = nullptr;
				}
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
		}
	}
	- (void) setFrame: (NSRect) rect {
		[self resizeCallback: rect];
		[super setFrame: rect];
	}
	- (BOOL) acceptsFirstResponder {
		/*
		 * This overridden method tells the event chain whether the drawing area can accept key events.
		 * It is important that the Demo window and the RunnerMFC window accept key events.
		 * A side effect of accepting key events is that the drawing area obtains the key focus when the user clicks in the drawing area.
		 * It is important, however, that the drawing area of the TextGrid window cannot take away the key focus
		 * from the text field at the top; therefore, that drawing area should not accept key events.
		 * The implementation below is based on the fact that, naturally, the Demo window and the RunnerMFC window
		 * have a key callback, and the drawing area of the TextGrid window has not
		 * (a side effect of this implementation is that the drawing area of the Manual window does not take away
		 * the key focus from the Search field, a situation that cannot hurt).
		 */
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		return !! my d_keyCallback;
	}
	- (void) mouseEntered: (NSEvent *) nsEvent {
		(void) nsEvent;
		[[NSCursor crosshairCursor] push];
	}
	- (void) mouseExited: (NSEvent *) nsEvent {
		(void) nsEvent;
		[[NSCursor currentCursor] pop];
	}
	- (void) mouse: (NSEvent *) nsEvent inPhase: (structGuiDrawingArea_MouseEvent::Phase) phase {
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (me && my mouseCallback) {
			structGuiDrawingArea_MouseEvent event = { me, 0, 0, phase, false, false, false };
			NSPoint local_point = [self   convertPoint: [nsEvent locationInWindow]   fromView: nil];
			event. x = local_point. x;
			event. y = local_point. y;
			NSUInteger modifiers = [nsEvent modifierFlags];
			event. shiftKeyPressed = modifiers & NSEventModifierFlagShift;
			event. optionKeyPressed = modifiers & NSEventModifierFlagOption;
			event. commandKeyPressed = modifiers & NSEventModifierFlagCommand;
			try {
				my mouseCallback (my mouseBoss, & event);
			} catch (MelderError) {
				switch (phase) {
					case structGuiDrawingArea_MouseEvent::Phase::CLICK:
						Melder_flushError (U"Mouse click not completely handled.");
					break; case structGuiDrawingArea_MouseEvent::Phase::DRAG:
						Melder_flushError (U"Mouse drag not completely handled.");
					break; case structGuiDrawingArea_MouseEvent::Phase::DROP:
						Melder_flushError (U"Mouse drop not completely handled.");
					break;
				}
			}
		}
	}
	- (void) mouseDown: (NSEvent *) nsEvent {
	 	[self   mouse: nsEvent   inPhase: structGuiDrawingArea_MouseEvent::Phase::CLICK];
	}
	- (void) mouseDragged: (NSEvent *) nsEvent {
	 	[self   mouse: nsEvent   inPhase: structGuiDrawingArea_MouseEvent::Phase::DRAG];
	}
	- (void) mouseUp: (NSEvent *) nsEvent {
	 	[self   mouse: nsEvent   inPhase: structGuiDrawingArea_MouseEvent::Phase::DROP];
	}
	- (void) scrollWheel: (NSEvent *) nsEvent {
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (me && (my d_horizontalScrollBar || my d_verticalScrollBar)) {
			/*
				Automatically implemented by macOS: Shift-Wheel means horizontal scrolling.

				To implement here:
					- Command-Wheel means zooming.
					- Wheel means horizontal scrolling if there is no vertical scrollbar.
			*/
			const bool commandKeyPressed = ( ([nsEvent modifierFlags] & NSEventModifierFlagCommand) != 0 );
			const CGFloat deltaX = [nsEvent scrollingDeltaX];
			const CGFloat deltaY = [nsEvent scrollingDeltaY];
			const CGFloat biggestDelta = ( fabs (deltaX) > fabs (deltaY) ? deltaX : deltaY );
			if (commandKeyPressed) {
				/*
					Implement zooming.
				*/
				const CGFloat relativeSpeed = 0.01;
				if (my d_horizontalScrollBar && my d_verticalScrollBar) {
					[(GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget  magnifyBy: deltaX * relativeSpeed];
					[(GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget  magnifyBy: deltaY * relativeSpeed];
				} else if (my d_horizontalScrollBar)
					[(GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget  magnifyBy: biggestDelta * relativeSpeed];
				else
					[(GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget  magnifyBy: biggestDelta * relativeSpeed];
			} else {
				/*
					Implement scrolling.
				 */
				if (my d_horizontalScrollBar && my d_verticalScrollBar) {
					[(GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget   scrollBy: deltaX];
					[(GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget   scrollBy: deltaY];
				} else if (my d_horizontalScrollBar)
					[(GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget   scrollBy: biggestDelta];
				else
					[(GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget   scrollBy: biggestDelta];
			}
		} else {
			[super scrollWheel: nsEvent];
		}
	}
	- (void) magnifyWithEvent: (NSEvent *) nsEvent {
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (me && (my d_horizontalScrollBar || my d_verticalScrollBar)) {
			if (my d_horizontalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget;
				[cocoaScrollBar magnifyBy: [nsEvent magnification]];
			}
			if (my d_verticalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget;
				[cocoaScrollBar magnifyBy: [nsEvent magnification]];
			}
		} else {
			[super magnifyWithEvent: nsEvent];
		}
	}
	- (BOOL) isFlipped {
		return YES;
	}
	- (void) keyDown: (NSEvent *) nsEvent {
		trace (U"key pressed ");
		GuiDrawingArea me = (GuiDrawingArea) self -> userData;
		if (me && my d_keyCallback) {
			structGuiDrawingArea_KeyEvent event = { me, U'\0', false, false, false };
			event. key = [[nsEvent charactersIgnoringModifiers]   characterAtIndex: 0];
			if (event. key == NSLeftArrowFunctionKey)
				event. key = 0x2190;
			if (event. key == NSRightArrowFunctionKey)
				event. key = 0x2192;
			if (event. key == NSUpArrowFunctionKey)
				event. key = 0x2191;
			if (event. key == NSDownArrowFunctionKey)
				event. key = 0x2193;
			trace (U"key ", event. key);
			NSUInteger modifiers = [nsEvent modifierFlags];
			event. shiftKeyPressed = modifiers & NSEventModifierFlagShift;
			event. optionKeyPressed = modifiers & NSEventModifierFlagOption;
			event. commandKeyPressed = modifiers & NSEventModifierFlagCommand;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
		}
	}
	- (void) cancelOperation: (id) sender {
		Melder_casual (U"escape key pressed");
	}
	@end
#endif

void structGuiDrawingArea :: v9_destroy () noexcept {
	if (Melder_debug == 55)
		Melder_casual (U"\t", Thing_messageNameAndAddress (this), U" v9_destroy");
	#if cocoa
		if (our d_widget)
			[our d_widget setUserData: nullptr];   // undangle reference to this
	#endif
	GuiDrawingArea_Parent :: v9_destroy ();
}

GuiDrawingArea GuiDrawingArea_create (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 /* flags */)
{
	autoGuiDrawingArea me = Thing_new (GuiDrawingArea);
	if (Melder_debug == 55)
		Melder_casual (U"\t", Thing_messageNameAndAddress (me.get()), U" init in ", Thing_messageNameAndAddress (parent -> d_shell));
	my d_shell = parent -> d_shell;
	my d_shell -> drawingArea = me.get();
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my mouseCallback = mouseCallback;
	my mouseBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	my d_zoomCallback = zoomCallback;
	my d_zoomBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "draw",         G_CALLBACK (_guiGtkDrawingArea_drawCallback),       me.get());
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_guiGtkDrawingArea_destroyCallback),      me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_guiGtkDrawingArea_mouseDownCallback),    me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_guiGtkDrawingArea_mouseUpCallback),      me.get());
		//g_signal_connect (G_OBJECT (my d_widget), "drag-motion-event",    G_CALLBACK (_guiGtkDrawingArea_mouseUpCallback),   me.get());
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_guiGtkDrawingArea_mouseDraggedCallback), me.get());
		if (parent) {
			Melder_assert (parent -> d_widget);
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_guiGtkDrawingArea_keyCallback), me.get());
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_guiGtkDrawingArea_resizeCallback), me.get());

		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif motif
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, U"drawingArea");
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindowEx (0, Melder_peek32toW (_GuiWin_getDrawingAreaClassName ()), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, nullptr, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (! my d_widget -> shell -> textFocus)
			my d_widget -> shell -> textFocus = my d_widget;   // even if not-yet-managed. But in that case it will not receive global focus
	#elif cocoa
		GuiCocoaDrawingArea *drawingArea = [[GuiCocoaDrawingArea alloc] init];
		if (Melder_debug == 55)
			Melder_casual (U"\t\tGuiCocoaDrawingArea-", Melder_pointer (drawingArea), U" init in ", Thing_messageNameAndAddress (me.get()));
		my d_widget = (GuiObject) drawingArea;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[drawingArea   setUserData: me.get()];
		if (keyCallback) {
			[[drawingArea window]   makeFirstResponder: drawingArea];   // needed in DemoWindow
		}
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiDrawingArea GuiDrawingArea_createShown (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, left, right, top, bottom,
		exposeCallback, mouseCallback,
		keyCallback, resizeCallback, zoomCallback, boss, flags
	);
	GuiThing_show (me);
	return me;
}

GuiDrawingArea GuiDrawingArea_create (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 /* flags */)
{
	autoGuiDrawingArea me = Thing_new (GuiDrawingArea);
	my d_shell = parent -> d_shell;
	my d_shell -> drawingArea = me.get();
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my mouseCallback = mouseCallback;
	my mouseBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	my d_zoomCallback = zoomCallback;
	my d_zoomBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "draw",         G_CALLBACK (_guiGtkDrawingArea_drawCallback),       me.get());
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_guiGtkDrawingArea_destroyCallback),      me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_guiGtkDrawingArea_mouseDownCallback),    me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_guiGtkDrawingArea_mouseUpCallback),      me.get());
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_guiGtkDrawingArea_mouseDraggedCallback), me.get());
		if (parent) {
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_guiGtkDrawingArea_keyCallback), me.get());
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_guiGtkDrawingArea_resizeCallback), me.get());
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
	#elif motif
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, U"drawingArea");
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindowEx (0, Melder_peek32toW (_GuiWin_getDrawingAreaClassName ()), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			0, 0, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, nullptr, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
		if (! my d_widget -> shell -> textFocus)
			my d_widget -> shell -> textFocus = my d_widget;   // even if not-yet-managed. But in that case it will not receive global focus
	#elif cocoa
		GuiCocoaDrawingArea *drawingArea = [[GuiCocoaDrawingArea alloc] init];
		my d_widget = (GuiObject) drawingArea;
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
		[drawingArea setUserData: me.get()];
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiDrawingArea GuiDrawingArea_createShown (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, width, height,
		exposeCallback, mouseCallback,
		keyCallback, resizeCallback, zoomCallback, boss, flags
	);
	GuiThing_show (me);
	return me;
}

void GuiDrawingArea_setSwipable (GuiDrawingArea me, GuiScrollBar horizontalScrollBar, GuiScrollBar verticalScrollBar) {
	my d_horizontalScrollBar = horizontalScrollBar;
	my d_verticalScrollBar = verticalScrollBar;
	#if gtk
		gtk_widget_add_events (GTK_WIDGET (my d_widget), GDK_SCROLL_MASK);
		g_signal_connect (G_OBJECT (my d_widget), "scroll-event", G_CALLBACK (_guiGtkDrawingArea_swipeCallback), me);
		#if 0   // GTK 3
			GtkGesture *zoom = gtk_gesture_zoom_new (GTK_WIDGET (my d_widget));
			g_object_ref (zoom);
			g_signal_connect (zoom, "scale-changed", G_CALLBACK (_guiGtkDrawingArea_zoomCallback), me);
		#elif 0
			gtk_widget_add_events (GTK_WIDGET (my d_widget), GDK_TOUCHPAD_GESTURE_MASK | GDK_SMOOTH_SCROLL_MASK);
			g_signal_connect (G_OBJECT (my d_widget), "event", G_CALLBACK (_guiGtkDrawingArea_genericEventCallback), me);
		#elif 0   // GTK 4
			g_signal_connect (G_OBJECT (my d_widget), "touchpad-pinch", G_CALLBACK (_guiGtkDrawingArea_pinchCallback), me);
		#endif
	#endif
}

void GuiDrawingArea_setExposeCallback (GuiDrawingArea me, GuiDrawingArea_ExposeCallback callback, Thing boss) {
	my d_exposeCallback = callback;
	my d_exposeBoss = boss;
}

void GuiDrawingArea_setMouseCallback (GuiDrawingArea me, GuiDrawingArea_MouseCallback callback, Thing boss) {
	my mouseCallback = callback;
	my mouseBoss = boss;
}

void GuiDrawingArea_setResizeCallback (GuiDrawingArea me, GuiDrawingArea_ResizeCallback callback, Thing boss) {
	my d_resizeCallback = callback;
	my d_resizeBoss = boss;
}

void GuiDrawingArea_setZoomCallback (GuiDrawingArea me, GuiDrawingArea_ZoomCallback callback, Thing boss) {
	my d_zoomCallback = callback;
	my d_zoomBoss = boss;
}

/* End of file GuiDrawingArea.cpp */
