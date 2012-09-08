CFLAGS=-Wall -O3 $(shell pkg-config --cflags opencv)
CFLAGS+=$(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv)

all: ibird_tracking camera_tune

ibird_tracking: ibird_tracking.cpp
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< $(LDFLAGS) -lm -lstdc++ -O3

debug: ibird_tracking.cpp
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< $(LDFLAGS) -lm -lstdc++ -fno-omit-frame-pointer -ggdb

camera_tune: camera_tune.cpp
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< $(LDFLAGS) -lm -lstdc++ -O3

clean:
	mkdir -p bin
	rm -f bin/*
