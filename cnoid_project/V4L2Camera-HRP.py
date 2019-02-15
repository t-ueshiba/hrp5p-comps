import sys,os,datetime,socket,rtm
from rtm import *

def setupRTM(nshost=None, nsport=2809, mgrport=2810):
  rtm.nsport  = nsport
  rtm.mgrport = mgrport

  hostname = socket.gethostname()
  if os.getenv("ROBOTHOST"):
    rtm.nshost = os.getenv("ROBOTHOST")+"c"
    vision_pc  = os.getenv("ROBOTHOST")+"v"
  elif hostname == "hrp2001t":
    rtm.nshost = "hrp2001c"
    vision_pc  = "hrp2001v"
  elif hostname == "hrp5p01t":
    rtm.nshost = "hrp5p01c.local"
    vision_pc  = "hrp5p01v.local"
  elif hostname == "hrp2012t" or hostname == "hrp2010t" or hostname == "edin":
    rtm.nshost = "hrp2012c"
    vision_pc  = "hrp2012v"
  else:
    rtm.nshost = hostname
    vision_pc  = "localhost"

  rtm.initCORBA()

  nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
  rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)

  return vision_pc

try:
  vision_pc = setupRTM()

  print("Find ImageViewer0")
  viewer = rtm.findRTC("ImageViewer0")
  if viewer == None:
    raise Exception("Failed to find ImageViewer0")
  print("Find ControlPanel0")
  cpanel = rtm.findRTC("ControlPanel0")
  if cpanel == None:
    raise Exception("Failed to find ControlPanel0")

  print("Find manager")
  mgr = rtm.findRTCmanager(vision_pc)
  if mgr == None:
    raise Exception("Failed to find manager")

  print("Load V4L2CameraRTC")
  mgr.load("V4L2CameraRTC")
  camera = mgr.create("V4L2CameraRTC", "V4L2Camera0")
  if camera == None:
    raise Exception("Failed to find V4L2Camera0")

  connectPorts(cpanel.port("Command"),          camera.port("Command"))
  connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))

  viewer.start()
  cpanel.start()
  camera.start()
except Exception as err:
  print(err)
