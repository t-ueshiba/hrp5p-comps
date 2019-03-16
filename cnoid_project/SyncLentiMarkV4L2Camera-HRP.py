execfile("rtm-utils.py")

try:
  rtm.initCORBA()

  setupRTM()                            # Set NameServer to this host.
  viewer = findRTC("ImageViewer0")      # Find for existing viewer.
  cpanel = findRTC("ControlPanel0")     # Find for existing control panel.

  vision_pc = setupNameserver()         # Set NameServer to control PC.
  robohw = findRTC("RobotHardware0")    # Find for existing robot hardware.
     
  mgr = rtm.findRTCmanager(vision_pc)   # Find RTC manager on vision PC.
  if mgr == None:
    raise Exception("Failed to find manager on {}.".format(vision_pc))

  camera = createRTC(mgr, "V4L2CameraRTC",        "v4l2")
  syncer = createRTC(mgr, "VideoSynchronizerRTC", "sync")
  lmtrck = createRTC(mgr, "LentiMarkTrackerRTC",  "lmt")
     
  syncer.setProperty("verbose", "1")

  lmtrck_dir = "LentiMarkTrackerRTC/Data/"
  lmtrck.setProperty("AUTO_ADJUSTING", "1")
  lmtrck.setProperty("AUTO_TUNING",    "1")
  lmtrck.setProperty("IMAGE_DISP",     "0")
  lmtrck.setProperty("CAMERA_PARAM_FILE", lmtrck_dir + "asahi-NCM03-V-02.yml")
  lmtrck.setProperty("MARKER_DEF_FILE",   lmtrck_dir + "markers.def")
  lmtrck.setProperty("RECOG_PARAM_FILE",  lmtrck_dir + "recog.yml")
  
  rtm.connectPorts(camera.port("TimedCameraImage"), syncer.port("primary"))
  rtm.connectPorts(robohw.port("q"),                syncer.port("secondary"))
  rtm.connectPorts(cpanel.port("Command"),     camera.port("Command"))
  rtm.connectPorts(syncer.port("primaryOut"),  lmtrck.port("TimedCameraImage"))
  rtm.connectPorts(lmtrck.port("ResultImage"), viewer.port("images"))
   
  viewer.start()
  cpanel.start()
  robohw.start()
  syncer.start()
  lmtrck.start()
  camera.start()
except Exception as err:
  print(err)
