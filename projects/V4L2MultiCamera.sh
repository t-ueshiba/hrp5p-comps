#!/bin/bash
nameserver=cv077
camera=/${nameserver}/V4L2MultiCamera0.rtc
viewer=/${nameserver}/ImageViewer0.rtc
cpanel=/${nameserver}/ControlPanel0.rtc
       
rtcon ${camera}:TimedImages ${viewer}:TimedImages
rtcon ${camera}:Command ${cpanel}:Command

rtact ${camera} ${viewer} ${cpanel}
