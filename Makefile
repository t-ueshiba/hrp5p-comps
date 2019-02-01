#
#  $Id$
#
SUBDIR	= IIDCMultiCamera		\
	  V4L2MultiCamera		\
	  IIDCCamera			\
	  V4L2Camera			\
	  MultiImageViewer		\
	  ImageViewer			\
	  VideoSynchronizer		\
	  TUMultiImageViewerPlugin	\
	  TUImageViewerPlugin		\
	  TUControlPanelPlugin		\
#	  SADStereo			\
	  GFStereo

TARGETS	= all clean depend install other

$(TARGETS):
	@for d in $(SUBDIR); do				\
	  echo "";					\
	  echo "*** Current directory: $$d ***";	\
	  cd $$d;					\
	  $(MAKE) NAME=$$d $@;				\
	  cd ..;					\
	done
