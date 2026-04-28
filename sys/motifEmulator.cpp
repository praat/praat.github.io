/* motifEmulator.cpp
 *
 * Copyright (C) 1993-2026 Paul Boersma
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "melder.h"
#include "GuiP.h"
#include "machine.h"

static void (*theQuitApplicationCallback) ();

void Gui_setQuitApplicationCallback (void (*quitApplicationCallback) ()) {
	theQuitApplicationCallback = quitApplicationCallback;
}
void Gui_runQuitApplicationCallback () {
	theQuitApplicationCallback ();
}

#if defined (_WIN32)
#define TRY_BARLESS  0

/* The Motif emulator for Windows. */

#define PRAAT_WINDOW_CLASS_NUMBER  1

/*
 * The MEMBER macros only work if klas1 etc are no macros themselves.
 * Therefore, we undefine those who are:
 */
#undef MessageBox

#define _motif_SHIFT_MASK  1
#define _motif_COMMAND_MASK  2
#define _motif_OPTION_MASK  4

#define CELL_HEIGHT  15
#define MESSAGE_BOX_BUTTON_WIDTH  100

static void _motif_update (GuiObject me, void *event);

/********** Resource names. **********/

#define motif_RESOURCE(xxx)  #xxx,
static const char *motif_resourceNames [] = {
	"XmNnull",
	#include "motifEmulator_resources.h"
	"XmNend"
};
#undef motif_RESOURCE

/* Modes. */

struct Gui theGui;   // global variable

/********** XWindows routines. **********/

void XMapRaised (int displayDummy, Window window) {
	(void) displayDummy;
	ShowWindow ((HWND) window, SW_SHOW);
	SetForegroundWindow ((HWND) window);
}

int WidthOfScreen (int screen) {
	(void) screen;
	return GetSystemMetrics (SM_CXMAXIMIZED);
}
int HeightOfScreen (int screen) {
	(void) screen;
	return GetSystemMetrics (SM_CYMAXIMIZED);
}

/********** X Toolkit **********/

void _Gui_callCallbacks (GuiObject w, XtCallbackList *callbacks, XtPointer call) {
	for (int i = 0; i < MAXNUM_CALLBACKS; i ++)
		if (callbacks -> pairs [i]. proc)
			callbacks -> pairs [i]. proc (w, callbacks -> pairs [i]. closure, call);
}

#define MAXIMUM_NUMBER_OF_MENUS  4000
static GuiObject theMenus [1+MAXIMUM_NUMBER_OF_MENUS];   // we can freely use and reuse these menu ids
static char32 theWindowClassName [100], theDrawingAreaClassName [100], theApplicationClassName [100];
char32 * _GuiWin_getDrawingAreaClassName () { return theDrawingAreaClassName; }
static int (*theUserMessageCallback) ();
#define MINIMUM_MENU_ITEM_ID  (MAXIMUM_NUMBER_OF_MENUS + 1)
#define MAXIMUM_MENU_ITEM_ID  32767
static short theMenuItems [1+MAXIMUM_MENU_ITEM_ID];   // we can freely use and reuse the item ids 4001..32767

static GuiObject theApplicationShell;   // for global menus
static int theBackground = False;   // set by suspend and resume events; used by Motif-style activation methods
static int theDialogHint = False;   // should the shell that is currently being created, have dialog or document looks?
integer numberOfWidgets = 0;
integer Gui_getNumberOfMotifWidgets () { return numberOfWidgets; }

/* AppContext level */

static int theNumberOfWorkProcs;
static XtWorkProc theWorkProcs [10];
static XtPointer theWorkProcClosures [10];

static int theNumberOfTimeOuts;
static XtTimerCallbackProc theTimeOutProcs [10];
static XtPointer theTimeOutClosures [10];
static double theTimeOutStarts [10];
static double theTimeOutIntervals [10];

static void Native_move (GuiObject w, int dx, int dy);   // forward

static void cb_scroll (GuiObject scrollBar, XtPointer closure, XtPointer call) {
	GuiObject scrolledWindow = (GuiObject) closure;
	GuiObject workWindow = scrolledWindow -> motiff.scrolledWindow.workWindow;
	int previousShift, newShift, distance;
	bool horizontal = ( scrolledWindow -> motiff.scrolledWindow.horizontalBar == scrollBar );
	(void) call;
	if (! workWindow)
		return;
	Melder_assert (scrolledWindow -> motiff.scrolledWindow.clipWindow);
	previousShift = horizontal ?
		scrolledWindow -> motiff.scrolledWindow.clipWindow -> x - workWindow -> x :
		scrolledWindow -> motiff.scrolledWindow.clipWindow -> y - workWindow -> y;
	newShift = scrollBar -> value;
	distance = previousShift - newShift;
	if (horizontal) {
		workWindow -> x += distance;
		Native_move (workWindow, distance, 0);
	} else {
		workWindow -> y += distance;
		Native_move (workWindow, 0, distance);
	}
	GdiFlush ();   // for dragging
}

/* These are like the toolkit's query_geometry methods: */

static int Native_titleWidth (GuiObject me) {
	if (my parent -> window) {
		HDC dc = GetDC (my parent -> window);
		SIZE size;
		SelectFont (dc, theWinGuiNormalLabelFont ());   // possible BUG
		conststringW nameW = Melder_peek32toW (my name.get());
		GetTextExtentPoint32 (dc, nameW, wcslen (nameW), & size);
		ReleaseDC (my parent -> window, dc);
		return size. cx;
	} else {
		return 7 * Melder_length (my name.get());
	}
}

static int NativeLabel_preferredWidth (GuiObject me) {
	return Native_titleWidth (me) + 10;
}

static int NativeButton_preferredWidth (GuiObject me) {
	int width = Native_titleWidth (me) + 10;
	return width < 41 ? 41 : width;
}

static int NativeToggleButton_preferredWidth (GuiObject me) {
	return Native_titleWidth (me) + 25;
}

static int NativeButton_preferredHeight (GuiObject me) {
	(void) me;
	return 22;
}

/***** WIDGET *****/

GuiObject _Gui_initializeWidget (int widgetClass, GuiObject parent, conststring32 name) {
	GuiObject me = Melder_calloc_f (struct structGuiObject, 1);
	if (Melder_debug == 34)
		Melder_casual (U"from _Gui_initializeWidget\t", Melder_pointer (me), U"\t1\t", sizeof (struct structGuiObject));
	my magicNumber = 15111959;
	numberOfWidgets ++;
	my widgetClass = widgetClass;
	my parent = parent;
	my inMenu = parent && MEMBER (parent, PulldownMenu);
	my name = Melder_dup_f (name);

	/*
		Install me into my parent's list of children.
	*/
	if (! parent) {
		;
	} else if (! parent -> firstChild) {
		parent -> firstChild = me;
	} else {
		GuiObject lastChild = parent -> firstChild;
		while (lastChild -> nextSibling)
			lastChild = lastChild -> nextSibling;
		lastChild -> nextSibling = me;
		my previousSibling = lastChild;
	}

	/*
		I am in the same shell as my parent, so I inherit my parent's "shell" attribute.
	*/
	if (MEMBER (me, Shell))
		my shell = me;
	else
		my shell = ( parent ? parent -> shell : nullptr );

	/*
	 * The remainder of initialization is about positioning, sizes, attachments, and the contents of a scrolled window.
	 * All of that is irrelevant to menu items.
	 */
	if (my inMenu)
		return me;

	/* Initial defaults: mainly positioning and sizes. */

	switch (my widgetClass) {
		case xmDrawingAreaWidgetClass: {
			my x = 2;
			my y = 2;
			my width = 100;
			my height = 100;
		} break; case xmShellWidgetClass: {
			my x = 20;
			my y = 3;
			my width = 30;
			my height = 50;
			my deleteResponse = XmDESTROY;
		} break; case xmTextWidgetClass: {
			my x = 2;
			my y = 2;
			my width = 102;
			my height = Gui_TEXTFIELD_HEIGHT;
		} break; case xmPushButtonWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeButton_preferredWidth (me);
			my height = Gui_PUSHBUTTON_HEIGHT;
		} break; case xmLabelWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeLabel_preferredWidth (me);
			my height = Gui_LABEL_HEIGHT;
		} break; case xmCascadeButtonWidgetClass: {
			if (my parent -> rowColumnType == XmMENU_BAR) {
				my x = 2;
				my y = 2;
				my width = NativeButton_preferredWidth (me);
				my height = NativeButton_preferredHeight (me) + 4;   // BUG: menu bar should be large enough
			} else {
				my motiff.cascadeButton.inBar = true;
			}
		} break; case xmToggleButtonWidgetClass: {
			my x = 2;
			my y = 2;
			my width = NativeToggleButton_preferredWidth (me);
			my height = Gui_CHECKBUTTON_HEIGHT;
		} break; case xmSeparatorWidgetClass: {
			my width = parent -> width;
			my height = 10;
		} break; case xmScrollBarWidgetClass: {
			my width = 16;
			my height = 100;
		} break; case xmMenuBarWidgetClass: {
			my width = 10;
			my height = 10;
		} break; case xmRowColumnWidgetClass: {
			my width = 10;
			my height = 10;
			my orientation = XmVERTICAL;
		} break; case xmScaleWidgetClass: {
			my width = 300;
			my height = 25;
		} break; case xmFormWidgetClass: {
			if (MEMBER (parent, Shell)) {
				/*
				 * The following trick is necessary for forms that contain scroll bars.
				 */
				my width = parent -> width;
				my height = parent -> height;
			} else {
				/*
				 * EXPERIMENT:
				 * this relies on a certain sequence in building up a window: from top to bottom.
				 * If the form is inside a RowColumn of a certain width established by a widget
				 * closer to the top, this width will be copied. The height, of course, should not be copied.
				 * See Praat's SoundRecorder for an example.
				 */
				my width = parent -> width;
				my height = 10;
			}
		} break; default: {
			my width = parent ? parent -> width : 0;
			my height = parent ? parent -> height : 0;
		}
	}

	/* Some positions depend on parent. */

	if (parent) switch (parent -> widgetClass) {
		case xmFrameWidgetClass: {
			my x = 1;
			my y = 1;
			my width = parent -> width - 2;
			my height = parent -> height - 2;
		} break; case xmScrolledWindowWidgetClass: {
			my x = 1;
			my y = 1;
			my width = parent -> width - 17;   // exact fit: scroll bar (16) plus border (1)
			my height = parent -> height - 17;
			if (my widgetClass == xmTextWidgetClass) {
				my width = 3000;
				my height = 30000;
			}   // BUG: either put in GuiText or erase
		}
	}

	if (my width < 0)
		my width = 0;
	if (my height < 0)
		my height = 0;

	/* Automatic attachment of dialog to parent shell. */

	if (MEMBER2 (me, BulletinBoard, Form) && MEMBER (my parent, Shell))
		my leftAttachment = my rightAttachment = my topAttachment = my bottomAttachment = XmATTACH_FORM;

	if (MEMBER (me, CascadeButton) && str32equ (name, U"Help"))
		my rightAttachment = XmATTACH_FORM;   /* !!!!!! */

	/* A child of a scrolled window will be installed as the workWindow of that scrolled window,
	 * except if it is a scroll bar or if the clipWindow does not exist yet.
	 * This is because the creations of the scroll bars and the clip window will also arrive here.
	 * Our XmScrolledWindow creation method always creates two scroll bars and a clip window,
	 * before you can create any other children.
	 */

	if (my parent && MEMBER (my parent, ScrolledWindow) &&
		! MEMBER (me, ScrollBar) &&   // 'me' is one of the two scroll bars, or a new one
		my parent -> motiff.scrolledWindow.clipWindow   // 'me' is probably the clip window now
	)
		my parent -> motiff.scrolledWindow.workWindow = me;   // install
	return me;
}

/***** NATIVE *****/

void _GuiNativeControl_check (GuiObject me, Boolean value) {
	Button_SetCheck (my window, value ? BST_CHECKED : BST_UNCHECKED);
}

void _GuiNativeControl_destroy (GuiObject me) {
	DestroyWindow (my window);
}

void _GuiNativeControl_show (GuiObject me) {
	ShowWindow (my window, SW_SHOW);
}

void _GuiNativeControl_hide (GuiObject me) {
	ShowWindow (my window, SW_HIDE);
}

void _GuiNativeControl_setSensitive (GuiObject me) {
	EnableWindow (my window, ! my insensitive);
}

char32 * _GuiWin_expandAmpersands (conststring32 title) {
	static char32 buffer [300];
	const char32 *from = & title [0];
	char32 *to = & buffer [0];
	while (*from) { if (*from == U'&') * to ++ = U'&'; * to ++ = * from ++; } * to = U'\0';
	return buffer;
}

void _GuiNativeControl_setTitle (GuiObject me) {
	HDC dc = GetDC (my window);
	SelectPen (dc, GetStockPen (NULL_PEN));
	SelectBrush (dc, theWinGuiBackgroundBrush ());
	Rectangle (dc, 0, 0, my width, my height);
	ReleaseDC (my window, dc);
	SetWindowTextW (my window, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get())));
}

static int _XmScrollBar_check (GuiObject me) {
	if (my maximum < my minimum)
		Melder_warning (U"XmScrollBar: maximum (", my maximum, U") less than minimum (", my minimum, U").");
	else if (my sliderSize > my maximum - my minimum)
		Melder_warning (U"XmScrollBar: slider size (", my sliderSize, U") greater than maximum (",
			my maximum, U") minus minimum (", my minimum, U").");
	else if (my value < my minimum)
		Melder_warning (U"XmScrollBar: value (", my value, U") less than minimum (", my minimum, U").");
	else if (my value > my maximum - my sliderSize)
		Melder_warning (U"XmScrollBar: value (", my value, U") greater than maximum (",
			my maximum, U") minus slider size (", my sliderSize, U").");
	else
		return 1;
	return 0;
}

static void NativeScrollBar_set (GuiObject me) {
	if (! _XmScrollBar_check (me))
		return;
	SCROLLINFO scrollInfo;
	scrollInfo. cbSize = sizeof (SCROLLINFO);
	scrollInfo. fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL;
	if (my maximum == my minimum + my sliderSize) {
		scrollInfo. nMin = 0;
		scrollInfo. nMax = 0;
		scrollInfo. nPage = 1;
		scrollInfo. nPos = 0;
	} else {
		scrollInfo. nMin = 0;
		scrollInfo. nMax = 32767;
		scrollInfo. nPage = (32767.0 * my sliderSize) / (my maximum - my minimum);
		scrollInfo. nPos = ((32767.0 - scrollInfo. nPage) * (my value - my minimum)) / (my maximum - my minimum - my sliderSize);
	}
	if (my window)
		SetScrollInfo (my window, SB_CTL, & scrollInfo, true);
}

static void NativeMenuItem_delete (GuiObject me) {
	RemoveMenu (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND);
}

