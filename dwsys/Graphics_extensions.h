#ifndef _Graphics_extensions_h_
#define _Graphics_extensions_h_
/* Graphics_extensions.h
 *
 * Copyright (C) 2012-2022 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Graphics.h"
#include "Graphics_extensions_enums.h"

/*
	Get a font size to display at least 'maxNumberOfLines' of text that consistes of 
	'maxNumberOfCharacters_line' characters per line.
*/
double Graphics_getFontSizeInsideBox (Graphics g, double widthWC, double heightWC, double maxNumberOfCharacters_line, double maxNumberOfLines);


/**
	Draw a box plot of data [1..ndata]. The vertical center line of the plot
	is at position 'x'. The rectangle box is 2*w wide, the whisker 2*r.
	All drawing outside [ymin, ymax] is clipped.
*/
void Graphics_boxAndWhiskerPlot (Graphics g, constVEC data, double x, double r, double w, double ymin, double ymax);

void Graphics_quantileQuantilePlot (Graphics g, integer numberOfQuantiles, constVEC xdata, constVEC ydata,
	double xmin, double xmax, double ymin, double ymax, double labelSize, conststring32 plotLabel);

void Graphics_lagPlot (Graphics g, constVEC x, double xmin, double xmax, integer lag, double labelSize, conststring32 plotLabel);

void getGridLayout (integer numberOfItems, integer *out_numberOfRows, integer *out_numberOfColumns);
/*
	Get dimensions of a two-dimensional grid to layout n elements as nrow x ncol,
	where nrow x ncol >= n and nrow >= ncol and nrow - ncol <= 1. 
*/

integer getGridCellIndex (double x, double y, integer numberOfRows, integer numberOfColumns);
/*
	Implicit:
		Rectangular grid numberOfRows x numberOfColumns
		Origin (0,0) is at left-bottom, top-right is at (1, 1)
*/


/*
	Clipline segments from (x1,y1) to (x2, y2) against a rectangular clipping window
	whose bottom-left point is (xL,yB) anf top-right point is (xR,yT).
*/
typedef struct structLineSegmentClipper *LineSegmentClipper;
using autoLineSegmentClipper = std::unique_ptr<structLineSegmentClipper>;

autoLineSegmentClipper LineSegmentClipper_create (double xL, double xR, double yB, double yT);

bool LineSegmentClipper_clip (LineSegmentClipper me, double& x1, double& y1, double& x2, double& y2);

struct structLineSegmentClipper {

	private:

	typedef struct structHPoint {
		double x, y, w;
	} *HPoint;
	/*
		The four corners of the rectangle in homogeneous coordinates (x, y, w)
		Order BottomLeft, BottomRight, TopRight, TopLeft
		The edges from c[1] to c[2], c[2] to c[3], c[3] to c[4], c[4] to c[1].
	*/
	autovector<structHPoint> corners;
	autovector<structHPoint> edges;
	struct structHPoint from, to, p; // working storage
	autoINTVEC tab1, tab2, mask;
	
	const int INSIDE = 0, LEFT = 1, RIGHT = 1<<1, BOTTOM = 1<<2, TOP = 1<<3;
	
	int getCohenSutherlandCode (HPoint p) {
		int result = INSIDE;
		if (p -> x < corners [1].x)
			result |= LEFT;
		else if (p -> x > corners[3].x)      // to the right of clip window
			result |= RIGHT;
		if (p -> y < corners[1].y)           // below the clip window
			result |= BOTTOM;
		else if (p -> y > corners[3].y)      // above the clip window
			result |= TOP;
		return result;
	}
	
	void cross (HPoint a, HPoint b, HPoint p) {  // p = a x b
		p -> x = a -> y * b -> w - b -> y * a -> w;
		p -> y = b -> x * a -> w - a -> x * b -> w;
		p -> w = a -> x * b -> y - b -> x * a -> y;
	}
	
	double in (HPoint a, HPoint b) { // sum a[i]*b[i]
		return  a -> x * b -> x + a -> y * b -> y + a -> w * b -> w;
	}

	bool clipLine () {
		const int fromCode = getCohenSutherlandCode (& from);
		const int toCode = getCohenSutherlandCode (& to);
		
		if ((fromCode | toCode) == 0) // case 1, Fig 5: all inside?
			return true;

		if ((fromCode & toCode) != 0) // case 2, Fig 5: all outside?
			return false;
		/*
			trivial cases are done
		*/

		cross (& from, & to, & p);
		int c = 0;
		for (int i = 1; i <= 4; i ++)
			if (in (& p, & corners [i]) >= 0)
				c |= 1 << (i - 1);
		c += 1;// we count from 1
		if (c == 1 || c == 16) // case 6, Fig 5:
			return false;
		Melder_assert (c % 5 != 1);
		const int i = tab1 [c], j = tab2 [c];
		if (fromCode != 0 && toCode != 0) { // two intersections?
			cross (& p, & edges [i], & from);
			cross (& p, & edges [j], & to);
		} else { // only one intersection
			if (fromCode == 0) {  // to is outside
				if ((toCode & mask [c]) != 0)
					cross (& p, & edges [i], & to);
				else
					cross (& p, & edges [j], & to);
			} else if (toCode == 0){ // from is outside
				if ((fromCode & mask [c]) != 0)
					cross (& p, & edges [i], & from);
				else
					cross (& p, & edges [j], & from);
			}
		}
		return true;
	}
public:

	/*
		For rectangular window:
	*/
	void init (double xL, double xR, double yB, double yT) {
		Melder_assert (xL < xR && yB < yT);
		corners = {{xL, yB, 1.0}, {xR, yB, 1.0}, {xR, yT, 1.0}, {xL, yT, 1.0}};
		edges = newvectorzero<structHPoint> (4);
		for (integer i = 1; i <= 4; i ++) {
			const integer ip1 = i % 4 + 1;
			cross (& corners [i], & corners[ip1], & edges[i]);
		}
		/*
			Table 1 in Skala (2005) for indices 0,1,2,3
			tab1 = {-1, 0, 0, 1, 1, -2, 0, 2, 2, 0, -2, 1, 1, 0, 0, -1};
			tab2 = {-1, 3, 1, 3, 2, -2, 2, 3, 3, 2, -2, 2, 3, 1, 3, -1};
			Our indices are 1,2,3,4 so we add 1 to these tables.
		*/
		tab1 = {-1, 1, 1, 2, 2, -2, 1, 3, 3, 1, -2, 2, 2, 1, 1, -1}; // count from 1
		tab2 = {-1, 4, 2, 4, 3, -2, 3, 4, 4, 3, -2, 3, 4, 2, 4, -1};  // count from 1
		mask = {-1, 4, 4, 2, 2, -2, 4, 8, 8, 4, -2, 2, 2, 4, 4, -1};
	}
	
	bool clip (double& x1, double& y1, double& x2, double& y2) {
		bool result = false;
		from = { x1, y1, 1.0 };
		to   = { x2, y2, 1.0 };
		if (clipLine ()) {
			x1 = from.x / from.w; y1 = from.y / from.w;
			x2 = to.x / to.w;     y2 = to.y / to.w;
			result = true;
		}
		return result;
	}
};

#endif /* _Graphics_extensions_h_ */
