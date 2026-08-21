/* DTW.cpp
 *
 * Copyright (C) 1993-2023 David Weenink, 2017 Paul Boersma
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
 * ilong with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020813 GPL header
 djmw 20050302 Path finder modified with sakoe-chiba band
 djmw 20050305 DTW_to_Polygon
 djmw 20050306 DTW_swapAxes
 djmw 20050530 Added Matrices_to_DTW
 djmw 20060909 DTW_getPathY linear behaviour outside domains.
 djmw 20061205 Pitches_to_DTW
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20071016 To Melder_error<n>
 djmw 20071022 Extra comments + possible bug correction in DTW_Path_recode.
 djmw 20071201 Melder_warning<n>.
 djmw 20071204 DTW_and_Sounds_draw.
 djmw 20080122 float -> double
 djmw 20081123 DTW_and_Sounds_checkDomains did not swap sounds correctly.
 djmw 20091009 Removed a bug in DTW_Path_recode that could cause two identical x and y times in succession at the end.
 djmw 20100504 extra check in DTW_Path_makeIndex
 djmw 20110304 Thing_new
*/

#include "DTW.h"
#include "Graphics_extensions.h"
#include "Sound_extensions.h"
#include "NUM2.h"
#include "NUMmachar.h"

#include "oo_DESTROY.h"
#include "DTW_def.h"
#include "oo_COPY.h"
#include "DTW_def.h"
#include "oo_EQUAL.h"
#include "DTW_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "DTW_def.h"
#include "oo_WRITE_TEXT.h"
#include "DTW_def.h"
#include "oo_WRITE_BINARY.h"
#include "DTW_def.h"
#include "oo_READ_TEXT.h"
#include "DTW_def.h"
#include "oo_READ_BINARY.h"
#include "DTW_def.h"
#include "oo_DESCRIPTION.h"
#include "DTW_def.h"

Thing_implement (DTW, Matrix, 2);

#define DTW_BIG 1e308

void structDTW :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Domain of prototype: ", ymin, U" to ", ymax, U" (s).");   // ppgb: Wat is een domain prototype?
	MelderInfo_writeLine (U"Domain of candidate: ", xmin, U" to ", xmax, U" (s).");   // ppgb: Wat is een domain candidate?
	MelderInfo_writeLine (U"Number of frames in prototype: ", ny);
	MelderInfo_writeLine (U"Number of frames in candidate: ", nx);
	MelderInfo_writeLine (U"Path length (frames): ", pathLength);
	MelderInfo_writeLine (U"Global warped distance: ", weightedDistance);
	if (nx == ny) {
		double dd = 0;
		for (integer i = 1; i <= nx; i ++) {
			dd += z [i] [i];
		}
		MelderInfo_writeLine (U"Distance along diagonal: ", dd / nx);
	}
}

static void DTW_drawPath_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish, bool inset);
static void DTW_paintDistances_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish, bool inset);
static double _DTW_Sounds_getPartY (Graphics g, double dtw_part_x);
static void DTW_findPath_special (DTW me, bool matchStart, bool matchEnd, int slope, autoMatrix *cumulativeDists);
/*
	Two 'slope lines, lh and ll, start in the lower left corner, the upper/lower has the maximum/minimum allowed slope.
	Two other lines, ru and rl, end in the upper-right corner. The upper/lower line have minimum/maximum slope.
	1. the four lines with the two intersections determine a diamond
		For vertical line at x we return the upper and lower y of the two intersections
	1. When we don't have a diamond, we return an error and the vertical distance between the slopelines in ylow.
*/


/*
   The actual path will be interpolated as follows:
   The distance matrix has cells of dimensions dx by dy.
   The optimal path connects these cells with one another in the following way:

   In a diagonal ''path'' segment, i.e. when cells have no side in common,
   the interpolated path runs from the lowerleft corner to the upperright corner.
   If a path segment is horizontal or vertical the path also runs from lowerleft to upperright.
   It is only when a horizontal and a vertical segment have a cell in common that we have to make
   some adjustments to the path in the common cell.
   This will be done as follows:
   Let nx and ny be the number of horizontal and vertical cells in this path segment.
   The path will always start at the lowerleft of the leftmost block and end
   at the upperright of the rightmost block.
   In the common cell we have to introduce an extra point in the path at (x1,y1)
   where the line from the horizontal segment and the line from the vertical segment intersect.
   Let the lowerleft and upperright point of the common cell be (0,0) and (dx,dy),
   then the coordinates (x,y) of the intersection can be calculated as follows:
   If we have two lines y = a1*x+b1 and y = a2*x+b2 they intersect at
      (x,y) = (b2-b1, a1*b2-a2*b1)/(a1-a2)
	For horizontal-vertical (hv) cells:
		a1 = dy/(nx*dx),   a2 = ny*dy/dx
		b1 = dy*(nx-1)/nx, b2 = 0
	Then:
		(x,y) = (dx, dy*ny ) * (nx-1)/(nx*ny-1)

	For vertical-horizontal (vh) cells:
		a1 = ny*dy/dx,   a2 = dy/(nx*dx)
		b1 = -(ny-1)*dy   b2 = 0
	Then:
		(x,y) = (nx*dx, dy) * (ny-1)/(nx*ny-1)
*/

/* DTW_getXTime (DTW me, (DTW_getYTime (DTW me, double tx)) == tx */
double DTW_getYTimeFromXTime (DTW me, double tx) {
	double time = tx; // Catch cases where tier would give constant extrapolation
	/*
		If `eps` were zero, then the condition would rely on floating-point equality,
		which is a bad idea (e.g. the result would be platform-dependent).
		(fix ppgb 20240420, after this crashed on Windows only)
		(rewind 20240501, because of retries in SpeechSynthesizer & Sound & TextGrid alignment)
	*/
	const double eps = 0.0 * (my xmax - my xmin);
	if (tx > my xmin + eps && tx < my xmax - eps) {
		DTW_Path_Query thee = & my pathQuery;
		time = RealTier_getValueAtTime (thy yfromx.get(), tx);
	} else {
		//TRACE
		trace (U"Time set to ", tx, U" instead of between ", my xmin, U" and ", my xmax, U".");
	}
	return time;
}

double DTW_getXTimeFromYTime (DTW me, double ty) {
	double time = ty; // Catch cases where tier would give constant extrapolation
	/*
		If `eps` were zero, then the condition would rely on floating-point equality,
		which is a bad idea (e.g. the result would be platform-dependent).
		(fix ppgb 20240420, after this crashed on Windows only)
		(rewind 20240501, because of retries in SpeechSynthesizer & Sound & TextGrid alignment)
	*/
	const double eps = 0.0 * (my ymax - my ymin);
	if (ty > my ymin + eps && ty < my ymax - eps) {
		DTW_Path_Query thee = & my pathQuery;
		time = RealTier_getValueAtTime (thy xfromy.get(), ty);
	}
	return time;
}