static int NativeMenuItem_getPosition (GuiObject me) {
	int position = 1;
	/*
	 * The following routine could also be used for Mac.
	 */
	GuiObject sibling;
	for (sibling = my parent -> firstChild; sibling; sibling = sibling -> nextSibling) {
		if (sibling == me)
			break;
		if (sibling -> managed && ! MEMBER (sibling, PulldownMenu))
			position += 1;
	}
	/*
	 * Bill Gates counts like 0, 1, 2...
	 */
	position -= 1;
	return position;
}

static void NativeMenuItem_check (GuiObject me, Boolean value) {
	if (! my managed)
		return;
	CheckMenuItem (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | ( value ? MF_CHECKED : MF_UNCHECKED ));
}

static void NativeMenuItem_setSensitive (GuiObject me) {
	//if (! my managed)
	//	return;
	if (my widgetClass == xmPulldownMenuWidgetClass) {
		Melder_assert (MEMBER (my parent, MenuBar));
		EnableMenuItem (my parent -> nat.menu.handle, my nat.menu.id, MF_BYCOMMAND | ( my insensitive ? MF_GRAYED : MF_ENABLED ));
	} else {
		EnableMenuItem (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | ( my insensitive ? MF_GRAYED : MF_ENABLED ));
	}
	//DrawMenuBar (my shell -> window);
}

/********** **********/

/*
 * We now create the native objects associated with this widget,
 * but do not show them on the screen yet (ideally).
 * A reference must be made from widget to native object and back,
 * with SetWindowLongPtr (window, GWLP_USERDATA, (LONG_PTR) widget).
 */

