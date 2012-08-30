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
    H_SIZE = 640
    V_SIZE = 480
    
    # Create an image sequence of 20 frames long
    seq = [ im for im in zeros((N_FRAMES,V_SIZE,H_SIZE), int)]      
    
    # Add a square with starting position x0 moving along trajectory xs
    x0 = array([120, 160])                              
    xs = vstack((arange(N_FRAMES)*3, arange(N_FRAMES)*2)).T + x0
    for t, x in enumerate(xs):
        xslice = slice(x[0]-8, x[0]+8)
        yslice = slice(x[1]-8, x[1]+8)
        seq[t][xslice, yslice] = 255

    # Run particle filter 
    color_model = iter(seq).next()[tuple(x0)]
    p_init = randint(0, V_SIZE, (N_PARTICLES, 2))
    start = time.time()
    result = particlefilter(seq, p_init, 8, color_model, N_PARTICLES)
    end = time.time()
    print "FPS: ",N_FRAMES*REPEAT/(end-start)
    
    # Render result
    for im, p in izip(seq, result):                     # Track the square through the sequence
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
    show()
