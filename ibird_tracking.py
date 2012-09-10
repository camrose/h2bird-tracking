from itertools import izip
import time

from pylab import *
from numpy.random import *
import cv2

from particle_filter.particle_filter import *

def pf_display(frame, p):
    pos, xs, ws = p
    cv2.circle(frame, (pos[1], pos[0]), 10, (255, 0, 0))    # Draw position estimate
    frame[tuple(xs.T)] = (0, 0, 255)                        # Draw particles
    cv2.imshow("Particle Filter", frame)                    # Show frame

def pf_test(n_frames=200, n_particles=1000, h_size=640, v_size=480, display=False):

    # matplotlib interactive mode
    ion()
    
    # Create an image sequence of 20 frames long
    seq = [im for im in zeros((N_FRAMES,V_SIZE,H_SIZE,3), int)]      
    
    # Add a square with starting position x0 moving along trajectory xs
    x0 = array([120, 160])                              
    xs = vstack((arange(N_FRAMES)*3, arange(N_FRAMES)*2)).T + x0
    for t, x in enumerate(xs):
        xslice = slice(x[0]-8, x[0]+8)
        yslice = slice(x[1]-8, x[1]+8)
        seq[t][xslice, yslice] = 255
        
    # Initialize particle filter
    p_init = randint(0, V_SIZE, (N_PARTICLES, 2))
    color_model = iter(seq).next()[tuple(x0)]
    pf = ParticleFilter(p_init, 8, color_model, seq[0].shape, N_PARTICLES)

    # Run particle filter
    sequence = iter(seq)
    start = time.time()
    for im in sequence:
        pf.elapse_time()
        pf.observe(im)
        if display:
            pf_display(im, pf.get_state())
    end = time.time()
    print "FPS: ",N_FRAMES/(end-start)

if __name__ == "__main__":

    # Parameters
    N_PARTICLES = 1000
    CAM = 1
    H_SIZE = 640
    V_SIZE = 480
    DISPLAY = True

    # Color Model
    color_model = (88, 129, 220)

    # Open camera
    cap = cv2.VideoCapture(CAM)
    retval, frame = cap.read()

    # Initialize particle filter
    p_init = randint(0, V_SIZE, (N_PARTICLES, 2))
    pf = ParticleFilter(p_init, 16, color_model, frame.shape[:-1], N_PARTICLES)

    # Run particle filter
    times = 30*ones(10)
    while True:
        start = time.time()     # start timer
        
        # Capture
        retval, frame = cap.read()

        # Process
        pf.elapse_time()
        pf.observe(frame)

        end = time.time()       # stop timer

        # Display
        times = append(times[1:end], end-start)
        print "FPS: ",1/mean(times)
        if DISPLAY:
            pf_display(frame, pf.get_state())
        
        if cv2.waitKey(5) == ord('q'):
            break;

