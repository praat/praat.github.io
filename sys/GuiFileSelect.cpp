/* GuiFileSelect.cpp
 *
 * Copyright (C) 2010-2026 Paul Boersma, 2013 Tom Naughton
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
#include <locale.h>
#if motif
	#include <shlobj.h>
#endif

autoStringSet GuiFileSelect_getInfileNames (GuiWindow optionalParent, conststring32 title, bool allowMultipleFiles) {
	autoMelderSaveCurrentFolder saveFolder;
	autoStringSet me = StringSet_create ();
	#if gtk
		static structMelderFolder folder { };
		GuiObject dialog = gtk_file_chooser_native_new (Melder_peek32to8 (title), nullptr, GTK_FILE_CHOOSER_ACTION_OPEN,
				"_Open", "_Cancel");
		Melder_assert (dialog);
		if (optionalParent)
			gtk_native_dialog_set_transient_for (GTK_NATIVE_DIALOG (dialog), GTK_WINDOW (optionalParent -> d_gtkWindow));
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), allowMultipleFiles);
		if (MelderFolder_isNull (& folder))   // first time?
			Melder_getCurrentFolder (& folder);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), MelderFolder_peekPath8 (& folder));
		if (gtk_native_dialog_run (GTK_NATIVE_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *infolderName_utf8 = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			if (infolderName_utf8) {
				conststring32 infolderName = Melder_peek8to32_u (infolderName_utf8);   // dangle
				Melder_pathToFolder (infolderName, & folder);
				g_free (infolderName_utf8);
			}
			GSList *infileNames_list = gtk_file_chooser_get_filenames (GTK_FILE_CHOOSER (dialog));
			for (GSList *element = infileNames_list; element != nullptr; element = g_slist_next (element)) {
				char *infileName_utf8 = (char *) element -> data;
				my addString_copy (Melder_peek8to32_u (infileName_utf8));
				g_free (infileName_utf8);
			}
			g_slist_free (infileNames_list);
		}
		gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (dialog));
		setlocale (LC_ALL, "C");
	#elif motif
		static OPENFILENAMEW openFileName;   // TODO: replace with Common Item Dialog API (since Vista)
		constexpr integer MAXIMUM_SIZE = 3'000'000;
		autostringW fullFileNameW (MAXIMUM_SIZE + 2);
		ZeroMemory (& openFileName, sizeof (OPENFILENAMEW));
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = ( optionalParent && optionalParent -> d_xmShell ? (HWND) XtWindow (optionalParent -> d_xmShell) : nullptr );
		openFileName. hInstance = nullptr;
		openFileName. lpstrFilter = L"All Files\0*.*\0";
		ZeroMemory (fullFileNameW.get(), (MAXIMUM_SIZE+2) * sizeof (WCHAR));
		openFileName. lpstrCustomFilter = nullptr;
		openFileName. nMaxCustFilter = 0;
		openFileName. lpstrFile = fullFileNameW.get();
		openFileName. nMaxFile = MAXIMUM_SIZE;
		openFileName. lpstrFileTitle = nullptr;
		openFileName. nMaxFileTitle = 0;
		openFileName. lpstrInitialDir = nullptr;
		openFileName. lpstrTitle = Melder_peek32toW_fileSystem (title);
		openFileName. Flags = OFN_EXPLORER | OFN_LONGNAMES | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY
				| (allowMultipleFiles ? OFN_ALLOWMULTISELECT : 0);
		openFileName. lpstrDefExt = nullptr;
		openFileName. lpfnHook = nullptr;
		openFileName. lpTemplateName = nullptr;
		openFileName. pvReserved = nullptr;
		openFileName. dwReserved = 0;
		openFileName. FlagsEx = 0;
		OSVERSIONINFO osVersionInfo;
		ZeroMemory (& osVersionInfo, sizeof (OSVERSIONINFO));
		osVersionInfo. dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		GetVersionEx (& osVersionInfo);
		if (GetOpenFileNameW (& openFileName)) {
			const integer firstFileNameLength = wcslen (fullFileNameW.get());
			if (fullFileNameW [firstFileNameLength + 1] == L'\0') {
				/*
					The user selected one file.
				*/
				my addString_copy (Melder_peekWto32 (fullFileNameW.get()));
			} else {
				/*
					The user selected multiple files.
					'fullFileNameW' is a folder name; the file names follow.
				*/
				structMelderFolder folder { };
				Melder_pathToFolder (Melder_peekWto32 (fullFileNameW.get()), & folder);
				for (const WCHAR *p = & fullFileNameW [firstFileNameLength + 1]; *p != L'\0'; p += wcslen (p) + 1) {
					structMelderFile file { };
					MelderFolder_getFile (& folder, Melder_peekWto32 (p), & file);
					my addString_copy (MelderFile_peekPath (& file));
				}
			}
		}
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) optionalParent;
		NSOpenPanel	*openPanel = [NSOpenPanel openPanel];
		[openPanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		[openPanel setAllowsMultipleSelection: allowMultipleFiles];
		[openPanel setCanChooseDirectories: NO];
		if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
			for (NSURL *url in [openPanel URLs]) {
				structMelderFile file { };
				Melder_8bitFileRepresentationToStr32_inplace ([[url path] UTF8String], file. path);   // BUG: unsafe buffer
				my addString_copy (MelderFile_peekPath (& file));
			}
		}
		setlocale (LC_ALL, "C");
	#endif
	return me;
}

