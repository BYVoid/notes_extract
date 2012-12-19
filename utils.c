#include "aubio.h"

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h> /* for isfinite */
#include "utils.h"

/* settings */
const char * output_filename = NULL;
const char * input_filename  = NULL;
const char * onset_filename  = "woodblock.aiff";
int frames = 0;
int verbose = 0;
int usedoubled = 1;

/* energy,specdiff,hfc,complexdomain,phase */
aubio_onsetdetection_type type_onset  = aubio_onset_kl;
aubio_onsetdetection_type type_onset2 = aubio_onset_complex;
smpl_t threshold                      = 0.3;
smpl_t silence                        = -90.;
uint_t buffer_size                    = 512; //1024;
uint_t overlap_size                   = 256; //512;
uint_t channels                       = 1;
uint_t samplerate                     = 44100;

//type_onset = aubio_onset_energy;
//type_onset = aubio_onset_specdiff;
//type_onset = aubio_onset_hfc;
//type_onset = aubio_onset_complex;
//type_onset = aubio_onset_complex;
//type_onset = aubio_onset_phase;
//type_onset = aubio_onset_mkl;
//type_onset = aubio_onset_kl;

aubio_sndfile_t * file = NULL;
aubio_sndfile_t * fileout = NULL;

aubio_pvoc_t * pv;
fvec_t * ibuf;
fvec_t * obuf;
cvec_t * fftgrain;
fvec_t * woodblock;
aubio_onsetdetection_t *o;
aubio_onsetdetection_t *o2;
fvec_t *onset;
fvec_t *onset2;
int isonset = 0;
aubio_pickpeak_t * parms;

/* pitch objects */
smpl_t pitch               = 0.;
aubio_pitchdetection_t * pitchdet;
aubio_pitchdetection_type type_pitch = aubio_pitch_yin;
aubio_pitchdetection_mode mode_pitch = aubio_pitchm_freq;
//type_pitch = aubio_pitch_mcomb;
//type_pitch = aubio_pitch_yinfft;
//type_pitch = aubio_pitch_yin;
//type_pitch = aubio_pitch_schmitt;
//type_pitch = aubio_pitch_fcomb;

uint_t median         = 6;

fvec_t * note_buffer  = NULL;
fvec_t * note_buffer2 = NULL;
smpl_t curlevel       = 0.;
smpl_t maxonset       = 0.;

/* midi objects */
aubio_midi_player_t * mplay; 
aubio_midi_driver_t * mdriver; 
aubio_midi_event_t  * event;

smpl_t curnote = 0.;
smpl_t newnote = 0.;
uint_t isready = 0;

/* badly redeclare some things */
aubio_onsetdetection_type type_onset;
smpl_t averaging = 0;

void common_init(int argc, char ** argv) {
  aubio_sndfile_t * onsetfile = NULL;
  
  if (argc == 1) {
    printf("filename pitch_algo(0:mcomb 1:yinfft 2:yin 3:schmitt 4:fcomb)\n");
    exit(1);
  }
  
  /* parse command line arguments */
  input_filename = argv[1];
  
  int pitch_algo = atoi(argv[2]);
  switch (pitch_algo) {
    case 0:
      type_pitch = aubio_pitch_mcomb;
      break;
    case 1:
      type_pitch = aubio_pitch_yinfft;
      break;
    case 2:
      type_pitch = aubio_pitch_yin;
      break;
    case 3:
      type_pitch = aubio_pitch_schmitt;
      break;
    case 4:
      type_pitch = aubio_pitch_fcomb;
      break;
  }

  woodblock = new_fvec(buffer_size,1);
  if (output_filename) {
    /* dummy assignement to keep egcs happy */
    isonset = (onsetfile = new_aubio_sndfile_ro(onset_filename)) ||
          (onsetfile = new_aubio_sndfile_ro("sounds/woodblock.aiff")) ||
          (onsetfile = new_aubio_sndfile_ro("../sounds/woodblock.aiff"));
    if (onsetfile == NULL) {
      outmsg("Could not find woodblock.aiff\n");
      exit(1);
    }
  }
  if (onsetfile) {
    /* read the output sound once */
    aubio_sndfile_read(onsetfile, overlap_size, woodblock);
  }

  debug("Opening files ...\n");
  file = new_aubio_sndfile_ro (input_filename);
  if (file == NULL) {
    outmsg("Could not open input file %s.\n", input_filename);
    exit(1);
  }
  if (verbose) aubio_sndfile_info(file);
  channels = aubio_sndfile_channels(file);
  samplerate = aubio_sndfile_samplerate(file);
  if (output_filename != NULL)
    fileout = new_aubio_sndfile_wo(file, output_filename);

  ibuf      = new_fvec(overlap_size, channels);
  obuf      = new_fvec(overlap_size, channels);
  fftgrain  = new_cvec(buffer_size, channels);

  if (usepitch) {
    pitchdet = new_aubio_pitchdetection(buffer_size*4, 
        overlap_size, channels, samplerate, type_pitch, mode_pitch);
    aubio_pitchdetection_set_yinthresh(pitchdet, 0.7);

    if (median) {
      note_buffer = new_fvec(median, 1);
      note_buffer2= new_fvec(median, 1);
    }
  }
  /* phase vocoder */
  pv = new_aubio_pvoc(buffer_size, overlap_size, channels);
  /* onsets */
  parms = new_aubio_peakpicker(threshold);
  o = new_aubio_onsetdetection(type_onset,buffer_size,channels);
  onset = new_fvec(1, channels);
  if (usedoubled)    {
    o2 = new_aubio_onsetdetection(type_onset2,buffer_size,channels);
    onset2 = new_fvec(1 , channels);
  }
}

void common_del(void){
  if (usepitch) {
          //send_noteon(curnote,0);
          del_aubio_pitchdetection(pitchdet);
          if (median) {
                  del_fvec(note_buffer);
                  del_fvec(note_buffer2);
          }
  }
  if (usedoubled)    {
    del_aubio_onsetdetection(o2);
    del_fvec(onset2);
  }
  del_aubio_onsetdetection(o);
  del_aubio_peakpicker(parms);
  del_aubio_pvoc(pv);
  del_fvec(obuf);
  del_fvec(ibuf);
  del_cvec(fftgrain);
  del_fvec(onset);
  del_fvec(woodblock);
  aubio_cleanup();
}

void common_process(aubio_process_func_t process_func){
  debug("Processing ...\n");
  frames = 0;
  while ((signed)overlap_size == aubio_sndfile_read(file, overlap_size, ibuf)) {
    isonset=0;
    process_func(ibuf->data, obuf->data, overlap_size);
    if (output_filename != NULL) {
      aubio_sndfile_write(fileout,overlap_size,obuf);
    }
    frames++;
  }
  debug("Processed %d frames of %d samples.\n", frames, buffer_size);
  del_aubio_sndfile(file);
  if (output_filename != NULL)
    del_aubio_sndfile(fileout);
}
