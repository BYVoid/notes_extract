CC = gcc
OPTFLAGS = -O0
INCLUDES = -I/usr/include/aubio
FLAGS = $(OPTFLAGS) $(INCLUDES) -c
LDFLAGS = -laubio -laubioext

TARGETS = notes onset beat
ALLOBJS = $(patsubst %.c, %.o, $(wildcard *.c))
T2MF_OBJS = midifile.o t2mf.o t2mflex.o yyread.o
BEAT_OBJS = beat.o utils.o $(T2MF_OBJS)
NOTES_OBJS = notes.o utils.o $(T2MF_OBJS)
ONSET_OBJS = onset.o utils.o $(T2MF_OBJS)

all: $(TARGETS)

%.o : %.c *.h
	$(CC) $(FLAGS) -o $@ $<

notes: $(NOTES_OBJS)
	gcc $(NOTES_OBJS) -o $@ $(LDFLAGS)

beat: $(BEAT_OBJS)
	gcc $(BEAT_OBJS) -o $@ $(LDFLAGS)

onset: $(ONSET_OBJS)
	gcc $(ONSET_OBJS) -o $@ $(LDFLAGS)

.PHONY : clean

clean :
	rm -f $(ALLOBJS) $(TARGETS)
	rm -f *.mid *.txt *.png