/* ManPages.cpp
 *
 * Copyright (C) 1996-2026 Paul Boersma
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

#include "ManPages.h"

Thing_implement (ManPages, Daata, 0);

#define MAXIMUM_LINK_LENGTH  500

static integer lookUp_unsorted (ManPages me, conststring32 title);

void structManPages :: v9_destroy () noexcept {
	for (integer ipage = 1; ipage <= our pages.size; ipage ++) {
		ManPage page = our pages.at [ipage];
		for (integer ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
			ManPage_Paragraph par = & page -> paragraphs [ipar];
			Melder_free (par -> text);   // not an autostring32, because it can be a string literal (if not dynamic)
		}
		page -> linksHither. reset();   // TODO automate
		page -> linksThither. reset();
	}
	ManPages_Parent :: v9_destroy ();
}

static conststring32 ManPage_Paragraph_extractLink (ManPage_Paragraph par, const char32 *p, char32 *link, bool verbatimAware) {
	const bool paragraphIsVerbatim = ( verbatimAware && par -> couldVerbatim () );
	conststring32 text = par -> text;
	Melder_assert (text);
	char32 *to = & link [0];
	if (! p)
		p = text;
	/*
		Search for the next link.
	*/
	for (;; p ++) {
		if (*p == U'\0')
			return nullptr;   // no link found
		if (*p == U'@' && ! paragraphIsVerbatim) {
			/*
				Found a "@" in running text.
				Ignore it if it is inside a backslash trigraph.
			*/
			if (! (p - text <= 0 || (p [-1] != U'\\' && (p - text <= 1 || p [-2] != U'\\'))))
				continue;
			if (p [1] == U'@') {
				/*
					We found "@@", starting a link in running text.
				*/
				const char32 *from = p + 2;
				while (*from != U'@' && *from != U'|' && *from != U'\0') {
					if (to - link >= MAXIMUM_LINK_LENGTH)
						Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “@@” is too long:\n", text);
					*to ++ = *from ++;
				}
				/*
					Ignore the "|...@" part, unless it is "||...@".
				*/
				if (*from == U'|') {
					if (from [1] == U'|') {
						/*
							Found a "||xxx@" part. Append the xxx part.
						*/
						from += 2;   // skip "||"
						while (*from != U'@' && *from != U'\0') {
							if (*from == U'\0')
								break;
							if (to - link >= MAXIMUM_LINK_LENGTH)
								Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “@@” and containing “||” is too long:\n", text);
							*to ++ = *from ++;
						}
					} else {
						/*
							Found a "|xxx@" part. ignore all of it.
						*/
						from ++;
						while (*from != U'@' && *from != U'\0')
							from ++;
					}
				}
				Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
				*to = U'\0';
				p = from + ( *from == U'@' );   // add bool to pointer: skip '@' but not '\0'
				return p;
			} else if (p [1] == U'`' /*&& verbatimAware*/) {   // TODO: remove once manuals have been converted to notebooks
				/*
					We found "@`", starting a verbatim link in running text.
				*/
				const char32 *from = p + 1;
				*to ++ = *from ++;   // copy opening backquote
				while (*from != U'`' && *from != U'\0') {
					if (to - link >= MAXIMUM_LINK_LENGTH)
						Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “@`” is too long:\n", text);
					*to ++ = *from ++;
				}
				if (*from == U'`')
					*to ++ = *from ++;   // copy closing backquote
				Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
				*to = U'\0';
				p = from;
				return p;
			} else {
				const char32 *from = p + 1;
				while (isSingleWordCharacter (*from)) {
					if (to - link >= MAXIMUM_LINK_LENGTH)
						Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “@” is too long:\n", text);
					*to ++ = *from ++;
				}
				Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
				*to = U'\0';
				p = from;
				return p;
			}
		} else if (*p == U'`' && verbatimAware && ! paragraphIsVerbatim) {
			/*
				We found "`", starting a verbatim stretch in which '@' is to be ignored.
				Doesn't count if part of a backslash trigraph.
			*/
			if (p - text <= 0 || (p [-1] != U'\\' && (p - text <= 1 || p [-2] != U'\\'))) {
				/*
					Jump to the matching closing backquote.
				*/
				p ++;   // step over opening backquote
				for (;;) {
					if (*p == U'\0')
						return nullptr;   // no more '@'
					if (*p == U'`') {
						if (p [1] == U'`')
							p ++;   // jump over the first member of a double backquote
						else
							break;   // found the closing backquote
					}
					p ++;
				}
			}
		} else if (paragraphIsVerbatim &&
			(*p == U'\\' && p [1] == U'@' && p [2] == U'{' ||
			 *p == U'\\' && p [1] == U'#' && p [2] == U'@' && p [3] == U'{' ||
			 *p == U'\\' && p [1] == U'`' && p [2] == U'{' ||
			 *p == U'\\' && p [1] == U'#' && p [2] == U'`' && p [3] == U'{')
		) {
			/*
				We found "\@{" or "\#@{" or "\`{" or "\#`{",
				starting a link in verbatim text or in code.
			*/
			const bool boldLink = ( p [1] == U'#' );
			const char32 *from = p + 3 + boldLink;
			const bool verbatimLink = ( from [-2] == U'`' );
			const conststring32 startMessage =
				boldLink ? verbatimLink ? U"\\#`{" : U"\\#@{" : verbatimLink ? U"\\`{" : U"\\@{";
			if (verbatimLink) {
				if (to - link >= MAXIMUM_LINK_LENGTH)
					Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “", startMessage, U"” is too long:\n", text);
				*to ++ = U'`';
			}
			while (*from != U'}' && *from != U'|' && *from != U'\0') {
				if (to - link >= MAXIMUM_LINK_LENGTH)
					Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “", startMessage, U"” is too long:\n", text);
				*to ++ = *from ++;
			}
			/*
				Ignore the "|...}" part, unless it is "||...}"
			*/
			if (*from == U'|') {
				if (from [1] == U'|') {
					/*
						Found a "||xxx}" part. Append the xxx part.
					*/
					from += 2;   // skip "||"
					while (*from != U'}' && *from != U'\0') {
						if (*from == U'\0')
							break;
						if (to - link >= MAXIMUM_LINK_LENGTH)
							Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “", startMessage, U"” and containing “||” is too long:\n", text);
						*to ++ = *from ++;
					}
					Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
					*to = U'\0';
				} else {
					/*
						Found a "|xxx}" part. Ignore all of it.
					*/
					from ++;
					while (*from != U'}' && *from != U'\0')
						from ++;
				}
			}

			/*
				Replace final colon with three dots.
				For example, the code
					\@{Create Poisson process:}
				should yield a link to
					Create Poisson process...
			*/
			if (to - link > 0 && to [-1] == U':') {
				to --;
				for (integer idot = 1; idot <= 3; idot ++) {
					if (to - link >= MAXIMUM_LINK_LENGTH)
						Melder_throw (U"(ManPage_Paragraph_extractLink:) Link starting with “\\@{” is too long:\n", text);
					*to ++ = U'.';
				}
			}
			if (verbatimLink) {
				Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
				*to ++ = U'`';
			}
			Melder_assert (to - link <= MAXIMUM_LINK_LENGTH);
			*to = U'\0';

			p = from + ( *from == U'}' );   // add bool to pointer: skip '}' but not '\0'
			return p;
		}
	}
}

