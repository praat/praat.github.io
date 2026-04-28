/* Printer.cpp
 *
 * Copyright (C) 1998-2017,2021,2023,2025,2026 Paul Boersma
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

#include <unistd.h> // close

#include "melder.h"

#if defined (_WIN32)
	#include <windows.h>
#endif

#include "Printer.h"
#include "praat.h"   // topShell
#include "Ui.h"
#include "site.h"
#include "GraphicsP.h"

#if cocoa
	#include "Picture.h"
#endif

/*
	Everything must look the same on every printer, including on PDF,
	so the margins must be constant with respect to the paper, not to the writable page.
*/
Printer thePrinter = {
	kGraphicsPostscript_spots::DEFAULT,
	kGraphicsPostscript_paperSize::DEFAULT,
	kGraphicsPostscript_orientation::DEFAULT,
	false, true,
	kGraphicsPostscript_fontChoiceStrategy::DEFAULT,
	600, 5100, 6600,
	1.0
};

void Printer_prefs () {
	Preferences_addEnum (U"Printer.spots", & thePrinter. spots, kGraphicsPostscript_spots, kGraphicsPostscript_spots::DEFAULT);
	Preferences_addEnum (U"Printer.paperSize", & thePrinter. paperSize, kGraphicsPostscript_paperSize, kGraphicsPostscript_paperSize::DEFAULT);
	Preferences_addBool (U"Printer.allowDirectPostScript", & thePrinter. allowDirectPostScript, true);
	Preferences_addEnum (U"Printer.fontChoiceStrategy", & thePrinter. fontChoiceStrategy, kGraphicsPostscript_fontChoiceStrategy, kGraphicsPostscript_fontChoiceStrategy::DEFAULT);
}

#if cocoa
	static NSView *theMacView;
#endif
#ifdef _WIN32
	static PRINTDLG theWinPrint;
	static HDC theWinDC;
#endif

#if defined (_WIN32)
	int Printer_postScript_printf (void *stream, const char *format, ... ) {
		static union { char chars [3002]; short shorts [1501]; } theLine;
		int length;
		va_list args;
		va_start (args, format);
		(void) stream;
		vsprintf (theLine.chars + 2, format, args);
		length = strlen (theLine.chars + 2);   // TODO: check overflow
		theLine.shorts [0] = length;
		if (length > 0 && theLine.chars [length + 1] == '\n') {
			theLine.chars [length + 1] = '\r';
			theLine.chars [length + 2] = '\n';
			theLine.chars [length + 3] = '\0';
			length ++;
		}
		Escape (theWinDC, POSTSCRIPT_PASSTHROUGH, length + 2, theLine.chars, nullptr);
		va_end (args);
		return 1;
	}
#endif

#if defined (_WIN32)
	static void initPostScriptPage () {
		/*
		 * Save the driver's state.
		 */
		Printer_postScript_printf (nullptr, "/PraatPictureSaveObject save def\n");
		/*
		 * The LaserWriter driver puts the coordinates upside down.
		 * According to the PostScript Reference Manual,
		 * "There are few situations in which a PostScript language program
		 *  should execute initgraphics explicitly."
		 * This is one of them.
		 BUG: it probably is *not* one of them. Just do something like
		 	currentmatrix [1 0 -1 0 0 0] mul setmatrix
		 or whatever it is.
		 */
		#if 1
		Printer_postScript_printf (nullptr, "initmatrix initclip\n");
		#else
Printer_postScript_printf (nullptr, "8 8 scale initclip\n");
		#endif
	}
	static void exitPostScriptPage () {
		Printer_postScript_printf (nullptr, "PraatPictureSaveObject restore\n");
	}
#endif

#if cocoa
#elif defined (_WIN32)
	static void initPrinter () {
	}
#endif

void Printer_nextPage () {
	#if cocoa
		[theMacView endPage];
		[theMacView beginPageInRect: [theMacView bounds] atPlacement: NSMakePoint (0, 0)];
	#elif defined (_WIN32)
		if (thePrinter. postScript) {
			exitPostScriptPage ();
			EndPage (theWinDC);
			StartPage (theWinDC);
			initPostScriptPage ();
		} else {
			if (EndPage (theWinDC) < 0)
				(void) 0;   // FIXME: should give the opportunity of cancellation
			StartPage (theWinDC);
			/*
			 * Since StartPage has reset the DC, restore some of our non-default settings.
			 */
			SetBkMode (theWinDC, TRANSPARENT);
			SetTextAlign (theWinDC, TA_LEFT | TA_BASELINE | TA_NOUPDATECP);
		}
	#endif
}

int Printer_pageSetup () {
	#if cocoa
		NSPageLayout *cocoaPageSetupDialog = [NSPageLayout pageLayout];
		[cocoaPageSetupDialog runModal];
	#elif defined (_WIN32)
	#endif
	return 1;
}

