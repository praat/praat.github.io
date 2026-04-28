#ifndef _Gui_h_
#define _Gui_h_
/* Gui.h
 *
 * Copyright (C) 1993-2026 Paul Boersma, 2013 Tom Naughton
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

/*
	Determine the widget set, honouring the compiler switch NO_GUI
	and/or the compiler switch NO_GRAPHICS, which entails NO_GUI.
 */
#if defined (NO_GRAPHICS) && ! defined (NO_GUI)
	#define NO_GUI
#endif
#if defined (NO_GUI)
	#define gtk 0
	#define motif 0
	#define cocoa 0
#elif defined (UNIX)
	#define gtk 1
	#define motif 0
	#define cocoa 0
#elif defined (_WIN32)
	#define gtk 0
	#define motif 1
	#define cocoa 0
#elif defined (macintosh)
	#define gtk 0
	#define motif 0
	#define cocoa 1
#else
	/*
		Unknown platforms have no GUI.
	*/
	#define gtk 0
	#define motif 0
	#define cocoa 0
#endif

constexpr bool theCommandKeyIsToTheLeftOfTheOptionKey =
	#if defined (macintosh)
		false;
	#else
		true;
	#endif

#include "Collection.h"

#if defined (UNIX)
	#if gtk
		#include <gtk/gtk.h>
		#include <gdk/gdk.h>
	#endif
	#if ! defined (NO_GRAPHICS)
		#include <cairo/cairo.h>
		#include <pango/pango.h>
		#include <pango/pangocairo.h>
	#endif
#elif defined (macintosh)
	#include "macport_on.h"
    #include <Cocoa/Cocoa.h>
	#include "macport_off.h"
#elif defined (_WIN32)
	#include "winport_on.h"
	#include <windows.h>
	#include <windowsx.h>
	#include "winport_off.h"
#endif

#include "machine.h"

#define Gui_LEFT_DIALOG_SPACING  20
#define Gui_RIGHT_DIALOG_SPACING  20
#if defined (chrome)
	#define Gui_TOP_DIALOG_SPACING  34
#else
	#define Gui_TOP_DIALOG_SPACING  14
#endif
#define Gui_BOTTOM_DIALOG_SPACING  20
#define Gui_HORIZONTAL_DIALOG_SPACING  12
#define Gui_VERTICAL_DIALOG_SPACING_SAME  12
#define Gui_VERTICAL_DIALOG_SPACING_DIFFERENT  20
#define Gui_TEXTFIELD_HEIGHT  Machine_getTextHeight ()
#define Gui_LABEL_HEIGHT  16
#define Gui_RADIOBUTTON_HEIGHT  18
#define Gui_RADIOBUTTON_SPACING  8
#define Gui_CHECKBUTTON_HEIGHT  20
#define Gui_LABEL_SPACING  8
#define Gui_OPTIONMENU_HEIGHT  20
#define Gui_PUSHBUTTON_HEIGHT  Machine_getButtonHeight ()
#define Gui_OK_BUTTON_WIDTH  69
#define Gui_CANCEL_BUTTON_WIDTH  69
#define Gui_APPLY_BUTTON_WIDTH  69

#define Gui_HOMOGENEOUS  1

#if gtk
	typedef void *GuiObject;
#elif cocoa
	Thing_declare (GuiThing);
	@protocol GuiCocoaAny
		- (GuiThing) getUserData;
		- (void) setUserData: (GuiThing) userData;
	@end
	typedef NSObject <GuiCocoaAny> *GuiObject;
	@interface GuiCocoaApplication : NSApplication @end
	@interface GuiCocoaButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaCheckButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaDrawingArea : NSView <GuiCocoaAny> @end
	@interface GuiCocoaLabel : NSTextField <GuiCocoaAny> @end
	@interface GuiCocoaList : NSView <GuiCocoaAny, NSTableViewDataSource, NSTableViewDelegate> @end
	@interface GuiCocoaMenu : NSMenu <GuiCocoaAny> @end
	@interface GuiCocoaMenuButton : NSPopUpButton <GuiCocoaAny> @end
	@interface GuiCocoaMenuItem : NSMenuItem <GuiCocoaAny> @end
	@interface GuiCocoaOptionMenu : NSPopUpButton <GuiCocoaAny> @end
	@interface GuiCocoaProgressBar : NSProgressIndicator <GuiCocoaAny> @end
	@interface GuiCocoaRadioButton : NSButton <GuiCocoaAny> @end
	@interface GuiCocoaScale : NSProgressIndicator <GuiCocoaAny> @end
	@interface GuiCocoaScrollBar : NSScroller <GuiCocoaAny>
		- (void) scrollBy: (double) step;
		- (void) magnifyBy: (double) step;
	@end
	@interface GuiCocoaScrolledWindow : NSScrollView <GuiCocoaAny> @end
	@interface GuiCocoaShell : NSWindow <GuiCocoaAny>
		- (void) setDistinctiveBackGround;   // slightly darker than Tahoe's background, in order to still see text widgets
	@end
	@interface GuiCocoaShellDelegate : NSObject <NSWindowDelegate>
	@end
	@interface GuiCocoaTextField : NSTextField <GuiCocoaAny> @end
	@interface GuiCocoaTextView : NSTextView <GuiCocoaAny, NSTextViewDelegate> @end