static void readOnePage (ManPages me, MelderReadText text);   // forward

static void resolveLinks (ManPages me, ManPage_Paragraph par, bool verbatimAware) {
	//if (par -> type == kManPage_type::SCRIPT)
	//	return;   // links would be invisible
	char32 linkBuffer [MAXIMUM_LINK_LENGTH + 1], fileNameBuffer [ManPages_FILENAME_BUFFER_SIZE];
	for (const char32 *plink = ManPage_Paragraph_extractLink (par, nullptr, linkBuffer, verbatimAware);
		 plink != nullptr;
		 plink = ManPage_Paragraph_extractLink (par, plink, linkBuffer, verbatimAware)
	) {
		/*
			Now, `linkBuffer` contains the link text, with spaces and all.
			Transform it into a file name.
		*/
		structMelderFile file2 { };
		if (linkBuffer [0] == U'\\' && linkBuffer [1] == U'F' && linkBuffer [2] == U'I') {
			/*
				A link to a sound file: see if it exists.
			*/
			MelderFolder_relativePathToFile (& my rootDirectory, linkBuffer + 3, & file2);
			if (! MelderFile_exists (& file2))
				Melder_warning (U"Cannot find sound file ", MelderFile_messageName (& file2), U".");
		} else if (linkBuffer [0] == U'\\' && linkBuffer [1] == U'S' && linkBuffer [2] == U'C') {
			/*
				A link to a script: see if it exists.
			*/
			char32 *p = linkBuffer + 3;
			if (*p == U'"') {
				char32 *q = fileNameBuffer;
				p ++;
				while (*p != U'"' && *p != U'\0')
					* q ++ = * p ++;
				*q = U'\0';
			} else {
				char32 *q = fileNameBuffer;
				while (*p != U' ' && *p != U'\0')
					* q ++ = * p ++;   // one word, up to the next space
				*q = U'\0';
			}
			MelderFolder_relativePathToFile (& my rootDirectory, fileNameBuffer, & file2);
			if (! MelderFile_exists (& file2))
				Melder_warning (U"Cannot find script ", MelderFile_messageName (& file2), U".");
			my executable = true;
		} else {
			/*
				A link to another page: follow it.
			*/
			try {
				integer extensionSize = 4;   // .man
				Melder_sprint (fileNameBuffer,ManPages_FILENAME_BUFFER_SIZE - extensionSize, linkBuffer);
				/*
					For the `.man` version, we replace every funny symbol with an underscore.
				*/
				for (char32 *q = fileNameBuffer; *q; q ++)
					if (! isAllowedFileNameCharacter (*q))
						*q = U'_';
				str32cat (fileNameBuffer, U".man");
				MelderFolder_getFile (& my rootDirectory, fileNameBuffer, & file2);
				if (MelderFile_exists (& file2)) {
					autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
					readOnePage (me, text2.get());
				} else {
					/*
						Second try: with upper case.
					*/
					linkBuffer [0] = Melder_toUpperCase (linkBuffer [0]);
					Melder_sprint (fileNameBuffer,ManPages_FILENAME_BUFFER_SIZE, linkBuffer, U".man");
					MelderFolder_getFile (& my rootDirectory, fileNameBuffer, & file2);
					if (MelderFile_exists (& file2)) {
						autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
						readOnePage (me, text2.get());
					} else {
						/*
							For the `.praatnb` version, we replace most funny symbols with underscores,
							but `#` with `-H`, `$` with `-S`, and `@` with `-C`.
						*/
						extensionSize = 8;   // .praatnb
						char32 *to = fileNameBuffer, *max = fileNameBuffer + ManPages_FILENAME_BUFFER_SIZE - (extensionSize + 1);
						for (char32 *from = & linkBuffer [0]; *from != U'\0'; from ++) {
							if (isAllowedFileNameCharacter (*from)) {
								if (to < max)
									*to ++ = *from;
							} else if (*from == U'#') {
								if (to < max)
									*to ++ = U'-';
								if (to < max)
									*to ++ = U'H';
							} else if (*from == U'$') {
								if (to < max)
									*to ++ = U'-';
								if (to < max)
									*to ++ = U'S';
							} else if (*from == U'@') {
								if (to < max)
									*to ++ = U'-';
								if (to < max)
									*to ++ = U'C';
							} else {
								if (to < max)
									*to ++ = U'_';
							}
						}
						*to = U'\0';
						str32cat (fileNameBuffer, U".praatnb");
						MelderFolder_getFile (& my rootDirectory, fileNameBuffer, & file2);
						if (MelderFile_exists (& file2)) {
							autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
							readOnePage (me, text2.get());
						} else {
							fileNameBuffer [0] = Melder_toLowerCase (fileNameBuffer [0]);
							MelderFolder_getFile (& my rootDirectory, fileNameBuffer, & file2);
							if (MelderFile_exists (& file2)) {
								autoMelderReadText text2 = MelderReadText_createFromFile (& file2);
								readOnePage (me, text2.get());
							}
						}
					}
				}
			} catch (MelderError) {
				Melder_throw (U"File ", & file2, U".");
			}
		}
	}
}