void DTW_Path_Query_init (DTW_Path_Query me, integer ny, integer nx) {
	Melder_assert (ny > 0 && nx > 0);
	my ny = ny;
	my nx = nx;
	my nxy = 2 * std::max (ny, nx) + 2; // maximum number of points
	my xfromy = Thing_new (RealTier);
	my yfromx = Thing_new (RealTier);
}

/* Recode the path from a chain of cells to a piecewise linear path. */
void DTW_Path_recode (DTW me) {
	try {
		DTW_Path_Query thee = & my pathQuery;
		integer nxy;		// current number of elements in recoded path
		integer nsc_x = 1;	// current number of successive horizontal cells in the cells chain
		integer nsc_y = 1;	// current number of successive vertical cells in the cells chain
		integer nd = 0;	// current number of successive diagonal cells in the cells chain
		bool yDirection = false;	// previous segment in the original path was vertical
		bool xDirection = false;	// previous segment in the original path was horizontal
		integer ixp = 0, iyp = 0; // previous cell
		struct structPoint {
			double x,y;
		};

		// 1. Starting point always at origin
		
		const integer nxymax = thy nx + thy ny + 2;
		autovector <structPoint> xytimes = newvectorzero <structPoint> (nxymax);
		xytimes [1]. x = my xmin;
		xytimes [1]. y = my ymin;
		
		// 2. next point lower left of first cell
		
		nsc_x = my path [1]. x;
		ixp = nsc_x - 1;
		xytimes [2]. x = my x1 + (nsc_x - 1 - 0.5) * my dx;
		nsc_y = my path [1]. y;
		iyp = nsc_y - 1;
		xytimes [2]. y = my y1 + (nsc_y - 1 - 0.5) * my dy;
		
		// 3. follow all cells. implicit: my x1 - 0.5 * my dx > my xmin && my y1 - 0.5 * my dy > my ymin */
		
		nxy = 2;
		for (integer j = 1; j <= my pathLength; j ++) {
			const integer ix = my path [j]. x, iy = my path [j]. y;
			const double xright = my x1 + (ix - 1 + 0.5) * my dx;
			const double ytop = my y1 + (iy - 1 + 0.5) * my dy;
			double x, y, f;
			integer index; // where are we in the new path?

			if (iy == iyp) { // horizontal path?
				xDirection = true;
				if (yDirection) {
					// We came from a vertical direction so this is the second horizontal cell in a row.
					// The statement after this "if" updates nsc_x to 2.
					nsc_x = 1;
					yDirection = false;
				}
				nsc_x ++;

				if (nsc_y > 1 || nd > 1) {
					// Previous segment was diagonal or vertical: modify intersection
					// The vh intersection (x,y) = (nsc_x*dx, dy) * (nsc_y-1)/(nsc_x*nsc_y-1)
					// A diagonal segment has nsc_y = 1.
					f = (nsc_y - 1.0) / (nsc_x * nsc_y - 1);
					x = xright - nsc_x * my dx + nsc_x * my dx * f;
					y = ytop - my dy + my dy * f;
					index = nxy - 1;
					if (nsc_x == 2)
						index = nxy ++;

					xytimes [index]. x = x;
					xytimes [index]. y = y;
				}
				nd = 0;
			} else if (ix == ixp) { // vertical
				yDirection = true;
				if (xDirection) {
					nsc_y = 1;
					xDirection = false;
				}
				nsc_y ++;

				if (nsc_x > 1 || nd > 1) {
					// The hv intersection (x,y) = (dx, dy*nsc_y ) * (nsc_x-1)/(nsc_x*nsc_y-1)
					f = (nsc_x - 1.0) / (nsc_x * nsc_y - 1);
					x = xright - my dx + my dx * f;
					y = ytop - nsc_y * my dy + nsc_y * my dy * f;
					index = nxy - 1;
					if (nsc_y == 2)
						index = nxy ++;
					xytimes [index]. x = x;
					xytimes [index]. y = y;
				}
				nd = 0;
			} else if (ix == ixp + 1 && iy == iyp + 1) {   // diagonal
				nd ++;
				if (nd == 1)
					nxy ++;
				nsc_x = nsc_y = 1;
			} else {
				Melder_throw (U"The path goes back in time.");
			}
			// update
			xytimes [nxy]. x = xright;
			xytimes [nxy]. y = ytop;
			ixp = ix;
			iyp = iy;
		}

		if (my xmax > xytimes [nxy].x || my ymax > xytimes [nxy].y) {
			xytimes [++ nxy]. x = my xmax;
			xytimes [nxy]. y = my ymax;
		}
		Melder_assert (nxy <= 2 * std::max (my ny, my nx) + 2);
		
		thy nxy = nxy;
		thy yfromx = RealTier_create (my xmin, my xmax);
		thy xfromy = RealTier_create (my ymin, my ymax);
		for (integer i = 1; i <= nxy; i ++) {
			RealTier_addPoint (thy yfromx.get(), xytimes [i]. x, xytimes [i]. y);
			RealTier_addPoint (thy xfromy.get(), xytimes [i]. y, xytimes [i]. x);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not recoded.");
	}
}

void DTW_pathRemoveRedundantNodes (DTW me) {
	integer i = 1, skip = 0;

	for (integer j = 2; j <= my pathLength; j ++) {
		if (my path [j]. y == my path [i]. y || my path [j]. x == my path [i]. x)
			skip ++;
		else {
			/* if (j-1)^th was the last of a series: store it */
			if (skip > 0)
				my path [++ i] = my path [j - 1];
			/* same check again */
			skip = 0;
			if (my path [j]. y == my path [i]. y || my path [j]. x == my path [i]. x)
				skip ++;
			else 
				my path [++ i] = my path [j];
		}
	}
	if (skip > 0)
		my path [++ i] = my path [my pathLength];
	my pathLength = i;
	my path.resize (my pathLength);   // maintain invariant
}

/* Prototype must be on y-axis and test on x-axis */

autoDTW DTW_create (double tminp, double tmaxp, integer ntp, double dtp, double t1p, double tminc, double tmaxc, integer ntc, double dtc, double t1c) {
	try {
		autoDTW me = Thing_new (DTW);
		Matrix_init (me.get(), tminc, tmaxc, ntc, dtc, t1c, tminp, tmaxp, ntp, dtp, t1p);
		my path = newvectorzero <structDTW_Path> (ntc + ntp - 1);
		DTW_Path_Query_init (& my pathQuery, ntp, ntc);
		my wx = 1;
		my wy = 1;
		my wd = 2;
		return me;
	} catch (MelderError) {
		Melder_throw (U"DTW not created.");
	}
}

void DTW_setWeights (DTW me, double wx, double wy, double wd) {
	my wx = wx;
	my wy = wy;
	my wd = wd;
}

autoDTW DTW_swapAxes (DTW me) {
	try {
		autoDTW thee = DTW_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		
		transpose_MAT_out (thy z.get(), my z.get());
		
		thy pathLength = my pathLength;
		for (integer i = 1; i <= my pathLength; i ++) {
			thy path [i]. x = my path [i]. y;
			thy path [i]. y = my path [i]. x;
		}
		thy path.resize (thy pathLength);   // maintain invariant
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": axes not swapped.");
	}
}

