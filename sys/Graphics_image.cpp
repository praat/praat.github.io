/* Graphics_image.cpp
 *
 * Copyright (C) 1992-2021,2024-2026 Paul Boersma
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

#include "../fon/Photo.h"

#if gdi
	#include <gdiplus.h>
#elif quartz
	#include <time.h>
	#include "macport_on.h"
	static void _mac_releaseDataCallback (void *info, const void *data, size_t size) {
		(void) info;
		(void) size;
		Melder_free (data);
	}
#endif

#define wdx(x)  ((x) * my scaleX + my deltaX)
#define wdy(y)  ((y) * my scaleY + my deltaY)

static void _GraphicsScreen_cellArrayOrImage (GraphicsScreen me,
	constMATVU const& z_float, constmatrixview <MelderColour> const& z_rgbt, constmatrixview <unsigned char> const& z_byte,
	integer ix1, integer ix2, integer x1DC, integer x2DC,
	integer iy1, integer iy2, integer y1DC, integer y2DC,
	double minimum, double maximum,
	integer clipx1, integer clipx2, integer clipy1, integer clipy2, bool interpolate)
{
	/*double t=Melder_clock();*/
	integer nx = ix2 - ix1 + 1;   /* The number of cells along the horizontal axis. */
	integer ny = iy2 - iy1 + 1;   /* The number of cells along the vertical axis. */
	double dx = (double) (x2DC - x1DC) / (double) nx;   /* Horizontal pixels per cell. Positive. */
	double dy = (double) (y2DC - y1DC) / (double) ny;   /* Vertical pixels per cell. Negative. */
	double scale = 255.0 / (maximum - minimum), offset = 255.0 + minimum * scale;
	if (x2DC <= x1DC || y1DC <= y2DC) return;
	trace (U"scale ", scale);
	/* Clip by the intersection of the world window and the outline of the cells. */
	//Melder_casual (U"clipy1 ", clipy1, U" clipy2 ", clipy2);
	if (clipx1 < x1DC) clipx1 = x1DC;
	if (clipx2 > x2DC) clipx2 = x2DC;
	if (clipy1 > y1DC) clipy1 = y1DC;
	if (clipy2 < y2DC) clipy2 = y2DC;
	/*
	 * The first decision is whether we are going to use the standard rectangle drawing
	 * (cellArray only), or whether we are going to write into a bitmap.
	 * The standard drawing is best for small numbers of cells,
	 * provided that some cells are larger than a pixel.
	 */
	if (! interpolate && nx * ny < 3000 && (dx > 1.0 || dy < -1.0)) {
		try {
			/*unsigned int cellWidth = (unsigned int) dx + 1;*/
			unsigned int cellHeight = (unsigned int) (- (int) dy) + 1;
			integer ix, iy;
			#if cairo
				cairo_pattern_t *grey [256];
				for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey ++) {
					double v = igrey / ((double) (sizeof (grey) / sizeof (*grey)) - 1.0);
					grey [igrey] = cairo_pattern_create_rgb (v, v, v);
				}
			#elif gdi
				static HBRUSH greyBrush [256];
				RECT rect;
				if (! greyBrush [0])
					for (int igrey = 0; igrey <= 255; igrey ++)
						greyBrush [igrey] = CreateSolidBrush (RGB (igrey, igrey, igrey));   // once
			#elif quartz
				CGContextSetAlpha (my d_macGraphicsContext, 1.0);
				CGContextSetBlendMode (my d_macGraphicsContext, kCGBlendModeNormal);
			#endif
			autoINTVEC leftsBuffer = zero_INTVEC (ix2 - ix1 + 2);
			integer *lefts = & leftsBuffer [1 - ix1];
			for (ix = ix1; ix <= ix2 + 1; ix ++)
				lefts [ix] = x1DC + (integer) ((ix - ix1) * dx);
			for (iy = iy1; iy <= iy2; iy ++) {
				integer bottom = y1DC + (integer) ((iy - iy1) * dy), top = bottom - cellHeight;
				if (top > clipy1 || bottom < clipy2)
					continue;
				Melder_clipLeft (clipy2, & top);
				Melder_clipRight (& bottom, clipy1);
				#if gdi
					rect. bottom = bottom;
					rect. top = top;
				#endif
				for (ix = ix1; ix <= ix2; ix ++) {
					integer left = lefts [ix], right = lefts [ix + 1];
					if (right < clipx1 || left > clipx2)
						continue;
					Melder_clipLeft (clipx1, & left);
					Melder_clipRight (& right, clipx2);
					if (! NUMisEmpty (z_rgbt)) {
						#if cairo
							// NYI
						#elif gdi
							// NYI
						#elif quartz
							double red          = z_rgbt [iy] [ix]. red;
							double green        = z_rgbt [iy] [ix]. green;
							double blue         = z_rgbt [iy] [ix]. blue;
							double transparency = z_rgbt [iy] [ix]. transparency;
							Melder_clip (0.0, & red,   1.0);
							Melder_clip (0.0, & green, 1.0);
							Melder_clip (0.0, & blue,  1.0);
							CGContextSetRGBFillColor (my d_macGraphicsContext, red, green, blue, 1.0 - transparency);
							CGContextFillRect (my d_macGraphicsContext, CGRectMake (left, top, right - left, bottom - top));
						#endif
					} else {
						#if cairo
							integer value = offset - scale * ( ! NUMisEmpty (z_float) ? z_float [iy] [ix] : z_byte [iy] [ix] );
							cairo_set_source (my d_cairoGraphicsContext, grey [value <= 0 ? 0 : value >= sizeof (grey) / sizeof (*grey) ? sizeof (grey) / sizeof (*grey) : value]);
							cairo_rectangle (my d_cairoGraphicsContext, left, top, right - left, bottom - top);
							cairo_fill (my d_cairoGraphicsContext);
						#elif gdi
							integer value = offset - scale * ( ! NUMisEmpty (z_float) ? z_float [iy] [ix] : z_byte [iy] [ix] );
							rect. left = left; rect. right = right;
							FillRect (my d_gdiGraphicsContext, & rect, greyBrush [value <= 0 ? 0 : value >= 255 ? 255 : value]);
						#elif quartz
							double value = offset - scale * ( ! NUMisEmpty (z_float) ? z_float [iy] [ix] : z_byte [iy] [ix] );
							double igrey = ( value <= 0 ? 0 : value >= 255 ? 255 : value ) / 255.0;
							CGContextSetRGBFillColor (my d_macGraphicsContext, igrey, igrey, igrey, 1.0);
							CGContextFillRect (my d_macGraphicsContext, CGRectMake (left, top, right - left, bottom - top));
						#endif
					}
				}
			}
			
			#if cairo
				for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey ++)
					cairo_pattern_destroy (grey [igrey]);
			#elif quartz
				CGContextSetRGBFillColor (my d_macGraphicsContext, 0.0, 0.0, 0.0, 1.0);
			#endif
		} catch (MelderError) { }
	} else {
		integer xDC, yDC;
		integer undersampling = 1;
		/*
		 * Prepare for off-screen bitmap drawing.
		 */
		#if cairo
			integer arrayWidth = clipx2 - clipx1;
			integer arrayHeight = clipy1 - clipy2;
			trace (U"arrayWidth ", arrayWidth, U", arrayHeight ", arrayHeight);
			cairo_surface_t *sfc = cairo_image_surface_create (CAIRO_FORMAT_RGB24, arrayWidth, arrayHeight);
			unsigned char *bits = cairo_image_surface_get_data (sfc);
			int scanLineLength = cairo_image_surface_get_stride (sfc);
			unsigned char grey [256];
			trace (
				U"image surface address ", Melder_pointer (sfc),
				U", bits address ", Melder_pointer (bits),
				U", scanLineLength ", scanLineLength,
				U", numberOfGreys ", sizeof (grey) / sizeof (*grey)
			);
			for (int igrey = 0; igrey < sizeof (grey) / sizeof (*grey); igrey++)
				grey [igrey] = 255 - (unsigned char) (igrey * 255.0 / (sizeof (grey) / sizeof (*grey) - 1));
		#elif gdi
			integer bitmapWidth = clipx2 - clipx1, bitmapHeight = clipy1 - clipy2;
			int igrey;
			/*
			 * Create a device-independent bitmap, 32 bits deep.
			 */
			struct { BITMAPINFOHEADER header; } bitmapInfo;
			integer scanLineLength = bitmapWidth * 4;   // for 24 bits: (bitmapWidth * 3 + 3) & ~3L;
			HBITMAP bitmap;
			unsigned char *bits;   // a pointer to memory allocated by VirtualAlloc or by CreateDIBSection ()
			bitmapInfo. header.biSize = sizeof (BITMAPINFOHEADER);
			bitmapInfo. header.biWidth = bitmapWidth;   // scanLineLength;
			bitmapInfo. header.biHeight = bitmapHeight;
			bitmapInfo. header.biPlanes = 1;
			bitmapInfo. header.biBitCount = 32;
			bitmapInfo. header.biCompression = 0;
			bitmapInfo. header.biSizeImage = 0;
			bitmapInfo. header.biXPelsPerMeter = 0;
			bitmapInfo. header.biYPelsPerMeter = 0;
			bitmapInfo. header.biClrUsed = 0;
			bitmapInfo. header.biClrImportant = 0;
			bitmap = CreateDIBSection (my d_gdiGraphicsContext /* ignored */, (CONST BITMAPINFO *) & bitmapInfo,
				DIB_RGB_COLORS, (VOID **) & bits, nullptr, 0);
		#elif quartz
			integer bytesPerRow = (clipx2 - clipx1) * 4;
			Melder_assert (bytesPerRow > 0);
			integer numberOfRows = clipy1 - clipy2;
			Melder_assert (numberOfRows > 0);
			unsigned char *imageData = Melder_malloc_f (unsigned char, bytesPerRow * numberOfRows);
		#endif
		/*
		 * Draw into the bitmap.
		 */
		#if cairo
			#define ROW_START_ADDRESS  (bits + (clipy1 - 1 - yDC) * scanLineLength)
			#define PUT_PIXEL \
				if (1) { \
					unsigned char kar = value <= 0 ? 0 : value >= 255 ? 255 : (int) value; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = 0; \
				}
		#elif gdi
			#define ROW_START_ADDRESS  (bits + (clipy1 - 1 - yDC) * scanLineLength)
			#define PUT_PIXEL \
				if (1) { \
					unsigned char kar = value <= 0 ? 0 : value >= 255 ? 255 : (int) value; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = 0; \
				}
		#elif quartz
			#define ROW_START_ADDRESS  (imageData + (clipy1 - 1 - yDC) * bytesPerRow)
			#define PUT_PIXEL \
				if (my colourScale == kGraphics_colourScale::GREY) { \
					unsigned char kar = value <= 0 ? 0 : value >= 255 ? 255 : (int) value; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = kar; \
					*pixelAddress ++ = 255; \
				} else if (my colourScale == kGraphics_colourScale::BLUE_TO_RED) { \
					if (value < 0.0) { \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 63; \
						*pixelAddress ++ = 255; \
					} else if (value < 64.0) { \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = (int) (value * 3 + 63.999); \
						*pixelAddress ++ = 255; \
					} else if (value < 128.0) { \
						*pixelAddress ++ = (int) (value * 4 - 256.0); \
						*pixelAddress ++ = (int) (value * 4 - 256.0); \
						*pixelAddress ++ = 255; \
						*pixelAddress ++ = 255; \
					} else if (value < 192.0) { \
						*pixelAddress ++ = 255; \
						*pixelAddress ++ = (int) ((256.0 - value) * 4 - 256.0); \
						*pixelAddress ++ = (int) ((256.0 - value) * 4 - 256.0); \
						*pixelAddress ++ = 255; \
					} else if (value < 256.0) { \
						*pixelAddress ++ = (int) ((256.0 - value) * 3 + 63.999); \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 255; \
					} else { \
						*pixelAddress ++ = 63; \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 0; \
						*pixelAddress ++ = 255; \
					} \
				}
		#else
			#define ROW_START_ADDRESS  nullptr
			#define PUT_PIXEL
		#endif
		if (interpolate) {
			try {
				autoINTVEC ileftBuffer = zero_INTVEC (clipx2 - clipx1 + 1);
				autoINTVEC irightBuffer = zero_INTVEC (clipx2 - clipx1 + 1);
				autoVEC leftWeightBuffer = zero_VEC (clipx2 - clipx1 + 1);
				autoVEC rightWeightBuffer = zero_VEC (clipx2 - clipx1 + 1);
				integer *ileft  = & ileftBuffer  [1 - clipx1];
				integer *iright = & irightBuffer [1 - clipx1];
				double *leftWeight  = & leftWeightBuffer  [1 - clipx1];
				double *rightWeight = & rightWeightBuffer [1 - clipx1];
				for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
					double ix_real = ix1 - 0.5 + ((double) nx * (xDC - x1DC)) / (x2DC - x1DC);
					ileft [xDC] = (integer) floor (ix_real), iright [xDC] = ileft [xDC] + 1;
					rightWeight [xDC] = ix_real - ileft [xDC];
					leftWeight [xDC] = 1.0 - rightWeight [xDC];
					if (ileft [xDC] < ix1)
						ileft [xDC] = ix1;
					if (iright [xDC] > ix2)
						iright [xDC] = ix2;
				}
				for (yDC = clipy2; yDC < clipy1; yDC += undersampling) {
					double iy_real = iy2 + 0.5 - ((double) ny * (yDC - y2DC)) / (y1DC - y2DC);
					integer itop = Melder_iceiling (iy_real), ibottom = itop - 1;
					double bottomWeight = itop - iy_real, topWeight = 1.0 - bottomWeight;
					unsigned char *pixelAddress = ROW_START_ADDRESS;
					if (itop > iy2) itop = iy2;
					if (ibottom < iy1) ibottom = iy1;
					if (! NUMisEmpty (z_float)) {
						constVECVU ztop = z_float [itop], zbottom = z_float [ibottom];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double interpol =
								rightWeight [xDC] *
									(topWeight * ztop [iright [xDC]] + bottomWeight * zbottom [iright [xDC]]) +
								leftWeight [xDC] *
									(topWeight * ztop [ileft [xDC]] + bottomWeight * zbottom [ileft [xDC]]);
							double value = offset - scale * interpol;
							PUT_PIXEL
						}
					} else if (! NUMisEmpty (z_rgbt)) {
						constvectorview <MelderColour> ztop = z_rgbt [itop], zbottom = z_rgbt [ibottom];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double red =
								rightWeight [xDC] * (topWeight * ztop [iright [xDC]]. red + bottomWeight * zbottom [iright [xDC]]. red) +
								leftWeight  [xDC] * (topWeight * ztop [ileft  [xDC]]. red + bottomWeight * zbottom [ileft  [xDC]]. red);
							double green =
								rightWeight [xDC] * (topWeight * ztop [iright [xDC]]. green + bottomWeight * zbottom [iright [xDC]]. green) +
								leftWeight  [xDC] * (topWeight * ztop [ileft  [xDC]]. green + bottomWeight * zbottom [ileft  [xDC]]. green);
							double blue =
								rightWeight [xDC] * (topWeight * ztop [iright [xDC]]. blue + bottomWeight * zbottom [iright [xDC]]. blue) +
								leftWeight  [xDC] * (topWeight * ztop [ileft  [xDC]]. blue + bottomWeight * zbottom [ileft  [xDC]]. blue);
							double transparency =
								rightWeight [xDC] * (topWeight * ztop [iright [xDC]]. transparency + bottomWeight * zbottom [iright [xDC]]. transparency) +
								leftWeight  [xDC] * (topWeight * ztop [ileft  [xDC]]. transparency + bottomWeight * zbottom [ileft  [xDC]]. transparency);
							Melder_clip (0.0, & red,          1.0);
							Melder_clip (0.0, & green,        1.0);
							Melder_clip (0.0, & blue,         1.0);
							Melder_clip (0.0, & transparency, 1.0);

							#if cairo
								*pixelAddress ++ = blue         * 255.0;
								*pixelAddress ++ = green        * 255.0;
								*pixelAddress ++ = red          * 255.0;
								*pixelAddress ++ = transparency * 255.0;
							#elif gdi
								*pixelAddress ++ = blue         * 255.0;
								*pixelAddress ++ = green        * 255.0;
								*pixelAddress ++ = red          * 255.0;
								*pixelAddress ++ = 0;
							#elif quartz
								*pixelAddress ++ = red          * 255.0;
								*pixelAddress ++ = green        * 255.0;
								*pixelAddress ++ = blue         * 255.0;
								*pixelAddress ++ = (1.0 - transparency) * 255.0;
							#endif
						}
					} else {
						constvectorview <unsigned char> ztop = z_byte [itop], zbottom = z_byte [ibottom];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double interpol =
								rightWeight [xDC] *
									(topWeight * ztop [iright [xDC]] + bottomWeight * zbottom [iright [xDC]]) +
								leftWeight [xDC] *
									(topWeight * ztop [ileft [xDC]] + bottomWeight * zbottom [ileft [xDC]]);
							double value = offset - scale * interpol;
							PUT_PIXEL
						}
					}
				}
			} catch (MelderError) {
				Melder_clearError ();
			}
		} else {
			try {
				autoINTVEC ixBuffer = zero_INTVEC (clipx2 - clipx1 + 1);
				integer *ix = & ixBuffer [1 - clipx1];
				for (xDC = clipx1; xDC < clipx2; xDC += undersampling)
					ix [xDC] = Melder_ifloor (ix1 + (nx * (xDC - x1DC)) / (x2DC - x1DC));
				for (yDC = clipy2; yDC < clipy1; yDC += undersampling) {
					integer iy = Melder_iceiling (iy2 - (ny * (yDC - y2DC)) / (y1DC - y2DC));
					unsigned char *pixelAddress = ROW_START_ADDRESS;
					Melder_assert (iy >= iy1 && iy <= iy2);
					if (! NUMisEmpty (z_float)) {
						constVECVU ziy = z_float [iy];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double value = offset - scale * ziy [ix [xDC]];
							PUT_PIXEL
						}
					} else {
						constvectorview <unsigned char> ziy = z_byte [iy];
						for (xDC = clipx1; xDC < clipx2; xDC += undersampling) {
							double value = offset - scale * ziy [ix [xDC]];
							PUT_PIXEL
						}
					}
				}
			} catch (MelderError) {
				Melder_clearError ();
			}
		}
		/*
		 * Copy the bitmap to the screen.
		 */
		#if cairo
			cairo_matrix_t clip_trans;
			cairo_matrix_init_identity (& clip_trans);
			cairo_matrix_scale (& clip_trans, 1, -1);		// we painted in the reverse y-direction
			cairo_matrix_translate (& clip_trans, - clipx1, - clipy1);
			cairo_pattern_t *bitmap_pattern = cairo_pattern_create_for_surface (sfc);
			trace (U"bitmap pattern ", Melder_pointer (bitmap_pattern));
			if (cairo_status_t status = cairo_pattern_status (bitmap_pattern)) {
				Melder_casual (U"bitmap pattern status: ", Melder_peek8to32_u (cairo_status_to_string (status)));
			} else {
				cairo_pattern_set_matrix (bitmap_pattern, & clip_trans);
				cairo_save (my d_cairoGraphicsContext);
				cairo_set_source (my d_cairoGraphicsContext, bitmap_pattern);
				cairo_paint (my d_cairoGraphicsContext);
				cairo_restore (my d_cairoGraphicsContext);
			}
			cairo_pattern_destroy (bitmap_pattern);
		#elif gdi
			SetDIBitsToDevice (my d_gdiGraphicsContext, clipx1, clipy2, bitmapWidth, bitmapHeight, 0, 0, 0, bitmapHeight,
				bits, (CONST BITMAPINFO *) & bitmapInfo, DIB_RGB_COLORS);
			//StretchDIBits (my d_gdiGraphicsContext, clipx1, clipy2, bitmapWidth, bitmapHeight, 0, 0, 0, bitmapHeight,
			//	bits, (CONST BITMAPINFO *) & bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
		#elif quartz
			CGImageRef image;
			static CGColorSpaceRef colourSpace = nullptr;
			if (! colourSpace) {
				colourSpace = CGColorSpaceCreateWithName (kCGColorSpaceGenericRGB);   // used to be kCGColorSpaceUserRGB
				Melder_assert (colourSpace != nullptr);
			}
			if (1) {
				CGDataProviderRef dataProvider = CGDataProviderCreateWithData (nullptr,
					imageData,
					bytesPerRow * numberOfRows,
					_mac_releaseDataCallback   // we need this because we cannot release the image data immediately after drawing,
						// because in PDF files the imageData has to stay available through EndPage
				);
				Melder_assert (dataProvider != nullptr);
				image = CGImageCreate (clipx2 - clipx1, numberOfRows,
					8, 32, bytesPerRow, colourSpace, kCGImageAlphaLast, dataProvider, nullptr, false, kCGRenderingIntentDefault);
				CGDataProviderRelease (dataProvider);
			} else if (0) {
				Melder_assert (CGBitmapContextCreate != nullptr);
				CGContextRef bitmaptest = CGBitmapContextCreate (imageData, 100, 100,
					8, 800, colourSpace, 0);
				Melder_assert (bitmaptest != nullptr);
				CGContextRef bitmap = CGBitmapContextCreate (nullptr /* imageData */, clipx2 - clipx1, numberOfRows,
					8, bytesPerRow, colourSpace, kCGImageAlphaLast);
				Melder_assert (bitmap != nullptr);
				image = CGBitmapContextCreateImage (bitmap);
				// release bitmap?
			}
			Melder_assert (image != nullptr);
			CGContextDrawImage (my d_macGraphicsContext, CGRectMake (clipx1, clipy2, clipx2 - clipx1, clipy1 - clipy2), image);
			//CGColorSpaceRelease (colourSpace);
			CGImageRelease (image);
		#endif
		/*
		 * Clean up.
		 */
		#if cairo
			cairo_surface_destroy (sfc);
		#elif gdi
			DeleteBitmap (bitmap);
		#endif
	}
	#if gdi
		end:
		return;
	#endif
}

