/* PitchArea.cpp
 *
 * Copyright (C) 1992-2012,2014-2022 Paul Boersma
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

#include "PitchArea.h"
#include "Pitch_to_Sound.h"
#include "EditorM.h"

Thing_implement (PitchArea, FunctionArea, 0);

#define HEIGHT_UNV  3.0
#define HEIGHT_INTENS  6.0
#define RADIUS  2.5


#pragma mark - PitchArea Query selection

static void QUERY_DATA_FOR_REAL__getPitch (PitchArea me, EDITOR_ARGS) {
	QUERY_DATA_FOR_REAL
		const double result = ( my startSelection() == my endSelection()
			? Pitch_getValueAtTime (my pitch(), my startSelection(), kPitch_unit::HERTZ, 1)
			: Pitch_getMean (my pitch(), my startSelection(), my endSelection(), kPitch_unit::HERTZ)
		);
	QUERY_DATA_FOR_REAL_END (U" Hz")
}


#pragma mark - PitchArea Modify whole

static void menu_cb_setCeiling (PitchArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Change ceiling", nullptr)
		POSITIVE (ceiling, U"Ceiling (Hz)", U"600.0")
	EDITOR_OK
		SET_REAL (ceiling, my pitch() -> ceiling)
	EDITOR_DO
		MODIFY_DATA (U"Change ceiling")
			Pitch_setCeiling (my pitch(), ceiling);
		MODIFY_DATA_END
	EDITOR_END
}

static void menu_cb_pathFinder (PitchArea me, EDITOR_ARGS) {
	EDITOR_FORM (U"Path finder", nullptr)
		REAL (silenceThreshold, U"Silence threshold", U"0.03")
		REAL (voicingThreshold, U"Voicing threshold", U"0.45")
		REAL (octaveCost, U"Octave cost", U"0.01")
		REAL (octaveJumpCost, U"Octave-jump cost", U"0.35")
		REAL (voicedUnvoicedCost, U"Voiced/unvoiced cost", U"0.14")
		POSITIVE (ceiling, U"Ceiling (Hz)", U"600.0")
		BOOLEAN (pullFormants, U"Pull formants", false)
	EDITOR_OK
		SET_REAL (ceiling, my pitch() -> ceiling)
	EDITOR_DO
		MODIFY_DATA (U"Path finder")
			Pitch_pathFinder (my pitch(), silenceThreshold, voicingThreshold,
					octaveCost, octaveJumpCost, voicedUnvoicedCost, ceiling, pullFormants);
		MODIFY_DATA_END
	EDITOR_END
}


#pragma mark - PitchArea Modify selection

static void menu_cb_octaveUp (PitchArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Octave up")
		Pitch_step (my pitch(), 2.0, 0.1, my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void menu_cb_fifthUp (PitchArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Fifth up")
		Pitch_step (my pitch(), 1.5, 0.1, my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void menu_cb_fifthDown (PitchArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Fifth down")
		Pitch_step (my pitch(), 1 / 1.5, 0.1, my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void menu_cb_octaveDown (PitchArea me, EDITOR_ARGS) {
	MODIFY_DATA (U"Octave down")
		Pitch_step (my pitch(), 0.5, 0.1, my startSelection(), my endSelection());
	MODIFY_DATA_END
}

static void menu_cb_voiceless (PitchArea me, EDITOR_ARGS) {
	const integer ileft = Melder_clippedLeft (1_integer, Sampled_xToHighIndex (my pitch(), my startSelection()));
	const integer iright = Melder_clippedRight (Sampled_xToLowIndex (my pitch(), my endSelection()), my pitch() -> nx);
	MODIFY_DATA (U"Unvoice")
		for (integer iframe = ileft; iframe <= iright; iframe ++) {
			const Pitch_Frame frame = & my pitch() -> frames [iframe];
			for (integer cand = 1; cand <= frame -> nCandidates; cand ++)
				if (frame -> candidates [cand]. frequency == 0.0)
					std::swap (frame -> candidates [1], frame -> candidates [cand]);
		}
	MODIFY_DATA_END
}


#pragma mark - PitchArea all menus

void structPitchArea :: v_createMenus () {
	PitchArea_Parent :: v_createMenus ();

	EditorMenu menu = Editor_addMenu (our functionEditor(), U"Pitch", 0);

	FunctionAreaMenu_addCommand (menu, U"- Query selected part of pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Get pitch", GuiMenu_F5,
			QUERY_DATA_FOR_REAL__getPitch, this);

	FunctionAreaMenu_addCommand (menu, U"- Modify whole pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Change ceiling...", 0,
			menu_cb_setCeiling, this);
	FunctionAreaMenu_addCommand (menu, U"Path finder...", 0,
			menu_cb_pathFinder, this);
	FunctionAreaMenu_addCommand (menu, U"- Modify selected part of pitch:", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Unvoice", 0,
			menu_cb_voiceless, this);
	FunctionAreaMenu_addCommand (menu, U"-- up and down --", 0, nullptr, this);
	FunctionAreaMenu_addCommand (menu, U"Octave up", 0,
			menu_cb_octaveUp, this);
	FunctionAreaMenu_addCommand (menu, U"Fifth up", 0,
			menu_cb_fifthUp, this);
	FunctionAreaMenu_addCommand (menu, U"Fifth down", 0,
			menu_cb_fifthDown, this);
	FunctionAreaMenu_addCommand (menu, U"Octave down", 0,
			menu_cb_octaveDown, this);
}


#pragma mark - PitchArea drawing

void structPitchArea :: v_drawInside () {
	const double dyUnv = Graphics_dyMMtoWC (our graphics(), HEIGHT_UNV);
	const double dyIntens = Graphics_dyMMtoWC (our graphics(), HEIGHT_INTENS);

	integer it1, it2;
	Sampled_getWindowSamples (our pitch(), our startWindow(), our endWindow(), & it1, & it2);

	/*
		Show pitch.
	*/
	{
		const double df =
			our pitch() -> ceiling > 10000.0 ? 2000.0 :
			our pitch() -> ceiling > 5000.0 ? 1000.0 :
			our pitch() -> ceiling > 2000.0 ? 500.0 :
			our pitch() -> ceiling > 800.0 ? 200.0 :
			our pitch() -> ceiling > 400.0 ? 100.0 :
			50.0
		;
		Graphics_Viewport previous;
		previous = Graphics_insetViewport (our graphics(), 0.0, 1.0, dyUnv, 1.0 - dyIntens);
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, our pitch() -> ceiling);
		const double radius = Graphics_dxMMtoWC (our graphics(), RADIUS);

		/* Horizontal hair at current pitch. */

		if (our startSelection() == our endSelection() && our startSelection() >= our startWindow() && our startSelection() <= our endWindow()) {
			const double frequency = Pitch_getValueAtTime (our pitch(), our startSelection(), kPitch_unit::HERTZ, Pitch_LINEAR);
			if (isdefined (frequency)) {
				Graphics_setColour (our graphics(), Melder_RED);
				Graphics_line (our graphics(), our startWindow() - radius, frequency, our endWindow(), frequency);
				Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
				Graphics_text (our graphics(), our startWindow() - radius, frequency, Melder_fixed (frequency, 2));
			}
		}

		/* Horizontal scaling lines. */

		Graphics_setColour (our graphics(), Melder_BLUE);
		Graphics_setLineType (our graphics(), Graphics_DOTTED);
		Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_HALF);
		for (double frequency = df; frequency <= our pitch() -> ceiling; frequency += df) {
			Graphics_line (our graphics(), our startWindow(), frequency, our endWindow(), frequency);
			Graphics_text (our graphics(), our endWindow() + 0.5 * radius, frequency,   frequency, U" Hz");
		}
		Graphics_setLineType (our graphics(), Graphics_DRAWN);

		/*
			Show candidates.
		*/
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & our pitch() -> frames [it];
			const double time = Sampled_indexToX (our pitch(), it);
			double frequency = frame -> candidates [1]. frequency;
			if (Pitch_util_frequencyIsVoiced (frequency, our pitch() -> ceiling)) {
				Graphics_setColour (our graphics(), Melder_MAGENTA);
				Graphics_fillCircle_mm (our graphics(), time, frequency, RADIUS * 2.0);
			}
			Graphics_setColour (our graphics(), Melder_BLACK);
			Graphics_setTextAlignment (our graphics(), Graphics_CENTRE, Graphics_HALF);
			for (integer icand = 1; icand <= frame -> nCandidates; icand ++) {
				frequency = frame -> candidates [icand]. frequency;
				if (Pitch_util_frequencyIsVoiced (frequency, our pitch() -> ceiling)) {
					const double strength = Melder_clippedRight (Melder_round_tieUp (10.0 * frame -> candidates [icand]. strength),
							9.0);   // map 0.0-1.0 to 0-9   ; this is a double because the occasional `undefined` should be shown as "--undefined--"
					Graphics_text (our graphics(), time, frequency,   strength);
				}
			}
		}
		Graphics_resetViewport (our graphics(), previous);
	}

	/*
		Show intensity.
	*/
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics(), 0.0, 1.0, 1.0 - dyIntens, 1.0);
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
		Graphics_setColour (our graphics(), Melder_BLACK);
		Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics(), our startWindow(), 0.5, U"intens");
		Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics(), our endWindow(), 0.5, U"intens");
		Graphics_setTextAlignment (our graphics(), Graphics_CENTRE, Graphics_HALF);
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & our pitch() -> frames [it];
			const double time = Sampled_indexToX (our pitch(), it);
			const integer strength = Melder_clippedRight (Melder_iround (10.0 * frame -> intensity + 0.5),
					9_integer);   // map 0.0-1.0 to 1-9
			Graphics_text (our graphics(), time, 0.5,   strength);
		}
		Graphics_resetViewport (our graphics(), previous);
	}

	if (it1 > 1)
		it1 -= 1;
	if (it2 < our pitch() -> nx)
		it2 += 1;

	/*
		Show voicelessness.
	*/
	{
		Graphics_Viewport previous = Graphics_insetViewport (our graphics(), 0.0, 1.0, 0.0, dyUnv);
		Graphics_setColour (our graphics(), Melder_BLUE);
		Graphics_line (our graphics(), our startWindow(), 1.0, our endWindow(), 1.0);
		Graphics_setTextAlignment (our graphics(), Graphics_RIGHT, Graphics_HALF);
		Graphics_text (our graphics(), our startWindow(), 0.5, U"Unv");
		Graphics_setTextAlignment (our graphics(), Graphics_LEFT, Graphics_HALF);
		Graphics_text (our graphics(), our endWindow(), 0.5, U"Unv");
		for (integer it = it1; it <= it2; it ++) {
			const Pitch_Frame frame = & our pitch() -> frames [it];
			const double time = Sampled_indexToX (our pitch(), it);
			double tleft = time - 0.5 * our pitch() -> dx, tright = time + 0.5 * our pitch() -> dx;
			double frequency = frame -> candidates [1]. frequency;
			if (Pitch_util_frequencyIsVoiced (frequency, our pitch() -> ceiling) || tright <= our startWindow() || tleft >= our endWindow())
				continue;
			Melder_clipLeft (our startWindow(), & tleft);
			Melder_clipRight (& tright, our endWindow());
			Graphics_fillRectangle (our graphics(), tleft, tright, 0.0, 1.0);
		}
		Graphics_setColour (our graphics(), Melder_BLACK);
		Graphics_resetViewport (our graphics(), previous);
	}
}