static void readOnePage_man (ManPages me, MelderReadText text) {
	autostring32 title;
	try {
		title = texgetw16 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find page title.");
	}

	/*
		Check whether a page with this title is already present.
	*/
	if (lookUp_unsorted (me, title.get()))
		return;

	autoManPage autopage = Thing_new (ManPage);
	autopage -> title = title.move();

	/*
		Add the page early, so that lookUp can find it.
	*/
	ManPage page = my pages. addItem_move (autopage.move());
	page -> verbatimAware = false;

	autostring32 author;
	try {
		author = texgetw16 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find author.");
	}
	uinteger date;
	try {
		date = texgetu32 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find date.");
	}
	page -> signature = Melder_dup (Melder_cat (U"© ", author.get(), U" ", date));
	try {
		page -> recordingTime = texgetr64 (text);
	} catch (MelderError) {
		Melder_throw (U"Cannot find recording time.");
	}

	for (;;) {
		enum kManPage_type type;
		try {
			type = (kManPage_type) texgete8 (text, (enum_generic_getValue) kManPage_type_getValue);
		} catch (MelderError) {
			if (Melder_hasError (U"end of text")) {
				Melder_clearError ();
				break;
			} else {
				throw;
			}
		}
		ManPage_Paragraph par = page -> paragraphs. append ();
		par -> type = type;
		if (par -> type == kManPage_type::SCRIPT) {
			par -> width = texgetr64 (text);
			par -> height = texgetr64 (text);
		}
		try {
			par -> text = texgetw16 (text).transfer();
			//if (str32str (par -> text, U":|"))
			//	par -> text = replace_STR (par -> text, U":|", U"...|", 0).transfer();
			//if (str32str (par -> text, U":@"))
			//	par -> text = replace_STR (par -> text, U":@", U"...@", 0).transfer();
		} catch (MelderError) {
			Melder_throw (U"Cannot find text.");
		}
		resolveLinks (me, par, page -> verbatimAware);
	}
}
static bool stringHasInk (conststring32 line) {
	const char32 *endOfSpace = Melder_findEndOfHorizontalSpace (line);
	return *endOfSpace != U'\0';
}
static bool isTerm (kManPage_type type) {
	return type == kManPage_type::TERM || type >= kManPage_type::TERM1 && type <= kManPage_type::TERM3;
}
static bool stringStartsWithParenthesizedNumber (conststring32 string) {
	const char32 *p = & string [0];
	if (*p != U'(')
		return false;
	p ++;   // skip opening parenthesis
	if (*p < U'0' || *p > U'9')
		return false;
	p ++;   // skip first digit
	while (*p >= U'0' && *p <= U'9')
		p ++;
	if (*p != U')')
		return false;
	return true;
}
static bool stringStartsWithNumberAndDot (conststring32 string) {
	const char32 *p = & string [0];
	for (;;) {
		if (*p < U'0' || *p > U'9')
			return false;
		p ++;   // skip first digit
		while (*p >= U'0' && *p <= U'9')
			p ++;
		if (*p != U'.')
			return false;
		p ++;
		if (Melder_isHorizontalSpace (*p))
			return true;
	}
}

