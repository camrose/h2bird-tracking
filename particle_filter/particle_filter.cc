/*
 * particle_filter.cpp
 *
 * Particle filter for color-based object tracking in OpenCV
 *
 * Based loosely on the BallBot particle filter
 * https://github.com/preetnum/ballbot
 *
 *  Created on: Sep 08, 2012
 *      Author: Ryan Julian
 *  
 */
/* C Standard Library */
#include <stdio.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* OpenCV */
#include <opencv2/opencv.hpp>

/* Project */
#include "particle_filter.h"

using namespace cv;
using namespace std;

/* Globals */
cv::RNG random(time(NULL));

/* ParticleFilter */
ParticleFilter::ParticleFilter(Mat initial_particles) {
  Initialize(initial_particles);
}

void ParticleFilter::Initialize(Mat initial_particles) {
  num_particles_ = initial_particles.rows;
  particles = initial_particles;
  normalize();
}

void ParticleFilter::InitializeUniformly(unsigned int n, vector<Point> bounds) {
  // Initialize each dimension with (possibly) different bounds
  for(int i = 0; i < bounds.size(); i++) {
    randu(particles_[i], Scalar(bounds[i].min), Scalar(bounds[i].max));
  }
  //randu(particles_, Scalar(
}
