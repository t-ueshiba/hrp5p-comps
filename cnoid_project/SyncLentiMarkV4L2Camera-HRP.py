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
  setupRTM()                            # Set NameServer to this host.

  viewer = rtm.findRTC("ImageViewer0")
  if viewer == None:
    raise Exception("Failed to find ImageViewer0")
  cpanel = rtm.findRTC("ControlPanel0")
  if cpanel == None:
    raise Exception("Failed to find ControlPanel0")

  vision_pc = setNameserver()           # Set NameServer to control PC.
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

  mgr.load("LentiMarkTrackerRTC")
  lmtrck = mgr.create("LentiMarkTrackerRTC", "lmt")
  if lmtrck == None:
    raise Exception("Failed to create LentiMarkTrackerRTC")
     
  syncer.setProperty("verbose", "1")

  lmtrck_dir = "LentiMarkTrackerRTC/Data/"
  lmtrck.setProperty("AUTO_ADJUSTING", "1")
  lmtrck.setProperty("AUTO_TUNING",    "1")
  lmtrck.setProperty("IMAGE_DISP",     "0")
  lmtrck.setProperty("CAMERA_PARAM_FILE", lmtrck_dir + "asahi-NCM03-V-02.yml")
  lmtrck.setProperty("MARKER_DEF_FILE",   lmtrck_dir + "markers.def")
  lmtrck.setProperty("RECOG_PARAM_FILE",  lmtrck_dir + "recog.yml")
  
  connectPorts(camera.port("TimedCameraImage"), syncer.port("primary"))
  connectPorts(robohw.port("q"),                syncer.port("secondary"))
  connectPorts(cpanel.port("Command"),          camera.port("Command"))
  connectPorts(syncer.port("primaryOut"),       lmtrck.port("TimedCameraImage"))
  connectPorts(lmtrck.port("ResultImage"),      viewer.port("images"))
   
  viewer.start()
  cpanel.start()
  robohw.start()
  syncer.start()
  lmtrck.start()
  camera.start()
except Exception as err:
  print(err)