static void readOnePage_notebook (ManPages me, MelderReadText text) {
	/*
		Handle the title line.
	*/
	mutablestring32 firstLine = MelderReadText_readLine (text);
	Melder_require (!! firstLine,
		U"Empty notebook.");
	Melder_require (*firstLine == U'"',
		U"There should be a page title (starting with an opening quote) on the first line, at or around “", firstLine, U"”.");
	++ firstLine;   // jump over opening quote
	char32 *p_closingQuote = str32rchr (firstLine, U'"');
	Melder_require (p_closingQuote - firstLine > 0,
		U"There should be a page title (starting with an closing quote) on the first line.");
	*p_closingQuote = U'\0';

	/*
		Check whether a page with this title is already present.
	*/
	if (lookUp_unsorted (me, firstLine))
		return;

	autoManPage autopage = Thing_new (ManPage);
	autopage -> title = Melder_dup (firstLine);

	/*
		Add the page early, so that lookUp can find it.
	*/
	ManPage page = my pages. addItem_move (autopage.move());
	page -> verbatimAware = true;

	/*
		Handle the signature line.
	*/
	mutablestring32 line = MelderReadText_readLine (text);
	Melder_require (!! line,
		U"Missing signature line (typically author and date) in page “", firstLine, U"”.");
	page -> signature = Melder_dup (line);

	/*
		Find all the notebook attributes.
		All of them are optional.
		The order is not fixed.
		We allow for new attributes in the future.
	*/
	for (;;) {
		line = MelderReadText_readLine (text);
		if (! line)
			break;   // end of file
		char32 *colon = str32chr (line, U':');
		if (! colon)
			break;   // end of header
		const integer lengthOfAttributeName = colon - line;
		if (Melder_nequ (line, U"Recording time", lengthOfAttributeName)) {
			Melder_skipHorizontalSpace (& (colon += 1));
			page -> recordingTime = Melder_atof (colon);
		}
		/*
			TODO: add:
				Use Praat windows: Objects, Picture(?), Info, Demo(?)
				Keywords:   ; lower case, separate by comma
				Code chunk visibility: +   ; + (the default) or -
				Text style language: praat   ; praat (the default), i.e. % # $ @ _ ^, or markdown, i.e. * ` ** [] <sub> <sup>
		*/
	}

	line = nullptr;
	do {
		line = MelderReadText_readLine (text);
	} while (line && ! stringHasInk (line));
	ManPage_Paragraph previousParagraph = nullptr;
	autoMelderString buffer_graphical, buffer_graphicalCode;
	for (;;) {
		if (! line)
			return;
		integer numberOfLeadingEmptyLines = 0;
		while (! stringHasInk (line)) {
			numberOfLeadingEmptyLines += 1;
			line = MelderReadText_readLine (text);
			if (! line)
				return;
		}
		integer numberOfLeadingSpaces = 0;
		while (Melder_isHorizontalSpace (*line))
			numberOfLeadingSpaces += ( *(line ++) == U'\t' ? 4 - ( numberOfLeadingSpaces & 0b11_integer ) : 1 );
		MelderString_empty (& buffer_graphical);
		kManPage_type type;
		double width = 0.0, height = 0.001;
		if (numberOfLeadingSpaces == 0 && Melder_startsWith (line, U"===")) {
			if (previousParagraph)
				previousParagraph -> type = kManPage_type::ENTRY;
			line = MelderReadText_readLine (text);
			if (! line)
				return;
			continue;
		} else if (numberOfLeadingSpaces == 0 && Melder_startsWith (line, U"---")) {
			if (previousParagraph)
				previousParagraph -> type = kManPage_type::SUBHEADER;
			line = MelderReadText_readLine (text);
			if (! line)
				return;
			continue;
		} else if (line [0] == U'/' && line [1] == U'/') {
			line = MelderReadText_readLine (text);
			if (! line)
				return;
			continue;
		/*
			Now we try several kinds of list items.
			To not prepend a character, use ",".
			To prepend a bullet, use "-" or "*" or "•".
		*/
		} else if (
			line [0] == U',' && (Melder_isHorizontalSpace (line [1]) || line [1] == U'\0') ||
			line [0] == U'-' && Melder_isHorizontalSpace (line [1]) ||
			line [0] == U'*' && Melder_isHorizontalSpace (line [1]) ||
			line [0] == U'•' && Melder_isHorizontalSpace (line [1]) ||
			stringStartsWithParenthesizedNumber (line) ||
			stringStartsWithNumberAndDot (line) ||
			line [0] == U'|' && Melder_isHorizontalSpace (line [1])
		) {
			type = (
				numberOfLeadingSpaces <  3 ? kManPage_type::LIST_ITEM :
				numberOfLeadingSpaces <  7 ? kManPage_type::LIST_ITEM1 :
				numberOfLeadingSpaces < 11 ? kManPage_type::LIST_ITEM2 :
				kManPage_type::LIST_ITEM3
			);
			if (line [0] == U',') {
				if (line [1] == U'\0') {
					MelderString_append (& buffer_graphical, U" ");   // a dummmy to make sure that the line is drawn at all
					line += 1;
				} else {
					MelderString_append (& buffer_graphical, U" ");
					line += 2;
					//Melder_skipHorizontalSpace (& line);
				}
				MelderString_append (& buffer_graphical, line);
			} else if (line [0] == U'-' || line [0] == U'*' || line [0] == U'•') {
				MelderString_append (& buffer_graphical, U"• ");
				line += 2;
				//Melder_skipHorizontalSpace (& line);
				MelderString_append (& buffer_graphical, line);
			} else if (line [0] == U'|') {
				MelderString_append (& buffer_graphical, U"\t");
				line += 2;
				while (*line != U'\0') {
					if (line [0] == U'|' && Melder_isHorizontalSpace (line [-1]) && (Melder_isHorizontalSpace (line [1]) || line [1] == U'\0'))
						MelderString_appendCharacter (& buffer_graphical, U'\t');
					else
						MelderString_appendCharacter (& buffer_graphical, *line);
					line ++;
				}
			} else {
				MelderString_append (& buffer_graphical, line);
			}
		} else if (line [0] == U':' && Melder_isHorizontalSpace (line [1])) {
			type = (
				numberOfLeadingSpaces <  3 ? kManPage_type::DEFINITION :
				numberOfLeadingSpaces <  7 ? kManPage_type::DEFINITION1 :
				numberOfLeadingSpaces < 11 ? kManPage_type::DEFINITION2 :
				kManPage_type::DEFINITION3
			);
			if (previousParagraph && (previousParagraph -> type == kManPage_type::NORMAL || previousParagraph -> type == kManPage_type::CAPTION)) {
				if (type == kManPage_type::DEFINITION)
					previousParagraph -> type = kManPage_type::TERM;
				else if (type == kManPage_type::DEFINITION1)
					previousParagraph -> type = kManPage_type::TERM1;
				else if (type == kManPage_type::DEFINITION2)
					previousParagraph -> type = kManPage_type::TERM2;
				else if (type == kManPage_type::DEFINITION3)
					previousParagraph -> type = kManPage_type::TERM3;
			}
			line += 2;
			Melder_skipHorizontalSpace (& line);
			MelderString_append (& buffer_graphical, line);
		} else if (line [0] == U'>' && Melder_isHorizontalSpace (line [1])) {
			type = (
				numberOfLeadingSpaces <  3 ? kManPage_type::QUOTE :
				numberOfLeadingSpaces <  7 ? kManPage_type::QUOTE1 :
				numberOfLeadingSpaces < 11 ? kManPage_type::QUOTE2 :
				kManPage_type::QUOTE3
			);
			line += 2;
			Melder_skipHorizontalSpace (& line);
			MelderString_append (& buffer_graphical, line);
		} else if (numberOfLeadingSpaces == 0 && line [0] == U'~' && Melder_isHorizontalSpace (line [1])) {
			type = kManPage_type::EQUATION;
			line += 2;
			Melder_skipHorizontalSpace (& line);
			MelderString_append (& buffer_graphical, line);
		} else if (numberOfLeadingSpaces == 0 && line [0] == U'`' && ! stringHasInk (line + 1)) {
			//TRACE
			trace (U"Verbatim: <<", page -> title.get(), U">>");
			type = kManPage_type::SCRIPT;   // TODO: make different type, such as kManPage_type::VERBATIM
			do {
				line = MelderReadText_readLine (text);
				if (! line)
					break;
				if (line [0] == U'`' && ! stringHasInk (line + 1)) {
					line = MelderReadText_readLine (text);
					break;
				}
				MelderString_empty (& buffer_graphicalCode);
				const char32 *p = & line [0];
				while (*p) {
					if (*p == U'\t') {
						MelderString_append (& buffer_graphicalCode, p == line ? nullptr : U"    ");
					} else
						MelderString_appendCharacter (& buffer_graphicalCode, *p);
					p ++;
				}
				ManPage_Paragraph par = page -> paragraphs. append ();
				par -> type = kManPage_type::CODE;
				par -> text = Melder_dup (buffer_graphicalCode.string).transfer();
			} while (1);
			MelderString_empty (& buffer_graphical);   // this makes sure that no actual SCRIPT (or VERBATIM) paragraph will be added
		} else if (numberOfLeadingSpaces == 0 && line [0] == U'{') {
			const bool shouldShowCode = ! str32chr (line, U'-');
			const char32 *sizeLocation = str32chr (line, U'x');
			const bool shouldShowOutput = ! str32chr (line, U';');
			if (sizeLocation) {
				const char32 *widthLocation = sizeLocation - 1;
				while (Melder_isDecimalNumber (*widthLocation) || *widthLocation == U'.')
					widthLocation --;
				width = Melder_atof (widthLocation);
				height = Melder_atof (sizeLocation + 1);
			} else
				width = 6.0;
			type = kManPage_type::SCRIPT;
			integer procedureDepth = 0;
			do {
				line = MelderReadText_readLine (text);
				if (! line)
					Melder_throw (U"Script chunk not closed.");
				const char32 *firstNonspace = Melder_findEndOfHorizontalSpace (line);
				if (line [0] == U'}') {
					line = MelderReadText_readLine (text);
					break;
				}
				if (Melder_startsWith (firstNonspace, U"procedure "))
					procedureDepth += 1;
				else if (Melder_startsWith (firstNonspace, U"endproc"))
					procedureDepth -= 1;
				if (shouldShowCode) {
					/*
						Convert to graphical code.
					*/
					MelderString_empty (& buffer_graphicalCode);
					const char32 *p = & line [0];
					while (*p) {
						if (*p == U'\t') {
							MelderString_append (& buffer_graphicalCode, p == line ? nullptr : U"    ");
						} else
							MelderString_appendCharacter (& buffer_graphicalCode, *p);
						p ++;
					}
					ManPage_Paragraph par = page -> paragraphs. append ();
					par -> type = kManPage_type::CODE;
					par -> text = Melder_dup (buffer_graphicalCode.string).transfer();
				}
				if (shouldShowOutput) {
					/*
						Collect the code, for later execution when the output is drawn.
					*/

					/*
						Look for drawing commmands. They could either stand on their own,
						or be within "\@{xxx}" or within "\#{xxx}" (note: not within "\`{xxx}").

						TODO: make less brittle.
					*/
					if (firstNonspace [0] == U'\\' &&
						(firstNonspace [1] == U'@' && firstNonspace [2] == U'{' ||
						 firstNonspace [1] == U'#' && firstNonspace [2] == U'{' ||
						 firstNonspace [1] == U'#' && firstNonspace [2] == U'@' && firstNonspace [3] == U'{')
					)
						firstNonspace += 3 + ( firstNonspace [2] == U'@' );
					if (
						procedureDepth == 0 &&
						height == 0.001 &&
						(Melder_startsWith (firstNonspace, U"Draw")  ||
						 Melder_startsWith (firstNonspace, U"Paint")  ||
						 Melder_startsWith (firstNonspace, U"Axes:")  ||
						 Melder_startsWith (firstNonspace, U"Axes}:")  ||
						 Melder_startsWith (firstNonspace, U"Text ")  ||
						 Melder_startsWith (firstNonspace, U"Text:")  ||
						 Melder_startsWith (firstNonspace, U"Text}:")  ||
						 Melder_startsWith (firstNonspace, U"Marks "))
					)
						height = 3.0;

					const char32 *p = & line [0];
					bool inBold = false, inItalic = false;
					while (*p) {
						if (*p == U'\\' &&
							(p [1] == U'@' && p [2] == U'{' ||
							 p [1] == U'`' && p [2] == U'{' ||
							 p [1] == U'#' && p [2] == U'@' && p [3] == U'{' ||
							 p [1] == U'#' && p [2] == U'`' && p [3] == U'{'))
						{
							const bool thinLink = ( p [1] != U'#' );
							p += 4 - thinLink;   // "\@{" should not be included in the code
							/*
								We collect the link text separately,
								because we cannot collect it into buffer_graphical directly,
								because in case of a "|" the link buffer has to be cleared.
							*/
							static MelderString linkText;
							MelderString_empty (& linkText);
							while (*p != U'\0') {
								if (*p == U'|') {
									MelderString_empty (& linkText);   // ignore link target as well as "|" and "||"
									if (p [1] == U'|')
										p += 1;
								} else if (*p == U'}') {
									break;   // but a missing closing brace at the end of a line is also fine
								} else {
									MelderString_appendCharacter (& linkText, *p);
								}
								p ++;
							}
							MelderString_append (& buffer_graphical, linkText.string);
							if (*p == U'\0')
								break;   // double break
						} else if (*p == U'\\' && p [1] == U'#' && p [2] == U'{') {
							inBold = true;
							p += 2;
						} else if (*p == U'\\' && p [1] == U'%' && p [2] == U'{') {
							inItalic = true;
							p += 2;
						} else if (*p == U'}') {
							if (inBold)
								inBold = false;
							else if (inItalic)
								inItalic = false;
							else
								MelderString_appendCharacter (& buffer_graphical, U'}');
						} else
							MelderString_appendCharacter (& buffer_graphical, *p);
						p ++;
					}
					MelderString_appendCharacter (& buffer_graphical, U'\n');
				}
			} while (1);
			if (! shouldShowOutput)
				MelderString_empty (& buffer_graphical);   // add no SCRIPT paragraph
		} else if (numberOfLeadingSpaces >= 3) {
			type = kManPage_type::CAPTION;
			MelderString_append (& buffer_graphical, line);
		} else {
			type = kManPage_type::NORMAL;
			MelderString_append (& buffer_graphical, line);
		}
		ManPage_Paragraph par = nullptr;
		if (buffer_graphical.string [0] != U'\0') {
			par = page -> paragraphs. append ();
			par -> type = type;
			par -> text = Melder_dup (buffer_graphical.string). transfer();
			par -> width = width;
			par -> height = height;
		}

		/*
			Do continuation lines, except for code.
		*/
		const bool isCode = ( type == kManPage_type::CODE || type >= kManPage_type::CODE1 && type <= kManPage_type::CODE5 );
		if (isCode) {
			line = MelderReadText_readLine (text);
		} else if (type == kManPage_type::SCRIPT) {
		} else if (par) {
			do {
				mutablestring32 continuationLine = MelderReadText_readLine (text);
				if (! continuationLine) {
					line = nullptr;   // signals end of text
					break;
				}
				integer nextNumberOfLeadingSpaces = 0, ikar = 0;
				while (Melder_isHorizontalSpace (continuationLine [ikar])) {
					nextNumberOfLeadingSpaces += ( continuationLine [ikar] == U'\t' ? 4 - ( nextNumberOfLeadingSpaces & 0b11_integer ) : 1 );
					ikar ++;
				}
				char32 *firstNonSpace = continuationLine + ikar;
				if (*firstNonSpace == U':' ||
					*firstNonSpace == U'>' ||
					*firstNonSpace == U',' && (Melder_isHorizontalSpace (firstNonSpace [1]) || firstNonSpace [1] == U'\0') ||
					*firstNonSpace == U'-' && Melder_isHorizontalSpace (firstNonSpace [1]) ||
					*firstNonSpace == U'*' && Melder_isHorizontalSpace (firstNonSpace [1]) ||
					*firstNonSpace == U'•' && Melder_isHorizontalSpace (firstNonSpace [1]) ||
					stringStartsWithParenthesizedNumber (firstNonSpace) ||
					stringStartsWithNumberAndDot (firstNonSpace) ||
					*firstNonSpace == U'|' && Melder_isHorizontalSpace (firstNonSpace [1]) ||
					firstNonSpace == continuationLine && *firstNonSpace == U'{' ||
					firstNonSpace == continuationLine && *firstNonSpace == U'`' && ! stringHasInk (firstNonSpace + 1) ||
					firstNonSpace == continuationLine && *firstNonSpace == U'~' ||
					Melder_startsWith (firstNonSpace, U"===") ||
					Melder_startsWith (firstNonSpace, U"---") ||
					*firstNonSpace == U'/' && firstNonSpace [1] == U'/' ||
					*firstNonSpace == U'\0'
				) {
					line = continuationLine;   // not really a continuation line, but a new paragraph
					break;
				}
				conststring32 separator = ( par -> text [0] == U'\0' ? U"" : U" " );
				Melder_skipHorizontalSpace (& continuationLine);
				par -> text = Melder_dup (Melder_cat (par -> text, separator, continuationLine)).transfer();
			} while (1);
		}
		if (par) {
			if (my dynamic)
				resolveLinks (me, par, page -> verbatimAware);
			previousParagraph = par;
		}
	}
	trace (U"end");
}
static void readOnePage (ManPages me, MelderReadText text) {
	const bool isNotebook = (
		text -> string32 ?
			text -> string32 [0] == U'"'
		:
			text -> string8 [0] == '"'
	);
	if (isNotebook)
		readOnePage_notebook (me, text);
	else
		readOnePage_man (me, text);
}
void structManPages :: v1_readText (MelderReadText text, int /*formatVersion*/) {
	our dynamic = true;
	MelderFolder_copy (& Data_directoryBeingRead, & our rootDirectory);
	readOnePage (this, text);
}

