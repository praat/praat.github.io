/* Graphics_record.cpp
 *
 * Copyright (C) 1992-2005,2007-2020,2023,2025,2026 Paul Boersma
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

#include "GraphicsP.h"

#define RECORDING_HEADER_LENGTH 2

double * _Graphics_check (Graphics me, integer number) {
	Melder_assert (number >= 0);
	static bool messageHasAlreadyBeenShownOnce = false;
	double *result = nullptr;
	double *record = my record;
	integer nrecord = my nrecord;
	if (nrecord == 0) {
		nrecord = 1000;
		try {
			record = Melder_malloc (double, 1 + nrecord);
		} catch (MelderError) {
			if (messageHasAlreadyBeenShownOnce) {
				Melder_clearError ();
			} else {
				messageHasAlreadyBeenShownOnce = true;
				Melder_flushError (U"_Graphics_growRecorder: out of memory.\n"
					U"This message will not show up on future occasions.");   // because of loop danger when redrawing
			}
			return nullptr;
		}
		my record = record;
		my nrecord = nrecord;
	}
	if (nrecord < my irecord + RECORDING_HEADER_LENGTH + number) {
		while (nrecord < my irecord + RECORDING_HEADER_LENGTH + number)
			nrecord *= 2;
		try {
			record = (double *) Melder_realloc (record, (1 + nrecord) * (integer) sizeof (double));
		} catch (MelderError) {
			if (messageHasAlreadyBeenShownOnce) {
				Melder_clearError ();
			} else {
				messageHasAlreadyBeenShownOnce = true;
				Melder_flushError (U"_Graphics_growRecorder: out of memory.\n"
					U"This message will not show up on future occasions.");   // because of loop danger when redrawing
			}
			return nullptr;
		}
		my record = record;
		my nrecord = nrecord;
	}
	result = my record + my irecord;
	my irecord += number + RECORDING_HEADER_LENGTH;
	return result;
}

/***** RECORD AND PLAY *****/

bool Graphics_startRecording (Graphics me) {
	const bool wasRecording = my recording;
	my recording = true;
	return wasRecording;
}

bool Graphics_stopRecording (Graphics me) {
	const bool wasRecording = my recording;
	my recording = false;
	return wasRecording;
}

void Graphics_clearRecording (Graphics me) {
	if (my record) {
		Melder_free (my record);
		my irecord = 0;
		my nrecord = 0;
	}
}

