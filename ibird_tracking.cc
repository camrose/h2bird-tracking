/*
 * ibird_tracking.cpp
 *
 *  Created on: Sep 09, 2011
 *      Authors: Ryan Julian, Cameron Rose, and Humphrey Hu
 *  
 */
 
/* C Standard Library */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* CPP Standard Library */
#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>

/* OpenCV */
#include <opencv2/opencv.hpp>

/* Project */
#include "particle_filter/particle_filter.hpp"

/* Modes (TODO: command line) */
#define DISPLAY_CAPTURE             0 // Display camera capture
#define DISPLAY_PIPELINE            0 // Display all steps of pipeline
#define RECORD                      0 // Record video
#define VERBOSE                     1 // Lots of printings

/* Camera driver config values */
#define CAM_BRIGHTNESS      (0.4)
#define CAM_CONTRAST        (0.11)
#define CAM_SATURATION      (0.11)
#define CAM_GAIN            (1.0)

using namespace cv;
using namespace std;

RNG rng;
VideoWriter record;
char filename[200];

vector<Mat> frames;

int main( int argc, char** argv ) {
  
  Mat frame;
  Size frame_size;    
  double cam_brightness, cam_contrast, cam_saturation, cam_gain;

  // Open and configure camera
  VideoCapture cap(0);
  if( !cap.isOpened() ) {
    cout << "Camera open failed." << endl;
    return -1;
  }

  // Set camera parameters
  cap.set(CV_CAP_PROP_BRIGHTNESS, CAM_BRIGHTNESS);
  cap.set(CV_CAP_PROP_CONTRAST, CAM_CONTRAST);
  cap.set(CV_CAP_PROP_SATURATION, CAM_SATURATION);
  cap.set(CV_CAP_PROP_GAIN, CAM_GAIN);
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
  cap.set(CV_CAP_PROP_FPS, 30);
  cap.set(CV_CAP_PROP_FORMAT, 0);
  cap.set(CV_CAP_PROP_FORMAT, CV_FOURCC('Y','U','Y','V'));

  // Get camera parameters to make sure they were set correctly
  frame_size = Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  cam_brightness = cap.get(CV_CAP_PROP_BRIGHTNESS);
  cam_contrast = cap.get(CV_CAP_PROP_CONTRAST);
  cam_saturation = cap.get(CV_CAP_PROP_SATURATION);
  cam_gain = cap.get(CV_CAP_PROP_GAIN);

#if VERBOSE
  printf("Opened %u by %u camera stream.\n", frame_size.width, frame_size.height);
  printf("Brightness: %f\n", cam_brightness);
  printf("Contrast: %f\n", cam_contrast);
  printf("Saturation: %f\n", cam_saturation);
  printf("Gain: %f\n", cam_gain);
#endif

  // open video recording
#if RECORD
  record = VideoWriter("../video/output.mjpg", CV_FOURCC('M','J','P','G'), 25, frame_size, true);
  if( !record.isOpened() ) {
    printf("Recording failed to open!\n");
    return -1;
  }
#endif
  int stop = 0;
  double framerate;
  clock_t prev_time, new_time;
    
  prev_time = clock();

  cap >> frame;
  imshow("First frame",frame);
  Mat frameHSV(frame.size(), frame.type());
  Mat colorRangeMask(frame.size(), frame.type());

  while(1) {

    cap >> frame;

    cout.flush();

    new_time = clock();
    framerate = CLOCKS_PER_SEC/((float)(new_time - prev_time));
    prev_time = new_time;
    

#if VERBOSE
    cout << "Framerate: " << framerate << "\n";
#endif
 
    processNewFrame( frame, frameHSV, colorRangeMask );
    if(waitKey(5) == 'q') {
        break;
    }
    
#if RECORD
    frames.push_back(*(new Mat));
    frame.copyTo(frames[frames.size()-1]);
#endif
    /*if(frames.size() > 1) {
      printf("frame pointers = %d", frames[frames.size()-2]);
    }*/
    stop++;
#if RECORD
    if(stop > 80) {
      break;
    }
#endif
  }
  
  cap.release();
 
#if RECORD
  unsigned int i=0;
  for (i=0; i < frames.size(); i++) {
    record << (frames[i]);
  }
#endif

  return 0;

}