static void _GuiNativizeWidget (GuiObject me) {
	if (my nativized)
		return;
	if (my inMenu) {
		if (MEMBER (me, PulldownMenu)) {
			int id;
			for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++)
				if (! theMenus [id])
					break;
			my nat.menu.id = id;
			theMenus [my nat.menu.id] = me;   // instead of UserData fields
			/*
			 * This will be a hierarchical menu.
			 */
			my nat.menu.handle = CreatePopupMenu ();
		} else {
			/*
			 * Any menu item (push button, toggle button, or cascade button) shall contain its native parent menu handle.
			 */
			my nat.entry.handle = my parent -> nat.menu.handle;
			{
				/*
				 * A Windows menu item shall have a shell-unique ID,
				 * which we can use to make changes and
				 * which will be sent to us by the WM_COMMAND message.
				 * This ID should be higher than 4000, in order to be different from the menu IDs.
				 * In our implementation, item IDs are application-unique.
				 */
				int id;
				for (id = MINIMUM_MENU_ITEM_ID; id <= MAXIMUM_MENU_ITEM_ID; id ++)
					if (! theMenuItems [id])
						break;
				my nat.entry.id = id;   // install unique ID
				theMenuItems [id] = true;
			}
		}
	} else switch (my widgetClass) {
		case xmBulletinBoardWidgetClass: {
			my window = CreateWindowEx (0, Melder_peek32toW (theWindowClassName), L"bulletinBoard", WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
		} break;
		case xmDrawingAreaWidgetClass: Melder_crash (U"Should be implemented in GuiDrawingArea."); break;
		case xmFormWidgetClass: {
			my window = CreateWindowEx (0, Melder_peek32toW (theWindowClassName), L"form", WS_CHILD | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
		} break;
		case xmRowColumnWidgetClass: {
			my window = CreateWindowEx (0, Melder_peek32toW (theWindowClassName), L"rowColumn", WS_CHILD | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
			//TRACE
			trace (U"Created window ", Melder_pointer (my window), U" for original RowColumn ", Melder_pointer (me));
		} break;
		case xmListWidgetClass: Melder_crash (U"Should be implemented in GuiList."); break;
		case xmMenuBarWidgetClass: {
			if (! my shell -> motiff.shell.isDialog && my shell -> nat.shell.menuBar == NULL && my parent -> widgetClass != xmRowColumnWidgetClass) {
				HMENU bar = CreateMenu ();
				SetMenu (my shell -> window, bar);
				my nat.menu.handle = bar;
				my shell -> nat.shell.menuBar = me;   // does this have to be?
			} else {
				my widgetClass = xmRowColumnWidgetClass;   // !!!!!!!!!!!!!
				my orientation = XmHORIZONTAL;
				my rowColumnType = XmMENU_BAR;
				#if TRY_BARLESS
					my window = my parent -> window;
				#else
					my window = CreateWindowEx (0, Melder_peek32toW (theWindowClassName), L"rowColumn", WS_CHILD,
						my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
					SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
				#endif
				//TRACE
				trace (U"Created window ", Melder_pointer (my window), U" for MenuBar-derived RowColumn ", Melder_pointer (me));
			}
		} break;
		case xmPulldownMenuWidgetClass: {
			int id;
			for (id = 1; id <= MAXIMUM_NUMBER_OF_MENUS; id ++)
				if (! theMenus [id])
					break;
			my nat.menu.id = id;
			theMenus [my nat.menu.id] = me;   // instead of UserData fields
			if (MEMBER (my parent, MenuBar)) {
				GuiObject menu;
				UINT beforeID = -1;
				my nat.menu.handle = CreatePopupMenu ();
				/*
				 * Insert the menu before the Help menu, if that exists; otherwise, at the end.
				 */
				for (menu = my parent -> firstChild; menu; menu = menu -> nextSibling) {
					if (MEMBER (menu, PulldownMenu) && str32equ (menu -> name.get(), U"Help") && menu != me) {
						beforeID = (UINT) menu -> nat.menu./*handle*/id;
						break;
					}
				}
				{
					MENUITEMINFO info;
					info. cbSize = sizeof (MENUITEMINFO);
					info. fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_ID;
					info. fType = MFT_STRING | ( str32equ (my name.get(), U"Help") ? MFT_RIGHTJUSTIFY : 0 );
					info. dwTypeData = (mutablestringW) Melder_peek32toW (my name.get());
					info. hSubMenu = my nat.menu.handle;
					info. wID = (UINT) my nat.menu./*handle*/id;
					InsertMenuItem (my parent -> nat.menu.handle, beforeID, 0, & info);
				} /*else if (beforeID >= 0)
					InsertMenu (my parent -> nat.menu.handle, beforeID, MF_STRING | MF_POPUP | MF_BYCOMMAND,
						(UINT) my nat.menu.handle, my name);
				else
					AppendMenu (my parent -> nat.menu.handle, MF_STRING | MF_POPUP, (UINT) my nat.menu.handle, my name);*/
				DrawMenuBar (my shell -> window);
			} else if (MEMBER (my parent, RowColumn) && my parent -> rowColumnType == XmMENU_BAR) {
				my nat.menu.handle = CreatePopupMenu ();
			}
		} break;
		case xmLabelWidgetClass: Melder_crash (U"Should be implemented in GuiLabel."); break;
		case xmCascadeButtonWidgetClass: {
			if (my motiff.cascadeButton.inBar) {
				my nat.entry.handle = my parent -> nat.menu.handle;   // TODO: superfluous?
			} else {
				my window = CreateWindow (L"button", Melder_peek32toW (_GuiWin_expandAmpersands (my name.get())),
					WS_CHILD | BS_PUSHBUTTON | WS_CLIPSIBLINGS,
					my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
				SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
				SetWindowFont (my window, theWinGuiNormalLabelFont (), false);
			}
		} break;
		case xmPushButtonWidgetClass: Melder_crash (U"Should be implemented in GuiButton."); break;
		case xmTextWidgetClass: Melder_crash (U"Should be implemented in GuiText."); break;
		case xmToggleButtonWidgetClass: Melder_crash (U"Should be implemented in GuiCheckButton and GuiRadioButton."); break;
		case xmScaleWidgetClass: {
			my window = CreateWindow (PROGRESS_CLASS, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get())), WS_CHILD | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
			SendMessage (my window, PBM_SETRANGE, (WPARAM) 0, (LPARAM) MAKELONG (0, 10000));
		} break;
		case xmScrollBarWidgetClass: {
			my window = CreateWindow (L"scrollbar", Melder_peek32toW (my name.get()), WS_CHILD |
				( str32equ (my name.get(), U"verticalScrollBar") ? SBS_VERT : SBS_HORZ ) | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
			NativeScrollBar_set (me);
			my minimum = 0;
			my maximum = 100;
			my value = 0;
			my sliderSize = 100;
		} break;
		case xmScrolledWindowWidgetClass: {
			/*
			 * The space of the scrolled window is completely filled by three of its children:
			 * the two scroll bars and the clip window. The first child you create yourself will be the
			 * work window: this work window can grow larger than the clip window.
			 * While normally every widget is only clipped to the rects of its ancestors,
			 * for a scrolled window the clip window will be inserted into this chain.
			 * Example: if the widget hierarchy is
			 *      shell.form.scrolledWindow.column.row.pushButton,
			 * the clipping hierarchy will be
			 *      (shell.)form.(scrolledWindow.)clipWindow.column.row.pushButton
			 */
			my window = CreateWindowEx (0, Melder_peek32toW (theWindowClassName), L"scrolledWindow", WS_CHILD | WS_CLIPSIBLINGS,
				my x, my y, my width, my height, my parent -> window, NULL, theGui.instance, NULL);
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
			my motiff.scrolledWindow.horizontalBar = XmCreateScrollBar (me, "horizontalScrollBar", NULL, 0);
			my motiff.scrolledWindow.verticalBar = XmCreateScrollBar (me, "verticalScrollBar", NULL, 0);
			XtVaSetValues (my motiff.scrolledWindow.horizontalBar, XmNorientation, XmHORIZONTAL,
				XmNleftAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 15, XmNheight, 16,
				XmNminimum, 0, XmNmaximum, 100, XmNsliderSize, 100,
				XmNincrement, CELL_HEIGHT, XmNpageIncrement, 101 - CELL_HEIGHT, NULL);
			XtVaSetValues (my motiff.scrolledWindow.verticalBar, XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 15, XmNwidth, 16,
				XmNminimum, 0, XmNmaximum, 100, XmNsliderSize, 100,
				XmNincrement, CELL_HEIGHT, XmNpageIncrement, 101 - CELL_HEIGHT, NULL);
			my motiff.scrolledWindow.clipWindow = XmCreateBulletinBoard (me, "clipWindow", NULL, 0);
			XtVaSetValues (my motiff.scrolledWindow.clipWindow,
				XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 1,   // for border
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 16,   // for scroll bar
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, 1,   // for border
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 16, NULL);   // for scroll bar
			XtAddCallback (my motiff.scrolledWindow.verticalBar, XmNvalueChangedCallback, cb_scroll, (XtPointer) me);
			XtAddCallback (my motiff.scrolledWindow.verticalBar, XmNdragCallback, cb_scroll, (XtPointer) me);
			XtAddCallback (my motiff.scrolledWindow.horizontalBar, XmNvalueChangedCallback, cb_scroll, (XtPointer) me);
			XtAddCallback (my motiff.scrolledWindow.horizontalBar, XmNdragCallback, cb_scroll, (XtPointer) me);
		} break;
		case xmShellWidgetClass: {
			static char32 *className { theApplicationClassName };   // only for first window
			my window = CreateWindowEx (theDialogHint ? WS_EX_DLGMODALFRAME /* | WS_EX_TOPMOST */ : 0,
				Melder_peek32toW (className), Melder_peek32toW (className),
				theDialogHint ? WS_CAPTION | WS_SYSMENU : WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, my parent ? my parent -> window : NULL, NULL, theGui.instance, NULL);
			className = theWindowClassName;   // all later windows
			SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
			my motiff.shell.isDialog = theDialogHint;   // so we can maintain a single Shell class instead of two different
		} break;
		default: break;
	}
	my nativized = True;
}

static GuiObject createWidget (int widgetClass, GuiObject parent, const char *name) {
	GuiObject me = _Gui_initializeWidget (widgetClass, parent, Melder_peek8to32_u (name));
	_GuiNativizeWidget (me);
	//TRACE
	trace (U"Created widget ", Melder_pointer (me));
	return me;
}

void _Gui_invalidateWidget (GuiObject me) {
	if (! my managed)   // should be: visible
		return;
	if (MEMBER (me, Shell) /*||
		 my widgetClass == xmRowColumnWidgetClass ||
		 my widgetClass == xmFormWidgetClass*/) return;   // composites are not invalidated !!!!! ????
}

void _Gui_validateWidget (GuiObject me) {
	if (! my managed)   // should be: visible
		return;
	if (MEMBER (me, Shell))
		return;
}

static void Native_move (GuiObject me, int dx, int dy) {
/* Native_move () changes the native attributes and visual position of the widget:
 * No Motif attributes (such as 'x' and 'y') are changed.
 * Usage:
 *     Native_move () is normally called immediately after changing the 'x' or 'y' attribute.
 *     It can be seen as the 'realization' of a Motif move.
 */
	(void) dx;
	(void) dy;
	if (! my window)
		return;   // ignore menu items
	if (MEMBER (me, Shell)) {
		my nat.shell.duringMoveWindow = True;
		if (my motiff.shell.isDialog) {
			#ifndef __clang__
				/*
					In a Windows app, both following system frame measurements should be 8:
				*/
				const int cxFixedFrame = GetSystemMetrics (SM_CXFIXEDFRAME);
				const int cyFixedFrame = GetSystemMetrics (SM_CYFIXEDFRAME);
			#else
				/*
					However, CLANG under MSYS2 thinks it's in a console app,
					so that these two values are 4 instead of 8,
					which causes text fields and caption to run off the right edge of the window.
					Repair.
				*/
				const int cxFixedFrame = 8;
				const int cyFixedFrame = 8;
			#endif
			MoveWindow (my window, my x, my y,
				my width + 2 * cxFixedFrame,
				my height + 2 * cyFixedFrame + GetSystemMetrics (SM_CYCAPTION),
				true
			);
		} else {
			#ifndef __clang__
				/*
					In a Windows app, both following system frame measurements should be 8:
				*/
				const int cxSizeFrame = GetSystemMetrics (SM_CXSIZEFRAME);
				const int cySizeFrame = GetSystemMetrics (SM_CYSIZEFRAME);
			#else
				/*
					However, CLANG under MSYS2 thinks it's in a console app,
					so that these two values are 4 instead of 8,
					which causes the scroll bars to be cut off.
					Repair.
				*/
				const int cxSizeFrame = 8;
				const int cySizeFrame = 8;
			#endif
			MoveWindow (my window, my x, my y,
				my width + 2 * cxSizeFrame,
				my height + 2 * cySizeFrame + GetSystemMetrics (SM_CYCAPTION) +
						( my nat.shell.menuBar ? GetSystemMetrics (SM_CYMENU) : 0 ),
				true
			);
		}
	} else
		MoveWindow (my window, my x, my y, my width, my height, True);
}

static void shellResizeWidget (GuiObject me, int dx, int dy, int dw, int dh) {
	GuiObject child;
	Melder_assert (! my shell || ! my shell -> nat.shell.duringMoveWindow);
	if (my window && ! MEMBER (me, Shell)) {
		/*RECT rect, client;
		GetWindowRect (my window, & rect);
		GetClientRect (my window, & client);
		ScreenToClient (my parent -> window, (LPPOINT) & rect.left);
		ScreenToClient (my parent -> window, (LPPOINT) & rect.right);
		if (rect.left!=my x||rect.right!=my x + my width||rect.top !=my y||rect.bottom!=my y +my height)
		Melder_warning ("class %d x %d left %d y %d top %d width %d right %d height %d bottom %d",
		my widgetClass, my x, rect.left, my y, rect.top, my width, rect.right, my height, rect.bottom);*/
		MoveWindow (my window, my x, my y, my width, my height, true);
		if (MEMBER (me, DrawingArea))
			_GuiWinDrawingArea_shellResize (me);
	}
	for (child = my firstChild; child; child = child -> nextSibling) {
		int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
		if (MEMBER (child, Shell))
			continue;
		if (child -> rightAttachment == XmATTACH_FORM) {
			if (child -> leftAttachment == XmATTACH_FORM)
				cdw = dw;
			else
				cdx = dw;
		}
		if (child -> leftAttachment == XmATTACH_POSITION && child -> rightAttachment == XmATTACH_POSITION) {
			int xLeft = my width * (child -> leftPosition / 100.0);
			int xRight = my width * (child -> rightPosition / 100.0);
			cdx = xLeft - child -> x;
			cdw = (xRight - xLeft) - child -> width;
		}
		if (child -> bottomAttachment == XmATTACH_FORM) {
			if (child -> topAttachment == XmATTACH_FORM)
				cdh = dh;
			else
				cdy = dh;
		}
		if (child -> topAttachment == XmATTACH_POSITION && child -> bottomAttachment == XmATTACH_POSITION) {
			int yTop = my height * (child -> topPosition / 100.0);
			int yBottom = my height * (child -> bottomPosition / 100.0);
			cdy = yTop - child -> y;
			cdh = (yBottom - yTop) - child -> height;
		}
		child -> x += cdx;
		child -> y += cdy;
		child -> width += cdw;
		child -> height += cdh;
		shellResizeWidget (child, dx + cdx, dy + cdy, cdw, cdh);
	}
	if (MEMBER (me, ScrolledWindow))
		_Gui_manageScrolledWindow (me);
}

static void resizeWidget (GuiObject me, int dw, int dh) {
	GuiObject child;
	if (my window && ! MEMBER (me, Shell)) {
		MoveWindow (my window, my x, my y, my width, my height, true);
		if (MEMBER (me, DrawingArea))
			_GuiWinDrawingArea_shellResize (me);
	}
	if (MEMBER2 (me, Form, ScrolledWindow))
		for (child = my firstChild; child; child = child -> nextSibling) {
			int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
			if (child -> widgetClass == xmShellWidgetClass) continue;
			if (child -> rightAttachment == XmATTACH_FORM) {
				if (child -> leftAttachment == XmATTACH_FORM)
					cdw = dw;
				else
					cdx = dw;
			}
			if (child -> leftAttachment == XmATTACH_POSITION && child -> rightAttachment == XmATTACH_POSITION) {
				int xLeft = my width * (child -> leftPosition / 100.0);
				int xRight = my width * (child -> rightPosition / 100.0);
				cdx = xLeft - child -> x;
				cdw = (xRight - xLeft) - child -> width;
			}
			if (child -> bottomAttachment == XmATTACH_FORM) {
				if (child -> topAttachment == XmATTACH_FORM)
					cdh = dh;
				else
					cdy = dh;
			}
			if (child -> topAttachment == XmATTACH_POSITION && child -> bottomAttachment == XmATTACH_POSITION) {
				int yTop = my height * (child -> topPosition / 100.0);
				int yBottom = my height * (child -> bottomPosition / 100.0);
				cdy = yTop - child -> y;
				cdh = (yBottom - yTop) - child -> height;
			}
			if (cdx || cdy) {
				child -> x += cdx;
				child -> y += cdy;
				Native_move (child, cdx, cdy);
			}
			if (cdw || cdh) {
				child -> width += cdw;
				child -> height += cdh;
				resizeWidget (child, cdw, cdh);
			}
		}
	if (MEMBER (me, Shell)) {
		int right = 1000, bottom = 500;
		for (child = my firstChild; child; child = child -> nextSibling)
			if (MEMBER2 (child, Form, ScrolledWindow)) {
				child -> width += dw;
				child -> height += dh;
				resizeWidget (child, dw, dh);
			}
	}
}

static void _motif_setValues (GuiObject me, va_list arg) {
	int resource;
	Boolean move = False, resize = False, attach = False, scrollset = False;
	char *text;
	int oldX = my x, oldY = my y, oldWidth = my width, oldHeight = my height;
	while (resource = va_arg (arg, int), resource != 0) switch (resource) {
		case XmNautoUnmanage:
			my autoUnmanage = va_arg (arg, int);
			break;
		case XmNbottomAttachment:
			my bottomAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNbottomOffset:
			my bottomOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNbottomPosition:
			my bottomPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNcolumns: {
			int columns = va_arg (arg, int);
			Melder_assert (MEMBER (me, Text));
			my width = columns * 9 + 4;
			resize = True;
		} break;
		case XmNdeleteResponse:
			Melder_assert (MEMBER (me, Shell));
			my deleteResponse = va_arg (arg, int);
			if (my deleteResponse == XmDO_NOTHING && ! my motiff.shell.goAwayCallback) {
				// change window attributes
			}
			break;
		case XmNdialogStyle:
			Melder_assert (MEMBER2 (me, Form, BulletinBoard));
			my shell -> dialogStyle = my dialogStyle = va_arg (arg, int);
			break;
		case XmNdialogTitle:
			Melder_assert (MEMBER2 (me, Form, BulletinBoard));
			text = va_arg (arg, char *);
			SetWindowTextW (my shell -> window, Melder_peek32toW (Melder_peek8to32_u (text)));
			break;
		case XmNheight:
			my height = va_arg (arg, int);
			if (MEMBER (me, Shell)) {
				int maximumHeight =
					GetSystemMetrics (SM_CYMAXIMIZED) - GetSystemMetrics (SM_CYCAPTION) -
						GetSystemMetrics (SM_CYSIZEFRAME) - GetSystemMetrics (SM_CYMENU) - 15;
				if (my height > maximumHeight)
					my height = maximumHeight;
			}
			resize = True;
			break;
		case XmNhorizontalScrollBar: {
			/* Have to kill my own bar first. */
			XtDestroyWidget (my motiff.scrolledWindow.horizontalBar);
			/* Then replace by new bar. */
			my motiff.scrolledWindow.horizontalBar = va_arg (arg, GuiObject);
			/* Make sure it is in the right position. */
			XtVaSetValues (my motiff.scrolledWindow.horizontalBar, XmNorientation, XmHORIZONTAL,
				XmNleftAttachment, XmATTACH_FORM, XmNbottomAttachment, XmATTACH_FORM,
				XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, 15, XmNheight, 16, NULL);
			/*
			 * Note that we may now have a horizontal bar with application-defined scrolling policy,
			 * and a vertical bar with automatic scrolling policy.
			 * This is more flexible than X Motif, but may be simulated in X Motif with the following strategy,
			 * which is compatible with MotifEmulator:
			 *   - set the scrolling policy to XmAUTOMATIC: two scroll bars will be created;
			 *   - ask for the horizontal scroll bar with XtVaGetValues;
			 *   - remove all the callbacks from the horizontal scroll bar with XtRemoveAllCallbacks;
			 *   - add your application-defined callback with XtAddCallback.
			 * We just hope now that X Motif does not look into the XmNscrollingPolicy resource after this...
			 */
		} break;
		case XmNincrement:
			Melder_assert (MEMBER (me, ScrollBar));
			my increment = va_arg (arg, int);
			break;
		case XmNlabelString:
			Melder_assert (MEMBER2 (me, CascadeButton, PushButton));
			text = va_arg (arg, char *);
			my name = Melder_8to32_e (text);   // BUG throwable
			if (my inMenu) {
				_GuiWinMenuItem_setText (me);
			} else if (MEMBER (me, CascadeButton) && my motiff.cascadeButton.inBar) {
				/* BUG: menu title change not implemented */
			} else {
				_GuiNativeControl_setTitle (me);
			}
			break;
		case XmNleftAttachment:
			my leftAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNleftOffset:
			my leftOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNleftPosition:
			my leftPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNmaximum:
			my maximum = va_arg (arg, int);
			if (MEMBER (me, ScrollBar))
				scrollset = True;
			else if (MEMBER (me, Scale))
				_Gui_invalidateWidget (me);
			break;
		case XmNmenuHelpWidget:
			(void) va_arg (arg, GuiObject);
			break;
		case XmNminimum:
			my minimum = va_arg (arg, int);
			if (MEMBER (me, ScrollBar))
				scrollset = True;
			else if (MEMBER (me, Scale))
				_Gui_invalidateWidget (me);
			break;
		case XmNorientation:
			Melder_assert (MEMBER3 (me, RowColumn, ScrollBar, Scale));
			my orientation = va_arg (arg, int);
			break;
		case XmNpageIncrement:
			Melder_assert (MEMBER (me, ScrollBar));
			my pageIncrement = va_arg (arg, int);
			break;
		case XmNradioBehavior:
			Melder_assert (MEMBER (me, RowColumn));
			my radioBehavior = va_arg (arg, int);
			break;
		case XmNrightAttachment:
			my rightAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNrightOffset:
			my rightOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNrightPosition:
			my rightPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNrowColumnType:
			Melder_assert (MEMBER (me, RowColumn));
			my rowColumnType = va_arg (arg, int);
			break;
		case XmNrows:
			(void) va_arg (arg, int);
			break;
		case XmNscaleHeight:
			Melder_assert (MEMBER (me, Scale));
			my height = va_arg (arg, int);
			resize = True;
			break;
		case XmNscaleWidth:
			Melder_assert (MEMBER (me, Scale));
			my width = va_arg (arg, int);
			resize = True;
			break;
		case XmNscrollingPolicy:
			(void) va_arg (arg, int);
			break;
		case XmNsliderSize:
			Melder_assert (MEMBER (me, ScrollBar));
			my sliderSize = va_arg (arg, int);
			scrollset = True;
			break;
		case XmNspacing:
			(void) va_arg (arg, int);
			break;
		case XmNsubMenuId:
			Melder_assert (MEMBER (me, CascadeButton));
			my subMenuId = va_arg (arg, GuiObject);
			my subMenuId -> popUpButton = me;
			break;
		case XmNtitle:
			Melder_assert (MEMBER (me, Shell));
			text = va_arg (arg, char *);
			SetWindowTextW (my window, Melder_peek32toW (Melder_peek8to32_u (text)));
			break;
		case XmNtitleString:
			Melder_assert (MEMBER (me, Scale));
			text = va_arg (arg, char *);
			my name = Melder_8to32_e (text);   // BUG throwable
			_Gui_invalidateWidget (me);
			break;
		case XmNtopAttachment:
			my topAttachment = va_arg (arg, int);
			attach = True;
			break;
		case XmNtopOffset:
			my topOffset = va_arg (arg, int);
			attach = True;
			break;
		case XmNtopPosition:
			my topPosition = va_arg (arg, int);
			attach = True;
			break;
		case XmNuserData:
			my userData = va_arg (arg, void *);
			break;
		case XmNvalue:
			my value = va_arg (arg, int);
			if (MEMBER (me, ScrollBar))
				scrollset = True;
			else if (MEMBER (me, Scale))
				SendMessage (my window, PBM_SETPOS, (WPARAM) my value, 0);
			break;
		case XmNverticalScrollBar: {
			/* Have to kill my own bar first. */
			XtDestroyWidget (my motiff.scrolledWindow.verticalBar);
			/* Then replace by new bar. */
			my motiff.scrolledWindow.verticalBar = va_arg (arg, GuiObject);
			/* Make sure it is in the right position. */
			XtVaSetValues (my motiff.scrolledWindow.verticalBar, XmNorientation, XmVERTICAL,
				XmNtopAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 15, XmNwidth, 16, NULL);
		} break;
		case XmNwidth:
			my width = va_arg (arg, int);
			resize = True;
			break;
		case XmNx:
			my x = va_arg (arg, int);
			move = True;
			break;
		case XmNy:
			my y = va_arg (arg, int);
			move = True;
			break;
		default: {
			if (resource < 0 || resource >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError (U"(XtVaSetValues:) Resource out of range (", resource, U").");
			else
				Melder_flushError (U"(XtVaSetValues:) Unknown resource \"", Melder_peek8to32_u (motif_resourceNames [resource]), U"\".");
			return;   // because we do not know how to skip this unknown resource
		}
	}

	if (attach) {
		Melder_assert (MEMBER2 (my parent, Form, ScrolledWindow));
		if (my leftAttachment == XmATTACH_FORM) {
			my x = my leftOffset;
			move = True;
			if (my rightAttachment == XmATTACH_FORM) {
				my width = my parent -> width - my leftOffset - my rightOffset;
				resize = True;
			}
		} else if (my rightAttachment == XmATTACH_FORM) {
			my x = my parent -> width - my width - my rightOffset;
			move = True;
		}
		if (my leftAttachment == XmATTACH_POSITION && my rightAttachment == XmATTACH_POSITION) {
			my x = my parent -> width * (my leftPosition / 100.0);
			int xRight = my parent -> width * (my rightPosition / 100.0);
			my width = xRight - my x;
			resize = True;
		}
		if (my topAttachment == XmATTACH_FORM) {
			my y = my topOffset;
			move = True;
			if (my bottomAttachment == XmATTACH_FORM) {
				my height = my parent -> height - my topOffset - my bottomOffset;
				resize = True;
			}
		} else if (my bottomAttachment == XmATTACH_FORM) {
			my y = my parent -> height - my height - my bottomOffset;
			move = True;
		}
		if (my topAttachment == XmATTACH_POSITION && my bottomAttachment == XmATTACH_POSITION) {
			my y = my parent -> height * (my topPosition / 100.0);
			int yBottom = my parent -> height * (my bottomPosition / 100.0);
			my height = yBottom - my y;
			resize = True;
		}
	}
	if (move)
		Native_move (me, 0, 0);
	if (resize)
		resizeWidget (me, my width - oldWidth, my height - oldHeight);
	if (scrollset)
		NativeScrollBar_set (me);
}

void _Gui_manageScrolledWindow (GuiObject me) {
	int workWidth, workHeight, horzAutomatic, vertAutomatic;
	GuiObject clipWindow, workWindow, horzBar = my motiff.scrolledWindow.horizontalBar, vertBar = my motiff.scrolledWindow.verticalBar;
	Melder_assert (my widgetClass == xmScrolledWindowWidgetClass);
	clipWindow = my motiff.scrolledWindow.clipWindow;
	workWindow = my motiff.scrolledWindow.workWindow;
	if (clipWindow == NULL || horzBar == NULL || vertBar == NULL)
		return;   // apparently during destruction of scrolled window
	/*
	 * We must find out if the scrolling policy of each bar is automatic.
	 * Otherwise, we must not change them automatically.
	 */
	horzAutomatic = horzBar -> motiff.scrollBar.valueChangedCallbacks.pairs [0].proc == cb_scroll;
	vertAutomatic = vertBar -> motiff.scrollBar.valueChangedCallbacks.pairs [0].proc == cb_scroll;
	/*
	 * If the work window has been unmanaged or destroyed, the automatic scroll bars should be empty and insensitive.
	 */
	if (workWindow == NULL || ! workWindow -> managed) {
		if (horzAutomatic)
			XtVaSetValues (horzBar, XmNmaximum, 100, XmNsliderSize, 100, XmNvalue, 0, XmNpageIncrement, 1, NULL);
		if (vertAutomatic)
			XtVaSetValues (vertBar, XmNmaximum, 100, XmNsliderSize, 100, XmNvalue, 0, XmNpageIncrement, 1, NULL);
		return;
	}
	workWidth = workWindow -> width > 10 ? workWindow -> width : 10;
	workHeight = workWindow -> height > 10 ? workWindow -> height : 10;
	/*
	 * If the scroll bar is automatic, the slider width is set to the visible height of the work window,
	 * and the maximum is set to the entire height of the work window.
	 * If the value becomes greater than the maximum minus the slider size,
	 * the value is reduced and the work window is scrolled up (i.e. moved down).
	 */
	if (horzAutomatic) {
		int maximum = workWidth;
		int sliderSize = workWidth < clipWindow -> width ? workWidth : clipWindow -> width;
		int value = horzBar -> value;
		if (value > maximum - sliderSize) {
			value = maximum - sliderSize;
			workWindow -> x += horzBar -> value - value;
			Native_move (workWindow, horzBar -> value - value, 0);
		}
		XtVaSetValues (horzBar, XmNmaximum, maximum, XmNsliderSize, sliderSize, XmNvalue, value,
				XmNpageIncrement, clipWindow -> width - (CELL_HEIGHT - 1), NULL);
	}
	if (vertAutomatic) {   /* Automatic? */
		int maximum = workHeight;
		int sliderSize = workHeight < clipWindow -> height ? workHeight : clipWindow -> height;
		int value = vertBar -> value;
		if (value > maximum - sliderSize) {
			value = maximum - sliderSize;
			workWindow -> y += vertBar -> value - value;
			Native_move (workWindow, 0, vertBar -> value - value);
		}
		XtVaSetValues (vertBar, XmNmaximum, maximum, XmNsliderSize, sliderSize, XmNvalue, value,
				XmNpageIncrement, clipWindow -> height - (CELL_HEIGHT - 1), NULL);
	}
}

static void _motif_manage (GuiObject me) {
	GuiObject child;
	int x = 2, y = 2;
	int width = 0, height = 0, dw = 0, dh = 0;
	/*if (my widgetClass == xmScrolledWindowWidgetClass) return;   /* Ignore. */

	/*
	 * My geometry follows the layout of my children.
	 * If I am a RowColumn, I am growing while my children move.
	 * Because these moves may involve drawing, which needs clipping to my rect,
	 * I have to grow with every managed child separately,
	 * not just after managing all of them.
	 */

	for (child = my firstChild; child; child = child -> nextSibling) {
		if (child -> managed && ! MEMBER (child, Shell)) {
			int dx = 0, dy = 0;   // by default, the child does not move
			if (MEMBER (me, RowColumn)) {
				{
					if (x > child -> x)
						dx = x - child -> x;
					if (y > child -> y)
						dy = y - child -> y;
					child -> x += dx; x = child -> x;
					child -> y += dy; y = child -> y;
					if (my orientation != XmHORIZONTAL)
						y += child -> height + 1;
					else
						x += child -> width + 3;
				}
			}
			if (child -> x + child -> width > width)
				width = child -> x + child -> width;
			if (child -> y + child -> height > height)
				height = child -> y + child -> height;
			if (MEMBER3 (me, Shell, Form, BulletinBoard)) {
				/*
					These widgets grow with their children.
				*/
				dw = Melder_clippedLeft (0, width - my width);
				dh = Melder_clippedLeft (0, height - my height);
			} else if (MEMBER (me, RowColumn)) {
				/*
					A RowColumn shrinks and grows with its children.
				*/
				dw = width - my width + 2;
				dh = height - my height + 2;
			} else {   /* ? */
				dw = width - my width;
				dh = height - my height;
			}
			my width += dw;
			my height += dh;

			/*
				Now that I have grown, there is room for my child to move.
			*/
			if (MEMBER (me, RowColumn))
				Native_move (child, dx, dy);

			/*
				Resize my attached other children.
			*/
			#if 0
			if (MEMBER3 (me, RowColumn, Form, ScrolledWindow)) {
				GuiObject child2;
				for (child2 = my firstChild; child2; child2 = child2 -> nextSibling) if (child2 != child && child2 -> managed) {
					int cdx = 0, cdy = 0, cdw = 0, cdh = 0;
					if (child2 -> widgetClass == xmShellWidgetClass)
						continue;
					if (child2 -> rightAttachment == XmATTACH_FORM)
						if (child2 -> leftAttachment == XmATTACH_FORM)
							cdw = dw;
						else
							cdx = dw;
					if (child2 -> bottomAttachment == XmATTACH_FORM)
						if (child2 -> topAttachment == XmATTACH_FORM)
							cdh = dh;
						else
							cdy = dh;
					if (cdx || cdy) {
						child2 -> x += cdx;
						child2 -> y += cdy;
						Native_move (child2, cdx, cdy);
					}
					if (cdw || cdh) {
						child2 -> width += cdw;
						child2 -> height += cdh;
						resizeWidget (child2, cdw, cdh);
					}
				}
			}
			#endif
		}
	}

	if (my window) Native_move (me, 0, 0);

	/*
		If I have grown, I have to notify my parent.
	*/
	if (! MEMBER (me, Shell)) {
		//if (MEMBER4 (my parent, RowColumn, Form, BulletinBoard, Shell)) _motif_manage (my parent);
		if (MEMBER (my parent, ScrolledWindow))
			_Gui_manageScrolledWindow (my parent);
	}
}

/***** X TOOLKIT *****/

static void xt_addCallback (XtCallbackList *callbacks, XtCallbackProc proc, XtPointer closure) {
	int i;
	for (i = 0; i < MAXNUM_CALLBACKS; i ++) {
		if (! callbacks -> pairs [i]. proc) {
			callbacks -> pairs [i]. proc = proc;
			callbacks -> pairs [i]. closure = closure;
			return;
		}
	}
	Melder_assert (i < MAXNUM_CALLBACKS);   // will always fail if we arrive here
}

void XtAddCallback (GuiObject me, int kind, XtCallbackProc proc, XtPointer closure) {
	switch (kind) {
		case XmNactivateCallback:
			my activateCallback = proc; my activateClosure = closure;
		break;
		case XmNdestroyCallback:
			my destroyCallback = proc; my destroyClosure = closure;
		break;
		case XmNdragCallback:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			xt_addCallback (& my motiff.scrollBar.dragCallbacks, proc, closure);
		break;
		case XmNvalueChangedCallback:
			if (my widgetClass == xmScrollBarWidgetClass)
				xt_addCallback (& my motiff.scrollBar.valueChangedCallbacks, proc, closure);
			else if (my widgetClass == xmToggleButtonWidgetClass)
				xt_addCallback (& my motiff.toggleButton.valueChangedCallbacks, proc, closure);
			else
				Melder_assert (False);
		break;
		default:
			if (kind < 0 || kind >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError (U"(XtAddCallback:) Callback name out of range (", kind, U").");
			else
				Melder_flushError (U"(XtAddCallback:) Unknown callback \"", Melder_peek8to32_u (motif_resourceNames [kind]), U"\".");
	}
}

XtWorkProcId XtAddWorkProc (XtWorkProc workProc, XtPointer closure) {
	int i = 1;
	while (i < 10 && theWorkProcs [i])
		i ++;
	Melder_assert (i < 10);
	theWorkProcs [i] = workProc;
	theWorkProcClosures [i] = closure;
	theNumberOfWorkProcs ++;
	return i;
}

void XtRemoveWorkProc (XtWorkProcId id) {
	theWorkProcs [id] = NULL;
	theNumberOfWorkProcs --;
}

XtIntervalId XtAddTimeOut (double interval, XtTimerCallbackProc proc, XtPointer closure) {
	integer i = 1;
	while (i < 10 && theTimeOutProcs [i])
		i ++;
	Melder_assert (i < 10);
	theTimeOutProcs [i] = proc;
	theTimeOutStarts [i] = Melder_clock ();
	theTimeOutIntervals [i] = interval;
	theTimeOutClosures [i] = closure;
	theNumberOfTimeOuts ++;
	return i;
}

void XtRemoveTimeOut (XtIntervalId id) {
	theTimeOutProcs [id] = NULL;
	theNumberOfTimeOuts --;
}

void XtDestroyWidget (GuiObject me) {
	//TRACE
	trace (U"Destroying widget ", Melder_pointer (me));
	GuiObject subview = my firstChild;
	/*
	 * Prevent subsequent messages.
	 */
	HWND natWindow;
	if (my window)
		SetWindowLongPtr (my window, GWLP_USERDATA, 0);
	if (my widgetClass == xmShellWidgetClass) {
		XtUnmanageChild (me);
		natWindow = my window;   // save for destroy
		my window = NULL;
	}
	if (MEMBER2 (me, Form, BulletinBoard) && MEMBER (my parent, Shell) &&
		my parent -> window
	) {
		XtDestroyWidget (my parent);
		return;
	}
	while (subview) {
		GuiObject nextSibling = subview -> nextSibling;   // save...
		//if (subview -> widgetClass != xmShellWidgetClass)   /* FIX instead of Xm's being_destroyed mark. */   // removed 20090105
			XtDestroyWidget (subview);
		subview = nextSibling;   // ...because we can't dereference dead subview
	}
	if (my destroyCallback)
		my destroyCallback (me, my destroyClosure, NULL);
	switch (my widgetClass) {
		case xmLabelWidgetClass: {
			_GuiWinLabel_destroy (me);
		} break;
		case xmCascadeButtonWidgetClass: {
			if (! my inMenu && ! MEMBER (my parent, MenuBar))
				_GuiNativeControl_destroy (me);
		} break;
		case xmScaleWidgetClass: {
			_GuiWinScale_destroy (me);
		} break;
		case xmShellWidgetClass: {
			DestroyWindow (natWindow);
		} break;
		case xmListWidgetClass: {
			_GuiWinList_destroy (me);
		} break;
		case xmDrawingAreaWidgetClass: {
			_GuiWinDrawingArea_destroy (me);
		} break;
		case xmRowColumnWidgetClass: {
			//TRACE
			trace (U"Destroying window ", Melder_pointer (my window), U" for RowColumn ", Melder_pointer (me));
			#if TRY_BARLESS
				if (my rowColumnType == XmMENU_BAR)
					; // my window is my parent's window
				else
					DestroyWindow (my window);
			#else
				DestroyWindow (my window);
			#endif
		} break;
		case xmFormWidgetClass:
		case xmBulletinBoardWidgetClass: {
			DestroyWindow (my window);
		} break;
		case xmTextWidgetClass: {
			_GuiWinText_destroy (me);
		} break;
		case xmPushButtonWidgetClass: {
			if (my inMenu) {
				if (my nat.entry.id)
					theMenuItems [my nat.entry.id] = false;
			} else {
				_GuiWinButton_destroy (me);
			}
		} break;
		case xmToggleButtonWidgetClass: {
			if (my inMenu) {
				if (my nat.entry.id)
					theMenuItems [my nat.entry.id] = false;
			} else {
				if (my isRadioButton)
					_GuiWinRadioButton_destroy (me);
				else
					_GuiWinCheckButton_destroy (me);
			}
		} break;
		case xmScrollBarWidgetClass: {
			_GuiWinScrollBar_destroy (me);
		} break;
		case xmScrolledWindowWidgetClass: {
			/* The scroll bars will be destroyed automatically because they are my children. */
			_GuiWinScrolledWindow_destroy (me);
		} break;
		case xmSeparatorWidgetClass: {
			if (my inMenu) {
				if (my nat.entry.id)
					theMenuItems [my nat.entry.id] = false;
			}
		} break;
		case xmPulldownMenuWidgetClass: {
			if (MEMBER (my parent, MenuBar))
				RemoveMenu (my parent -> nat.menu.handle, (UINT) my nat.menu./*handle*/id, MF_BYCOMMAND);
			DestroyMenu (my nat.menu.handle);
			theMenus [my nat.menu.id] = NULL;
		} break;
	}
	my name. reset();   // not automatic
	if (my parent && me == my parent -> firstChild)   // remove dangling reference
		my parent -> firstChild = my nextSibling;
	if (my previousSibling)   // remove dangling reference
		my previousSibling -> nextSibling = my nextSibling;
	if (my nextSibling)   // remove dangling reference: two-way linked list
		my nextSibling -> previousSibling = my previousSibling;
	if (my parent && MEMBER (my parent, ScrolledWindow)) {
		if (me == my parent -> motiff.scrolledWindow.workWindow) {
			my parent -> motiff.scrolledWindow.workWindow = NULL;
			_Gui_manageScrolledWindow (my parent);
		} else if (me == my parent -> motiff.scrolledWindow.horizontalBar) {
			my parent -> motiff.scrolledWindow.horizontalBar = NULL;
		} else if (me == my parent -> motiff.scrolledWindow.verticalBar) {
			my parent -> motiff.scrolledWindow.verticalBar = NULL;
		} else if (me == my parent -> motiff.scrolledWindow.clipWindow) {
			my parent -> motiff.scrolledWindow.clipWindow = NULL;
		}
	}
	Melder_free (me);
	numberOfWidgets --;
}

Boolean XtIsManaged (GuiObject me) { return my managed; }

Boolean XtIsShell (GuiObject me) {
	return my widgetClass == xmShellWidgetClass;
}

void XtMapWidget (GuiObject me) {
	switch (my widgetClass) {
		case xmShellWidgetClass:
			ShowWindow (my window, theGui.commandShow);
			//UpdateWindow (my window);
			break;
		default:
			break;
	}
}

static void mapWidget (GuiObject me) {
	GuiObject child;
	Melder_assert (my widgetClass != xmPulldownMenuWidgetClass);
	if (my inMenu) {
		trace (U"showing a menu item");
		int position = NativeMenuItem_getPosition (me);
		switch (my widgetClass) {
			case xmPushButtonWidgetClass: {
				InsertMenu (my nat.entry.handle, position,
					MF_STRING | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
					my nat.entry.id, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get()))
				);
			} break;
			case xmToggleButtonWidgetClass: {
				InsertMenu (my nat.entry.handle, position,
					MF_STRING | MF_UNCHECKED | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
					my nat.entry.id, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get()))
				);
			} break;
			case xmCascadeButtonWidgetClass: {
				my nat.entry.id = (ULONG_PTR) my subMenuId -> nat.menu.handle;
				InsertMenu (my nat.entry.handle, position,
					MF_POPUP | MF_BYPOSITION | ( my insensitive ? MF_GRAYED : MF_ENABLED ),
					my nat.entry.id, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get()))
				);
			} break;
			case xmSeparatorWidgetClass: {
				InsertMenu (my nat.entry.handle, position,
					MF_SEPARATOR | MF_BYPOSITION,
					my nat.entry.id, Melder_peek32toW (_GuiWin_expandAmpersands (my name.get()))
				);
			} break;
		}
	} else switch (my widgetClass) {
		case xmBulletinBoardWidgetClass:
		case xmDrawingAreaWidgetClass:
		case xmScrolledWindowWidgetClass:
		case xmFormWidgetClass: {
			ShowWindow (my window, SW_SHOW);
		} break;
		case xmRowColumnWidgetClass: {
			ShowWindow (my window, SW_SHOW);
		} break;
		case xmShellWidgetClass: {
			ShowWindow (my window, theGui.commandShow);
			if (my dialogStyle == XmDIALOG_FULL_APPLICATION_MODAL)
				;
		} break;
		case xmMenuBarWidgetClass: {
			DrawMenuBar (my shell -> window);   // every window has its own menu bar
		} break;
		case xmPushButtonWidgetClass: _GuiNativeControl_show (me); break;
		case xmToggleButtonWidgetClass: _GuiNativeControl_show (me); break;
		case xmScrollBarWidgetClass: {
			if (! my window) {
				my window = CreateWindow (L"scrollbar",
					Melder_peek32toW (my name.get()),
					WS_CHILD | ( my orientation == XmHORIZONTAL ? SBS_HORZ : SBS_VERT) | WS_CLIPSIBLINGS,
					my x, my y, my width, my height,
					my parent -> window,
					(HMENU) 1,
					theGui.instance,
					NULL
				);
				SetWindowLongPtr (my window, GWLP_USERDATA, (LONG_PTR) me);
				NativeScrollBar_set (me);
			}
			_GuiNativeControl_show (me);
		} break;
		case xmLabelWidgetClass: {
			_GuiNativeControl_show (me);
		} break;
		case xmCascadeButtonWidgetClass: {
			if (! MEMBER (my parent, MenuBar))
				_GuiNativeControl_show (me);
		} break;
		case xmScaleWidgetClass: {
			_GuiNativeControl_show (me);
		} break;
		case xmTextWidgetClass: {
			_GuiWinText_map (me);
		} break;
		case xmListWidgetClass: {
			_GuiWinList_map (me);
		} break;
		default:
			break;
	}
	for (child = my firstChild; child != NULL; child = child -> nextSibling)
		if (child -> managed) mapWidget (child);
}

