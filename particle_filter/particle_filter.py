from numpy import *
from numpy.random import *

# SciPy Particle Filter Example
# http://www.scipy.org/Cookbook/ParticleFilter
def resample(weights):
    n = len(weights)
    indices = []
    C = [0.] + [sum(weights[:i+1]) for i in range(n)]
    u0, j = random(), 0
    for u in [(u0+i)/n for i in range(n)]:
        while u > C[j]:
            j+=1
        indices.append(j-1)
    return indices


def particlefilter(sequence, pos, stepsize, color_model, n):
    seq = iter(sequence)
    x = ones((n, 2), int) * pos                     # Initial position
    f0 = color_model * ones(n)                      # Target colour model
    yield pos, x, ones(n)/n                         # Return expected position, particles and weights
    for im in seq:
        x += uniform(-stepsize, stepsize, x.shape)  # Particle motion model: uniform step
        x  = x.clip(zeros(2), array(im.shape)-1).astype(int) # Clip out-of-bounds particles
        f  = im[tuple(x.T)]                         # Measure particle colours
        w  = 1./(1. + (f0-f)**2)                    # Weight~ inverse quadratic colour distance
        w /= sum(w)                                 # Normalize w
        yield sum(x.T*w, axis=1), x, w              # Return expected position, particles and weights
        if 1./sum(w**2) < n/2.:                     # If particle cloud degenerate:
            x  = x[resample(w),:]                   # Resample particles according to weightsfrom numpy import *
################################################################################

# Based loosely on the BallBot particle filter
# https://github.com/radicalnerd/ballbot/
class ParticleFilter:
    # TODO: Generic transition and emission models

    def __init__(self, pos, stepsize, color_model, bounds, num_particles=1000):
        self.num_particles = num_particles
        self.particles = ones((num_particles,2), int) * pos     # Initial position
        self.f0 = color_model * ones(num_particles)
        self.pos = pos
        self.w = ones(num_particles)/num_particles
        self.ss = stepsize
        self.bounds = bounds
        
    def observe(self, observation):
        f = observation[tuple(self.particles.T)]         # Measure particle colors
        self.w = 1./(1. + (self.f0-f)**2)               # Weight ~ inverse quadratic color distance
        self.w /= sum(self.w)                           # Normalize w
        if 1./sum(self.w**2) < self.num_particles/2.:   # Resample if particles degenerate
            self._resample()
    
    def elapse_time(self):
        pass
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
        return sum(self.particles.T*self.w, axis=1)
        
    def get_weights(self):
        return self.w
        
    def get_deprecated(self):
        return self.get_estimate(), self.get_beliefs(), self.get_weights()
    
    
    
    
