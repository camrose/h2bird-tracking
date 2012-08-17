CFLAGS=-Wall -O3 $(shell pkg-config --cflags opencv)
CFLAGS+=$(shell pkg-config --cflags opencv)
LDFLAGS=$(shell pkg-config --libs opencv)

ibird_tracking: ibird_tracking.cpp
	gcc $(CFLAGS) -o bin/$@ $< camera.cpp $(LDFLAGS) -lm -lstdc++ -O3

debug: ibird_tracking.cpp 
	gcc $(CFLAGS) -o bin/$@ $< camera.cpp $(LDFLAGS) -lm -lstdc++ -fno-omit-frame-pointer -ggdb

camera_tune: camera_tune.cpp
	gcc $(CFLAGS) -o bin/$@ $< camera.cpp $(LDFLAGS) -lm -lstdc++ -O3

clean:
	rm -f bin/*