void XtManageChild (GuiObject me) {
	if (my managed)
		return;

	if (MEMBER (me, ScrolledWindow)) {
		XtManageChild (my motiff.scrolledWindow.horizontalBar);
		XtManageChild (my motiff.scrolledWindow.verticalBar);
		/*XtManageChild (my motiff.scrolledWindow.clipWindow);*/
	}

	my managed = 1;

	/* Geometry management if my parent is a manager. */

	if (! MEMBER (me, Shell)) {
		if (MEMBER4 (my parent, RowColumn, Form, BulletinBoard, Shell))
			_motif_manage (my parent);
		if (MEMBER (me, ScrolledWindow))
			_Gui_manageScrolledWindow (me);
		if (MEMBER (my parent, ScrolledWindow))
			_Gui_manageScrolledWindow (my parent);
	}

	/* Map to the screen (suppose that mapped_when_managed is true). */
	/* Condition: the entire up chain has been managed. */
	/* Shells or their immediate manager children can be mapped directly. */

	if (my parent && MEMBER (my parent, Shell) && MEMBER2 (me, Form, BulletinBoard)) {
		my parent -> managed = 1;
		mapWidget (my parent);
	} else if (my inMenu) {
		mapWidget (me);
	} else {
		int visible = True;
		GuiObject widget;
		for (widget = me; widget != NULL; widget = widget -> parent) {
			if (! widget -> managed &&   // if a parent is invisible, so are its children
					! MEMBER (widget, PulldownMenu))   // the exception: "shown" even if not popped up
				{ visible = False; break; }
			if (MEMBER (widget, Shell))
				break;   // root: end of chain
		}
		if (visible) mapWidget (me);
	}

	/* I may have been created by XmCreateScrolledText or XmCreateScrolledList. */
	/* In that case, my parent should be managed. */

	if (MEMBER2 (me, Text, List) && MEMBER (my parent, ScrolledWindow))
		XtManageChild (my parent);
}

