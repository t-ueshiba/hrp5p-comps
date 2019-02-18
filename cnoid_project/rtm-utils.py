import os, socket, rtm

def setupRTM(nshost="localhost", nsport=2809, mgrport=2810):
  rtm.nshost  = nshost
  rtm.nsport  = nsport
  rtm.mgrport = mgrport
  nsloc       = "corbaloc:iiop:%s:%d/NameService" % (rtm.nshost, rtm.nsport)
  rtm.rootnc  = rtm.orb.string_to_object(nsloc)._narrow(rtm.CosNaming.NamingContext)
      
def setupNameserver(nsport=2809, mgrport=2810):
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
    nshost    = "localhost"
    vision_pc = "localhost"
  setupRTM(nshost, nsport, mgrport)
  return vision_pc

def findRTC(name):
  rtc = rtm.findRTC(name)
  if rtc == None:
    raise Exception("Failed to find {}.".format(name))
  return rtc

def createRTC(mgr, module, name):
  mgr.load(module)
  rtc = mgr.create(module, name)
  if rtc == None:
    raise Exception("Failed to create comp. {} named {}.".format(module, name))
  return rtc

