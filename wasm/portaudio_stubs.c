/* portaudio_stubs.c
 *
 * Stub implementations of PortAudio functions for the WASM build.
 * Audio I/O is not available in the browser/WASM environment,
 * so these stubs return error codes to indicate unavailability.
 *
 * This file is only compiled for the WASM target.
 */

#include "../external/portaudio/portaudio.h"

PaError Pa_Initialize (void) {
	return paNoError;
}

PaError Pa_Terminate (void) {
	return paNoError;
}

PaDeviceIndex Pa_GetDeviceCount (void) {
	return 0;
}

PaDeviceIndex Pa_GetDefaultInputDevice (void) {
	return paNoDevice;
}

PaDeviceIndex Pa_GetDefaultOutputDevice (void) {
	return paNoDevice;
}

const PaDeviceInfo * Pa_GetDeviceInfo (PaDeviceIndex device) {
	(void) device;
	return (const PaDeviceInfo *) 0;
}

const char * Pa_GetErrorText (PaError errorCode) {
	(void) errorCode;
	return "PortAudio not available in WASM build";
}

PaError Pa_OpenStream (
	PaStream **stream,
	const PaStreamParameters *inputParameters,
	const PaStreamParameters *outputParameters,
	double sampleRate,
	unsigned long framesPerBuffer,
	PaStreamFlags streamFlags,
	PaStreamCallback *streamCallback,
	void *userData)
{
	(void) stream; (void) inputParameters; (void) outputParameters;
	(void) sampleRate; (void) framesPerBuffer; (void) streamFlags;
	(void) streamCallback; (void) userData;
	return paInternalError;
}

PaError Pa_StartStream (PaStream *stream) {
	(void) stream;
	return paInternalError;
}

PaError Pa_StopStream (PaStream *stream) {
	(void) stream;
	return paNoError;
}

PaError Pa_CloseStream (PaStream *stream) {
	(void) stream;
	return paNoError;
}

PaError Pa_AbortStream (PaStream *stream) {
	(void) stream;
	return paNoError;
}

PaError Pa_IsStreamActive (PaStream *stream) {
	(void) stream;
	return 0;
}

PaError Pa_IsStreamStopped (PaStream *stream) {
	(void) stream;
	return 1;
}

const PaStreamInfo * Pa_GetStreamInfo (PaStream *stream) {
	(void) stream;
	return (const PaStreamInfo *) 0;
}

PaTime Pa_GetStreamTime (PaStream *stream) {
	(void) stream;
	return 0.0;
}

double Pa_GetStreamCpuLoad (PaStream *stream) {
	(void) stream;
	return 0.0;
}

const PaHostApiInfo * Pa_GetHostApiInfo (PaHostApiIndex hostApiIndex) {
	(void) hostApiIndex;
	return (const PaHostApiInfo *) 0;
}

PaError Pa_ReadStream (PaStream *stream, void *buffer, unsigned long frames) {
	(void) stream; (void) buffer; (void) frames;
	return paInternalError;
}

PaError Pa_WriteStream (PaStream *stream, const void *buffer, unsigned long frames) {
	(void) stream; (void) buffer; (void) frames;
	return paInternalError;
}

PaHostApiIndex Pa_GetHostApiCount (void) {
	return 0;
}

PaHostApiIndex Pa_GetDefaultHostApi (void) {
	return 0;
}

void Pa_Sleep (long msec) {
	(void) msec;
}