autoManPages ManPages_create () {
	autoManPages me = Thing_new (ManPages);
	return me;
}

autoManPages ManPages_createFromText (MelderReadText text, MelderFile file) {
	autoManPages me = Thing_new (ManPages);
	my dynamic = true;
	MelderFile_getParentFolder (file, & my rootDirectory);
	readOnePage (me.get(), text);
	return me;
}

/*
	Praat-internal version without references to external files.
*/
autoManPages ManPages_createFromText (MelderReadText text) {
	autoManPages me = Thing_new (ManPages);
	my dynamic = false;
	readOnePage (me.get(), text);
	return me;
}

void ManPages_addPage (ManPages me, conststring32 title, conststring32 signature,
	structManPage_Paragraph paragraphs [])
{
	autoManPage page = Thing_new (ManPage);
	page -> title = Melder_dup (title);
	for (ManPage_Paragraph par = & paragraphs [0]; (int) par -> type != 0; par ++) {
		ManPage_Paragraph targetParagraph = page -> paragraphs. append ();
		targetParagraph -> type = par -> type;
		targetParagraph -> text = par -> text;   // static string
		targetParagraph -> width = par -> width;
		targetParagraph -> height = par -> height;
		targetParagraph -> draw = par -> draw;
	}
	page -> signature = Melder_dup (signature);
	my pages. addItem_move (page.move());
}
void ManPages_addPagesFromNotebookText (ManPages me, conststring8 multiplePagesText) {
	autoMelderReadText multiplePagesReader = MelderReadText_createFromText (Melder_8to32_e (multiplePagesText));
	autoMelderString pageText;
	for (;;) {
		mutablestring32 line = MelderReadText_readLine (multiplePagesReader.get());
		const bool atEndOfPage = ( ! line || Melder_startsWith (line, U"####################") );
		if (atEndOfPage) {
			if (pageText.length > 0) {
				autoMelderReadText pageReader = MelderReadText_createFromText (Melder_dup (pageText.string));
				readOnePage_notebook (me, pageReader.get());
			}
			if (! line)
				return;
			MelderString_empty (& pageText);
		} else
			MelderString_append (& pageText, line, U"\n");
	}
}
integer ManPages_addPagesFromNotebookReader (ManPages me, MelderReadText multiplePagesReader, integer startOfSelection, integer endOfSelection) {
	bool foundFirstPage = false;
	integer numberOfCharactersRead = 0;
	integer startingPage = -1, numberOfPages = 0;
	autoMelderString pageText;
	for (;;) {
		mutablestring32 line = MelderReadText_readLine (multiplePagesReader);
		if (startingPage == -1) {
			numberOfCharactersRead += Melder_length (line) + 1;
			if (numberOfCharactersRead >= startOfSelection)
				startingPage = numberOfPages;
		}
		const bool atEndOfPage = ( ! line || Melder_startsWith (line, U"####################") );
		if (atEndOfPage) {
			if (! foundFirstPage) {
				if (! line)
					Melder_throw (U"Empty notebook (no line starting with “####################”.");
				/*
					When we are here, the line starts with "####################".
				*/
				foundFirstPage = true;
			}
			if (pageText.length > 0) {
				autoMelderReadText pageReader = MelderReadText_createFromText (Melder_dup (pageText.string));
				readOnePage_notebook (me, pageReader.get());
			}
			if (! line)
				break;
			MelderString_empty (& pageText);
			numberOfPages += 1;
		} else if (Melder_startsWith (line, U")~~~\"")) {
			break;
		} else if (foundFirstPage) {
			MelderString_append (& pageText, line, U"\n");
		} else {
			// ignore all the lines before the first line that starts with "####################".
		}
	}
	return startingPage;
}