static void _GraphicsPostscript_cellArrayOrImage (GraphicsPostscript me,
	constMATVU const& z_float,
	constmatrixview <MelderColour> const& z_rgbt,
	constmatrixview <unsigned char> const& z_byte,
	integer ix1, integer ix2, integer x1DC, integer x2DC,
	integer iy1, integer iy2, integer y1DC, integer y2DC,
	double minimum, double maximum,
	integer clipx1, integer clipx2, integer clipy1, integer clipy2, bool interpolate)
{
	integer interpolateX = 1, interpolateY = 1;
	integer nx = ix2 - ix1 + 1, ny = iy2 - iy1 + 1, filling = 0;
	double scale = ( my photocopyable ? 200.1f : 255.1f ) / (maximum - minimum);
	double offset = 255.1f + minimum * scale;
	int minimalGrey = my photocopyable ? 55 : 0;
	my d_printf (my d_file, "gsave N %s %s M %s %s L %s %s L %s %s L closepath clip\n",
		Melder8_integer (clipx1), Melder8_integer (clipy1), Melder8_integer (clipx2 - clipx1), Melder8_integer (0),
		Melder8_integer (0), Melder8_integer (clipy2 - clipy1), Melder8_integer (clipx1 - clipx2), Melder8_integer (0));
	my d_printf (my d_file, "%s %s translate %s %s scale\n",
		Melder8_integer (x1DC), Melder8_integer (y1DC), Melder8_integer (x2DC - x1DC), Melder8_integer (y2DC - y1DC));
	if (interpolate) {
		/* The smallest image resolution is 300 dpi. If a sample takes up more than 25.4/300 mm, the 300 dpi resolution is achieved by interpolation. */
		const double smallestImageResolution = 300.0;
		double colSize_pixels = (double) (x2DC - x1DC) / nx;
		double rowSize_pixels = (double) (y2DC - y1DC) / ny;
		double colSize_inches = colSize_pixels / my resolution;
		double rowSize_inches = rowSize_pixels / my resolution;
		interpolateX = Melder_iceiling (colSize_inches * smallestImageResolution);   // number of interpolation points per horizontal sample
		interpolateY = Melder_iceiling (rowSize_inches * smallestImageResolution);   // number of interpolation points per vertical sample
	}

	if (interpolateX <= 1 && interpolateY <= 1) {
		/* Do not interpolate. */
		my d_printf (my d_file, "/picstr %s string def %s %s 8 [%s 0 0 %s 0 0]\n"
			"{ currentfile picstr readhexstring pop } image\n",
			Melder8_integer (nx), Melder8_integer (nx), Melder8_integer (ny), Melder8_integer (nx), Melder8_integer (ny));
	} else if (interpolateX > 1 && interpolateY > 1) {
		/* Interpolate both horizontally and vertically. */
		integer nx_new = nx * interpolateX;
		integer ny_new = ny * interpolateY;
		/* Interpolation between rows requires us to remember two original rows: */
		my d_printf (my d_file, "/lorow %s string def /hirow %s string def\n",
			Melder8_integer (nx), Melder8_integer (nx));
		/* New rows (scanlines) are longer: */
		my d_printf (my d_file, "/scanline %s string def\n",
			Melder8_integer (nx_new));
		/* The first four arguments to the 'image' command, */
		/* namely the new number of columns, the new number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%s %s 8 [%s 0 0 %s 0 0]\n",
			Melder8_integer (nx_new), Melder8_integer (ny_new), Melder8_integer (nx_new), Melder8_integer (ny_new));
		/* Since our imageproc is going to output only one scanline at a time, */
		/* the outer loop variable (scanline number) has to be initialized outside the imageproc: */
		my d_printf (my d_file, "/irow 0 def\n");
		/* The imageproc starts here. First, we fill one or two original rows if necessary; */
		/* they are read as hexadecimal strings from the current file, i.e. just after the image command. */
		my d_printf (my d_file, "{\n"
			/* First test: are we in the first scanline? If so, read two original rows: */
			"irow 0 eq { currentfile lorow readhexstring pop pop lorow hirow copy pop } if\n"
			/* Second test: did we just pass an original data row? */
			/* If so, */
			/*    (1) move that row backwards; */
			/*    (2) read a new one unless we just passed the last original row: */
			"irow %s mod %s eq { hirow lorow copy pop\n"
			"irow %s ne { currentfile hirow readhexstring pop pop } if } if\n",
			Melder8_integer (interpolateY), Melder8_integer (interpolateY / 2),
			Melder8_integer (ny_new - interpolateY + interpolateY / 2));
		/* Where are we between those two rows? */
		my d_printf (my d_file, "/rowphase irow %s add %s mod %s div def\n",
			Melder8_integer (interpolateY - interpolateY / 2), Melder8_integer (interpolateY), Melder8_integer (interpolateY));
		/* Inner loop starts here. It cycles through all new columns: */
		my d_printf (my d_file, "0 1 %s {\n", Melder8_integer (nx_new - 1));
		/* Get the inner loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Where are the two original columns? */
		my d_printf (my d_file, "   /locol icol %s sub %s idiv def\n",
			Melder8_integer (interpolateX / 2), Melder8_integer (interpolateX));
		my d_printf (my d_file, "   /hicol icol %s ge { %s } { icol %s add %s idiv } ifelse def\n",
			Melder8_integer (nx_new - interpolateX / 2), Melder8_integer (nx - 1),
			Melder8_integer (interpolateX / 2), Melder8_integer (interpolateX));
		/* Where are we between those two columns? */
		my d_printf (my d_file, "   /colphase icol %s add %s mod %s div def\n",
			Melder8_integer (interpolateX - interpolateX / 2), Melder8_integer (interpolateX), Melder8_integer (interpolateX));
		/* Four-point interpolation: */
		my d_printf (my d_file,
			"   /plow lorow locol get def\n"
			"   /phigh lorow hicol get def\n"
			"   /qlow hirow locol get def\n"
			"   /qhigh hirow hicol get def\n"
			"   /value\n"
			"      plow phigh plow sub colphase mul add 1 rowphase sub mul\n"
			"      qlow qhigh qlow sub colphase mul add rowphase mul\n"
			"      add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"/irow irow 1 add def scanline } image\n");
	} else if (interpolateX > 1) {
		/* Interpolate horizontally only. */
		integer nx_new = nx * interpolateX;
		/* Remember one original row: */
		my d_printf (my d_file, "/row %s string def\n", Melder8_integer (nx));
		/* New rows (scanlines) are longer: */
		my d_printf (my d_file, "/scanline %s string def\n", Melder8_integer (nx_new));
		/* The first four arguments to the 'image' command, */
		/* namely the new number of columns, the number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%s %s 8 [%s 0 0 %s 0 0]\n",
			Melder8_integer (nx_new), Melder8_integer (ny), Melder8_integer (nx_new), Melder8_integer (ny));
		/* The imageproc starts here. We fill one original row. */
		my d_printf (my d_file, "{\n"
			"currentfile row readhexstring pop pop\n");
		/* Loop starts here. It cycles through all new columns: */
		my d_printf (my d_file, "0 1 %s {\n", Melder8_integer (nx_new - 1));
		/* Get the loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Where are the two original columns? */
		my d_printf (my d_file, "   /locol icol %s sub %s idiv def\n",
			Melder8_integer (interpolateX / 2), Melder8_integer (interpolateX));
		my d_printf (my d_file, "   /hicol icol %s ge { %s } { icol %s add %s idiv } ifelse def\n",
			Melder8_integer (nx_new - interpolateX / 2), Melder8_integer (nx - 1),
			Melder8_integer (interpolateX / 2), Melder8_integer (interpolateX));
		/* Where are we between those two columns? */
		my d_printf (my d_file, "   /colphase icol %s add %s mod %s div def\n",
			Melder8_integer (interpolateX - interpolateX / 2), Melder8_integer (interpolateX), Melder8_integer (interpolateX));
		/* Two-point interpolation: */
		my d_printf (my d_file,
			"   /plow row locol get def\n"
			"   /phigh row hicol get def\n"
			"   /value plow phigh plow sub colphase mul add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"scanline } image\n");
	} else {
		/* Interpolate vertically only. */
		integer ny_new = ny * interpolateY;
		/* Interpolation between rows requires us to remember two original rows: */
		my d_printf (my d_file, "/lorow %s string def /hirow %s string def\n", Melder8_integer (nx), Melder8_integer (nx));
		/* New rows (scanlines) are equally long: */
		my d_printf (my d_file, "/scanline %s string def\n", Melder8_integer (nx));
		/* The first four arguments to the 'image' command, */
		/* namely the number of columns, the new number of rows, the bit depth, and the matrix: */
		my d_printf (my d_file, "%s %s 8 [%s 0 0 %s 0 0]\n",
			Melder8_integer (nx), Melder8_integer (ny_new), Melder8_integer (nx), Melder8_integer (ny_new));
		/* Since our imageproc is going to output only one scanline at a time, */
		/* the outer loop variable (scanline number) has to be initialized outside the imageproc: */
		my d_printf (my d_file, "/irow 0 def\n");
		/* The imageproc starts here. First, we fill one or two original rows if necessary; */
		/* they are read as hexadecimal strings from the current file, i.e. just after the image command. */
		my d_printf (my d_file, "{\n"
			/* First test: are we in the first scanline? If so, read two original rows: */
			"irow 0 eq { currentfile lorow readhexstring pop pop lorow hirow copy pop } if\n"
			/* Second test: did we just pass an original data row? */
			/* If so, */
			/*    (1) move that row backwards; */
			/*    (2) read a new one unless we just passed the last original row: */
			"irow %s mod %s eq { hirow lorow copy pop\n"
			"irow %s ne { currentfile hirow readhexstring pop pop } if } if\n",
			Melder8_integer (interpolateY), Melder8_integer (interpolateY / 2),
			Melder8_integer (ny_new - interpolateY + interpolateY / 2));
		/* Where are we between those two rows? */
		my d_printf (my d_file, "/rowphase irow %s add %s mod %s div def\n",
			Melder8_integer (interpolateY - interpolateY / 2),
			Melder8_integer (interpolateY), Melder8_integer (interpolateY));
		/* Inner loop starts here. It cycles through all columns: */
		my d_printf (my d_file, "0 1 %s {\n", Melder8_integer (nx - 1));
		/* Get the inner loop variable: */
		my d_printf (my d_file, "   /icol exch def\n");
		/* Two-point interpolation: */
		my d_printf (my d_file,
			"   /p lorow icol get def\n"
			"   /q hirow icol get def\n"
			"   /value\n"
			"      p 1 rowphase sub mul\n"
			"      q rowphase mul\n"
			"      add def\n"
			"   scanline icol value 0 le { 0 } { value 255 ge { 255 } { value } ifelse } ifelse cvi put\n"
			"} for\n"
			"/irow irow 1 add def scanline } image\n");
	}
	for (integer iy = iy1; iy <= iy2; iy ++) for (integer ix = ix1; ix <= ix2; ix ++) {
		int value = (int) (offset - scale * ( ! NUMisEmpty (z_float) ? z_float [iy] [ix] : z_byte [iy] [ix] ));
		my d_printf (my d_file, "%.2x", value <= minimalGrey ? minimalGrey : value >= 255 ? 255 : value);
		if (++ filling == 39) { my d_printf (my d_file, "\n"); filling = 0; }
	}
	if (filling) my d_printf (my d_file, "\n");
	my d_printf (my d_file, "grestore\n");
}

