/* EEG.cpp
 *
 * Copyright (C) 2011-2026 Paul Boersma
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

#include "EEG.h"
#include "Sound_and_Spectrum.h"

#include "oo_DESTROY.h"
#include "EEG_def.h"
#include "oo_COPY.h"
#include "EEG_def.h"
#include "oo_EQUAL.h"
#include "EEG_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "EEG_def.h"
#include "oo_WRITE_TEXT.h"
#include "EEG_def.h"
#include "oo_READ_TEXT.h"
#include "EEG_def.h"
#include "oo_WRITE_BINARY.h"
#include "EEG_def.h"
#include "oo_READ_BINARY.h"
#include "EEG_def.h"
#include "oo_DESCRIPTION.h"
#include "EEG_def.h"

Thing_implement (EEG, Function, 0);

void structEEG :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	if (our sound) {
		MelderInfo_writeLine (U"Time sampling of the signal:");
		MelderInfo_writeLine (U"   Number of samples: ", our sound -> nx);
		MelderInfo_writeLine (U"   Sampling period: ", our sound -> dx, U" seconds");
		MelderInfo_writeLine (U"   Sampling frequency: ", Melder_single (1.0 / our sound -> dx), U" Hz");
		MelderInfo_writeLine (U"   First sample centred at: ", our sound -> x1, U" seconds");
	}
	MelderInfo_writeLine (U"Number of cap electrodes: ", EEG_getNumberOfCapElectrodes (this));
	MelderInfo_writeLine (U"Number of external electrodes: ", EEG_getNumberOfExternalElectrodes (this));
	MelderInfo_writeLine (U"Number of extra sensors: ", EEG_getNumberOfExtraSensors (this));
}

void structEEG :: v_shiftX (double xfrom, double xto) {
	EEG_Parent :: v_shiftX (xfrom, xto);
	if (our sound   )  Function_shiftXTo (our sound.get(),    xfrom, xto);
	if (our textgrid)  Function_shiftXTo (our textgrid.get(), xfrom, xto);
}

void structEEG :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	EEG_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (our sound   )  our sound    -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	if (our textgrid)  our textgrid -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
}

void EEG_init (EEG me, double tmin, double tmax) {
	my xmin = tmin;
	my xmax = tmax;
}

autoEEG EEG_create (double tmin, double tmax) {
	try {
		autoEEG me = Thing_new (EEG);
		EEG_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"EEG object not created.");
	}
}

integer EEG_getChannelNumber (EEG me, conststring32 channelName) {
	for (integer ichan = 1; ichan <= my numberOfChannels; ichan ++)
		if (Melder_equ (my channelNames [ichan].get(), channelName))
			return ichan;
	return 0;
}

autoEEG EEG_readFromBdfFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		char buffer [81];
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		const bool is24bit = ( buffer [0] == (char) 255 );
		(void) fread (buffer, 1, 80, f);
		buffer [80] = '\0';
		trace (U"Local subject identification: \"", Melder_peek8to32_u (buffer), U"\"");
		(void) fread (buffer, 1, 80, f);
		buffer [80] = '\0';
		trace (U"Local recording identification: \"", Melder_peek8to32_u (buffer), U"\"");
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		trace (U"Start date of recording: \"", Melder_peek8to32_u (buffer), U"\"");
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		trace (U"Start time of recording: \"", Melder_peek8to32_u (buffer), U"\"");
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		const integer numberOfBytesInHeaderRecord = atol (buffer);
		trace (U"Number of bytes in header record: ", numberOfBytesInHeaderRecord);
		(void) fread (buffer, 1, 44, f);
		buffer [44] = '\0';
		trace (U"Version of data format: \"", Melder_peek8to32_u (buffer), U"\"");
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		const integer numberOfDataRecords = strtol (buffer, nullptr, 10);
		trace (U"Number of data records: ", numberOfDataRecords);
		(void) fread (buffer, 1, 8, f);
		buffer [8] = '\0';
		const double durationOfDataRecord = atof (buffer);
		trace (U"Duration of a data record: ", durationOfDataRecord);
		(void) fread (buffer, 1, 4, f);
		buffer [4] = '\0';
		const integer numberOfChannels = atol (buffer);
		trace (U"Number of channels in data record: ", numberOfChannels);
		if (numberOfBytesInHeaderRecord != (numberOfChannels + 1) * 256)
			Melder_throw (U"Number of bytes in header record (", numberOfBytesInHeaderRecord,
					U") doesn't match number of channels (", numberOfChannels, U").");
		autoSTRVEC channelNames (numberOfChannels);
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			(void) fread (buffer, 1, 16, f);
			buffer [16] = '\0';   // labels of the channels
			/*
				Strip all final spaces.
			*/
			for (int i = 15; i >= 0; i --) {
				if (buffer [i] == ' ')
					buffer [i] = '\0';
				else
					break;
			}
			channelNames [ichannel] = Melder_8to32_e (buffer);
			trace (U"Channel <<", channelNames [ichannel].get(), U">>");
		}
		const bool hasLetters = str32equ (channelNames [numberOfChannels].get(), U"EDF Annotations");
		double samplingFrequency = undefined;
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			(void) fread (buffer, 1, 80, f);
			buffer [80] = '\0';   // transducer type
		}
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';   // physical dimension of channels
		}
		autoVEC physicalMinimum = raw_VEC (numberOfChannels);
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';
			physicalMinimum [ichannel] = atof (buffer);
		}
		autoVEC physicalMaximum = raw_VEC (numberOfChannels);
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';
			physicalMaximum [ichannel] = atof (buffer);
		}
		autoVEC digitalMinimum = raw_VEC (numberOfChannels);
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';
			digitalMinimum [ichannel] = atof (buffer);
		}
		autoVEC digitalMaximum = raw_VEC (numberOfChannels);
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';
			digitalMaximum [ichannel] = atof (buffer);
		}
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			(void) fread (buffer, 1, 80, f);
			buffer [80] = '\0';   // prefiltering
		}
		integer numberOfSamplesPerDataRecord = 0;
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			(void) fread (buffer, 1, 8, f);
			buffer [8] = '\0';   // number of samples in each data record
			const integer numberOfSamplesInThisDataRecord = atol (buffer);
			if (isundef (samplingFrequency)) {
				numberOfSamplesPerDataRecord = numberOfSamplesInThisDataRecord;
				samplingFrequency = numberOfSamplesInThisDataRecord / durationOfDataRecord;
			}
			if (numberOfSamplesInThisDataRecord / durationOfDataRecord != samplingFrequency)
				Melder_throw (U"Number of samples per data record in channel ", channel,
					U" (", numberOfSamplesInThisDataRecord,
					U") doesn't match sampling frequency of channel 1 (", samplingFrequency, U").");
		}
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			(void) fread (buffer, 1, 32, f);
			buffer [32] = '\0';   // reserved
		}
		const double duration = numberOfDataRecords * durationOfDataRecord;
		autoEEG him = EEG_create (0, duration);
		his numberOfChannels = numberOfChannels;
		autoSound me = Sound_createSimple (numberOfChannels, duration, samplingFrequency);
		Melder_assert (my nx == numberOfSamplesPerDataRecord * numberOfDataRecords);
		autoBYTEVEC dataBuffer = zero_BYTEVEC (3 * numberOfSamplesPerDataRecord);
		for (integer record = 1; record <= numberOfDataRecords; record ++) {
			for (integer channel = 1; channel <= numberOfChannels; channel ++) {
				double factor = ( channel == numberOfChannels ? 1.0 : physicalMinimum [channel] / digitalMinimum [channel] );
				if (channel < numberOfChannels - EEG_getNumberOfExtraSensors (him.get()))
					factor /= 1000000.0;
				if (is24bit) {
					(void) fread (dataBuffer.asArgumentToFunctionThatExpectsZeroBasedArray(), 3, (size_t) numberOfSamplesPerDataRecord, f);
					byte *p = & dataBuffer [1];
					for (integer i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
						const integer sample = i + (record - 1) * numberOfSamplesPerDataRecord;
						Melder_assert (sample <= my nx);
						const uint8 lowByte = *p ++, midByte = *p ++, highByte = *p ++;
						uint32 externalValue = ((uint32) highByte << 16) | ((uint32) midByte << 8) | (uint32) lowByte;
						if ((highByte & 128) != 0)   // is the 24-bit sign bit on?
							externalValue |= 0xFF00'0000;   // extend negative sign to 32 bits
						my z [channel] [sample] = (int32) externalValue * factor;
					}
				} else {
					(void) fread (dataBuffer.asArgumentToFunctionThatExpectsZeroBasedArray(), 2, (size_t) numberOfSamplesPerDataRecord, f);
					byte *p = & dataBuffer [1];
					for (integer i = 1; i <= numberOfSamplesPerDataRecord; i ++) {
						const integer sample = i + (record - 1) * numberOfSamplesPerDataRecord;
						Melder_assert (sample <= my nx);
						const uint8 lowByte = *p ++, highByte = *p ++;
						const uint16 externalValue = (uint16) ((uint16) highByte << 8) | (uint16) lowByte;
						my z [channel] [sample] = (int16) externalValue * factor;
					}
				}
			}
		}
		int numberOfStatusBits = 8;
		for (integer i = 1; i <= my nx; i ++) {
			const uint32 value = (uint32) (int32) my z [numberOfChannels] [i];
			if (value & 0x0000'FF00)
				numberOfStatusBits = 16;
		}
		autoTextGrid thee;
		if (hasLetters) {
			thee = TextGrid_create (0, duration, U"Mark Trigger", U"Mark Trigger");
			autoMelderString letters;
			double time = undefined;
			for (integer i = 1; i <= my nx; i ++) {
				const uint32 value = (uint32) (int32) my z [numberOfChannels] [i];
				for (int ibyte = 1; ibyte <= numberOfStatusBits / 8; ibyte ++) {
					const uint32 mask = ( ibyte == 1 ? 0x0000'00ff : 0x0000'ff00 );
					const char32 kar = ( ibyte == 1 ? (value & mask) : (value & mask) >> 8 );
					if (kar != U'\0' && kar != 20) {
						MelderString_appendCharacter (& letters, kar);
					} else if (letters.string [0] != U'\0') {
						if (letters.string [0] == U'+') {
							if (isdefined (time)) {
								try {
									TextGrid_insertPoint (thee.get(), 1, time, U"");
								} catch (MelderError) {
									Melder_throw (U"Did not insert empty mark (", letters.string, U") on Mark tier.");
								}
								time = undefined;   // defensive
							}
							time = Melder_atof (& letters.string [1]);
							MelderString_empty (& letters);
						} else {
							if (isundef (time)) {
								Melder_throw (U"Undefined time for label at sample ", i, U".");
							}
							try {
								if (Melder_nequ (letters.string, U"Trigger-", 8)) {
									try {
										TextGrid_insertPoint (thee.get(), 2, time, & letters.string [8]);
									} catch (MelderError) {
										Melder_clearError ();
										trace (U"Duplicate trigger at ", time, U" seconds: ", & letters.string [8]);
									}
								} else {
									TextGrid_insertPoint (thee.get(), 1, time, & letters.string [0]);
								}
							} catch (MelderError) {
								Melder_throw (U"Did not insert mark (", letters.string, U") on Trigger tier.");
							}
							time = undefined;   // crucial
							MelderString_empty (& letters);
						}
					}
				}
			}
			if (isdefined (time)) {
				TextGrid_insertPoint (thee.get(), 1, time, U"");
				time = undefined;   // defensive
			}
		} else {
			thee = TextGrid_create (0.0, duration,
				numberOfStatusBits == 8 ? U"S1 S2 S3 S4 S5 S6 S7 S8" : U"S1 S2 S3 S4 S5 S6 S7 S8 S9 S10 S11 S12 S13 S14 S15 S16",
				U""
			);
			for (int bit = 1; bit <= numberOfStatusBits; bit ++) {
				const uint32 bitValue = 1 << (bit - 1);
				IntervalTier tier = (IntervalTier) thy tiers->at [bit];
				for (integer i = 1; i <= my nx; i ++) {
					const uint32 previousValue = ( i == 1 ? 0 : (uint32) (int32) my z [numberOfChannels] [i - 1] );
					const uint32 thisValue = (uint32) (int32) my z [numberOfChannels] [i];
					if ((thisValue & bitValue) != (previousValue & bitValue)) {
						const double time = ( i == 1 ? 0.0 : my x1 + (i - 1.5) * my dx );
						if (time != 0.0)
							TextGrid_insertBoundary (thee.get(), bit, time);
						if ((thisValue & bitValue) != 0)
							TextGrid_setIntervalText (thee.get(), bit, tier -> intervals.size, U"1");
					}
				}
			}
		}
		f.close (file);
		his channelNames = std::move (channelNames);
		his sound = me.move();
		his textgrid = thee.move();
		if (EEG_getNumberOfCapElectrodes (him.get()) == 32) {
			EEG_setChannelName (him.get(), 1, U"Fp1");
			EEG_setChannelName (him.get(), 2, U"AF3");
			EEG_setChannelName (him.get(), 3, U"F7");
			EEG_setChannelName (him.get(), 4, U"F3");
			EEG_setChannelName (him.get(), 5, U"FC1");
			EEG_setChannelName (him.get(), 6, U"FC5");
			EEG_setChannelName (him.get(), 7, U"T7");
			EEG_setChannelName (him.get(), 8, U"C3");
			EEG_setChannelName (him.get(), 9, U"CP1");
			EEG_setChannelName (him.get(), 10, U"CP5");
			EEG_setChannelName (him.get(), 11, U"P7");
			EEG_setChannelName (him.get(), 12, U"P3");
			EEG_setChannelName (him.get(), 13, U"Pz");
			EEG_setChannelName (him.get(), 14, U"PO3");
			EEG_setChannelName (him.get(), 15, U"O1");
			EEG_setChannelName (him.get(), 16, U"Oz");
			EEG_setChannelName (him.get(), 17, U"O2");
			EEG_setChannelName (him.get(), 18, U"PO4");
			EEG_setChannelName (him.get(), 19, U"P4");
			EEG_setChannelName (him.get(), 20, U"P8");
			EEG_setChannelName (him.get(), 21, U"CP6");
			EEG_setChannelName (him.get(), 22, U"CP2");
			EEG_setChannelName (him.get(), 23, U"C4");
			EEG_setChannelName (him.get(), 24, U"T8");
			EEG_setChannelName (him.get(), 25, U"FC6");
			EEG_setChannelName (him.get(), 26, U"FC2");
			EEG_setChannelName (him.get(), 27, U"F4");
			EEG_setChannelName (him.get(), 28, U"F8");
			EEG_setChannelName (him.get(), 29, U"AF4");
			EEG_setChannelName (him.get(), 30, U"Fp2");
			EEG_setChannelName (him.get(), 31, U"Fz");
			EEG_setChannelName (him.get(), 32, U"Cz");
		} else if (EEG_getNumberOfCapElectrodes (him.get()) == 64) {
			EEG_setChannelName (him.get(), 1, U"Fp1");
			EEG_setChannelName (him.get(), 2, U"AF7");
			EEG_setChannelName (him.get(), 3, U"AF3");
			EEG_setChannelName (him.get(), 4, U"F1");
			EEG_setChannelName (him.get(), 5, U"F3");
			EEG_setChannelName (him.get(), 6, U"F5");
			EEG_setChannelName (him.get(), 7, U"F7");
			EEG_setChannelName (him.get(), 8, U"FT7");
			EEG_setChannelName (him.get(), 9, U"FC5");
			EEG_setChannelName (him.get(), 10, U"FC3");
			EEG_setChannelName (him.get(), 11, U"FC1");
			EEG_setChannelName (him.get(), 12, U"C1");
			EEG_setChannelName (him.get(), 13, U"C3");
			EEG_setChannelName (him.get(), 14, U"C5");
			EEG_setChannelName (him.get(), 15, U"T7");
			EEG_setChannelName (him.get(), 16, U"TP7");
			EEG_setChannelName (him.get(), 17, U"CP5");
			EEG_setChannelName (him.get(), 18, U"CP3");
			EEG_setChannelName (him.get(), 19, U"CP1");
			EEG_setChannelName (him.get(), 20, U"P1");
			EEG_setChannelName (him.get(), 21, U"P3");
			EEG_setChannelName (him.get(), 22, U"P5");
			EEG_setChannelName (him.get(), 23, U"P7");
			EEG_setChannelName (him.get(), 24, U"P9");
			EEG_setChannelName (him.get(), 25, U"PO7");
			EEG_setChannelName (him.get(), 26, U"PO3");
			EEG_setChannelName (him.get(), 27, U"O1");
			EEG_setChannelName (him.get(), 28, U"Iz");
			EEG_setChannelName (him.get(), 29, U"Oz");
			EEG_setChannelName (him.get(), 30, U"POz");
			EEG_setChannelName (him.get(), 31, U"Pz");
			EEG_setChannelName (him.get(), 32, U"CPz");
			EEG_setChannelName (him.get(), 33, U"Fpz");
			EEG_setChannelName (him.get(), 34, U"Fp2");
			EEG_setChannelName (him.get(), 35, U"AF8");
			EEG_setChannelName (him.get(), 36, U"AF4");
			EEG_setChannelName (him.get(), 37, U"AFz");
			EEG_setChannelName (him.get(), 38, U"Fz");
			EEG_setChannelName (him.get(), 39, U"F2");
			EEG_setChannelName (him.get(), 40, U"F4");
			EEG_setChannelName (him.get(), 41, U"F6");
			EEG_setChannelName (him.get(), 42, U"F8");
			EEG_setChannelName (him.get(), 43, U"FT8");
			EEG_setChannelName (him.get(), 44, U"FC6");
			EEG_setChannelName (him.get(), 45, U"FC4");
			EEG_setChannelName (him.get(), 46, U"FC2");
			EEG_setChannelName (him.get(), 47, U"FCz");
			EEG_setChannelName (him.get(), 48, U"Cz");
			EEG_setChannelName (him.get(), 49, U"C2");
			EEG_setChannelName (him.get(), 50, U"C4");
			EEG_setChannelName (him.get(), 51, U"C6");
			EEG_setChannelName (him.get(), 52, U"T8");
			EEG_setChannelName (him.get(), 53, U"TP8");
			EEG_setChannelName (him.get(), 54, U"CP6");
			EEG_setChannelName (him.get(), 55, U"CP4");
			EEG_setChannelName (him.get(), 56, U"CP2");
			EEG_setChannelName (him.get(), 57, U"P2");
			EEG_setChannelName (him.get(), 58, U"P4");
			EEG_setChannelName (him.get(), 59, U"P6");
			EEG_setChannelName (him.get(), 60, U"P8");
			EEG_setChannelName (him.get(), 61, U"P10");
			EEG_setChannelName (him.get(), 62, U"PO8");
			EEG_setChannelName (him.get(), 63, U"PO4");
			EEG_setChannelName (him.get(), 64, U"O2");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"BDF file not read.");
	}
}