bool structPitchArea :: v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction) {
	if (! event -> isClick())
		return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world);   // move cursor or drag selection
	const double dyUnv = Graphics_dyMMtoWC (our graphics(), HEIGHT_UNV);
	const double dyIntens = Graphics_dyMMtoWC (our graphics(), HEIGHT_INTENS);
	const double clickedFrequency = (localY_fraction - dyUnv) / (1.0 - dyIntens - dyUnv) * our pitch() -> ceiling;
	double minimumDf = 1e30;
	integer bestCandidate = -1;

	integer ibestFrame = Sampled_xToNearestIndex (our pitch(), x_world);
	Melder_clip (1_integer, & ibestFrame, our pitch() -> nx);
	const Pitch_Frame bestFrame = & our pitch() -> frames [ibestFrame];

	const double tmid = Sampled_indexToX (our pitch(), ibestFrame);
	for (integer icand = 1; icand <= bestFrame -> nCandidates; icand ++) {
		const Pitch_Candidate candidate = & bestFrame -> candidates [icand];
		const double df = clickedFrequency - candidate -> frequency;
		if (fabs (df) < minimumDf) {
			minimumDf = fabs (df);
			bestCandidate = icand;
		}
	}
	if (bestCandidate != -1) {
		const double bestFrequency = bestFrame -> candidates [bestCandidate]. frequency;
		const double distanceWC = (clickedFrequency - bestFrequency) / our pitch() -> ceiling * (1.0 - dyIntens - dyUnv);
		const double dx_mm = Graphics_dxWCtoMM (our graphics(), x_world - tmid), dy_mm = Graphics_dyWCtoMM (our graphics(), distanceWC);
		if (bestFrequency < our pitch() -> ceiling &&   // above ceiling: ignore
		    ((bestFrequency <= 0.0 && fabs (x_world - tmid) <= 0.5 * our pitch() -> dx && clickedFrequency <= 0.0) ||   // voiceless: click within frame
		     (bestFrequency > 0.0 && dx_mm * dx_mm + dy_mm * dy_mm <= RADIUS * RADIUS)))   // voiced: click within circle
		{
			Editor_save (our boss(), U"Change path");
			std::swap (bestFrame -> candidates [1], bestFrame -> candidates [bestCandidate]);
			Editor_broadcastDataChanged (our boss());
			our setSelection (tmid, tmid);   // cursor will snap to candidate
			return FunctionEditor_UPDATE_NEEDED;
		} else {
			return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world);   // move cursor or drag selection
		}
	}
	return FunctionEditor_defaultMouseInWideDataView (our functionEditor(), event, x_world);   // move cursor or drag selection
}

/* End of file PitchArea.cpp */