static void _cellArrayOrImage (Graphics me,
	constMATVU const& z_float, constmatrixview <MelderColour> const& z_rgbt, constmatrixview <unsigned char> const& z_byte,
	integer ix1, integer ix2, integer x1DC, integer x2DC,
	integer iy1, integer iy2, integer y1DC, integer y2DC, double minimum, double maximum,
	integer clipx1, integer clipx2, integer clipy1, integer clipy2, bool interpolate)
{
	if (my screen) {
		_GraphicsScreen_cellArrayOrImage (static_cast <GraphicsScreen> (me), z_float, z_rgbt, z_byte, ix1, ix2, x1DC, x2DC, iy1, iy2, y1DC, y2DC,
			minimum, maximum, clipx1, clipx2, clipy1, clipy2, interpolate);
	} else if (my postScript) {
		_GraphicsPostscript_cellArrayOrImage (static_cast <GraphicsPostscript> (me), z_float, z_rgbt, z_byte, ix1, ix2, x1DC, x2DC, iy1, iy2, y1DC, y2DC,
			minimum, maximum, clipx1, clipx2, clipy1, clipy2, interpolate);
	}
	_Graphics_setColour (me, my colour);
}

static void _autoSize (
	const constGraphics me,
	const double x1WC, const double x2WC, const double y1WC, const double y2WC,
	const integer naturalWidthDC, const integer naturalHeightDC,
	/* mutable out */ integer *const x1DC, /* mutable out */ integer *const x2DC,
	/* mutable out */ integer *const y1DC, /* mutable out */ integer *const y2DC
) {
	*x1DC = wdx (x1WC);
	*x2DC = wdx (x2WC);
	*y1DC = wdy (y1WC);
	*y2DC = wdy (y2WC);
	if (x1WC == x2WC) {
		if (y1WC == y2WC) {
			/*
				Horizontal and vertical autosizing.
			*/
			const integer width = naturalWidthDC;
			*x1DC -= width / 2;
			*x2DC = *x1DC + width;
			const integer height = naturalHeightDC;
			if (my yIsZeroAtTheTop) {
				*y2DC -= height / 2;
				*y1DC = *y2DC + height;
			} else {
				*y1DC -= height / 2;
				*y2DC = *y1DC + height;
			}
		} else {
			/*
				Horizontal autosizing only.
			*/
			const integer height = ( my yIsZeroAtTheTop ? *y1DC - *y2DC : *y2DC - *y1DC );
			const integer width = height / (double) naturalHeightDC * (double) naturalWidthDC;
			*x1DC -= width / 2;
			*x2DC = *x1DC + width;
		}
	} else if (y1WC == y2WC) {
		/*
			Vertical autosizing only.
		*/
		const integer width = *x2DC - *x1DC;
		const integer height = width / (double) naturalWidthDC * (double) naturalHeightDC;
		if (my yIsZeroAtTheTop) {
			*y2DC -= height / 2;
			*y1DC = *y2DC + height;
		} else {
			*y1DC -= height / 2;
			*y2DC = *y1DC + height;
		}
	}
}