void XtManageChildren (GuiObjectList children, Cardinal num_children) {
	for (Cardinal i = 0; i < num_children; i ++)
		XtManageChild (children [i]);
}

void XtSetSensitive (GuiObject me, Boolean value) {
	//TRACE
	if (my insensitive != value)
		return;
	my insensitive = ! value;
	if (! my parent)
		return;
	switch (my widgetClass) {
		case xmPushButtonWidgetClass:
		case xmToggleButtonWidgetClass: {
			if (my inMenu)
				NativeMenuItem_setSensitive (me);
			else
				_GuiNativeControl_setSensitive (me);
		} break;
		case xmScrollBarWidgetClass: _GuiNativeControl_setSensitive (me); break;
		case xmLabelWidgetClass: _GuiNativeControl_setSensitive (me); break;
		case xmCascadeButtonWidgetClass: {
			trace (U"setting the sensitivity of a cascade button to ", value);
			if (my inMenu || my motiff.cascadeButton.inBar) {
				trace (U"in the menu bar");
				if (my subMenuId) {
					trace (U"submenu ID ", Melder_pointer (my subMenuId));
					my subMenuId -> insensitive = my insensitive;
					NativeMenuItem_setSensitive (my subMenuId);
					//NativeMenuItem_setSensitive (me);
					DrawMenuBar (my shell -> window);
				}
			} else {
				trace (U"outside the menu bar");
				_GuiNativeControl_setSensitive (me);
			}
		} break;
		case xmPulldownMenuWidgetClass: {
			trace (U"setting the sensitivity of a menu title to ", value);
			if (my popUpButton)
				XtSetSensitive (my popUpButton, value);
		} break;
		default:
			break;
	}
}

void XtUnmanageChild (GuiObject me) {
	if (! my managed)
		return;
	if (my inMenu) {
		if (! MEMBER (me, PulldownMenu))
			NativeMenuItem_delete (me);
	} else switch (my widgetClass) {
		case xmShellWidgetClass:
			_GuiText_handleFocusLoss (my textFocus);
			ShowWindow (my window, SW_HIDE);
			if (my firstChild && MEMBER2 (my firstChild, Form, BulletinBoard))
				my firstChild -> managed = 0;
			break;
		case xmPushButtonWidgetClass:
			_GuiNativeControl_hide (me);
			break;
		case xmToggleButtonWidgetClass:
			_GuiNativeControl_hide (me);
			break;
		case xmLabelWidgetClass:
			_GuiNativeControl_hide (me);
			break;
		case xmCascadeButtonWidgetClass:
			if (! MEMBER (my parent, MenuBar))
				_GuiNativeControl_hide (me);
			break;
		case xmScrollBarWidgetClass:
			_GuiNativeControl_hide (me);
			break;
		case xmFormWidgetClass:
		case xmBulletinBoardWidgetClass:
			if (MEMBER (my parent, Shell))
				XtUnmanageChild (my parent);
			break;
		case xmTextWidgetClass:
			_GuiText_unmanage (me);
			break;
		default:
			_Gui_invalidateWidget (me);
			break;
	}

	my managed = 0;

	if (! MEMBER (me, Shell)) {
		if (MEMBER4 (my parent, RowColumn, Form, BulletinBoard, Shell))
			_motif_manage (my parent);
		else if (MEMBER (my parent, ScrolledWindow))
			_Gui_manageScrolledWindow (my parent);
	}
}

void XtUnmanageChildren (GuiObjectList children, Cardinal num_children) {
	for (Cardinal i = 0; i < num_children; i ++)
		XtUnmanageChild (children [i]);
}

static LRESULT CALLBACK windowProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam);

void * GuiWin_initialize1 (conststring32 name)
{
	Melder_sprint (theApplicationClassName,100, U"PraatShell", PRAAT_WINDOW_CLASS_NUMBER, U" ", name);
	Melder_sprint (theWindowClassName,100, U"PraatChildWindow", PRAAT_WINDOW_CLASS_NUMBER, U" ", name);
	trace (U"Window class name <<", theWindowClassName, U">>");
	Melder_sprint (theDrawingAreaClassName,100, U"PraatDrawingArea", PRAAT_WINDOW_CLASS_NUMBER, U" ", name);
	HWND window = FindWindow (Melder_peek32toW (theWindowClassName), NULL);   // or theApplicationClassName?
	return window;
}
void GuiWin_initialize2 (unsigned int argc, char **argv)
{
	WNDCLASSEX windowClass;
	windowClass. cbSize = sizeof (WNDCLASSEX);
	windowClass. style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS |
		CS_OWNDC   // crucial: see GraphicsScreen_init ()
		;
	windowClass. lpfnWndProc = windowProc;
	windowClass. cbClsExtra = 0;
	windowClass. cbWndExtra = 0;
	windowClass. hInstance = theGui.instance;
	windowClass. hIcon = NULL;
	windowClass. hCursor = LoadCursor (NULL, IDC_ARROW);
	windowClass. hbrBackground = theWinGuiBackgroundBrush ();
	windowClass. lpszMenuName = NULL;
	windowClass. lpszClassName = Melder_32toW (theWindowClassName).transfer();
	windowClass. hIconSm = NULL;
	RegisterClassEx (& windowClass);
	windowClass. hbrBackground = theWinGuiBackgroundBrush ();
	windowClass. lpszClassName = Melder_32toW (theDrawingAreaClassName).transfer();
	RegisterClassEx (& windowClass);
	windowClass. lpszClassName = Melder_32toW (theApplicationClassName).transfer();
	RegisterClassEx (& windowClass);
	InitCommonControls ();
	EnableMouseInPointer (TRUE);   // from Windows 8 on
}

void GuiApp_setApplicationShell (GuiObject shell) {
	theApplicationShell = shell;
}

GuiObject XtVaCreateManagedWidget (const char *name, int widgetClass, GuiObject parent, ...) {
	GuiObject me;
	va_list arg;
	va_start (arg, parent);
	me = createWidget (widgetClass, parent, name);
	_motif_setValues (me, arg);
	va_end (arg);
	XtManageChild (me);
	return me;
}

GuiObject XtVaCreateWidget (const char *name, int widgetClass, GuiObject parent, ...) {
	GuiObject me;
	va_list arg;
	va_start (arg, parent);
	me = createWidget (widgetClass, parent, name);
	_motif_setValues (me, arg);
	va_end (arg);
	return me;
}

void XtVaGetValues (GuiObject me, ...) {
	char *text;
	unsigned int resource;
	va_list arg;
	va_start (arg, me);
	while ((resource = va_arg (arg, int)) != 0) switch (resource) {
		case XmNx: *va_arg (arg, int *) = my x; break;
		case XmNy: *va_arg (arg, int *) = my y; break;
		case XmNwidth: *va_arg (arg, int *) = my width; break;
		case XmNheight: *va_arg (arg, int *) = my height; break;
		case XmNuserData: *va_arg (arg, void **) = my userData; break;
		case XmNtitle:
			Melder_assert (my widgetClass == xmShellWidgetClass);
			*va_arg (arg, char **) = NULL;
			break;
		case XmNlabelString:
		case XmNtitleString:
			Melder_assert (my widgetClass == xmCascadeButtonWidgetClass || my widgetClass == xmScaleWidgetClass);
			text = Melder_32to8 (my name.get()).transfer();   // BUG throwable
			*va_arg (arg, char **) = text;
			break;
		case XmNdialogTitle:
			Melder_assert (my widgetClass == xmFormWidgetClass || my widgetClass == xmBulletinBoardWidgetClass);
			*va_arg (arg, char **) = NULL;   // NYI
			break;
		case XmNradioBehavior:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = my radioBehavior;
			break;
		case XmNautoUnmanage:
			*va_arg (arg, int *) = my autoUnmanage;
			break;
		case XmNorientation:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass ||
								my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my orientation;
			break;
		case XmNrowColumnType:
			Melder_assert (my widgetClass == xmRowColumnWidgetClass);
			*va_arg (arg, int *) = my rowColumnType;
			break;
		case XmNsubMenuId:
			Melder_assert (my widgetClass == xmCascadeButtonWidgetClass);
			*va_arg (arg, GuiObject *) = my subMenuId;
			break;
		case XmNdialogStyle:
			Melder_assert (my widgetClass == xmFormWidgetClass ||
								my widgetClass == xmBulletinBoardWidgetClass);
			*va_arg (arg, int *) = my dialogStyle;
			break;
		case XmNleftAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my leftAttachment;
			break;
		case XmNrightAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my rightAttachment;
			break;
		case XmNtopAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my topAttachment;
			break;
		case XmNbottomAttachment:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my bottomAttachment;
			break;
		case XmNleftOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my leftOffset;
			break;
		case XmNrightOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my rightOffset;
			break;
		case XmNtopOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my topOffset;
			break;
		case XmNbottomOffset:
			Melder_assert (my parent -> widgetClass == xmFormWidgetClass);
			*va_arg (arg, int *) = my bottomOffset;
			break;
		case XmNminimum: *va_arg (arg, int *) = my minimum; break;
		case XmNmaximum: *va_arg (arg, int *) = my maximum; break;
		case XmNvalue: *va_arg (arg, int *) = my value; break;
		case XmNincrement:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my increment;
			break;
		case XmNpageIncrement:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my pageIncrement;
			break;
		case XmNsliderSize:
			Melder_assert (my widgetClass == xmScrollBarWidgetClass);
			*va_arg (arg, int *) = my sliderSize;
			break;
		case XmNdeleteResponse:
			Melder_assert (my widgetClass == xmShellWidgetClass);
			*va_arg (arg, int *) = my deleteResponse;
			break;
		case XmNcolumns: *va_arg (arg, int *) = 40; break;
		case XmNhorizontalScrollBar: *va_arg (arg, GuiObject *) = my motiff.scrolledWindow.horizontalBar; break;
		case XmNverticalScrollBar: *va_arg (arg, GuiObject *) = my motiff.scrolledWindow.verticalBar; break;
		default: {
			if (resource < 0 || resource >= sizeof motif_resourceNames / sizeof (char *))
				Melder_flushError (U"(XtVaGetValues:) Resource out of range (", resource, U").");
			else
				Melder_flushError (U"(XtVaGetValues:) Unknown resource \"", Melder_peek8to32_u (motif_resourceNames [resource]), U"\".");
			return;
		}
	}
	va_end (arg);
}