double DTW_getPathY (DTW me, double tx) {
	// Assume linear behaviour outside domains
	//  Other option: scale with x_domain/y_domain

	if (tx < my xmin)
		return my ymin - (my xmin - tx);
	if (tx > my xmax)
		return my ymax + (tx - my xmax);


	// Find column in DTW matrix

	integer ix = Melder_ifloor ((tx - my x1) / my dx) + 1;
	if (ix < 1)
		ix = 1;
	if (ix > my nx)
		ix = my nx;

	// Find index in the path and the row number (iy)

	integer i = ix + my path [1]. x - 1;
	while (i <= my pathLength && my path [i]. x != ix)
		i ++;

	if (i > my pathLength)
		return undefined;

	const integer iy = my path [i]. y; /* row */

	/*
		We like to have a "continuous" interpretation of time for the quantized x and y times.
		A time block is specified by lower left (x,y) and upper right (x+dx,y+dy).
		The path is interpreted as piecewise linear.
		Two special cases:
		1. the local path is horizontal, i.e. two or more path elements with the same path.y value.
			We calculate the y-time from the line that connects the lower-left position of the leftmost
			horizontal time block to the upper-right position of the rightmost time horizontal block.
			(lowest column number to highest column number)
			For the first column and the last column we need to do something special if
		2. the local path is vertical, i.e. two or path elements with the same path.x value.
			We calculate the y-time from the line that connects the lower-left position of the bottommost
			vertical time block to the upper-right position of the topmost time horizontal block.
			(lowest row number to highest row number)
	*/

	// Horizontal path? Find left and right positions.

	integer ileft = i - 1;
	while (ileft >= 1 && my path [ileft]. y == iy)
		ileft --;

	ileft ++;
	if (ileft == 1 && ix > 1 && my path [ileft]. y > 1)
		ileft ++;

	integer iright = i + 1;
	while (iright <= my pathLength && my path [iright]. y == iy)
		iright ++;

	iright --;
	if (iright == my pathLength && ix < my nx && my path [iright]. y < my ny)
		iright --;

	integer nxx = iright - ileft + 1;

	// Vertical path? Only if not already horizontal.

	integer ibottom = i, itop = i;

	if (nxx == 1) {
		ibottom --;
		while (ibottom >= 1 && my path [ibottom]. x == ix)
			ibottom --;

		ibottom ++;

		itop ++;
		while (itop <= 1 && my path [itop]. x == ix)
			itop --;

		itop ++;
	}

	const integer nyy = itop - ibottom + 1;
	double boxx = nxx * my dx;
	double boxy = nyy * my dy;
	double ty;

	// Corrections at extreme left and right if path [1].x=1 && path [1].y>1

	if (ix == my nx) {
		boxx = my xmax - (my x1 + (ix - 1) * my dx - my dx / 2.0);
		boxy = my ymax - (my y1 + (iy - 1) * my dy - my dy / 2.0);
		ty = my ymax - (boxy - (boxx - (my xmax - tx)) * boxy / boxx);
	} else if (ix == 1) {
		boxx = my x1 + my dx / 2.0 - my xmin;
		boxy = my y1 + (itop - 1) * my dy + my dy / 2.0 - my ymin;
		ty = (tx - my xmin) * boxy / boxx + my ymin;
	} else {
		// Diagonal interpolation in a box with lower left (0,0) and upper right (nxx*dx, nyy*dy).
		const double ty0 = (tx - (my x1 + (my path [ileft]. x - 1.0) * my dx - my dx / 2.0)) * boxy / boxx;
		ty =  my y1 + (my path [ibottom]. y - 1.0) * my dy - my dy / 2.0 + ty0;
	}
	return ty;
}

integer DTW_getMaximumConsecutiveSteps (DTW me, int direction) {
	integer nglobal = 1, nlocal = 1;
	for (integer i = 2; i <= my pathLength; i ++) {
		int localdirection;
		if (my path [i]. y == my path [i - 1]. y)
			localdirection = DTW_X;
		else if (my path [i]. x == my path [i - 1]. x)
			localdirection = DTW_Y;
		else
			localdirection = DTW_XANDY;
		if (localdirection == direction)
			nlocal += 1;
		if (direction != localdirection || i == my pathLength) {
			if (nlocal > nglobal)
				nglobal = nlocal;
			nlocal = 1;
		}
	}
	return nglobal;
}

