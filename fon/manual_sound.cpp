/* manual_sound.cpp
 *
 * Copyright (C) 1992-2008,2010-2012,2014-2025 Paul Boersma
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

#include "ManPagesM.h"

#include "Sound.h"
#include "../kar/UnicodeData.h"

void manual_sound_init (ManPages me);
void manual_sound_init (ManPages me) {

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Sounds: Combine to stereo"
© Paul Boersma 2007,2008,2012,2023

To combine two (or more) mono @Sound objects into one single stereo (i.e. multi-channel) Sound,
select those Sounds in the list and choose ##Combine to stereo# from the #Combine menu.
A new stereo Sound will appear in the list.

Of the two original mono Sounds, the one that is higher in the list will become the left channel
of the new stereo Sound, and the one that is lower in the list will become the right channel of the new Sound.

You can also create Sounds with e.g. 3 or 5 or 100 channels in this way. The topmost selected mono Sound
will become channel 1, and the bottommost selected mono Sound will become the last channel.

################################################################################
"Create Sound as pure tone..."
© Paul Boersma 2023

A command in the #Sound submenu of the @@New menu@, to create a sine wave.

A new Sound object will appear in the list,
with the specified sine frequency and the specified amplitude.
You can also specify the number of channels, the start and end times, and the sampling frequency.
In order to prevent hearing clicks at the beginning and end,
you can specify a fade-in duration and a fade-out duration.

################################################################################
"Create Sound from formula..."
© Paul Boersma 2007-02-25

A command in the @@New menu@ to create a @Sound with a specified duration and sampling frequency,
filled with values from a formula.

See the @Formulas tutorial for explanations and examples.
{-;
	Create Sound from formula... blok Mono 0 1 44100 if round(x*377*2) mod 2 then 0.9 else -0.9 fi

	Je kunt dus heel veel maken met een scriptje:

	form Blokgolf
	   word Naam blok
	   real Begintijd_(s) 0.0
	   real Eindtijd_(s) 1.0
	   positive Samplefrequentie_(Hz) 22050
	   positive Frequentie_(Hz) 377
	   real Amplitude 0.9
	endform
	Create Sound from formula... 'Naam' Mono begintijd eindtijd samplefrequentie
	   ... if round(x*frequentie*2) mod 2 then amplitude else -amplitude fi
}

################################################################################
"Create Sound as tone complex..."
© Paul Boersma 2022-01-22

A command in the @@New menu@ to create a @Sound as the sum of a number of sine waves
with equidistant frequencies.

Settings
========

##Name
:	the name of the resulting Sound object.

##Start time (s)

##End time (s)
:	the time domain of the resulting Sound.

##Sampling frequency (Hz)
:	the sampling frequency of the resulting Sound.

##Phase
:	determines whether the result is a sum of %sines or a sum of %cosines,
i.e., whether the zero crossings or the maxima of the components are synchronized.
This choice has little perceptual consequences.

##Frequency step (Hz)
:	the distance between the components. In first approximation,
this is the perceived fundamental frequency.

##First frequency (Hz)
:	the lowest frequency component. If you supply a value of 0,
##First frequency# is taken equal to ##Frequency step#.

##Ceiling (Hz)
:	the frequency above which no components are used. If you supply a value of 0
or a value above the Sound's @@Nyquist frequency@, %ceiling is taken equal to
the Nyquist frequency.

##Number of components
:	determines how many sinusoids are used. If you supply a value of 0
or a very high value, the maximum number of components is used,
limited by #Ceiling.

Example 1: a pulse train
========================

A series of pulses at regular intervals,
sampled after low-pass filtering at the Nyquist frequency,
can be regarded as a sum of cosine waves. For instance, a 100-Hz pulse train,
sampled at 44100 Hz, can be created with:
{;
	Create Sound as tone complex: "train", 0, 1, 44100, "cosine", 100, 0, 0, 0
}
Supplying the value 0 for %firstFrequency yields an unshifted harmonic complex.

Example 2: a shifted harmonic complex
=====================================

Some experiments on human pitch perception (%%residue pitch%) use
a number of sinusoidal components with harmonically
related frequencies that are all shifted by a constant amount.

For instance, to get a sum of sine waves with frequencies 105 Hz, 205 Hz, and 305 Hz,
you would use:
{;
	Create Sound as tone complex: "train", 0.3, 1, 44100, "sine", 100, 105, 0, 3
}
or
{;
	Create Sound as tone complex: "train", 0.3, 1, 44100, "sine", 100, 105, 350, 0
}
whichever you prefer.

Some of these experiments are described in @@Plomp (1967)@ and @@Patterson & Wightman (1976)@.

Algorithm
=========

For the “sine” phase, the resulting Sound is given by the following formula:

~ %x(%t) = \su__%i=1..%numberOfComponents_ sin (2%\pi·(%firstFrequency + (%i−1)·%frequencyStep)·%t)

More flexibility?
=================

Suppose you wanted to vary the relative strengths of the frequency components.
You could achieve this by creating a Sound with the command discussed here,
take its Fourier transform, run a formula on the resulting Spectrum,
and take the inverse Fourier transform.

A more general approach is described shortly.

Suppose you need a sum of sine waves with frequencies 105, 205, 305, ..., 1905 Hz,
and with relative amplitudes 1, 1/2, 1/3, ..., 1/19. You could build a script that computes
the various components, and add them to each other as you go along. Instead of calling 19
scripts, however, you can achieve this with the following more general script:
{;
	form: "Add waves with decreasing amplitudes"
		natural: "Number of components", "19"
	endform
	# Create a Matrix with frequency and amplitude information in each row:
	Create simple Matrix: "freqAndGain", number_of_components, 2, ~ 0
	Formula: ~ if col = 1 then row * 100 + 5 else 1 / row fi
	# Create a large Matrix with all the component sine waves:
	Create Matrix: "components", 0, 1, 10000, 1e-4, 0.5e-4, 1, number_of_components, number_of_components, 1, 1, ~ 0
	Formula: ~ object ["Matrix freqAndGain", 2] * sin (2 * pi * object ["Matrix freqAndGain", 1] * x)
	# Integrate:
	Formula: ~ self + self [row - 1, col]
	# Publish last row:
	To Sound (slice): number_of_components
	Scale amplitudes: 0.99
}

################################################################################
"Extract one channel..."
© Paul Boersma 2011-01-29

To extract the left channel of an existing stereo @Sound as a new mono Sound,
select that stereo Sound in the list and choose ##Extract one channel...# from the #Convert menu,
Then set the #Channel to 1 and click OK.
A new mono Sound will appear in the list. For the right channel, set #Channel to 2.

Details
=======

The name of the new mono Sound will be based on the name of the original stereo Sound.
For instance, if the original Sound is called `hello` and you extract channel 2, the new Sound will be called `hello_ch2`.

This command also works if you select more than one stereo Sound.
For each of them, Praat creates a new mono Sound.

################################################################################
"Read separate channels from sound file..."
© Paul Boersma 2011-10-10

A command in the @@Open menu@ of the #Objects window.
You use this if you want to get the channels of a @stereo (or multi-channel) sound file
as two (or more) separate @Sound objects in the list.
If the file name is `hello.wav`, Praat will name the channels `hello_ch1`, `hello_ch2`, and so on.

################################################################################
"Record mono Sound..."
© Paul Boersma 2020-11-20

A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window.

################################################################################
"Record stereo Sound..."
© Paul Boersma 2002-12-12

A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window.

################################################################################
"Record Sound (fixed time)..."
© Paul Boersma 2002-12-12

A usually hidden command in the @@New menu@ to record a @Sound.

################################################################################
"Sound"
© Paul Boersma 2016-10-13

One of the @@types of objects@ in Praat. For tutorial information, see all of the @Intro.

Commands
========

Creation:
- @@Record mono Sound...@ (from microphone or line input, with the @SoundRecorder)
- @@Record stereo Sound...@
- @@Create Sound from formula...@
- @@Create Sound as tone complex...
- @@Create Sound as gammatone...
- @@Create Sound as Shepard tone...

Opening and saving:
- @@Sound files

You can also use the text and binary (real-valued) formats for Sounds, like for any other class:
- @@Save as text file...
- @@Save as binary file...

Viewing and editing:
- @SoundEditor, @ManipulationEditor, @TextGridEditor, @PointEditor, @PitchTierEditor, @SpectrumEditor

Playing:
- @@Sound: Play
- @@PointProcess: Hum
- @@PointProcess: Play

Drawing:
- @@Sound: Draw...
- @@Sound: Draw where...
- @@Sound: Paint where...
- @@Sounds: Paint enclosed...

Queries:
, structure:
	- @@time domain
	- @@Get number of samples
	- @@Get sampling period
	- @@Get sampling frequency
	- @@Get time from sample number...
	- @@Get sample number from time...
, content:
	- @@Sound: Get value at time...
	- @@Sound: Get value at sample number...
, shape:
	- @@Sound: Get minimum...
	- @@Sound: Get time of minimum...
	- @@Sound: Get maximum...
	- @@Sound: Get time of maximum...
	- @@Sound: Get absolute extremum...
	- @@Sound: Get nearest zero crossing...
, statistics:
	- @@Sound: Get mean...
	- @@Sound: Get root-mean-square...
	- @@Sound: Get standard deviation...
, energy:
	- @@Sound: Get energy...
	- @@Sound: Get power...
, in air:
	- @@Sound: Get energy in air
	- @@Sound: Get power in air
	- @@Sound: Get intensity (dB)

Modification:
- @@Matrix: Formula...
- @@Sound: Set value at sample number...
- @@Sound: Filter with one formant (in-place)...
- @@Sound: Pre-emphasize (in-place)...
- @@Sound: De-emphasize (in-place)...

Annotation (see @@Intro 7. Annotation@):
- @@Sound: To TextGrid...

Periodicity analysis:
- @@Sound: To Pitch (filtered autocorrelation)...
- @@Sound: To Pitch (raw cross-correlation)...
- @@Sound: To Pitch (raw autocorrelation)...
- @@Sound: To Pitch (filtered cross-correlation)...
- @@Sound: To Pitch (shs)...
- @@Sound: To Harmonicity (ac)...
- @@Sound: To Harmonicity (cc)...
- @@Sound: To PointProcess (periodic, cc)...
- @@Sound: To PointProcess (periodic, peaks)...
- @@Sound & Pitch: To PointProcess (cc)
- @@Sound & Pitch: To PointProcess (peaks)...
- @@Sound: To Intensity...

Spectral analysis:
- @@Sound: To Spectrum...
- @@Sound: To Spectrogram...
- @@Sound: To Formant (burg)...
- @@Sound: To Formant (sl)...
- @@Sound: LPC analysis
- @@Sound: To LPC (autocorrelation)...
- @@Sound: To LPC (covariance)...
- @@Sound: To LPC (burg)...
- @@Sound: To LPC (marple)...

Filtering (see @Filtering tutorial):
- @@Sound: Filter (pass Hann band)...
- @@Sound: Filter (stop Hann band)...
- @@Sound: Filter (formula)...
- @@Sound: Filter (one formant)...
- @@Sound: Filter (pre-emphasis)...
- @@Sound: Filter (de-emphasis)...
- @@LPC & Sound: Filter...
- @@LPC & Sound: Filter (inverse)
- @@Sound & Formant: Filter
- @@Sound & FormantGrid: Filter

Conversion:
- @@Sound: Resample...

Enhancement:
- @@Sound: Lengthen (overlap-add)...@: lengthen by a constant factor
- @@Sound: Deepen band modulation...@: strenghten intensity modulations in each critical band

Combination:
- @@Sounds: Convolve...
- @@Sounds: Cross-correlate...
- @@Sound: Autocorrelate...
- @@Sounds: Concatenate

Synthesis
- @@Source-filter synthesis@ tutorial
- @Manipulation (@@overlap-add@ etc.)
- @@Spectrum: To Sound
- ##Pitch: To Sound...
- @@PointProcess: To Sound (pulse train)...
- @@PointProcess: To Sound (hum)...
- ##Pitch & PointProcess: To Sound...
- @@Articulatory synthesis@ tutorial
- @@Artword & Speaker: To Sound...

Inside a Sound
==============

With @Inspect, you will see that a Sound contains the following data:

%x__%min_
:	start time, in seconds.

%x__%max_ > %x__%min_
:	end time, in seconds.

%n_%x
:	the number of samples (\\>_ 1).

%dx
:	sample period, in seconds. The inverse of the sampling frequency (in Hz).

%x_1
:	the time associated with the first sample (in seconds).
This will normally be in the range [%xmin, %xmax].
The time associated with the last sample (i.e., %x_1 + (%nx \-m 1) %dx))
will also normally be in that range. Mostly, the sound starts at %t = 0 seconds
and %x_1 = %dx / 2. Also, usually, %x__%max_ = %n_%x %dx.

%z [1] [1..%n_%x]
:	the amplitude of the sound (stored as single-precision floating-point numbers).
For the most common applications (playing and file I-O), Praat assumes that
the amplitude is greater than -1 and less than +1.
For some applications (modelling of the inner ear;
articulatory synthesis), Praat assumes that the amplitude is expressed in Pascal units.
If these interpretations are combined, we see that the maximum peak amplitude of
a calibrated sound is 1 Pascal; for a sine wave, this means 91 dB SPL.

Limitations
===========

Since the Sound object completely resides in memory, its size is limited
to the amount of RAM in your computer. For sounds longer than a few minutes,
you could use the @LongSound object instead, which you can view in the @LongSoundEditor.

################################################################################
"Sound: De-emphasize (in-place)..."
© Paul Boersma 2017-11-14

A command to change the spectral slope of every selected @Sound object.

The reverse of @@Sound: Pre-emphasize (in-place)...@. For an example, see @@Source-filter synthesis@.

This is the in-place version of @@Sound: Filter (de-emphasis)...@,
i.e., it does not create a new Sound object but modifies an existing object.

Setting
=======

##From frequency (Hz)
:	the frequency %F above which the spectral slope will decrease by 6 dB/octave.

Algorithm
=========

The de-emphasis factor %\\al is computed as
~	%\al = exp (-2 %\pi %F %\Det)

where %\Det is the sampling period of the sound. Every sample %x_%i of the sound,
except %x_1, is then changed, going up from the second sample:
~	%x_%i = %x_%i + %\al %x__%i−1_

################################################################################
"Sound: Deepen band modulation..."
© Paul Boersma 2010-10-26

A command to enhance the fast spectral changes, like %F_2 movements, in each selected @Sound object.

Settings
========

##Enhancement (dB)
:	the maximum increase in the level within each critical band. The standard value is 20 dB.

##From frequency (Hz)
:	the lowest frequency that shall be manipulated. The bottom frequency of the first critical band that is to be enhanced.
The standard value is 300 Hertz.

##To frequency (Hz)
:	the highest frequency that shall be manipulated (the last critical band may be narrower than the others). The standard value is 8000 Hz.

##Slow modulation (Hz)
:	the frequency %f__%slow_ below which the intensity modulations in the bands should not be expanded. The standard value is 3 Hz.

##Fast modulation (Hz)
:	the frequency %f__%fast_ above which the intensity modulations in the bands should not be expanded. The standard value is 30 Hz.

##Band smoothing (Hz)
:	the degree of overlap of each band into its adjacent bands. Prevents %ringing. The standard value is 100 Hz.

Algorithm
=========

This algorithm was inspired by @@Nagarajan, Wang, Merzenich, Schreiner, Johnston, Jenkins, Miller & Tallal (1998)@,
but not identical to it. Now follows the description.

Suppose the settings have their standard values. The resulting sound will composed of the unfiltered part of the original sound,
plus all manipulated bands.

First, the resulting sound becomes the original sound, stop-band filtered between 300 and 8000 Hz:
after a forward Fourier transform, all values in the @Spectrum at frequencies between 0 and 200 Hz and
between 8100 Hz and the Nyquist frequency of the sound are retained unchanged.
The spectral values at frequencies between 400 and 7900 Hz are set to zero.
Between 200 and 400 Hz and between 7900 and 8100 Hz, the values are multiplied by a raised sine,
so as to give a smooth transition without ringing in the time domain (the raised sine also allows us to view
the spectrum as a sum of spectral bands). Finally, a backward Fourier transform gives us the filtered sound.

The remaining part of the spectrum is divided into %%critical bands%, i.e. frequency bands one Bark wide.
For instance, the first critical band run from 300 to 406 Hz, the second from 406 to 520 Hz, and so on.
Each critical band is converted to a pass-band filtered sound by means of the backward Fourier transform.

Each filtered sound will be manipulated, and the resulting manipulated sounds are added to the stop-band filtered sound
we created earlier. If the manipulation is the identity transformation, the resulting sound will be equal to the original sound.
But, of course, the manipulation does something different. Here are the steps.

First, we compute the local intensity of the filtered sound %x (%t):
~	%intensity (%t) = 10 log__10_ (%x^2 (%t) + 10^^-6^)

This intensity is subjected to a forward Fourier transform. In the frequency domain, we administer a band filter.
We want to enhance the intensity modulation in the range between 3 and 30 Hz.
We can achieve this by comparing the very smooth intensity contour, low-pass filtered at %f__%slow_ = 3 Hz,
with the intensity contour that has enough temporal resolution to see the place-discriminating %F_2 movements,
which is low-pass filtered at %f__%fast_ = 30 Hz. In the frequency domain, the filter is
~	%H (%f) = exp (- (%\al%f / %f__%fast_)^2) - exp (- (%\al%f / %f__%slow_)^2)

where %\al equals \Vrln 2 \~~ 1 / 1.2011224, so that %H (%f) has its -6 dB points at %f__%slow_ and %f__%fast_:
{ 5x3
	alpha = sqrt (ln (2))
	filter = Create Sound from formula: "filter", 1, 0.0, 100.0, 10.0,
	... ~ exp (- (alpha * x / 30.0) ^ 2) - exp (- (alpha * x / 3.0) ^ 2)
	Red
	Draw: 0, 0, 0.0, 1.0, "no", "curve"
	Black
	Draw inner box
	Text bottom: "yes", "Frequency %f (Hz)"
	Text left: "yes", "Amplitude filter %H (%f)"
	One mark left: 0.0, "yes", "yes", "no", ""
	One mark left: 0.5, "yes", "yes", "yes", ""
	One mark left: 1.0, "yes", "yes", "no", ""
	One mark right: 1.0, "no", "yes", "no", "0 dB"
	One mark right: 0.5, "no", "yes", "no", "\-m6 dB"
	One mark bottom: 3.0, "yes", "yes", "yes", ""
	One mark bottom: 30.0, "yes", "yes", "yes", ""
}

Now, why do we use such a flat filter? Because a steep filter would show ringing effects in the time domain,
dividing the sound into 30-ms chunks. If our filter is a sum of exponentials in the frequency domain, it will also
be a sum of exponentials in the time domain. The backward Fourier transform of the frequency response %H (%f)
is the impulse response %h (%t). It is given by
~	%h (%t) = 2%\pi\Vr%\pi %f__%fast_/%\al exp (-(%\pi%t%f__%fast_/%\al)^2) -
2%\pi\Vr%\pi %f__%slow_/%\\al exp (-(%\pi%t%f__%slow_/%\al)^2)

This impulse response behaves well:
{ 5x3
	impulseResponse = Create Sound from formula: "impulseResponse", 1,
	... -0.2, 0.2, 2500, ~ 2 * pi * sqrt (pi) / alpha *
	... (30.0 * exp (- (pi * 30.0 / alpha * x) ^ 2) -
	...   3.0 * exp (- (pi * 3.0 / alpha * x) ^ 2))
	Red
	Draw: 0, 0, -100.0, 400.0, "no", "curve"
	Black
	Draw inner box
	Text bottom: "yes", "Time %t (s)"
	Text left: "yes", "Intensity impulse response %h (%t)"
	One mark bottom: -0.2, "yes", "yes", "no", ""
	One mark bottom: 0.0, "yes", "yes", "yes", ""
	One mark bottom: 0.2, "yes", "yes", "no", ""
	One mark left: 0.0, "yes", "yes", "yes", ""
}

We see that any short intensity peak will be enhanced, and that this enhancement will suppress the intensity
around 30 milliseconds from the peak. Non-Gaussian frequency-domain filters would have given several maxima and minima
in the impulse response, clearly an undesirable phenomenon.

After the filtered band is subjected to a backward Fourier transform, we convert it into power again:
~	%power (%t) = 10^^%filtered / 2^

The relative enhancement has a maximum that is smoothly related to the basilar place:
~	%ceiling = 1 + (10^^%enhancement / 20^ - 1) \\.c (1/2 - 1/2 cos (%\pi %f__%midbark_ / 13))

where %f__%midbark_ is the mid frequency of the band. Clipping is implemented as
~	%factor (%t) = 1 / (1 / %power (%t) + 1 / %ceiling)

Finally, the original filtered sound %x (%t), multiplied by this factor, is added to the output.
{-;
	form Consonantversterker
		positive Enhancement_(dB) 20
		choice Algoritme: 3
			button (1) Luide stukken spreiden in de tijd
			button (2) Versterken bij beweging
			button (3) Intensiteitscontour filteren
		comment Al deze algoritmen werken binnen alle frequentiebanden!
	endform
	smooth = 100
	low = 300
	high = 8000
	maximumFactor = 10^(Enhancement/20)

	Copy... original

	# Remove filter bands.
	To Spectrum (fft)
	Formula... if x<'low'-'smooth' then self else
	... if x<'low'+'smooth' then self*(0.5+0.5*cos(pi*(x-('low'-'smooth'))/(2*'smooth'))) else
	... if x<'high'-'smooth' then 0 else
	... if x<'high'+'smooth' then self*(0.5-0.5*cos(pi*(x-('high'-'smooth'))/(2*'smooth'))) else
	... self fi fi fi fi
	To Sound (fft)
	Rename... enhanced

	min = low
	while min < high
		mid_bark = hertzToBark (min) + 0.5
		max = round (barkToHertz (hertzToBark (min) + 1))
		if max > high
			max = high
		endif
		call enhance_one_band
		min = max
	endwhile

	select Spectrum original
	plus Sound original
	Remove

	select Sound enhanced
	Rename... 'Algoritme'_'Enhancement'
	Scale... 0.99

	procedure enhance_one_band
		select Sound original
		To Spectrum (fft)
		Formula... if x<'min'-'smooth'then 0 else
		... if x<'min'+'smooth' then self*(0.5-0.5*cos(pi*(x-('min'-'smooth'))/(2*'smooth'))) else
		... if x<'max'-'smooth' then self else
		... if x<'max'+'smooth' then self*(0.5+0.5*cos(pi*(x-('max'-'smooth'))/(2*'smooth'))) else
		... 0 fi fi fi fi
		Rename... band
		To Sound (fft)
	if Algoritme = 1
		To Intensity... 120 0.01
		select Intensity band
		Erase all
		Viewport... 0 6 0 4
		Draw... 0 5 0 80 yes
		Down to Matrix
		Rename... factor
		Formula... if Intensity_band [col-2] > self then Intensity_band [col-2] else self fi
		Formula... if Intensity_band [col-1] > self then Intensity_band [col-1] else self fi
		Formula... if Intensity_band [col+1] > self then Intensity_band [col+1] else self fi
		Formula... if Intensity_band [col+2] > self then Intensity_band [col+2] else self fi
		Red
		Draw rows... 0 5 0 0 0 80
		Black
		Formula... self - Intensity_band []
		Formula... if self > 'Enhancement' then 'Enhancement' else self fi
		Formula... 10^(self/20)
		Viewport... 0 6 4 8
		Draw rows... 0 5 0 0 0 Enhancement
		Draw inner box
		Text top... yes Band from 'min' to 'max' Hertz
		Marks bottom... 2 yes yes no
		Marks left every... 1 5 yes yes yes
		select Intensity band
		Remove
	elsif Algoritme = 2
		To Intensity... 120 0.01
		# Enhance contour
		Erase all
		Viewport... 0 6 0 4
		Draw... 0 5 0 80 yes
		Down to Matrix
		Rename... factor
		Formula... if col = 1 or col = ncol then 1 else
			... (abs (Intensity_band [col-1] - Intensity_band []) +
			... abs (Intensity_band [col+1] - Intensity_band []))*0.5 fi
		Formula... if self < 1 then 1 else if self > 'maximumFactor' then 'maximumFactor' else self fi fi
		Red
		Draw rows... 0 5 0 0 1 maximumFactor
		Black
		Text top... yes Band from 'min' to 'max' Hertz
		Marks right every... 1 1 yes yes yes
		select Intensity band
		Remove
	elsif Algoritme = 3
		Copy... intensity
		Formula... self^2+1e-6
	Formula... 10*log10(self)
		To Spectrum (fft)
		Rename... intensityfilt
		Formula... self*exp(-(x/35)^2)-self*exp(-(x/3.5)^2); if x<3 or x>30 then 0 else self fi;
		To Sound (fft)
		Erase all
		Viewport... 0 6 0 4
		Text top... no Band van 'min' tot 'max' Hertz
		Draw... 0 5 -10 10 yes
		Down to Matrix
		Rename... factor
		Formula... 10^(Sound_intensityfilt[]/2)
	ceiling = 1+(maximumFactor -1)*(0.5-0.5*cos(pi*mid_bark/13))
		Formula... 1/(1/self+1/'ceiling')
		Viewport... 0 6 4 8
		Draw rows... 0 5 0 0 0 ceiling
		Draw inner box
		Marks bottom... 2 yes yes no
		Marks left every... 1 1 yes yes yes
		select Sound intensity
		plus Spectrum intensityfilt
		plus Sound intensityfilt
		Remove
	endif
		select Sound band
		Formula... self*Matrix_factor()

		select Sound enhanced
		Formula... self+Sound_band[]

		select Matrix factor
		plus Spectrum band
		plus Sound band
		Remove
	endproc
}

{-;
	Sound: Filter (freq. domain):
	>> Nog iets anders. Ik heb het een tijdje geleden met jou gehad over 'troep'
	>> die met jouw eenvoudige filtertechniek in geluidsfiles terechtkomt. In de
	>> attachment zit een (synthetische) spraakuiting met hier en daar een (echte)
	>> stilte erin. Filter die maar eens met een 'telefoonfilter'(alles 0 voor
	>> x<300 or x>3400). Hier in Nijmegen krijg ik er dan een hele vieze zoemtoon
	>> in, die ook duidelijk te zien is in de stilten van het signaal.
	>
	>Je bedoelt met de formule if x<300 or x>3400 then 0 else self fi ?

	Ja


	>De top van de 300-Hz-ringing is bij mij -40 dB bij 0.54 seconde,
	>en -46 dB bij 1.89 seconde. Hoort dat niet?
}

################################################################################
"Sound: Extract part..."
© Paul Boersma 2024

A command to copy a part of each selected @Sound object to the Objects window.

Settings
========

##Time range (s)
: the time range (%t_1, %t_2) of the part that has to be extracted.

##Window shape# (standard: “rectangular”)
:	one of 12 possible window shapes; see below.

##Relative width# (standard: 1.0)
:	the relative width of the extracted part, as compared to the duration %t_2 \-m %t_1.
If the relative width is 1.0, then the part from %t_1 to %t_2 will be extracted.
If the relative width is 2.0, then the part from %t_1 \-m 0.5 %dur to %t_2 + 0.5 %dur will be extracted,
where %dur is %t_2 \-m %t_1. See below for when and why this is useful.

##Preserve times# (standard: off)
: if %on, then the time domain of the resulting extracted Sound object will run from %t_1 to %t_2.
If %off, then the resulting extracted Sound will be time-shifted to start at zero,
i.e. its time domain will run from 0 seconds to %t_2 \-m %t_1.

Window shapes
=============

Suppose we have the following sound, with the part we want to extract (from 0.06 to 0.16 seconds)
already marked:
{ 6x3
	sweep = Create Sound from formula: "sweep", 1, 0.0, 0.3, 44100, ~ sin(2*pi*1000*x^2)
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
	One mark bottom: 0.06, "yes", "yes", "no", ""
	One mark bottom: 0.16, "yes", "yes", "no", ""
}
We now extract the part form 0.05 to 0.15 seconds:
{
	Extract part: 0.06, 0.16, "rectangular", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With %%Preserve times% off, we instead get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "rectangular", 1.0, "no"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
These were extracted with a rectangular window. We have many other shapes
(see @@Sound: Multiply by window...@). With a Hanning window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Hanning", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With a Hamming window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Hamming", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With a triangular window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "triangular", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With a parabolic window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "parabolic", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With a Gaussian1 window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Gaussian1", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
With a Kaiser1 window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Kaiser1", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
These last six windows have a comparable effective duration.
The names of the last two windows, however, sound as if they
want to approximate a Gaussian shape, but they obviously don’t
(they don’t go to zero fast enough at the edges).
We can improve the Gaussianness of the window shape
by halving the effective duration.
With a Gaussian2 window, we get:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Gaussian2", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
and the very similar Kaiser2 window:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Kaiser2", 1.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
The advantage of these two windows is that they cut away too muich of the signal at the edges.
To make the effective window duration similar again, we would like to
extract a part that is physically twice as long.
We can do that by setting %%Relative width% to 2.0 instead of 1.0.
The Gaussian2 window then results in:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Gaussian2", 2.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
	One mark bottom: 0.06, "yes", "yes", "yes", ""
	One mark bottom: 0.16, "yes", "yes", "yes", ""
}
and the very similar Kaiser2 window:
{
	selectObject: sweep
	Extract part: 0.06, 0.16, "Kaiser2", 2.0, "yes"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
	One mark bottom: 0.06, "yes", "yes", "yes", ""
	One mark bottom: 0.16, "yes", "yes", "yes", ""
}
The Gaussian3, Gaussian4 and Gaussian5 windows approximate the Gaussian shape
even better than Gaussian2 and Kaiser2 do,
but they require even longer relative widths, namely 3.0, 4.0 and 5.0 respectively.

In Praat, most acoustical analyses are done with Kaiser2 windows,
such as @@Sound: To Spectrogram...@ and @@Sound: To Pitch (ac)...@
(the latter with the %%Very accurate% setting switched on).

################################################################################
"Extract selected sound (windowed)..."
© Paul Boersma 2024

A command in the Sound menu of several windows (SoundEditor, TextGridEditor...),
to copy the selected part of the sound to the Objects window.

For settings and behaviour, see @@Sound: Extract part...@.

################################################################################
"Sound: Filter (de-emphasis)..."
© Paul Boersma 2003-03-09

A command to filter every selected @Sound object. The resulting Sound object has a lower spectral slope.

The reverse of @@Sound: Filter (pre-emphasis)...@. For an example, see @@Source-filter synthesis@.

Setting
=======

##From frequency (Hz)
:	the frequency %F above which the spectral slope will decrease by 6 dB/octave.

Algorithm
=========

The de-emphasis factor %\al is computed as
~	%\al = exp (-2 %\pi %F %\Det)

where %\Det is the sampling period of the sound. The new sound %y is then computed recursively as:
~	%y_1 = %x_1
~	%y_%i = %x_%i + %\al %y__%i\-m1_

################################################################################
"Sound: Filter (formula)..."
© Paul Boersma 2004-11-23

A command to convert every selected @Sound object into a filtered sound.

The filtering is done in the frequency domain. This command is equivalent to the following sequence:
, 1. @@Sound: To Spectrum...@ yes
, 2. @@Matrix: Formula...
, 3. @@Spectrum: To Sound

For a comparative discussion of various filtering methods, see the @Filtering tutorial.

The example formula is the following:
{;
	if x<500 or x>1000 then 0 else self fi   ; rectangular band
}

This formula represents a rectangular pass band between 500 Hz and 1000 Hz (%x is the frequency).
Rectangular bands are %not recommended, since they may lead to an appreciable amount of %#ringing in the time domain.
The transition between stop and pass band should be smooth, as e.g. in @@Sound: Filter (pass Hann band)...@.

################################################################################
"Sound: Filter (one formant)..."
© Paul Boersma 2003-03-09

A command to filter every selected @Sound object, with a single formant of a specified frequency and bandwidth.

Algorithm
=========

Two recursive filter coefficients are computed as follows:
~	%p = \-m2 exp (\-m%\pi %bandwidth %dt) cos (2%\pi %frequency %dt)
~	%q = exp (\-m2%\\pi %bandwidth %dt)

where %dt is the sample period. The new signal %y is then computed from the old signal %x and itself as
~	%y__1_ := %x_1
~	%y__2_ := %x_2 \-m %p %y_1
~	\At%n \>_ 3:   %y_%n := %x_%n \-m %p %y__%n\-m1_ \-m %q %y__%n\-m2_

After filtering, the sound %y is scaled so that its absolute extremum is 0.9.

For a comparative discussion of various filtering methods, see the @Filtering tutorial.

This filter has an in-place version: @@Sound: Filter with one formant (in-place)...@.

################################################################################
"Sound: Filter with one formant (in-place)..."
© Paul Boersma 2017-11-14

A command to filter every selected @Sound object in-place, with a single formant of a specified frequency and bandwidth.

This is the in-place version of @@Sound: Filter (one formant)...@,
i.e. it does not create a new Sound object but modifies the selected object.

################################################################################
"Sound: Filter (pass Hann band)..."
© Paul Boersma 2004-11-23

A command to convert every selected @Sound object into a filtered sound.

The filtering is done in the frequency domain. This command is equivalent to the following sequence:
, 1. @@Sound: To Spectrum...
, 2. @@Spectrum: Filter (pass Hann band)...
, 3. @@Spectrum: To Sound

For a comparative discussion of various filtering methods, see the @Filtering tutorial.

For a complementary filter, see @@Sound: Filter (stop Hann band)...@.

################################################################################
"Sound: Filter (stop Hann band)..."
© Paul Boersma 2004-11-23

A command to convert every selected @Sound object into a filtered sound.

The filtering is done in the frequency domain. This command is equivalent to the following sequence:
, 1. @@Sound: To Spectrum...
, 2. @@Spectrum: Filter (stop Hann band)...
, 3. @@Spectrum: To Sound

For a comparative discussion of various filtering methods, see the @Filtering tutorial.

For a complementary filter, see @@Sound: Filter (pass Hann band)...@.

################################################################################
"Sound: Formula..."
© Paul Boersma 2002-12-06

A command for changing the data in all selected @Sound objects.

See the @Formulas tutorial for examples and explanations.

################################################################################
"Sound: Get absolute extremum..."
© Paul Boersma 2004-07-11

A @@Query submenu|query@ to the selected @Sound object.

Return value
============

the absolute extremum (in Pascal) within a specified time window.

Settings
========

%%Time range% (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

%%Interpolation
:	the interpolation method (None, Parabolic, Sinc) of the @@vector peak interpolation@.
The standard is Sinc70 because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Get energy..."
© Paul Boersma 2021-07-19

A @@Query submenu|query@ to the selected @Sound object.

Availability
============

This command becomes available in the Query submenu when you select one Sound.
Like most query commands, it is greyed out if you select two Sounds or more.

Behaviour
=========

If you execute this command, Praat should write the energy of the selected Sound (in the time interval you are asking for) into the Info window.
If the unit of sound amplitude is Pa (Pascal), the unit of energy will be Pa^2·s.

Setting
=======

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

Definition
==========

The energy is defined as
~	\in__%t1_^^%t2^ %x^2(%t) %dt

where %x(%t) is the amplitude of the sound. For stereo sounds, it is
~	\in__%t1_^^%t2^ (%x^2(%t) + %y^2(%t))/2 %dt

where %x(%t) and %y(%t) are the two channels; this definition, which averages (rather than sums) over the channels, ensures that
if you convert a mono sound to a stereo sound, the energy will stay the same.

Related commands
================

For an interpretation of the energy as the sound energy in air, see @@Sound: Get energy in air@.
For the power, see @@Sound: Get power...@.

Implementation
==============

In Praat, a Sound is defined only at a finite number of time points, spaced evenly.
For instance, a three-seconds long Sound with a sampling frequency of 10 kHz is defined at 30,000 time points,
which usually (e.g. when you create the Sound with @@Create Sound from formula...@) lie
at 0.00005, 0.00015, 0.00025 ... 2.99975, 2.99985 and 2.99995 seconds.
The simple way Praat looks at this is that the first %sample is centred around 0.00005 seconds,
and the amplitude of that sample (%x_1) represents %x(%t) for %t between 0 and 0.00010 seconds.
Likewise, the second sample is centred around 0.00015 seconds but can be said to run from 0.00010 to 0.00020 seconds,
and the 30,000th and last sample is centred around 2.99995 seconds and its amplitude (%x__30000_)
represents all times between 2.99990 and 3.00000 seconds.
This example sound %x(%t) is therefore defined for all times between 0 and 3 seconds,
but is undefined before 0 seconds or after 3 seconds.

The energy of the whole example sound is therefore
~	\in_0^3 %x^2(%t) %dt

and we approximate this as a sum over all 30,000 samples:
~	\su__%i=1_^^30000^ %x_%i^2 %\Det_%i

where %\Det_%i is the duration of the %%i%th sample, i.e. 0.0001 seconds for every sample.

Now consider what happens if we want to know the energy between %t_1 = 0.00013 and %t_2 = 0.00054 seconds.
The first sample of the sound falls entirely outside this interval;
70 percent of the second sample falls within the interval, namely the part from 0.00013 to 0.00020 seconds;
all of the third, fourth and fifth samples fall within the interval;
and 40 percent of the sixth sample falls within the interval, namely the part from 0.00050 to 0.00054 seconds
(note that the centre of this sixth sample, which is at 0.00055 seconds, even lies outside the interval).
The energy is then
~	\su__%i=2_^6 %x_%i^2 %\Det_%i

where %\Det_3 = %\Det_4 = %\Det_5 = 0.0001 seconds,
but %\Det_2 is only 0.00007 seconds (namely the part of the second sample that falls between %t_1 and %t_2),
and %\Det_6 is only 0.00004 seconds (namely the part of the sixth sample that falls between %t_1 and %t_2).

This way of integrating the squared signal (technically, a Riemann sum over a partition [of the interval from
%t_1 to %t_2] that is regular everywhere except at the edges and has central tags everywhere except at the edges)
ensures that the result is a continuous function of %t_1 and %t_2,
i.e., a very small change in %t_1 or %t_2 can only lead to a very small change in the computed energy
(instead, simply summing over all samples whose centre falls between %t_1 and %t_2 would result instead in a sudden jump
in the computed energy whenever %t_1 or %t_2 crosses a sample centre,
which would be unphysical behaviour and therefore not how Praat should behave;
see the @@energy integration continuity test@).

Edge cases
==========

If the sound is not defined everywhere between %t_1 and %t_2, then the energy is not defined there either.
Those times are skipped in the integral, i.e. they count as if they have zero energy.

################################################################################
"energy integration continuity test"
© Paul Boersma 2025-05-14

{
	\`{random_initializeWithSeedUnsafelyButPredictably} (89678363)
	samplingFrequency = 44100
	my.Sound = \@{Create Sound from formula:} "sineWithNoise", 1, 0.0, 1.0,
	... samplingFrequency, ~ 1/2 * sin(2*pi*377*x) + randomGauss(0,0.1)
}

What is the biggest step between two consecutive samples?
{
	difference.Sound = \@{Copy:} "difference"
	\@{Formula:} ~ if col = ncol then 0 else self [col + 1] - self [col] fi
	biggestAbsoluteWaveformStepBetweenSamples = \@{Sound: ||Get absolute extremum:} 0, 0, "none"
	\`{appendInfoLine}: "## biggest absolute waveform step between samples: ",
	... biggestAbsoluteWaveformStepBetweenSamples, " Pa"
}

This corresponds to the following biggest energy step between two consecutive samples:
{
	biggestAbsoluteEnergyStepBetweenSamples = biggestAbsoluteWaveformStepBetweenSamples ^ 2 / samplingFrequency
	\`{appendInfoLine}: "## biggest absolute energy step between samples: ",
	... biggestAbsoluteEnergyStepBetweenSamples, " Pa²s"
}

We’ll divide up every sample into 10 ministeps:
{
	oversampling = 10
}

Between any consecutive ministeps, the energy step should not exceed one tenth
of the biggest energy step between two consecutive samples:
{
	biggestAbsoluteEnergyMinistep = biggestAbsoluteEnergyStepBetweenSamples / oversampling
	\`{appendInfoLine}: "## biggest absolute energy ministep: ",
	... biggestAbsoluteEnergyMinistep, " Pa²s"
}

Large test:
{
	\`{stopwatch}
	minidt = 1.0 / samplingFrequency / oversampling
	tmin = 1e-9   ; not 0.0, in order to prevent autowindowing
	previousEnergy = \#@{Sound: ||Get energy:} 0, tmin
	for i from 1 to 10000
		energy = \#@{Sound: ||Get energy:} 0, tmin + i * minidt
		energyGain = energy - previousEnergy
		if i < 100
			\`{appendInfoLine}: i, " ", energy, " ", energyGain
		endif
		step = \`{abs} (energyGain)
		\`{assert} step > 0.0
		\`{assert} step < 1.0000000000001 * biggestAbsoluteEnergyMinistep   ; also captures undefined samples
		previousEnergy = energy
	endfor
	\`{appendInfoLine}: "## time elapsed: ", \`{fixed$} (\`{stopwatch} * 1000, 3), " ms"
}

################################################################################
"Sound: Get energy in air"
© Paul Boersma 2007-01-29

A @@Query submenu|query@ to the selected @Sound object.

Return value
============

The energy in air, expressed in Joule/m^2.

Algorithm
=========

The energy of a sound in air is defined as
~	1 / (%\ro%c) \in %x^2(%t) %dt

where %x(%t) is the sound pressure in units of Pa (Pascal), %\ro is the air density
(apx. 1.14 kg/m^3), and %c is the velocity of sound in air (apx. 353 m/s).
For how stereo sounds are handled, see @@Sound: Get energy...@.

See also
========

For an air-independent interpretation of the energy, see @@Sound: Get energy...@.
For the power, see @@Sound: Get power in air@.

################################################################################
"Sound: Get intensity (dB)"
© Paul Boersma 2007-01-29

A @@Query submenu|query@ to the selected @Sound object.

Return value
============

the intensity in air, expressed in dB relative to the auditory threshold.

Algorithm
=========

The intensity of a sound in air is defined as
~	10 log__10_ { 1 / (%T %P_0^2) \in%dt %x^2(%t) }

where %x(%t) is the sound pressure in units of Pa (Pascal), %T is the duration of the sound,
and %P_0 = 2·10^^-5^ Pa is the auditory threshold pressure.
For how stereo sounds are handled, see @@Sound: Get energy...@.

See also
========

For the intensity in Watt/m^2, see @@Sound: Get power in air@. For an auditory intensity,
see @@Excitation: Get loudness@.

################################################################################
"Sound: Get maximum..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the maximum amplitude (sound pressure in Pascal) within a specified time window.

Settings
========

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored, except for purposes of interpolation.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

##Interpolation
:	the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@.
The standard is “sinc70” because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Get mean..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the mean amplitude (sound pressure in Pascal) within a specified time range.

Setting
=======

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

Mathematical definition
=======================

The mean amplitude between the times %t_1 and %t_2 is defined as
~	1/(%t_2 \-m %t_1)  \in__%t1_^^%t2^ %x(%t) %dt

where %x(%t) is the amplitude of the sound in Pa.

################################################################################
"Sound: Get minimum..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the minimum amplitude (sound pressure in Pascal) within a specified time window.

Settings
========

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored, except for purposes of interpolation.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

%%Interpolation
:	the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@.
The standard is “sinc70” because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Get nearest zero crossing..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the time associated with the zero crossing nearest to a specified time point.
It is @undefined if there are no zero crossings or if the specified time is outside the time domain of the sound.
Linear interpolation is used between sample points.

Setting
=======

##Time (s)
:	the time for which you want to get the time of the nearest zero crossing.

################################################################################
"Sound: Get power..."
© Paul Boersma 1999,2004,2007,2022,2026

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the power within a specified time window.
If the unit of sound amplitude is Pa (Pascal), the unit of power will be Pa^2.

Setting
=======

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

Mathematical definition
=======================

The power is defined as
~	1/(%t_2\-m%t_1)  \in__%t1_^^%t2^ %x^2(%t) %dt

where %x(%t) is the amplitude of the sound.
For how stereo sounds are handled, see @@Sound: Get energy...@.

See also
========

For an interpretation of the power as the sound power in air, see @@Sound: Get power in air@.
For the total energy, see @@Sound: Get energy...@.

################################################################################
"Sound: Get power in air"
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the power in air, expressed in Watt/m^2.

Mathematical definition
=======================

The power of a sound in air is defined as
~	1 / (%\ro%c%T) \in %x^2(%t) %dt

where %x(%t) is the sound pressure in units of Pa (Pascal), %\ro is the air density
(apx. 1.14 kg/m^3), %c is the velocity of sound in air (apx. 353 m/s), and %T is the duration of the sound.
For how stereo sounds are handled, see @@Sound: Get energy...@.

For an air-independent interpretation of the power, see @@Sound: Get power...@.
For the energy, see @@Sound: Get energy in air@. For the intensity in dB,
see @@Sound: Get intensity (dB)@.

################################################################################
"Sound: Get root-mean-square..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the root-mean-square (rms) value of the sound pressure, expressed in Pascal.

Setting
=======

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

Mathematical definition
=======================

The root-mean-square value is defined as
~	\Vr { 1/(%t_2\-m%t_1)  \in__%t1_^^%t2^ %x^2(%t) %dt }

where %x(%t) is the amplitude of the sound.
For how stereo sounds are handled, see @@Sound: Get energy...@.

################################################################################
"Sound: Get standard deviation..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the standard deviation (in Pascal) of the sound pressure within a specified window.
If the sound contains less than 2 samples, the value is @undefined.

Setting
=======

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

Mathematical definition
=======================

The standard deviation is defined as
~	1/(%t_2-%t_1)  \in%__%t1_^^%t2^ (%x(%t) - %\mu)^2 %dt

where %x(%t) is the amplitude of the sound, and %\mu is its mean.
For our discrete Sound object, the standard deviation is approximated by
~	1/(%n\-m1) \su__%i=%m..%m+%n\-m1_ (%x_%i \-m %\mu)^2

where %n is the number of sample centres between %t_1 and %t_2. Note the \"minus 1\".

################################################################################
"Sound: Get time of maximum..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the time (in seconds) associated with the maximum pressure in a specified time range.

Settings
========

##Time range (s)
:	the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation.
If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.

##Interpolation
:	the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@.
The standard is “sinc70” because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Get time of minimum..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the time (in seconds) associated with the minimum pressure in a specified time range.

Settings
========

##Time range (s)
:	the time range (%t_1, %t_2). Values outside this range are ignored, except for purposes of interpolation.
If %t_1 is not less than %t_2, the entire time domain of the sound is considered.

##Interpolation
:	the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@.
The standard is “sinc70” because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Get value at sample number..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show the amplitude (sound pressure in Pascal) at a specified sample number.
If the sample number is less than 1 or greater than the number of samples, the result is @undefined.

Setting
=======

##Sample number
:	the sample number at which the value is to be evaluated.

################################################################################
"Sound: Get value at time..."
© Paul Boersma 2022-12-02

A command available in the @@Query submenu@ if you select a @Sound object.
The Info window will show an estimate of the amplitude (sound pressure in Pascal) at a specified time.
If that time is outside the samples of the Sound, the result is equal to the value of the nearest sample;
otherwise, the result is an interpolated value.

Settings
========

##Time (s)
:	the time at which the value is to be evaluated.

##Interpolation
:	the interpolation method, see @@vector value interpolation@.
The standard is “sinc70” because a Sound object is normally a sampled band-limited signal,
which can be seen as a sum of sinc functions.

################################################################################
"Sound: Lengthen (overlap-add)..."
© Paul Boersma 20030916)   // 2023--

A command to convert each selected @Sound object into a longer new @Sound object.

Settings
========

##Pitch floor (Hz)
:	the minimum pitch used in the periodicity analysis. The standard value is 75 Hz. For the voice of a young child, set this to 150 Hz."
The shortest voiceless interval in the decomposition is taken as 1.5 divided by %%minimum frequency%.

##Pitch ceiling (Hz)
:	the maximum pitch used in the periodicity analysis. The standard value is 600 Hz. For an adult male voice, set this to 300 Hz.

##Factor
:	the factor with which the sound will be lengthened. The standard value is 1.5. If you take a value less than 1,
the resulting sound will be shorter than the original. A value larger than 3 will not work.

Algorithm
=========

@@overlap-add@.

################################################################################
"Sound: Multiply by window..."
© Paul Boersma 2024

A command to multiply each selected @Sound object by a window shape.

Settings
========

##Window shape
:	one of 12 possible window shapes.

Window shapes
=============

Suppose we have the following sound:
{ 6x3
	Create Sound from formula: "sweep", 1, 0.0, 0.1, 44100, ~ sin(2*pi*1000*x^2)
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
The shape of a ##Hanning window# is
{ 6x3
	Create Sound from formula: "window", 1, 0.0, 0.1, 44100, ~ 1
	Multiply by window: "Hanning"
	Draw: 0, 0, 0.0, 1.0, "yes", "curve"
}
If you multiply these with each other, the result will be
{ 6x3
	selectObject: "Sound sweep"
	Multiply by window: "Hanning"
	Draw: 0, 0, -1.0, 1.0, "yes", "curve"
}
In short:
{-
	procedure drawWindowing: .windowShape$
		Create Sound from formula: "sweep", 1, 0.0, 0.1, 44100, ~ sin(2*pi*1000*x^2)
		Select outer viewport: 0, 3.1, 0, 2.5
		Draw: 0, 0, -1.1, 1.1, "yes", "curve"
		Create Sound from formula: "window", 1, 0.0, 0.1, 44100, ~ 1
		Multiply by window: .windowShape$
		Select outer viewport: 2.2, 5.3, 0, 2.5
		Draw: 0, 0, -1.1, 1.1, "no", "curve"
		Draw inner box
		Text bottom: "yes", "Time (s)"
		Marks bottom: 2, "yes", "yes", "no"
		Marks left: 2, "yes", "yes", "no"
		One mark left: 0.0, "no", "no", "yes", ""
		selectObject: "Sound sweep"
		Multiply by window: .windowShape$
		Text top: "no", "##" + .windowShape$
		Select outer viewport: 4.4, 7.5, 0, 2.5
		Draw: 0, 0, -1.1, 1.1, "no", "curve"
		Draw inner box
		Text bottom: "yes", "Time (s)"
		Marks bottom: 2, "yes", "yes", "no"
		Marks left: 2, "yes", "yes", "no"
		One mark left: 0.0, "no", "no", "yes", ""
		Axes: 0, 100, 0, 100
		info$ = Picture info
		fontSize = extractNumber (info$, "Font size: ")
		Select outer viewport: 2.2, 3.1, 0, 2.5
		Text special: 50, "centre", 50, "half", "Times", 2*fontSize, "0", "\xx"
		Select outer viewport: 4.4, 5.3, 0, 2.5
		Text special: 50, "centre", 50, "half", "Times", 2*fontSize, "0", "="
	endproc
}
{- 7.5x2.5
	@drawWindowing: "Hanning"
}
Other windows:
{- 7.5x2.5
	@drawWindowing: "Hamming"
}
{- 7.5x2.5
	@drawWindowing: "triangular"
}
{- 7.5x2.5
	@drawWindowing: "parabolic"
}
{- 7.5x2.5
	@drawWindowing: "Gaussian1"
}
{- 7.5x2.5
	@drawWindowing: "Kaiser1"
}
The following one does nothing:
{- 7.5x2.5
	@drawWindowing: "rectangular"
}
These two have an effective duration that is 50 percent shorter:
{- 7.5x2.5
	@drawWindowing: "Gaussian2"
}
{- 7.5x2.5
	@drawWindowing: "Kaiser2"
}
And these are narrower again:
{- 7.5x2.5
	@drawWindowing: "Gaussian3"
}
{- 7.5x2.5
	@drawWindowing: "Gaussian4"
}
{- 7.5x2.5
	@drawWindowing: "Gaussian5"
}

################################################################################
"audio control panel"
© Paul Boersma 2022-09-11,2024,2026

Your platform’s way of controlling where sounds will be played, and how loud.

On macOS X or 11 or 12 or 13 or 14 or 15 or 26, go to the Apple menu,
then to ##System Settings# (older: ##System Preferences#),
then to #Sound, then to #Output. If you want to change only the loudness,
then modern Macs have three keys on the keyboard for that.

On Ubuntu Linux 22.04 or 24.04, the fastest way is to click the loudspeaker icon in the menu bar,
then click the #Settings symbol (or the cartwheel), then #Output
(otherwise, look for the #Settings app, then #Sound, then #Output).

On Windows 10 or 11, it lies somewhat deeper:
go to the Start icon, then to #Settings, then to #System, then to #Sound, then to #Output.

################################################################################
"Sound: Play"
© Paul Boersma 2005-08-22

A command to play @Sound objects.

Availability
============

You can choose this command after selecting one or more Sounds.

Purpose
=======

To play the selected Sounds through the internal or external loudspeakers,
the headphones, or the analog or digital outputs of your computer.

Behaviour
=========

All of the Sounds selected are played,
in the order in which they appear in the list. If the sampling frequency of the Sound does not match any of
the system's sampling frequencies, a fast but inaccurate conversion is performed via linear interpolation.

Usage
=====

The output level and the choice of the output device(s) depend on the settings
in your @@audio control panel@.

################################################################################
"Sound: Draw..."
© Paul Boersma 2023

A command that becomes available in the #Draw menu when you select one or more @Sound objects.

Purpose
=======
To draw the selected Sound into the selected part of the @@Picture window@.

################################################################################
"Sound: Filter (pre-emphasis)..."
© Paul Boersma 2003-03-09

A command to filter each selected @Sound object. The resulting Sound object has a higher spectral slope.

The reverse of @@Sound: Filter (de-emphasis)...@.

Setting
=======

##From frequency (Hz)
:	the frequency %F above which the spectral slope will increase by 6 dB/octave.

Algorithm
=========

The pre-emphasis factor %\al is computed as
~	%\al = exp (-2 %\pi %F \De%t)

where \De%t is the sampling period of the sound. The new sound %y is then computed as:
~	%y_%i = %x_%i - %\al %x__%i-1_

################################################################################
"Sound: Pre-emphasize (in-place)..."
© Paul Boersma 2022-04-07

A command to change the spectral slope of every selected @Sound object.

The reverse of @@Sound: De-emphasize (in-place)...@.

This is the in-place version of @@Sound: Filter (pre-emphasis)...@,
i.e., it does not create a new Sound object but modifies an existing object.

Setting
=======

##From frequency (Hz)
:	the frequency %F above which the spectral slope will increase by 6 dB/octave.

Algorithm
=========

The pre-emphasis factor %\al is computed as
~	%\al = exp (-2 %\pi %F \De%t)

where \De%t is the sampling period of the sound. Every sample %x_%i of the sound,
except %x_1, is then changed, going down from the last sample:
~	%x_%i = %x_%i - %\al %x__%i-1_

################################################################################
"Sound: Resample..."
© Paul Boersma 2004-03-30

A command that creates new @Sound objects from the selected Sounds.

Purpose
=======

High-precision resampling from any sampling frequency to any other sampling frequency.

Settings
========

##Sampling frequency (Hz)
:	the new sampling frequency, in hertz.

##Precision
:	the depth of the interpolation, in samples (standard is 50).
This determines the quality of the interpolation used in resampling.

Algorithm
=========

If #Precision is 1, the method is linear interpolation, which is inaccurate but fast.
// BUG

If #Precision is greater than 1, the method is sin(%x)/%x (“%sinc”) interpolation,
with a depth equal to #Precision.
For higher #Precision, the algorithm is slower but more accurate.

If ##Sampling frequency# is less than the sampling frequency of the selected sound,
an anti-aliasing low-pass filtering is performed prior to resampling.

Behaviour
=========

A new Sound will appear in the list of objects,
bearing the same name as the original Sound, followed by the sampling frequency.
For instance, the Sound `hallo` will give a new Sound `hallo_10000`.

################################################################################
"Sound: Set value at sample number..."
© Paul Boersma 2014-04-21

A command to change a specified sample of the selected @Sound object.

Settings
========

##Sample number
:	the sample whose value is to be changed. Specify any value between 1 and the number of samples in the Sound.
If you specify a value outside that range, you will get an error message.

##New value
:	the value that is to be put into the specified sample.

Scripting
=========

Example:
{;
	\`{selectObject}: "Sound hallo"
	\#{Set value at sample number:} 100, 1/2
}
This sets the value of the 100th sample to 0.5.

################################################################################
"SoundEditor"
© Paul Boersma 20220814,2023-06-08

An @@Editors|Editor@ for viewing and editing a @Sound object.
Most of the functions of this editor are described in the @Intro.

The markers
===========

To set the cursor, use the left mouse button. A short horizontal line will also be shown at the left edge (in cyan);
the extension of this line crosses the cursor line at the sound's function value (seen as a cyan dot).
This function value (shown at the left in cyan) is the sinc-interpolated value, and is generally different from the
value that you would expect when looking at the linearly interpolated version of the sampled sound.

To select a part of the time domain, use the @@time selection@ mechanism.

Playing
=======

To play any part of the sound, click in one of the rectangles
below or above the sound window (there can be 1 to 8 of these rectangles),
or choose a command from the Play menu.

To mute one or more channels (of a multi-channel sound), @@Command-click@
on the 🔈 icon at the right side of the corresponding channel number.
The icon will turn to 🔇. In subsequent playing actions,
the channel will not be played. Another @@Command-click@ on a muted channel icon will activate the channel again.

Publishing
==========

To perform analyses on the selection, or save it to a file,
create an independent Sound as a copy of the selection,
by clicking on the button that will copy the selection to the List of Objects;
the resulting Sound will be called “Sound untitled”.

Editing
=======
- Cut: cut the selection to the clipboard, for later pasting into the same or another Sound.
- Copy selection to Sound clipboard: copy the selection to the clipboard, for later pasting into the same or another Sound.
- Paste after selection: paste the clipboard to the cursor or after the selection.
- Set selection to zero: set the selected samples to zero.
- Reverse selection: reverse the selected part of the sound.

You can undo these commands with Undo (@@Keyboard shortcuts|Command-Z@).

The Group button
================

To synchronize a SoundEditor window with other windows that show a time signal,
push the Group button in all the windows that you want to synchronize.
You cannot Cut from or Paste into a synchronized SoundEditor window.

################################################################################
"SoundRecorder"
© Paul Boersma 2020-11-20,2024

With the Praat SoundRecorder window you can record a mono or stereo sound
for subsequent viewing and analysis in Praat.
The SoundRecorder appears on your screen if you choose @@Record mono Sound...@ or @@Record stereo Sound...@
from the @@New menu@.

Depending on your system, the SoundRecorder window may allow you to choose
the sampling frequency, the input gain, and the input device (microphone, line, or digital).
The sound input level is monitored continuously with one or two meters.
The resulting sound has 16 bits per sample, like sounds on an audio CD.

Usage
=====

To record the sound, use the Record and Stop buttons in the SoundRecorder window.
Click ##Save to list# to copy the recorded sound to the object window
(or ##Save left channel to list# or ##Save right channel to list#
to copy the left or right channel if you have a stereo sound).
The name of the resulting Sound object will be taken from the text field next to the button clicked.

Size of the recording buffer
============================

The size of the recording buffer determines how many seconds of sound you can record.
For instance, if the recording buffer is 20 megabytes (the standard value), you can record 220 seconds in stereo
(440 seconds in mono) at a sampling frequency of 22050 Hz,
or 110 seconds in stereo (220 seconds in mono) at a sampling frequency of 44100 Hz.
You can change the size of the recording buffer
with #Praat \-> #Settings \-> ##Sound recording settings...#.

Recording sounds on the Mac or in Linux
======================================

On the Mac or in Linux, you can record from the list on the left in the SoundRecorder window.
The list can contain several devices, such as the internal microphone, a line input, or external USB devices.
Audio tracks on a CD can be opened directly with @@Read from file...@ or @@Open long sound file...@.

Recording sounds in Windows
===========================

In Windows 10, you can choose your input device by right-clicking on the loudspeaker icon in the Start bar,
then choosing ##Open Sound settings# \-> ##Choose your input device#. In Windows 11,
you right-click the loudspeaker icon and choose ##Sound settings# \-> ##Choose a device for speaking or recording# instead."
To set some input properties, right-click the loudspeaker icon,
then on Windows 10 you choose #Sounds \-> #Recording \-> #Properties,
while on Windows 11 you choose ##Sound settings# \-> #Advanced \-> ##All sound devices# \-> ##Input devices# \-> #Microphone.

Watching the input level
========================

While recording, you can watch the input level as a green rectangle whose size changes.
Whenever the input is loud, the top of the rectangle becomes yellow; if it turns red, the sound may have been clipped.
In the Meter menu you can choose other visualizations,
such as a moving ball that measures spectral centre of gravity (horizontally) versus intensity (vertically).

The File menu
=============

If your computer has little memory, a very long recorded sound might be too big to be copied to the list of objects.
Fortunately, the File menu contains commands to save the recording
to a sound file on disk, so that you will never have to lose your recording.
You can later open such a long sound file in Praat with @@Open long sound file...@ from the Open menu.

Sound pressure calibration
==========================

Your computer's sound-recording software returns integer values between -32768 and 32767.
Praat divides them by 32768 before putting them into a Sound object,
so that the values in the Sound objects are always between -1 and +1.

The Praat program considers these numbers to be air pressures in units of Pascal,
but they are probably not the actual true air pressures that went into the microphone.
For how to obtain the true air pressures, perform a @@sound pressure calibration@.

################################################################################
"Sounds: Concatenate"
© Paul Boersma 2011-02-11

A command to concatenate all selected @Sound objects into a single large Sound.
	All sounds must have equal sampling frequencies and equal numbers of channels.
They are concatenated in the order in which they appear in the list of objects
(not in the order in which you select them; remember: What You See Is What You Get).

How to concatenate directly to a file
=====================================
If the resulting sound does not fit into memory, use one of the
commands in the @@Save menu@. See @@How to concatenate sound files@.

See also
========
If you want the sounds to fade into each other smoothly, choose @@Sounds: Concatenate with overlap...@ instead.

################################################################################
"Sounds: Concatenate with overlap..."
© Paul Boersma 2017-09-04

A command to concatenate all selected @Sound objects into a single large Sound, with smooth cross-fading between the sounds.

All sounds must have equal sampling frequencies and equal numbers of channels.
They are concatenated in the order in which they appear in the list of objects (not in the order in which you select them; remember: What You See Is What You Get).

Settings
========
// 1 field, of which 0 texts and 0 additional radio buttons,
// hence lines = 1 + 0 * 0.6 - 0 * 0.3 = 1.0,
// hence height = 1.4 + 1.0 * 0.4 = 1.8
{- 5.4x1.8
	)~~~"
		Manual_DRAW_SETTINGS_WINDOW ("Sounds: Concatenate with overlap", 1.0)
		Manual_DRAW_SETTINGS_WINDOW_FIELD ("Overlap time (s)", "0.01")
	R"~~~(
}

##Overlap time (s)
:	the time by which any two adjacent sounds will come to overlap,
i.e. the time during which the earlier sound fades out and the later sound fades in.

Procedure
=========

Suppose we start with the following two sounds. They are both 0.1 seconds long.
The first sound is a sine wave with a frequency of 100 Hz, the second a sine wave with a frequency of 230 Hz:
{- 5.0x3.0
	Create Sound from formula: "sine100", 1, 0, 0.1, 10000, ~ 0.9*sin(2*pi*100*x)
	Draw: 0, 0, -1, 1, "yes", "Curve"
	Remove
}
{- 5.0x3.0
	Create Sound from formula: "sine230", 1, 0, 0.1, 10000, ~ 0.9*sin(2*pi*230*x)
	Draw: 0, 0, -1, 1, "yes", "Curve"
	Remove
}
If the overlap time is 0.01 seconds, the concatenation of these two sounds will produce a Sound with a duration of 0.19 seconds,
which is the sum of the durations of the two sounds, minus the overlap time.

The concatenation works in the following way.
The last 0.01 seconds of the first sound is multiplied by a falling raised cosine (the second half of a Hann window, see the first red curve),
and the first 0.01 seconds of the second sound is multiplied by a rising raised cosine (the first half of a Hann window, see the second red curve):
{- 6.7x5
	Create Sound from formula: "sine", 1, 0, 0.1, 10000, ~ 0.9
	Formula (part): 0.09, 0.1, 1, 1, ~ self*(0.5-0.5*cos(pi*(xmax-x)/0.01))
	Select inner viewport: 0.5, 3.5, 0.5, 2.5
	Red
	Draw: 0, 0, -1, 1, "no", "Curve"
	Formula: ~ self*sin(2*pi*100*x)
	Black
	Draw: 0, 0, -1, 1, "no", "Curve"
	Draw inner box
	One mark top: 0, "yes", "yes", "no", ""
	One mark top: 0.09, "yes", "yes", "yes", ""
	One mark top: 0.1, "yes", "yes", "no", ""
	Text top: "no", "Time (s)"
	One mark left: -1, "yes", "yes", "no", ""
	One mark left: 0, "yes", "yes", "yes", ""
	One mark left: 1, "yes", "yes", "no", ""
	Formula: ~ 0.9
	Formula (part): 0, 0.01, 1, 1, ~ self*(0.5-0.5*cos(pi*x/0.01))
	Select inner viewport: 3.2, 6.2, 2.5, 4.5
	Red
	Draw: 0, 0, -1, 1, "no", "Curve"
	Formula: ~ self*sin(2*pi*230*x)
	Black
	Draw: 0, 0, -1, 1, "no", "Curve"
	Draw inner box
	One mark bottom: 0, "yes", "yes", "no", ""
	One mark bottom: 0.01, "yes", "yes", "yes", ""
	One mark bottom: 0.1, "yes", "yes", "no", ""
	Text bottom: "no", "Time (s)"
	One mark right: -1, "yes", "yes", "no", ""
	One mark right: 0, "yes", "yes", "yes", ""
	One mark right: 1, "yes", "yes", "no", ""
	Remove
}

This figure shows how the two sounds are windowed (faded out and in), as well as how they will overlap.

Finally, the two windowed ("cross-faded") sounds are added to each other:
{- 6.7x3.0
	sine100 = Create Sound from formula... sine100 1 0 0.1 10000 0.9*sin(2*pi*100*x)
	sine230 = Create Sound from formula... sine100 1 0 0.1 10000 0.9*sin(2*pi*230*x)
	plus sine100
	Concatenate with overlap... 0.01
	Draw... 0 0 -1 1 yes Curve
	One mark bottom... 0.09 yes yes yes
	One mark bottom... 0.1 yes yes yes
	plus sine100
	plus sine230
	Remove
}

This example showed how it works if you concatenate two sounds;
if you concatenate three sounds, there will be two overlaps, and so on.

################################################################################
"Sounds: Convolve..."
© Paul Boersma & David Weenink 2010-04-04,2024

A command available in the #Combine menu when you select two @Sound objects.
This command convolves two selected @Sound objects with each other.
As a result, a new Sound will appear in the list of objects;
this new Sound is the %convolution of the two original Sounds.

Settings
========

##Amplitude scaling
:	Here you can choose between the “principled” options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below.
There is also a “pragmatic” option, namely ##peak 0.99#, which scales the resulting sound in such a way
that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it
(see @@Sound: Scale peak...@).

##Signal outside time domain is...
:	Here you can choose whether outside their time domains the sounds are considered to be #zero
(the standard value), or #similar to the sounds within the time domains.
This is explained in 4 below.

1. Convolution as an integral
=============================

The convolution %f*%g of two continuous time signals %f(%t) and %g(%t) is defined as the #integral
~	(%f*%g) (%t) \=3 \in %f(%\ta) %g(%t−%\ta) %d\ta

If %f and %g are sampled signals (as Sounds are in Praat), with the same @@sampling period@ %\Det,
the definition is discretized as
~	(%f*%g) [%t] \=3 \su_%\ta %f[%\ta] %g[%t−%\ta] %\Det

where %\ta and %t-%\ta are the discrete times at which %f and %g are defined, respectively.

Convolution is a commutative operation, i.e. %g*%f equals %f*%g.
This means that the order in which you put the two Sounds in the object list does not matter: you get the same result either way.

2. Convolution as a sum
=======================

You can see in the formula above that if both input Sounds are expressed in units of Pa,
the resulting Sound should ideally be expressed in Pa^^2^s.
Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.

This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes.
For this reason, Praat considers a different definition of convolution as well, namely as the #sum
~	(%f*%g) [%t] \=3 \su_%\ta %f[%\ta] %g[%t−%\ta]

The sum definition is appropriate if you want to filter a pulse train with a finite-impulse-response filter
and expect the amplitudes of each resulting period to be equal to the amplitude of the filter. Thus, the pulse train
{ 5x3
	Create Sound from formula: "peaks", 1, 0.0, 0.6, 1000, ~ x * (col mod 100 = 0)
	Draw: 0, 0, 0.0, 0.6, "yes", "curve"
}
convolved with the “leaky integrator” filter
{ 5x3
	Create Sound from formula: "leak", 1, 0.0, 1.0, 1000, ~ exp (-x / 0.1)
	Draw: 0, 0, 0.0, 1.0, "yes", "curve"
}
yields the convolution
{ 5x3
	selectObject: "Sound peaks", "Sound leak"
	Convolve: "sum", "zero"
	Draw: 0, 0, 0.0, 0.8523, "yes", "curve"
}

The difference between the integral and sum definitions is that in the sum definition
the resulting sound is divided by %\Det.

3. Normalized convolution
=========================

The %%normalized convolution% is defined as
~	(normalized %f*%g) (%t) \=3 \in %f(%\ta) %g(%t−%\ta) %d\ta
/ \Vr (\in %f^2(%\ta) %d\ta \in %g^^2^(%\ta) %d\ta)

4. Shape scaling
================

The boundaries of the integral in 1 are −\oo and +\oo.
However, %f and %g are Sound objects in Praat and therefore have finite time domains.
If %f runs from %t_1 to %t_2 and is assumed to be #zero before %t_1 and after %t_2, and
%g runs from %t_3 to %t_4 and is assumed to be zero outside that domain,
then the convolution will be zero before %t_1+%t_3 and after %t_2+%t_4,
while between %t_1+%t_3 and %t_2+%t_4 it is
~	(%f*%g) (%t) = \in__%t1_^^%t2^ %f(%\ta) %g(%t−%\ta) %d\ta

In this formula, the argument of %f runs from %t_1 to %t_2,
but the argument of %g runs from (%t_1+%t_3)−%t_2 to (%t_2+%t_4)−%t_1,
i.e. from %t_3−(%t_2−%t_1) to %t_4+(%t_2−%t_1).
This means that the integration is performed over two equal stretches of time during which %g must be taken zero,
namely a time stretch before %t_3 and a time stretch after %t_4, both of duration %t_2−%t_1
(equivalent equations can be formulated that rely on two stretches of %t_4−%t_3 of zeroes in %f rather than in %g,
or on a stretch of %t_2−%t_1 of zeroes in %g and a stretch of %t_4−%t_3 of zeroes in %f.

If you consider the sounds outside their time domains as #similar to what they are within their time domains, instead of #zero,
the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\ta] and %g[%t−%\ta],
without counting any multiplications of values outside the time domains.
Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere,
and %g is defined on the time domain [0, 3] with the value 1 everywhere.
Their convolution under the assumption that they are #zero elsewhere is then
{- 5x3
	Create Sound from formula... short mono 0 1.2 1000 1
	Create Sound from formula... long mono 0 3 1000 1
	plus Sound short
	Convolve... integral zero
	Draw... 0 0 0 1.5 yes curve
	One mark left... 1.2 yes yes yes
	plus Sound short
	plus Sound long
	Remove
}
but under the assumption that the sounds are #similar (i.e. 1) elsewhere, their convolution should be
{- 5x3
	Create Sound from formula... short mono 0 1.2 1000 1
	Create Sound from formula... long mono 0 3 1000 1
	plus Sound short
	Convolve... integral similar
	Draw... 0 0 0 1.5 yes curve
	One mark left... 1.2 yes yes yes
	plus Sound short
	plus Sound long
	Remove
}
i.e. a constant value of 1.2. This is what you get by choosing the #similar option;
if %f is shorter than %g, the first and last parts of the convolution will be divided by a straight line of duration %t_2−%t_1
to compensate for the fact that the convolution has been computed over fewer values of %f and %g there.

5. Behaviour
============

The start time of the resulting Sound will be the sum of the start times of the original Sounds,
the end time of the resulting Sound will be the sum of the end times of the original Sounds,
the time of the first sample of the resulting Sound will be the sum of the first samples of the original Sounds,
the time of the last sample of the resulting Sound will be the sum of the last samples of the original Sounds,
and the number of samples in the resulting Sound will be the sum of the numbers of samples of the original Sounds minus 1.

6. Behaviour for stereo and other multi-channel sounds
======================================

You can convolve e.g. a 10-channel sound either with another 10-channel sound or with a 1-channel (mono) sound.

If both Sounds have more than one channel, the two Sounds have to have the same number of channels;
each channel of the resulting Sound is then computed as the convolution of the corresponding channels
of the original Sounds. For instance, if you convolve two 10-channel sounds,
the resulting sound will have 10 channels, and its 9th channel will be the convolution of the 9th channels
of the two original sounds.

If one of the original Sounds has multiple channels and the other Sound has only one channel,
the resulting Sound will have multiple channels; each of these is computed as the convolution of
the corresponding channel of the multiple-channel original and the single channel of the single-channel original.
For instance, if you convolve a 10-channel sound with a mono sound,
the resulting sound will have 10 channels, and its 9th channel will be the convolution of the mono sound
with the 9th channel of the original 10-channel sound.

The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels
will be preserved in the resulting sound. For the #normalize scaling, for instance, the norm of %f in the formula above
is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak
of 0.99 in only one channel, and lower absolute peaks in the other channels.

7. Algorithm
============

The computation makes use of the fact that convolution in the time domain corresponds to multiplication in the frequency domain:
we first pad %f with a stretch of %t_4−%t_3 of zeroes and %g with a stretch of %t_2−%t_1 of zeroes (see 4 above),
so that both sounds obtain a duration of (%t_2−%t_1)+(%t_4−%t_3);
we then calculate the spectra of the two zero-padded sounds by Fourier transformation,
then multiply the two spectra with each other,
and finally Fourier-transform the result of this multiplication back to the time domain;
the result will again have a duration of (%t_2−%t_1)+(%t_4−%t_3).

################################################################################
"Sounds: Cross-correlate..."
© David Weenink & Paul Boersma 2010-04-04,2024

A command available in the #Combine menu when you select two @Sound objects.
This command cross-correlates two selected @Sound objects with each other.
As a result, a new Sound will appear in the list of objects;
this new Sound is the %cross-correlation of the two original Sounds.

Settings
========

##Amplitude scaling
:	Here you can choose between the “principled” options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below.
There is also a “pragmatic” option, namely ##peak 0.99#, which scales the resulting sound in such a way
that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it
(see @@Sound: Scale peak...@).

##Signal outside time domain is...
:	Here you can choose whether outside their time domains the sounds are considered to be #zero
(the standard value), or #similar to the sounds within the time domains.
This is explained in 4 below.

1. Cross-correlation as an integral
===================================

The cross-correlation of two continuous time signals %f(%t) and %g(%t) is a function of the lag time %\\ta,
and defined as the #integral
~	cross-corr (%f, %g) (%\ta) \=3 \in %f(%t) %g(%t+%\ta) %dt

If %f and %g are sampled signals (as Sounds are in Praat), with the same @@sampling period@ %%\\Det%,
the definition is discretized as
~	cross-corr (%f, %g) [%\ta] \=3 \su_%t %f[%t] %g[%t+%\ta] %\Det

where %\ta and %t+%\ta are the discrete times at which %f and %g are defined, respectively.

Cross-correlation is not a commutative operation, i.e. cross-corr (%g, %f) equals the time reversal of cross-corr (%f, %g).
This means that the order in which you put the two Sounds in the object list does matter:
the two results are each other's time reversals.

2. Cross-correlation as a sum
=============================

You can see in the formula above that if both input Sounds are expressed in units of Pa,
the resulting Sound should ideally be expressed in Pa^^2^s.
Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.

This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes.
For this reason, Praat considers a different definition of cross-correlation as well, namely as the #sum
~	cross-corr (%f, %g) [%\ta] \=3 \su_%t %f[%t] %g[%t+%\ta]

The difference between the integral and sum definitions is that in the sum definition
the resulting sound is divided by %\Det.

3. Normalized cross-correlation
===============================
The %%normalized cross-correlation% is defined as
~	norm-cross-corr (%f, %g) (%\ta) \=3 \in %f(%t) %g(%t+%\ta) %dt
/ \Vr (\in %f^2(%t) %dt \in %g^2(%t) %dt)

4. Shape scaling
================
The boundaries of the integral in 1 are −\oo and +\oo.
However, %f and %g are Sound objects in Praat and therefore have finite time domains.
If %f runs from %t_1 to %t_2 and is assumed to be #zero before %t_1 and after %t_2, and
%g runs from %t_3 to %t_4 and is assumed to be zero outside that domain,
then the cross-correlation will be zero before %t_3−%t_2 and after %t_4−%t_1,
while between %t_3−%t_2 and %t__4_−%t_1 it is
~	cross-corr (%f, %g) (%\ta) = \in__%t1_^^%t2^ %f(%t) %g(%t+%\ta) %dt

In this formula, the argument of %f runs from %t_1 to %t_2,
but the argument of %g runs from %t_1+(%t_3−%t_2) to %t_2+(%t_4−%t_1),
i.e. from %t_3−(%t_2−%t_1) to %t_4+(%t_2−%t_1).
This means that the integration is performed over two equal stretches of time during which %g must be taken zero,
namely a time stretch before %t_3 and a time stretch after %t_4, both of duration %t_2−%t_1
(equivalent equations can be formulated that rely on two stretches of %t_4−%t_3 of zeroes in %f rather than in %g,
or on a stretch of %t_2−%t_1 of zeroes in %g and a stretch of %t_4−%t_3 of zeroes in %f.

If you consider the sounds outside their time domains as #similar to what they are within their time domains, instead of #zero,
the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\ta] and %g[%t−%\ta],
without counting any multiplications of values outside the time domains.
Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere,
and %g is defined on the time domain [0, 3] with the value 1 everywhere.
Their cross-correlation under the assumption that they are #zero elsewhere is then
{ 5x3
	short = Create Sound from formula: "short", 1, 0.0, 1.2, 1000, ~ 1
	long = Create Sound from formula: "long", 1, 0.0, 3.0, 1000, ~ 1
	plusObject: short
	Cross-correlate: "integral", "zero"
	Draw: 0, 0, 0.0, 1.5, "yes", "curve"
	One mark left: 1.2, "yes", "yes", "yes", ""
}
but under the assumption that the sounds are #similar (i.e. 1) elsewhere, their cross-correlation should be
{ 5x3
	selectObject: short, long
	Cross-correlate: "integral", "similar"
	Draw: 0, 0, 0.0, 1.5, "yes", "curve"
	One mark left: 1.2, "yes", "yes", "yes", ""
}

i.e. a constant value of 1.2. This is what you get by choosing the #similar option;
if %f is shorter than %g, the first and last parts of the cross-correlation will be divided by a straight line of duration %t_2−%t_1
to compensate for the fact that the cross-correlation has been computed over fewer values of %f and %g there.

5. Behaviour
============

The start time of the resulting Sound will be the start time of %f minus the end time of %g,
the end time of the resulting Sound will be the end time of %f minus the start time of %g,
the time of the first sample of the resulting Sound will be the first sample of %f minus the last sample of %g,
the time of the last sample of the resulting Sound will be the last sample of %f minus the first sample of %g,
and the number of samples in the resulting Sound will be the sum of the numbers of samples of %f and %g minus 1.

6. Behaviour for stereo and other multi-channel sounds
======================================

You can cross-correlate e.g. a 10-channel sound either with another 10-channel sound or with a 1-channel (mono) sound.

If both Sounds have more than one channel, the two Sounds have to have the same number of channels;
each channel of the resulting Sound is then computed as the cross-correlation of the corresponding channels
of the original Sounds. For instance, if you cross-correlate two 10-channel sounds,
the resulting sound will have 10 channels, and its 9th channel will be the cross-correlation of the 9th channels
of the two original sounds.

If one of the original Sounds has multiple channels and the other Sound has only one channel,
the resulting Sound will have multiple channels; each of these is computed as the cross-correlation of
the corresponding channel of the multiple-channel original and the single channel of the single-channel original.
For instance, if you cross-correlate a 10-channel sound with a mono sound,
the resulting sound will have 10 channels, and its 9th channel will be the cross-correlation of the mono sound
with the 9th channel of the original 10-channel sound.

The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels
will be preserved in the resulting sound. For the #normalize scaling, for instance, the norm of %f in the formula above
is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak
of 0.99 in only one channel, and lower absolute peaks in the other channels.

7. Algorithm
============

The computation makes use of the fact that cross-correlation in the time domain corresponds to multiplication
of the time-reversal of %f with %g in the frequency domain:
we first pad %f with a stretch of %t_4−%t_3 of zeroes and %g with a stretch of %t_2−%t_1 of zeroes (see 4 above),
so that both sounds obtain a duration of (%t_2−%t_1) + (%t_4−%t_3);
we then calculate the spectra of the two zero-padded sounds by Fourier transformation,
then multiply the complex conjugate of the spectrum of %f with the spectrum of %g,
and finally Fourier-transform the result of this multiplication back to the time domain;
the result will again have a duration of (%t_2−%t_1) + (%t_4−%t_3).

################################################################################
"Sound: Autocorrelate..."
© David Weenink & Paul Boersma 2010-04-04,2024

A command available in the #Periodicity menu when you select one or more @Sound objects.
This command autocorrelates the selected @Sound object.
As a result, a new Sound will appear in the list of objects;
this new Sound is the %autocorrelation of the original Sound.

Settings
========

##Amplitude scaling
:	Here you can choose between the “principled” options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below.
There is also a “pragmatic” option, namely ##peak 0.99#, which scales the resulting sound in such a way
that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it
(see @@Sound: Scale peak...@).

##Signal outside time domain is...
:	Here you can choose whether outside its time domain the sound is considered to be #zero
(the standard value), or #similar to the sound within the time domain.
This is explained in 4 below.
//DEFINITION (U"Here you can choose whether outside its time domain the sound is considered to be #zero
//(the standard value), or #similar to the sound within the time domain, or #periodic, i.e. that
//outside the time domain the waveforms is a repetition of the waveform within the time domain.
//This is explained in 4 below.

1. Autocorrelation as an integral
=================================

The autocorrelation of a continuous time signal %f(%t) is a function of the lag time %\ta,
and defined as the #integral
~	%R_%f (%\ta) \=3 \in %f(%t) %f(%t+%\ta) %dt

If %f is a sampled signal (as Sounds are in Praat), with @@sampling period@ %\Det,
the definition is discretized as
~	%R_%f [%\ta] \=3 \su_%t %f[%t] %f[%t+%\ta] %\Det

where %\ta and %t+%\ta are the discrete times at which %f is defined.

The autocorrelation is symmetric: %R_%f (−%\ta) = %R_%f (%\ta).

2. Autocorrelation as a sum
===========================

You can see in the formula above that if the input Sound is expressed in units of Pa,
the resulting Sound should ideally be expressed in Pa^2s.
Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.

This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes.
For this reason, Praat considers a different definition of autocorrelation as well, namely as the #sum
~	%R_%f [%\ta] \=3 \su_%t %f[%t] %g[%t+%\ta]

The difference between the integral and sum definitions is that in the sum definition
the resulting sound is divided by %\Det.

3. Normalized autocorrelation
=============================

The %%normalized autocorrelation% is defined as
~	norm-autocorr (%f) (%\ta) \=3 \in %f(%t) %f(%t+%\ta) %dt / \in %f^2(%t) %dt

4. Shape scaling
================

The boundaries of the integral in 1 are −\oo and +\oo.
However, %f is a Sound object in Praat and therefore has a finite time domain.
If %f runs from %t_1 to %t_2 and is assumed to be #zero before %t_1 and after %t_2,
then the autocorrelation will be zero before %t_1−%t_2 and after %t_2−%t_1,
while between %t_1−%t_2 and %t_2−%t_1 it is
~	%R_%f (%\ta) = \in__%t1_^^%t2^ %f(%t) %f(%t+%\ta) %dt

In this formula, the argument of the first %f runs from %t_1 to %t_2,
but the argument of the second %f runs from %t_1+(%t_1−%t_2) to %t_2+(%t_2−%t_1),
i.e. from %t_1−(%t_2−%t_1) to %t_2+(%t_2−%t_1).
This means that the integration is performed over two equal stretches of time during which %f must be taken zero,
namely a time stretch before %t_1 and a time stretch after %t_2, both of duration %t_2−%t_1.

If you consider the sound outside its time domains as #similar to what it is within its time domain, instead of #zero,
the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\ta] and %f[%t-%\ta],
without counting any multiplications of values outside the time domain.
Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere.
Its autocorrelation under the assumption that it is #zero elsewhere is then
{ 5x3
	Create Sound from formula: "sound", 1, 0.0, 1.2, 1000, ~ 1
	Autocorrelate: "integral", "zero"
	Draw: 0, 0, 0.0, 1.5, "yes", "curve"
	One mark left: 1.2, "yes", "yes", "yes", ""
}
but under the assumption that the sound is #similar (i.e. 1) elsewhere, its autocorrelation should be
{ 5x3
	Create Sound from formula: "sound", 1, 0.0, 1.2, 1000, ~ 1
	Autocorrelate: "integral", "similar"
	Draw: 0, 0, 0.0, 1.5, "yes", "curve"
	One mark left: 1.2, "yes", "yes", "yes", ""
}
i.e. a constant value of 1.2. This is what you get by choosing the #similar option;
the autocorrelation will be divided by a triangular function
to compensate for the fact that the autocorrelation has been computed over fewer values closer to the edges;
this procedure is followed in all autocorrelation-based pitch computations in Praat (see @@Sound: To Pitch...@).
For examples, see @@Boersma (1993)@.

5. Behaviour
============

The start time of the resulting Sound will be the start time of %f minus the end time of %f,
the end time of the resulting Sound will be the end time of %f minus the start time of %f,
the time of the first sample of the resulting Sound will be the first sample of %f minus the last sample of %f,
the time of the last sample of the resulting Sound will be the last sample of %f minus the first sample of %f,
and the number of samples in the resulting Sound will be twice the number of samples of %f, minus 1.

6. Behaviour for stereo and other multi-channel sounds
======================================

If the selected Sound has more than one channel, each channel of the resulting Sound is computed
as the cross-correlation of the corresponding channel
of the original Sound. For instance, if you autocorrelate a 10-channel sound,
the resulting sound will again have 10 channels, and its 9th channel will be the autocorrelation of the 9th channel
of the original sound.

The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels
will be preserved in the resulting sound. For the #normalize scaling, for instance, the squared norm of %f in the formula above
is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak
of 0.99 in only one channel, and lower absolute peaks in the other channels.

Algorithm
=========

The autocorrelation is calculated as the @@Sounds: Cross-correlate...|cross-correlation@ of a sound with itself.

################################################################################
"Sound: Scale intensity..."
© Paul Boersma 2012-06-14

A command available in the #Modify menu when you select one or more @Sound objects.
With this command you multiply the amplitude of each Sound in such a way that its average (i.e. root-mean-square) intensity becomes
the ##new average intensity# that you specify (see Settings).

Settings
========

##New average intensity (dB SPL)
:	the new average intensity of the Sound. The standard value is 70 dB SPL. This means that the root-mean-square amplitude
of the sound will come to lie 70 dB above the assumed auditory threshold of 0.00002 Pa. Please check whether as a result a peak in the sound
does not get below -1 Pa or above +1 Pa. If that happens, the sound will be clipped when played, which will be audible as distortion;
in that case you may want to set this number to 60 dB or even lower.

################################################################################
"Sound: Scale peak..."
© Paul Boersma 2010-03-28,2024

A command available in the #Modify menu when you select one or more @Sound objects.
With this command you multiply the amplitude of each Sound in such a way that its absolute peak becomes
the ##new absolute peak# that you specify (see Settings).

Settings
========

##New absolute peak
:	the new absolute peak of the Sound. The standard value is 0.99: this maximizes the audibility of the Sound
(sounds with lower amplitude are weaker) without distorting it
(sounds with absolute peaks above 1 are clipped when they are played).

Examples
========

The absolute peak of the following sound is 0.033:
{ 6.0x3.0
	Create Sound from formula: "033", 1, 0.0, 0.1, 10000,
	... ~ -0.02 * (sin (2 * pi * 205 * x) + sin (2 * pi * 5 * x)) + 0.007
	Draw: 0, 0, -0.033, 0.0247, "yes", "curve"
}

This sound will play rather weakly. To make it louder, you can do ##Scale peak...# with a ##new absolute peak# of 0.99.
Praat will then multiply the waveform by 30, changing the sound to the following:
{ 6.0x3.0
	Scale peak: 0.99
	Draw: 0, 0, -0.99, 0.741, "yes", "curve"
}

The absolute peak is now 0.99, and the sound will play loudly.

The reverse is also possible. The absolute peak of the following sound is 19.8:
{ 6.0x3.0
	Create Sound from formula: "198", 1, 0.0, 0.1, 10000,
	... ~ 12 * (sin (2 * pi * 305 * x) + sin (2 * pi * 5 * x)) - 4.2
	Draw: 0, 0, -15.28, 19.8, "yes", "curve"
}

This sound will not play correctly: all samples with an amplitude outside the [-1;+1] range will be clipped to -1 or +1.
To make this sound nicer to play, you can again do ##Scale peak...# with a ##new absolute peak# of 0.99.
Praat will then divide the waveform by 20, changing the sound to the following:
{ 6.0x3.0
	Scale peak: 0.99
	Draw: 0, 0, -0.764, 0.99, "yes", "curve"
}

The absolute peak is now 0.99, and the sound will play without distortion.
)~~~"
MAN_PAGES_END

}

/* End of file manual_sound.cpp */