void XtVaSetValues (GuiObject me, ...) {
	va_list arg;
	va_start (arg, me);
	_motif_setValues (me, arg);
	va_end (arg);
}

Window XtWindow (GuiObject me) {
	return (Window) my window;
}

/***** MOTIF *****/

void XmAddWMProtocolCallback (GuiObject me, Atom protocol, XtCallbackProc callback, char *closure) {
	if (protocol == 'delw') {
		my motiff.shell.goAwayCallback = callback;
		my motiff.shell.goAwayClosure = closure;
	}
}

GuiObject XmCreateBulletinBoard (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmBulletinBoardWidgetClass, parent, name);
}

GuiObject XmCreateBulletinBoardDialog (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	GuiObject shell = XmCreateDialogShell (parent, name, dum1, dum2);
	return XmCreateBulletinBoard (shell, name, dum1, dum2);
}

GuiObject XmCreateCascadeButton (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmCascadeButtonWidgetClass, parent, name);
}

GuiObject XmCreateCascadeButtonGadget (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmCascadeButtonGadgetClass, parent, name);
}

GuiObject XmCreateDialogShell (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	GuiObject shell;
	theDialogHint = True;
	shell = XmCreateShell (parent, name, dum1, dum2);
	theDialogHint = False;
	return shell;
}

GuiObject XmCreateForm (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmFormWidgetClass, parent, name);
}

GuiObject XmCreateFormDialog (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	GuiObject shell = XmCreateDialogShell (parent, name, dum1, dum2);
	return XmCreateForm (shell, name, dum1, dum2);
}

GuiObject XmCreateMenuBar (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmMenuBarWidgetClass, parent, name);
}

GuiObject XmCreatePulldownMenu (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmPulldownMenuWidgetClass, parent, name);
}

GuiObject XmCreateRadioBox (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	GuiObject result = createWidget (xmRowColumnWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	XtVaSetValues (result, XmNradioBehavior, True, NULL);
	return result;
}

GuiObject XmCreateRowColumn (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmRowColumnWidgetClass, parent, name);
}

GuiObject XmCreateScale (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScaleWidgetClass, parent, name);
}

GuiObject XmCreateScrollBar (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScrollBarWidgetClass, parent, name);
}

GuiObject XmCreateScrolledWindow (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmScrolledWindowWidgetClass, parent, name);
}

GuiObject XmCreateSeparator (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmSeparatorWidgetClass, parent, name);
}

GuiObject XmCreateSeparatorGadget (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmSeparatorWidgetClass, parent, name);
}