static void DTW_paintDistances_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish, bool inset) {
	integer ixmin, ixmax, iymin, iymax;
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	SampledXY_unidirectionalAutowindowY (me, & ymin, & ymax);
	(void) Matrix_getWindowSamplesX (me, xmin - 0.49999 * my dx, xmax + 0.49999 * my dx, & ixmin, & ixmax);
	(void) Matrix_getWindowSamplesY (me, ymin - 0.49999 * my dy, ymax + 0.49999 * my dy, & iymin, & iymax);
	if (maximum <= minimum)
		(void) Matrix_getWindowExtrema (me, ixmin, ixmax, iymin, iymax, & minimum, & maximum);

	if (maximum <= minimum) {
		minimum -= 1.0;
		maximum += 1.0;
	}
	if (xmin >= xmax || ymin >= ymax)
		return;
	if (inset)
		Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_cellArray (g, my z.part (iymin, iymax, ixmin, ixmax),
			Matrix_columnToX (me, ixmin - 0.5), Matrix_columnToX (me, ixmax + 0.5),
			Matrix_rowToY (me, iymin - 0.5), Matrix_rowToY (me, iymax + 0.5),
			minimum, maximum);
	Graphics_rectangle (g, xmin, xmax, ymin, ymax);
	if (inset)
		Graphics_unsetInner (g);
	if (garnish) {
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DTW_paintDistances (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish) {
	DTW_paintDistances_raw (me, g, xmin, xmax, ymin, ymax, minimum, maximum, garnish, true);
}

static double RealTier_getXAtIndex (RealTier me, integer ipoint) {
	return ( ipoint > 0 && ipoint <= my points.size ? my points.at [ipoint] -> number : undefined );
}

static void DTW_drawPath_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish, bool inset) {
	DTW_Path_Query thee = & my pathQuery;

	if (xmin >= xmax) {
		xmin = my xmin;
		xmax = my xmax;
	}
	if (ymin >= ymax) {
		ymin = my ymin;
		ymax = my ymax;
	}

	if (inset)
		Graphics_setInner (g);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	double x1 = RealTier_getXAtIndex (thy yfromx.get(), 1);
	double y1 = RealTier_getValueAtIndex (thy yfromx.get(), 1);
	autoLineSegmentClipper clipper = LineSegmentClipper_create (xmin, xmax, ymin, ymax);
	for (integer i = 2; i <= thy yfromx -> points.size; i ++) {
		const double x2 = RealTier_getXAtIndex (thy yfromx.get(), i);
		const double y2 = RealTier_getValueAtIndex (thy yfromx.get(), i);
		double x1c = x1, y1c = y1, x2c = x2, y2c = y2;
		if (clipper -> clip (x1c, y1c, x2c, y2c))
			Graphics_line (g, x1c, y1c, x2c, y2c);
		x1 = x2;
		y1 = y2;
	}

	if (inset)
		Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void DTW_drawPath (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	DTW_drawPath_raw (me, g, xmin, xmax, ymin, ymax, garnish, true);
}

static void DTW_drawWarp_raw (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double t, bool garnish, bool inset, bool warpX) {
	const double tx = ( warpX ? t : DTW_getXTimeFromYTime (me, t) );
	const double ty = ( warpX ? DTW_getYTimeFromXTime (me, t) : t );
	const int lineType = Graphics_inqLineType (g);

	if (xmin >= xmax) {
		xmin = my xmin;
		xmax = my xmax;
	}
	if (ymin >= ymax) {
		ymin = my ymin;
		ymax = my ymax;
	}

	if (inset)
		Graphics_setInner (g);

	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	Graphics_setLineType (g, Graphics_DOTTED);
	if (ty <= ymax) {
		Graphics_line (g, tx, ymin, tx, ty);
		Graphics_line (g, tx, ty, xmin, ty);
	} else {
		Graphics_line (g, tx, ymin, tx, ymax);
	}

	Graphics_setLineType (g, lineType);

	if (inset)
		Graphics_unsetInner (g);

	if (garnish) {
		Graphics_markBottom (g, tx, true, true, false, nullptr);
		if (ty <= ymax)
			Graphics_markLeft (g, ty, true, true, false, nullptr);
	}
}

void DTW_drawWarpX (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, bool garnish) {
	DTW_drawWarp_raw (me, g, xmin, xmax, ymin, ymax, tx, garnish, true, true);
}

void DTW_drawWarpY (DTW me, Graphics g, double xmin, double xmax, double ymin, double ymax, double ty, bool garnish) {
	DTW_drawWarp_raw (me, g, xmin, xmax, ymin, ymax, ty, garnish, true, false);
}

static void DTW_Sounds_checkDomains (DTW me, Sound *y, Sound *x, double *xmin, double *xmax, double *ymin, double *ymax) {
	Melder_require ((my ymin == (*y) -> xmin && my ymax == (*y) -> xmax &&
		my xmin == (*x) -> xmin && my xmax == (*x) -> xmax) ||
		(my ymin == (*x) -> xmin && my ymax == (*x) -> xmax &&
		my xmin == (*y) -> xmin && my xmax == (*y) -> xmax),
			U"The domains of the DTW and the sound(s) should be equal.");

	if (*xmin >= *xmax) {
		*xmin = my xmin;
		*xmax = my xmax;
	}
	if (*ymin >= *ymax) {
		*ymin = my ymin;
		*ymax = my ymax;
	}
}

static void drawBox (Graphics g) {
	double x1WC, x2WC, y1WC, y2WC;
	const double lineWidth = Graphics_inqLineWidth (g);
	Graphics_inqWindow (g, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setLineWidth (g, 2.0 * lineWidth);
	Graphics_rectangle (g, x1WC, x2WC, y1WC, y2WC);
	Graphics_setLineWidth (g, lineWidth);
}

/*
  In a picture with a DTW and a left and bottom Sound, we want "width" of the vertical sound
  and the "height" of the horizontal Sound to be equal.
  Given the horizontal fraction of the DTW-part, this routine returns the vertical part.
*/
static double _DTW_Sounds_getPartY (Graphics g, double dtw_part_x) {
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (g, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	return 1.0 - (1.0 - dtw_part_x) * (x2NDC - x1NDC) / (y2NDC - y1NDC);
}

void DTW_Sounds_draw (DTW me, Sound y, Sound x, Graphics g, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	DTW_Sounds_checkDomains (me, & y, & x, & xmin, & xmax, & ymin, & ymax);

	Graphics_setInner (g);
	Graphics_Viewport ovp = g -> outerViewport; // save for unsetInner

	const double dtw_part_x = 0.85;
	const double dtw_part_y = _DTW_Sounds_getPartY (g, dtw_part_x);

	// DTW

	Graphics_Viewport vp = Graphics_insetViewport (g, 1.0 - dtw_part_x, 1.0, 1.0 - dtw_part_y, 1.0);
	DTW_paintDistances_raw (me, g, xmin, xmax, ymin, ymax, 0.0, 0.0, false, false);
	DTW_drawPath_raw (me, g, xmin, xmax, ymin, ymax, false, false);
	drawBox (g);
	Graphics_resetViewport (g, vp);

	// Sound y

	vp = Graphics_insetViewport (g, 0.0, 1.0 - dtw_part_x, 1.0 - dtw_part_y, 1.0);
	Sound_draw_btlr (y, g, ymin, ymax, -1.0, 1.0, kSoundDrawingDirection::BOTTOM_TO_TOP, false);
	if (garnish)
		drawBox (g);
	Graphics_resetViewport (g, vp);

	// Sound x

	vp = Graphics_insetViewport (g, 1 - dtw_part_x, 1, 0, 1 - dtw_part_y);
	Sound_draw_btlr (x, g, xmin, xmax, -1.0, 1.0, kSoundDrawingDirection::LEFT_TO_RIGHT, false);
	if (garnish)
		drawBox (g);
	Graphics_resetViewport (g, vp);


	// Set window coordinates so that margins will work, i.e. extend time domains

	const double xmin3 = xmax - (xmax - xmin) / dtw_part_x;
	const double ymin3 = ymax - (ymax - ymin) / dtw_part_y;
	Graphics_setWindow (g, xmin3, xmax, ymin3, ymax);

	g -> outerViewport = ovp; // restore from _setInner
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_markLeft (g, ymin, true, true, false, nullptr);
		Graphics_markLeft (g, ymax, true, true, false, nullptr);
		Graphics_markBottom (g, xmin, true, true, false, nullptr);
		Graphics_markBottom (g, xmax, true, true, false, nullptr);
	}
}

void DTW_Sounds_drawWarpX (DTW me, Sound yy, Sound xx, Graphics g, double xmin, double xmax, double ymin, double ymax, double tx, bool garnish)
{
	Sound y = yy, x = xx;
	const int lineType = Graphics_inqLineType (g);

	DTW_Sounds_checkDomains (me, & y, & x, & xmin, & xmax, & ymin, & ymax);

	Graphics_setInner (g);
	const double dtw_part_x = 0.85;
	const double dtw_part_y = _DTW_Sounds_getPartY (g, dtw_part_x);

	xmin = xmax - (xmax - xmin) / dtw_part_x;
	ymin = ymax - (ymax - ymin) / dtw_part_y;
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	const double ty = DTW_getYTimeFromXTime (me, tx);
	Graphics_setLineType (g, Graphics_DOTTED);

	Graphics_line (g, tx, ymin, tx, ty);
	Graphics_line (g, tx, ty, xmin, ty);

	Graphics_setLineType (g, lineType);

	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_markBottom (g, tx, true, true, false, nullptr);
		Graphics_markLeft (g, ty, true, true, false, nullptr);
	}
}

autoMatrix DTW_to_Matrix_distances (DTW me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		thy z.all()  <<=  my z.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": distances not converted to Matrix.");
	}
}