#ifdef _WIN32
	static BOOL CALLBACK AbortFunc (HDC hdc, int nCode) {
		MSG msg;
		(void) hdc;
		(void) nCode;
		while (PeekMessage (& msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage (& msg);
			DispatchMessage (& msg);
		}
		return true;
	}
	HDC Printer_getDC () {
		if (! theWinPrint. hDevMode) {
			memset (& theWinPrint, 0, sizeof (PRINTDLG));
			theWinPrint. lStructSize = sizeof (PRINTDLG);
			theWinPrint. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
			PrintDlg (& theWinPrint);
		}
		return theWinPrint. hDC;
	}
#endif

#if cocoa
	static void (*theDraw) (void *boss, Graphics g);
	static void *theBoss;
	@interface GuiCocoaPrintingArea : NSView @end
	@implementation GuiCocoaPrintingArea {
		//GuiButton d_userData;
	}
	- (void) drawRect: (NSRect) dirtyRect {
		trace (U"printing ", dirtyRect. origin. x, U" ", dirtyRect. origin. y, U" ", dirtyRect. size. width, U" ", dirtyRect. size. height);
		int currentPage = [[NSPrintOperation currentOperation] currentPage];
		{// scope
			autoGraphics graphics = Graphics_create_screenPrinter (nullptr, self);
			theDraw (theBoss, graphics.get());
		}
	}
	- (BOOL) isFlipped {
		return YES;
	}
	- (NSPoint) locationOfPrintRect: (NSRect) aRect {
		(void) aRect;
		return NSMakePoint (0.0, 0.0);   // the origin of the rect's coordinate system is always the top left corner of the physical page
	}
	- (BOOL) knowsPageRange: (NSRangePointer) range {
		range -> length = 1;
		return YES;
	}
	- (NSRect) rectForPage: (NSInteger) pageNumber {
		(void) pageNumber;   // every page has the same rectangle
		return [self bounds];
	}
	- (void) printOperationDidRun: (NSPrintOperation *) printOperation  success: (BOOL) success  contextInfo: (void *) contextInfo {
	}
	@end
#endif

int Printer_print (void (*draw) (void *boss, Graphics g), void *boss) {
	try {
		#if defined (UNIX)
			structMelderFile tempFile { };
			char tempPath_utf8 [] = "/tmp/picXXXXXX";
			close (mkstemp (tempPath_utf8));
			Melder_pathToFile (Melder_peek8to32_u (tempPath_utf8), & tempFile);
			{// scope
				autoGraphics graphics = Graphics_create_postscriptjob (& tempFile, thePrinter. resolution,
					thePrinter. spots, thePrinter. paperSize, thePrinter. orientation, thePrinter. magnification);
				draw (boss, graphics.get());
			}
			char command [500];
			snprintf (command,500, Melder_peek32to8 (Site_getPrintCommand ()), tempPath_utf8);
			system (command);
			MelderFile_delete (& tempFile);
		#elif cocoa
			theDraw = draw;
			theBoss = boss;
			NSPrintInfo *info = [NSPrintInfo sharedPrintInfo];
			NSSize paperSize = [info paperSize];
			//NSLog (@"%f %f", paperSize. width, paperSize. height);
			thePrinter. paperWidth = paperSize. width / 0.12;
			thePrinter. paperHeight = paperSize. height / 0.12;
			[info setLeftMargin: 0.0];
			[info setRightMargin: 0.0];
			[info setTopMargin: 0.0];
			[info setBottomMargin: 0.0];
			/*
			 * Although the paper size reported may be 595 x 842 points (A4),
			 * 783 points (just under 11 inches) is the largest height that keeps the view on one page.
			 */
			int viewWidth = paperSize. width;
			int viewHeight = paperSize. height;
			NSLog (@"%d %d", viewWidth, viewHeight);
			NSRect rect = NSMakeRect (0, 0, viewWidth, viewHeight);
			NSView *cocoaPrintingArea = [[GuiCocoaPrintingArea alloc] initWithFrame: rect];
			theMacView = cocoaPrintingArea;
			[cocoaPrintingArea setBoundsSize: NSMakeSize (viewWidth / 0.12, viewHeight / 0.12)];   // 72 points per inch / 600 dpi = 0.12 points per dot
			[cocoaPrintingArea setBoundsOrigin: NSMakePoint (0, 0)];
			NSPrintOperation *op = [NSPrintOperation
				printOperationWithView: cocoaPrintingArea];
			#if 1
				if (op) [op runOperation];
			#else
				/*
				 * This may crash with multiple pages.
				 */
				if (op) {
					[op setCanSpawnSeparateThread: NO];
					NSView *pictureView = ((GraphicsScreen) Picture_peekGraphics ((Picture) boss)) -> d_macView;
					[op
						runOperationModalForWindow: [pictureView window]
						delegate: cocoaPrintingArea
						didRunSelector: @selector(printOperationDidRun:success:contextInfo:)
						contextInfo: nil
					];
				}
			#endif
		#elif defined (_WIN32)
			int postScriptCode = POSTSCRIPT_PASSTHROUGH;
			DOCINFO docInfo;
			DEVMODE *devMode;
			initPrinter ();
			#if 1
			if (! theWinPrint. hDevMode) {
				memset (& theWinPrint, 0, sizeof (PRINTDLG));
				theWinPrint. lStructSize = sizeof (PRINTDLG);
				theWinPrint. Flags = PD_RETURNDEFAULT;
				if (! PrintDlg (& theWinPrint))
					Melder_throw (U"Cannot initialize printer.");
			}
			#endif
			#if 0
			constexpr DWORD PRINTER_NAME_BUFFER_SIZE = 100;
			WCHAR printerName [PRINTER_NAME_BUFFER_SIZE];
			DWORD printerNameLengthIncludingTrailingNull = PRINTER_NAME_BUFFER_SIZE;
			GetDefaultPrinter (printerName, & printerNameLengthIncludingTrailingNull);
			Melder_casual (U"Printer_print: name ", Melder_peekWto32 (printerName));
			if (Melder_equ (Melder_peekWto32 (printerName), U"Microsoft Print to PDF")) {
				HWND hwnd = (HWND) XtWindow (theCurrentPraatApplication -> topShell -> d_xmShell);
				HANDLE printerHandle;
				OpenPrinter (printerName, & printerHandle, nullptr);
				WCHAR deviceName [] = L"device name";
				LONG devModeSize = DocumentProperties (hwnd, printerHandle, deviceName, nullptr, nullptr, 0);
				Melder_casual (U"Printer_print: devModeSize ", devModeSize);
				static BYTE devMode2 [5420];
				if (DocumentProperties (hwnd, printerHandle, deviceName, (PDEVMODEW) devMode2, nullptr, DM_OUT_BUFFER) < 0)
					Melder_casual (U"Printer_print: second call to DocumentProperties fails.");
				((DEVMODE *) devMode2) -> dmFields = DM_PAPERLENGTH | DM_PAPERWIDTH;
				((DEVMODE *) devMode2) -> dmPaperWidth = 6.0 * thePrinter. resolution;   // attempt to change, but...
				((DEVMODE *) devMode2) -> dmPaperLength = 4.0 * thePrinter. resolution;   // attempt to change, but...
				//((DEVMODE *) devMode2) -> dmPaperSize = 0;
				Melder_casual (U"Printer_print: paper width ", ((DEVMODE *) devMode2) -> dmPaperWidth);
				Melder_casual (U"Printer_print: paper length ", ((DEVMODE *) devMode2) -> dmPaperLength);
				if (DocumentProperties (hwnd, printerHandle, deviceName, (PDEVMODEW) devMode2, (PDEVMODEW) devMode2, DM_OUT_BUFFER | DM_IN_BUFFER) < 0)
					Melder_casual (U"Printer_print: third call to DocumentProperties fails.");
				Melder_casual (U"Printer_print: paper width ", ((DEVMODE *) devMode2) -> dmPaperWidth);   // ...as of 20210920, this has still been overridden
				Melder_casual (U"Printer_print: paper length ", ((DEVMODE *) devMode2) -> dmPaperLength);   // ...as of 20210920, this has still been overridden
				theWinPrint. hDC = CreateDCW (nullptr, L"Microsoft Print to PDF", nullptr, (PDEVMODEW) devMode2);
				Melder_casual (U"Printer_print: hDC ", Melder_pointer (theWinPrint. hDC));
				static DEVMODE * devMode3;
				devMode3 = (PDEVMODEW) devMode2;
				Melder_casual (U"Printer_print: devMode3 ", Melder_pointer (devMode3));
				theWinPrint. hDevMode = & devMode3;
				Melder_casual (U"Printer_print: theWinPrint. hDevMode ", Melder_pointer (theWinPrint. hDevMode));
				ClosePrinter (printerHandle);
			}
			#else
			if (Melder_backgrounding) {
				theWinPrint. Flags = PD_RETURNDEFAULT | PD_RETURNDC;
				if (! PrintDlg (& theWinPrint) || ! theWinPrint. hDC)
					Melder_throw (U"Cannot print from a script on this computer.");
			} else {
				theWinPrint. Flags &= ~ PD_RETURNDEFAULT;
				theWinPrint. Flags |= PD_RETURNDC;
				if (! PrintDlg (& theWinPrint))
					return 1;
			}
			#endif
			theWinDC = theWinPrint. hDC;
			thePrinter. postScript = thePrinter. allowDirectPostScript &&
				Escape (theWinDC, QUERYESCSUPPORT, sizeof (int), (LPSTR) & postScriptCode, nullptr);
			Melder_casual (U"Printer_print: PostScript ", thePrinter. postScript);
			/*
			 * The HP colour inkjet printer returns in dmFields:
			 * 0, 1, 8, 9, 10, 11, 12, 13, 14, 15, 23, 24, 25, 26 = DM_ORIENTATION |
			 *    DM_PAPERSIZE | DM_COPIES | DM_DEFAULTSOURCE | DM_PRINTQUALITY |
			 *    DM_COLOR | DM_DUPLEX | DM_YRESOLUTION | DM_TTOPTION | DM_COLLATE |
			 *    DM_ICMMETHOD | DM_ICMINTENT | DM_MEDIATYPE | DM_DITHERTYPE
			 */
			devMode = * (DEVMODE **) theWinPrint. hDevMode;
			thePrinter. resolution = devMode -> dmFields & DM_YRESOLUTION ? devMode -> dmYResolution :
				devMode -> dmFields & DM_PRINTQUALITY ?
				( devMode -> dmPrintQuality > 0 ? devMode -> dmPrintQuality : 300 ) : 300;
			if (devMode -> dmFields & DM_PAPERWIDTH) {
				thePrinter. paperWidth = devMode -> dmPaperWidth * thePrinter. resolution / 254;
				thePrinter. paperHeight = devMode -> dmPaperLength * thePrinter. resolution / 254;
			} else if (devMode -> dmFields & DM_PAPERSIZE) {
				static struct { float width, height; } sizes [] = { { 0, 0 }, { 8.5, 11 }, { 8.5, 11 }, { 11, 17 },
					{ 17, 11 }, { 8.5, 14 }, { 5.5, 8.5 }, { 7.25, 10.5 }, { 297/25.4, 420/25.4 },
					{ 210/25.4, 297/25.4 }, { 210/25.4, 297/25.4 }, { 148.5/25.4, 210/25.4 },
					{ 250/25.4, 354/25.4 }, { 182/25.4, 257/25.4 }, { 8.5, 13 },
					{ 215/25.4, 275/25.4 }, { 10, 14 }, { 11, 17 }, { 8.5, 11 }, { 3.875, 8.875 },
					{ 4.125, 9.5 }, { 4.5, 10.375 } };
				int paperSize = devMode -> dmPaperSize;
				if (paperSize <= 0 || paperSize > 21)
					paperSize = 1;
				thePrinter. paperWidth = sizes [paperSize]. width * thePrinter. resolution;
				thePrinter. paperHeight = sizes [paperSize]. height * thePrinter. resolution;
				if (devMode -> dmOrientation == DMORIENT_LANDSCAPE)
					std::swap (thePrinter. paperWidth, thePrinter. paperHeight);
			} else {
				thePrinter. paperWidth = 1000;
				thePrinter. paperHeight = 1000;
			}
			Melder_casual (U"Printer_print: paper width ", thePrinter. paperWidth);
			Melder_casual (U"Printer_print: paper length ", thePrinter. paperHeight);
			EnableWindow ((HWND) XtWindow (theCurrentPraatApplication -> topShell -> d_xmShell), false);
			SetAbortProc (theWinDC, AbortFunc);
			memset (& docInfo, 0, sizeof (DOCINFO));
			docInfo. cbSize = sizeof (DOCINFO);
			docInfo. lpszDocName = L"Praatjes";
			docInfo. lpszOutput = nullptr;
			if (thePrinter. postScript) {
				StartDoc (theWinDC, & docInfo);
				StartPage (theWinDC);
				initPostScriptPage ();
				{// scope
					autoGraphics graphics = Graphics_create_postscriptprinter ();
					draw (boss, graphics.get());
				}
				exitPostScriptPage ();
				EndPage (theWinDC);
				EndDoc (theWinDC);
			} else {
				StartDoc (theWinDC, & docInfo);
				StartPage (theWinDC);
				{// scope
					autoGraphics graphics = Graphics_create_screenPrinter (nullptr, theWinDC);
					draw (boss, graphics.get());
				}
				if (EndPage (theWinDC) < 0) {
					Melder_throw (U"Cannot print page.");
				} else {
					EndDoc (theWinDC);
				}
			}
			EnableWindow ((HWND) XtWindow (theCurrentPraatApplication -> topShell -> d_xmShell), true);
			DeleteDC (theWinDC), theWinDC = nullptr;
		#endif
		return 1;
	} catch (MelderError) {
		Melder_throw (U"Not printed.");
	}
}

/* End of file Printer.cpp */
