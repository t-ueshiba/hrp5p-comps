import sys,os,datetime,socket,rtm
from rtm import *

def setNameserver(nshost=None, nsport=2809):
  if nshost == None:
    rtm.nshost = socket.gethostname()
  else:
    rtm.nshost = nshost
  rtm.nsport = nsport
  nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
  rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
      
try:
  import cnoid.Corba
  rtm.orb = cnoid.Corba.getORB()
except:
  rtm.initCORBA()

try:
  setNameserver()

  viewer = rtm.findRTC("ImageViewer0")
  if viewer == None:
    raise Exception("Failed to find ImageViewer0")
  cpanel = rtm.findRTC("ControlPanel0")
  if cpanel == None:
    raise Exception("Failed to find ControlPanel0")
  camera = rtm.findRTC("V4L2Camera0")
  if camera == None:
    raise Exception("Failed to find V4L2Camera0")

  connectPorts(cpanel.port("Command"),          camera.port("Command"))
  connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))

  viewer.start()
  cpanel.start()
  camera.start()
except Exception as err:
  print(err)


    