void Graphics_play (Graphics me, Graphics thee) {
	const double *p = my record;
	const double *const endp = p + my irecord;
	const bool wasRecording = my recording;
	if (! p)
		return;
	my recording = false;   // temporarily, in case me == thee
	while (p < endp) {
		#define get  (* ++ p)
		#define iget  (integer) (* ++ p)
		#define mget(n)  (p += n, p - n)
		#define sget(n)  ((char *) (p += n, p - n + 1))
		int opcode = (int) get;
		(void) (integer) get;   // ignore number of arguments
		switch (opcode) {
			case SET_VIEWPORT: {
				const double x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setViewport (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			} break;
			case SET_INNER: {
				Graphics_setInner (thee);
			} break;
			case UNSET_INNER: {
				Graphics_unsetInner (thee);
			} break;
			case SET_WINDOW: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_setWindow (thee, x1, x2, y1, y2);
			} break;
			case TEXT: {
				const double x = get, y = get;
				const integer length = iget;
				const conststring8 text_utf8 = sget (length);
				Graphics_text (thee, x, y, Melder_peek8to32_u (text_utf8));
			} break;
			case POLYLINE: {
				const integer n = iget;
				const double *x = mget (n), *y = mget (n);
				Graphics_polyline (thee, n, & x [1], & y [1]);
			} break;
			case LINE: {
				const double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_line (thee, x1, y1, x2, y2);
			} break;
			case ARROW: {
				const double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_arrow (thee, x1, y1, x2, y2);
			} break;
			case FILL_AREA: {
				const integer n = iget;
				const double *const x = mget (n), *const y = mget (n);
				Graphics_fillArea (thee, n, & x [1], & y [1]);
			} break;
			case FUNCTION: {
				integer n = iget;
				const double x1 = get, x2 = get, *y = mget (n);
				Graphics_function (thee, y, 1, n, x1, x2);
			} break;
			case RECTANGLE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_rectangle (thee, x1, x2, y1, y2);
			} break;
			case FILL_RECTANGLE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillRectangle (thee, x1, x2, y1, y2);
			} break;
			case CIRCLE: {
				const double x = get, y = get, r = get;
				Graphics_circle (thee, x, y, r);
			} break;
			case FILL_CIRCLE: {
				const double x = get, y = get, r = get;
				Graphics_fillCircle (thee, x, y, r);
			} break;
			case ARC: {
				const double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_arc (thee, x, y, r, fromAngle, toAngle);
			} break;
			case ARC_ARROW: {
				const double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				const int arrowAtStart = (int) iget, arrowAtEnd = (int) iget;
				Graphics_arcArrow (thee, x, y, r, fromAngle, toAngle, arrowAtStart, arrowAtEnd);
			} break;
			case HIGHLIGHT: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_highlight (thee, x1, x2, y1, y2);
			} break;
			case CELL_ARRAY: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				const integer nrow = iget, ncol = iget;
				/*
					We don't copy all the data into a new matrix.
					Instead, we create row pointers z [1..nrow] that point directly into the recorded data.
					This works because the data is a packed array of double, just as Graphics_cellArray expects.
				*/
				#if 0
				autoMAT z = raw_MAT (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_cellArray (thee, z.all(), x1, x2, y1, y2, minimum, maximum);
				#else
				Graphics_cellArray (thee, constMATVU (p + 1, nrow, ncol, ncol, 1), x1, x2, y1, y2, minimum, maximum);
				p += nrow * ncol;
				#endif
			}  break;
			case SET_FONT: {
				Graphics_setFont (thee, (enum kGraphics_font) get);
			} break;
			case SET_FONT_SIZE: {
				Graphics_setFontSize (thee, get);
			} break;
			case SET_FONT_STYLE: {
				Graphics_setFontStyle (thee, (int) get);
			} break;
			case SET_TEXT_ALIGNMENT: {
				kGraphics_horizontalAlignment hor = (kGraphics_horizontalAlignment) iget;
				const int vert = (int) iget;
				Graphics_setTextAlignment (thee, hor, vert);
			}  break;
			case SET_TEXT_ROTATION: {
				Graphics_setTextRotation (thee, get);
			} break;
			case SET_LINE_TYPE: {
				Graphics_setLineType (thee, (int) get);
			} break;
			case SET_LINE_WIDTH: {
				Graphics_setLineWidth (thee, get);
			} break;
			case SET_STANDARD_COLOUR: {   // only used in old Praat picture files
				const int standardColour = (int) get;
				MelderColour colour =
					standardColour == 0 ? Melder_BLACK :
					standardColour == 1 ? Melder_WHITE :
					standardColour == 2 ? Melder_RED :
					standardColour == 3 ? Melder_GREEN :
					standardColour == 4 ? Melder_BLUE :
					standardColour == 5 ? Melder_CYAN :
					standardColour == 6 ? Melder_MAGENTA :
					standardColour == 7 ? Melder_YELLOW :
					standardColour == 8 ? Melder_MAROON :
					standardColour == 9 ? Melder_LIME :
					standardColour == 10 ? Melder_NAVY :
					standardColour == 11 ? Melder_TEAL :
					standardColour == 12 ? Melder_PURPLE :
					standardColour == 13 ? Melder_OLIVE :
					standardColour == 14 ? Melder_PINK :
					standardColour == 15 ? Melder_SILVER :
					Melder_GREY;
				Graphics_setColour (thee, colour);
			} break;
			case SET_GREY: {
				Graphics_setGrey (thee, get);
			} break;
			case MARK_GROUP: {
				Graphics_markGroup (thee);
			} break;
			case ELLIPSE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_ellipse (thee, x1, x2, y1, y2);
			} break;
			case FILL_ELLIPSE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_fillEllipse (thee, x1, x2, y1, y2);
			} break;
			case CIRCLE_MM: {
				const double x = get, y = get, d = get;
				Graphics_circle_mm (thee, x, y, d);
			} break;
			case FILL_CIRCLE_MM: {
				const double x = get, y = get, d = get;
				Graphics_fillCircle_mm (thee, x, y, d);
			} break;
			case IMAGE8: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				const uint8 minimum = (uint8) iget, maximum = (uint8) iget;
				const integer nrow = iget, ncol = iget;
				automatrix <uint8> z = newmatrixzero <uint8> (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = (uint8) iget;
				Graphics_image8 (thee, z.all(), x1, x2, y1, y2, minimum, maximum);
			} break;
			case UNHIGHLIGHT: {
				(void) mget (4);   // obsolete x1, x2, y1, y2
				// do nothing (this has become obsolete since the demise of XOR mode drawing)
			} break;
			case XOR_ON: {
				MelderColour colour; colour. red = get, colour. green = get, colour. blue = get;
				Graphics_xorOn (thee, colour);
			} break;
			case XOR_OFF: {
				Graphics_xorOff (thee);
			} break;
			case RECTANGLE_MM: {
				const double x = get, y = get, horSide = get, vertSide = get;
				Graphics_rectangle_mm (thee, x, y, horSide, vertSide);
			} break;
			case FILL_RECTANGLE_MM: {
				const double x = get, y = get, horSide = get, vertSide = get;
				Graphics_fillRectangle_mm (thee, x, y, horSide, vertSide);
			} break;
			case SET_WS_WINDOW: {
				const double x1NDC = get, x2NDC = get, y1NDC = get, y2NDC = get;
				Graphics_setWsWindow (thee, x1NDC, x2NDC, y1NDC, y2NDC);
			} break;
			case SET_WRAP_WIDTH: {
				Graphics_setWrapWidth (thee, get);
			} break;
			case SET_SECOND_INDENT: {
				Graphics_setSecondIndent (thee, get);
			} break;
			case SET_PERCENT_SIGN_IS_ITALIC: {
				Graphics_setPercentSignIsItalic (thee, (bool) get);
			} break;
			case SET_NUMBER_SIGN_IS_BOLD: {
				Graphics_setNumberSignIsBold (thee, (bool) get);
			} break;
			case SET_CIRCUMFLEX_IS_SUPERSCRIPT: {
				Graphics_setCircumflexIsSuperscript (thee, (bool) get);
			} break;
			case SET_UNDERSCORE_IS_SUBSCRIPT: {
				Graphics_setUnderscoreIsSubscript (thee, (bool) get);
			} break;
			case SET_DOLLAR_SIGN_IS_CODE: {
				Graphics_setDollarSignIsCode (thee, (bool) get);
			} break;
			case SET_AT_SIGN_IS_LINK: {
				Graphics_setAtSignIsLink (thee, (bool) get);
			} break;
			case BUTTON: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_button (thee, x1, x2, y1, y2);
			} break;
			case ROUNDED_RECTANGLE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_roundedRectangle (thee, x1, x2, y1, y2, r);
			} break;
			case FILL_ROUNDED_RECTANGLE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, r = get;
				Graphics_fillRoundedRectangle (thee, x1, x2, y1, y2, r);
			} break;
			case FILL_ARC: {
				const double x = get, y = get, r = get, fromAngle = get, toAngle = get;
				Graphics_fillArc (thee, x, y, r, fromAngle, toAngle);
			} break;
			case INNER_RECTANGLE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				Graphics_innerRectangle (thee, x1, x2, y1, y2);
			} break;
			case CELL_ARRAY8: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				const uint8 minimum = (uint8) iget, maximum = (uint8) iget;
				const integer nrow = iget, ncol = iget;
				automatrix <uint8> z = newmatrixzero <uint8> (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = (uint8) iget;
				Graphics_cellArray8 (thee, z.all(), x1, x2, y1, y2, minimum, maximum);
			}  break;
			case IMAGE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				const integer nrow = iget, ncol = iget;
				autoMAT z = raw_MAT (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++)
						z [irow] [icol] = get;
				Graphics_image (thee, z.all(), x1, x2, y1, y2, minimum, maximum);   // or with constMATVU construction
			}  break;
			case HIGHLIGHT2: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, innerX1 = get, innerX2 = get, innerY1 = get, innerY2 = get;
				Graphics_highlight2 (thee, x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2);
			}  break;
			case UNHIGHLIGHT2: {
				(void) mget (8);   // obsolete x1, x2, y1, y2, innerX1, innerX2, innerY1, innerY2
				// do nothing (this has become obsolete since the demise of XOR mode drawing)
			}  break;
			case SET_ARROW_SIZE: {
				Graphics_setArrowSize (thee, get);
			} break;
			case DOUBLE_ARROW: {
				const double x1 = get, y1 = get, x2 = get, y2 = get;
				Graphics_doubleArrow (thee, x1, y1, x2, y2);
			}  break;
			case SET_RGB_COLOUR: {
				MelderColour colour;
				colour. red = get, colour. green = get, colour. blue = get;
				Graphics_setColour (thee, colour);
			} break;
			case IMAGE_FROM_FILE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				const integer length = iget;
				const conststring8 text_utf8 = sget (length);
				Graphics_imageFromFile (thee, Melder_peek8to32_u (text_utf8), x1, x2, y1, y2);
			}  break;
			case POLYLINE_CLOSED: {
				const integer n = iget;
				const double *x = mget (n), *y = mget (n);
				Graphics_polyline_closed (thee, n, & x [1], & y [1]);
			} break;
			case CELL_ARRAY_COLOUR: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				const integer nrow = iget, ncol = iget;
				automatrix <MelderColour> z = newmatrixzero <MelderColour> (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++) {
						z [irow] [icol]. red = get;
						z [irow] [icol]. green = get;
						z [irow] [icol]. blue = get;
						z [irow] [icol]. transparency = get;
					}
				Graphics_cellArray_colour (thee, z.all(), x1, x2, y1, y2, minimum, maximum);
			}  break;
			case IMAGE_COLOUR: {
				const double x1 = get, x2 = get, y1 = get, y2 = get, minimum = get, maximum = get;
				const integer nrow = iget, ncol = iget;
				automatrix <MelderColour> z = newmatrixzero <MelderColour> (nrow, ncol);
				for (integer irow = 1; irow <= nrow; irow ++)
					for (integer icol = 1; icol <= ncol; icol ++) {
						z [irow] [icol]. red = get;
						z [irow] [icol]. green = get;
						z [irow] [icol]. blue = get;
						z [irow] [icol]. transparency = get;
					}
				Graphics_image_colour (thee, z.all(), x1, x2, y1, y2, minimum, maximum);
			}  break;
			case SET_COLOUR_SCALE: {
				Graphics_setColourScale (thee, (enum kGraphics_colourScale) get);
			} break;
			case SET_SPECKLE_SIZE: {
				Graphics_setSpeckleSize (thee, get);
			} break;
			case SPECKLE: {
				const double x = get, y = get;
				Graphics_speckle (thee, x, y);
			}  break;
			case CLEAR_WS: {
				Graphics_clearWs (thee);
			} break;
			case SET_BACKQUOTE_IS_VERBATIM: {
				Graphics_setBackquoteIsVerbatim (thee, (bool) get);
			} break;
			case RECTANGLE_TEXT_WRAP_AND_TRUNCATE: {
				const double x1 = get, x2 = get, y1 = get, y2 = get;
				const integer length = iget;
				const conststring8 text_utf8 = sget (length);
				Graphics_rectangleText_wrapAndTruncate (thee, x1, x2, y1, y2, Melder_peek8to32_u (text_utf8));
			} break;
			case RECTANGLE_TEXT_MAXIMAL_FIT: {
				const double x1 = get, x2 = get, minimumHorizontalMargin_in_textHeights = get, minimumHorizontalMargin_mm = get;
				const double y1 = get, y2 = get, minimumVerticalMargin_in_textHeights = get, minimumVerticalMargin_mm = get;
				const integer length = iget;
				const conststring8 text_utf8 = sget (length);
				Graphics_rectangleText_maximalFit (thee, x1, x2, minimumHorizontalMargin_in_textHeights, minimumHorizontalMargin_mm,
						y1, y2, minimumVerticalMargin_in_textHeights, minimumVerticalMargin_mm, Melder_peek8to32_u (text_utf8));
			} break;
			default:
				my recording = wasRecording;
				Melder_flushError (U"Graphics_play: unknown opcode (", opcode, U").\n", p [-1], U" ", p [1]);
				return;
		}
	}
	my recording = wasRecording;
}

