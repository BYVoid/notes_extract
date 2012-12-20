#include "utils.h"

unsigned int pos = 0; /*frames%dspblocksize*/
uint_t usepitch = 0;

void process_print (void) {
  /* output times in seconds, taking back some 
   * delay to ensure the label is _before_ the
   * actual onset */
  if (isonset) {
    if(frames >= 4) {
      outmsg("%f\n",(frames-4)*overlap_size/(float)samplerate);
    } else if (frames < 4) {
      outmsg("%f\n",0.);
    }
  }
}

int process(float **input, float **output, int nframes) {
  unsigned int i;       /*channels*/
  unsigned int j;       /*frames*/
  for (j=0;j<(unsigned)nframes;j++) {

    /*time for fft*/
    if (pos == overlap_size-1) {         
      /* block loop */
      aubio_pvoc_do (pv,ibuf, fftgrain);
      aubio_onsetdetection(o,fftgrain, onset);
      if (usedoubled) {
        aubio_onsetdetection(o2,fftgrain, onset2);
        onset->data[0][0] *= onset2->data[0][0];
      }
      isonset = aubio_peakpick_pimrt(onset,parms);
      if (isonset) {
        /* test for silence */
        if (aubio_silence_detection(ibuf, silence)==1)
          isonset = 0;
      }
      /* end of block loop */
      pos = -1; /* so it will be zero next j loop */
    }
    pos++;
  }
  process_print();
  return 1;
}

int main(int argc, char **argv) {
  common_init(argc, argv);
  common_process(process);
  common_del();
  return 0;
}
