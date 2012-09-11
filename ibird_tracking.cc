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
#include "particle_filter/particle_filter.h"

/* Modes (TODO: command line) */
#define DISPLAY                     0 // Display camera capture
#define RECORD                      0 // Record video
#define VERBOSE                     1 // Lots of printing

/* Camera driver config values */
#define CAM                 (0)
#define CAM_WIDTH           (320)
#define CAM_HEIGHT          (240)
#define CAM_BRIGHTNESS      (0.4)
#define CAM_CONTRAST        (0.11)
#define CAM_SATURATION      (0.11)
#define CAM_GAIN            (1.0)

/* Particle filter parameters */
#define NUM_PARTICLES       (1000)
#define STEP_SIZE           TransitionModel(8)
#define TARGET_COLOR        EmissionModel(107, 166, 165) // Tennis ball

/* Timing macros */
#define DECLARE_TIMING(s)  int64 timeStart_##s; double timeDiff_##s; double timeTally_##s = 0; int countTally_##s = 0
#define START_TIMING(s)    timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s) 	   timeDiff_##s = (double)(cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s) 	   (double)(timeDiff_##s / (cvGetTickFrequency()*1000.0))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? timeTally_##s/ ((double)countTally_##s * cvGetTickFrequency()*1000.0) : 0)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0

using namespace cv;
using namespace std;

VideoWriter record;
char filename[200];
vector<Mat> frames;

int main( int argc, char** argv ) {
  
  Mat frame;
  Size frame_size;    
  double cam_brightness, cam_contrast, cam_saturation, cam_gain;

  // Open and configure camera
  VideoCapture cam(CAM);
  if( !cam.isOpened() ) {
    cout << "Camera open failed." << endl;
    return -1;
  }

  // Set camera parameters
  cam.set(CV_CAP_PROP_BRIGHTNESS, CAM_BRIGHTNESS);
  cam.set(CV_CAP_PROP_CONTRAST, CAM_CONTRAST);
  //cam.set(CV_CAP_PROP_SATURATION, CAM_SATURATION);  // not supported on PS Eye
  cam.set(CV_CAP_PROP_GAIN, CAM_GAIN);
  cam.set(CV_CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
  //cam.set(CV_CAP_PROP_FPS, 125);                     // not supported on PS Eye
  cam.set(CV_CAP_PROP_FORMAT, 0);

  // Get camera parameters to make sure they were set correctly
  frame_size = Size(cam.get(CV_CAP_PROP_FRAME_WIDTH), cam.get(CV_CAP_PROP_FRAME_HEIGHT));
  cam_brightness = cam.get(CV_CAP_PROP_BRIGHTNESS);
  cam_contrast = cam.get(CV_CAP_PROP_CONTRAST);
  //cam_saturation = cam.get(CV_CAP_PROP_SATURATION);
  cam_gain = cam.get(CV_CAP_PROP_GAIN);

#if VERBOSE
  printf("Opened %u by %u camera stream.\n", frame_size.width, frame_size.height);
  printf("Brightness: %f\n", cam_brightness);
  printf("Contrast: %f\n", cam_contrast);
  //printf("Saturation: %f\n", cam_saturation);
  printf("Gain: %f\n", cam_gain);
#endif
  
  // Show first frame
  cam >> frame;
  imshow("First frame",frame);

  // Open video recording
#if RECORD
  record = VideoWriter("../video/output.mjpg", CV_FOURCC('M','J','P','G'), 25, frame_size, true);
  if( !record.isOpened() ) {
    printf("Recording failed to open!\n");
    return -1;
  }
#endif

  // Setup particle filter
  static const Range b[] = { Range(0, frame_size.height-1), 
                             Range(0, frame_size.width-1) };
  vector<Range> bounds(b, b + sizeof(b) / sizeof(b[0]));
  cout << "Initializing particle filter..." << endl;
  Vec3b color(107, 166, 165);
  //ParticleFilter pf();
  ParticleFilter pf(NUM_PARTICLES);
  //ParticleFilter pf(NUM_PARTICLES, bounds, 8, color);
  cout << "Done initializing particle filter..." << endl;
  
  DECLARE_TIMING(frameTimer);
  int count = 0;
  START_TIMING(frameTimer);
  while(1) {
    //START_TIMING(frameTimer); // Start timing
    
    cam >> frame;             // Capture a new frame
    count++;
    //pf.Observe(frame);      // Process frame
    
    //STOP_TIMING(frameTimer);  // Stop timing
    //cout.flush();

    if(waitKey(5) == 0x100000 + 'q') {
        break;
    }

#if VERBOSE
    //cout << "FPS = " << 1000./GET_AVERAGE_TIMING(frameTimer) << "\n";
#endif
    
#if RECORD
    frames.push_back(*(new Mat));
    frame.copyTo(frames[frames.size()-1]);
#endif
  }
  STOP_TIMING(frameTimer);
 
 
#if RECORD
  // Save frames before exiting
  for (int i = 0; i < frames.size(); i++) {
    record << (frames[i]);
  }
#endif

  cout << "FPS = " << (count*1000.)/GET_AVERAGE_TIMING(frameTimer) << "\n";
  
  // Close camera cleanly
  cam.release();

  return 0;
}

