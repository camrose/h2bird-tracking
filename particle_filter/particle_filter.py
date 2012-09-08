# Blob-Tracking Particle Filter
# Author: Ryan C. Julian
#
# Based loosely on the BallBot particle filter and SciPy particle filter
# https://github.com/radicalnerd/ballbot/
# http://www.scipy.org/Cookbook/ParticleFilter

from numpy import *
from numpy.random import *

class ParticleFilter:
    # TODO: Generic transition and emission models

    def __init__(self, pos, stepsize, color_model, bounds, num_particles=1000):
        self.num_particles = num_particles
        self.particles = ones((num_particles,2), int) * pos     # Initial position
        self.f0 = color_model * ones((num_particles, len(color_model)))
        self.pos = pos
        self.w = ones(num_particles)/num_particles
        self.ss = stepsize
        self.bounds = bounds
        
    def observe(self, observation):
        # TODO: better emission model
        f = observation[tuple(self.particles.T)]        # Measure particle colors
        self.w = 1./(1. + sum((self.f0-f)**2, axis=1))  # Weight ~ inverse quadratic color distance
        self.w /= sum(self.w)                           # Normalize w
        # TODO: better resampling condition
        if 1./sum(self.w**2) < self.num_particles/2.:   # Resample if particles degenerate
            self._resample()
    
    def elapse_time(self):
        # TODO: better transition model
        self.particles += uniform(-self.ss, self.ss, self.particles.shape)                  # Uniform step motion model
        self.particles = self.particles.clip(zeros(2), array(self.bounds)-1).astype(int)    # Clip out-of-bounds particles
        
    def _resample(self):
        indices = []
        C = [0.] + [sum(self.w[:i+1]) for i in range(self.num_particles)]
        u0, j = random(), 0
        for u in [(u0+i)/self.num_particles for i in range(self.num_particles)]:
            while u > C[j]:
                j+=1
            indices.append(j-1)
        self.particles = self.particles[indices,:]        
        
    def get_beliefs(self):
        return self.particles
        
    def get_estimate(self):
        return sum(self.particles.T*self.w, axis=1).astype(uint)
        
    def get_weights(self):
        return self.w
        
    def get_state(self):
        return self.get_estimate(), self.get_beliefs(), self.get_weights()
    
    
    
    
