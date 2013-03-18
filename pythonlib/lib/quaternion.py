from math import *

def quatGenerate(angle, axis):
    a_2 = angle/2.0;
    w = cos(a_2);
    sina_2 = sin(a_2);
    x = axis[0]*sina_2;
    y = axis[1]*sina_2;
    z = axis[2]*sina_2;
    return (w, x, y, z)
    

def eulerToQuaternion(yaw, pitch, roll):
    psi_2 = yaw/2.0
    theta_2 = pitch/2.0
    phi_2 = roll/2.0
    
    w = cos(phi_2)*cos(theta_2)*cos(psi_2) + \
        sin(phi_2)*sin(theta_2)*sin(psi_2)
    x = sin(phi_2)*cos(theta_2)*cos(psi_2) - \
        cos(phi_2)*sin(theta_2)*sin(psi_2)
    y = cos(phi_2)*sin(theta_2)*cos(psi_2) + \
        sin(phi_2)*cos(theta_2)*sin(psi_2)
    z = cos(phi_2)*cos(theta_2)*sin(psi_2) - \
        sin(phi_2)*sin(theta_2)*cos(psi_2)
        
    return (w, x, y, z)

def eulerToQuaternionDeg(yaw, pitch, roll):
    return eulerToQuaternion(radians(yaw), radians(pitch), radians(roll))
    
POLE_LIMIT = 0.499
    
def quaternionToEuler(q):
    w = q[0];
    x = q[1];
    y = q[2];
    z = q[3];
    temp1 = w*y - z*x;
    if temp1 > POLE_LIMIT:
        psi = 2*atan2(w, x)
        theta = -pi/2.0;
        phi = 0.0;
    elif temp1 < - POLE_LIMIT:
        psi = -2*atan2(w, x)
        theta = pi/2.0;
        phi = 0.0;
    else:
        theta = asin(2.0*temp1)
        phi = atan2(2.0*(w*x + y*z), 1.0 - 2.0*(x*x + y*y))
        psi = atan2(2.0*(w*z + x*y), 1.0 - 2.0*(y*y + z*z))
    return (psi, theta, phi)
    
if __name__ == '__main__':
    q1 = (0.7332657075825944, 0.18601817061114526, 0.6152829847875021, -0.22168782311363508)
    q2 = (0.7165254267866683, -0.22735636632457662, 0.6012362213458364, 0.27095276636647947)
    e1 = quaternionToEuler(q1)
    e2 = quaternionToEuler(q2)
    
    print "Yaw, Pitch, Roll 1: " + str(e1[0]) + " " + str(e1[1]) + " " + str(e1[2]) + "\n"
    print "Yaw, Pitch, Roll 2: " + str(e2[0]) + " " + str(e2[1]) + " " + str(e2[2]) + "\n"
    
    
