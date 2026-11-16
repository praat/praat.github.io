/* SpectrogramArea.cpp
 *
 * Copyright (C) 1992-2005,2007-2012,2014-2022,2026 Paul Boersma
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

#include "SpectrogramArea.h"

Thing_implement (SpectrogramArea, FunctionArea, 0);

void structSpectrogramArea :: v_drawInside () {
	integer itmin, itmax;
	Sampled_getWindowSamples (our spectrogram(), our startWindow(), our endWindow(), & itmin, & itmax);

	/*
		Autoscale frequency axis.
	*/
	our maximum = our spectrogram() -> ymax;

	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, our maximum);
	Spectrogram_paintInside (our spectrogram(), our graphics(), our startWindow(), our endWindow(), 0, 0, 0.0, true,
			60, 6.0, 0, kSpectrogram_unit::HERTZ, kSpectrogram_colourMap::GREY, false);

	/*
		Horizontal scaling lines.
	*/
	Graphics_setWindow (our graphics(), 0.0, 1.0, 0.0, our maximum);
	Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
	Graphics_setColour (our graphics(), Melder_RED);
	Graphics_setLineType (our graphics(), Graphics_DOTTED);
	const integer df = 1000;
	for (integer f = 0; f <= our maximum; f += df) {
		if (f != 0.0 && f != our maximum)
			Graphics_line (our graphics(), 0.0, f, 1.0, f);
		Graphics_text (our graphics(), 0.0, f,   f, U" Hz");
	}
	Graphics_setLineType (our graphics(), Graphics_DRAWN);
	if (our frequencyCursor > 0.0 && our frequencyCursor < our maximum) {
		Graphics_line (our graphics(), 0.0, our frequencyCursor, 1.0, our frequencyCursor);
		Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics(), 1.0, our frequencyCursor,   Melder_iround (our frequencyCursor), U" Hz");
	}

	/*
		Vertical cursor lines.
	*/
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, our maximum);
	if (our startSelection() > our startWindow() && our startSelection() < our endWindow())
		Graphics_line (our graphics(), our startSelection(), 0, our startSelection(), our maximum);
	if (our endSelection() > our startWindow() && our endSelection() < our endWindow())
		Graphics_line (our graphics(), our endSelection(), 0, our endSelection(), our maximum);
	Graphics_setColour (our graphics(), Melder_BLACK);
}

bool structSpectrogramArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	our frequencyCursor = localY_fraction * our maximum;
	const integer clickedFrame = Melder_clipped (1_integer, Sampled_xToNearestIndex (our spectrogram(), x_world), our spectrogram() -> nx);   // TODO
	return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world) || true;
}

/* End of file SpectrogramArea.cpp */