void Graphics_cellArray (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, double minimum, double maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (CELL_ARRAY, 8 + z.nrow * z.ncol);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++)
			for (integer icol = 1; icol <= z.ncol; icol ++)
				put (z [irow] [icol]);
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, z, constmatrixview<MelderColour>(), constmatrixview<unsigned char>(),
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), false
		);
	}
}

void Graphics_cellArray_colour (Graphics me, constmatrixview <MelderColour> const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, double minimum, double maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (CELL_ARRAY_COLOUR, 8 + z.nrow * z.ncol * 4);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++) {
			constvectorview <MelderColour> row = z [irow];
			for (integer icol = 1; icol <= z.ncol; icol ++) {
				put (row [icol]. red);
				put (row [icol]. green);
				put (row [icol]. blue);
				put (row [icol]. transparency);
			}
		}
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, constMATVU(), z, constmatrixview<unsigned char>(),
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), false
		);
	}
}

void Graphics_cellArray8 (Graphics me, constmatrixview<unsigned char> const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, unsigned char minimum, unsigned char maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (CELL_ARRAY8, 8 + z.nrow * z.ncol);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++)
			for (integer icol = 1; icol <= z.ncol; icol ++)
				put (z [irow] [icol]);
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, constMATVU(), constmatrixview<MelderColour>(), z,
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), false
		);
	}
}

