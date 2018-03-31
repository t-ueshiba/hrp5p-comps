#!/bin/bash
nameserver=localhost

camera_name=V4L2MultiCamera
viewer_name=MultiImageViewer

camera=/${nameserver}/${camera_name}0.rtc
viewer=/${nameserver}/${viewer_name}0.rtc
       
rtcon ${camera}:TimedImages ${viewer}:images

rtact ${camera} ${viewer}
