/*
 * particle_filter.hpp
 *
 * Particle filter for color-based object tracking in OpenCV
 *
 * Based loosely on the BallBot particle filter.
 * https://github.com/preetnum/ballbot
 *
 *  Created on: Sep 08, 2012
 *      Author: Ryan Julian
 *  
 */
#ifndef PARTICLE_FILTER_H_
#define PARTICLE_FILTER_H_

/* CPP Standard Library */
#include <iostream>
#include <vector>

/* OpenCV */
#include <opencv2/opencv.hpp>

class ParticleFilter {
 public:
  /* Constructors */
  ParticleFilter();
  ParticleFilter(cv::Mat initial_particles);
  
  /* Initialization */
  void Initialize(cv::Mat particles);
  void InitializeUniformly(unsigned int n, std::vector<cv::Point> bounds);
  
  /* Observe an emission and reweight particles accordingly */
  void Observe(cv::Mat frame)
  
  /* Transition the particles */
  void ElapseTime();
  
  /* Resampling and normalization */
  void Resample();
  void Normalize();
  
  /* State and accessors */
  cv::Point2d Estimate();
  unsigned int num_particles() { return num_particles_; }
  cv::Mat particles() { return particles_; }
  cv::Mat weights() { return weights_; }
 
 private:
  unsigned int num_particles_;
  cv::Mat particles_;
  cv::Mat weights_;
}
#endif
