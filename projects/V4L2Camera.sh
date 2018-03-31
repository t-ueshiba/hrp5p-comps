#!/bin/bash
nameserver=localhost

camera_name=V4L2Camera
viewer_name=ImageViewer

camera=/${nameserver}/${camera_name}0.rtc
viewer=/${nameserver}/${viewer_name}0.rtc
       
rtcon ${camera}:TimedCameraImage ${viewer}:images

rtact ${camera} ${viewer}