autostring32 GuiFileSelect_getOutfileName (GuiWindow optionalParent, conststring32 title, conststring32 defaultName) {
	autoMelderSaveCurrentFolder saveFolder;
	autostring32 outfileName;
	#if gtk
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_native_new (Melder_peek32to8 (title), nullptr, GTK_FILE_CHOOSER_ACTION_SAVE,
				"_Save", "_Cancel");
		if (optionalParent)
			gtk_native_dialog_set_transient_for (GTK_NATIVE_DIALOG (dialog), GTK_WINDOW (optionalParent -> d_gtkWindow));
		gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), true);
		if (! MelderFile_isNull (& file))
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), MelderFile_peekPath8 (& file));
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), Melder_peek32to8 (defaultName));
		if (gtk_native_dialog_run (GTK_NATIVE_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *outfileName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			outfileName = Melder_8to32_e (outfileName_utf8);
			g_free (outfileName_utf8);
			Melder_pathToFile (outfileName.get(), & file);
		}
		gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (dialog));
		setlocale (LC_ALL, "C");
	#elif motif
		OPENFILENAMEW openFileName;
		ZeroMemory (& openFileName, sizeof (OPENFILENAMEW));
		static WCHAR customFilter [100+2];
		static WCHAR fullFileNameW [300+2];
		wcsncpy (fullFileNameW, Melder_peek32toW_fileSystem (defaultName), 300+2);
		fullFileNameW [300+1] = L'\0';
		openFileName. lStructSize = sizeof (OPENFILENAMEW);
		openFileName. hwndOwner = ( optionalParent && optionalParent -> d_xmShell ? (HWND) XtWindow (optionalParent -> d_xmShell) : nullptr );
		openFileName. lpstrFilter = nullptr;   // like *.txt
		openFileName. lpstrCustomFilter = customFilter;
		openFileName. nMaxCustFilter = 100;
		openFileName. lpstrFile = fullFileNameW;
		openFileName. nMaxFile = 300;
		openFileName. lpstrFileTitle = nullptr;
		openFileName. lpstrInitialDir = nullptr;
		openFileName. lpstrTitle = Melder_peek32toW_fileSystem (title);
		openFileName. Flags = OFN_LONGNAMES | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_HIDEREADONLY;
		openFileName. lpstrDefExt = nullptr;
		if (GetSaveFileNameW (& openFileName))
			outfileName = Melder_Wto32 (fullFileNameW);
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) optionalParent;
		NSSavePanel	*savePanel = [NSSavePanel savePanel];   // will be autoreleased (release will crash; 2020-11-12)
		[savePanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		[savePanel setNameFieldStringValue: [NSString stringWithUTF8String: Melder_peek32to8_fileSystem (defaultName)]];
		if ([savePanel runModal] == NSFileHandlingPanelOKButton) {
			NSString *path = [[savePanel URL] path];
			if (path == nil)
				Melder_throw (U"Don't understand where you want to save (1).");
			const char *outfileName_utf8 = [path UTF8String];
			if (! outfileName_utf8)
				Melder_throw (U"Don't understand where you want to save (2).");
			structMelderFile file { };
			Melder_8bitFileRepresentationToStr32_inplace (outfileName_utf8, file. path);   // BUG: unsafe buffer
			outfileName = Melder_dup (MelderFile_peekPath (& file));
		}
		setlocale (LC_ALL, "C");
	#endif
	return outfileName;
}

