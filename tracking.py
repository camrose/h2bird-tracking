#!/usr/bin/python

import sys, subprocess, time, tempfile, threading, tty, datetime
from pythonlib.lib.command_interface import CommandInterface
from pythonlib.keyboardWireLinux import KeyboardInterface
from pythonlib.lib.xbee_manager import XBeeManager
from pythonlib.lib.keyboard_monitor import KeyboardMonitor
from xbee import XBee
from serial import *
from struct import *
from Queue import *
from pythonlib.lib.quaternion import *
from pylab import *
import numpy as np


# In future, add "from pythonlib.lib.network_coordinator import NetworkCoordinator"
# and discover/fetch client addresses
com = '/dev/ttyUSB0'
baud = 57600
DEFAULT_ADDR = 0x1020
DEFAULT_PAN = 0x1005
addr = '\x10\x21'
DEFAULT_CHAN = 0x12
xb = None

if __name__ == '__main__':
  
    framerate = 0.0
    pixel_pos = []
    row = 0
    tracking_flag = 0
    found = 0
    
    print "Beginning initialization..."

    # Initialize xbee radio 
    print "Initializing XBee.."
    xb = XBeeManager(com = '/dev/ttyUSB0', baud = 57600)  
    if not xb.isInitialized():
        print "XBee initialization failed."
        sys.exit(-1)
    xb.setAddress(DEFAULT_ADDR, DEFAULT_PAN, DEFAULT_CHAN)
    xb.start()

    # Initialize keyboard getter
    print "Initializing keyboard monitor..."
    kbmon = KeyboardMonitor()
    if not kbmon.isInitialized():
        print "Keyboard monitor initialization failed."
        sys.exit(-1)
    kbmon.start()

    # Initialize communication modules
    print "Initializing communication modules..."
    comm = CommandInterface(addr, xb.putTxPacket)
    comm.enableDebug()
    comm.setSlewLimit(0.0)
    kbint = KeyboardInterface(comm)
    
    # Pipe the output from the process so we can read out the bird location
    #tracking = subprocess.Popen(args = './tennis_ballbot', shell = False, stdout = subprocess.PIPE)

    # I-Bird preflight initialization
    line_open = 0
    try:
        process = 1
        start_time = datetime.datetime.now()
        yaw_offset = 0.0
        yaw_acc = np.zeros(7)
        prev_yaw = -100.0
        yaw_diff = 0.0
        Kp = 3.3
        Kd = 10.0
        Ki = 0.001
        while process:

            if kbmon.hasKey():
                c = kbmon.getCh()
                if c == 'q':
                    process = 0
                    break
                if c == 'm':
                    tracking = subprocess.Popen(args = ['nice', '-n', str(-10), './bin/ibird_tracking'], shell = False, stdout = subprocess.PIPE, bufsize = 0)
                    line_open = 1;              
                kbint.process(c)
            if line_open == 1:
                line = tracking.stdout.readline()
            else:
                line = ''
            if line != '' and line[0] == '#':
                components = line[1:-1].split(',')
            #    print components
                x = int(components[0])
                y = int(components[1])
                wx = int(components[2])
                wy = int(components[3])
                
#                if y > 440 or tracking_flag == 1:
#                    if y > 440 and found == 0:
#                        comm.setRegulatorRef( eulerToQuaternionDeg( 90.0, 80.0, 0.0 ) )
#                        comm.setRegulatorOffsets((0.0, 0.0, 0.85))
#                    elif y < wy:
#                        comm.setRegulatorRef( eulerToQuaternionDeg( yaw_offset, 80.0, 0.0 ) )
#                        comm.setRegulatorOffsets((0.0, 0.0, 0.85))
#                        found = 1
#                    elif y >= wy:
#                        comm.setRegulatorRef( eulerToQuaternionDeg( yaw_offset, 80.0, 0.0 ) )
#                        comm.setRegulatorOffsets((0.0, 0.0, 0.9))
#                        found = 1
#                    tracking_flag = 1
                    
                if y < wy:
                    comm.setRegulatorRef( eulerToQuaternionDeg( yaw_offset, 80.0, 0.0 ) )
                    comm.setRegulatorOffsets((0.0, 0.0, 0.85))
                elif y >= wy:
                    comm.setRegulatorRef( eulerToQuaternionDeg( yaw_offset, 80.0, 0.0 ) )
                    comm.setRegulatorOffsets((0.0, 0.0, 0.9))
                
                yaw_error_pixel = wx - x
                yaw_error_rad = (75.0/640.0)*yaw_error_pixel
                if prev_yaw != -100.0:
                    yaw_diff = yaw_error_rad - prev_yaw
                else:
                    yaw_diff = 0.0
                sum_yaw = sum(yaw_acc)
                yaw_offset = Kp*yaw_error_rad + Kd*yaw_diff + Ki*sum_yaw #
                if yaw_offset > 90.0:
                    yaw_offset = 90.0
                elif yaw_offset < -90.0:
                    yaw_offset = -90.0
                prev_yaw = yaw_error_rad
                yaw_acc = append(yaw_acc[1:], yaw_error_rad)
                # print str(x) + "," + str(y) + "," + str(wx) + "," + str(wy)
                end_time = datetime.datetime.now()
                round_time = end_time - start_time
                dt = round_time.seconds/1.0 + round_time.microseconds/1000000.0
                pixel_pos.append([dt,x,y,wx,wy,yaw_error_pixel,yaw_error_rad,yaw_offset])

            if line != '' and line[0] == 'F':
              framerate = float(line[6:12])
            #time.sleep(0.06)

    except Exception as e:
        print e

    finally:
        print str(framerate)
        today = datetime.datetime.today()
        d = str(today.year) + "_" + str(today.month) + "_" + str(today.day)
        t = str(today.hour) + "_" + str(today.minute) + "_" + str(today.second)
        fname = './data/TrackingOutput-' + d + '-' + t + '.txt'
        record_log = open(fname, 'w')
        record_log.write("Time\tBird X\tBird Y\tWindow X\tWindow Y\tPixel" +
                         "Error\tYaw Rad Error\tYaw Offset\tFramerate: " + str(framerate) +"\n")
        for i in range(len(pixel_pos)):
            record_log.write(str(pixel_pos[i][0]) + "\t" + str(pixel_pos[i][1]) +
                             "\t" + str(pixel_pos[i][2]) + "\t" + str(pixel_pos[i][3]) +
                             "\t" + str(pixel_pos[i][4]) + "\t" + str(pixel_pos[i][5]) +
                             "\t" + str(pixel_pos[i][6]) + "\t" + str(pixel_pos[i][7]) + "\n")

        # Clean up
        kbint.process('1') # Stop flight
        if line_open == 1:
          tracking.terminate()
        comm.close()
        #kbint.close()
        kbmon.close()
        xb.close()