static void detrend (VEC const& channel) {
	const double firstValue = channel [1], lastValue = channel [channel.size];
	channel [1] = channel [channel.size] = 0.0;
	for (integer isamp = 2; isamp < channel.size; isamp ++)
		channel [isamp] -= ((isamp - 1.0) * lastValue + (channel.size - isamp) * firstValue) / (channel.size - 1);
}

void EEG_detrend (EEG me) {
	for (integer ichan = 1; ichan <= my numberOfChannels - EEG_getNumberOfExtraSensors (me); ichan ++)
		detrend (my sound -> z.row (ichan));
}

void EEG_filter (EEG me, double lowFrequency, double lowWidth, double highFrequency, double highWidth, bool doNotch50Hz) {
	try {
/*
	integer nsampFFT = 1;
	while (nsampFFT < my sound -> nx)
		nsampFFT *= 2;
	autoNUMFourierTable fftTable = NUMFourierTable_create (nsampFFT);
*/
		for (integer ichan = 1; ichan <= my numberOfChannels - EEG_getNumberOfExtraSensors (me); ichan ++) {
			autoSound channel = Sound_extractChannel (my sound.get(), ichan);
			autoSpectrum spec = Sound_to_Spectrum (channel.get(), true);
			Spectrum_passHannBand (spec.get(), lowFrequency, 0.0, lowWidth);
			Spectrum_passHannBand (spec.get(), 0.0, highFrequency, highWidth);
			if (doNotch50Hz)
				Spectrum_stopHannBand (spec.get(), 48.0, 52.0, 1.0);
			autoSound him = Spectrum_to_Sound (spec.get());
			my sound -> z.row (ichan)  <<=  his z.row (1).part (1, my sound -> nx);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not filtered.");
	}
}

void EEG_setChannelName (EEG me, integer channelNumber, conststring32 newName) {
	my channelNames [channelNumber] = Melder_dup (newName);
}

void EEG_setExternalElectrodeNames (EEG me,
	conststring32 nameExg1, conststring32 nameExg2, conststring32 nameExg3, conststring32 nameExg4,
	conststring32 nameExg5, conststring32 nameExg6, conststring32 nameExg7, conststring32 nameExg8)
{
	if (EEG_getNumberOfExternalElectrodes (me) != 8)
		Melder_throw (U"There aren't 8 external electrodes.");
	const integer firstExternalElectrode = EEG_getNumberOfCapElectrodes (me) + 1;
	EEG_setChannelName (me, firstExternalElectrode, nameExg1);
	EEG_setChannelName (me, firstExternalElectrode + 1, nameExg2);
	EEG_setChannelName (me, firstExternalElectrode + 2, nameExg3);
	EEG_setChannelName (me, firstExternalElectrode + 3, nameExg4);
	EEG_setChannelName (me, firstExternalElectrode + 4, nameExg5);
	EEG_setChannelName (me, firstExternalElectrode + 5, nameExg6);
	EEG_setChannelName (me, firstExternalElectrode + 6, nameExg7);
	EEG_setChannelName (me, firstExternalElectrode + 7, nameExg8);
}

void EEG_subtractReference (EEG me, conststring32 channelName1, conststring32 channelName2) {
	integer channelNumber1 = EEG_getChannelNumber (me, channelName1);
	if (channelNumber1 == 0)
		Melder_throw (me, U": no channel named “", channelName1, U"”.");
	integer channelNumber2 = EEG_getChannelNumber (me, channelName2);
	if (channelNumber2 == 0 && channelName2 [0] != U'\0')
		Melder_throw (me, U": no channel named “", channelName2, U"”.");
	const integer numberOfElectrodeChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
	for (integer isamp = 1; isamp <= my sound -> nx; isamp ++) {
		const double referenceValue = ( channelNumber2 == 0 ? my sound -> z [channelNumber1] [isamp] :
				0.5 * (my sound -> z [channelNumber1] [isamp] + my sound -> z [channelNumber2] [isamp]) );
		my sound -> z.column (isamp).part (1, numberOfElectrodeChannels)  -=  referenceValue;
	}
}

void EEG_subtractMeanChannel (EEG me, integer fromChannel, integer toChannel) {
	if (fromChannel < 1 || fromChannel > my numberOfChannels)
		Melder_throw (U"No channel ", fromChannel, U".");
	if (toChannel < 1 || toChannel > my numberOfChannels)
		Melder_throw (U"No channel ", toChannel, U".");
	if (fromChannel > toChannel)
		Melder_throw (U"Channel range cannot run from ", fromChannel, U" to ", toChannel, U". Please reverse.");
	const integer numberOfElectrodeChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
	for (integer isamp = 1; isamp <= my sound -> nx; isamp ++) {
		const double referenceValue = NUMmean (my sound -> z.column (isamp).part (fromChannel, toChannel));
		my sound -> z.column (isamp).part (1, numberOfElectrodeChannels)  -=  referenceValue;
	}
}

void EEG_setChannelToZero (EEG me, integer channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > my numberOfChannels)
			Melder_throw (U"No channel ", channelNumber, U".");
		my sound -> z.row (channelNumber)  <<=  0.0;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not set to zero.");
	}
}