void Graphics_image (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, double minimum, double maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (IMAGE, 8 + z.nrow * z.ncol);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++)
			for (integer icol = 1; icol <= z.ncol; icol ++)
				put (z [irow] [icol]);
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, z, constmatrixview<MelderColour>(), constmatrixview<unsigned char>(),
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), true
		);
	}
}

void Graphics_image_colour (Graphics me, constmatrixview <MelderColour> const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, double minimum, double maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (IMAGE_COLOUR, 8 + z.nrow * z.ncol * 4);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++) {
			constvectorview <MelderColour> row = z [irow];
			for (integer icol = 1; icol <= z.ncol; icol ++) {
				put (row [icol]. red);
				put (row [icol]. green);
				put (row [icol]. blue);
				put (row [icol]. transparency);
			}
		}
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, constMATVU(), z, constmatrixview<unsigned char>(),
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), true
		);
	}
}

void Graphics_image8 (Graphics me, constmatrixview <unsigned char> const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, uint8 minimum, uint8 maximum)
{
	if (z.nrow < 1 || z.ncol < 1 || minimum == maximum)
		return;
	if (my recording) {
		op (IMAGE8, 8 + z.nrow * z.ncol);
		put (x1WC); put (x2WC); put (y1WC); put (y2WC);
		put (minimum); put (maximum);
		put (z.nrow); put (z.ncol);
		for (integer irow = 1; irow <= z.nrow; irow ++)
			for (integer icol = 1; icol <= z.ncol; icol ++)
				put (z [irow] [icol]);
	} else {
		/* mutable delayed init */ integer x1DC, x2DC, y1DC, y2DC;
		_autoSize (me, x1WC, x2WC, y1WC, y2WC, z.ncol, z.nrow, & x1DC, & x2DC, & y1DC, & y2DC);
		_cellArrayOrImage (me, constMATVU(), constmatrixview<MelderColour>(), z,
			1, z.ncol, x1DC, x2DC, 1, z.nrow, y1DC, y2DC, minimum, maximum,
			wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC), true
		);
	}
}

