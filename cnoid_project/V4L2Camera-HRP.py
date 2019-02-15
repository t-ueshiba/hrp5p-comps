import sys,os,datetime,socket,rtm
from rtm import *

def setupRTM(nshost=None, nsport=2809, mgrport=2810):
  rtm.nsport  = nsport
  rtm.mgrport = mgrport
  rtm.nshost  = socket.gethostname() if nshost == None else nshost
  nsloc      = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
  rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
      
def setNameserver(nsport=2809, mgrport=2810):
  hostname = socket.gethostname()
  if os.getenv("ROBOTHOST"):
    nshost    = os.getenv("ROBOTHOST")+"c"
    vision_pc = os.getenv("ROBOTHOST")+"v"
  elif hostname == "hrp2001t":
    nshost    = "hrp2001c"
    vision_pc = "hrp2001v"
  elif hostname == "hrp5p01t":
    nshost    = "hrp5p01c.local"
    vision_pc = "hrp5p01v.local"
  elif hostname == "hrp2012t" or hostname == "hrp2010t" or hostname == "edin":
    nshost    = "hrp2012c"
    vision_pc = "hrp2012v"
  else:
    nshost    = hostname
    vision_pc = "localhost"
  setupRTM(nshost, nsport, mgrport)
  return vision_pc

def searchRTC(comp_name):
  rtc = rtm.findRTC(comp_name)
  if rtc == None:
    raise Exception("Failed to find {}.".format(comp_name))
  return rtc

def createRTC(mgr, module_name, comp_name):
  mgr.load(module_name)
  rtc = mgr.create(module_name, comp_name)
  if rtc == None:
    raise Exception("Failed to create {} from {}.".format(comp_name,
                                                          module_name))
  return rtc


try:
  rtm.initCORBA()

  setupRTM()                            # Set NameServer to this host.
  viewer = searchRTC("ImageViewer0")    # Search for existing viewer.
  cpanel = searchRTC("ControlPanel0")   # Search for existing control panel.

  vision_pc = setNameserver()           # Set NameServer to control PC.
  mgr = rtm.findRTCmanager(vision_pc)   # Find RTC manager on vision PC.
  if mgr == None:
    raise Exception("Failed to find manager on {}.".format(vision_pc))
  camera = createRTC(mgr, "V4L2CameraRTC", "v4l2")

  connectPorts(cpanel.port("Command"),          camera.port("Command"))
  connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))

  viewer.start()
  cpanel.start()
  camera.start()
except Exception as err:
  print(err)
