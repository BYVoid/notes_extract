#include "utils.h"

unsigned int pos = 0; /*frames%dspblocksize*/
uint_t usepitch = 1;

FILE * midi_text;

static int note_velocity = 48;
static int note_pos;

void send_noteon(int pitch, int velo) {
    smpl_t mpitch = floor(aubio_freqtomidi(pitch) + 0.5);
    double curr = frames * overlap_size / (float) samplerate;
    note_pos = curr * note_velocity * 2;
    if (pitch == 0) return;
    fprintf(midi_text, "%d On ch=1 n=%.0f v=%d\n", note_pos, mpitch, velo);
    /*
      if (velo == 0) {
          outmsg("%f\n",frames * overlap_size / (float)samplerate);
      } else {
          outmsg("%f\t%f\t", mpitch,
                      frames * overlap_size / (float)samplerate);
      }
      */
}

void note_append(fvec_t * note_buffer, smpl_t curnote) {
  uint_t i = 0;
  for (i = 0; i < note_buffer->length - 1; i++) { 
      note_buffer->data[0][i] = note_buffer->data[0][i+1];
  }
  note_buffer->data[0][note_buffer->length - 1] = curnote;
  return;
}

uint_t get_note(fvec_t *note_buffer, fvec_t *note_buffer2) {
  uint_t i = 0;
  for (i = 0; i < note_buffer->length; i++) { 
      note_buffer2->data[0][i] = note_buffer->data[0][i];
  }
  return vec_median(note_buffer2);
}

int process(float **input, float **output, int nframes) {
  unsigned int i;       /*channels*/
  unsigned int j;       /*frames*/
  for (j=0;j<(unsigned)nframes;j++) {
    /*time for fft*/
    if (pos == overlap_size - 1) {         
      /* block loop */
      aubio_pvoc_do (pv, ibuf, fftgrain);
      aubio_onsetdetection(o, fftgrain, onset);
      if (usedoubled) {
        aubio_onsetdetection(o2, fftgrain, onset2);
        onset->data[0][0] *= onset2->data[0][0];
      }
      isonset = aubio_peakpick_pimrt(onset,parms);
      
      pitch = aubio_pitchdetection(pitchdet,ibuf);
      if (median) {
        note_append(note_buffer, pitch);
      }

      /* curlevel is negatif or 1 if silence */
      curlevel = aubio_level_detection(ibuf, silence);
      if (isonset) {
        /* test for silence */
        if (curlevel == 1.) {
          isonset=0;
          if (median) isready = 0;
          /* send note off */
          send_noteon(curnote,0);
        } else {
          if (median) {
            isready = 1;
          } else {
            /* kill old note */
            send_noteon(curnote,0);
            /* get and send new one */
            send_noteon(pitch,127+(int)floor(curlevel));
            curnote = pitch;
          }
          for (pos = 0; pos < overlap_size; pos++) {
            obuf->data[0][pos] = woodblock->data[0][pos];
          }
        }
      } else {
        if (median) {
          if (isready > 0)
            isready++;
          if (isready == median) {
            /* kill old note */
            send_noteon(curnote, 0);
            newnote = get_note(note_buffer, note_buffer2);
            curnote = newnote;
            /* get and send new one */
            if (curnote>45) {
              send_noteon(curnote, 127 + (int)floor(curlevel));
            }
          }
        } // if median
        for (pos = 0; pos < overlap_size; pos++)
          obuf->data[0][pos] = 0.;
      }
      /* end of block loop */
      pos = -1; /* so it will be zero next j loop */
    }
    pos++;
  }
  return 1;
}

int main(int argc, char **argv) {
  common_init(argc, argv);
  midi_text = fopen("notes.txt", "w");
  fprintf(midi_text, "MFile 0 1 %d\n", note_velocity);
  fprintf(midi_text, "MTrk\n");
  fprintf(midi_text, "0 Meta Text \"Notes\"\n");
  
  common_process(process);
  send_noteon(curnote, 0);
  common_del();

  note_pos += note_velocity * 2 * 2;
  fprintf(midi_text, "%d Meta TrkEnd\n", note_pos);
  fprintf(midi_text, "TrkEnd\n");
  fclose(midi_text);
  
  toMIDI("notes.txt", "notes.mid");
  return 0;
}
