import sys,os,datetime

import rtm
from rtm import *

import socket

rtm.nsport = 2809

import cnoid.Corba

# Get RTManager.
rtm.orb = cnoid.Corba.getORB()

# Find components running on localhost.
if rtm.nshost == None:
   rtm.nshost = socket.gethostname()
nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
print nsloc
rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
sys.argv = []
viewer = rtm.findRTC("ImageViewer0")
cpanel = rtm.findRTC("ControlPanel0")

# Find components running on hrp2001c.
rtm.nshost = "hrp2001c"
nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
print nsloc
rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
robohw = rtm.findRTC("RobotHardware0")

# Find components running on hrp2001v.
rtm.nshost = "hrp2001v"
nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
print nsloc
rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
camera = rtm.findRTC("V4L2Camera0")
syncer  = rtm.findRTC("VideoSynchronizer0")

connectPorts(camera.port("TimedCameraImage"), syncer.port("primary"))
connectPorts(robohw.port("q"),                syncer.port("secondary"))
connectPorts(cpanel.port("Command"),          camera.port("Command"))
connectPorts(syncer.port("primaryOut"),       viewer.port("images"))

cpanel.start()
viewer.start()
robohw.start()
syncer.start()
camera.start()


    
