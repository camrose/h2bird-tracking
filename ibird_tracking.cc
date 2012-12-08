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
#define RECORD                      1 // Record video
#define VERBOSE                     1 // Lots of printing

/* Camera driver config values */
#define CAM                 (0)
#define CAM_WIDTH           (640)
#define CAM_HEIGHT          (480)
#define CAM_BRIGHTNESS      (0.4)
#define CAM_CONTRAST        (0.11)
#define CAM_SATURATION      (0.11)
#define CAM_GAIN            (1.0)

/* Particle filter parameters */
#define NUM_PARTICLES       (2000)
#define STEP_SIZE           TransitionModel(8)
#define TARGET_COLOR        EmissionModel(72, 235, 211) // Tennis ball

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
Mat frame;
char filename[200];
vector<Mat> frames;
int drag = 0;
Point top_point = Point(0,0);
Point bottom_point = Point(0,0);
Point target = Point(0,0);

void mouseHandler(int event, int x, int y, int flags, void* param) {
  if (event == CV_EVENT_LBUTTONDOWN && !drag) {
    top_point.x = x;
    top_point.y = y;
    bottom_point.x = x;
    bottom_point.y = y;
    target.x = 0;
    target.y = 0;
    drag = 1;
  }
  if (event == CV_EVENT_MOUSEMOVE && drag) {
    bottom_point.x = x;
    bottom_point.y = y;
  }
  if (event == CV_EVENT_LBUTTONUP) {
    if (top_point.x > bottom_point.x) {
      target.x = (int)(top_point.x - abs(bottom_point.x-top_point.x)/2);
    } else {
      target.x = (int)(top_point.x + abs(bottom_point.x-top_point.x)/2);
    }
    if (top_point.y > bottom_point.y) {
      target.y = (int)(top_point.y - abs(bottom_point.y-top_point.y)/2);
    } else {
      target.y = (int)(top_point.y + abs(bottom_point.y-top_point.y)/2);
    }
    drag = 0;
  }
  if (event == CV_EVENT_RBUTTONUP) {
    imshow("First frame", frame);
  }
}

int main( int argc, char** argv ) {

  Size frame_size;    
  double cam_brightness, cam_contrast, cam_saturation, cam_gain;
  
  Point ibird;

  // Open and configure camera
  VideoCapture cam(CAM);
  if( !cam.isOpened() ) {
    cout << "Camera open failed." << endl;
    return -1;
  }

  // Set camera parameters
  //cam.set(CV_CAP_PROP_BRIGHTNESS, CAM_BRIGHTNESS);
  //cam.set(CV_CAP_PROP_CONTRAST, CAM_CONTRAST);
  //cam.set(CV_CAP_PROP_SATURATION, CAM_SATURATION);  // not supported on PS Eye
  //cam.set(CV_CAP_PROP_GAIN, CAM_GAIN);
  cam.set(CV_CAP_PROP_FRAME_WIDTH, CAM_WIDTH);
  cam.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
  //cam.set(CV_CAP_PROP_FPS, 125);                     // not supported on PS Eye
  //cam.set(CV_CAP_PROP_FORMAT, 0);

  // Get camera parameters to make sure they were set correctly
  frame_size = Size(cam.get(CV_CAP_PROP_FRAME_WIDTH), cam.get(CV_CAP_PROP_FRAME_HEIGHT));
  //cam_brightness = cam.get(CV_CAP_PROP_BRIGHTNESS);
  //cam_contrast = cam.get(CV_CAP_PROP_CONTRAST);
  //cam_saturation = cam.get(CV_CAP_PROP_SATURATION);
  //cam_gain = cam.get(CV_CAP_PROP_GAIN);

#if VERBOSE
  printf("Opened %u by %u camera stream.\n", frame_size.width, frame_size.height);
  //printf("Brightness: %f\n", cam_brightness);
  //printf("Contrast: %f\n", cam_contrast);
  //printf("Saturation: %f\n", cam_saturation);
  //printf("Gain: %f\n", cam_gain);
#endif

  // Open video recording
#if RECORD
  record = VideoWriter("video/output.mjpg", CV_FOURCC('M','J','P','G'), 30, frame_size, true);
  if( !record.isOpened() ) {
    printf("Recording failed to open!\n");
    return -1;
  }
#endif

  // Setup particle filter
  static const Range b[] = { Range(90, frame_size.height-1), 
                             Range(0, frame_size.width-1) };
  vector<Range> bounds(b, b + sizeof(b) / sizeof(b[0]));
  //Vec3b color = Vec3b(107, 166, 165); // Tennis ball
  //Vec3b color = Vec3b(123, 93, 189); // Pink tail
  //Vec3b color = Vec3b(66, 48, 140); // Pink tail shadows
  //Vec3b color = Vec3b(66, 81, 49); // Green tail
  // Tennis ball
  // H: 85-95
  // S: 63-136
  // V: 133-232
  //Vec3b color = Vec3b(90, 100, 0);
  
  // Draw small square near (0,0) on first frame
  cam >> frame;
  //rectangle(frame, Point(10,10), Point(11.5,11.5), Scalar(128, 128, 128), -1);
  
  // Show first frame
  imshow("First frame",frame);
  setMouseCallback( "First frame", mouseHandler, 0 );
  
  // Setup particle filter
  TransitionModel transition_model = {0, 16, 0.1};
  ParticleFilter pf(NUM_PARTICLES, bounds, transition_model, frame.clone());
  
  DECLARE_TIMING(frameTimer);
  int count = 0;
  START_TIMING(frameTimer);
  while(1) {
    cam >> frame;                     // Capture a new frame
    count++;
    pf.Observe(frame);                // Process frame
    pf.ElapseTime();                  // Transition particles
#if DISPLAY || RECORD
    ibird = pf.Draw(frame);  // Draw particles on frame
    if (top_point.x != bottom_point.x && top_point.y != bottom_point.y) {
      rectangle(frame, top_point, bottom_point, CV_RGB(255, 0, 0), 1, 8, 0);
      if (target.x != 0 && target.y != 0) {
         ellipse( frame, target, Size(10,10), 
            0, 0, 360, Scalar(0,255,0), CV_FILLED, 8, 0);
      }
    }
#else
    ibird = pf.Estimate();
#endif

    if (target.x != 0) {
      printf("#%d,%d,%d,%d\n",ibird.x, ibird.y, target.x, target.y);
      cout.flush();
    }

#if DISPLAY
    imshow("Particle filter", frame); // Show PF state
#endif

    //cout.flush();

    char key = waitKey(5);
    if(key == 0x100000 + 'q' || key == 'q') {
        break;
    }
    
    //printf("%x\n", waitKey(5));

#if VERBOSE
    //cout << "FPS = " << 1000./GET_AVERAGE_TIMING(frameTimer) << "\n";
#endif
    
#if RECORD
    frames.push_back(*(new Mat));
    frame.copyTo(frames[frames.size()-1]);
#endif
  }
  STOP_TIMING(frameTimer);
  cout << "FPS = " << (count*1000.)/GET_TIMING(frameTimer) << "\n";
  cout.flush();
 
#if RECORD
  // Save frames before exiting
  for (int i = 0; i < frames.size(); i++) {
    record << (frames[i]);
  }
#endif
  
  // Close camera cleanly
  cam.release();

  return 0;
}