void EEG_setChannelToZero (EEG me, conststring32 channelName) {
	try {
		const integer channelNumber = EEG_getChannelNumber (me, channelName);
		if (channelNumber == 0)
			Melder_throw (U"No channel named “", channelName, U"”.");
		EEG_setChannelToZero (me, channelNumber);
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelName, U" not set to zero.");
	}
}

void EEG_removeTriggers (EEG me, kMelder_string which, conststring32 criterion) {
	try {
		if (my textgrid -> tiers->size < 2 || ! Melder_equ (my textgrid -> tiers->at [2] -> name.get(), U"Trigger"))
			Melder_throw (me, U" does not have a Trigger channel.");
		TextGrid_removePoints (my textgrid.get(), 2, which, criterion);
	} catch (MelderError) {
		Melder_throw (me, U": triggers not removed.");
	}
}

autoEEG EEG_extractChannel (EEG me, integer channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > my numberOfChannels)
			Melder_throw (U"No channel ", channelNumber, U".");
		autoEEG thee = EEG_create (my xmin, my xmax);
		thy numberOfChannels = 1;
		thy channelNames = autoSTRVEC (1);
		thy channelNames [1] = Melder_dup (my channelNames [1].get());
		thy sound = Sound_extractChannel (my sound.get(), channelNumber);
		thy textgrid = Data_copy (my textgrid.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not extracted.");
	}
}

