#ifndef _Corpus_h_
#define _Corpus_h_
/* Corpus.h
 *
 * Copyright (C) 2011,2015,2016,2018,2026 Paul Boersma
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

#include "Table.h"
#include "TextGrid.h"
#include "Sound.h"

#include "Corpus_def.h"

autoCorpus Corpus_create (conststring32 folderWithSoundFiles, conststring32 soundFileExtension,
	conststring32 folderWithAnnotationFiles, conststring32 annotationFileExtension);

autoCorpus Corpus_importFromCGN (conststring32 rootFolderPath);

autoTextGrid Corpus_extractTextGrid_number (Corpus me, integer recordingNumber);
autoSound Corpus_extractSound_number (Corpus me, integer recordingNumber);

autoCorpus Corpus_extractComponents (Corpus me,
	bool comp_a, bool comp_b, bool comp_c, bool comp_d, bool comp_e,
	bool comp_f, bool comp_g, bool comp_h, bool comp_i, bool comp_j,
	bool comp_k, bool comp_l, bool comp_m, bool comp_n, bool comp_o
);

static conststring32 Corpus_CGN_regionCodes [25] {
	U"regN1a", U"regN1b", U"regN1c", U"regN2a", U"regN2b",
	U"regN2c", U"regN2d", U"regN2e", U"regN2f", U"regN3a",
	U"regN3b", U"regN3c", U"regN3d", U"regN3e", U"regN4a",
	U"regN4b", U"regNx", U"regV1", U"regV2", U"regV3",
	U"regV4", U"regVx", U"regW", U"regZ", U"regX"
};

static conststring32 Corpus_CGN_regions_short [25] {
	U"S-Holland", U"N-Holland", U"W-Utrecht", U"Zeeland", U"E-Utrecht",
	U"Guelders", U"Veluwe", U"W-Friesland", U"Polders", U"Achterhoek",
	U"Overijssel", U"Drenthe", U"Groningen", U"Frisia", U"N-Brabant",
	U"E-Limburg", U"Netherlands", U"S-Brabant", U"E-Flanders", U"W-Flanders",
	U"W-Limburg", U"N-Belgium", U"S-Belgium", U"reg_other", U"reg_unknown"
};

static conststring32 Corpus_CGN_regions_long [25] {
	U"South Holland (province of Zuid-Holland excl. Goeree-Overflakkee)",
	U"North Holland (province of Noord-Holland excl. West-Friesland)",
	U"West Utrecht (province of Utrecht, Western part incl. the city of Utrecht)",
	U"Zeeland (province of Zeeland, plus Goeree-Overflakkee)",
	U"East Utrecht (province of Utrecht, Eastern part excl. the city of Utrecht)",
	U"Guelders river area (province of Gelderland, Southern part incl. Arnhem en Nijmegen)",
	U"Veluwe (province of Gelderland, Northern part West of the IJssel river)",
	U"West-Friesland (part of the province of Noord-Holland",
	U"Polders (province of Flevoland)",
	U"Achterhoek (province of Gelderland, Eastern part)",
	U"Overijssel (province of Overijssel)",
	U"Drenthe (province of Drenthe)",
	U"Groningen (province of Groningen)",
	U"Frisia (province of Fryslân)",
	U"North Brabant (province of Noord-Brabant",
	U"East Limburg (Dutch province of Limburg)",
	U"unknown region in the Netherlands",
	U"South Brabant (provinces of Antwerpen and Brabant (Northern part))",
	U"East Flanders (province of Oost-Vlaanderen)",
	U"West Flanders (province of West-Vlaanderen",
	U"West Limburg (Belgian province of Limburg)",
	U"unknown region in North Belgium",
	U"South Belgium (Wallonia)",
	U"region outside Belgium and the Netherlands",
	U"unknown region"
};

autoCorpus Corpus_extractEducationRegions (Corpus me,
	bool sHolland, bool nHolland, bool wUtrecht, bool zeeland, bool eUtrecht,
	bool guelders, bool veluwe, bool wFriesland, bool polders, bool achterhoek,
	bool overijssel, bool drenthe, bool groningen, bool frisia, bool nBrabant,
	bool eLimburg, bool netherlands, bool sBrabant, bool eFlanders, bool wFlanders,
	bool wLimburg, bool nBelgium, bool sBelgium, bool reg_other, bool reg_unknown
);

#endif
/* End of file Corpus.h */