void Graphics_writeRecordings (Graphics me, FILE *f) {
	const double * p = my record;
	if (! p)
		return;
	const double *const endp = p + my irecord;
	if (my irecord > INT32_MAX)
		Melder_throw (U"Graphics recordings too large to save (", my irecord, U" elements).");
	binputi32 (integer_to_int32_a (my irecord), f);   // guarded conversion
	while (p < endp) {
		#define get  (* ++ p)
		const int opcode = (int) get;
		binputr32 ((float) opcode, f);
		integer numberOfArguments = (integer) get;
		const integer largestIntegerRepresentableAs32BitFloat = 0x00FFFFFF;
		if (numberOfArguments > largestIntegerRepresentableAs32BitFloat) {
			binputr32 (-1.0, f);
			if (numberOfArguments > INT32_MAX)
				Melder_throw (U"Graphics element too large to save (", numberOfArguments, U" arguments).");
			binputi32 (integer_to_int32_a (numberOfArguments), f);   // guarded conversion
		} else {
			binputr32 ((float) numberOfArguments, f);
		}
		if (opcode == TEXT) {
			binputr32 (get, f);   // x
			binputr32 (get, f);   // y
			binputr32 (get, f);   // length
			Melder_assert (sizeof (double) == 8);
			if (uinteger_to_integer_a (fwrite (++ p, 8, integer_to_uinteger_a (numberOfArguments - 3), f)) < numberOfArguments - 3)   // text
				Melder_throw (U"Error writing graphics recordings.");
			p += numberOfArguments - 4;
		} else if (opcode == IMAGE_FROM_FILE) {
			binputr32 (get, f);   // x1
			binputr32 (get, f);   // x2
			binputr32 (get, f);   // y1
			binputr32 (get, f);   // y2
			binputr32 (get, f);   // length
			Melder_assert (sizeof (double) == 8);
			if (uinteger_to_integer_a (fwrite (++ p, 8, integer_to_uinteger_a (numberOfArguments - 5), f)) < numberOfArguments - 5)   // text
				Melder_throw (U"Error writing graphics recordings.");
			p += numberOfArguments - 6;
		} else {
			for (integer i = numberOfArguments; i > 0; i --)
				binputr32 (get, f);
		}
	}
}

