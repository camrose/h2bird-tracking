def pf_display(im, p):
    pos, xs, ws = p
    position_overlay = zeros_like(im)
    position_overlay[tuple(x0)] = 1
    particle_overlay = zeros_like(im)
    particle_overlay[tuple(xs.T)] = 1
    hold(True)
    draw()
    time.sleep(0.3)
    clf()                                           # Causes flickering, but without the spy plots aren't overwritten
    imshow(im,cmap=cm.gray)                         # Plot the image
    spy(position_overlay, marker='.', color='b')    # Plot the expected position
    spy(particle_overlay, marker=',', color='r')    # Plot the particles

if __name__ == "__main__":
    from pylab import *
    from itertools import izip
    from particle_filter.particle_filter import *
    from numpy.random import *
    import time
    
    # matplotlib interactive mode
    ion()
    
    # some parameters
    N_FRAMES = 200
    N_PARTICLES = 1000
    REPEAT = 10000
    H_SIZE = 640
    V_SIZE = 480
    
    # Create an image sequence of 20 frames long
    seq = [im for im in zeros((N_FRAMES,V_SIZE,H_SIZE), int)]      
    
    # Add a square with starting position x0 moving along trajectory xs
    x0 = array([120, 160])                              
    xs = vstack((arange(N_FRAMES)*3, arange(N_FRAMES)*2)).T + x0
    for t, x in enumerate(xs):
        xslice = slice(x[0]-8, x[0]+8)
        yslice = slice(x[1]-8, x[1]+8)
        seq[t][xslice, yslice] = 255
        
    # Original
#    p_init = randint(0, V_SIZE, (N_PARTICLES, 2))
#    color_model = iter(seq).next()[tuple(x0)]
#    start = time.time()
#    for i in range(REPEAT):
#        result = particlefilter(seq, p_init, 8, color_model, N_PARTICLES)
#    end = time.time()
#    print "FPS: ",N_FRAMES*REPEAT/(end-start)
#    for im, p in izip(seq, result): # Track the square through the sequence
#        pf_display(im,p)
#        pass

#    # Initialize particle filter
    p_init = randint(0, V_SIZE, (N_PARTICLES, 2))
    color_model = iter(seq).next()[tuple(x0)]
    pf = ParticleFilter(p_init, 8, color_model, seq[0].shape, N_PARTICLES)
    
    # Run particle filter
    
#        seq = iter(sequence)
#    x = ones((n, 2), int) * pos                     # Initial position
#    f0 = color_model * ones(n)                      # Target colour model
#    yield pos, x, ones(n)/n                         # Return expected position, particles and weights
#    for im in seq:
#        x += uniform(-stepsize, stepsize, x.shape)  # Particle motion model: uniform step
#        x  = x.clip(zeros(2), array(im.shape)-1).astype(int) # Clip out-of-bounds particles
#        f  = im[tuple(x.T)]                         # Measure particle colours
#        w  = 1./(1. + (f0-f)**2)                    # Weight~ inverse quadratic colour distance
#        w /= sum(w)                                 # Normalize w
#        yield sum(x.T*w, axis=1), x, w              # Return expected position, particles and weights
#        if 1./sum(w**2) < n/2.:                     # If particle cloud degenerate:
#            x  = x[resample(w),:]                   # Resample particles according to weightsfrom numpy import *
    sequence = iter(seq)
    #num_particles = N_PARTICLES
    #particles = ones((num_particles,2), int) * p_init
    #f0 = color_model * ones(num_particles)
    #pos = p_init
    #w = ones(num_particles)/num_particles
    #ss = 8
    #bounds = seq[0].shape
    start = time.time()
    for im in sequence:
        #particles += uniform(-ss, ss, particles.shape)                      # Uniform step motion model
        #particles = particles.clip(zeros(2), array(bounds)-1).astype(int)   # Clip out-of-bounds particles
        #f = im[tuple(particles.T)]                                          # Measure particle colors
        #w = 1./(1. + (f0-f)**2)                                             # Weight ~ inverse quadratic color distance
        #w /= sum(w)                                                         # Normalize w
        #if 1./sum(w**2) < num_particles/2.:                                 # Resample if particles degenerate
            #particles = particles[resample(w),:]
        pf.elapse_time()
        pf.observe(im)
        pf_display(im, pf.get_deprecated())
    end = time.time()
    print "FPS: ",N_FRAMES/(end-start)
    
    #show()
    
