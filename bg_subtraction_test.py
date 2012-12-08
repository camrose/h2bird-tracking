import time

from pylab import *
from numpy.random import *
import cv2
import cv

if __name__ == "__main__":

    # Parameters
    CAM = 0
    H_SIZE = 640
    V_SIZE = 480
    DISP_H = 800
    DISP_V = 600

    # Open camera
    cap = cv2.VideoCapture(CAM)
    cap.set(cv.CV_CAP_PROP_FRAME_WIDTH, 640)
    cap.set(cv.CV_CAP_PROP_FRAME_HEIGHT, 480)
    retval, first_frame = cap.read()

    # Show emission model results
    while True:
        retval, frame = cap.read()
        
        em_frame = zeros(frame.shape[:-1])
        
        for i in range(frame.shape[0]):
        #for i in range(1):
            for j in range(frame.shape[1]):
            #for j in range(1):
                em_frame[i,j] = (1 + sum((first_frame[i,j].astype(float)-frame[i,j].astype(float))**2))/195076
                
        cv2.imshow("Emission model", em_frame)
        cv2.imshow("Frame", frame)
        
        if cv2.waitKey(5) > 0:
            break;

