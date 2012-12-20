#include "utils.h"

unsigned int pos = 0;
fvec_t * out = NULL;
aubio_tempo_t * bt = NULL;
uint_t istactus = 0;

int process(float **input, float **output, int nframes) {
  unsigned int i;       /*channels*/
  unsigned int j;       /*frames*/
  for (j=0;j<(unsigned)nframes;j++) {
    if (pos == overlap_size-1) {    
      aubio_tempo(bt, ibuf, out);
      if (out->data[0][0]==1) 
        istactus = 1;
      else 
        istactus = 0;
      pos = -1;
    }
    pos++;
  }
  if (istactus)
    outmsg("%f\n",(frames)*overlap_size/(float)samplerate); 
  if (isonset && verbose)
    outmsg(" \t \t%f\n",(frames)*overlap_size/(float)samplerate);
  return 1;
}

int main(int argc, char **argv) {
  buffer_size = 1024;
  overlap_size = 512;
  common_init(argc,argv);
  out = new_fvec(2,channels);
  bt = new_aubio_tempo(type_onset, buffer_size, overlap_size, channels);
  common_process(process);
  del_aubio_tempo(bt);
  del_fvec(out);
  common_del();
  return 0;
}