autostring32 GuiFileSelect_getFolderName (GuiWindow optionalParent, conststring32 title) {
	autoMelderSaveCurrentFolder saveFolder;
	autostring32 folderName;
	#if gtk
		static structMelderFile file;
		GuiObject dialog = gtk_file_chooser_native_new (Melder_peek32to8 (title), nullptr, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
					"Ch_oose", "_Cancel");
		if (optionalParent)
			gtk_native_dialog_set_transient_for (GTK_NATIVE_DIALOG (dialog), GTK_WINDOW (optionalParent -> d_gtkWindow));
		if (! MelderFile_isNull (& file))
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), MelderFile_peekPath8 (& file));
		if (gtk_native_dialog_run (GTK_NATIVE_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
			char *folderName_utf8 = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			folderName = Melder_8to32_e (folderName_utf8);
			g_free (folderName_utf8);
			Melder_pathToFile (folderName.get(), & file);
		}
		gtk_native_dialog_destroy (GTK_NATIVE_DIALOG (dialog));
		setlocale (LC_ALL, "C");
	#elif motif
		static WCHAR fullFileNameW [3000+2];
		static bool comInited = false;
		if (! comInited) {
			CoInitializeEx (nullptr, COINIT_APARTMENTTHREADED);
			comInited = true;
		}
		static BROWSEINFO info;
		ZeroMemory (& info, sizeof (BROWSEINFO));
		info. hwndOwner = ( optionalParent && optionalParent -> d_xmShell ? (HWND) XtWindow (optionalParent -> d_xmShell) : nullptr );
		info. ulFlags = BIF_USENEWUI;
		info. pidlRoot = nullptr;   // everything on the computer should be browsable
		info. pszDisplayName = nullptr;   // this would only give the bare folder name, not the full path
		info. lpszTitle = Melder_peek32toW_fileSystem (title);
		LPITEMIDLIST idList = SHBrowseForFolder (& info);
		SHGetPathFromIDList (idList, fullFileNameW);
		CoTaskMemFree (idList);
		folderName = Melder_Wto32 (fullFileNameW);
		setlocale (LC_ALL, "C");
	#elif cocoa
		(void) optionalParent;
		NSOpenPanel	*openPanel = [NSOpenPanel openPanel];
		[openPanel setTitle: [NSString stringWithUTF8String: Melder_peek32to8 (title)]];
		[openPanel setAllowsMultipleSelection: NO];
		[openPanel setCanChooseDirectories: YES];
		[openPanel setCanChooseFiles: NO];
		[openPanel setPrompt: @"Choose"];
		if ([openPanel runModal] == NSFileHandlingPanelOKButton) {
			for (NSURL *url in [openPanel URLs]) {
				const char *folderName_utf8 = [[url path] UTF8String];
				structMelderFolder folder { };
				Melder_8bitFileRepresentationToStr32_inplace (folderName_utf8, folder. path);   // BUG: unsafe buffer
				folderName = Melder_dup (MelderFolder_peekPath (& folder));
			}
		}
		setlocale (LC_ALL, "C");
	#endif
	return folderName;
}

/* End of file GuiFileSelect.cpp */