static void _GraphicsScreen_imageFromFile (GraphicsScreen me, conststring32 relativeFileName, double x1, double x2, double y1, double y2) {
	integer x1DC = wdx (x1), x2DC = wdx (x2), y1DC = wdy (y1), y2DC = wdy (y2);
	integer width = x2DC - x1DC, height = my yIsZeroAtTheTop ? y1DC - y2DC : y2DC - y1DC;
	#if 0
		structMelderFile file { };
		Melder_relativePathToFile (relativeFileName, & file);
		try {
			autoPhoto photo = Photo_readFromImageFile (& file);
			if (x1 == x2 && y1 == y2) {
				width = photo -> nx, x1DC -= width / 2, x2DC = x1DC + width;
				height = photo -> ny, y2DC -= height / 2, y1DC = y2DC + height;
			} else if (x1 == x2) {
				width = height * (double) photo -> nx / (double) photo -> ny;
				x1DC -= width / 2, x2DC = x1DC + width;
			} else if (y1 == y2) {
				height = width * (double) photo -> ny / (double) photo -> nx;
				y2DC -= height / 2, y1DC = y2DC + height;
			}
			automatrix <MelderColour> z = newmatrixraw <MelderColour> (photo -> ny, photo -> nx);
			for (integer iy = 1; iy <= photo -> ny; iy ++) {
				for (integer ix = 1; ix <= photo -> nx; ix ++) {
					z [iy] [ix]. red          = photo -> d_red          -> z [iy] [ix];
					z [iy] [ix]. green        = photo -> d_green        -> z [iy] [ix];
					z [iy] [ix]. blue         = photo -> d_blue         -> z [iy] [ix];
					z [iy] [ix]. transparency = photo -> d_transparency -> z [iy] [ix];
				}
			}
			_cellArrayOrImage (me, constMATVU(), z.get(), constmatrixview<unsigned char>(),
				1, photo -> nx, x1DC, x2DC, 1, photo -> ny, y1DC, y2DC,
				0.0, 1.0,
				//wdx (my d_x1WC), wdx (my d_x2WC), wdy (my d_y1WC), wdy (my d_y2WC),   // in case of clipping
				LONG_MIN, LONG_MAX, LONG_MAX, LONG_MIN,   // in case of no clipping
				true
			);
		} catch (MelderError) {
			Melder_clearError ();
		}
	#elif gdi
		if (my d_useGdiplus) {
			structMelderFile file { };
			Melder_relativePathToFile (relativeFileName, & file);
			Gdiplus::Bitmap image (MelderFile_peekPathW (& file));
			if (x1 == x2 && y1 == y2) {
				width = image. GetWidth (), x1DC -= width / 2, x2DC = x1DC + width;
				height = image. GetHeight (), y2DC -= height / 2, y1DC = y2DC + height;
			} else if (x1 == x2) {
				width = height * (double) image. GetWidth () / (double) image. GetHeight ();
				x1DC -= width / 2, x2DC = x1DC + width;
			} else if (y1 == y2) {
				height = width * (double) image. GetHeight () / (double) image. GetWidth ();
				y2DC -= height / 2, y1DC = y2DC + height;
			}
			Gdiplus::Graphics dcplus (my d_gdiGraphicsContext);
			Gdiplus::Rect rect (x1DC, y2DC, width, height);
			dcplus. DrawImage (& image, rect);
		} else {
		}
	#elif quartz
		structMelderFile file { };
		Melder_relativePathToFile (relativeFileName, & file);
		CFURLRef url = CFURLCreateWithFileSystemPath (nullptr, (CFStringRef) MelderFile_peekPathCfstring (& file), kCFURLPOSIXPathStyle, false);
		CGImageSourceRef imageSource = CGImageSourceCreateWithURL (url, nullptr);
		CFRelease (url);
		if (imageSource) {
			CGImageRef image = CGImageSourceCreateImageAtIndex (imageSource, 0, nullptr);
			CFRelease (imageSource);
			if (image) {
				if (x1 == x2 && y1 == y2) {
					width = CGImageGetWidth (image), x1DC -= width / 2, x2DC = x1DC + width;
					height = CGImageGetHeight (image), y2DC -= height / 2, y1DC = y2DC + height;
				} else if (x1 == x2) {
					width = height * (double) CGImageGetWidth (image) / (double) CGImageGetHeight (image);
					x1DC -= width / 2, x2DC = x1DC + width;
				} else if (y1 == y2) {
					height = width * (double) CGImageGetHeight (image) / (double) CGImageGetWidth (image);
					y2DC -= height / 2, y1DC = y2DC + height;
				}
				CGContextSaveGState (my d_macGraphicsContext);
                
                //NSCAssert(my d_macGraphicsContext, @"nil context");

				CGContextTranslateCTM (my d_macGraphicsContext, 0, y1DC);
				CGContextScaleCTM (my d_macGraphicsContext, 1.0, -1.0);
				CGContextDrawImage (my d_macGraphicsContext, CGRectMake (x1DC, 0, width, height), image);
				CGContextRestoreGState (my d_macGraphicsContext);
				CGImageRelease (image);
			}
		}
	#endif
}

