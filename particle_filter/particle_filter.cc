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
#include <math.h>

/* OpenCV */
#include <opencv2/opencv.hpp>

/* Project */
#include "particle_filter.h"
#include <typeinfo>

using namespace cv;
using namespace std;

/* Globals */
cv::RNG rng(time(NULL));

/* ParticleFilter */
ParticleFilter::ParticleFilter(unsigned int n) { }
ParticleFilter::ParticleFilter(unsigned int n, 
                               vector<Range> bounds,
                               TransitionModel transition_model,
                               EmissionModel emission_model)
    : num_particles_(n),
      bounds_(bounds),
      transition_model_(transition_model),
      emission_model_(emission_model)
{
  particles_ = Mat::zeros(num_particles_, bounds_.size(), DataType<short>::type);
  weights_ = Mat::ones(num_particles_, 1, DataType<double>::type);
  InitializeUniformly();
}

ParticleFilter::ParticleFilter(Mat initial_particles,
                               vector<Range> bounds,
                               TransitionModel transition_model,
                               EmissionModel emission_model)
    : num_particles_(initial_particles.rows),
      bounds_(bounds),
      transition_model_(transition_model),
      emission_model_(emission_model)
{
  weights_ = Mat::ones(num_particles_, 1, DataType<double>::type);
  Normalize();
}

void ParticleFilter::InitializeUniformly() {
  // Initialize each dimension uniform randomly within its bounds
  Mat col;
  for(unsigned int i = 0; i < bounds_.size(); i++) {
    col = particles_.col(i);
    randu(col, Scalar(bounds_[i].start), Scalar(bounds_[i].end));
  }
  Normalize();
}

// TODO: better emission model
void ParticleFilter::Observe(Mat frame) {
    
  // Weight ~ inverse quadratic color distance
  Mat_<short> p;
  Vec3b color;
  for(unsigned int i = 0; i < num_particles_; i++) {
    p = particles_.row(i);
    color = frame.at<Vec3b>(p(0), p(1));
    weights_.at<double>(i) = 1/(1 + pow(norm(color, emission_model_), 2));
    // TODO: calculate cumsum of weights, to speed resampling
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
  Mat col;
  for(unsigned int i = 0; i < bounds_.size(); i++) {
    col = particles_.col(i);
    randu(r, -transition_model_, transition_model_);
    col += r;
    // Threshold particles outside the bounds
    min(col, bounds_[i].end, col);
    max(col, bounds_[i].start, col);
  }
}

State ParticleFilter::Estimate() {
  Mat float_particles;
  particles_.convertTo(float_particles, weights_.type());
  Mat_<double> p = float_particles.t()*weights_;
  return State(p(0), p(1));
}

void ParticleFilter::Resample() {
  // Stochastic Universal Sampling
  // J.E. Baker. "Reducing bias an inefficiency in selection algorithms.", 1987
  Mat sum_weights, new_row;
  integral(weights_, sum_weights);                              // Find the cumulative sum of the weights
  double stride = 1/num_particles_;                             // Walk the array in equal strides of total weight
  
  int old_idx = 0;                                              // Index in old population
  int new_idx = 0;                                              // Index in new population
  for(double marker = stride * rng.uniform(0., 1.);             // Start the walk at a random point inside the first stride
      marker < num_particles_; marker += stride) {              // Walk in equal strides thereafter until reaching the end
    while(marker > sum_weights.at<double>(old_idx)) {           // Find the particle the marker points to
      old_idx++;
    }
    new_row = particles_.row(new_idx);
    particles_.row(old_idx-1).copyTo(new_row);  // Add the particle under the marker to the new population
    new_idx++;
  }
}

void ParticleFilter::Normalize() {
  normalize(weights_, weights_, 1, 0, NORM_L1);
}

void ParticleFilter::Draw(Mat frame) {
  circle(frame, Estimate(), 10, Scalar(255, 0, 0), 2); // Draw position estimate
  Mat_<short> p;
  for(unsigned int i = 0; i < num_particles_; i++) {      // Draw particles
    p = particles_.row(i);
    frame.at<Vec3b>(p(0),p(1))[0] = 0;
    frame.at<Vec3b>(p(0),p(1))[1] = 0;
    frame.at<Vec3b>(p(0),p(1))[2] = 255;
  }
}
