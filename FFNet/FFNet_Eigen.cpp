/* FFNet_Eigen.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
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

/*
 djmw 20020712 GPL header
 djmw 20071202 Melder_warning<n>
*/

#include "FFNet_Eigen.h"
#include "Graphics.h"
#include "NUM2.h"

void FFNet_Eigen_drawIntersection (FFNet me, Eigen eigen, Graphics g, integer pcx, integer pcy, double xmin, double xmax, double ymin, double ymax) {
	Melder_assert (pcx > 0 && pcy > 0);
	if (pcx > eigen -> numberOfEigenvalues || pcy > eigen -> numberOfEigenvalues || my numberOfInputs != eigen -> dimension)
		return;

	double x1, x2, y1, y2;
	if (xmax <= xmin || ymax <= ymin)
		Graphics_inqWindow (g, & x1, & x2, & y1, & y2);

	if (xmax <= xmin) {
		xmin = x1;
		xmax = x2;
	}
	if (ymax <= ymin) {
		ymin = y1;
		ymax = y2;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	for (integer i = 1; i <= my numberOfUnitsInLayer [1]; i ++) {
		const integer unitOffset = my numberOfInputs + 1;
		double c1 = 0.0, c2 = 0.0;
		for (integer j = 1; j <= my numberOfInputs; j ++) {
			c1 += my w [my wFirst [unitOffset + i] + j - 1] * eigen -> eigenvectors [pcx] [j];
			c2 += my w [my wFirst [unitOffset + i] + j - 1] * eigen -> eigenvectors [pcy] [j];
		}
		double x [6], y [6], xs [3], ys [3];
		x [1] = x [2] = x [5] = xmin;
		x [3] = x [4] = xmax;
		y [1] = y [4] = y [5] = ymin;
		y [2] = y [3] = ymax;
		integer ns = 0;
		const double bias = my w [my wLast [unitOffset + i]];
		for (integer j = 1; j <= 4; j++) {
			const double p1 = c1 * x [j] + c2 * y [j] + bias;
			const double p2 = c1 * x [j + 1] + c2 * y [j + 1] + bias;
			const double r = fabs (p1) / (fabs (p1) + fabs (p2));
			if (p1 *p2 > 0 || r == 0.0) 
				continue;
			if (++ ns > 2)
				break;
			xs [ns] = x [j] + (x [j + 1] - x [j]) * r;
			ys [ns] = y [j] + (y [j + 1] - y [j]) * r;
		}
		if (ns < 2)
			Melder_casual (U"Intersection for unit ", i, U" outside range");
		else
			Graphics_line (g, xs [1], ys [1], xs [2], ys [2]);
	}
	Graphics_unsetInner (g);
}

/*
	Draw the intersection line of the decision hyperplane 'w.e-b' of the weights of unit i
	from layer j with the plane spanned by eigenvectors pcx and pcy.
*/
void FFNet_Eigen_drawDecisionPlaneInEigenspace (FFNet me, Eigen thee, Graphics g, integer unit, integer layer,
	integer pcx, integer pcy, double xmin, double xmax, double ymin, double ymax) {
	Melder_assert (layer > 0);
	Melder_assert (unit > 0);
	Melder_assert (pcx > 0 && pcy > 0);
	if (layer > my numberOfLayers || unit > my numberOfUnitsInLayer [layer] || pcx > thy numberOfEigenvalues || pcy > thy numberOfEigenvalues)
		return;
	const integer numberOfUnitsInLayer_m1 = ( layer == 1 ? my numberOfInputs : my numberOfUnitsInLayer [layer - 1] );
	if (numberOfUnitsInLayer_m1 != thy dimension)
		return;

	double x1, x2, y1, y2;
	Graphics_inqWindow (g, & x1, & x2, & y1, & y2);
	if (xmax <= xmin) {
		xmin = x1;
		xmax = x2;
	}
	if (ymax <= ymin) {
		ymin = y1;
		ymax = y2;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);

	const integer node = FFNet_getNodeNumberFromUnitNumber (me, unit, layer);
	if (node < 1)
		return;

	/*
		Suppose p1 and p2 are the two points in the eigenplane, spanned by the eigenvectors
		e1 and e2, where the neural net decision hyperplane intersects these eigenvectors.
		Their coordinates in the eigenplane will be (x0*e1, 0) and (0,y0*e2).
		At the same time, the two points are part of the decision hyperplane of the
		chosen unit. The hyperplane equation is:
			w.e+bias = 0,
		where 'w' is the weight vector, 'e' is the input vector and 'b' is the bias.
		This results in two equations for the unknown x0 and y0:
			w.(x0*e1)+bias = 0
			w.(y0*e2)+bias = 0
		This suggests the solution for x0 and y0:

			x0 = -bias / (w.e1)
			y0 = -bias / (w.e2)

		If w.e1 != 0 && w.e2 != 0
		 	p1 = (x0, 0) and p2 = (0, y0)
		If w.e1 == 0 && w.e2 != 0
			The line is parallel to e1 and intersects e2 at y0.
		If w.e2 == 0 && w.e1 != 0
			The line is parallel to e2 and intersects e1 at x0.
		If w.e1 == 0 && w.e2 == 0
			Both planes are parallel, no intersection.
	*/

	const integer iw = my wFirst [node] - 1;
	double we1 = 0.0, we2 = 0.0;
	for (integer i = 1; i <= numberOfUnitsInLayer_m1; i ++) {
		we1 += my w [iw + i] * thy eigenvectors [pcx] [i];
		we2 += my w [iw + i] * thy eigenvectors [pcy] [i];
	}

	const double bias = my w [my wLast [node]];
	x1 = xmin;
	x2 = xmax;
	y1 = ymin;
	y2 = ymax;
	if (we1 != 0.0) {
		x1 = -bias / we1;
		y1 = 0.0;
	}
	if (we2 != 0.0) {
		x2 = 0.0;
		y2 = -bias / we2;
	}
	if (we1 == 0.0 && we2 == 0.0) {
		Melder_warning (U"We cannot draw the intersection of the neural net decision plane\n"
		    "for unit ", unit, U" in layer ", layer, U" with the plane spanned by the eigenvectors because \nboth planes are parallel.");
		return;
	}
	double xi [4], yi [4]; // Intersections
	const double ni = NUMgetIntersectionsWithRectangle (x1, y1, x2, y2, xmin, ymin, xmax, ymax, xi, yi);
	if (ni == 2)
		Graphics_line (g, xi [1], yi [1], xi [2], yi [2]);
	else
		Melder_warning (U"There were no intersections in the drawing area.\nPlease enlarge the drawing area.");

	Graphics_unsetInner (g);
}

/* End of file FFNet_Eigen.cpp */
