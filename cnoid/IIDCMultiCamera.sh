#!/bin/bash
nameserver=localhost
camera=/${nameserver}/IIDCMultiCamera0.rtc
viewer=/${nameserver}/ImageViewer0.rtc
cpanel=/${nameserver}/ControlPanel0.rtc
       
rtcon ${camera}:TimedImages ${viewer}:TimedImages
rtcon ${camera}:Command ${cpanel}:Command

rtact ${camera} ${viewer} ${cpanel}
#rtact ${camera} ${viewer}
