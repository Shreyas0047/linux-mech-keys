CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra
LDFLAGS ?= -lpthread -lm -ldl
TARGET = ../bin/mech-sound

all: $(TARGET)

$(TARGET): mech-sound.c miniaudio.h
	mkdir -p ../bin
	$(CC) $(CFLAGS) mech-sound.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