/* nog aanpassen, dl = sqrt (dx^2 + dy^2) */
void DTW_drawDistancesAlongPath (DTW me, Graphics g, double xmin, double xmax, double dmin, double dmax, bool garnish) {
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer ixmax, ixmin;
	if (Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax) == 0)
		return;

	integer ii = 1;
	while (ii < my pathLength && my path [ii]. x < ixmin)
		ii ++;
	ixmin = ii;

	while (ii <= my pathLength && my path [ii]. x < ixmax)
		ii ++;
	ixmax = ii;

	const integer numberOfSelected = ixmax - ixmin + 1;
	autoVEC d = raw_VEC (numberOfSelected);

	for (integer i = ixmin; i <= ixmax; i ++)
		d [i - ixmin + 1] = my z [my path [i]. y] [i];

	if (dmin >= dmax) {
		NUMextrema_u (d.get(), & dmin, & dmax);
		if (isundef (dmin) || isundef (dmax))
			return;
	} else
		VECclip_inplace (dmin, d.get(), dmax);

	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, dmin, dmax);
	Graphics_function (g, d.asArgumentToFunctionThatExpectsOneBasedArray(), 1, numberOfSelected, xmin, xmax);
	Graphics_unsetInner (g);

	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, true, U"distance");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

/*
	metric = 1...n (sum (a_i^n))^(1/n)
*/
autoDTW Matrices_to_DTW (Matrix me, Matrix thee, bool matchStart, bool matchEnd, int slope, double metric) {
	try {
		Melder_require (thy ny == my ny,
			U"Column sizes should be equal.");

		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoMelderProgress progess (U"Matrices to DTW: Calculate distances...");
		for (integer i = 1; i <= my nx; i ++) {
			for (integer j = 1; j <= thy nx; j ++) {
				/*
					First divide distance by maximum to prevent overflow when metric
					is a large number.
					d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would not overflow!
				*/
				double dmax = 0.0, d = 0.0;
				for (integer k = 1; k <= my ny; k ++) {
					const double dtmp = fabs (my z [k] [i] - thy z [k] [j]);
					if (dtmp > dmax)
						dmax = dtmp;
				}
				if (dmax > 0) {
					for (integer k = 1; k <= my ny; k ++) {
						const double dtmp = fabs (my z [k] [i] - thy z [k] [j]) / dmax;
						d +=  pow (dtmp, metric);
					}
				}
				d = dmax * pow (d, 1.0 / metric);
				his z [i] [j] = d / my ny; // == d * dy / ymax
			}
			if ((i % 10) == 1) {
				Melder_progress (0.999 * i / my nx, U"Calculate distances: column ", i, U" from ", my nx, U".");
			}
		}
		DTW_findPath (him.get(), matchStart, matchEnd, slope);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from matrices.");
	}
}

autoDTW Spectrograms_to_DTW (Spectrogram me, Spectrogram thee, bool matchStart, bool matchEnd, int slope, double metric) {
	try {
		Melder_require (my xmin == thy xmin && my ymax == thy ymax && my ny == thy ny,
			U"The number of frequencies and/or frequency ranges should be equal.");

		autoMatrix m1 = Spectrogram_to_Matrix (me);
		autoMatrix m2 = Spectrogram_to_Matrix (thee);

		/*
			Take log10 for dB's (4e-10 scaling not necessary)
		*/
		for (integer i = 1; i <= my ny; i ++) {
			for (integer j = 1; j <= my nx; j ++)
				m1 -> z [i] [j] = 10.0 * log10 (m1 -> z [i] [j]);
		}
		for (integer i = 1; i <= thy ny; i ++) {
			for (integer j = 1; j <= thy nx; j ++)
				m2 -> z [i] [j] = 10.0 * log10 (m2 -> z [i] [j]);
		}

		autoDTW him = Matrices_to_DTW (m1.get(), m2.get(), matchStart, matchEnd, slope, metric);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from Spectrograms.");
	}
}

static int Pitch_findFirstAndLastVoicedFrame (Pitch me, integer *first, integer *last) {
	*first = 1;
	while (*first <= my nx && ! Pitch_isVoiced_i (me, *first))
		(*first) ++;

	*last = my nx;
	while (*last >= *first && ! Pitch_isVoiced_i (me, *last))
		(*last) --;

	return *first <= my nx && *last >= 1;
}

autoDTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, bool matchStart, bool matchEnd, int slope);
autoDTW Pitches_to_DTW_sgc (Pitch me, Pitch thee, double vuv_costs, double time_weight, bool matchStart, bool matchEnd, int slope) { // vuv_costs=24, time_weight=10 ?
	try {
		Melder_require (vuv_costs >= 0.0,
			U"Voiced-unvoiced costs should not be negative.");
		Melder_require (time_weight >= 0.0,
			U"Time costs weight should not be negative.");

		integer myfirst, mylast, thyfirst, thylast;
		Melder_require (Pitch_findFirstAndLastVoicedFrame (me, & myfirst, & mylast) && Pitch_findFirstAndLastVoicedFrame (thee, & thyfirst, & thylast),
			U"No voiced frames.");
		
		/*
			We do not want the silences before the first voiced frame and after the last voiced frame
			to determine the distances.
			We create paths from (1,1)...(thyfirst,myfirst) and (thylast,mylast)...(thy nx,my nx)
			by making the other cell's distances very large.
		*/
		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoVEC pitchx = raw_VEC (thy nx);
		kPitch_unit unit = kPitch_unit::SEMITONES_100;
		for (integer j = 1; j <= thy nx; j ++)
			pitchx [j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, (int) unit);

		for (integer i = 1; i <= my nx; i ++) {
			const double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, (int) unit);
			const double t1 = my x1 + (i - 1) * my dx;
			for (integer j = 1; j <= thy nx; j ++) {
				const double t2 = thy x1 + (j - 1) * thy dx;
				const double dist_t = fabs (t1 - t2);
				double dist_f = 0.0;   // based on pitch difference
				if (isundef (pitchy)) {
					if (isdefined (pitchx [j]))
						dist_f = vuv_costs;
				} else if (isundef (pitchx [j])) {
					dist_f = vuv_costs;
				} else {
					dist_f = fabs (pitchy - pitchx [j]);
				}
				his z [i] [j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
			}
		}
		DTW_findPath (him.get(), matchStart, matchEnd, slope);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from Pitches.");
	}
}

