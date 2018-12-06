import sys,os,datetime,socket,rtm
from rtm import *
import cnoid.Corba

def setNameserver(nshost=None, nsport=2809):
   if nshost == None:
      rtm.nshost = socket.gethostname()
   else:
      rtm.nshost = nshost
   rtm.nsport = nsport
   nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
   rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
      
try:
   rtm.orb = cnoid.Corba.getORB()

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


    
