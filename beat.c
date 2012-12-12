#include <aubio.h>
#include "utils.h"

unsigned int pos          = 0;    /* frames%dspblocksize */
uint_t usepitch           = 0;
fvec_t * out              = NULL;
aubio_tempo_t * bt        = NULL;
uint_t istactus           = 0;

void process_print (void) {
        if (output_filename == NULL) {
                if (istactus)
                        outmsg("%f\n",(frames)*overlap_size/(float)samplerate); 
                if (isonset && verbose)
                        outmsg(" \t \t%f\n",(frames)*overlap_size/(float)samplerate);
        }
}

int aubio_process(float **input, float **output, int nframes) {
  unsigned int i;       /*channels*/
  unsigned int j;       /*frames*/
  for (j=0;j<(unsigned)nframes;j++) {
    /*time for fft*/
    if (pos == overlap_size-1) {         
      /* block loop */
      aubio_tempo(bt,ibuf,out);
      if (out->data[0][0]==1) 
        istactus = 1;
      else 
        istactus = 0;
      if (istactus) {
              for (pos = 0; pos < overlap_size; pos++)
                      obuf->data[0][pos] = woodblock->data[0][pos];
      } else {
              for (pos = 0; pos < overlap_size; pos++)
                      obuf->data[0][pos] = 0.;
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
  
  buffer_size = 1024;
  overlap_size = 512;
  /* override default settings */
  common_init(argc,argv);

  out = new_fvec(2,channels);
  bt  = new_aubio_tempo(type_onset,buffer_size,overlap_size,channels);

  common_process(aubio_process);

  del_aubio_tempo(bt);
  del_fvec(out);

  common_del();

  debug("End of program.\n");

  fflush(stderr);

  return 0;
}

