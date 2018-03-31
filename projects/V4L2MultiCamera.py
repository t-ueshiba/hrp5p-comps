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

camera = rtm.findRTC("V4L2MultiCamera0")
viewer = rtm.findRTC("MultiImageViewer0")
cpanel = rtm.findRTC("ControlPanel0")

connectPorts(camera.port("TimedImages"), viewer.port("image"))
connectPorts(cpanel.port("Command"),     camera.port("Command"))

#rtm.serializeComponents([camera, viewer, cpanel])

camera.start()
viewer.start()
cpanel.start()


    
