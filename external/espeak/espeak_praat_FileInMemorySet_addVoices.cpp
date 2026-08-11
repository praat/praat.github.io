/* espeak_praat_FileInMemorySet_addVoices.cpp
 *
 * This file was automatically created from files in the folder `generate/espeak/data`
 * by the script `generate/espeak/GENERATE.praat` in the Praat source distribution.
 *
 * eSpeak NG version: 1.52-dev, downloaded 2024-08-24T19:38Z from https://github.com/espeak-ng/espeak-ng
 * File creation date: Mon Mar 30 10:08:24 2026
 *
 * Copyright (C) 2005-2014 Jonathan Duddington (for eSpeak)
 * Copyright (C) 2015-2023 Reese Dunn (for eSpeak-NG)
 * Copyright (C) 2012-2024 David Weenink, 2024,2026 Paul Boersma (for Praat)
 *
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see: <http://www.gnu.org/licenses/>.
 */

#include "espeak_praat.h"
#include "FileInMemory.h"

void espeak_praat_FileInMemorySet_addVoices (FileInMemorySet me) {
	try {
		static unsigned char fim1_data[129] = 
"language variant\nname Alex\n\nvoicing 70\npitch 105 115\nflutter 0\n\nformant 1 110 115 100\nformant 2 100 "
"110 100\nformant 3 100 80 75\n"
		;
		autoFileInMemory fim1 = FileInMemory_createWithData (128, fim1_data, true, U"./data/voices/!v/Alex");
		my addItem_move (fim1.move());

		static unsigned char fim2_data[475] = 
"language variant\r\nname Alicia\r\ngender female\r\npitch 180 275\r\necho 40 50\r\nformant 0 115 115 110\r\nform"
"ant 1 130 160 120\r\nformant 2 150 110 150\r\nformant 3 135 150 100\r\nformant 4 120 120 120\r\nformant 5 12"
"0 120 120\r\nformant 6 100 110 105\r\nformant 7 100 110 160\r\nformant 8 200 120 100\r\nintonation 2\r\nvoicin"
"g 38\r\nconsonants 100 20\r\nroughness 1\r\nstressAdd 1 64 64 50 50 100 100 200\r\nstressAmp 12 12 20 20 12 "
"12 20 20\r\nbreathw 150 150 200 200 400 400 600 600\r\nbreath 0 4 5 2 3 13 3 2"
		;
		autoFileInMemory fim2 = FileInMemory_createWithData (474, fim2_data, true, U"./data/voices/!v/Alicia");
		my addItem_move (fim2.move());

		static unsigned char fim3_data[358] = 
"language variant \nname Andrea\ngender female \n\npitch 200 265\nroughness 0\n\nformant 0 100  100 100\nform"
"ant 1 110  100 80\nformant 2 110  80 80\nformant 3 115  110 80\nformant 4 115  80 100\nformant 5 95  50 "
"100\nformant 6 0 0 0\nformant 7 120  100 100\nformant 8 110  100 100\nintonation 3\nstressLength 0 1 2 3 "
"4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nvoicing 150"
		;
		autoFileInMemory fim3 = FileInMemory_createWithData (357, fim3_data, true, U"./data/voices/!v/Andrea");
		my addItem_move (fim3.move());

		static unsigned char fim4_data[321] = 
"language variant\r\nname Andy\r\ngender Male\r\n\r\npitch 85 110\r\n\r\nflutter 0\r\nformant 0 80 80 80 80\r\nforman"
"t 1 100 100 100 120\r\nformant 2 100 88 100\r\nformant 3 0 0 0\r\nformant 4 80 80 80\r\nformant 5 80 80 80\r\n"
"formant 6 0 0 0\r\nformant 7 0 0 0\r\nformant 8 0 0 0\r\nstressAdd 0 0 0 0 0 0 0 200\r\nstressAmp 35 35 35 3"
"5 35 35 35 35 35\r\n\r\n"
		;
		autoFileInMemory fim4 = FileInMemory_createWithData (320, fim4_data, true, U"./data/voices/!v/Andy");
		my addItem_move (fim4.move());

		static unsigned char fim5_data[316] = 
"language variant\r\nname Annie\r\ngender female\r\npitch 120 280\r\n\r\nformant 0 105 125 120\r\nformant 1 120 1"
"40 120\r\nformant 2 120 150 140\r\nformant 3 130 150 130\r\nformant 4 120 120 110\r\nformant 5 120 120 110\r\n"
"formant 6 120 140 130\r\nformant 7 120 140 130\r\nformant 8 120 140 130\r\nintonation 1\r\nvoicing 30\r\nconso"
"nants 110 120\r\n"
		;
		autoFileInMemory fim5 = FileInMemory_createWithData (315, fim5_data, true, U"./data/voices/!v/Annie");
		my addItem_move (fim5.move());

		static unsigned char fim6_data[362] = 
"language variant\r\nname anxiousAndy\r\ngender Male\r\n\r\npitch 115 110\r\n\r\nflutter 0\r\nformant 0 80 80 80 80"
"\r\nformant 1 100 100 100 120\r\nformant 2 100 100 100\r\nformant 3 0 0 0\r\nformant 4 0 0 0\r\nformant 5 100 "
"100 100\r\nformant 6 100 100 100\r\nformant 7 100 100 100\r\nformant 8 100 100 100\r\nstressAdd 100 100 100 "
"100 100 100 100 300\r\nstressAmp 35 35 35 35 35 35 35 35 35\r\n\r\n"
		;
		autoFileInMemory fim6 = FileInMemory_createWithData (361, fim6_data, true, U"./data/voices/!v/AnxiousAndy");
		my addItem_move (fim6.move());

		static unsigned char fim7_data[3859] = 
"##Ten en cuenta que los 2 signos de n\303\272mero en este archivo tienen explicaciones de las comfiguracio"
"nes que puede aplicar y c\303\263mo comfigurarlas\r\n## Language Establece el idioma de la voz. Esta opci\303\263n"
" es necesaria para cualquier comfiguraci\303\263n que realices\r\n##La siguiente l\303\255nea es una configuraci\303\263"
"n que puede cambiar. Sin embargo, si no conoce los c\303\263digos de idioma, puede ser mejor dejar la conf"
"iguraci\303\263n tal y como est\303\241.\r\nlanguage variant\r\n\r\n## La configuraci\303\263n de nombre es el nombre que ap"
"arecer\303\241 en la configuraci\303\263n de voz en el cuadro combinado de variante.\r\n##La siguiente l\303\255nea es u"
"na opci\303\263n que puede cambiar\r\nname Demonic\r\n##La siguiente l\303\255nea establece el g\303\251nero de la voz. Ma"
"le or Female (hombre o mujer)\r\n##La siguiente l\303\255nea es una opci\303\263n que puede cambiar\r\ngender male\r\n"
"\r\nflutter 5\r\nstressAmp 20 18  20 20  20 22  22 22\r\n##Las opciones de formantes\r\n##  Formant 0 es usa"
"do para dar una baja frecuencia a los sonnidos\r\n## Los tres n\303\272meros son frecuencia, fuerza y ancho,"
" en orden. Ten en cuenta que los n\303\272meros est\303\241n separados por espacios\r\n##La siguiente l\303\255nea es un"
"a opci\303\263n que puede cambiar\r\nformant 0 100 100 100\r\n\r\n# Formant 1, 2, y 3 son las 3 formantes est\303\241n"
"dar para definir las vocales.\r\n##Las siguientes 3 l\303\255neas son opciones que puedes cambiar\r\nformant 1"
" 70 100 100\r\nformant 2 80 100 90\r\nformant 3 80 160 90\r\n\r\n# Formants 4 y 5 afectan a f3. Esto afectar"
"\303\241 la calidad de la voz.\r\n##Las siguientes 2 l\303\255neas son comfiguraciones que puede cambiar.\r\nformant"
" 4 80 85\r\nformant 5 100 100 80\r\n\r\n## Formantes 6, 7 y 8 son opciones que te ofrecen un sonido m\303\241s c"
"laro de las vocales\r\n##Las siguientes 3 l\303\255neas son opciones que puedes cambiar\r\nformant 6 80 80 100"
"\r\nformant 7 130 130 110\r\nformant 8 120 120 150\r\n\r\n##Intonation afecta el ascenso y la ca\303\255da de la v"
"oz\r\n## Las opciones son: 1 predeterminado, 2 entonaci\303\263n media, 3 entonaci\303\263n media y no afecta a la"
"s comas, 4 al final de la oraci\303\263n o punto aumenta el tono de la voz.\r\n##La siguiente l\303\255nea es una "
"opci\303\263n que puedes cambiar.\r\nintonation 10\r\n\r\n# Establecer el rango de tono. El primer n\303\272mero le da"
" un tono base a la voz (valor en hz). El segundo n\303\272mero controla el rango de tonos usado por la voz"
". Poni\303\251ndolo igual\r\n# si los 2 n\303\272meros son iguales, la voz ser\303\241 mon\303\263tona. Por defecto los ajuste"
"s son 82 y 118 \r\npitch 43 120\r\n## La configuraci\303\263n del tono. El primer n\303\272mero en la l\303\255nea de conf"
"iguraci\303\263n, 600, es la configuraci\303\263n de frecuencia para la cantidad de graves en la voz.\r\n## El seg"
"undo n\303\272mero en la l\303\255nea de tono es el volumen de la frecuencia de graves. Puede configurarlo de 0 "
"a 255, siendo 0 la menor cantidad, 255 la mayor.\r\n##El tercer n\303\272mero en la l\303\255nea de tono, 1200, es"
" la frecuencia de rango medio. El cuarto n\303\272mero en la l\303\255nea es la configuraci\303\263n para cambiar el v"
"olumen de la frecuencia de rango medio.\r\n##0 es la menor cantidad y 255 es la mayor.\r\n## El quinto n"
"\303\272mero en la l\303\255nea de tono, 2000, es la frecuencia de agudos. El sexto n\303\272mero es el volumen de la "
"frecuencia de agudos. 0 es el m\303\255nimo y 255 es el m\303\241ximo.\r\n##  Notar\303\241 que las 3 frecuencias est\303\241n"
" configuradas en 255.\r\n###La siguiente l\303\255nea es una opci\303\263n que puedes cambiar.\r\ntone 100 255 1200 "
"255 1500 255\r\necho 8 10000\r\nroughness 3\r\nbreath 20 5 2 10 5 0 27 100\r\nbreathw 255 255 60 180 160 255"
" 255 255\r\nconsonants 194 255\r\nvoicing 65\r\nstressLength 0 1 2 3 4 5 6 7\r\nstressAdd 250 350 700 500 45"
"0 290 100 225\r\nstressAmp 16 16 24 24 16 16 20 24\r\n##Este archivo no incluye todas las configuracione"
"s que se pueden usar para modificar una voz E Speak. Su objetivo es familiarizarlo con lo que hace l"
"a configuraci\303\263n.\r\n##Sin envargo puedes visitar la p\303\241gina http://espeak.sourceforge.net/voices.html"
" y consultar m\303\241s informaci\303\263n acerca de c\303\263mo agregar o cambiar otras configuraciones.\r\n## Espero q"
"ue te haya servido esta ayuda, y que te hayas divertido.\r\n"
		;
		autoFileInMemory fim7 = FileInMemory_createWithData (3858, fim7_data, true, U"./data/voices/!v/Demonic");
		my addItem_move (fim7.move());

		static unsigned char fim8_data[306] = 
"language variant\r\nname Denis\r\ngender male 35\r\npitch  80 115\r\nflutter 0\r\nroughness 0\r\n\r\n\r\nformant 0 1"
"00 160 160\r\nformant 1 95 95 95\r\nformant 2 100 100 100\r\nformant 3 90 90 90\r\nformant 4 40 40 40\r\nforma"
"nt 5 80 80 80\r\nformant 6 10 10 10\r\nformant 7 10 10 10\r\nformant 8 10 10 10\r\nvoicing 40\r\nconsonants 80"
" 80\r\n"
		;
		autoFileInMemory fim8 = FileInMemory_createWithData (305, fim8_data, true, U"./data/voices/!v/Denis");
		my addItem_move (fim8.move());

		static unsigned char fim9_data[380] = 
"language variant\nname Diogo\ngender male 25\npitch  82 122\necho 0 0\nflutter 0\nroughness 0\nstressAmp 20"
" 18  20 20  20 22  22 22\n\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140\nformant "
"3 95 95 140\nformant 4 30 30 30 -100\nformant 5 90 90 90\nformant 6 110 60 65\nformant 7 100 0 100\nforma"
"nt 8 100 0 100\nvoicing 35\nconsonants 60 40\ntone 60 250 140 100 1000 50 3500 35\n"
		;
		autoFileInMemory fim9 = FileInMemory_createWithData (379, fim9_data, true, U"./data/voices/!v/Diogo");
		my addItem_move (fim9.move());

		static unsigned char fim10_data[282] = 
"language variant\r\nname Gene\r\n\r\npitch  80 110\r\n\r\nformant 0 120 120 120\r\nformant 1 90 100 110\r\nformant"
" 2 100 100 95\r\nformant 3 90 100 100\r\nformant 4 90 100 110\r\nformant 5 90 110 110\r\nformant 6 100 70 10"
"0\r\nformant 7 100 70 100\r\nformant 8 100 80 100\r\nvoicing 120\r\nconsonants 50 110\r\n\r\n"
		;
		autoFileInMemory fim10 = FileInMemory_createWithData (281, fim10_data, true, U"./data/voices/!v/Gene");
		my addItem_move (fim10.move());

		static unsigned char fim11_data[284] = 
"language variant\r\nname Gene2\r\n\r\npitch  100 130\r\n\r\nformant 0 120 120 120\r\nformant 1 90 100 110\r\nforma"
"nt 2 100 100 95\r\nformant 3 90 100 100\r\nformant 4 90 100 110\r\nformant 5 90 110 110\r\nformant 6 100 70 "
"100\r\nformant 7 100 70 100\r\nformant 8 100 80 100\r\nvoicing 120\r\nconsonants 50 110\r\n\r\n"
		;
		autoFileInMemory fim11 = FileInMemory_createWithData (283, fim11_data, true, U"./data/voices/!v/Gene2");
		my addItem_move (fim11.move());

		static unsigned char fim12_data[382] = 
"language variant\nname Henrique\ngender male 25\npitch  70 130\necho 0 0\nflutter 0\nroughness 0\nstressAmp"
" 20 18  20 20  20 22  22 22\n\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140\nforma"
"nt 3 95 95 140\nformant 4 30 30 30 -100\nformant 5 90 90 90\nformant 6 110 60 65\nformant 7 100 0 100\nfo"
"rmant 8 100 0 100\nvoicing 35\nconsonants 60 40\ntone 70 250 230 80 1100 30 3500 40\n"
		;
		autoFileInMemory fim12 = FileInMemory_createWithData (381, fim12_data, true, U"./data/voices/!v/Henrique");
		my addItem_move (fim12.move());

		static unsigned char fim13_data[379] = 
"language variant\nname Hugo\ngender male 25\npitch  70 130\necho 0 0\nflutter 0\nroughness 0\nstressAmp 20 "
"18  20 20  20 22  22 22\n\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140\nformant 3"
" 95 95 140\nformant 4 30 30 30 -100\nformant 5 90 90 90\nformant 6 110 60 65\nformant 7 100 0 100\nforman"
"t 8 100 0 100\nvoicing 35\nconsonants 60 40\ntone 400 160 1100 90 3500 90 150 35\n"
		;
		autoFileInMemory fim13 = FileInMemory_createWithData (378, fim13_data, true, U"./data/voices/!v/Hugo");
		my addItem_move (fim13.move());

		static unsigned char fim14_data[268] = 
"language variant\r\nname Jacky\r\n\r\npitch  85 130\r\n\r\nformant 0 150 155 100\r\nformant 1 90 155 70\r\nformant"
" 2 95 70 64\r\nformant 3 15 20 30\r\nformant 4 20 30 40\r\nformant 5 65 20 65\r\nformant 6 70 80 100\r\nforman"
"t 7 20 80 100\r\nformant 8 100 95 80\r\nvoicing 135\r\nconsonants 110\r\n\r\n"
		;
		autoFileInMemory fim14 = FileInMemory_createWithData (267, fim14_data, true, U"./data/voices/!v/Jacky");
		my addItem_move (fim14.move());

		static unsigned char fim15_data[339] = 
"language variant\r\nname Lee\r\ngender Male\r\n\r\n#echo 230 30\r\npitch 85 110\r\n\r\nflutter 0\r\nformant 0 80 80 "
"80 80\r\nformant 1 80 80 100 100\r\nformant 2 80 80 80\r\nformant 3 9 9 9\r\nformant 4 290 290\r\nformant 5 13"
"0 0 0\r\nformant 6 90 90 90\r\nformant 7 90 90 90\r\nformant 8 90 90 90\r\nstressAdd 0 0 0 200 0 0 0 100\r\nst"
"ressAmp 30 30 30 30 30 30 30 30 30\r\n\r\n"
		;
		autoFileInMemory fim15 = FileInMemory_createWithData (338, fim15_data, true, U"./data/voices/!v/Lee");
		my addItem_move (fim15.move());

		static unsigned char fim16_data[468] = 
"language variant\r\nname Marco\r\ngender male 30\r\nintonation 1\r\npitch  100 152\r\necho 50 80\r\nflutter 2\r\nr"
"oughness 0\r\nstressAmp 25 25  24 20  38 31  39 27\r\nstressAdd 250 125 250 250 225 145 50 256\r\n\r\nforman"
"t 0 100 120 130\r\nformant 1 75 180 170\r\nformant 2 92 120 110\r\nformant 3 140 120 110\r\nformant 4 10 20 "
"20 -50\r\nformant 5 110 70 20\r\nformant 6 140 100 98\r\nformant 7 130 120 115\r\nformant 8 105 120 108\r\nvoi"
"cing 38\r\nconsonants 90 140\r\ntone 420 150 1200 135 3000 70 4700 40\r\n"
		;
		autoFileInMemory fim16 = FileInMemory_createWithData (467, fim16_data, true, U"./data/voices/!v/Marco");
		my addItem_move (fim16.move());

		static unsigned char fim17_data[271] = 
"language variant\r\nname Mario\r\n\r\npitch  75 125\r\n\r\nformant 0 100 111 95\r\nformant 1 100 111 60\r\nformant"
" 2 95 90 55\r\nformant 3 100 50 65\r\nformant 4 69 65 65\r\nformant 5 79 60 75\r\nformant 6 89 60 75\r\nforman"
"t 7 99 0 100\r\nformant 8 109 0 100\r\nvoicing 135\r\nconsonants 115 120\r\n\r\n"
		;
		autoFileInMemory fim17 = FileInMemory_createWithData (270, fim17_data, true, U"./data/voices/!v/Mario");
		my addItem_move (fim17.move());

		static unsigned char fim18_data[271] = 
"language variant\r\nname Michael\r\n\r\npitch  75 125\r\n\r\nformant 0 105 111 95\r\nformant 1 85 111 60\r\nforman"
"t 2 95 90 55\r\nformant 3 59 50 65\r\nformant 4 69 65 65\r\nformant 5 79 60 75\r\nformant 6 89 60 75\r\nforman"
"t 7 99 0 100\r\nformant 8 109 0 100\r\nvoicing 135\r\nconsonants 115 120\r\n\r\n"
		;
		autoFileInMemory fim18 = FileInMemory_createWithData (270, fim18_data, true, U"./data/voices/!v/Michael");
		my addItem_move (fim18.move());

		static unsigned char fim19_data[113] = 
"language variant\nname Mike\nvoicing 70\nformant 1 96 97 100\nformant 2 96 97 100\nformant 5 95 103 100\np"
"itch 67 107\n"
		;
		autoFileInMemory fim19 = FileInMemory_createWithData (112, fim19_data, true, U"./data/voices/!v/Mike");
		my addItem_move (fim19.move());

		static unsigned char fim20_data[3194] = 
"##Please note the 2 number signs, or pound signs in this file are for comments to help you to unders"
"tand what the settings are and how to set them.  \r\n## Language sets the language of your voice.  Thi"
"s setting is required for every voice that you make.\r\n##The next line is a setting you can change.  "
"However if you don't know the language codes it may be best to leave the setting as it is.\r\nlanguage"
" variant\r\n\r\n## The name setting is the name that will show up in the voice settings in the variant c"
"ombo box.\r\n##The next line is a setting you can change\r\nname Mr_Serious\r\n\r\n##The formant settings\r\n#"
"#  Formant 0 is used to give a low frequency component to the sounds.\r\n## The three numbers are freq"
"uency, strength, and Width, in that order.  Please note, the numbers are separated by a space.\r\n##Th"
"e next line is a setting you can change\r\nformant 0 100 100 100\r\n\r\n# Formants 1,2, and 3 are the stan"
"dard three formants which define vowels. \r\n##The next 3 lines are settings you can change\r\nformant 1"
" 100 100 100\r\nformant 2 100 100 100\r\nformant 3 87 100 100\r\n\r\n# Formants 4,5 are higher than F3. They"
" affect the quality of the voice. \r\n##The next 2 lines are settings that you can change.\r\nformant 4 "
"100 100 100\r\nformant 5 100 100 100\r\n\r\n## Formants 6, 7, and 8 are weak, high frequency, additions to"
" vowels to give a clearer sound. \r\n##The next 3 lines are settings that you can change.\r\nformant 6 1"
"00 100 100\r\nformant 7 100 100 100\r\nformant 8 100 100 100\r\n\r\n##Intonation affects the rise and fall o"
"f the voice\t\r\n## The settings are 1 default, 2 less intonation, 3 less intonation and commas do not "
"raise the pitch, 4 the pitch rises at the end of a sentence rather than falling.\r\n##The next line is"
" a setting you can change.\r\nintonation 1\r\n\r\n# Setting the pitch range.  The first number gives a bas"
"e pitch to the voice (value in Hertz).  The second number controls the range of pitches used by the "
"voice. Setting it equal\r\n# to the first number will give a monotone sounding voice.  The default val"
"ues are 82 and 118. \r\npitch 82 118\r\n## The tone setting.  The first number on the setting line, 600,"
" is the frequency setting for the amount of bass in the voice. \r\n## The second number on the tone li"
"ne is the volume of the bass frequency.  You can set it from 0 to 255, 0 being the least amount, 255"
" being the most.\r\n##The third number on the tone line, 1200, is the mid range frequency.  The fourth"
" number on the line is the setting to change the volume of the mid range frequency.\r\n##0 being the l"
"east amount and 255 being the maximum.\r\n## The fifth number on the tone line, 2000, is the treble fr"
"equency.  The sixth number is the volume of the treble frequency.  0 is the minimum and 255 is the m"
"aximum.\r\n##  You will notice that all 3 frequencies are set to 255.\r\n##The next line is a setting th"
"at you can change.\r\ntone 600 255 1200 255 2000 255\r\n##This file does not include all of the settings"
" that can be used to modify an E Speak voice.  It is intended to get you familiar with what the sett"
"ings do.\r\n##However, you can go to http://espeak.sourceforge.net/voices.html and read further inform"
"ation about other settings that can be added and changed.  I hope this helps, and Have fun.\r\n"
		;
		autoFileInMemory fim20 = FileInMemory_createWithData (3193, fim20_data, true, U"./data/voices/!v/Mr serious");
		my addItem_move (fim20.move());

		static unsigned char fim21_data[281] = 
"language variant\nname Nguyen\n\npitch 95 175\n\nformant 0 100 125 100\nformant 1 96 90 80\nformant 2 97 70"
" 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  75 50 90\nformant 6  90 50 100\nformant 7 100 50 "
"100\nformant 8 100 50 100\n\ntone 100 200 600 150 800 100 2400 80 3600 95 5400 100\n"
		;
		autoFileInMemory fim21 = FileInMemory_createWithData (280, fim21_data, true, U"./data/voices/!v/Nguyen");
		my addItem_move (fim21.move());

		static unsigned char fim22_data[203] = 
"language variant\nname Reed\nklatt 6\nconsonants 85 85\nvoicing 130\nbreath 45\n\npitch 85 135\n\nformant 1 7"
"2 100 90 90\nformant 2 83 100 75 180\nformant 3 98 100 100 90\nformant 4 98 100 90\nformant 5 100 100 90"
"\n\n"
		;
		autoFileInMemory fim22 = FileInMemory_createWithData (202, fim22_data, true, U"./data/voices/!v/Reed");
		my addItem_move (fim22.move());

		static unsigned char fim23_data[234] = 
"language variant\nname RicishayMax\necho 100 10000\n\nformant 0 90 120 100\nformant 1 100 100 75\nformant "
"2 100 100 75\nformant 3 100 80 75\nformant 4 100 80 75\nformant 5 100 80 75\nformant 6 100 0 75\nformant "
"7 100 0 75\nformant 8 100 0 75\n\n\n\n"
		;
		autoFileInMemory fim23 = FileInMemory_createWithData (233, fim23_data, true, U"./data/voices/!v/RicishayMax");
		my addItem_move (fim23.move());

		static unsigned char fim24_data[436] = 
"language variant\nname RicishayMax2\necho 150 500\n\nformant 0 90 120 100\nformant 1 100 100 75\nformant 2"
" 100 100 75\nformant 3 100 80 75\nformant 4 100 80 75\nformant 5 100 80 75\nformant 6 100 0 75\nformant 7"
" 100 0 75\nformant 8 100 0 75\n\n\nroughness 5\n\nintonation 10\nvoicing 150\nconsonants 110 120\nstressLengt"
"h 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 24\n\ntone 100 "
"255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim24 = FileInMemory_createWithData (435, fim24_data, true, U"./data/voices/!v/RicishayMax2");
		my addItem_move (fim24.move());

		static unsigned char fim25_data[436] = 
"language variant\nname RicishayMax3\necho 200 500\n\nformant 0 90 120 100\nformant 1 100 100 75\nformant 2"
" 100 100 75\nformant 3 100 80 75\nformant 4 100 80 75\nformant 5 100 80 75\nformant 6 100 0 75\nformant 7"
" 100 0 75\nformant 8 100 0 75\n\n\nroughness 5\n\nintonation 10\nvoicing 150\nconsonants 110 120\nstressLengt"
"h 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 24\n\ntone 100 "
"255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim25 = FileInMemory_createWithData (435, fim25_data, true, U"./data/voices/!v/RicishayMax3");
		my addItem_move (fim25.move());

		static unsigned char fim26_data[421] = 
"language variant\r\nlanguage en-us\r\nname Storm\r\ngender male\r\nformant 0 100 100 100\r\nformant 1 95 95 95"
"\r\nformant 2 95 95 95\r\nformant 3 95 95 95\r\nformant 4 70 70 70\r\nformant 5 70 70 70\r\nformant 6 25 25 25"
"\r\nformant 7 25 25 25\r\nformant 8 25 25 25\r\nbreath 0 0 0 0 0 0 0 0\r\nconsonants 100\r\necho 0 0\r\nflutter "
"0\r\nintonation 3\r\npitch 60 100\r\nroughness 0\r\nstressAdd 5 5 3 3 0 0 -15 -15\r\ntone 500 255 1500 255 250"
"0 255\r\nvoicing 100\r\n"
		;
		autoFileInMemory fim26 = FileInMemory_createWithData (420, fim26_data, true, U"./data/voices/!v/Storm");
		my addItem_move (fim26.move());

		static unsigned char fim27_data[3190] = 
"##Please note the 2 number signs, or pound signs in this file are for comments to help you to unders"
"tand what the settings are and how to set them.  \r\n## Language sets the language of your voice.  Thi"
"s setting is required for every voice that you make.\r\n##The next line is a setting you can change.  "
"However if you don't know the language codes it may be best to leave the setting as it is.\r\nlanguage"
" variant\r\n\r\n## The name setting is the name that will show up in the voice settings in the variant c"
"ombo box.\r\n##The next line is a setting you can change\r\nname Tweaky\r\n\r\n##The formant settings\r\n##  F"
"ormant 0 is used to give a low frequency component to the sounds.\r\n## The three numbers are frequenc"
"y, strength, and Width, in that order.  Please note, the numbers are separated by a space.\r\n##The ne"
"xt line is a setting you can change\r\nformant 0 100 100 100\r\n\r\n# Formants 1,2, and 3 are the standard"
" three formants which define vowels. \r\n##The next 3 lines are settings you can change\r\nformant 1 100"
" 100 100\r\nformant 2 100 100 100\r\nformant 3 200 100 100\r\n\r\n# Formants 4,5 are higher than F3. They af"
"fect the quality of the voice. \r\n##The next 2 lines are settings that you can change.\r\nformant 4 100"
" 100 100\r\nformant 5 100 100 100\r\n\r\n## Formants 6, 7, and 8 are weak, high frequency, additions to vo"
"wels to give a clearer sound. \r\n##The next 3 lines are settings that you can change.\r\nformant 6 100 "
"100 100\r\nformant 7 100 100 100\r\nformant 8 100 100 100\r\n\r\n##Intonation affects the rise and fall of t"
"he voice\t\r\n## The settings are 1 default, 2 less intonation, 3 less intonation and commas do not rai"
"se the pitch, 4 the pitch rises at the end of a sentence rather than falling.\r\n##The next line is a "
"setting you can change.\r\nintonation 1\r\n\r\n# Setting the pitch range.  The first number gives a base p"
"itch to the voice (value in Hertz).  The second number controls the range of pitches used by the voi"
"ce. Setting it equal\r\n# to the first number will give a monotone sounding voice.  The default values"
" are 82 and 118. \r\npitch 82 118\r\n## The tone setting.  The first number on the setting line, 600, is"
" the frequency setting for the amount of bass in the voice.\r\n## The second number on the tone line i"
"s the volume of the bass frequency.  You can set it from 0 to 255, 0 being the least amount, 255 bei"
"ng the most.\r\n##The third number on the tone line, 1200, is the mid range frequency.  The fourth num"
"ber on the line is the setting to change the volume of the mid range frequency.\r\n##0 being the least"
" amount and 255 being the maximum.\r\n## The fifth number on the tone line, 2000, is the treble freque"
"ncy.  The sixth number is the volume of the treble frequency.  0 is the minimum and 255 is the maxim"
"um.\r\n##  You will notice that all 3 frequencies are set to 255.\r\n##The next line is a setting that y"
"ou can change.\r\ntone 600 255 1200 255 2000 255\r\n##This file does not include all of the settings tha"
"t can be used to modify an E Speak voice.  It is intended to get you familiar with what the settings"
" do.\r\n##However, you can go to http://espeak.sourceforge.net/voices.html and read further informatio"
"n about other settings that can be added and changed.  I hope this helps, and Have fun.\r\n"
		;
		autoFileInMemory fim27 = FileInMemory_createWithData (3189, fim27_data, true, U"./data/voices/!v/Tweaky");
		my addItem_move (fim27.move());

		static unsigned char fim28_data[418] = 
"language variant\r\nname UniversalRobot\r\ngender male\r\nklatt 4\r\npitch  100 160\r\necho 10 10000\r\nformant "
"1 75 120 135\r\nformant 2 90 50 140\r\nformant 3 70 85 95\r\nformant 4 150 60 80\r\nformant 5 100 85 80\r\nfor"
"mant 6 112 100 80\r\nformant 7 110 95 100\r\nformant 8 105 110 100\r\nconsonants 125 100\r\ntone 530 250 770"
" 100 215 225\r\nstressLength 0 1 2 3 4 5 6 7\r\nstressAdd 120 130 130 90 0 0 120 120\r\nstressAmp 16 16 24"
" 24 16 16 20 24\r\n"
		;
		autoFileInMemory fim28 = FileInMemory_createWithData (417, fim28_data, true, U"./data/voices/!v/UniRobot");
		my addItem_move (fim28.move());

		static unsigned char fim29_data[76] = 
"language variant\nname Adam\nklatt 6\nconsonants 85 85\n\nformant 1 100 100 130\n"
		;
		autoFileInMemory fim29 = FileInMemory_createWithData (75, fim29_data, true, U"./data/voices/!v/adam");
		my addItem_move (fim29.move());

		static unsigned char fim30_data[494] = 
"language variant\nname anika\ngender female\npitch 200 300\nflutter 6\nstressAmp 20 18  20 20  20 22  22 "
"22\n\nroughness 0\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140\nformant 3 95 95 14"
"0\nformant 4 120 120 110\nformant 5 120 120 110\nformant 6 110 60 65\nformant 7 100 0 100\nformant 8 100 "
"0 100\nintonation 10\nvoicing 30\nconsonants 60 40\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 1"
"00 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 24\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim30 = FileInMemory_createWithData (493, fim30_data, true, U"./data/voices/!v/anika");
		my addItem_move (fim30.move());

		static unsigned char fim31_data[513] = 
"language variant\nname anikaRobot\ngender female\npitch 200 300\nflutter 1\nstressAmp 20 18  20 20  20 22"
"  22 22\necho 10 10000\n\nroughness 0\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140"
"\nformant 3 95 95 140\nformant 4 120 120 110\nformant 5 120 120 110\nformant 6 110 60 65\nformant 7 100 0"
" 100\nformant 8 100 0 100\nintonation 10\nvoicing 30\nconsonants 60 40\nstressLength 0 1 2 3 4 5 6 7\nstre"
"ssAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 24\ntone 100 255 600 70 1200 22 2000"
" 66 3000 12\n"
		;
		autoFileInMemory fim31 = FileInMemory_createWithData (512, fim31_data, true, U"./data/voices/!v/anikaRobot");
		my addItem_move (fim31.move());

		static unsigned char fim32_data[301] = 
"name Half-LifeAnnouncementSystem\nlanguage variant\npitch 37 83\nklatt 4\n\nformant 1 88 100 100 0\nforman"
"t 2 96 100 100 0\nformant 3 98 100 100 0\nformant 4 96 100 100 0\nformant 5 100 100 100 0\nformant 6 100"
" 100 100 0\nformant 7 100 100 100 0\nformant 8 100 100 100 0\n\nvoicing 70\nconsonants 70 70\necho 154 26\n"
		;
		autoFileInMemory fim32 = FileInMemory_createWithData (300, fim32_data, true, U"./data/voices/!v/announcer");
		my addItem_move (fim32.move());

		static unsigned char fim33_data[382] = 
"language variant \nname Antonio\ngender male\n\npitch 82 128\nroughness 0\n\nformant 0 100  150 90\nformant "
"1 90  130 90\nformant 2 95  120 80\nformant 3 100  50 80\nformant 4 100  40 80\nformant 5 90  70 80\nform"
"ant 6 0  0 0\nformant 7 100  100 100\nformant 8 100  100 100\nvoicing 150\ntone 600 255 1200 255 2000 80"
"\nintonation 3\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\n"
		;
		autoFileInMemory fim33 = FileInMemory_createWithData (381, fim33_data, true, U"./data/voices/!v/antonio");
		my addItem_move (fim33.move());

		static unsigned char fim34_data[359] = 
"language variant\r\nname Auntie\r\ngender female\r\npitch 204 176\r\nflutter 12\r\n\r\nformant 0 88 85 154\r\nform"
"ant 1 115 80 160 -20\r\nformant 2 130 75 150 -200\r\nformant 3 123 75 150\r\nformant 4 125 80 150\r\nformant"
" 5 125 80 150\r\nformant 6 110 80 150\r\nformant 7 110 75 150\r\nformant 8 110 75 150\r\n\r\nstressAdd -20 -20"
" -20 -20 0 0 20 120\r\nstressAmp 18 16 20 20 20 20 20 20\r\n\r\n"
		;
		autoFileInMemory fim34 = FileInMemory_createWithData (358, fim34_data, true, U"./data/voices/!v/aunty");
		my addItem_move (fim34.move());

		static unsigned char fim35_data[341] = 
"language variant\nname Belinda\ngender female\n\npitch 200 247  \nflutter 3\n\nformant 0 88 85 154\nformant "
"1 135 58 169 -30\nformant 2 120 70 150 -260\nformant 3 120 39 150\nformant 4 125 57 80\nformant 5 125 80"
" 150\nformant 6 110 80 150\nformant 7 110 75 150\nformant 8 110 75 150\n\nstressAdd -20 -20 -20 -20 0 3 2"
"0 12\nstressAmp 18 16 20 20 10 20 27 20\n\n"
		;
		autoFileInMemory fim35 = FileInMemory_createWithData (340, fim35_data, true, U"./data/voices/!v/belinda");
		my addItem_move (fim35.move());

		static unsigned char fim36_data[202] = 
"language variant\nname Benjamin\nklatt 6\nconsonants 70 70\n\nformant 1 101 100 130\nformant 2 102 100 100"
"\nformant 3 100 100 100\nformant 4 100 100 100 470\nformant 5 100 100 100 350\nformant 6 100 100 100 100"
"\n"
		;
		autoFileInMemory fim36 = FileInMemory_createWithData (201, fim36_data, true, U"./data/voices/!v/benjamin");
		my addItem_move (fim36.move());

		static unsigned char fim37_data[225] = 
"language variant\r\nname Boris\r\n\r\nformant 0 47 120 100\r\nformant 1 100 90 75\r\nformant 2 104 100 75\r\nfor"
"mant 3 57 80 75\r\nformant 4 104 80 75\r\nformant 5 107 80 75\r\nformant 6 68 0 75\r\nformant 7 105 0 75\r\nfo"
"rmant 8 105 0 75\r\n\r\n\r\n\r\n"
		;
		autoFileInMemory fim37 = FileInMemory_createWithData (224, fim37_data, true, U"./data/voices/!v/boris");
		my addItem_move (fim37.move());

		static unsigned char fim38_data[58] = 
"language variant\nname Caleb\nklatt 6\nbreath 100\nvoicing 0\n"
		;
		autoFileInMemory fim38 = FileInMemory_createWithData (57, fim38_data, true, U"./data/voices/!v/caleb");
		my addItem_move (fim38.move());

		static unsigned char fim39_data[94] = 
"language variant\nname croak\ngender male 70\n\npitch 85 117\nflutter 20\n\nformant 0 100 80 110\n\n\n\n"
		;
		autoFileInMemory fim39 = FileInMemory_createWithData (93, fim39_data, true, U"./data/voices/!v/croak");
		my addItem_move (fim39.move());

		static unsigned char fim40_data[113] = 
"language variant\nname David\nklatt 6\npitch 62 89\n\nformant 1 75 100 100\nformant 2 85 100 100\nformant 3"
" 85 100 100\n"
		;
		autoFileInMemory fim40 = FileInMemory_createWithData (112, fim40_data, true, U"./data/voices/!v/david");
		my addItem_move (fim40.move());

		static unsigned char fim41_data[288] = 
"language variant\nname Ed\n\npitch 90 145\n\nformant 0 110 120 200 5\nformant 1 102 100 80\nformant 2 101 1"
"20 100\nformant 3 100 80 75\nformant 4 150 30 80\nformant 5 95 95 155\nformant 6 167 100 75\nformant 7 10"
"0 200 75\nformant 8 60 200 95\nconsonants 55 80\nvoicing 100\ntone 650 250 1000 130 240 255"
		;
		autoFileInMemory fim41 = FileInMemory_createWithData (287, fim41_data, true, U"./data/voices/!v/ed");
		my addItem_move (fim41.move());

		static unsigned char fim42_data[152] = 
"language variant\nname Edward\nklatt 5\nvoicing 100\nconsonants 70 80\n\nformant 1 92 100 130\nformant 2 10"
"3 100 80\nformant 3 103 100 70\nformant 4 114 100 60\n"
		;
		autoFileInMemory fim42 = FileInMemory_createWithData (151, fim42_data, true, U"./data/voices/!v/edward");
		my addItem_move (fim42.move());

		static unsigned char fim43_data[153] = 
"language variant\nname Edward2\nklatt 6\nvoicing 100\nconsonants 70 80\n\nformant 1 92 100 130\nformant 2 1"
"03 100 80\nformant 3 103 100 70\nformant 4 114 100 60\n"
		;
		autoFileInMemory fim43 = FileInMemory_createWithData (152, fim43_data, true, U"./data/voices/!v/edward2");
		my addItem_move (fim43.move());

		static unsigned char fim44_data[325] = 
"language variant\nname female1\ngender female 70\n\npitch 140 200\nflutter 8\nroughness 4\nformant 0 115  8"
"0 150\nformant 1 120  80 180\nformant 2 100  70 150  150\nformant 3 115  70 150\nformant 4 110  80 150\nf"
"ormant 5 110  90 150\nformant 6 105  80 150\nformant 7 110  70 150\nformant 8 110  70 150\n\nstressAdd -1"
"0 -10 -20 -20 0 0 40 60\n"
		;
		autoFileInMemory fim44 = FileInMemory_createWithData (324, fim44_data, true, U"./data/voices/!v/f1");
		my addItem_move (fim44.move());

		static unsigned char fim45_data[358] = 
"language variant\nname female2\ngender female\n\npitch 142 220\nroughness 3\n\nformant 0 105  80 150\nforman"
"t 1 110  80 160\nformant 2 110  70 150\nformant 3 110  70 150\nformant 4 115  80 150\nformant 5 115  80 "
"150\nformant 6 110  70 150\nformant 7 110  70 150\nformant 8 110  70 150\n\nstressAdd 0 0 -10 -10 0 0 10 "
"40\nbreath 0 2 3 3 3 3 3 2\necho 140 10\nconsonants 125 125\n"
		;
		autoFileInMemory fim45 = FileInMemory_createWithData (357, fim45_data, true, U"./data/voices/!v/f2");
		my addItem_move (fim45.move());

		static unsigned char fim46_data[376] = 
"language variant\nname female3\ngender female\n\npitch 140 240\nformant 0 105  80 150\nformant 1 120  75 1"
"50 -50\nformant 2 135  70 150 -250\nformant 3 125  80 150\nformant 4 125  80 150\nformant 5 125  80 150\n"
"formant 6 120  70 150\nformant 7 110  70 150\nformant 8 110  70 150\n\nstressAmp 18 18 20 20 20 20 20 20"
"\n//breath 0 2 4 4 4 4 4 4\nbreath 0 2 3 3 3 3 3 2\necho 120 10\nroughness 4\n\n\n"
		;
		autoFileInMemory fim46 = FileInMemory_createWithData (375, fim46_data, true, U"./data/voices/!v/f3");
		my addItem_move (fim46.move());

		static unsigned char fim47_data[351] = 
"language variant\nname female4\ngender female\n\necho 130 15\npitch 142 200\nformant 0 120  80 150\nformant"
" 1 115  80 160 -20\nformant 2 130  75 150 -200\nformant 3 123  75 150\nformant 4 125  80 150\nformant 5 "
"125  80 150\nformant 6 110  80 150\nformant 7 110  75 150\nformant 8 110  75 150\n\nstressAdd -20 -20 -20"
" -20 0 0 20 120\nstressAmp 18 16 20 20 20 20 20 20\n"
		;
		autoFileInMemory fim47 = FileInMemory_createWithData (350, fim47_data, true, U"./data/voices/!v/f4");
		my addItem_move (fim47.move());

		static unsigned char fim48_data[433] = 
"language variant \nname female5\ngender female \n\npitch 160 228\nroughness 0\n\nformant 0 105  80 150 \nfor"
"mant 1 110  80 160 \nformant 2 110  70 150 \nformant 3 110  70 150 \nformant 4 115  80 200 \nformant 5 1"
"15  80 100 \nformant 6 110  70 150 \nformant 7 110  70 100 \nformant 8 110  70 150 \n\nstressAdd 0 0 -10 "
"-10 0 0 10 40 \nbreath 0 4  6   6   6   6  0 10 \necho 140 10 \nvoicing 75 \nconsonants 150 150\nbreathw "
"150 150 200 200 400 400 600 600\n"
		;
		autoFileInMemory fim48 = FileInMemory_createWithData (432, fim48_data, true, U"./data/voices/!v/f5");
		my addItem_move (fim48.move());

		static unsigned char fim49_data[150] = 
"language variant\nname fast_test\n\n// Try decreasing these values to make eSpeak's fastest speed faste"
"r.\n// This is currently unstable.\n\nfast_test2 15\n"
		;
		autoFileInMemory fim49 = FileInMemory_createWithData (149, fim49_data, true, U"./data/voices/!v/fast");
		my addItem_move (fim49.move());

		static unsigned char fim50_data[264] = 
"language variant\n\nname grandma\ngender female 90\npitch 120 230\n\nflutter 20\nformant 0 105 150 150\nform"
"ant 1 100 80 100\nformant 2 105 105 105\nformant 3 80 80 80\nformant 4 60 60 60\nformant 5 90 90 90\nform"
"ant 6 10 10 10\nformant 7 10 10 10\nformant 8 20 20 20\nvoicing 50"
		;
		autoFileInMemory fim50 = FileInMemory_createWithData (263, fim50_data, true, U"./data/voices/!v/grandma");
		my addItem_move (fim50.move());

		static unsigned char fim51_data[257] = 
"language variant\nname grandpa\npitch 80 120\nflutter 20\nformant 0 100 100 100\nformant 1 100 100 100\nfo"
"rmant 2 100 100 100\nformant 3 100 100 100\nformant 4 100 100 100\nformant 5 100 100 100\nformant 6 10 1"
"0 10\nformant 7 10 10 10\nformant 8 10 10 10\nintonation 1\n"
		;
		autoFileInMemory fim51 = FileInMemory_createWithData (256, fim51_data, true, U"./data/voices/!v/grandpa");
		my addItem_move (fim51.move());

		static unsigned char fim52_data[254] = 
"language variant\nname Gustave\n\npitch  80 123\n\nformant 0 85 141 135\nformant 1 77 131 45\nformant 2 92 "
"70 55\nformant 3 59 50 65\nformant 4 69 65 65\nformant 5 79 60 75\nformant 6 89 60 75\nformant 7 99 0 100"
"\nformant 8 109 0 100\nvoicing 135\nconsonants 115 120\n\n"
		;
		autoFileInMemory fim52 = FileInMemory_createWithData (253, fim52_data, true, U"./data/voices/!v/gustave");
		my addItem_move (fim52.move());

		static unsigned char fim53_data[3169] = 
"##Please note the 2 number signs, or pound signs in this file are for comments to help you to unders"
"tand what the settings are and how to set them.  \r\n## Language sets the language of your voice.  Thi"
"s setting is required for every voice that you make.\r\n##The next line is a setting you can change.  "
"However if you don't know the language codes it may be best to leave the setting as it is.\r\nlanguage"
" variant\r\n\r\n## The name setting is the name that will show up in the voice settings in the variant c"
"ombo box.\r\n##The next line is a setting you can change\r\nname Ian\r\n\r\n##The formant settings\r\n##  Form"
"ant 0 is used to give a low frequency component to the sounds.\r\n## The three numbers are frequency, "
"strength, and Width, in that order.  Please note, the numbers are separated by a space.\r\n##The next "
"line is a setting you can change\r\nformant 0 20 120 50\r\n\r\n# Formants 1,2, and 3 are the standard thre"
"e formants which define vowels. \r\n##The next 3 lines are settings you can change\r\nformant 1 80 80 80"
"\r\nformant 2 80 80 80\r\nformant 3 80 80 80\r\n\r\n# Formants 4,5 are higher than F3. They affect the quali"
"ty of the voice. \r\n##The next 2 lines are settings that you can change.\r\nformant 4 50 50 50\r\nformant"
" 5 50 50 50\r\n\r\n## Formants 6, 7, and 8 are weak, high frequency, additions to vowels to give a clear"
"er sound. \r\n##The next 3 lines are settings that you can change.\r\nformant 6 100 100 100\r\nformant 7 2"
"00 50 200\r\nformant 8 200 50 200\r\n\r\n##Intonation affects the rise and fall of the voice\t\r\n## The sett"
"ings are 1 default, 2 less intonation, 3 less intonation and commas do not raise the pitch, 4 the pi"
"tch rises at the end of a sentence rather than falling.\r\n##The next line is a setting you can change"
".\r\nintonation 2\r\n\r\n# Setting the pitch range.  The first number gives a base pitch to the voice (val"
"ue in Hertz).  The second number controls the range of pitches used by the voice.\r\n# Setting it equa"
"l to the first number will give a monotone sounding voice.  The default values are 82 and 118. \r\npit"
"ch 69 96\r\n\r\n## The tone setting.  The first number on the setting line, 600, is the frequency settin"
"g for the amount of bass in the voice.\r\n##The second number on the tone line is the volume of the ba"
"ss frequency.  You can set it from 0 to 255, 0 being the least amount, 255 being the most.\r\n##The th"
"ird number on the tone line, 1200, is the mid range frequency.  The fourth number on the line is the"
" setting to change the volume of the mid range frequency.\r\n##0 being the least amount and 255 being "
"the maximum.\r\n## The fifth number on the tone line, 2000, is the treble frequency.  The sixth number"
" is the volume of the treble frequency.  0 is the minimum and 255 is the maximum.\r\n##  You will noti"
"ce that all 3 frequencies are set to 255.\r\n##The next line is a setting that you can change.\r\ntone 1"
"000 127 1200 127 2000 127\r\n##This file does not include all of the settings that can be used to modi"
"fy an E Speak voice.  It is intended to get you familiar with what the settings do.\r\n##However, you "
"can go to http://espeak.sourceforge.net/voices.html and read further information about other setting"
"s that can be added and changed.  I hope this helps, and Have fun.\r\n"
		;
		autoFileInMemory fim53 = FileInMemory_createWithData (3168, fim53_data, true, U"./data/voices/!v/ian");
		my addItem_move (fim53.move());

		static unsigned char fim54_data[262] = 
"language variant\nname Iven\npitch  74 118\nformant 0 52 133 88\nformant 1 87 82 76\nformant 2 94 56 42\nf"
"ormant 3 93 52 130\nformant 4 110 76 65\nformant 5  102 45 20\nformant 6  40 50 50\nformant 7 60 50 60\nf"
"ormant 8 100 50 40\nvoicing 530\ntone 600 255 1200 255 2000 160"
		;
		autoFileInMemory fim54 = FileInMemory_createWithData (261, fim54_data, true, U"./data/voices/!v/iven");
		my addItem_move (fim54.move());

		static unsigned char fim55_data[280] = 
"language variant\nname Iven2\npitch  74 118\nformant 0 52 133 88\nformant 1 87 82 76\nformant 2 94 56 42\n"
"formant 3 93 52 130\nformant 4 110 76 65\nformant 5  102 45 20\nformant 6  40 50 50\nformant 7 60 50 60\n"
"formant 8 100 50 40\nvoicing 220\nconsonants 28 42\ntone 600 255 1200 255 2000 150"
		;
		autoFileInMemory fim55 = FileInMemory_createWithData (279, fim55_data, true, U"./data/voices/!v/iven2");
		my addItem_move (fim55.move());

		static unsigned char fim56_data[263] = 
"language variant\nname Iven3\npitch  74 118\nformant 0 52 133 88\nformant 1 87 82 76\nformant 2 94 56 42\n"
"formant 3 93 52 130\nformant 4 110 76 65\nformant 5  102 45 20\nformant 6  40 50 50\nformant 7 60 50 60\n"
"formant 8 100 50 40\nvoicing 165\ntone 600 255 1200 255 2000 160"
		;
		autoFileInMemory fim56 = FileInMemory_createWithData (262, fim56_data, true, U"./data/voices/!v/iven3");
		my addItem_move (fim56.move());

		static unsigned char fim57_data[262] = 
"language variant\nname Iven4\npitch  74 118\nformant 0 52 133 88\nformant 1 87 82 76\nformant 2 94 56 42\n"
"formant 3 93 52 130\nformant 4 110 76 65\nformant 5  102 45 20\nformant 6  40 50 50\nformant 7 60 50 60\n"
"formant 8 100 50 40\nvoicing 165\ntone 600 170 1200 100 2000 40"
		;
		autoFileInMemory fim57 = FileInMemory_createWithData (261, fim57_data, true, U"./data/voices/!v/iven4");
		my addItem_move (fim57.move());

		static unsigned char fim58_data[3187] = 
"##Please note the 2 number signs, or pound signs in this file are for comments to help you to unders"
"tand what the settings are and how to set them.  \r\n## Language sets the language of your voice.  Thi"
"s setting is required for every voice that you make.\r\n##The next line is a setting you can change.  "
"However if you don't know the language codes it may be best to leave the setting as it is.\r\nlanguage"
" variant\r\n\r\n## The name setting is the name that will show up in the voice settings in the variant c"
"ombo box.\r\n##The next line is a setting you can change\r\nname John\r\n\r\n##The formant settings\r\n##  For"
"mant 0 is used to give a low frequency component to the sounds.\r\n## The three numbers are frequency,"
" strength, and Width, in that order.  Please note, the numbers are separated by a space.\r\n##The next"
" line is a setting you can change\r\nformant 0 100 100 100\r\n\r\n# Formants 1,2, and 3 are the standard t"
"hree formants which define vowels. \r\n##The next 3 lines are settings you can change\r\nformant 1 100 1"
"00 100\r\nformant 2 100 100 100\r\nformant 3 100 100 100\r\n\r\n# Formants 4,5 are higher than F3. They affe"
"ct the quality of the voice. \r\n##The next 2 lines are settings that you can change.\r\nformant 4 100 1"
"00 100\r\nformant 5 100 100 100\r\n\r\n## Formants 6, 7, and 8 are weak, high frequency, additions to vowe"
"ls to give a clearer sound. \r\n##The next 3 lines are settings that you can change.\r\nformant 6 100 10"
"0 100\r\nformant 7 100 100 100\r\nformant 8 100 100 100\r\n\r\n##Intonation affects the rise and fall of the"
" voice\t\r\n## The settings are 1 default, 2 less intonation, 3 less intonation and commas do not raise"
" the pitch, 4 the pitch rises at the end of a sentence rather than falling.\r\n##The next line is a se"
"tting you can change.\r\nintonation 1\r\n\r\n# Setting the pitch range.  The first number gives a base pit"
"ch to the voice (value in Hertz).  The second number controls the range of pitches used by the voice"
".\r\n# Setting it equal to the first number will give a monotone sounding voice.  The default values a"
"re 82 and 118. \r\npitch 82 118\r\n## The tone setting.  The first number on the setting line, 600, is t"
"he frequency setting for the amount of bass in the voice.\r\n##The second number on the tone line is t"
"he volume of the bass frequency.  You can set it from 0 to 255, 0 being the least amount, 255 being "
"the most.\r\n##The third number on the tone line, 1200, is the mid range frequency.  The fourth number"
" on the line is the setting to change the volume of the mid range frequency.\r\n##0 being the least am"
"ount and 255 being the maximum.\r\n## The fifth number on the tone line, 2000, is the treble frequency"
".  The sixth number is the volume of the treble frequency.  0 is the minimum and 255 is the maximum."
"\r\n##  You will notice that all 3 frequencies are set to 255.\r\n##The next line is a setting that you "
"can change.\r\ntone 600 255 1200 255 2000 255\r\n##This file does not include all of the settings that c"
"an be used to modify an E Speak voice.  It is intended to get you familiar with what the settings do"
".\r\n##However, you can go to http://espeak.sourceforge.net/voices.html and read further information a"
"bout other settings that can be added and changed.  I hope this helps, and Have fun.\r\n"
		;
		autoFileInMemory fim58 = FileInMemory_createWithData (3186, fim58_data, true, U"./data/voices/!v/john");
		my addItem_move (fim58.move());

		static unsigned char fim59_data[362] = 
"language variant \r\nname Kaukovalta\r\nformant 0 80    80 100   \r\nformant 1 40 80 100     \r\nformant 2 7"
"0 100  130      \r\nformant 3 80    100 60    \r\nformant 4 70 90 100 \r\nformant 5 70  90  100  \r\nformant"
" 6 70  100  90 \r\n   formant 7 100   90 110   \r\nformant 8 100   95 100 \r\npitch 70 120 \r\ntone 100 130 "
" 800  130 2000  130 \r\nconsonants 70 70      \r\nroughness 4\r\n  "
		;
		autoFileInMemory fim59 = FileInMemory_createWithData (361, fim59_data, true, U"./data/voices/!v/kaukovalta");
		my addItem_move (fim59.move());

		static unsigned char fim60_data[39] = 
"language variant\nname klatt\nklatt 1\n \n"
		;
		autoFileInMemory fim60 = FileInMemory_createWithData (38, fim60_data, true, U"./data/voices/!v/klatt");
		my addItem_move (fim60.move());

		static unsigned char fim61_data[39] = 
"language variant\nname klatt2\nklatt 2\n\n"
		;
		autoFileInMemory fim61 = FileInMemory_createWithData (38, fim61_data, true, U"./data/voices/!v/klatt2");
		my addItem_move (fim61.move());

		static unsigned char fim62_data[40] = 
"language variant\nname klatt3\nklatt 3\n \n"
		;
		autoFileInMemory fim62 = FileInMemory_createWithData (39, fim62_data, true, U"./data/voices/!v/klatt3");
		my addItem_move (fim62.move());

		static unsigned char fim63_data[40] = 
"language variant\nname klatt4\nklatt 4\n \n"
		;
		autoFileInMemory fim63 = FileInMemory_createWithData (39, fim63_data, true, U"./data/voices/!v/klatt4");
		my addItem_move (fim63.move());

		static unsigned char fim64_data[40] = 
"language variant\nname klatt5\nklatt 5\n \n"
		;
		autoFileInMemory fim64 = FileInMemory_createWithData (39, fim64_data, true, U"./data/voices/!v/klatt5");
		my addItem_move (fim64.move());

		static unsigned char fim65_data[40] = 
"language variant\nname klatt6\nklatt 6\n \n"
		;
		autoFileInMemory fim65 = FileInMemory_createWithData (39, fim65_data, true, U"./data/voices/!v/klatt6");
		my addItem_move (fim65.move());

		static unsigned char fim66_data[351] = 
"language variant\nname Linda\ngender female\n\n#echo 130 15\npitch 200 247\nflutter 3\nformant 0 88 85 154\n"
"formant 1 135 58 169 -30\nformant 2 131 75 152 -260\nformant 3 123 75 150\nformant 4 125 80 150\nformant"
" 5 125 80 150\nformant 6 110 80 150\nformant 7 110 75 150\nformant 8 110 75 150\n\nstressAdd -20 -20 -20 "
"-20 0 3 20 120\nstressAmp 18 16 20 20 20 20 27 20\n\n"
		;
		autoFileInMemory fim66 = FileInMemory_createWithData (350, fim66_data, true, U"./data/voices/!v/linda");
		my addItem_move (fim66.move());

		static unsigned char fim67_data[336] = 
"language variant\nname male1\ngender male 70\n\npitch 75 109\nflutter 5\nroughness 4\nconsonants 80 100\n\nfo"
"rmant 0  98 100 100\nformant 1  97 100 100\nformant 2  97  95 100\nformant 3  97  95 100\nformant 4  97 "
" 85 100\nformant 5 105  80 100\nformant 6  95  80 100\nformant 7 100 100 100\nformant 8 100 100 100\n\n//s"
"tressAdd -10 -10 -20 -20 0 0 40 70\n"
		;
		autoFileInMemory fim67 = FileInMemory_createWithData (335, fim67_data, true, U"./data/voices/!v/m1");
		my addItem_move (fim67.move());

		static unsigned char fim68_data[265] = 
"language variant\nname male2\ngender male\n\npitch 88 115\necho 130 15\nformant 0 100  80 120\nformant 1  9"
"0  85 120\nformant 2 110  85 120\nformant 3 105  90 120\nformant 4 100  90 120\nformant 5 100  90 120\nfo"
"rmant 6 100  90 120\nformant 7 100  90 120\nformant 8 100  90 120\n"
		;
		autoFileInMemory fim68 = FileInMemory_createWithData (264, fim68_data, true, U"./data/voices/!v/m2");
		my addItem_move (fim68.move());

		static unsigned char fim69_data[301] = 
"language variant\nname male3\ngender male\n\npitch 80 122\nformant 0 100 100 100\nformant 1  96  97 100\nfo"
"rmant 2  96  97 100\nformant 3  96 103 100\nformant 4  95 103 100\nformant 5  95 103 100\nformant 6 100 "
"100 100\nformant 7 100 100 100\nformant 8 100 100 100\n\nconsonants 100\nstressAdd 10 10 0 0 0 0 -30 -30\n"
		;
		autoFileInMemory fim69 = FileInMemory_createWithData (300, fim69_data, true, U"./data/voices/!v/m3");
		my addItem_move (fim69.move());

		static unsigned char fim70_data[291] = 
"language variant\nname male4\ngender male\n\npitch 70 110\n\nformant 0 103 100 100\nformant 1 103 100 100\nf"
"ormant 2 103 100 100\nformant 3 103 100 100\nformant 4 106 100 100\nformant 5 106 100 100\nformant 6 106"
" 100 100\nformant 7 103 100 100\nformant 8 103 100 100\n\nstressAdd -10 -10 -30 -30 0 0 60 90\n"
		;
		autoFileInMemory fim70 = FileInMemory_createWithData (290, fim70_data, true, U"./data/voices/!v/m4");
		my addItem_move (fim70.move());

		static unsigned char fim71_data[263] = 
"language variant\nname male5\ngender male\n\nformant 0 100  85 130\nformant 1  90  85 130  40\nformant 2  "
"80  85 130  310\nformant 3 105  85 130\nformant 4 105  85 130\nformant 5 105  85 130\nformant 6 105  85 "
"150\nformant 7 105  85 150\nformant 8 105  85 150\n\nintonation 2\n"
		;
		autoFileInMemory fim71 = FileInMemory_createWithData (262, fim71_data, true, U"./data/voices/!v/m5");
		my addItem_move (fim71.move());

		static unsigned char fim72_data[189] = 
"language variant\nname male6\ngender male\n\npitch 82 117\n\nformant 0 100  90 120\nformant 1 100  90 140\nf"
"ormant 2 100  70 140\nformant 3 100  75 140\nformant 4 100  80 140\nformant 5 100  80 140\n\n"
		;
		autoFileInMemory fim72 = FileInMemory_createWithData (188, fim72_data, true, U"./data/voices/!v/m6");
		my addItem_move (fim72.move());

		static unsigned char fim73_data[255] = 
"language variant\nname male7\ngender male\n\npitch  75 125\n\nformant 0 100 125 100\nformant 1 100 90 80\nfo"
"rmant 2 100 70 90\nformant 3 100 60 90\nformant 4 100 60 90\nformant 5  75 50 90\nformant 6  90 50 100\nf"
"ormant 7 100 50 100\nformant 8 100 50 100\nvoicing 155\n\n"
		;
		autoFileInMemory fim73 = FileInMemory_createWithData (254, fim73_data, true, U"./data/voices/!v/m7");
		my addItem_move (fim73.move());

		static unsigned char fim74_data[285] = 
"language variant\nname male8\ngender male 50\n\npitch 65 102\nformant 0 100 125 100\nformant 1 96 90 80\nfo"
"rmant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 50 100\nfor"
"mant 7 100 50 100\nformant 8 100 50 100\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim74 = FileInMemory_createWithData (284, fim74_data, true, U"./data/voices/!v/m8");
		my addItem_move (fim74.move());

		static unsigned char fim75_data[252] = 
"language variant\nname Marcelo\n\npitch  65 115\n\nformant 0 65 161 35\nformant 1 75 131 65\nformant 2 90 6"
"0 40\nformant 3 59 50 55\nformant 4 69 65 35\nformant 5 69 60 25\nformant 6 59 60 35\nformant 7 149 0 10\n"
"formant 8 199 0 90\nvoicing 135\nconsonants 115 120\n\n"
		;
		autoFileInMemory fim75 = FileInMemory_createWithData (251, fim75_data, true, U"./data/voices/!v/marcelo");
		my addItem_move (fim75.move());

		static unsigned char fim76_data[226] = 
"language variant\r\nname Max\r\n\r\nformant 0 90 120 100\r\nformant 1 100 100 75\r\nformant 2 100 100 75\r\nform"
"ant 3 100 80 75\r\nformant 4 100 80 75\r\nformant 5 100 80 75\r\nformant 6 100 0 75\r\nformant 7 100 0 75\r\nf"
"ormant 8 100 0 75\r\n\r\n\r\n\r\n"
		;
		autoFileInMemory fim76 = FileInMemory_createWithData (225, fim76_data, true, U"./data/voices/!v/max");
		my addItem_move (fim76.move());

		static unsigned char fim77_data[405] = 
"language variant\r\nname Michel\r\ngender male 25\r\npitch  82 122\r\necho 0 0\r\nflutter 0\r\nroughness 0\r\nstre"
"ssAmp 20 18  20 20  20 22  22 22\r\n\r\n\r\nformant 0 105 200 140\r\nformant 1 95 150 120\r\nformant 2 100 120"
" 140\r\nformant 3 95 95 140\r\nformant 4 30 30 30 -100\r\nformant 5 90 90 90\r\nformant 6 110 60 65\r\nformant"
" 7 100 0 100\r\nformant 8 100 0 100\r\nvoicing 35\r\nconsonants 60 40\r\ntone 400 160 1500 100 3000 70 4500 "
"40\r\n"
		;
		autoFileInMemory fim77 = FileInMemory_createWithData (404, fim77_data, true, U"./data/voices/!v/michel");
		my addItem_move (fim77.move());

		static unsigned char fim78_data[383] = 
"language variant\nname Miguel\ngender male 25\npitch  80 130\necho 0 0\nflutter 0\nroughness 0\nstressAmp 2"
"0 18  20 20  20 22  22 22\n\n\nformant 0 105 200 140\nformant 1 95 150 120\nformant 2 100 120 140\nformant"
" 3 95 95 140\nformant 4 30 30 30 -100\nformant 5 90 90 90\nformant 6 110 60 65\nformant 7 100 0 100\nform"
"ant 8 100 0 100\nvoicing 35\nconsonants 60 40\ntone 300 240 400 160 1500 100 3000 70\n"
		;
		autoFileInMemory fim78 = FileInMemory_createWithData (382, fim78_data, true, U"./data/voices/!v/miguel");
		my addItem_move (fim78.move());

		static unsigned char fim79_data[189] = 
"language variant\nname Mike2\nklatt 6\nvoicing 170\npitch 67 107\nformant 1 95 100 100\nformant 2 95 100 1"
"00\nformant 3 105 100 100\nformant 4 115 100 100\nformant 5 115 100 100\n\nconsonants 70 150\n"
		;
		autoFileInMemory fim79 = FileInMemory_createWithData (188, fim79_data, true, U"./data/voices/!v/mike2");
		my addItem_move (fim79.move());

		static unsigned char fim80_data[3190] = 
"##Please note the 2 number signs, or pound signs in this file are for comments to help you to unders"
"tand what the settings are and how to set them.  \r\n## Language sets the language of your voice.  Thi"
"s setting is required for every voice that you make.\r\n##The next line is a setting you can change.  "
"However if you don't know the language codes it may be best to leave the setting as it is.\r\nlanguage"
" variant\r\n\r\n## The name setting is the name that will show up in the voice settings in the variant c"
"ombo box.\r\n##The next line is a setting you can change\r\nname norbert\r\n\r\n##The formant settings\r\n##  "
"Formant 0 is used to give a low frequency component to the sounds.\r\n## The three numbers are frequen"
"cy, strength, and Width, in that order.  Please note, the numbers are separated by a space.\r\n##The n"
"ext line is a setting you can change\r\nformant 0 100 100 100\r\n\r\n# Formants 1,2, and 3 are the standar"
"d three formants which define vowels. \r\n##The next 3 lines are settings you can change\r\nformant 1 10"
"0 100 100\r\nformant 2 75  50 100\r\nformant 3 100 100 100\r\n\r\n# Formants 4,5 are higher than F3. They af"
"fect the quality of the voice. \r\n##The next 2 lines are settings that you can change.\r\nformant 4 100"
" 100 100\r\nformant 5 100 100 100\r\n\r\n## Formants 6, 7, and 8 are weak, high frequency, additions to vo"
"wels to give a clearer sound. \r\n##The next 3 lines are settings that you can change.\r\nformant 6 100 "
"100 100\r\nformant 7 100 100 100\r\nformant 8 100 100 100\r\n\r\n##Intonation affects the rise and fall of t"
"he voice\t\r\n## The settings are 1 default, 2 less intonation, 3 less intonation and commas do not rai"
"se the pitch, 4 the pitch rises at the end of a sentence rather than falling.\r\n##The next line is a "
"setting you can change.\r\nintonation 1\r\n\r\n# Setting the pitch range.  The first number gives a base p"
"itch to the voice (value in Hertz).  The second number controls the range of pitches used by the voi"
"ce. Setting it equal\r\n# to the first number will give a monotone sounding voice.  The default values"
" are 82 and 118. \r\npitch 82 118\r\n## The tone setting.  The first number on the setting line, 600, is"
" the frequency setting for the amount of bass in the voice.\r\n## The second number on the tone line i"
"s the volume of the bass frequency.  You can set it from 0 to 255, 0 being the least amount, 255 bei"
"ng the most.\r\n##The third number on the tone line, 1200, is the mid range frequency.  The fourth num"
"ber on the line is the setting to change the volume of the mid range frequency.\r\n##0 being the least"
" amount and 255 being the maximum.\r\n## The fifth number on the tone line, 2000, is the treble freque"
"ncy.  The sixth number is the volume of the treble frequency.  0 is the minimum and 255 is the maxim"
"um.\r\n##  You will notice that all 3 frequencies are set to 255.\r\n##The next line is a setting that y"
"ou can change.\r\ntone 600 255 1000 100 5000 255\r\n##This file does not include all of the settings tha"
"t can be used to modify an E Speak voice.  It is intended to get you familiar with what the settings"
" do.\r\n##However, you can go to http://espeak.sourceforge.net/voices.html and read further informatio"
"n about other settings that can be added and changed.  I hope this helps, and Have fun.\r\n"
		;
		autoFileInMemory fim80 = FileInMemory_createWithData (3189, fim80_data, true, U"./data/voices/!v/norbert");
		my addItem_move (fim80.move());

		static unsigned char fim81_data[3143] = 
"##Pleas note the 2 number signs, or pound signs in this file are for comments to help you to underst"
"and what the settings are and how to set them.  \n## Language sets the language of your voice.  This "
"setting is required for every voice that you make.\n##The next line is a setting you can change.  How"
"ever if you don't know the language codes it may be best to leave the setting as it is.\nlanguage var"
"iant\n\n## The name setting is the name that will show up in the voice settings in the variant combo b"
"ox.\n##The next line is a setting you can change\nname Pablo\n\n##The formant settings\n##  Formant 0 is "
"used to give a low frequency component to the sounds.\n## The three numbers are frequency, strength, "
"and Width, in that order.  Please note, the numbers are separated by a space.\n##The next line is a s"
"etting you can change\nformant 0 90 100 90\n\n# Formants 1,2, and 3 are the standard three formants whi"
"ch define vowels. \n##The next 3 lines are settings you can change\nformant 1 95 100 80\nformant 2 97 1"
"00 80\nformant 3 98 90 80\n\n# Formants 4,5 are higher than F3. They affect the quality of the voice. \n"
"##The next 2 lines are settings that you can change.\nformant 4 110 100 100\nformant 5 110 100 100\n\n##"
" Formants 6, 7, and 8 are weak, high frequency, additions to vowels to give a clearer sound. \n##The "
"next 3 lines are settings that you can change.\nformant 6 100 100 100\nformant 7 100 100 100\nformant 8"
" 100 100 100\n\n##Intonation affects the rise and fall of the voice\t\n## The settings are 1 default, 2 "
"less intonation, 3 less intonation and commas do not raise the pitch, 4 the pitch rises at the end o"
"f a sentence rather than falling.\n##The next line is a setting you can change.\nintonation 3\necho 30 "
"30\n# Setting the pitch range.  The first number gives a base pitch to the voice (value in Hertz).  T"
"he second number controls the range of pitches used by the voice. Setting it equal\n# to the first nu"
"mber will give a monotone sounding voice.  The default values are 82 and 118. \npitch 82 130\n## The t"
"one setting.  The first number on the setting line, 600, is the frequency setting for the amount of "
"bass in the voice.\n## The second number on the tone line is the volume of the bass frequency.  You c"
"an set it from 0 to 255, 0 being the least amount, 255 being the most.\n##The third number on the ton"
"e line, 1200, is the mid range frequency.  The fourth number on the line is the setting to change th"
"e volume of the mid range frequency.\n## 0 being the least amount and 255 being the maximum.\n## The f"
"ifth number on the tone line, 2000, is the treble frequency.  The sixth number is the volume of the "
"treble frequency.  0 is the minimum and 255 is the maximum.\n##  You will notice that all 3 frequenci"
"es are set to 255.\n##The next line is a setting that you can change.\ntone 600 255 1200 200 2000 255\n"
"\n##This file does not include all of the settings that can be used to modify an E Speak voice.  It i"
"s intended to get you familiar with what the settings do.  \n#However, you can go to http://espeak.so"
"urceforge.net/voices.html and read further information about other settings that can be added and ch"
"anged.  I hope this helps, and Have fun.\n\n"
		;
		autoFileInMemory fim81 = FileInMemory_createWithData (3142, fim81_data, true, U"./data/voices/!v/pablo");
		my addItem_move (fim81.move());

		static unsigned char fim82_data[285] = 
"language variant\nname Paul\n\npitch 70 100\n\nformant 0 90 120 100\nformant 1 103 100 75\nformant 2 98 100"
" 75\nformant 3 100 80 75\nformant 4 102 30 100\nformant 5 100 80 100\nformant 6 100 80 75\nformant 7 100 "
"0 75\nformant 8 100 60 75\nconsonants 90 60\nvoicing 230\ntone 420 255 1300 130 4000 100"
		;
		autoFileInMemory fim82 = FileInMemory_createWithData (284, fim82_data, true, U"./data/voices/!v/paul");
		my addItem_move (fim82.move());

		static unsigned char fim83_data[353] = 
"language variant\n\nname Pedro\n\nformant 0 100 150 100\n\nformant 1 95 100 80\nformant 2 95 100 80\nformant"
" 3 100 100 90\n\nformant 4 100 100 100\nformant 5 100 100 100\n\nformant 6 100 100 100\nformant 7 100 100 "
"100\nformant 8 100 100 100\n\nintonation 3\n\npitch 82 118\ntone 600 255 1200 255 2000 255\nstressLength 0 "
"1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\n"
		;
		autoFileInMemory fim83 = FileInMemory_createWithData (352, fim83_data, true, U"./data/voices/!v/pedro");
		my addItem_move (fim83.move());

		static unsigned char fim84_data[355] = 
"language variant\r\nname Quincy\r\n\r\npitch  67 100\r\n\r\nformant 0 85 108 106 3\r\nformant 1 97 110 56\r\nforma"
"nt 2 96 80 60\r\nformant 3 101 50 50\r\nformant 4 110 33 55\r\nformant 5 110 22 65\r\nformant 6 77 60 60 65\r"
"\nformant 7 66 0 100\r\nformant 8 100 0 100\r\nvoicing 99\r\nconsonants 66 90\r\n\r\nroughness 0\r\ntone 600 170 "
"1200 100 2000 70\r\nstressAmp 16 16 24 20 20 16 28 24 \r\n"
		;
		autoFileInMemory fim84 = FileInMemory_createWithData (354, fim84_data, true, U"./data/voices/!v/quincy");
		my addItem_move (fim84.move());

		static unsigned char fim85_data[266] = 
"language variant\r\nname Rob\r\n\r\npitch  50 130\r\nformant 0 100 100 100\r\nformant 1 95 100 60\r\nformant 2 9"
"7 90 50\r\nformant 3 101 70 50\r\nformant 4 110 65 55 \r\nformant 5 110 70 65\r\nformant 6 110 70 65\r\nforman"
"t 7 0 0 0\r\nformant 8 0 0 0\r\n\r\nvoicing 115\r\nconsonants 110 120\r\n\r\n"
		;
		autoFileInMemory fim85 = FileInMemory_createWithData (265, fim85_data, true, U"./data/voices/!v/rob");
		my addItem_move (fim85.move());

		static unsigned char fim86_data[275] = 
"language variant\r\nname Robert\r\n\r\npitch  65 115\r\n\r\nformant 0 85 108 100\r\nformant 1 95 110 60\r\nformant"
" 2 97 90 50\r\nformant 3 101 50 50\r\nformant 4 110 65 55\r\nformant 5 110 60 65\r\nformant 6 110 60 65\r\nfor"
"mant 7 100 0 100\r\nformant 8 100 0 100\r\nvoicing 115\r\nconsonants 110 120\r\n\r\n"
		;
		autoFileInMemory fim86 = FileInMemory_createWithData (274, fim86_data, true, U"./data/voices/!v/robert");
		my addItem_move (fim86.move());

		static unsigned char fim87_data[452] = 
"language variant\nname Robosoft\necho 30 1000\nklatt 5\n\npitch 60 90\nformant 0 100 125 100\nformant 1 96 "
"90 80\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 50"
" 100\nformant 7 100 50 100\nformant 8 100 50 100\n\nroughness 50\n\nintonation 0\nvoicing 80\nconsonants 110"
" 120\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 "
"20 24\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim87 = FileInMemory_createWithData (451, fim87_data, true, U"./data/voices/!v/robosoft");
		my addItem_move (fim87.move());

		static unsigned char fim88_data[455] = 
"language variant\nname Robosoft2\necho 10 600\nklatt 4\n\npitch 70 100\nformant 0 100 125 100\nformant 1 96"
" 90 80\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 5"
"0 100\nformant 7 100 50 100\nformant 8 100 50 100\n\nroughness 75\n\nintonation -25\nvoicing 80\nconsonants "
"110 120\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 "
"16 20 24\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim88 = FileInMemory_createWithData (454, fim88_data, true, U"./data/voices/!v/robosoft2");
		my addItem_move (fim88.move());

		static unsigned char fim89_data[456] = 
"language variant\nname Robosoft3\necho 10 10000\nklatt 4\n\npitch 75 115\nformant 0 100 125 100\nformant 1 "
"96 90 80\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90"
" 50 100\nformant 7 100 50 100\nformant 8 100 50 100\n\nroughness 5\n\nintonation 10\nvoicing 150\nconsonants"
" 110 120\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16"
" 16 20 24\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim89 = FileInMemory_createWithData (455, fim89_data, true, U"./data/voices/!v/robosoft3");
		my addItem_move (fim89.move());

		static unsigned char fim90_data[448] = 
"language variant\nname Robosoft4\necho 10 10000\n\npitch 75 115\nformant 0 100 125 100\nformant 1 96 90 80"
"\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 50 100\n"
"formant 7 100 50 100\nformant 8 100 50 100\n\nroughness 5\n\nintonation 10\nvoicing 150\nconsonants 110 120"
"\nstressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 2"
"4\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim90 = FileInMemory_createWithData (447, fim90_data, true, U"./data/voices/!v/robosoft4");
		my addItem_move (fim90.move());

		static unsigned char fim91_data[446] = 
"language variant\nname Robosoft5\necho 10 10000\n\npitch 75 115\nformant 0 100 125 100\nformant 1 96 90 80"
"\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 50 100\n"
"formant 7 100 50 100\nformant 8 100 50 100\n\nroughness 0\n\nintonation 10\nvoicing 150\nconsonants 60 40\ns"
"tressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\nstressAmp 16 16 24 24 16 16 20 24\n"
"\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim91 = FileInMemory_createWithData (445, fim91_data, true, U"./data/voices/!v/robosoft5");
		my addItem_move (fim91.move());

		static unsigned char fim92_data[288] = 
"language variant\nname Robosoft6\necho 40 10000\npitch 150 150\nformant 0 100 125 100\nformant 1 96 90 80"
"\nformant 2 97 70 90\nformant 3 97 60 90\nformant 4 97 60 90\nformant 5  100 50 90\nformant 6  90 50 100\n"
"formant 7 100 50 100\nformant 8 100 50 100\n\ntone 100 255 600 70 1200 22 2000 66 3000 12\n"
		;
		autoFileInMemory fim92 = FileInMemory_createWithData (287, fim92_data, true, U"./data/voices/!v/robosoft6");
		my addItem_move (fim92.move());

		static unsigned char fim93_data[411] = 
"language variant\nname Robosoft7\necho 10 10000\n\npitch 75 115\n\nformant 0 90 120 100\nformant 1 100 100 "
"75\nformant 2 100 100 75\nformant 3 100 80 75\nformant 4 100 80 75\nformant 5 100 80 75\nformant 6 100 0 "
"75\nformant 7 100 0 75\nformant 8 100 0 75\n\nroughness 0\n\nintonation 10\nvoicing 150\nconsonants 60 40\nst"
"ressLength 0 1 2 3 4 5 6 7\nstressAdd 130 140 140 100 0 0 130 160\n\ntone 100 255 600 70 1200 22 2000 6"
"6 3000 12\n"
		;
		autoFileInMemory fim93 = FileInMemory_createWithData (410, fim93_data, true, U"./data/voices/!v/robosoft7");
		my addItem_move (fim93.move());

		static unsigned char fim94_data[244] = 
"language variant\nname Robosoft8\necho 40 10000\npitch 150 150\nformant 0 90 120 100\nformant 1 100 100 7"
"5\nformant 2 100 100 75\nformant 3 100 80 75\nformant 4 100 80 75\nformant 5 100 80 75\nformant 6 100 0 7"
"5\nformant 7 100 0 75\nformant 8 100 0 75\n\n\n\n"
		;
		autoFileInMemory fim94 = FileInMemory_createWithData (243, fim94_data, true, U"./data/voices/!v/robosoft8");
		my addItem_move (fim94.move());

		static unsigned char fim95_data[531] = 
"// This file is UTF-8 encoded\n// Variant sandro (ver.25-3) for eSpeak-ng Copyright (C)2019 by Lolo v"
"manolo301@gmail.com\n\nlanguage variant\nname sandro\ngender male\n\nformant 0 95 146 100\nformant 1 98 90 "
"100 \nformant 2 103 98 100\nformant 3 100 90 100\nformant 4 100 101 100\nformant 5 110 120 100 2123\nform"
"ant 6 100 100 100 1200\nformant 7 32 125 80 600\nformant 8 34 95 30 49\n\nvoicing 165\nconsonants 194 255"
"\npitch 78 115\nroughness 3\nbreath 20 5 2 10 5 0 27 100\nbreathw 255 255 60 180 160 255 255 255\n\ntone 5"
"00 210 470 70 160 155 2985 32\n"
		;
		autoFileInMemory fim95 = FileInMemory_createWithData (530, fim95_data, true, U"./data/voices/!v/sandro");
		my addItem_move (fim95.move());

		static unsigned char fim96_data[281] = 
"language variant\nname shelby\nflutter 0\nroughness 0\n\nformant 0 100 160 190\nformant 1 90 90 90\nformant"
" 2 140 140 140\nformant 3 130 150 130\nformant 4 110 110 110\nformant 5 120 120 110\nformant 6 10 10 10\n"
"formant 7 10 10 10\nformant 8 10 10 10\n\npitch 100 210\nvoicing 40\nconsonants 90 70"
		;
		autoFileInMemory fim96 = FileInMemory_createWithData (280, fim96_data, true, U"./data/voices/!v/shelby");
		my addItem_move (fim96.move());

		static unsigned char fim97_data[365] = 
"language variant\nname Steph\ngender female\n\npitch 166 200\nflutter 1\nroughness 0\ntone 100 255 600 70 1"
"200 22 2000 66 3000 12\n\nformant 0 99  80 150\nformant 1 120 60 160\nformant 2 99 70 110 150\nformant 3 "
"116 77 150\nformant 4 9 59 110\nformant 5 100 50 2\nformant 6 104 80 150\nformant 7 110 70 150\nformant 8"
" 110 70 150\n\nstressAmp 16 16 24 24 16 16 20 24\nconsonants 55 90\n"
		;
		autoFileInMemory fim97 = FileInMemory_createWithData (364, fim97_data, true, U"./data/voices/!v/steph");
		my addItem_move (fim97.move());

		static unsigned char fim98_data[368] = 
"language variant\nname Steph2\ngender female\n\npitch 166 200\nflutter 1\nroughness 0\ntone 100 255 600 70 "
"1200 22 2000 66 3000 12\n\nformant 0 99  100 150\nformant 1 120 80 160\nformant 2 99 90 110 150\nformant "
"3 116 97 150\nformant 4 9 73 116\nformant 5 100 70 2\nformant 6 104 100 150\nformant 7 110 90 150\nforman"
"t 8 110 90 150\n\nstressAmp 16 16 24 24 16 16 20 24\nconsonants 55 90\n"
		;
		autoFileInMemory fim98 = FileInMemory_createWithData (367, fim98_data, true, U"./data/voices/!v/steph2");
		my addItem_move (fim98.move());

		static unsigned char fim99_data[378] = 
"language variant\nname Steph3\ngender female\n\npitch 166 200\nflutter 1\nroughness 0\nvoicing 200\ntone 100"
" 255 600 70 1200 22 2000 66 3000 12\n\nformant 0 99  80 150\nformant 1 120 60 160\nformant 2 99 70 110 1"
"50\nformant 3 116 77 150\nformant 4 9 59 110\nformant 5 100 50 2\nformant 6 104 80 150\nformant 7 110 70 "
"150\nformant 8 110 70 150\n\nstressAmp 16 16 24 24 16 16 20 24\nconsonants 70 90\n"
		;
		autoFileInMemory fim99 = FileInMemory_createWithData (377, fim99_data, true, U"./data/voices/!v/steph3");
		my addItem_move (fim99.move());

		static unsigned char fim100_data[384] = 
"language variant\r\nname travis\r\ngender male 30\r\n\r\npitch 75 120\r\n\r\nformant 0 90 90 90 90 \r\nformant 1 5"
"0 100 80 95\r\nformant 2 90 60 90 100\r\nformant 3 80 80 90 100\r\nformant 4 50 90 100 100\r\nformant 5 100 "
"95 100 55\r\nformant 6 80 50 100 85\r\nformant 7 60 60 60 120\r\nformant 8 80 80 140 100\r\n\r\ntone 600 100 1"
"000 200 1500 50 \r\nflutter 1 \r\n\r\nroughness 3 \r\n\r\nvoicing 200 \r\nconsonants 120 190 \r\n"
		;
		autoFileInMemory fim100 = FileInMemory_createWithData (383, fim100_data, true, U"./data/voices/!v/travis");
		my addItem_move (fim100.move());

		static unsigned char fim101_data[254] = 
"language variant\nname victor\ngender male 25\n\nformant 0 100 100 100\nformant 1 95 95 95\nformant 2 90 9"
"0 90\nformant 3 90 90 90\nformant 4 40 40 40\nformant 5 80 80 80\nformant 6 20 20 20\nformant 7 20 20 20\n"
"formant 8 20 20 20\npitch 80 110\nvoicing 60\nbreath 2 4"
		;
		autoFileInMemory fim101 = FileInMemory_createWithData (253, fim101_data, true, U"./data/voices/!v/victor");
		my addItem_move (fim101.move());

		static unsigned char fim102_data[187] = 
"language variant\nname whisper\ngender male\n\npitch 82 117\nflutter 20\n\nformant 0 100  0 100\nformant 1 1"
"00 80 100\n\nvoicing 17\nbreath   75  75  50  40  15  10\nbreathw 150 150 200 200 400 400\n"
		;
		autoFileInMemory fim102 = FileInMemory_createWithData (186, fim102_data, true, U"./data/voices/!v/whisper");
		my addItem_move (fim102.move());

		static unsigned char fim103_data[393] = 
"language variant\nname female_whisper\ngender female\n\npitch 160 220\nroughness 3\n\nformant 0 105   0 150"
"\nformant 1 110  40 160\nformant 2 110  70 150\nformant 3 110  70 150\nformant 4 115  80 150\nformant 5 1"
"15  80 150\nformant 6 110  70 150\nformant 7 110  70 150\nformant 8 110  70 150\n\nstressAdd 0 0 -10 -10 "
"0 0 10 40\n\n// whisper\nvoicing 20\nbreath 75 75 50 40 15 10\nbreathw 150 150 200 200 400 400\n \n"
		;
		autoFileInMemory fim103 = FileInMemory_createWithData (392, fim103_data, true, U"./data/voices/!v/whisperf");
		my addItem_move (fim103.move());

		static unsigned char fim104_data[276] = 
"language variant\r\nname Zac\r\nflutter 5\r\n\r\npitch 240 390\r\nformant 0 145 100 145\r\nformant 1 145 100 145"
"\r\nformant 2 145 100 145\r\nformant 3 145 100 145\r\nformant 4 145 100 145\r\nformant 5 145 120 145\r\nforman"
"t 6 145 120 145\r\nformant 7 145 120 145\r\nformant 8 145 120 145\r\nvoicing 80\r\n"
		;
		autoFileInMemory fim104 = FileInMemory_createWithData (275, fim104_data, true, U"./data/voices/!v/zac");
		my addItem_move (fim104.move());

	} catch (MelderError) {
		Melder_throw (U"Not everything was added to the FileInMemorySet.");
	}
}

/* End of file espeak_praat_FileInMemorySet_addVoices.cpp */
