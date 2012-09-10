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

typedef cv::Mat State
typedef cv::Vec3b EmissionModel
typedef unsigned int TransitionModel

double ColorDistance(Color c, Color c0);

class ParticleFilter {
 public:
  /**
   * Constructors
   */
  ParticleFilter(unsigned int n, std::vector<Range> bounds,
                 TransitionModel transition_model,
                 EmissionModel emission_model);
  ParticleFilter(cv::Mat initial_particles, std::vector<Range> bounds,
                 TransitionModel transition_model,
                 EmissionModel emission_model);
  
  /**
   * Initialization
   */
  void InitializeUniformly();
  
  /**
   * Observe a frame and reweight particles according the emission
   * model
   */
  void Observe(cv::Mat frame)
  
  /**
   * Elapse time according to the transition model
   */
  void ElapseTime();
  
  /**
   * Estimate state
   */
  State Estimate();

  /**
   * Accessors
   */
  unsigned int num_particles() { return num_particles_; }
  std::vector<Range> bounds() { return bounds_; }
  cv::Mat particles() { return particles_; }
  cv::Mat weights() { return weights_; }
 
 private:
  unsigned int num_particles_;
  vector<Range> bounds_;
  TransitionModel transition_model_;
  EmissionModel emission_model_;
  cv::Mat particles_;
  cv::Mat weights_;

  /**
   * Resampling
   */
  void Resample();

  /**
   * Normalization
   */
  void Normalize();
}
#endif