GuiObject XmCreateShell (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	GuiObject me = createWidget (xmShellWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	return me;
}

GuiObject XmCreateToggleButtonGadget (GuiObject parent, const char *name, ArgList dum1, int dum2) {
	(void) dum1;
	(void) dum2;
	return createWidget (xmToggleButtonWidgetClass, parent, name);
}

void XmScaleGetValue (GuiObject me, int *value_return) {
	Melder_assert (my widgetClass == xmScaleWidgetClass);
	*value_return = my value;
}

void XmScaleSetValue (GuiObject me, int value) {
	Melder_assert (my widgetClass == xmScaleWidgetClass);
	my value = value;
	SendMessage (my window, PBM_SETPOS, (WPARAM) value, 0);
	_motif_update (me, 0);
}

void XmScrollBarGetValues (GuiObject me, int *value, int *sliderSize,
	int *increment, int *pageIncrement)
{
	Melder_assert (my widgetClass == xmScrollBarWidgetClass);
	*value = my value;
	*sliderSize = my sliderSize;
	*increment = my increment;
	*pageIncrement = my pageIncrement;
}

void XmScrollBarSetValues (GuiObject me, int value, int sliderSize,
	int increment, int pageIncrement, Boolean notify)
{
	Melder_assert (my widgetClass == xmScrollBarWidgetClass);
	my value = value;
	my sliderSize = sliderSize;
	my increment = increment;
	my pageIncrement = pageIncrement;
	NativeScrollBar_set (me);
	if (notify)
		_Gui_callCallbacks (me, & my motiff.scrollBar.valueChangedCallbacks, NULL);
}

Boolean XmToggleButtonGadgetGetState (GuiObject me) {
	Melder_assert (MEMBER (me, ToggleButton));
	Melder_assert (my inMenu);
	return GetMenuState (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND) & MF_CHECKED ? True : False;
}

void XmToggleButtonGadgetSetState (GuiObject me, Boolean value, Boolean notify) {
	Melder_assert (MEMBER (me, ToggleButton));
	Melder_assert (my inMenu);
	NativeMenuItem_check (me, value);
	if (notify)
		_Gui_callCallbacks (me, & my motiff.toggleButton.valueChangedCallbacks, NULL);
}

static void _motif_update (GuiObject me, void *event) { (void) me; (void) event; }

/***** EVENT *****/

static void _motif_inspectTextWidgets (GuiObject me, GuiObject text,
	integer *p_numberOfTextWidgets, integer *p_textWidgetLocation)
{
	for (GuiObject sub = my firstChild; sub != NULL; sub = sub -> nextSibling) {
		if (MEMBER (sub, Shell))
			continue;
		if (MEMBER (sub, Text)) {
			(*p_numberOfTextWidgets) ++;
			if (sub == text)
				*p_textWidgetLocation = *p_numberOfTextWidgets;
		} else {
			_motif_inspectTextWidgets (sub, text, p_numberOfTextWidgets, p_textWidgetLocation);
		}
	}
}
static GuiObject _motif_getLocatedTextWidget (GuiObject me,
	integer *p_itextWidget, integer textWidgetLocation)
{
	for (GuiObject sub = my firstChild; sub != NULL; sub = sub -> nextSibling) {
		if (MEMBER (sub, Shell))
			continue;
		if (MEMBER (sub, Text)) {
			(*p_itextWidget) ++;
			if (*p_itextWidget == textWidgetLocation)
				return sub;
		} else {
			GuiObject locatedTextWidget = _motif_getLocatedTextWidget (sub, p_itextWidget, textWidgetLocation);
			if (locatedTextWidget) return locatedTextWidget;
		}
	}
	return NULL;
}
static GuiObject _motif_getNextTextWidget (GuiObject shell, GuiObject text, bool backward) {
	integer numberOfTextWidgets = 0, textWidgetLocation = 0;
	_motif_inspectTextWidgets (shell, text, & numberOfTextWidgets, & textWidgetLocation);
	trace (U"Found ", numberOfTextWidgets, U" text widgets.");
	if (numberOfTextWidgets == 0)
		return nullptr;   // no tab navigation if there is no text widget (shouldn't normally occur)
	Melder_assert (textWidgetLocation >= 1);
	Melder_assert (textWidgetLocation <= numberOfTextWidgets);
	if (numberOfTextWidgets == 1) return NULL;   // no tab navigation if there is only one text widget
	if (backward) {
		textWidgetLocation --;   // tab to previous text widget
		if (textWidgetLocation < 1)
			textWidgetLocation = numberOfTextWidgets;   // if at beginning, then tab around to last text widget
	} else {
		textWidgetLocation ++;   // tab to next text widget
		if (textWidgetLocation > numberOfTextWidgets)
			textWidgetLocation = 1;   // if at end, then tab around to first text widget
	}
	integer itextWidget = 0;
	return _motif_getLocatedTextWidget (shell, & itextWidget, textWidgetLocation);
}

static void on_scroll (GuiObject me, UINT part, int pos) {
	if (my maximum == my minimum)
		return;
	switch (part) {
		case SB_LINEUP:
			my value -= my increment;
			break;
		case SB_LINEDOWN:
			my value += my increment;
			break;
		case SB_PAGEUP:
			my value -= my pageIncrement;
			break;
		case SB_PAGEDOWN:
			my value += my pageIncrement;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			my value = my minimum + pos * ((my maximum - my minimum - my sliderSize) / (32767.0 - (32767.0 * my sliderSize) / (my maximum - my minimum)));
			break;
		default:
			break;
	}
	Melder_clip (my minimum, & my value, my maximum - my sliderSize);
	NativeScrollBar_set (me);
	if (part == SB_THUMBTRACK || part == SB_THUMBPOSITION)
		_Gui_callCallbacks (me, & my motiff.scrollBar.dragCallbacks, (XtPointer) (ULONG_PTR) part);
	else
		_Gui_callCallbacks (me, & my motiff.scrollBar.valueChangedCallbacks, (XtPointer) (ULONG_PTR) part);
}

void XtNextEvent (XEvent *xevent) {
	GetMessage (xevent, NULL, 0, 0);
}

static void processWorkProcsAndTimeOuts () {
	if (theNumberOfWorkProcs != 0)
		for (integer i = 9; i >= 1; i --)
			if (theWorkProcs [i])
				if (theWorkProcs [i] (theWorkProcClosures [i]))
					XtRemoveWorkProc (i);
	if (theNumberOfTimeOuts != 0) {
		const double now = Melder_clock ();
		for (integer i = 1; i < 10; i ++) if (theTimeOutProcs [i]) {
			static volatile double timeElapsed;
			timeElapsed = now - theTimeOutStarts [i];
			if (timeElapsed > theTimeOutIntervals [i]) {
				theTimeOutProcs [i] (theTimeOutClosures [i], & i);
				XtRemoveTimeOut (i);
			}
		}
	}
}

void GuiNextEvent (XEvent *xevent) {
	if (theNumberOfWorkProcs != 0 || theNumberOfTimeOuts != 0) {
		if (PeekMessage (xevent, 0, 0, 0, PM_REMOVE)) {   // Message available?
			;   // Hand message to XtDispatchEvent.
		} else {
			processWorkProcsAndTimeOuts ();   // Handle chores during idle time.
			xevent -> message = 0;   // Hand null message to XtDispatchEvent.
		}
	} else GetMessage (xevent, NULL, 0, 0);   // be neighbour-friendly: do not hand null events
}

static GuiObject _motif_findDrawingArea (GuiObject me) {
	if (my widgetClass == xmDrawingAreaWidgetClass)
		return me;
	for (GuiObject sub = my firstChild; sub != NULL; sub = sub -> nextSibling)
		if (! MEMBER (sub, Shell)) {   // only in same top window
			GuiObject drawingArea = _motif_findDrawingArea (sub);
			if (drawingArea)
				return drawingArea;
		}
	return NULL;   // no DrawingArea found
}

static int win_shell_processKeyboardEquivalent (GuiObject me, int kar, int modifiers) {
	//TRACE
	for (int imenu = 1; imenu <= MAXIMUM_NUMBER_OF_MENUS; imenu ++) if (theMenus [imenu] && theMenus [imenu] -> shell == me) {
		for (GuiObject child = theMenus [imenu] -> firstChild; child != NULL; child = child -> nextSibling) {
			if ((child -> widgetClass == xmPushButtonWidgetClass || child -> widgetClass == xmToggleButtonWidgetClass) &&
					child -> motiff.pushButton.acceleratorChar == kar &&
					child -> motiff.pushButton.acceleratorModifiers == modifiers) {
				if (child -> activateCallback && ! child -> insensitive) {
					child -> activateCallback (child, child -> activateClosure, 0);
					return 1;
				} else if (child -> widgetClass == xmToggleButtonWidgetClass) {
					XmToggleButtonGadgetSetState (child, 1 - XmToggleButtonGadgetGetState (child), False);
					_Gui_callCallbacks (child, & child -> motiff.toggleButton.valueChangedCallbacks, 0);
					return 1;
				}
			}
		}
	}
	if (me && my shell) {
		GuiObject drawingArea = _motif_findDrawingArea (my shell);
		if (drawingArea) {
			trace (U"kar5 ", kar);
			if ((modifiers & _motif_COMMAND_MASK) && kar >= 'A' && kar <= 'Z' && ! (modifiers & _motif_SHIFT_MASK))
				kar += 'a' - 'A';
			if (kar == VK_RETURN) kar = 10;
			if (kar == VK_LEFT)   kar = 0x2190;
			if (kar == VK_RIGHT)  kar = 0x2192;
			if (kar == VK_UP)     kar = 0x2191;
			if (kar == VK_DOWN)   kar = 0x2193;
			if (kar == VK_OEM_7)  kar = '\'';
			_GuiWinDrawingArea_handleKey (drawingArea, kar);   // TODO: event -> key?
			return 1;
		}
	}
	return 0;
}

static int win_processKeyboardEquivalent (GuiObject me, int kar, int modifiers) {
	/*
	 * First try to send the key command to the active shell.
	 * If that fails, try to send the key command to the application shell.
	 */
	if (win_shell_processKeyboardEquivalent (me, kar, modifiers))
		return 1;
	if (win_shell_processKeyboardEquivalent (theApplicationShell, kar, modifiers))
		return 1;
	return 0;
}

void XtDispatchEvent (XEvent *xevent) {
	//TRACE
	MSG *message = (MSG *) xevent;
	if (message -> message == 0)
		return;   // null message from PeekMessage during work proc or time out.
	if (message -> message == WM_APP_WORK_PROC) {
		auto *container = reinterpret_cast <GuiWinWorkProcWrapper_base *> (message -> lParam);
		try {
			container -> run ();
		} catch (MelderError) {
			Melder_flushError ();   // no error message should leave the event loop
		}
		delete container;   // called even if there was an exception
		return;
	}
/*if (message -> message == WM_KEYDOWN || message -> message == WM_SYSKEYDOWN)
{
int kar = LOWORD (message -> wParam);
int modifiers = 0;
GuiObject me = (GuiObject) GetWindowLongPtr (message -> hwnd, GWLP_USERDATA);
if (GetKeyState (VK_CONTROL) < 0) modifiers |= _motif_COMMAND_MASK;
if (GetKeyState (VK_MENU) < 0) modifiers |= _motif_OPTION_MASK;
if (GetKeyState (VK_SHIFT) < 0) modifiers |= _motif_SHIFT_MASK;
if(kar>=48)Melder_flushError ("modifiers:%s%s%s\nmessage: %s\nkar: %d",
modifiers & _motif_COMMAND_MASK ? " control" : "",
modifiers & _motif_OPTION_MASK ? " alt" : "",
modifiers & _motif_SHIFT_MASK ? " shift" : "", message -> message == WM_KEYDOWN ? "keydown" : "syskeydown", kar);
}*/
	/*
	 * Intercept accelerators, which may be function keys or Command combinations.
	 * Some Alt-GR combinations denote special characters (e.g. backslash) on some (e.g. German) keyboards;
	 * in such a case, the message is WM_KEYDOWN, and the reported modifier keys are Control (!) and Alt
	 * (on a German keyboard, the backslash can be generated by Ctrl-Alt-ringel-s as well);
	 * otherwise Alt keys give WM_SYSKEYDOWN messages.
	 */
	if (message -> message == WM_KEYDOWN && (GetKeyState (VK_CONTROL) < 0 || ! (message -> lParam & (1<<29))) ||
		message -> message == WM_SYSKEYDOWN && GetKeyState (VK_MENU) < 0
			&& (message -> lParam & (1<<29)) ||   // R&N 413: Alt key is pressed
		message -> message == WM_SYSKEYDOWN && GetKeyState (VK_CONTROL) < 0)
	{
		int kar = LOWORD (message -> wParam);
		trace (U"kar1 ", kar);
		GuiObject me = (GuiObject) GetWindowLongPtr (message -> hwnd, GWLP_USERDATA);
		int modifiers = 0;
		if (GetKeyState (VK_CONTROL) < 0)
			modifiers |= _motif_COMMAND_MASK;
		if (GetKeyState (VK_MENU) < 0)
			modifiers |= _motif_OPTION_MASK;
		if (GetKeyState (VK_SHIFT) < 0)
			modifiers |= _motif_SHIFT_MASK;
/*if(kar>=48)Melder_casual ("modifiers:%s%s%s\nmessage: %s\nkar: %d",
modifiers & _motif_COMMAND_MASK ? " control" : "",
modifiers & _motif_OPTION_MASK ? " alt" : "",
modifiers & _motif_SHIFT_MASK ? " shift" : "", message -> message == WM_KEYDOWN ? "keydown" : "syskeydown", kar);*/
		if (me && my shell) {
			uint32 acc = my shell -> motiff.shell.lowAccelerators [modifiers];
			//if (kar != VK_CONTROL) Melder_casual ("%d %d", acc, kar);
			if (kar < 48) {
				if (kar == VK_BACK) {   // shortcut or text
					if (acc & 1 << GuiMenu_BACKSPACE) { win_processKeyboardEquivalent (my shell, GuiMenu_BACKSPACE, modifiers); return; }
				} else if (kar == VK_TAB) {   // shortcut or text
					if (acc & 1 << GuiMenu_TAB) { win_processKeyboardEquivalent (my shell, GuiMenu_TAB, modifiers); return; }
				} else if (kar == VK_RETURN) {   // shortcut, default button, or text
					//Melder_information (U"RETURN ", acc, U" def ", Melder_pointer (my shell -> defaultButton));
					if (acc & 1 << GuiMenu_ENTER) { win_processKeyboardEquivalent (my shell, GuiMenu_ENTER, modifiers); return; }
					else {
						if (my shell -> defaultButton && _GuiWinButton_tryToHandleShortcutKey (my shell -> defaultButton)) return;
					}
				} else if (kar == VK_ESCAPE) {   // shortcut or cancel button (and from 2024-04-06:) or text
					if (acc & 1 << GuiMenu_ESCAPE) { win_processKeyboardEquivalent (my shell, GuiMenu_ESCAPE, modifiers); return; }
					else {
						if (my shell -> cancelButton && _GuiWinButton_tryToHandleShortcutKey (my shell -> cancelButton)) return;
					}
					//return;   // this return has been here from before 2002; don't know why an Escape cannot be text (Demo wants it)
				} else if (kar == VK_PRIOR) {   // shortcut or text
					if (acc & 1 << GuiMenu_PAGE_UP) { win_processKeyboardEquivalent (my shell, GuiMenu_PAGE_UP, modifiers); return; }
				} else if (kar == VK_NEXT) {   // shortcut or text
					if (acc & 1 << GuiMenu_PAGE_DOWN) { win_processKeyboardEquivalent (my shell, GuiMenu_PAGE_DOWN, modifiers); return; }
				} else if (kar == VK_HOME) {   // shortcut or text
					if (acc & 1 << GuiMenu_HOME) { win_processKeyboardEquivalent (my shell, GuiMenu_HOME, modifiers); return; }
				} else if (kar == VK_END) {   // shortcut or text
					if (acc & 1 << GuiMenu_END) { win_processKeyboardEquivalent (my shell, GuiMenu_END, modifiers); return; }
				} else if (kar == VK_LEFT) {   // shortcut or text
					if (acc & 1 << GuiMenu_LEFT_ARROW) { win_processKeyboardEquivalent (my shell, GuiMenu_LEFT_ARROW, modifiers); return; }
				} else if (kar == VK_RIGHT) {   // shortcut or text
					if (acc & 1 << GuiMenu_RIGHT_ARROW) { win_processKeyboardEquivalent (my shell, GuiMenu_RIGHT_ARROW, modifiers); return; }
				} else if (kar == VK_UP) {   // shortcut or text
					if (acc & 1 << GuiMenu_UP_ARROW) { win_processKeyboardEquivalent (my shell, GuiMenu_UP_ARROW, modifiers); return; }
				} else if (kar == VK_DOWN) {   // shortcut or text
					if (acc & 1 << GuiMenu_DOWN_ARROW) { win_processKeyboardEquivalent (my shell, GuiMenu_DOWN_ARROW, modifiers); return; }
				} else if (kar == VK_INSERT) {   // shortcut
					win_processKeyboardEquivalent (my shell, GuiMenu_INSERT, modifiers);
					return;
				} else if (kar == VK_DELETE) {   // shortcut or text
					if (acc & 1 << GuiMenu_DELETE) { win_processKeyboardEquivalent (my shell, GuiMenu_DELETE, modifiers); return; }
				} else if (kar == VK_HELP) {   // simulate Command-?
					win_processKeyboardEquivalent (my shell, '?', modifiers | _motif_SHIFT_MASK);
					return;
				}
			} else if (kar >= VK_F1 && kar <= VK_F12) {   /* 112...123 */
				/*
				 * She has pressed one of the F keys.
				 */
				if (win_processKeyboardEquivalent (my shell, kar - VK_F1 + GuiMenu_F1, modifiers)) return;
				/* Let windowProc handle Alt-F4 etc. */
			/*
			 * If the Command key is pressed with a printable character, this is often a menu shortcut.
			 */
			} else if ((modifiers & _motif_COMMAND_MASK) && ! (modifiers & _motif_OPTION_MASK)) {
				if (MEMBER (me, Text) && (kar == 'X' || kar == 'C' || kar == 'V' || kar == 'Z')) {
					;   // let window proc handle text editing
				} else if (kar >= 186) {
					const int shift = modifiers & _motif_SHIFT_MASK;
					/*
					 * BUG: The following is not internationally correct.
					 */
					if (kar == 186 && win_processKeyboardEquivalent (my shell, shift ? ':' : ';', modifiers) ||
						kar == 187 && win_processKeyboardEquivalent (my shell, shift ? '+' : '=', modifiers) ||
						kar == 188 && win_processKeyboardEquivalent (my shell, shift ? '<' : ',', modifiers) ||
						kar == 189 && win_processKeyboardEquivalent (my shell, shift ? '_' : '-', modifiers) ||
						kar == 190 && win_processKeyboardEquivalent (my shell, shift ? '>' : '.', modifiers) ||
						kar == 191 && win_processKeyboardEquivalent (my shell, shift ? '?' : '/', modifiers) ||
						kar == 192 && win_processKeyboardEquivalent (my shell, shift ? '~' : '`', modifiers) ||
						kar == 219 && win_processKeyboardEquivalent (my shell, shift ? '{' : '[', modifiers) ||   // Alt-GR-ringel-s is here
						kar == 220 && win_processKeyboardEquivalent (my shell, shift ? '|' : '\\', modifiers) ||
						kar == 221 && win_processKeyboardEquivalent (my shell, shift ? '}' : ']', modifiers) ||
						kar == 222 && win_processKeyboardEquivalent (my shell, shift ? '\"' : '\'', modifiers))
					{
						return;
					}
				} else {
					if (win_processKeyboardEquivalent (my shell, kar, modifiers))
						return;   // handle shortcuts like Ctrl-T (but not Ctrl-Alt-T, because of Alt_GR on some keyboards)
					/* Let window proc handle international Alt-GR (= Ctrl-Alt) sequences, which are plain characters. */
				}
			}
			/* Other characters: to text. */
		}
		/* Not me or not my shell: let windowProc handle. */
	} else if (message -> message == WM_CHAR) {
		int kar = LOWORD (message -> wParam);
		/*
		 * Catch character messages to push buttons and toggle buttons:
		 * divert them to a drawing area, if possible.
		 */
		GuiObject me = (GuiObject) GetWindowLongPtr (message -> hwnd, GWLP_USERDATA);
		if (me && MEMBER2 (me, PushButton, ToggleButton)) {
			GuiObject drawingArea = _motif_findDrawingArea (my shell);
			if (drawingArea) {
				trace (U"kar2 ", kar);
				if (kar == VK_RETURN) kar = 10;
				if (kar == VK_LEFT)   kar = 0x2190;
				if (kar == VK_RIGHT)  kar = 0x2192;
				if (kar == VK_UP)     kar = 0x2191;
				if (kar == VK_DOWN)   kar = 0x2193;
				if (kar == VK_OEM_7)  kar = '\'';
				_GuiWinDrawingArea_handleKey (drawingArea, kar);   // TODO: event -> key?
				return;
			}
		}
		/*
		 * Next, try tab navigation.
		 */
		if (me && MEMBER (me, Text) && kar == 9) {
			trace (U"Tab navigation with shell ", Melder_pointer (my shell), U" and from text widget ", Melder_pointer (me));
			GuiObject nextTextWidget = _motif_getNextTextWidget (my shell, me, GetKeyState (VK_SHIFT) < 0);
			trace (U"Tab navigation to text widget ", Melder_pointer (nextTextWidget));
			if (nextTextWidget != NULL) {
				_GuiText_setTheTextFocus (nextTextWidget);
				GuiText_setSelection ((GuiText) nextTextWidget -> userData, 0, 10000000);
				return;
			}
		}
	} else if (message -> message == WM_LBUTTONDOWN) {
		/*
		 * Catch mouse-down messages to cascade buttons:
		 * post the associated menu, if any.
		 */
		GuiObject me = (GuiObject) GetWindowLongPtr (message -> hwnd, GWLP_USERDATA);
		//Melder_information (Melder_pointer (me), U" -- ", Melder_pointer (my subMenuId));
		if (me && MEMBER (me, CascadeButton) && my subMenuId) {
			RECT rect;
			GetWindowRect (my window, & rect);
			TrackPopupMenu (my subMenuId -> nat.menu.handle, TPM_LEFTBUTTON |
				TPM_LEFTALIGN | TPM_TOPALIGN, rect.left, rect.bottom - 3, 0, my parent -> window, NULL);
			return;
		}
	}
	TranslateMessage (xevent);   // Generate WM_CHAR messages.
	DispatchMessage (xevent);
}

void GuiMainLoop () {
	for (;;) {
		XEvent event;
		GuiNextEvent (& event);
		XtDispatchEvent (& event);
	}
}

#define main wingwmain
extern int main (int argc, char *argv []);
int APIENTRY WinMain (HINSTANCE instance, HINSTANCE /*previousInstance*/, LPSTR commandLine, int commandShow) {
	trace (U"Entering WinMain");
	theGui.instance = instance;
	theGui.commandShow = commandShow;
	int argc;
	WCHAR** argvW = CommandLineToArgvW (GetCommandLineW (), & argc);
	char** argv = Melder_malloc (char*, argc);
	for (int iarg = 0; iarg < argc; iarg ++)
		argv [iarg] = Melder_32to8 (Melder_peekWto32 (argvW [iarg])).transfer();
	/*
		It cannot hurt to establish the current thread as the GUI thread,
		by calling one event-loop function (without further side effects).
		Perhaps this will help later calls to PostMessage().
	*/
	{
		MSG dummy;
		PeekMessage (& dummy, nullptr, 0, 0, PM_NOREMOVE);
	}
	return main (argc, argv);
}

static void on_close (HWND window) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (my widgetClass == xmShellWidgetClass) {
			int deleteResponse = my deleteResponse;   // save this, in case the callback should kill the widget (XmDO_NOTHING)
			GuiObject parent = my parent;
			if (my motiff.shell.goAwayCallback)
				my motiff.shell.goAwayCallback (me, my motiff.shell.goAwayClosure, NULL);
			if (deleteResponse == XmDESTROY) {
				XtDestroyWidget (me);
			} else if (deleteResponse == XmUNMAP) {
				/*
					Unmapping is not the same as unmanaging.
					One of the oldest bugs in Praat was that in the Windows edition
					the Picture window could not be closed a second time,
					and that after it had been closed and opened the scroll bars did not move when
					the user resized the window.
					The solution was to use only some of the code from XtUnmanageChild here,
					without clearing the 'managed' attribute.
				*/
				_GuiText_handleFocusLoss (my textFocus);
				ShowWindow (my window, SW_HIDE);
			}
			return;
		} else FORWARD_WM_CLOSE (window, DefWindowProc);
	} else FORWARD_WM_CLOSE (window, DefWindowProc);
}
static GuiObject findItem (GuiObject menu, int id) {
	GuiObject child = menu -> firstChild;
	for (child = menu -> firstChild; child != NULL; child = child -> nextSibling) {
		if (child -> widgetClass == xmPulldownMenuWidgetClass) {
			GuiObject result = findItem (child, id);
			if (result)
				return result;
		} else {
			Melder_assert (MEMBER4 (child, PushButton, ToggleButton, CascadeButton, Separator));
			if (child -> nat.entry.id == id)
				return child;
		}
	}
	return NULL;
}
static void on_command (HWND window, int id, HWND controlWindow, UINT codeNotify) {
	GuiObject parent = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (parent) {
		if (controlWindow) {
			GuiObject control = (GuiObject) GetWindowLongPtr (controlWindow, GWLP_USERDATA);
			if (control) {
				switch (control -> widgetClass) {
					/*
					 * Clicking on a button takes away the focus from a text widget.
					 * We have already been notified of that by EN_KILLFOCUS.
					 * We have to put the focus back (see GuiText.c, parenthesized discussion 1.4);
					 * this has to be done before calling the callbacks, because these may destroy or hide the text widget.
					 */
					case xmPushButtonWidgetClass:
						/*
						 * If EN_KILLFOCUS had not cleared the global text focus,
						 * the following message would not actually do SetFocus!
						 */
						_GuiText_setTheTextFocus (control -> shell -> textFocus);
						_GuiWinButton_handleClick (control);
						break;
					case xmToggleButtonWidgetClass:
						_GuiText_setTheTextFocus (control -> shell -> textFocus);
						if (control -> isRadioButton)
							_GuiWinRadioButton_handleClick (control);
						else
							_GuiWinCheckButton_handleClick (control);
						break;
					case xmListWidgetClass:
						if (codeNotify == LBN_SELCHANGE) {
							_GuiWinList_handleClick (control);
						} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
						break;
					case xmTextWidgetClass:
						if (codeNotify == EN_CHANGE)
							_GuiText_handleValueChanged (control);
						else if (codeNotify == EN_SETFOCUS)
							_GuiText_handleFocusReception (control);
						else if (codeNotify == EN_KILLFOCUS)
							_GuiText_handleFocusLoss (control);   // for button clicks (see above)
						break;
					default:
						break;
				}
			} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
		} else {   // menu choice
			GuiObject menuBar = NULL;
			if (MEMBER (parent, Shell))
				menuBar = parent -> nat.shell.menuBar;
			else if (MEMBER (parent, RowColumn))
				menuBar = parent;
			if (menuBar) {
				GuiObject item = findItem (menuBar, id);
				if (item) {
					if (item -> widgetClass == xmPushButtonWidgetClass) {
						if (item -> activateCallback)
							item -> activateCallback (item, item -> activateClosure, 0);
					} else if (item -> widgetClass == xmToggleButtonWidgetClass) {
						XmToggleButtonGadgetSetState (item, 1 - XmToggleButtonGadgetGetState (item), False);
						_Gui_callCallbacks (item, & item -> motiff.toggleButton.valueChangedCallbacks, 0);
					}
				}
			}
		}
	} else FORWARD_WM_COMMAND (window, id, controlWindow, codeNotify, DefWindowProc);
}
static void on_destroy (HWND window) {
	(void) window;
}
static void on_lbuttonDown (HWND window, BOOL doubleClick, int x, int y, UINT flags) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (MEMBER (me, DrawingArea)) {
			SetCapture (window);
			_GuiWinDrawingArea_handleMouse (me, structGuiDrawingArea_MouseEvent::Phase::CLICK, x, y);
		} else FORWARD_WM_LBUTTONDOWN (window, doubleClick, x, y, flags, DefWindowProc);
	} else FORWARD_WM_LBUTTONDOWN (window, doubleClick, x, y, flags, DefWindowProc);
}
static void on_mouseMove (HWND window, int x, int y, UINT flags) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (MEMBER (me, DrawingArea) && (flags & MK_LBUTTON)) {
			_GuiWinDrawingArea_handleMouse (me, structGuiDrawingArea_MouseEvent::Phase::DRAG, x, y);
		} else FORWARD_WM_MOUSEMOVE (window, x, y, flags, DefWindowProc);
	} else FORWARD_WM_MOUSEMOVE (window, x, y, flags, DefWindowProc);
}
static void on_lbuttonUp (HWND window, int x, int y, UINT flags) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (MEMBER (me, DrawingArea)) {
			ReleaseCapture ();
			_GuiWinDrawingArea_handleMouse (me, structGuiDrawingArea_MouseEvent::Phase::DROP, x, y);
		} else FORWARD_WM_LBUTTONUP (window, x, y, flags, DefWindowProc);
	} else FORWARD_WM_LBUTTONUP (window, x, y, flags, DefWindowProc);
}
static void on_paint (HWND window) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (my widgetClass == xmDrawingAreaWidgetClass) {
			_GuiWinDrawingArea_update (me);
		} else FORWARD_WM_PAINT (window, DefWindowProc);
	} else FORWARD_WM_PAINT (window, DefWindowProc);
}
static void on_hscroll (HWND window, HWND controlWindow, UINT code, int pos) {
	GuiObject parent = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (parent) {
		GuiObject control = (GuiObject) GetWindowLongPtr (controlWindow, GWLP_USERDATA);
		if (control) {
			on_scroll (control, code, pos);
		} else FORWARD_WM_HSCROLL (window, controlWindow, code, pos, DefWindowProc);
	} else FORWARD_WM_HSCROLL (window, controlWindow, code, pos, DefWindowProc);
}
static void on_vscroll (HWND window, HWND controlWindow, UINT code, int pos) {
	GuiObject parent = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (parent) {
		GuiObject control = (GuiObject) GetWindowLongPtr (controlWindow, GWLP_USERDATA);
		if (control) {
			on_scroll (control, code, pos);
		} else FORWARD_WM_VSCROLL (window, controlWindow, code, pos, DefWindowProc);
	} else FORWARD_WM_VSCROLL (window, controlWindow, code, pos, DefWindowProc);
}
#undef FORWARD_WM_MOUSEWHEEL   // bug in our windowsx.h
#define FORWARD_WM_MOUSEWHEEL(hwnd,xPos,yPos,zDelta,fwKeys,fn) \
	(void)(fn)((hwnd),WM_MOUSEWHEEL,MAKEWPARAM((fwKeys),(zDelta)),MAKELPARAM((xPos),(yPos)))