autoEEG EEG_extractChannel (EEG me, conststring32 channelName) {
	try {
		const integer channelNumber = EEG_getChannelNumber (me, channelName);
		if (channelNumber == 0)
			Melder_throw (U"No channel named “", channelName, U"”.");
		return EEG_extractChannel (me, channelNumber);
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelName, U" not extracted.");
	}
}

autoEEG EEG_extractChannels (EEG me, constINTVECVU const& channelNumbers) {
	try {
		const integer numberOfChannels = channelNumbers.size;
		Melder_require (numberOfChannels > 0,
			U"The number of channels should be greater than 0.");
		autoEEG you = EEG_create (my xmin, my xmax);
		your sound = Sound_extractChannels (my sound.get(), channelNumbers);
		your numberOfChannels = numberOfChannels;
		your channelNames = autoSTRVEC (numberOfChannels);
		for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
			const integer originalChannelNumber = channelNumbers [ichan];
			your channelNames [ichan] = Melder_dup (my channelNames [originalChannelNumber].get());
		}
		your textgrid = Data_copy (my textgrid.get());
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": channels not extracted.");
	}
}

static void Sound_removeChannel (Sound me, integer channelNumber) {
	try {
		Melder_require (channelNumber >= 1 && channelNumber <= my ny,
			U"No channel ", channelNumber, U".");
		Melder_require (my ny > 1,
			U"Cannot remove last remaining channel.");
		for (integer ichan = channelNumber; ichan < my ny; ichan ++)
			my z.row (ichan)  <<=  my z.row (ichan + 1);
		my ymax -= 1.0;
		my ny -= 1;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not removed.");
	}
}

