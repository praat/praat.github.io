/* Transition.cpp
 *
 * Copyright (C) 1997-2012,2015-2020,2022,2026 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include "Transition.h"
#include "NUM2.h"
#include "Eigen.h"

#include "oo_DESTROY.h"
#include "Transition_def.h"
#include "oo_COPY.h"
#include "Transition_def.h"
#include "oo_EQUAL.h"
#include "Transition_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Transition_def.h"
#include "oo_WRITE_BINARY.h"
#include "Transition_def.h"
#include "oo_READ_TEXT.h"
#include "Transition_def.h"
#include "oo_READ_BINARY.h"
#include "Transition_def.h"
#include "oo_DESCRIPTION.h"
#include "Transition_def.h"

Thing_implement (Transition, Daata, 0);

void structTransition :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Number of states: ", numberOfStates);
}

void structTransition :: v1_writeText (MelderFile file) {
	texputi32 (file, numberOfStates, U"numberOfStates");
	MelderFile_write (file, U"\nstateLabels []: ");
	if (numberOfStates < 1) MelderFile_write (file, U"(empty)");
	MelderFile_write (file, U"\n");
	for (integer i = 1; i <= numberOfStates; i ++) {
		MelderFile_write (file, U"\"");
		if (stateLabels [i]) MelderFile_write (file, stateLabels [i].get());
		MelderFile_write (file, U"\"\t");
	}
	for (integer i = 1; i <= numberOfStates; i ++) {
		MelderFile_write (file, U"\nstate [", i, U"]:");
		for (integer j = 1; j <= numberOfStates; j ++) {
			MelderFile_write (file, U"\t", data [i] [j]);
		}
	}
}

void Transition_init (Transition me, integer numberOfStates) {
	if (numberOfStates < 1)
		Melder_throw (U"Cannot create empty matrix.");
	my numberOfStates = numberOfStates;
	my stateLabels = autoSTRVEC (numberOfStates);
	my data = zero_MAT (my numberOfStates, my numberOfStates);
}

autoTransition Transition_create (integer numberOfStates) {
	try {
		autoTransition me = Thing_new (Transition);
		Transition_init (me.get(), numberOfStates);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Transition not created.");
	}
}

static void NUMrationalize (double x, integer *numerator, integer *denominator) {
	double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator, rounded = round (numerator_d);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = (integer) rounded;
			return;
		}
	}
	*denominator = 0;   // failure
}

static void print4 (char *buffer, double value, int iformat, int width, int precision) {
	char formatString [40];
	if (iformat == 4) {
		integer numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			snprintf (buffer, 40, "0");
		else if (denominator > 1)
			snprintf (buffer, 40, "%s/%s", Melder8_integer (numerator), Melder8_integer (denominator));
		else
			snprintf (buffer, 40, "%.7g", value);
	} else {
		snprintf (formatString, 40, "%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		snprintf (buffer, 40, formatString, value);
	}
}

void Transition_drawAsNumbers (Transition me, Graphics g, int iformat, int precision) {
	double maxTextWidth = 0.0, maxTextHeight = 0.0;
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfStates + 0.5, 0.0, 1.0);
	const double leftMargin = Graphics_dxMMtoWC (g, 1.0);
	const double lineSpacing = Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72.0);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (integer col = 1; col <= my numberOfStates; col ++) {
		if (my stateLabels && my stateLabels [col] && my stateLabels [col] [0]) {
			Graphics_text (g, col, 1, my stateLabels [col].get());
			if (maxTextHeight == 0.0) maxTextHeight = lineSpacing;
		}
	}
	for (integer row = 1; row <= my numberOfStates; row ++) {
		double y = 1 - lineSpacing * (row - 1 + 0.7);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my stateLabels && my stateLabels [row]) {
			double textWidth = Graphics_textWidth (g, my stateLabels [row].get());
			if (textWidth > maxTextWidth) maxTextWidth = textWidth;
			Graphics_text (g, 0.5 - leftMargin, y, my stateLabels [row].get());
		}
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (integer col = 1; col <= my numberOfStates; col ++) {
			char text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, Melder_peek8to32_u (text));
		}
	}
	if (maxTextWidth != 0.0)
		Graphics_line (g, 0.5 - maxTextWidth - leftMargin, 1, my numberOfStates + 0.5, 1);
	if (maxTextHeight != 0.0)
		Graphics_line (g, 0.5, 1 + maxTextHeight, 0.5, 1 - lineSpacing * (my numberOfStates + 0.2));
	Graphics_unsetInner (g);
}

static void Transition_transpose (Transition me) {
	for (integer i = 1; i < my numberOfStates; i ++) {
		for (integer j = i + 1; j <= my numberOfStates; j ++) {
			double temp = my data [i] [j];
			my data [i] [j] = my data [j] [i];
			my data [j] [i] = temp;
		}
	}
}

void Transition_eigen (Transition me, autoMatrix *out_eigenvectors, autoMatrix *out_eigenvalues) {
	bool transposed = false;
	try {
		Transition_transpose (me);
		autoEigen eigen = Eigen_createFromSquareMAT (my data.get(), kMAT_TYPE::SYMMETRIC, 0, false);
		Transition_transpose (me);
		transposed = true;
		autoMatrix eigenvectors = Matrix_createSimple (my numberOfStates, my numberOfStates);
		autoMatrix eigenvalues = Matrix_createSimple (my numberOfStates, 1);
		for (integer i = 1; i <= my numberOfStates; i ++) {
			eigenvalues -> z [i] [1] = eigen -> eigenvalues [i];
			for (integer j = 1; j <= my numberOfStates; j ++)
				eigenvectors -> z [i] [j] = eigen -> eigenvectors [j] [i];
		}
		*out_eigenvectors = eigenvectors.move();
		*out_eigenvalues = eigenvalues.move();
	} catch (MelderError) {
		if (transposed)
			Transition_transpose (me);
		Melder_throw (me, U": eigenvectors not computed.");
	}
}

autoTransition Transition_power (Transition me, integer power) {
	try {
		autoTransition thee = Data_copy (me);
		autoTransition him = Data_copy (me);
		for (integer ipow = 2; ipow <= power; ipow ++) {
			std::swap (his data.cells, thy data.cells);   // OPTIMIZE
			for (integer irow = 1; irow <= my numberOfStates; irow ++) {
				for (integer icol = 1; icol <= my numberOfStates; icol ++) {
					thy data [irow] [icol] = 0.0;
					for (integer i = 1; i <= my numberOfStates; i ++) {
						thy data [irow] [icol] += his data [irow] [i] * my data [i] [icol];
					}
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": power not computed.");
	}
}

autoMatrix Transition_to_Matrix (Transition me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfStates, my numberOfStates);
		for (integer i = 1; i <= my numberOfStates; i ++)
			for (integer j = 1; j <= my numberOfStates; j ++)
				thy z [i] [j] = my data [i] [j];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoTransition Matrix_to_Transition (Matrix me) {
	try {
		if (my nx != my ny)
			Melder_throw (U"Matrix should be square.");
		autoTransition thee = Transition_create (my nx);
		for (integer i = 1; i <= my nx; i ++)
			for (integer j = 1; j <= my nx; j ++)
				thy data [i] [j] = my z [i] [j];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Transition.");
	}
}

/* End of file Transition.cpp */
