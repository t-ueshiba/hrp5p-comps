import sys,os,datetime,socket,rtm
from rtm import *
import cnoid.Corba

def setNameserver(nshost, nsport=2809):
   if nshost == None:
      rtm.nshost = socket.gethostname()
   else:
      rtm.nshost = nshost
   rtm.nsport = nsport
   nsloc = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
   print nsloc
   rtm.rootnc = rtm.orb.string_to_object(nsloc)._narrow(CosNaming.NamingContext)
      
try:
   rtm.orb = cnoid.Corba.getORB()

   setNameserver(None)
   viewer = rtm.findRTC("ImageViewer0")
   if viewer == None:
      raise Exception("Failed to find ImageViewer0")
   cpanel = rtm.findRTC("ControlPanel0")
   if cpanel == None:
      raise Exception("Failed to find ControlPanel0")

   setNameserver("hrp2001v")
   camera = rtm.findRTC("V4L2Camera0")
   if camera == None:
      raise Exception("Failed to find V4L2Camera0")

   connectPorts(cpanel.port("Command"), camera.port("Command"))
   connectPorts(camera.port("TimedCameraImage"), viewer.port("images"))
   
   cpanel.start()
   camera.start()
   viewer.start()
except Exception as err:
   print(err)
