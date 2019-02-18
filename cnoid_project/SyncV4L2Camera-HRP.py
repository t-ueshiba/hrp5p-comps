import sys, os, datetime, rtm

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

  syncer.setProperty("verbose", "1")

  rtm.connectPorts(camera.port("TimedCameraImage"), syncer.port("primary"))
  rtm.connectPorts(robohw.port("q"),                syncer.port("secondary"))
  rtm.connectPorts(cpanel.port("Command"),          camera.port("Command"))
  rtm.connectPorts(syncer.port("primaryOut"),       viewer.port("images"))
    
  cpanel.start()
  viewer.start()
  robohw.start()
  syncer.start()
  camera.start()
except Exception as err:
  print(err)


    