autoDTW Pitches_to_DTW (Pitch me, Pitch thee, double vuv_costs, double time_weight, bool matchStart, bool matchEnd, int slope) { // vuv_costs=24, time_weight=10 ?
	try {
		Melder_require (vuv_costs >= 0.0,
			U"Voiced-unvoiced costs should not be negative.");
		Melder_require (time_weight >= 0.0,
			U"Time costs weight should not be negative.");

		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoVEC pitchx = raw_VEC (thy nx);
		kPitch_unit unit = kPitch_unit::SEMITONES_100;
		for (integer j = 1; j <= thy nx; j ++)
			pitchx [j] = Sampled_getValueAtSample (thee, j, Pitch_LEVEL_FREQUENCY, (int) unit);

		for (integer i = 1; i <= my nx; i ++) {
			const double pitchy = Sampled_getValueAtSample (me, i, Pitch_LEVEL_FREQUENCY, (int) unit);
			const double t1 = my x1 + (i - 1) * my dx;
			for (integer j = 1; j <= thy nx; j ++) {
				const double t2 = thy x1 + (j - 1) * thy dx;
				const double dist_t = fabs (t1 - t2);
				double dist_f = 0; // based on pitch difference
				if (isundef (pitchy)) {
					if (isdefined (pitchx [j]))
						dist_f = vuv_costs;
				} else if (isundef (pitchx [j])) {
					dist_f = vuv_costs;
				} else {
					dist_f = fabs (pitchy - pitchx [j]);
				}
				his z [i] [j] = sqrt (dist_f * dist_f + time_weight * dist_t * dist_t);
			}
		}

		DTW_findPath (him.get(), matchStart, matchEnd, slope);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created from Pitches.");
	}
}

autoDurationTier DTW_to_DurationTier (DTW /* me */) {
	return autoDurationTier();
}

void DTW_Matrix_replace (DTW me, Matrix thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax && my ymin == thy ymin && my ymax == thy ymax,
			U"The X and Y domains of the matrix and the DTW must be equal.");
		Melder_require (my nx == thy nx && my dx == thy dx && my ny == thy ny && my dy == thy dy,
			U"The sampling of the matrix and the DTW should be equal.");
		double minimum, maximum;
		Matrix_getWindowExtrema (me, 0, 0, 0, 0, & minimum, & maximum);
		Melder_require (minimum >= 0.0,
			U"Distances should not be negative.");
		my z.all()  <<=  thy z.all();
	} catch (MelderError) {
		Melder_throw (me, U": distances not replaced.");
	}
}

/****************** new implementation ********/

void DTW_findPath (DTW me, bool matchStart, bool matchEnd, int slope) {
    DTW_findPath_special (me, matchStart, matchEnd, slope, nullptr);
}

autoMatrix DTW_to_Matrix_cumulativeDistances (DTW me, double sakoeChibaBand, int slope) {
    try {
        autoMatrix cumulativeDistances;
        DTW_findPath_bandAndSlope (me, sakoeChibaBand, slope, & cumulativeDistances);
        return cumulativeDistances;
    } catch (MelderError) {
        Melder_throw (me, U": cumulative costs matrix not created.");
    }
}

static void DTW_relaxConstraints (DTW me, double band, int /* slope */, double *relaxedBand, int *relaxedSlope) {

	//double dtw_slope = (my ymax - my ymin - band) / (my xmax - my xmin - band);
	//dtw_slope = dtw_slope+1.0; // fake instruction to avoid compiler warning
	*relaxedBand = 0.0;
	*relaxedSlope = 1;
}

static void DTW_checkSlopeConstraints (DTW me, double band, int slope) {
    try {
        const double slopes [5] = { DTW_BIG, DTW_BIG, 3.0, 2.0, 1.5 } ;
        double dtw_slope = (my ymax - my ymin - band) / (my xmax - my xmin - band);
		Melder_require (slope > 0 && slope < 5,
			U"Invalid slope constraint.");
		Melder_require (! (dtw_slope == 0.0 && slope != 1),
			U"Band too wide.");

        if (dtw_slope < 1.0)
            dtw_slope = 1.0 / dtw_slope;

        Melder_require (dtw_slope <= slopes [slope],
			U"There is a conflict between the chosen slope constraint and the relative duration. "
			U"The duration ratio of the longest and the shortest object is ", dtw_slope,
			U". This implies that the largest slope in the constraint must have a value greater than or equal to this ratio."
		);
    } catch (MelderError) {
        Melder_throw (U"Slope constraints cannot be met.");
    }
}

static void DTW_Polygon_setUnreachableParts (DTW me, Polygon thee, INTMAT const& psi) {
    try {
        const double eps = my dx / 100.0;   // safe enough
        const double dtw_slope = (my ymax - my ymin) / (my xmax - my xmin);

        double xmin, xmax, ymin, ymax;
        Polygon_getExtrema (thee, & xmin, & xmax, & ymin, & ymax);
        // if the Polygon and the DTW don't overlap everything is unreachable!
		Melder_require (! (xmax <= my xmin || xmin >= my xmax || ymax <= my ymin || ymin >= my ymax),
			U"DTW and Polygon don't overlap.");

        // find border "above" polygon
        for (integer ix = 1; ix <= my nx; ix ++) {
            const double x = my x1 + (ix - 1) * my dx;
            const integer iystart = Melder_ifloor (dtw_slope * ix * (my dx / my dy)) + 1;
            for (integer iy = iystart + 1; iy <= my ny; iy ++) {
				const double y = my y1 + (iy - 1) * my dy;
                if (Polygon_getLocationOfPoint (thee, x, y, eps) == Polygon_OUTSIDE) {
                    for (integer k = iy; k <= my ny; k ++)
                        psi [k] [ix] = DTW_UNREACHABLE;
                    break;
                }
            }
        }
        // find border "below" polygon
        for (integer ix = 2; ix <= my nx; ix ++) {
            const double x = my x1 + (ix - 1) * my dx;
            integer iystart = Melder_ifloor (dtw_slope * ix * (my dx / my dy));   // start 1 lower
            if (iystart > my ny)
				iystart = my ny;
            for (integer iy = iystart - 1; iy >= 1; iy --) {
                const double y = my y1 + (iy - 1) * my dy;
                if (Polygon_getLocationOfPoint (thee, x, y, eps) == Polygon_OUTSIDE) {
                    for (integer k = iy; k >= 1; k --)
                        psi [k] [ix] = DTW_UNREACHABLE;
                    break;
                }
            }
        }
    } catch (MelderError) {
        Melder_throw (me, U" cannot set unreachable parts.");
    }
}

#define DTW_ISREACHABLE(y,x) ((psi [y] [x] != DTW_UNREACHABLE) && (psi [y] [x] != DTW_FORBIDDEN))
static void DTW_findPath_special (DTW me, bool /* matchStart */, bool /* matchEnd */, int slope, autoMatrix *cumulativeDists) {
	try {
		autoPolygon thee = DTW_to_Polygon (me, 0.0, slope);
		DTW_Polygon_findPathInside (me, thee.get(), slope, cumulativeDists);
	} catch (MelderError) {
		Melder_throw (me, U": cannot find path.");
	}
}

/*
	Intersection of two straight lines y = a [i]*x+b [i], where a [2] = 1 / a [1]. Point (x1,y1) is on first line,
	point (x2,y2) is on second line.
*/
static void getIntersectionPoint (double x1, double y1, double x2, double y2, double a, double *x3, double *y3) {
	*x3 = (y2 - y1 + a * x1 - x2 / a) / (a - 1.0 / a);
	*y3 = a * *x3 + y1 - a * x1;
}