void Graphics_imageFromFile (Graphics me, conststring32 relativeFileName, double x1, double x2, double y1, double y2) {
	if (my recording) {
		conststring8 txt_utf8 = Melder_peek32to8 (relativeFileName);
		int length = strlen (txt_utf8) / sizeof (double) + 1;   // TODO: integer overflow
		op (IMAGE_FROM_FILE, 5 + length); put (x1); put (x2); put (y1); put (y2); sput (txt_utf8, length)
	} else if (my screen) {
		_GraphicsScreen_imageFromFile (static_cast <GraphicsScreen> (me), relativeFileName, x1, x2, y1, y2);
	}
}

void Graphics_imageFromFile_embedded (Graphics me, conststring32 relativeFileName, double x1, double x2, double y1, double y2) {
	if (Melder_endsWith_caseAware (relativeFileName, U".pdf"))
		Melder_throw (U"Cannot embed PDF pictures. Use “Insert picture from file...” instead.");
	structMelderFile file { };
	Melder_relativePathToFile (relativeFileName, & file);
	try {
		autoPhoto photo = Photo_readFromImageFile (& file);
		automatrix <MelderColour> z = newmatrixraw <MelderColour> (photo -> ny, photo -> nx);
		for (integer iy = 1; iy <= photo -> ny; iy ++) {
			for (integer ix = 1; ix <= photo -> nx; ix ++) {
				z [iy] [ix]. red          = photo -> d_red          -> z [iy] [ix];
				z [iy] [ix]. green        = photo -> d_green        -> z [iy] [ix];
				z [iy] [ix]. blue         = photo -> d_blue         -> z [iy] [ix];
				z [iy] [ix]. transparency = photo -> d_transparency -> z [iy] [ix];
			}
		}
		Graphics_image_colour (me, z.get(), x1, x2, y1, y2, 0.0, 1.0);   // this does clipping to the canvas
	} catch (MelderError) {
		Melder_clearError ();
	}
}

/* End of file Graphics_image.cpp */