//#define HANDLE_WM_MOUSEWHEEL(hwnd,wParam,lParam,fn) \
	((fn)((hwnd),(int)(short)LOWORD(lParam),(int)(short)HIWORD(lParam),(int)(short)HIWORD(wParam),(UINT)(short)LOWORD(wParam)),(LRESULT)0)
static void on_mouseWheel (HWND window, int xPos, int yPos, int zDelta, int fwKeys) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (my widgetClass == xmDrawingAreaWidgetClass) {
			const bool isHorizontal = ( fwKeys & MK_SHIFT );
			const int direction = ( isHorizontal ? ( zDelta < 0 ? SB_LINELEFT : SB_LINERIGHT ) : ( zDelta < 0 ? SB_LINEDOWN : SB_LINEUP ) );
			const bool controlKeyPressed = ( fwKeys & MK_CONTROL );
			if (controlKeyPressed) {
				_GuiWinDrawingArea_handleZoom (me, double (zDelta) / 10.0);
			} else if (my parent -> widgetClass == xmScrolledWindowWidgetClass) {
				on_scroll (isHorizontal ? my parent -> motiff.scrolledWindow.horizontalBar : my parent -> motiff.scrolledWindow.verticalBar, direction, 0);
			} else if (isHorizontal) {
				for (GuiObject child = my parent -> firstChild; child; child = child -> nextSibling)
					if (child -> widgetClass == xmScrollBarWidgetClass && child -> orientation == XmHORIZONTAL)
						on_scroll (child, direction, 0);
			} else {
				for (GuiObject child = my parent -> firstChild; child; child = child -> nextSibling)
					if (child -> widgetClass == xmScrollBarWidgetClass && child -> orientation == XmVERTICAL)
						on_scroll (child, direction, 0);
			}
		} else FORWARD_WM_MOUSEWHEEL (window, xPos, yPos, zDelta, fwKeys, DefWindowProc);
	} else FORWARD_WM_MOUSEWHEEL (window, xPos, yPos, zDelta, fwKeys, DefWindowProc);
}
static void on_size (HWND window, UINT state, int cx, int cy) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me && MEMBER (me, Shell) && (state == SIZE_RESTORED || state == SIZE_MAXIMIZED)) {
		const int oldWidth = my width, oldHeight = my height;
		const int newWidth = cx;
		const int newHeight = cy;
		my width = newWidth;
		my height = newHeight;
		FORWARD_WM_SIZE (window, state, cx, cy, DefWindowProc);
		if (! my managed) {
		} else if (my nat.shell.duringMoveWindow)   // Yeah, a BUG fix. Only react to user actions.
			my nat.shell.duringMoveWindow = False;
		else if (newWidth != oldWidth || newHeight != oldHeight) {
			shellResizeWidget (me, 0, 0, newWidth - oldWidth, newHeight - oldHeight);
		}
	} else FORWARD_WM_SIZE (window, state, cx, cy, DefWindowProc);
}
static void on_key (HWND window, UINT key, BOOL down, int repeat, UINT flags) {
	//TRACE
	trace (U"KEY ", key);
	Melder_assert (down == true);
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me && key >= VK_LEFT && key <= VK_DOWN) {
		//Melder_warning (U"Widget type ", my widgetClass);
		if (MEMBER (me, Shell) || my widgetClass == xmDrawingAreaWidgetClass) {   // any change in this condition should be mirrored in `on_char`
			GuiObject drawingArea = _motif_findDrawingArea (me);
			if (drawingArea) {
				GuiObject textFocus = drawingArea -> shell -> textFocus;   // BUG: ignore?
				trace (U"kar3 ", key);
				if (key == VK_RETURN) key = 10;
				if (key == VK_LEFT)   key = 0x2190;
				if (key == VK_RIGHT)  key = 0x2192;
				if (key == VK_UP)     key = 0x2191;
				if (key == VK_DOWN)   key = 0x2193;
				if (key == VK_OEM_7)  key = '\'';
				_GuiWinDrawingArea_handleKey (drawingArea, key);
			} else {
				FORWARD_WM_KEYDOWN (window, key, repeat, flags, DefWindowProc);
			}
		} else FORWARD_WM_KEYDOWN (window, key, repeat, flags, DefWindowProc);
	} else {
		FORWARD_WM_KEYDOWN (window, key, repeat, flags, DefWindowProc);
	}
}
static void on_char (HWND window, TCHAR kar, int repeat) {
	//TRACE
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me) {
		if (MEMBER (me, Shell) || my widgetClass == xmDrawingAreaWidgetClass) {   // any change in this condition should be mirrored in `on_key`
			GuiObject drawingArea = _motif_findDrawingArea (me);
			if (drawingArea) {
				GuiObject textFocus = drawingArea -> shell -> textFocus;   // BUG: ignore?
				trace (U"kar4 ", kar);
				_GuiWinDrawingArea_handleKey (drawingArea, kar);
			} else {
				FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
			}
		} else FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
	} else {
		FORWARD_WM_CHAR (window, kar, repeat, DefWindowProc);
	}
}
static void on_move (HWND window, int x, int y) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	/*if (me && MEMBER (me, Shell)) {
		my x = x - ( my motiff.shell.isDialog ? GetSystemMetrics (SM_CXFIXEDFRAME) : GetSystemMetrics (SM_CXSIZEFRAME) );
		my y = y - GetSystemMetrics (SM_CYCAPTION) - ( my motiff.shell.isDialog ? GetSystemMetrics (SM_CYFIXEDFRAME) :
			GetSystemMetrics (SM_CYSIZEFRAME) + GetSystemMetrics (SM_CYMENU) );
	}*/
	FORWARD_WM_MOVE (window, x, y, DefWindowProc);
}
static HBRUSH on_ctlColorStatic (HWND window, HDC hdc, HWND controlWindow, int type) {
	GuiObject parent = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	(void) type;
	if (parent) {
		GuiObject control = (GuiObject) GetWindowLongPtr (controlWindow, GWLP_USERDATA);
		if (control) {
			SetBkMode (hdc, TRANSPARENT);
			return theWinGuiBackgroundBrush ();
		}
	}
	return FORWARD_WM_CTLCOLORSTATIC (window, hdc, controlWindow, DefWindowProc);
}
static HBRUSH on_ctlColorBtn (HWND window, HDC hdc, HWND controlWindow, int type) {
	GuiObject parent = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	(void) type;
	if (parent) {
		GuiObject control = (GuiObject) GetWindowLongPtr (controlWindow, GWLP_USERDATA);
		if (control) {
			SetBkMode (hdc, TRANSPARENT);
			return theWinGuiBackgroundBrush ();
		}
	}
	return FORWARD_WM_CTLCOLORBTN (window, hdc, controlWindow, DefWindowProc);
}
static void on_activate (HWND window, UINT state, HWND hActive, BOOL minimized) {
	GuiObject me = (GuiObject) GetWindowLongPtr (window, GWLP_USERDATA);
	if (me && my widgetClass == xmShellWidgetClass) {
		if (state == WA_INACTIVE || minimized) {
			_GuiText_handleFocusLoss (my textFocus);
		} else {
			_GuiText_setTheTextFocus (my textFocus);
		}
		return;
	} else FORWARD_WM_ACTIVATE (window, state, hActive, minimized, DefWindowProc);
}
static LRESULT CALLBACK windowProc (HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		HANDLE_MSG (window, WM_CLOSE, on_close);
		HANDLE_MSG (window, WM_COMMAND, on_command);
		HANDLE_MSG (window, WM_DESTROY, on_destroy);
		HANDLE_MSG (window, WM_LBUTTONDOWN, on_lbuttonDown);
		HANDLE_MSG (window, WM_LBUTTONDBLCLK, on_lbuttonDown);   // double-click counts as two clicks
		HANDLE_MSG (window, WM_LBUTTONUP, on_lbuttonUp);
		HANDLE_MSG (window, WM_MOUSEMOVE, on_mouseMove);
		HANDLE_MSG (window, WM_PAINT, on_paint);
		HANDLE_MSG (window, WM_HSCROLL, on_hscroll);
		HANDLE_MSG (window, WM_VSCROLL, on_vscroll);
		HANDLE_MSG (window, WM_MOUSEWHEEL, on_mouseWheel);
		HANDLE_MSG (window, WM_SIZE, on_size);
		HANDLE_MSG (window, WM_KEYDOWN, on_key);
		HANDLE_MSG (window, WM_CHAR, on_char);
		HANDLE_MSG (window, WM_MOVE, on_move);
		HANDLE_MSG (window, WM_CTLCOLORBTN, on_ctlColorBtn);
		HANDLE_MSG (window, WM_CTLCOLORSTATIC, on_ctlColorStatic);
		HANDLE_MSG (window, WM_ACTIVATE, on_activate);
		case WM_POINTERWHEEL: {   // from Windows 8 on
			int zDelta = GET_WHEEL_DELTA_WPARAM (wParam);
			int fwKeys = GET_KEYSTATE_WPARAM (wParam);
			if (GetKeyState (VK_SHIFT) < 0)
				fwKeys |= MK_SHIFT;
			if (GetKeyState (VK_CONTROL) < 0)
				fwKeys |= MK_CONTROL;
			POINT point = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
			ScreenToClient (window, & point);
			on_mouseWheel (window, point.x, point.y, zDelta, fwKeys);
			return 0;
		}
		case WM_POINTERHWHEEL: {   // from Windows 8 on
			int zDelta = GET_WHEEL_DELTA_WPARAM (wParam);
			int fwKeys = GET_KEYSTATE_WPARAM (wParam) | MK_SHIFT;
			POINT point = { GET_X_LPARAM (lParam), GET_Y_LPARAM (lParam) };
			ScreenToClient (window, & point);
			on_mouseWheel (window, point.x, point.y, zDelta, fwKeys);
			return 0;
		}
		case WM_USER:   // TODO: remove once Elan's sendpraat is updated to using WM_APP instead of WM_USER
		case WM_APP:
		{
			/*if (IsIconic (window)) ShowWindow (window, SW_RESTORE);
			SetForegroundWindow (window);*/
			//TRACE
			trace (U"app message ", message);
			return theUserMessageCallback ? theUserMessageCallback () : 1;
		}
		default: return DefWindowProc (window, message, wParam, lParam);
	}
}
void motif_win_setUserMessageCallback (int (*userMessageCallback) (void)) {
	theUserMessageCallback = userMessageCallback;
}

#endif // defined (_WIN32)

/* End of file motifEmulator.cpp */
