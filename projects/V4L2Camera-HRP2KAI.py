import sys,os,datetime

import rtm
from rtm import *

import socket

rtm.nsport = 2809

import cnoid.Corba

rtm.orb = cnoid.Corba.getORB()
if rtm.nshost == None:
   rtm.nshost = socket.gethostname()
nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
print nsloc
rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
sys.argv = []

viewer = rtm.findRTC("ImageViewer0")
cpanel = rtm.findRTC("ControlPanel0")

rtm.nshost = "hrp2001v"
nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
print nsloc
rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)

camera = rtm.findRTC("V4L2Camera0")

connectPorts(cpanel.port("Command"), camera.port("Command"))
connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))

cpanel.start()
camera.start()
viewer.start()


    