void EEG_removeChannel (EEG me, integer channelNumber) {
	try {
		if (channelNumber < 1 || channelNumber > my numberOfChannels)
			Melder_throw (U"No channel ", channelNumber, U".");
		for (integer ichan = channelNumber; ichan < my numberOfChannels; ichan ++)
			my channelNames [ichan] = my channelNames [ichan + 1].move();
		my channelNames [my numberOfChannels]. reset();
		my numberOfChannels -= 1;
		Sound_removeChannel (my sound.get(), channelNumber);
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not removed.");
	}
}

void EEG_removeChannel (EEG me, conststring32 channelName) {
	try {
		const integer channelNumber = EEG_getChannelNumber (me, channelName);
		if (channelNumber == 0)
			Melder_throw (U"No channel named \"", channelName, U"\".");
		EEG_removeChannel (me, channelNumber);
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelName, U" not removed.");
	}
}

autoEEG EEGs_concatenate (OrderedOf<structEEG>* me) {
	try {
		if (my size < 1)
			Melder_throw (U"Cannot concatenate zero EEG objects.");
		EEG first = my at [1];
		const integer numberOfChannels = first -> numberOfChannels;
		autoSTRVEC channelNames = copy_STRVEC (first -> channelNames.get());
		for (integer ieeg = 2; ieeg <= my size; ieeg ++) {
			EEG other = my at [ieeg];
			if (other -> numberOfChannels != numberOfChannels)
				Melder_throw (U"The number of channels of ", other, U" does not match the number of channels of ", first, U".");
			for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
				if (! Melder_equ (other -> channelNames [ichan].get(), channelNames [ichan].get()))
					Melder_throw (U"Channel ", ichan, U" has a different name in ",
						other, U" (", other -> channelNames [ichan].get(), U") than in ",
						first, U" (", channelNames [ichan].get(), U")."
					);
			}
		}
		autoSoundList soundList = SoundList_create ();
		OrderedOf<structTextGrid> textgridList;
		for (integer ieeg = 1; ieeg <= my size; ieeg ++) {
			EEG eeg = my at [ieeg];
			soundList -> addItem_ref (eeg -> sound.get());
			textgridList. addItem_ref (eeg -> textgrid.get());
		}
		autoEEG thee = Thing_new (EEG);
		thy numberOfChannels = numberOfChannels;
		thy channelNames = std::move (channelNames);
		thy sound = Sounds_concatenate (soundList.get(), 0.0);
		thy textgrid = TextGrids_concatenate (& textgridList);
		thy xmin = thy textgrid -> xmin;
		thy xmax = thy textgrid -> xmax;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGrids not concatenated.");
	}
}