void Graphics_readRecordings (Graphics me, FILE *f, double heightCorrection) {
	integer old_irecord = my irecord;
	integer added_irecord = 0;
	double* p = nullptr;
	double* endp = nullptr;
	integer numberOfArguments = 0;
	int opcode = 0;   // large scope on behalf of message
	try {
		added_irecord = bingeti32 (f);
		p = _Graphics_check (me, added_irecord - RECORDING_HEADER_LENGTH);
		if (! p)
			return;
		Melder_assert (my irecord == old_irecord + added_irecord);
		endp = p + added_irecord;
		while (p < endp) {
			opcode = (int) bingetr32 (f);
			put (opcode);
			numberOfArguments = (integer) bingetr32 (f);
			if (numberOfArguments == -1)
				numberOfArguments = bingeti32 (f);
			put (numberOfArguments);
			if (opcode == TEXT) {
				put (bingetr32 (f));   // x
				put (bingetr32 (f));   // y
				put (bingetr32 (f));   // length
				if (uinteger_to_integer_a (fread (++ p, 8, integer_to_uinteger_a (numberOfArguments - 3), f)) < numberOfArguments - 3)   // text
					Melder_throw (U"Error reading graphics recordings.");
				p += numberOfArguments - 4;
			} else if (opcode == IMAGE_FROM_FILE) {
				put (bingetr32 (f));   // x1
				put (bingetr32 (f));   // x2
				put (bingetr32 (f));   // y1
				put (bingetr32 (f));   // y2
				put (bingetr32 (f));   // length
				if (uinteger_to_integer_a (fread (++ p, 8, integer_to_uinteger_a (numberOfArguments - 5), f)) < numberOfArguments - 5)   // text
					Melder_throw (U"Error reading graphics recordings.");
				p += numberOfArguments - 6;
			} else if (opcode == SET_VIEWPORT) {
				put (bingetr32 (f));                      // x1NDC
				put (bingetr32 (f));                      // x2NDC
				put (bingetr32 (f) + heightCorrection);   // y1NDC
				put (bingetr32 (f) + heightCorrection);   // y2NDC
			} else {
				for (integer i = numberOfArguments; i > 0; i --)
					put (bingetr32 (f));
			}
		}   
	} catch (MelderError) {
		my irecord = old_irecord;
		Melder_throw (U"Error reading graphics record ", added_irecord - (integer) (endp - p),
			U" out of ", added_irecord, U".\nOpcode ", opcode, U", args ", numberOfArguments, U".");
	}
}

void Graphics_markGroup (Graphics me) {
	if (my recording) { op (MARK_GROUP, 0); }
}

void Graphics_undoGroup (Graphics me) {
	integer lastMark = 0;   // not yet found
	integer jrecord = 0;
	while (jrecord < my irecord) {   // keep looking for marks until the end
		const int opcode = (int) my record [++ jrecord];
		integer number = (integer) my record [++ jrecord];
		if (opcode == MARK_GROUP)
			lastMark = jrecord - 1;   // found a mark
		jrecord += number;
	}
	if (jrecord != my irecord)
		Melder_flushError (U"jrecord != my irecord: ", jrecord, U", ", my irecord);
	if (lastMark > 0)   // found?
		my irecord = lastMark - 1;   // forget all graphics from and including the last mark
}

/* End of file Graphics_record.cpp */
