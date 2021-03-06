CFLAGS=-Wall -O3 $(shell pkg-config --cflags opencv)
CFLAGS+=$(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv)

all: ibird_tracking camera_tune

ibird_tracking: ibird_tracking.cc particle_filter.o
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< particle_filter/particle_filter.o $(LDFLAGS) -lstdc++ -lm -O3
	
debug: ibird_tracking.cc particle_filter_debug.o
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< particle_filter/particle_filter_debug.o $(LDFLAGS) -lm -lstdc++ -fno-omit-frame-pointer -g

profile: ibird_tracking.cc particle_filter_profile.o
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< particle_filter/particle_filter_profile.o $(LDFLAGS) -lm -lstdc++ -fno-omit-frame-pointer -g -pg

particle_filter.o: particle_filter/particle_filter.cc
	gcc $(CFLAGS) -o particle_filter/$@ -c $< $(LDFLAGS) -lstdc++ -lm -O3

particle_filter_debug.o: particle_filter/particle_filter.cc
	gcc $(CFLAGS) -o particle_filter/$@ -c $< $(LDFLAGS) -lstdc++ -lm -fno-omit-frame-pointer -g

particle_filter_profile.o: particle_filter/particle_filter.cc
	gcc $(CFLAGS) -o particle_filter/$@ -c $< $(LDFLAGS) -lstdc++ -lm -fno-omit-frame-pointer -g -pg

camera_tune: camera_tune.cc
	mkdir -p bin
	gcc $(CFLAGS) -o bin/$@ $< $(LDFLAGS) -lstdc++ -lm -O3

clean:
	mkdir -p bin
	rm -f *.pyc
	rm -f bin/*
	rm -f particle_filter/*.o
