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

    
try:
  rtm.initCORBA()
  setupRTM()

  viewer = rtm.findRTC("ImageViewer0")
  if viewer == None:
    raise Exception("Failed to find ImageViewer0")
  cpanel = rtm.findRTC("ControlPanel0")
  if cpanel == None:
    raise Exception("Failed to find ControlPanel0")

  vision_pc = setNameserver()
  robohw = rtm.findRTC("RobotHardware0")
  if robohw == None:
    raise Exception("Failed to find RobotHardware0")
     
  mgr = rtm.findRTCmanager(vision_pc)
  if mgr == None:
    raise Exception("Failed to find manager")

  mgr.load("V4L2CameraRTC")
  camera = mgr.create("V4L2CameraRTC", "v4l2")
  if camera == None:
    raise Exception("Failed to create V4L2CameraRTC")

  mgr.load("VideoSynchronizerRTC")
  syncer = mgr.create("VideoSynchronizerRTC", "sync")
  if syncer == None:
    raise Exception("Failed to create VideoSynchronizerRTC")

  syncer.setProperty("verbose", "1")

  connectPorts(camera.port("TimedCameraImage"), syncer.port("primary"))
  connectPorts(robohw.port("q"),                syncer.port("secondary"))
  connectPorts(cpanel.port("Command"),          camera.port("Command"))
  connectPorts(syncer.port("primaryOut"),       viewer.port("images"))
    
  cpanel.start()
  viewer.start()
  robohw.start()
  syncer.start()
  camera.start()
except Exception as err:
  print(err)


    