autoPolygon DTW_to_Polygon (DTW me, double band, int slope) {
    try {
		try {
			DTW_checkSlopeConstraints (me, band, slope);
		} catch (MelderError) {
			DTW_relaxConstraints (me, band, slope, & band, & slope);
			Melder_flushError ();
		}
			
        double slopes [5] = { DTW_BIG, DTW_BIG, 3.0, 2.0, 1.5 } ;
        if (band <= 0) {
            if (slope == 1) {
                autoPolygon thee = Polygon_create (4);
                thy x [1] = my xmin;
				thy y [1] = my ymin;
                thy x [2] = my xmin;
				thy y [2] = my ymax;
                thy x [3] = my xmax;
				thy y [3] = my ymax;
                thy x [4] = my xmax;
				thy y [4] = my ymin;
                return thee;
            } else {
                autoPolygon thee = Polygon_create (4);
                thy x [1] = my xmin;
				thy y [1] = my ymin;
                thy x [3] = my xmax;
				thy y [3] = my ymax;
                double x, y;
                getIntersectionPoint (my xmin, my ymin, my xmax, my ymax, slopes [slope], & x, & y);
                if (x < my xmin)
					x = my xmin;
                if (x > my xmax)
					x = my xmax;
                if (y < my ymin)
					y = my ymin;
                if (y > my ymax)
					y = my ymax;
                thy x [2] = x;
                thy y [2] = y;
                getIntersectionPoint (my xmin, my ymin, my xmax, my ymax, 1.0 / slopes [slope], & x, & y);
                if (x < my xmin)
					x = my xmin;
                if (x > my xmax)
					x = my xmax;
                if (y < my ymin)
					y = my ymin;
                if (y > my ymax)
					y = my ymax;
                thy x [4] = x;
                thy y [4] = y;
                return thee;
            }
        } else {
            if (slope == 1) {
                autoPolygon thee = Polygon_create (6);
                thy x [1] = my xmin;
				thy y [1] = my ymin;
                thy x [2] = my xmin;
				thy y [2] = my ymin + band;
                thy x [3] = my xmax - band;
				thy y [3] = my ymax;
                thy x [4] = my xmax;
				thy y [4] = my ymax;
                thy x [5] = my xmax;
				thy y [5] = my ymax - band;
                thy x [6] = my xmin + band;
				thy y [6] = my ymin;
                return thee;
            } else {
                autoPolygon thee = Polygon_create (8);
                double x, y;
                thy x [1] = my xmin;
				thy y [1] = my ymin;
                thy x [2] = my xmin;
				thy y [2] = my ymin + band;
                getIntersectionPoint (my xmin, my ymin + band, my xmax - band, my ymax, slopes [slope], & x, & y);
                if (x < my xmin)
					x = my xmin;
                if (x > my xmax)
					x = my xmax;
                if (y < my ymin)
					y = my ymin;
                if (y > my ymax)
					y = my ymax;
                thy x [3] = x;
                thy y [3] = y;
                thy x [4] = my xmax - band;
				thy y [4] = my ymax;
                thy x [5] = my xmax;
				thy y [5]= my ymax;
                thy x [6] = my xmax;
				thy y [6] = my ymax - band;
                getIntersectionPoint (my xmin + band, my ymin, my xmax, my ymax - band, 1.0 / slopes [slope], & x, & y);
                if (x < my xmin)
					x = my xmin;
                if (x > my xmax)
					x = my xmax;
                if (y < my ymin)
					y = my ymin;
                if (y > my ymax)
					y = my ymax;
                thy x [7] = x;
                thy y [7] = y;
                thy x [8] = my xmin + band;
				thy y [8] = my ymin;
                return thee;
            }
        }
    } catch (MelderError) {
        Melder_throw (me, U" no Polygon created.");
    }
}

autoMatrix DTW_Polygon_to_Matrix_cumulativeDistances (DTW me, Polygon thee, int localSlope) {
    try {
        autoMatrix cumulativeDistances;
        DTW_Polygon_findPathInside (me, thee, localSlope, & cumulativeDistances);
        return cumulativeDistances;
    } catch (MelderError) {
        Melder_throw (me, U": cumulative costs matrix not created from DTW and Polygon.");
    }
}

void DTW_findPath_bandAndSlope (DTW me, double sakoeChibaBand, int localSlope, autoMatrix *cumulativeDists) {
    try {
        autoPolygon thee = DTW_to_Polygon (me, sakoeChibaBand, localSlope);
        DTW_Polygon_findPathInside (me, thee.get(), localSlope, cumulativeDists);
    } catch (MelderError) {
        Melder_throw (me, U" cannot determine the path.");
    }
}

