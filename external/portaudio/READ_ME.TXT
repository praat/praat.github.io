Praats/external/portaudio/READ_ME.TXT
Paul Boersma, 30 April 2022
This file describes the adaptations to the PortAudio 19.7.0 sources
that are needed to make them compatible with Praat.

Deleted many lines in pa_***_hostapis.c.

Around pa_linux_alsa.c, do
#if defined (UNIX) && defined (ALSA)
...
#endif

Around pa_jack.c, do
#if defined (UNIX) && defined (JACK)
...
#endif

Around pa_win_*.c, do
#ifdef _WIN32
...
#endif

Around pa_unix_util.c, do
#if defined (UNIX) || defined (macintosh)
...
#endif

Replace all occurrences of
	#if defined (PA_BIG_ENDIAN)
with
	#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
and all occurrences of
	#if defined (PA_LITTLE_ENDIAN)
with
	#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
and as a result remove all of pa_endianness.h that has to do with these macros.

Copy declarations from pa_unix_hostapis.c and pa_win_hostapis.c to pa_hostapi.h, like this:

/*
	ppgb: the following declarations copied here from pa_unix_hostapis.c.
*/
PaError PaJack_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaAlsa_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaOSS_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
/* Added for IRIX, Pieter, oct 2, 2003: */
PaError PaSGI_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
/* Linux AudioScience HPI */
PaError PaAsiHpi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaMacCore_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );

/*
	ppgb: the following declarations copied here from pa_win_hostapis.c.
*/
PaError PaSkeleton_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinMme_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinDs_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaAsio_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWinWdm_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
PaError PaWasapi_Initialize( PaUtilHostApiRepresentation **hostApi, PaHostApiIndex index );
