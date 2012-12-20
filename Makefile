CC = gcc
OPTFLAGS = -O0
INCLUDES = -I./lib -I./ext
FLAGS = $(OPTFLAGS) $(INCLUDES) -c
LDFLAGS = -lm -lfftw3 -lsndfile

TARGETS = notes onset beat
ALLOBJS = $(patsubst %.c, %.o, $(wildcard *.c))
T2MF_OBJS = midifile.o t2mf.o t2mflex.o yyread.o
AUBIO_OBJS = \
	lib/mathutils.o \
    lib/phasevoc.o \
    lib/onsetdetection.o \
    lib/pitchdetection.o \
    lib/pitchfcomb.o \
    lib/pitchmcomb.o \
    lib/pitchschmitt.o \
    lib/pitchyin.o \
    lib/pitchyinfft.o \
    lib/peakpick.o \
    lib/sample.o \
    lib/fft.o \
    lib/filter.o \
    lib/hist.o \
    lib/biquad.o \
    lib/scale.o \
    lib/tempo.o \
    lib/beattracking.o \
    ext/sndfileio.o
BEAT_OBJS = beat.o utils.o $(T2MF_OBJS) $(AUBIO_OBJS)
NOTES_OBJS = notes.o utils.o $(T2MF_OBJS) $(AUBIO_OBJS)
ONSET_OBJS = onset.o utils.o $(T2MF_OBJS) $(AUBIO_OBJS)

all: $(TARGETS)

%.o : %.c *.h
	$(CC) $(FLAGS) -o $@ $<

notes: $(NOTES_OBJS)
	$(CC) $(NOTES_OBJS) -o $@ $(LDFLAGS)

beat: $(BEAT_OBJS)
	$(CC) $(BEAT_OBJS) -o $@ $(LDFLAGS)

onset: $(ONSET_OBJS)
	$(CC) $(ONSET_OBJS) -o $@ $(LDFLAGS)

.PHONY : clean

clean :
	rm -f $(ALLOBJS) $(TARGETS)
	rm -f *.mid *.txt *.png
