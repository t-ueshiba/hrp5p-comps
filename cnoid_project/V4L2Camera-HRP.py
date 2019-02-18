execfile("rtm-utils.py")

try:
  rtm.initCORBA()

  setupRTM()                            # Set NameServer to this host.
  viewer = findRTC("ImageViewer0")      # Find for existing viewer.
  cpanel = findRTC("ControlPanel0")     # Find for existing control panel.

  vision_pc = setupNameserver()         # Set NameServer to control PC.
  mgr = rtm.findRTCmanager(vision_pc)   # Find RTC manager on vision PC.
  if mgr == None:
    raise Exception("Failed to find manager on {}.".format(vision_pc))
  camera = createRTC(mgr, "V4L2CameraRTC", "v4l2")

  rtm.connectPorts(cpanel.port("Command"),          camera.port("Command"))
  rtm.connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))

  viewer.start()
  cpanel.start()
  camera.start()
except Exception as err:
  print(err)
