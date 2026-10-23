CC ?= gcc
CFLAGS ?= -O2 -s -Wall -Wextra -DMA_NO_FLAC -DMA_NO_MP3 -DMA_NO_VORBIS -DMA_NO_OPUS
LDFLAGS ?= -lpthread -lm -ldl
TARGET = ../bin/mech-sound

all: $(TARGET)

$(TARGET): mech-sound.c miniaudio.h
	mkdir -p ../bin
	$(CC) $(CFLAGS) mech-sound.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
