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

  setNameserver("hrp2001c")
  robohw = rtm.findRTC("RobotHardware0")
  if robohw == None:
    raise Exception("Failed to find RobotHardware0")
     
  setNameserver("hrp2001v")
  camera = rtm.findRTC("V4L2Camera0")
  if camera == None:
    raise Exception("Failed to find V4L2Camera0")
  syncer = rtm.findRTC("VideoSynchronizer0")
  if syncer == None:
    raise Exception("Failed to find VideoSynchronizer0")
  lmtrck = rtm.findRTC("LentiMarkTrackerRTC0")
  if lmtrck == None:
    raise Exception("Failed to find LentiMarkTrackerRTC0")

  syncer.setProperty("verbose", "1")

  lmtrck_dir = "/home/hrp2user/src/lentimark-tracker/LentiMarkTrackerRTC/Data/"
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
