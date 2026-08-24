# file "Corpus CGN/createRegion.praat"
# Paul Boersma, 21 August 2026

regionCodes$# = { "regN1a", "regN1b", "regN1c", "regN2a", "regN2b", "regN2c", "regN2d", "regN2e", "regN2f",
... "regN3a", "regN3b", "regN3c", "regN3d", "regN3e", "regN4a", "regN4b", "regNx",
... "regV1", "regV2", "regV3", "regV4", "regVx", "regW", "regZ", "regX" }

regions_short$# = { "S-Holland", "N-Holland", "W-Utrecht", "Zeeland", "E-Utrecht", "Guelders", "Veluwe", "W-Friesland", "Polders",
... "Achterhoek", "Overijssel", "Drenthe", "Groningen", "Frisia", "N-Brabant", "E-Limburg", "Netherlands",
... "S-Brabant", "E-Flanders", "W-Flanders", "W-Limburg", "N-Belgium", "S-Belgium", "reg_other", "reg_unknown" }

regions_long$# = {
... "South Holland (province of Zuid-Holland excl. Goeree-Overflakkee)",
... "North Holland (province of Noord-Holland excl. West-Friesland)",
... "West Utrecht (province of Utrecht, Western part incl. the city of Utrecht)",
... "Zeeland (province of Zeeland, plus Goeree-Overflakkee)",
... "East Utrecht (province of Utrecht, Eastern part excl. the city of Utrecht)",
... "Guelders river area (province of Gelderland, Southern part incl. Arnhem en Nijmegen)",
... "Veluwe (province of Gelderland, Northern part West of the IJssel river)",
... "West-Friesland (part of the province of Noord-Holland",
... "Polders (province of Flevoland)",
... "Achterhoek (province of Gelderland, Eastern part)",
... "Overijssel (province of Overijssel)",
... "Drenthe (province of Drenthe)",
... "Groningen (province of Groningen)",
... "Frisia (province of Fryslân)",
... "North Brabant (province of Noord-Brabant",
... "East Limburg (Dutch province of Limburg)",
... "unknown region in the Netherlands",
... "South Brabant (provinces of Antwerpen and Brabant (Northern part)",
... "East Flanders (province of Oost-Vlaanderen)",
... "West Flanders (province of West-Vlaanderen",
... "West Limburg (Belgian province of Limburg)",
... "unknown region in North Belgium",
... "South Belgium (Wallonia)",
... "region outside Belgium and the Netherlands",
... "unknown region" }

numberOfRegions = size (regionCodes$#)
assert size (regions_short$#) = numberOfRegions
assert size (regions_long$#) = numberOfRegions

imdiFolder$ = "/Volumes/CGN_2.0.3/data/meta/imdi/corpora"
table = Create Table with column names: "regionTable", 0, { "code", "short", "recording" }
numberOfRows = 0
for iregion to numberOfRegions
	regionCode$ = regionCodes$# [iregion]
	if regionCode$ <> "regW"
		lines$# = readLinesFromFile$#: imdiFolder$ + "/" + regionCodes$# [iregion] + ".imdi"
		for iline to size (lines$#)
			line$ = lines$# [iline]
			if left$ (line$, 18) = “<CorpusLink Name="”
				recording$ = mid$ (line$, 19, 8)
				numberOfRows += 1
				Append row
				Set string value: numberOfRows, "code", regionCode$
				Set string value: numberOfRows, "short", regions_short$# [iregion]
				Set string value: numberOfRows, "recording", recording$
			endif
		endfor
	endif
endfor
Sort rows: { "recording" }