static bool pageCompare (ManPage me, ManPage thee) {
	return str32coll_numberAware (my title.get(), thy title.get(), true) < 0;
}

static integer lookUp_unsorted (ManPages me, conststring32 title) {
	/*
		First try to match an unaltered 'title' with the titles of the man pages.
	*/
	for (integer i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title.get(), title))
			return i;
	}
	/*
		If that fails, try to find the upper-case variant.
	*/
	if (Melder_isLowerCaseLetter (title [0])) {
		char32 upperTitle [300];
		Melder_sprint (upperTitle,300, title);
		upperTitle [0] = Melder_toUpperCase (upperTitle [0]);
		for (integer i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			if (str32equ (page -> title.get(), upperTitle))
				return i;
		}
	}
	return 0;
}

static integer lookUp_sorted (ManPages me, conststring32 title) {
	static autoManPage dummy;
	if (! dummy)
		dummy = Thing_new (ManPage);
	dummy -> title = Melder_dup (title);
	ManPage *page = std::lower_bound (my pages.begin(), my pages.end(), dummy.get(), pageCompare);   // noexcept
	if (page != my pages.end() && Melder_equ ((*page) -> title.get(), dummy -> title.get()))
		return (page - my pages.begin()) + 1;
	if (Melder_isLowerCaseLetter (title [0]) || Melder_isUpperCaseLetter (title [0])) {
		char32 caseSwitchedTitle [300];
		Melder_sprint (caseSwitchedTitle,300, title);
		caseSwitchedTitle [0] = Melder_isLowerCaseLetter (title [0]) ? Melder_toUpperCase (caseSwitchedTitle [0]) : Melder_toLowerCase (caseSwitchedTitle [0]);
		dummy -> title = Melder_dup (caseSwitchedTitle);
		page = std::lower_bound (my pages.begin(), my pages.end(), dummy.get(), pageCompare);   // noexcept
		if (page != my pages.end() && Melder_equ ((*page) -> title.get(), dummy -> title.get()))
			return (page - my pages.begin()) + 1;
	}
	return 0;
}

