#
#  $Id$
#
SUBDIR	= IIDCMultiCamera	\
	  V4L2MultiCamera	\
	  MultiImageViewer	\
	  TU			\
#	  SADStereo		\
#	  GFStereo

TARGETS	= all clean depend install other

$(TARGETS):
	@for d in $(SUBDIR); do				\
	  echo "";					\
	  echo "*** Current directory: $$d ***";	\
	  cd $$d;					\
	  $(MAKE) NAME=$$d $@;				\
	  cd ..;					\
	done