autoEEG EEG_extractPart (EEG me, double tmin, double tmax, bool preserveTimes) {
	try {
		autoEEG thee = Thing_new (EEG);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = copy_STRVEC (my channelNames.get());
		thy sound = Sound_extractPart (my sound.get(), tmin, tmax, kSound_windowShape::RECTANGULAR, 1.0, preserveTimes);
		thy textgrid = TextGrid_extractPart (my textgrid.get(), tmin, tmax, preserveTimes);
		thy xmin = thy textgrid -> xmin;
		thy xmax = thy textgrid -> xmax;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

void EEG_replaceTextGrid (EEG me, TextGrid textgrid) {
	try {
		my textgrid = Data_copy (textgrid);
	} catch (MelderError) {
		Melder_throw (me, U": TextGrid not replaced with ", textgrid, U".");
	}
}

autoMixingMatrix EEG_to_MixingMatrix (EEG me,
	double startTime, double endTime, integer numberOfCrossCorrelations, double lagStep,
	integer maxNumberOfIterations, double tol, int method)
{
	try {
		autoCrossCorrelationTableList tables = Sound_to_CrossCorrelationTableList (my sound.get(), startTime, endTime, numberOfCrossCorrelations, lagStep);
		autoMixingMatrix thee = MixingMatrix_create (my sound -> ny, my sound -> ny);
		MixingMatrix_setRandomGauss (thee.get(), 0.0, 1.0);
		for (integer ichan = 1; ichan <= my numberOfChannels; ichan ++) {
			TableOfReal_setRowLabel (thee.get(), ichan, my channelNames [ichan].get());
			TableOfReal_setColumnLabel (thee.get(), ichan, Melder_cat (U"ic", ichan));
		}
		MixingMatrix_CrossCorrelationTableList_improveUnmixing (thee.get(), tables.get(), maxNumberOfIterations, tol, method);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MixingMatrix created.");
	}
}

autoEEG EEG_MixingMatrix_to_EEG_unmix (EEG me, MixingMatrix you) {
	Melder_require (my numberOfChannels == your numberOfRows,
		U"To be able to unmix, the number of channels in ", me, U" (", my numberOfChannels, U")",
		U" should be equal to the number of rows in ", you, U" (", your numberOfRows, U")."
	);
	for (integer ichan = 1; ichan <= your numberOfRows; ichan ++) {
		Melder_require (Melder_equ (my channelNames [ichan].get(), your rowLabels [ichan].get()),
			U"To be able to unmix, the name of channel ", ichan,
			U" should be the same in ", me, U" (where it is ", my channelNames [ichan].get(), U")",
			U" as in ", you, U" (where it is ", your rowLabels [ichan].get(), U")."
		);
	}
	autoEEG him = EEG_create (my xmin, my xmax);
	his sound = Sound_MixingMatrix_unmix (my sound.get(), you);
	his textgrid = Data_copy (my textgrid.get());
	his numberOfChannels = your numberOfColumns;
	his channelNames = copy_STRVEC (your columnLabels.get());
	return him;
}

autoEEG EEG_MixingMatrix_to_EEG_mix (EEG me, MixingMatrix you) {
	Melder_require (my numberOfChannels == your numberOfColumns,
		U"To be able to mix, the number of channels in ", me, U" (", my numberOfChannels, U")",
		U" should be equal to the number of columns in ", you, U" (", your numberOfColumns, U")."
	);
	for (integer ichan = 1; ichan <= your numberOfColumns; ichan ++) {
		Melder_require (Melder_equ (my channelNames [ichan].get(), your columnLabels [ichan].get()),
			U"To be able to mix, the name of channel ", ichan,
			U" should be the same in ", me, U" (where it is ", my channelNames [ichan].get(), U")",
			U" as in ", you, U" (where it is ", your columnLabels [ichan].get(), U")."
		);
	}
	autoEEG him = EEG_create (my xmin, my xmax);
	his sound = Sound_MixingMatrix_mix (my sound.get(), you);
	his textgrid = Data_copy (my textgrid.get());
	his numberOfChannels = your numberOfRows;
	his channelNames = copy_STRVEC (your rowLabels.get());
	return him;
}

/* End of file EEG.cpp */