void DTW_Polygon_findPathInside (DTW me, Polygon thee, int localSlope, autoMatrix *cumulativeDists) {
	try {
		const double slopes [5] = { DTW_BIG, DTW_BIG, 3.0, 2.0, 1.5 };
		// if localSlope == 1 start of path is within 10% of minimum duration. Starts farther away
		integer delta_xy = std::min (my nx, my ny) / 10; // if localSlope == 1 start within 10% of

		Melder_require (localSlope > 0 && localSlope < 5,
			U"Local slope parameter ", localSlope, U" not supported.");

		autoMAT delta = copy_MAT (my z.get());
		autoINTMAT psi = zero_INTMAT (my ny, my nx);
		/*
			Start by making the outside unreachable.
		*/
		for (integer j = 1; j <= my nx; j ++)
			psi [1] [j] = DTW_UNREACHABLE;
		for (integer i = 1; i <= my ny; i ++)
			psi [i] [1] = DTW_UNREACHABLE;

		/*
			Make begin part of first column reachable.
		*/
		const integer rowto = ( localSlope != 1 ? Melder_ifloor (slopes [localSlope]) + 1 : delta_xy );
		for (integer iy = 2; iy <= rowto; iy ++) {
			if (localSlope != 1) {
				delta [iy] [1] = delta [iy - 1] [1] + my z [iy] [1];
				psi [iy] [1] = DTW_Y;
			} else {
				psi [iy] [1] = DTW_START;   // will be adapted by DTW_Polygon_setUnreachableParts
			}
		}
		/*
			Make begin part of first row reachable.
		*/
		const integer colto = ( localSlope != 1 ? Melder_ifloor (slopes [localSlope]) + 1 : delta_xy );
		for (integer ix = 2; ix <= colto; ix ++) {
			if (localSlope != 1) {
				delta [1] [ix] = delta [1] [ix -1] + my z [1] [ix];
				psi [1] [ix] = DTW_X;
			} else {
				psi [1] [ix] = DTW_START;   // will be adapted by DTW_Polygon_setUnreachableParts
			}
		}

		/*
			Now we can set the unreachable parts from the Polygon.
		*/
		DTW_Polygon_setUnreachableParts (me, thee, psi.get());

		/*
			Forward pass.
		*/
		integer numberOfIsolatedPoints = 0;
		autoMelderProgress progress (U"DTW & Polygon: Find path...");
		for (integer j = 2; j <= my nx; j ++) {
			for (integer i = 2; i <= my ny; i ++) {
				if (! DTW_ISREACHABLE (i, j))
					continue;
				double g, gmin = DTW_BIG;
				integer direction = 0;
				if (DTW_ISREACHABLE (i - 1, j - 1)) {
					gmin = delta [i - 1] [j - 1] + 2.0 * my z [i] [j];
					direction = DTW_XANDY;
				} else if (DTW_ISREACHABLE (i, j - 1)) {
					gmin = delta [i] [j - 1] + my z [i] [j];
					direction = DTW_X;
				} else if (DTW_ISREACHABLE (i - 1, j)) {
					gmin = delta [i - 1] [j] + my z [i] [j];
					direction = DTW_Y;
				} else {
					numberOfIsolatedPoints ++;
					continue;
				}

				switch (localSlope) {
				case 1:  {   // no restriction
					if (DTW_ISREACHABLE (i, j - 1) && ((g = delta [i] [j - 1] + my z [i] [j]) < gmin)) {
						gmin = g;
						direction = DTW_X;
					}
					if (DTW_ISREACHABLE (i - 1, j) && ((g = delta [i - 1] [j] + my z [i] [j]) < gmin)) {
						gmin = g;
						direction = DTW_Y;
					}
				}
				break;

				/*
					Sakoe & Chiba (1978) define the slope constraint measure as P = n / m, 
						where n is the number of steps in the diagonal and 
						m the number of steps in one of the other directions.
						
					P = 1/2
				*/
				case 2: {   // P = 1/2
					if (j >= 4 && DTW_ISREACHABLE (i - 1, j - 3) && psi [i] [j - 1] == DTW_X && psi [i] [j - 2] == DTW_XANDY &&
						(g = delta [i-1] [j-3] + 2.0 * my z [i] [j-2] + my z [i] [j-1] + my z [i] [j]) < gmin) {
						gmin = g;
						direction = DTW_X;
					}
					if (j >= 3 && DTW_ISREACHABLE (i - 1, j - 2) && psi [i] [j - 1] == DTW_XANDY &&
						(g = delta [i - 1] [j - 2] + 2.0 * my z [i] [j - 1] + my z [i] [j]) < gmin) {
						gmin = g;
						direction = DTW_X;
					}
					if (i >= 3 && DTW_ISREACHABLE (i - 2, j - 1) && psi [i - 1] [j] == DTW_XANDY &&
						(g = delta [i - 2] [j - 1] + 2.0 * my z [i - 1] [j] + my z [i] [j]) < gmin) {
						gmin = g;
						direction = DTW_Y;
					}
					if (i >= 4 && DTW_ISREACHABLE (i - 3, j - 1) && psi [i - 1] [j] == DTW_Y && psi [i - 2] [j] == DTW_XANDY &&
						(g = delta [i-3] [j-1] + 2.0 * my z [i-2] [j] + my z [i-1] [j] + my z [i] [j]) < gmin) {
						gmin = g;
						direction = DTW_Y;
					}
				}
				break;

				// P = 1

				case 3: {
					if (j >= 3 && DTW_ISREACHABLE (i - 1, j - 2) && psi [i] [j - 1] == DTW_XANDY &&
							(g = delta [i - 1] [j - 2] + 2.0 * my z [i] [j - 1] + my z [i] [j]) < gmin)
					{
						gmin = g;
						direction = DTW_X;
					}
					if (i >= 3 && DTW_ISREACHABLE (i - 2, j - 1) && psi [i - 1] [j] == DTW_XANDY &&
							(g = delta [i - 2] [j - 1] + 2.0 * my z [i - 1] [j] + my z [i] [j]) < gmin)
					{
						gmin = g;
						direction = DTW_Y;
					}
				}
				break;

                // P = 2

				case 4: {
					if (i >= 3 && j >= 4 && DTW_ISREACHABLE (i - 2, j - 3) && psi [i] [j - 1] == DTW_XANDY && psi [i - 1] [j - 2] == DTW_XANDY &&
							(g = delta [i-2] [j-3] + 2.0 * my z [i-1] [j-2] + 2.0 * my z [i] [j-1] + my z [i] [j]) < gmin)
					{
						gmin = g;
						direction = DTW_X;
					}
					if (i >= 4 && j >= 3 && DTW_ISREACHABLE (i - 3, j - 2) && psi [i - 1] [j] == DTW_XANDY && psi [i - 2] [j - 1] == DTW_XANDY &&
							(g = delta [i-3] [j-2] + 2.0 * my z [i-2] [j-1] + 2.0 * my z [i-1] [j] + my z [i] [j]) < gmin)
					{
						gmin = g;
						direction = DTW_Y;
					}
				}
				break;
				default:
				break;
				}
				Melder_assert (direction != 0);
				psi [i] [j] = direction;
				delta [i] [j] = gmin;
			}
			if (j % 10 == 2)
				Melder_progress (0.999 * j / my nx, U"Calculate time warp: frame ", j, U" from ", my nx, U".");
		}

		/*
			Find minimum at end of path and trace back.
		*/
		integer iy = my ny;
		double minimum = delta [iy] [my nx];
		for (integer i = my ny - 1; i > 0; i --) {
			if (! DTW_ISREACHABLE (i, my nx)) {
				break;   // we're in unreachable places
			} else if (delta [i] [my nx] < minimum) {
				minimum = delta [iy = i] [my nx];
			}
		}

		integer pathIndex = my nx + my ny - 1;   // maximum path length
		my weightedDistance = minimum / (my nx + my ny);
		my path [pathIndex]. y = iy;
		integer ix = my path [pathIndex]. x = my nx;

		/*
			Fill path backwards.
		*/
		while (ix > 1) {
			if (psi [iy] [ix] == DTW_XANDY) {
				ix --;
				iy --;
			} else if (psi [iy] [ix] == DTW_X) {
				ix --;
			} else if (psi [iy] [ix] == DTW_Y) {
				iy --;
			} else if (psi [iy] [ix] == DTW_START) {
				break;
			}
			if (pathIndex < 2 || iy < 1)
				break;
			//Melder_assert (pathIndex > 1 && iy > 0);
			my path [-- pathIndex]. x = ix;
			my path [pathIndex]. y = iy;
		}

		my pathLength = my nx + my ny - 1 - pathIndex + 1;
		if (pathIndex > 1)
			for (integer j = 1; j <= my pathLength; j ++)
				my path [j] = my path [pathIndex ++];
		my path.resize (my pathLength);   // maintain invariant
		DTW_Path_recode (me);
		if (cumulativeDists) {
			autoMatrix him = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
					my ymin, my ymax, my ny, my dy, my y1);
			his z.all()  <<=  delta.all();
			*cumulativeDists = him.move();
		}
	} catch (MelderError) {
		Melder_throw (me, U": cannot find path.");
	}
}

/* End of file DTW.cpp */