#elif motif
	typedef class structGuiObject *GuiObject;   // opaque

	/*
	 * Definitions of X11 types.
	 */
	typedef MSG XEvent;
	typedef unsigned char Boolean;
	typedef integer Cardinal;
	typedef unsigned int Dimension;
	typedef int Position;
	typedef void *Window;
	typedef char *String;
	typedef struct Display Display;
	/*typedef integer Time;*/
	typedef integer Atom;
	typedef struct { int /* elsewhere: char* */ name; integer value; } Arg, *ArgList;

	/*
	 * Declarations of X11 functions.
	 */
	void XMapRaised (int displayDummy, Window window);
	#define DefaultScreenOfDisplay(d)  0
	int WidthOfScreen (int screen);
	int HeightOfScreen (int screen);

	/*
	 * Definitions of Xt types.
	 */
	typedef void *XtPointer;
	typedef GuiObject *GuiObjectList;
	typedef integer XtWorkProcId, XtIntervalId;
	typedef void (*XtCallbackProc) (GuiObject w, XtPointer client_data, XtPointer call_data);
	typedef bool (*XtWorkProc) (void *client_data);
	typedef void (*XtTimerCallbackProc) (XtPointer, XtIntervalId *);
	typedef uinteger WidgetClass;
	#define False 0
	#define True 1

	/*
	 * Declarations of Xt functions.
	 */
	void XtAddCallback (GuiObject w, int kind, XtCallbackProc proc, XtPointer closure);
	XtIntervalId XtAddTimeOut (uinteger interval,
			XtTimerCallbackProc timerProc, XtPointer closure);
	XtWorkProcId XtAddWorkProc (XtWorkProc workProc, XtPointer closure);
	void GuiMainLoop ();
	void GuiNextEvent (XEvent *event);
	#define XtCalloc  Melder_calloc
	#define XtClass(w)  (w) -> widgetClass
	void XtDestroyWidget (GuiObject w);
	void XtDispatchEvent (XEvent *event);
	#define XtDisplay(w)  0
	Boolean XtIsManaged (GuiObject w);
	Boolean XtIsShell (GuiObject w);
	void XtManageChild (GuiObject w);
	void XtManageChildren (GuiObjectList children, Cardinal num_children);
	void XtMapWidget (GuiObject w);
	void XtNextEvent (XEvent *event);
	void XtRemoveTimeOut (XtIntervalId id);
	void XtRemoveWorkProc (XtWorkProcId id);
	void XtSetKeyboardFocus (GuiObject tree, GuiObject descendant);
	void XtSetSensitive (GuiObject w, Boolean value);
	void XtUnmanageChild (GuiObject self);
	void XtUnmanageChildren (GuiObjectList children, Cardinal num_children);
	void * GuiWin_initialize1 (conststring32 name);
	void GuiWin_initialize2 (unsigned int argc, char **argv);
	void GuiApp_setApplicationShell (GuiObject shell);
	GuiObject XtVaCreateWidget (const char *name, int widgetClass, GuiObject parent, ...);
	GuiObject XtVaCreateManagedWidget (const char *name, int widgetClass, GuiObject parent, ...);
	void XtVaGetValues (GuiObject w, ...);
	void XtVaSetValues (GuiObject w, ...);
	Window XtWindow (GuiObject w);
	integer Gui_getNumberOfMotifWidgets ();

	/*
	 * Xm widget classes.
	 */
	#define xmBulletinBoardWidgetClass  0x0000'0001
	#define xmDrawingAreaWidgetClass  0x0000'0002
	#define xmFormWidgetClass  0x0000'0004
	#define xmFrameWidgetClass  0x0000'0008
	#define xmLabelWidgetClass  0x0000'0010
	#define xmListWidgetClass  0x0000'0020
	#define xmMenuBarWidgetClass  0x0000'0040
	#define xmPulldownMenuWidgetClass  0x0000'0100
	#define xmPushButtonWidgetClass  0x0000'0200
	#define xmRowColumnWidgetClass  0x0000'0400
	#define xmScaleWidgetClass  0x0000'0800
	#define xmScrollBarWidgetClass  0x0000'1000
	#define xmScrolledWindowWidgetClass  0x0000'2000
	#define xmSeparatorWidgetClass  0x0000'4000
	#define xmShellWidgetClass  0x0000'8000
	#define xmTextWidgetClass  0x0001'0000
	#define xmToggleButtonWidgetClass  0x0002'0000
	#define xmCascadeButtonWidgetClass  0x0004'0000
	#define xmPushButtonGadgetClass  xmPushButtonWidgetClass
	#define xmCascadeButtonGadgetClass  xmCascadeButtonWidgetClass
	#define xmSeparatorGadgetClass  xmSeparatorWidgetClass
	#define xmToggleButtonGadgetClass  xmToggleButtonWidgetClass

	/*
	 * Xm resource names.
	 */
	enum {
		XmNnull,
		#define motif_RESOURCE(xxx)  xxx,
		#include "motifEmulator_resources.h"
		#undef motif_RESOURCE
		XmNend
	};

	/*
	 * Xm enumerated types.
	 * All start at 1, because we have to reserve zero for the dynamic default value.
	 * This is important for the resources set during creation.
	 */
	enum /* dialog styles */ { XmDIALOG_MODELESS = 1, XmDIALOG_FULL_APPLICATION_MODAL };
	enum /* orientation */ { XmVERTICAL = 1, XmHORIZONTAL };
	enum /* attachment */ { XmATTACH_NONE = 1, XmATTACH_FORM, XmATTACH_POSITION };
	enum /* rowColumn types */ { XmWORK_AREA = 1, XmMENU_BAR };
	enum /* delete responses */ { XmDESTROY = 1, XmUNMAP, XmDO_NOTHING };
	enum /* indicator types */ { XmONE_OF_MANY = 1, XmN_OF_MANY };

	/*
	 * Declarations of Xm functions.
	 */
	void XmAddWMProtocolCallback (GuiObject shell, Atom protocol, XtCallbackProc callback, char *closure);
	GuiObject XmCreateBulletinBoard (GuiObject, const char *, ArgList, int);  
	GuiObject XmCreateBulletinBoardDialog (GuiObject, const char *, ArgList, int);  
	GuiObject XmCreateCascadeButton (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateCascadeButtonGadget (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateDialogShell (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateForm (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateFormDialog (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateMenuBar (GuiObject, const char *, ArgList, int);
	GuiObject XmCreatePulldownMenu (GuiObject, const char *, ArgList, int);   
	GuiObject XmCreateRadioBox (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateRowColumn (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScale (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScrolledWindow (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateScrollBar (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateSeparator (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateSeparatorGadget (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateShell (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateToggleButton (GuiObject, const char *, ArgList, int);
	GuiObject XmCreateToggleButtonGadget (GuiObject, const char *, ArgList, int);   
	void XmScaleGetValue (GuiObject widget, int *value_return);
	void XmScaleSetValue (GuiObject widget, int value);
	void XmScrollBarGetValues (GuiObject me, int *value, int *sliderSize, int *increment, int *pageIncrement);
	void XmScrollBarSetValues (GuiObject me, int value, int sliderSize, int increment, int pageIncrement, Boolean notify);
	Boolean XmToggleButtonGadgetGetState (GuiObject widget);
	#define XmToggleButtonGetState XmToggleButtonGadgetGetState
	void XmToggleButtonGadgetSetState (GuiObject widget, Boolean value, Boolean notify);
	#define XmToggleButtonSetState XmToggleButtonGadgetSetState

	void motif_win_setUserMessageCallback (int (*userMessageCallback) (void));

	constexpr UINT WM_APP_WORK_PROC = WM_APP + 0x5C;   // like "script"
	struct GuiWinWorkProcWrapper_base {
		virtual void run () = 0;
		virtual ~GuiWinWorkProcWrapper_base () { }
	};
	template <typename F>
	struct GuiWinWorkProcWrapper_derived : GuiWinWorkProcWrapper_base {
		F function;
		GuiWinWorkProcWrapper_derived (F&& f): function (std::move (f)) { }
		void run () override {
			function ();
		}
	};
#else
	typedef void *GuiObject;
#endif

/*
	`Gui_addWorkProc` can post any lambda to be scheduled during an idle moment in the main event loop.

	Example of usage:
		autostring32 clickedText = ...;
		Gui_addWorkProc (
			[clickedText = std::move (clickedText)] () mutable {   // transfer ownership of clickedText to the lambda
				try {
					autoPraatBackground background;
					Interpreter_resume (thePauseForm_interpreterReference);
				} catch (MelderError) {
					if (thePauseForm_clicked == theCancelContinueButton)
						Melder_throw (U"This happened after you cancelled the pause form.");
					else
						Melder_throw (U"This happened after you clicked “", clickedText.get(), U"” in the pause form.");
				}
			}
		);
 */
template <typename F>
static void Gui_addWorkProc (F&& function) {
	#if gtk
		auto *functionOnTheHeap = new std::decay_t <F> (std::forward <F> (function));
		g_idle_add (
			[] (gpointer closure) -> gboolean {
				auto *functionOnTheHeapFromClosure = static_cast <std::decay_t <F>*> (closure);
				try {
					(*functionOnTheHeapFromClosure) ();
				} catch (MelderError) {
					Melder_flushError ();   // no error message should leave the event loop
				}
				delete functionOnTheHeapFromClosure;   // called even if there was an exception
				return G_SOURCE_REMOVE;
			},
			functionOnTheHeap
		);
	#elif cocoa
		auto *functionOnTheHeap = new std::decay_t <F> (std::forward <F> (function));
		CFRunLoopPerformBlock (
			CFRunLoopGetMain (),
			kCFRunLoopCommonModes,
			^{
				try {
					(*functionOnTheHeap) ();
				} catch (MelderError) {
					Melder_flushError ();   // no error message should leave the event loop
				}
				delete functionOnTheHeap;   // called even if there was an exception
			}
		);
		CFRunLoopWakeUp (CFRunLoopGetMain ());
	#elif motif
		auto *functionWrapper = new GuiWinWorkProcWrapper_derived <F> (std::forward <F> (function));
		PostMessage (HWND (nullptr), WM_APP_WORK_PROC, 0, reinterpret_cast <LPARAM> (functionWrapper));
	#elif defined (NO_GUI)
	#else
		#error The function `Gui_addWorkProc`() is not implemented for this platform.
	#endif
}

int Gui_getResolution (GuiObject widget);
void Gui_getWindowPositioningBounds (double *x, double *y, double *width, double *height);

Thing_declare (GuiDrawingArea);
Thing_declare (GuiForm);
Thing_declare (GuiMenu);
Thing_declare (GuiScrolledWindow);
Thing_declare (GuiShell);
Thing_declare (GuiWindow);

Thing_define (GuiThing, Thing) {
	GuiShell d_shell;
	GuiThing d_parent;
	GuiObject d_widget;

	virtual void v_show ();
	virtual void v_hide ();
	virtual void v_setSensitive (bool sensitive);
};

void GuiThing_show (GuiThing me);
void GuiThing_hide (GuiThing me);
void GuiThing_setSensitive (GuiThing me, bool sensitive);

Thing_define (GuiControl, GuiThing) {
	int d_left, d_right, d_top, d_bottom;
	bool d_blockValueChangedCallbacks;

	virtual void v_positionInForm (GuiObject widget, int left, int right, int top, int bottom, GuiForm parent);
	virtual void v_positionInScrolledWindow (GuiObject widget, int width, int height, GuiScrolledWindow parent);
};

int GuiControl_getX (GuiControl me);
int GuiControl_getY (GuiControl me);
int GuiControl_getWidth  (GuiControl me);
int GuiControl_getHeight (GuiControl me);
void GuiControl_move (GuiControl me, int x, int y);
void GuiControl_setSize (GuiControl me, int width, int height);

Thing_define (GuiForm, GuiControl) {
};

using GuiShell_GoAwayCallback = MelderCallback <void, structThing /* boss */>;

Thing_define (GuiShell, GuiForm) {
	int d_width, d_height;
	#if gtk
		GtkWindow *d_gtkWindow;
		#if defined (chrome)
			GtkWidget *chrome_surrogateShellTitleLabelWidget;
		#endif
	#elif cocoa
		GuiCocoaShell *d_cocoaShell;
	#elif motif
		GuiObject d_xmShell;
	#endif
	GuiShell_GoAwayCallback d_goAwayCallback;
	Thing d_goAwayBoss;
	GuiDrawingArea drawingArea;

	void v9_destroy () noexcept
		override;
};

int GuiShell_getShellWidth  (GuiShell me);   // needed because GuiControl_getWidth yields the width of the inner form
int GuiShell_getShellHeight (GuiShell me);
void GuiShell_setTitle (GuiShell me, conststring32 title /* cattable */);
void GuiShell_drain (GuiShell me, bool waitUntilItHasHappened, bool allowOtherEvents);   // force display of update regions (forces the handling of an expose event)

/********** GuiButton **********/

Thing_declare (GuiButton);

typedef struct structGuiButtonEvent {
	GuiButton button;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed;
} *GuiButtonEvent;

using GuiButton_ActivateCallback = MelderCallback <void, structThing /* boss */, GuiButtonEvent>;

Thing_define (GuiButton, GuiControl) {
	GuiButton_ActivateCallback d_activateCallback;
	Thing d_activateBoss;
	GuiMenu d_menu;   // for cascade buttons
	integer d_sequentalIdInDialog;
};

/* GuiButton creation flags: */
#define GuiButton_DEFAULT  1
#define GuiButton_CANCEL  2
#define GuiButton_INSENSITIVE  4
#define GuiButton_ATTRACTIVE  8
#define GuiButton_MULTILINE  16
GuiButton GuiButton_create (GuiForm parent,
	int left, int right, int top, int bottom,
	conststring32 text,
	GuiButton_ActivateCallback activateCallback, Thing boss,
	uint32 flags
);
GuiButton GuiButton_createShown (GuiForm parent,
	int left, int right, int top, int bottom,
	conststring32 text,
	GuiButton_ActivateCallback activateCallback, Thing boss,
	uint32 flags
);

void GuiButton_setText (GuiButton me, conststring32 text /* cattable */);

/********** GuiCheckButton **********/

Thing_declare (GuiCheckButton);

typedef struct structGuiCheckButtonEvent {
	GuiCheckButton toggle;
} *GuiCheckButtonEvent;

using GuiCheckButton_ValueChangedCallback = MelderCallback <void, structThing /* boss */, GuiCheckButtonEvent>;

Thing_define (GuiCheckButton, GuiControl) {
	GuiCheckButton_ValueChangedCallback d_valueChangedCallback;
	Thing d_valueChangedBoss;
};

/* GuiCheckButton creation flags: */
#define GuiCheckButton_SET  1
#define GuiCheckButton_INSENSITIVE  2
GuiCheckButton GuiCheckButton_create (GuiForm parent,
	int left, int right, int top, int bottom,
	conststring32 text,
	GuiCheckButton_ValueChangedCallback valueChangedCallback, Thing boss,
	uint32 flags
);
GuiCheckButton GuiCheckButton_createShown (GuiForm parent,
	int left, int right, int top, int bottom,
	conststring32 text,
	GuiCheckButton_ValueChangedCallback valueChangedCallback, Thing boss,
	uint32 flags
);

bool GuiCheckButton_getValue (GuiCheckButton me);
void GuiCheckButton_setValue (GuiCheckButton me, bool value);

/********** GuiDialog **********/

using GuiDialog_DefaultCallback = MelderCallback <void, structThing /* boss */>;

Thing_define (GuiDialog, GuiShell) {
	GuiDialog_DefaultCallback d_defaultCallback;
	Thing d_defaultBoss;
	integer latestCreatedButtonId, clickedButtonId, defaultButtonId;   // especially if the dialog is blocking
	GuiButton defaultButton;
};

/* GuiDialog creation flags: */
enum class GuiDialog_Modality { MODELESS = 0, MODAL = 1, BLOCKING = 2 };
GuiDialog GuiDialog_create (GuiWindow parent,
	int x, int y, int width, int height,
	conststring32 title,
	GuiShell_GoAwayCallback goAwayCallback, Thing goAwayBoss,
	GuiDialog_Modality modality
);

void GuiDialog_setDefaultCallback (GuiDialog me, GuiDialog_DefaultCallback callback, Thing boss);

integer GuiDialog_run (GuiDialog me);
/*
	Only for blocking dialogs.
	Return value:
		- if she clicked a button (with text), the sequentual id of the clicked button (1, 2, 3...) is returned
		- if she clicked the Close button of the window, 0 is returned
		- if she typed Enter, and there was a default button, the sequentual id of the default button is returned
		- if she typed Enter and there was no default button, 0 is returned (i.e. this is the same as clicking the Close button)
*/

/********** GuiDrawingArea **********/

Thing_declare (GuiDrawingArea);
Thing_declare (GuiScrollBar);

typedef struct structGuiDrawingArea_ExposeEvent {
	GuiDrawingArea widget;
	int x, y, width, height;
} *GuiDrawingArea_ExposeEvent;

typedef struct structGuiDrawingArea_MouseEvent {
	GuiDrawingArea widget;
	int x, y;
	enum class Phase { CLICK, DRAG, DROP } phase;
	bool isClick() const { return our phase == Phase::CLICK; }
	bool isDrag()  const { return our phase == Phase::DRAG; }
	bool isDrop()  const { return our phase == Phase::DROP; }
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed;
	bool isLeftBottomFunctionKeyPressed () const {
		return theCommandKeyIsToTheLeftOfTheOptionKey ? our commandKeyPressed : our optionKeyPressed;
	}
	bool isRightBottomFunctionKeyPressed () const {
		return theCommandKeyIsToTheLeftOfTheOptionKey ? our optionKeyPressed : our commandKeyPressed;
	}
} *GuiDrawingArea_MouseEvent;

typedef struct structGuiDrawingArea_KeyEvent {
	GuiDrawingArea widget;
	char32 key;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed;
} *GuiDrawingArea_KeyEvent;

typedef struct structGuiDrawingArea_ResizeEvent {
	GuiDrawingArea widget;
	int width, height;
} *GuiDrawingArea_ResizeEvent;

typedef struct structGuiDrawingArea_ZoomEvent {
	GuiDrawingArea widget;
	double delta;   // positive means enlarge (zoom in), negative means shrink (zoom out)
} *GuiDrawingArea_ZoomEvent;

using GuiDrawingArea_ExposeCallback = MelderCallback <void, structThing /* boss */, GuiDrawingArea_ExposeEvent>;
using GuiDrawingArea_MouseCallback  = MelderCallback <void, structThing /* boss */, GuiDrawingArea_MouseEvent >;
using GuiDrawingArea_KeyCallback    = MelderCallback <void, structThing /* boss */, GuiDrawingArea_KeyEvent   >;
using GuiDrawingArea_ResizeCallback = MelderCallback <void, structThing /* boss */, GuiDrawingArea_ResizeEvent>;
using GuiDrawingArea_ZoomCallback   = MelderCallback <void, structThing /* boss */, GuiDrawingArea_ZoomEvent  >;

Thing_define (GuiDrawingArea, GuiControl) {
	GuiScrollBar d_horizontalScrollBar, d_verticalScrollBar;   // for swiping
	GuiDrawingArea_ExposeCallback d_exposeCallback;
	Thing d_exposeBoss;
	GuiDrawingArea_MouseCallback mouseCallback;
	Thing mouseBoss;
	GuiDrawingArea_KeyCallback d_keyCallback;
	Thing d_keyBoss;
	GuiDrawingArea_ResizeCallback d_resizeCallback;
	Thing d_resizeBoss;
	GuiDrawingArea_ZoomCallback d_zoomCallback;
	Thing d_zoomBoss;
	integer numberOfGraphicses;
	constexpr static integer MAXIMUM_NUMBER_OF_GRAPHICSES = 10;
	Graphics graphicses [1+MAXIMUM_NUMBER_OF_GRAPHICSES];

	void v9_destroy () noexcept
		override;
};

/* GuiDrawingArea creation flags: */
#define GuiDrawingArea_BORDER  1
GuiDrawingArea GuiDrawingArea_create (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags);
GuiDrawingArea GuiDrawingArea_createShown (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags);
GuiDrawingArea GuiDrawingArea_create (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags);
GuiDrawingArea GuiDrawingArea_createShown (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_MouseCallback mouseCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback,
	GuiDrawingArea_ZoomCallback zoomCallback, Thing boss,
	uint32 flags);

void GuiDrawingArea_setSwipable (GuiDrawingArea me, GuiScrollBar horizontalScrollBar, GuiScrollBar verticalScrollBar);
void GuiDrawingArea_setExposeCallback (GuiDrawingArea me, GuiDrawingArea_ExposeCallback callback, Thing boss);
void GuiDrawingArea_setMouseCallback (GuiDrawingArea me, GuiDrawingArea_MouseCallback callback, Thing boss);
void GuiDrawingArea_setResizeCallback (GuiDrawingArea me, GuiDrawingArea_ResizeCallback callback, Thing boss);
void GuiDrawingArea_setZoomCallback (GuiDrawingArea me, GuiDrawingArea_ZoomCallback callback, Thing boss);

/********** GuiFileSelect **********/

autoStringSet GuiFileSelect_getInfileNames (GuiWindow optionalParent, conststring32 title, bool allowMultipleFiles);
autostring32 GuiFileSelect_getOutfileName (GuiWindow optionalParent, conststring32 title, conststring32 defaultName);
autostring32 GuiFileSelect_getFolderName (GuiWindow optionalParent, conststring32 title);

/********** GuiForm **********/

GuiForm GuiForm_createInScrolledWindow (GuiScrolledWindow parent);

/********** GuiLabel **********/

//Thing_declare (GuiLabel);

Thing_define (GuiLabel, GuiControl) {
};

/* GuiLabel creation flags: */
#define GuiLabel_CENTRE  1
#define GuiLabel_RIGHT  2
#define GuiLabel_BOLD  4
#define GuiLabel_MULTILINE  8
GuiLabel GuiLabel_create      (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 text, uint32 flags);
GuiLabel GuiLabel_createShown (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 text, uint32 flags);

void GuiLabel_setText (GuiLabel me, conststring32 text /* cattable */);

/********** GuiList **********/

Thing_declare (GuiList);
Thing_declare (GuiScrolledWindow);

typedef struct structGuiList_SelectionChangedEvent {
	GuiList list;
} *GuiList_SelectionChangedEvent;
using GuiList_SelectionChangedCallback = MelderCallback <void, structThing /* boss */, GuiList_SelectionChangedEvent>;

typedef struct structGuiList_DoubleClickEvent {
	GuiList list;
} *GuiList_DoubleClickEvent;
using GuiList_DoubleClickCallback = MelderCallback <void, structThing /* boss */, GuiList_DoubleClickEvent>;

typedef struct structGuiList_ScrollEvent {
	GuiList list;
} *GuiList_ScrollEvent;
using GuiList_ScrollCallback = MelderCallback <void, structThing /* boss */, GuiList_ScrollEvent>;

Thing_define (GuiList, GuiControl) {
	bool d_allowMultipleSelection;
	GuiList_SelectionChangedCallback d_selectionChangedCallback;
	Thing d_selectionChangedBoss;
	GuiList_DoubleClickCallback d_doubleClickCallback;
	Thing d_doubleClickBoss;
	GuiList_ScrollCallback d_scrollCallback;
	Thing d_scrollBoss;
	#if gtk
		GtkListStore *d_liststore;
	#endif
};

GuiList GuiList_create      (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, conststring32 header);
GuiList GuiList_createShown (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, conststring32 header);

void GuiList_deleteAllItems (GuiList me);
void GuiList_deleteItem (GuiList me, integer position);
void GuiList_deselectAllItems (GuiList me);
void GuiList_deselectItem (GuiList me, integer position);
integer GuiList_getBottomPosition (GuiList me);
integer GuiList_getNumberOfItems (GuiList me);
autoINTVEC GuiList_getSelectedPositions (GuiList me);
integer GuiList_getTopPosition (GuiList me);

/**
	Inserts a new item into a GuiList at a given position.
	@param me
		The GuiList into which the new item is inserted.
	@param itemText
		The text of the new item. Cattable.
	@param position
		The position of the new item in the list after insertion.
		A value of 1 therefore puts the new item at the top of the list.
		A value of 0 is special: the item is put at the bottom of the list.
 */
void GuiList_insertItem  (GuiList me, conststring32 itemText /* cattable */, integer position);

void GuiList_replaceItem (GuiList me, conststring32 itemText /* cattable */, integer position);
void GuiList_setTopPosition (GuiList me, integer topPosition);
void GuiList_selectItem (GuiList me, integer position);
void GuiList_setSelectionChangedCallback (GuiList me, GuiList_SelectionChangedCallback callback, Thing boss);
void GuiList_setDoubleClickCallback (GuiList me, GuiList_DoubleClickCallback callback, Thing boss);
void GuiList_setScrollCallback (GuiList me, GuiList_ScrollCallback callback, Thing boss);

/********** GuiMenu **********/

Thing_declare (GuiMenuItem);

Thing_define (GuiMenu, GuiThing) {
	autoGuiMenuItem d_menuItem;
	autoGuiButton d_cascadeButton;
	#if gtk
		GtkMenuItem *d_gtkMenuTitle;
	#elif cocoa
		GuiCocoaMenu *d_cocoaMenu;
		GuiCocoaMenuItem *d_cocoaMenuItem;
		GuiCocoaMenuButton *d_cocoaMenuButton;
	#elif motif
		GuiObject d_xmMenuTitle;   // in case the menu is in a menu bar
		GuiObject d_xmMenuBar;   // in case the menu is in a form
	#endif

	void v_show ()
		override;
	void v_hide ()
		override;
	void v_setSensitive (bool sensitive)
		override;
};

GuiMenu GuiMenu_createInWindow (GuiWindow window, conststring32 title, uint32 flags);
GuiMenu GuiMenu_createInMenu (GuiMenu supermenu, conststring32 title, uint32 flags);
GuiMenu GuiMenu_createInForm (GuiForm form, int left, int right, int top, int bottom, conststring32 title, uint32 flags);

void GuiMenu_empty (GuiMenu me);

/********** GuiMenuItem **********/

Thing_declare (GuiMenuItem);

typedef struct structGuiMenuItemEvent {
	GuiMenuItem menuItem;
	bool shiftKeyPressed, commandKeyPressed, optionKeyPressed;
} *GuiMenuItemEvent;

using GuiMenuItemCallback = MelderCallback <void, structThing /* boss */, GuiMenuItemEvent>;

#if cocoa
	extern GuiMenuItemCallback theGuiEscapeMenuItemCallback;
	extern Thing theGuiEscapeMenuItemBoss;
#endif

Thing_define (GuiMenuItem, GuiThing) {
	GuiMenu d_menu;
	GuiMenuItemCallback d_callback;
	Thing d_boss;
	#if gtk
		bool d_callbackBlocked;
	#endif
};

/* Accelerators understood by GuiMenuItem: */
#define GuiMenu_OPTION  (1 << 24)
#define GuiMenu_SHIFT  (1 << 25)
#define GuiMenu_COMMAND  (1 << 26)
#if cocoa
	#define GuiMenu_COMMAND_EXTRA  (GuiMenu_OPTION | GuiMenu_COMMAND)   /* standard on the Mac (where the keys are next to each other) */
#else
	#define GuiMenu_COMMAND_EXTRA  (GuiMenu_SHIFT | GuiMenu_COMMAND)   /* standard on Windows (where the keys are next to each other, *and* Ctrl-Alt can mean Alt-GR) */
#endif
// 1 is the short form of GuiMenu_DEPTH_1
// 2 is the short form of GuiMenu_DEPTH_2
// 3 is the short form of GuiMenu_DEPTH_3
#define GuiMenu_LEFT_ARROW  4
#define GuiMenu_RIGHT_ARROW  5
#define GuiMenu_UP_ARROW  6
#define GuiMenu_DOWN_ARROW  7
#define GuiMenu_PAUSE  8
#define GuiMenu_DELETE  9
#define GuiMenu_INSERT  10
#define GuiMenu_BACKSPACE  11
#define GuiMenu_TAB  12
#define GuiMenu_LINEFEED  13
#define GuiMenu_HOME  14
#define GuiMenu_END  15
#define GuiMenu_ENTER  16
#define GuiMenu_PAGE_UP  17
#define GuiMenu_PAGE_DOWN  18
#define GuiMenu_ESCAPE  19
#define GuiMenu_F1  20
#define GuiMenu_F2  21
#define GuiMenu_F3  22
#define GuiMenu_F4  23
#define GuiMenu_F5  24
#define GuiMenu_F6  25
#define GuiMenu_F7  26
#define GuiMenu_F8  27
#define GuiMenu_F9  28
#define GuiMenu_F10  29
#define GuiMenu_F11  30
#define GuiMenu_F12  31
// or any ASCII character (preferably a letter or digit) between 32 and 126

/* Button layout and state understood by GuiMenuItem: */
#define GuiMenu_INSENSITIVE  (1 << 8)
#define GuiMenu_CHECKBUTTON  (1 << 9)
#define GuiMenu_TOGGLE_ON  (1 << 10)
#define GuiMenu_ATTRACTIVE  (1 << 11)
#define GuiMenu_UNDERLINED  (1 << 12)
#define GuiMenu_RADIO_FIRST  (1 << 13)
#define GuiMenu_RADIO_NEXT  (1 << 14)
#define GuiMenu_BUTTON_STATE_MASK  (GuiMenu_INSENSITIVE|GuiMenu_CHECKBUTTON|GuiMenu_TOGGLE_ON|GuiMenu_ATTRACTIVE|GuiMenu_UNDERLINED|GuiMenu_RADIO_FIRST|GuiMenu_RADIO_NEXT)

/* These are ingored by GuiMenu_addItem, but can be used by higher-level command logic: */
#define GuiMenu_DEPTH_1  0x0001'0000   /* 1<<16 */
#define GuiMenu_DEPTH_2  0x0002'0000   /* 1<<17 */
#define GuiMenu_DEPTH_3  0x0003'0000
#define GuiMenu_DEPTH_4  0x0004'0000   /* 1<<18 */
#define GuiMenu_DEPTH_5  0x0005'0000
#define GuiMenu_DEPTH_6  0x0006'0000
#define GuiMenu_DEPTH_7  0x0007'0000
#define GuiMenu_HIDDEN  0x0008'0000   /* 1<<19 */
#define GuiMenu_UNHIDABLE  0x0010'0000   /* 1<<20 */
#define GuiMenu_NO_API  0x0020'0000   /* 1<<21 */
#define GuiMenu_FORCE_API  0x0040'0000   /* 1<<22 */
#define GuiMenu_DEPRECATED  (0x0080'0000 /* 1<<23 */ | GuiMenu_HIDDEN)
#define GuiMenu_DEPRECATED_2004  (0x0400'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2005  (0x0500'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2006  (0x0600'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2007  (0x0700'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2008  (0x0800'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2009  (0x0900'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2010  (0x0A00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2011  (0x0B00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2012  (0x0C00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2013  (0x0D00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2014  (0x0E00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2015  (0x0F00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2016  (0x1000'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2017  (0x1100'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2018  (0x1200'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2019  (0x1300'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2020  (0x1400'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2021  (0x1500'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2022  (0x1600'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2023  (0x1700'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2024  (0x1800'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2025  (0x1900'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2026  (0x1A00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2027  (0x1B00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2028  (0x1C00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2029  (0x1D00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2030  (0x1E00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2031  (0x1F00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2032  (0x2000'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2033  (0x2100'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2034  (0x2200'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2035  (0x2300'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2036  (0x2400'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2037  (0x2500'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2038  (0x2600'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2039  (0x2700'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2040  (0x2800'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2041  (0x2900'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2042  (0x2A00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2043  (0x2B00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2044  (0x2C00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2045  (0x2D00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2046  (0x2E00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2047  (0x2F00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2048  (0x3000'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2049  (0x3100'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2050  (0x3200'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2051  (0x3300'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2052  (0x3400'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2053  (0x3500'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2054  (0x3600'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2055  (0x3700'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2056  (0x3800'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2057  (0x3900'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2058  (0x3A00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2059  (0x3B00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2060  (0x3C00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2061  (0x3D00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2062  (0x3E00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2063  (0x3F00'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2064  (0x4000'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2065  (0x4100'0000 | GuiMenu_DEPRECATED)
#define GuiMenu_DEPRECATED_2066  (0x4200'0000 | GuiMenu_DEPRECATED)

GuiMenuItem GuiMenu_addItem (GuiMenu menu, conststring32 title, uint32 flags,
		GuiMenuItemCallback callback, Thing boss);
/* Flags is a combination of the above defines (both layout and accelerators). */
GuiMenuItem GuiMenu_addSeparator (GuiMenu menu);

void GuiMenuItem_check (GuiMenuItem me, bool check);
void GuiMenuItem_setText (GuiMenuItem me, conststring32 text);

/********** GuiOptionMenu **********/

Thing_define (GuiOptionMenu, GuiControl) {
	int d_value;
	OrderedOf<structGuiMenuItem> d_options;
	#if gtk
		//GtkComboBox *d_gtkCascadeButton;
	#elif cocoa
	#elif motif
		GuiObject d_xmMenuBar, d_xmCascadeButton;
	#endif

	void v_show ()
		override;
};

void GuiOptionMenu_init (GuiOptionMenu me, GuiForm parent, int left, int right, int top, int bottom, uint32 flags);
GuiOptionMenu GuiOptionMenu_create        (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);
GuiOptionMenu GuiOptionMenu_createShown   (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);

void GuiOptionMenu_addOption (GuiOptionMenu me, conststring32 text);
int GuiOptionMenu_getValue (GuiOptionMenu me);
void GuiOptionMenu_setValue (GuiOptionMenu me, int value);

/********** GuiProgressBar **********/

Thing_declare (GuiProgressBar);

Thing_define (GuiProgressBar, GuiControl) {
	#if cocoa
		GuiCocoaProgressBar *d_cocoaProgressBar;
	#endif
};

GuiProgressBar GuiProgressBar_create      (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);
GuiProgressBar GuiProgressBar_createShown (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);

void GuiProgressBar_setValue (GuiProgressBar me, double value);   // between 0.0 and 1.0

/********** GuiRadioButton **********/

Thing_declare (GuiRadioButton);

typedef struct structGuiRadioButtonEvent {
	GuiRadioButton toggle;
	int position;
} *GuiRadioButtonEvent;

using GuiRadioButtonCallback = MelderCallback <void, structThing /* boss */, GuiRadioButtonEvent>;

Thing_define (GuiRadioButton, GuiControl) {
	GuiRadioButton d_previous, d_next;   // there's a linked list of grouped radio buttons
	GuiRadioButtonCallback d_valueChangedCallback;
	Thing d_valueChangedBoss;
	#if cocoa
		GuiCocoaRadioButton *d_cocoaRadioButton;
	#endif
};

/* GuiRadioButton creation flags: */
#define GuiRadioButton_SET  1
#define GuiRadioButton_INSENSITIVE  2
GuiRadioButton GuiRadioButton_create      (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiRadioButtonCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags);
GuiRadioButton GuiRadioButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiRadioButtonCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags);

void GuiRadioGroup_begin ();
void GuiRadioGroup_end ();

bool GuiRadioButton_getValue (GuiRadioButton me);
void GuiRadioButton_set (GuiRadioButton me);

/********** GuiScale **********/

Thing_declare (GuiScale);

Thing_define (GuiScale, GuiControl) { public:
	#if cocoa
		GuiCocoaScale *d_cocoaScale;
	#endif
};

GuiScale GuiScale_create      (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, uint32 flags);
GuiScale GuiScale_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int minimum, int maximum, int value, uint32 flags);

int GuiScale_getValue (GuiScale me);
void GuiScale_setValue (GuiScale me, int value);

/********** GuiScrollBar **********/

Thing_declare (GuiScrollBar);

typedef struct structGuiScrollBarEvent {
	GuiScrollBar scrollBar;
} *GuiScrollBarEvent;

using GuiScrollBarCallback = MelderCallback <void, structThing /* boss */, GuiScrollBarEvent>;

Thing_define (GuiScrollBar, GuiControl) {
	GuiScrollBarCallback d_valueChangedCallback;
	Thing d_valueChangedBoss;
};

/* GuiScrollBar creation flags: */
#define GuiScrollBar_HORIZONTAL  1
GuiScrollBar GuiScrollBar_create      (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	GuiScrollBarCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags);
GuiScrollBar GuiScrollBar_createShown (GuiForm parent, int left, int right, int top, int bottom,
	double minimum, double maximum, double value, double sliderSize, double increment, double pageIncrement,
	GuiScrollBarCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags);

double GuiScrollBar_getValue (GuiScrollBar me);
double GuiScrollBar_getSliderSize (GuiScrollBar me);
void GuiScrollBar_set (GuiScrollBar me, double minimum, double maximum, double value,
	double sliderSize, double increment, double pageIncrement);

/********** GuiScrolledWindow **********/

Thing_declare (GuiScrolledWindow);

Thing_define (GuiScrolledWindow, GuiControl) { public:
};

GuiScrolledWindow GuiScrolledWindow_create      (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, uint32 flags);
GuiScrolledWindow GuiScrolledWindow_createShown (GuiForm parent, int left, int right, int top, int bottom,
	int horizontalScrollbarPersistence, int verticalScrollbarPersistence, uint32 flags);

/********** GuiText **********/

Thing_declare (GuiText);

typedef struct structGuiTextEvent {
	GuiText text;
} *GuiTextEvent;

using GuiText_ChangedCallback = MelderCallback <void, structThing /* boss */, GuiTextEvent>;

#if gtk
	typedef gchar * history_data;
#else
	typedef char * history_data;
#endif

struct history_entry {
	history_entry *prev, *next;
	integer first, last;
	history_data text;
	bool type_del : 1;
};

Thing_define (GuiText, GuiControl) {
	GuiText_ChangedCallback d_changedCallback;
	Thing d_changedBoss;
	uint32 flags;
	#if cocoa
		GuiCocoaScrolledWindow *d_cocoaScrollView;
		GuiCocoaTextView *d_cocoaTextView;
		double d_macFontSize;
	#elif defined (macintosh)
		TXNObject d_macMlteObject;
		TXNFrameID d_macMlteFrameId;
	#else
		history_entry *d_prev, *d_next;
		GuiMenuItem d_undo_item, d_redo_item;
		bool d_history_change : 1;
	#endif
	#if motif
		bool d_editable;
	#endif
};

/* GuiText creation flags: */
#define GuiText_SCROLLED  1
#define GuiText_CHARWRAP  2
#define GuiText_INKWRAP  4
#define GuiText_ANYWRAP  (GuiText_CHARWRAP | GuiText_INKWRAP)
#define GuiText_NONEDITABLE  8
GuiText GuiText_create      (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);
GuiText GuiText_createShown (GuiForm parent, int left, int right, int top, int bottom, uint32 flags);

void GuiText_copy (GuiText me);
void GuiText_cut (GuiText me);
autostring32 GuiText_getSelection (GuiText me);
autostring32 GuiText_getString (GuiText me);
autostring32 GuiText_getStringAndSelectionPosition (GuiText me, integer *first, integer *last);
void GuiText_paste (GuiText me);
void GuiText_redo (GuiText me);
void GuiText_remove (GuiText me);
void GuiText_replace (GuiText me, integer from_pos, integer to_pos, conststring32 value);
void GuiText_scrollToSelection (GuiText me);
void GuiText_setChangedCallback (GuiText me, GuiText_ChangedCallback changedCallback, Thing changedBoss);
void GuiText_setFontSize (GuiText me, double size);
void GuiText_setRedoItem (GuiText me, GuiMenuItem item);
void GuiText_setSelection (GuiText me, integer first, integer last);
void GuiText_setString (GuiText me, conststring32 text, bool undoable = true);
void GuiText_setUndoItem (GuiText me, GuiMenuItem item);
void GuiText_undo (GuiText me);

/********** GuiWindow **********/

Thing_define (GuiWindow, GuiShell) {
	#if gtk
		GtkMenuBar *d_gtkMenuBar;
	#elif cocoa
		int d_menuBarWidth;
		GuiMenuItemCallback d_tabCallback;
		Thing d_tabBoss;
		GuiMenuItemCallback d_shiftTabCallback;
		Thing d_shiftTabBoss;
		GuiMenuItemCallback d_enterCallback;
		Thing d_enterBoss;
		GuiMenuItemCallback d_optionBackspaceCallback;
		Thing d_optionBackspaceBoss;
		GuiMenuItemCallback d_escapeCallback;
		Thing d_escapeBoss;
	#elif motif
		GuiObject d_xmMenuBar;
	#endif
};

/* GuiWindow creation flags: */
#define GuiWindow_FULLSCREEN  1
GuiWindow GuiWindow_create (int x, int y, int width, int height, int minimumWidth, int minimumHeight,
	conststring32 title /* cattable */, GuiShell_GoAwayCallback goAwayCallback, Thing goAwayBoss, uint32 flags);
	// returns a Form widget that has a new Shell parent.

void GuiWindow_addMenuBar (GuiWindow me);
bool GuiWindow_setDirty (GuiWindow me, bool dirty);
	/*
		Purpose: on OSX you get a little dot in the red close button,
			and the window proxy icon dims.
		Return value:
			`true` if the system supports this feature, `false` if not;
			the point of this is that you can use a different user feedback strategy, like appending
			the text "(modified)" to the window title, if this feature is not supported.
	*/
void GuiWindow_setFile (GuiWindow me, MelderFile file);
	/*
		Purpose: set the window title, and (on MacOS X) the window proxy icon and the window path menu.
	*/

void GuiObject_destroy (GuiObject me);

/********** EVENTS **********/

#if defined (macintosh)
void Gui_setOpenDocumentCallback (void (*openDocumentCallback) (MelderFile file), void (*finishedOpeningDocumentsCallback) ());
#endif

void Gui_setQuitApplicationCallback (void (*quitApplicationCallback) ());
void Gui_runQuitApplicationCallback ();

extern uinteger theGuiTopLowAccelerators [8];

/*
	'parent' is the top-level widget.
*/
void Gui_injectMessageProcs (GuiWindow parent);

/* End of file Gui.h */
#endif
