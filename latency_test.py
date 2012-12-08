import time

from pylab import *
from numpy.random import *
import cv2
import cv

if __name__ == "__main__":

    # Parameters
    CAM = 1
    H_SIZE = 320
    V_SIZE = 240
    DISP_H = 800
    DISP_V = 600
    THRESHOLD = 200
    N_SAMPLES = 100
    WINDOW = 30

    # Open camera
    cap = cv2.VideoCapture(CAM)
    cap.set(cv.CV_CAP_PROP_FRAME_WIDTH, H_SIZE)
    cap.set(cv.CV_CAP_PROP_FRAME_HEIGHT, V_SIZE)
    retval, frame = cap.read()
    
    # Open target window
    target = zeros((DISP_H, DISP_V, 3), uint8)
    cv2.imshow("target", target)
    cv2.waitKey(1)
    
    # Allow user to adjust camera
    while True:
        retval, frame = cap.read()
        cv2.imshow("camera", frame)
        if cv2.waitKey(1) > 0:
            break;
    cv2.destroyWindow("camera")     # close camera preview window
    cv2.waitKey(1)

    # Run latency test
    times = ones(WINDOW)
    samples = randint(0, V_SIZE, (N_SAMPLES, 2))
    while True:
        retval, frame = cap.read()                          # get original camera frame
        c0 = frame[tuple(samples.T)]                        # sample starting colors
        
        target = ~target                                    # invert display
        cv2.imshow("target", target)
        cv2.waitKey(1)
        start = time.time()                                 # start timer
        
        color_distance = 0;
        while color_distance < THRESHOLD:
            retval, frame = cap.read()
            c = frame[tuple(samples.T)]                     # sample current colors
            color_distance = sum(sum((c0-c)**2, axis=1))/N_SAMPLES
            #print color_distance
            
        end = time.time()                                   # stop timer
        times = append(times[1:end], end-start)             # update moving average
        print "Latency: %f" % (sum(times)/times.size)       # print average latency
        
        if cv2.waitKey(5) > 0:
            break;

