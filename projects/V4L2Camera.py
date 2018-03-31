import sys,os,datetime

import rtm
from rtm import *

import socket

rtm.nsport = 2809

try:
    # for choreonoid
    import cnoid.Corba

    rtm.orb = cnoid.Corba.getORB()
    if rtm.nshost == None:
        rtm.nshost = socket.gethostname()
    nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
    print nsloc
    rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
    sys.argv = []
    inChoreonoid = True
except:
    rtm.initCORBA()
    inChoreonoid = False

if inChoreonoid == True:
    camera = rtm.findRTC("V4L2Camera0")
    viewer = rtm.findRTC("ImageViewer0")
else:
    mgr = rtm.findRTCmanager()

    mgr.load("V4L2CameraComp")
    camera = mgr.create("V4L2Camera", "camera")

    mgr.load("ImageViewerComp")
    viewer = mgr.create("ImageViewer", "viewer")

connectPorts(camera.port("TimedCameraImage"), viewer.port("image"))

rtm.serializeComponents([camera, viewer])

camera.start()
viewer.start()


    