static void grind (ManPages me) {
	std::sort (my pages.begin(), my pages.end(), pageCompare);
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		page -> linksHither = zero_INTVEC (0);   // superfluous if not ground twice
		page -> linksThither = zero_INTVEC (0);   // superfluous if not ground twice
	}
	integer ndangle = 0;
	for (integer ipage = 1; ipage <= my pages.size; ipage ++) {
		ManPage page = my pages.at [ipage];
		const bool verbatimAware = page -> verbatimAware;
		for (int ipar = 1; ipar <= page -> paragraphs.size; ipar ++) {
			ManPage_Paragraph par = & page -> paragraphs [ipar];
			const char32 *p;
			char32 linkBuffer [MAXIMUM_LINK_LENGTH + 1];
			if (par -> text) for (p = ManPage_Paragraph_extractLink (par, nullptr, linkBuffer, verbatimAware);
				 p != nullptr;
				 p = ManPage_Paragraph_extractLink (par, p, linkBuffer, verbatimAware)
			) {
				if (linkBuffer [0] == U'\\' && ((linkBuffer [1] == U'F' && linkBuffer [2] == U'I') || (linkBuffer [1] == U'S' && linkBuffer [2] == U'C')))
					continue;   // ignore "FILE" and "SCRIPT" links
				const integer jpage = lookUp_sorted (me, linkBuffer);
				if (jpage == 0) {
					MelderInfo_writeLine (U"Page “", page -> title.get(), U"” contains a dangling link to “", linkBuffer, U"”.");
					ndangle ++;
				} else {
					bool alreadyPresent = false;
					for (int ilink = 1; ilink <= page -> linksThither.size; ilink ++) {
						if (page -> linksThither [ilink] == jpage) {
							alreadyPresent = true;
							break;
						}
					}
					if (! alreadyPresent) {
						ManPage otherPage = my pages.at [jpage];
						* page -> linksThither. append () = jpage;
						* otherPage -> linksHither. append () = ipage;
					}
				}
			}
		}
	}
	if (ndangle) {
		MelderInfo_close ();
		Melder_warning (U"(ManPages::grind:) ", ndangle, U" dangling links encountered. See console window.");
	}
	my ground = true;
}

