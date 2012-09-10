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
ParticleFilter::ParticleFilter(unsigned int n, vector<Range> bounds,
                               TransitionModel transition_model,
                               EmissionModel emission_model)
    : num_particles_(n),
      bounds_(bounds),
      transition_model_(transition_model)
      emission_model_(emission_model) {
  particles_ = Mat::zeros(num_particles_, 1, DataType<short>::type);
  weights_ = Mat::ones(num_particles_, 1, DataType<double>::type); 
  InitializeUniformly();
}

ParticleFilter::ParticleFilter(Mat initial_particles,
                               vector<Range> bounds,
                               TransitionModel transition_model,
                               EmissionModel emission_model)
    : num_particles_(initial_particles.rows)
      bounds_(bounds)
      transition_model_(transition_model)
      emission_model_(emission_model) {
  weights_ = Mat::ones(num_particles_, 1, DataType<double>::type);
  Normalize();
}

void ParticleFilter::InitializeUniformly() {
  // Initialize each dimension uniform randomly within its bounds
  for(int i = 0; i < bounds_.size(); i++) {
    randu(particles_.col(i), Scalar(bounds_[i].start), Scalar(bounds_[i].end));
  }
  Normalize();
}

// TODO: better emission model
void ParticleFilter::Observe(Mat frame) {
  
  // Weight ~ inverse quadratic color distance
  for(int i = 0, double w, Mat p; i < num_particles_; i++) {
    p = particles_.row(i);
    weights_.at(i) = 1/(1 + pow(norm(frame.at(p[0], p[1]), emission_model_), 2));
  }

  // Normalize
  Normalize();
                                         
  // Resample if particles degenerate
  // TODO: better resampling condition
  if(1/pow(norm(weights_), 2) < num_particles_/2) {
    Resample();
  }
}

// TODO: better transition model
void ParticleFilter::ElapseTime() {
  Mat r(particles_.col(0).size(), particles_.type());
  for(int i = 0; i < bounds_.size(); i++) {
    randu(r, Scalar(bounds_[i].start), Scalar(bounds_[i].end));
    particles_.col(i) += r;
  }
  // TODO: filter out-of-bounds values
}

State ParticleFilter::Estimate() {
  return new State((particles_.t()*weights_).t())
}

void ParticleFilter::Resample() {
  // Stochastic Universal Sampling (Baker, 1987)
  return;
}

void ParticleFilter::Normalize() {
  normalize(weights_, weights_, 1, 0, NORM_L1);
}