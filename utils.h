#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <aubioext.h>

#ifdef HAVE_C99_VARARGS_MACROS
#define debug(...)              if (verbose) fprintf (stderr, __VA_ARGS__)
#define errmsg(...)             fprintf (stderr, __VA_ARGS__)
#define outmsg(...)             fprintf (stdout, __VA_ARGS__)
#else
#define debug(format, args...)  if (verbose) fprintf(stderr, format , ##args)
#define errmsg(format, args...) fprintf(stderr, format , ##args)
#define outmsg(format, args...) fprintf(stdout, format , ##args)
#endif

typedef int (*aubio_process_func_t)
        (smpl_t **input, smpl_t **output, int nframes);

extern int frames;
extern unsigned int median;
extern int verbose;
extern int usedoubled;

extern aubio_onsetdetection_type type_onset;
extern aubio_onsetdetection_type type_onset2;
extern smpl_t threshold;
extern smpl_t silence;
extern uint_t buffer_size;
extern uint_t overlap_size;
extern uint_t channels;
extern uint_t samplerate;

extern aubio_pvoc_t * pv;
extern fvec_t * ibuf;
extern fvec_t * obuf;
extern cvec_t * fftgrain;
extern fvec_t * woodblock;
extern aubio_onsetdetection_t *o;
extern aubio_onsetdetection_t *o2;
extern fvec_t *onset;
extern fvec_t *onset2;
extern int isonset;
extern aubio_pickpeak_t * parms;

/* pitch objects */
extern smpl_t pitch;
extern aubio_pitchdetection_t * pitchdet;
extern aubio_pitchdetection_type mode;
extern uint_t median;

extern fvec_t * note_buffer;
extern fvec_t * note_buffer2;
extern smpl_t curlevel;
extern smpl_t maxonset;

/* midi objects */
extern aubio_midi_player_t * mplay; 
extern aubio_midi_driver_t * mdriver; 
extern aubio_midi_event_t  * event;

extern smpl_t curnote;
extern smpl_t newnote;
extern uint_t isready;