integer ManPages_uniqueLinksHither (ManPages me, integer ipage) {
	ManPage page = my pages.at [ipage];
	integer result = page -> linksHither.size;
	for (integer ilinkHither = 1; ilinkHither <= page -> linksHither.size; ilinkHither ++) {
		integer link = page -> linksHither [ilinkHither];
		for (integer ilinkThither = 1; ilinkThither <= page -> linksThither.size; ilinkThither ++) {
			if (page -> linksThither [ilinkThither] == link) {
				result --;
				break;
			}
		}
	}
	return result;
}

integer ManPages_lookUp (ManPages me, conststring32 title) {
	if (! my ground) {
		//Melder_stopwatch ();
		grind (me);
		//Melder_information (U"grinding lasted ", Melder_stopwatch (), U" seconds.");
	}
	return lookUp_sorted (me, title);
}

integer ManPages_lookUp_caseSensitive (ManPages me, conststring32 title) {
	if (! my ground)
		grind (me);
	for (integer i = 1; i <= my pages.size; i ++) {
		ManPage page = my pages.at [i];
		if (str32equ (page -> title.get(), title))
			return i;
	}
	return 0;
}

constSTRVEC ManPages_getTitles (ManPages me) {
	if (! my ground)
		grind (me);
	if (! my titles) {
		my titles = autoSTRVEC (my pages.size);
		for (integer i = 1; i <= my pages.size; i ++) {
			ManPage page = my pages.at [i];
			my titles [i] = Melder_dup (page -> title.get());
		}
	}
	return my titles.get();
}

/* End of file ManPages.cpp */
