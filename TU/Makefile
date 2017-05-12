#
#  $Id$
#
#################################
#  User customizable macros	#
#################################
VPATH		= ./idl

INCDIR		= $(PREFIX)/include/TU/idl

#########################
#  Macros set by mkmf	#
#########################
SUFFIX		= .idl:sC
IDLS		= Img.idl Cmd.idl

TARGIDLS	= $(IDLS:%=$(INCDIR)/%)


install: $(TARGIDLS)

$(INCDIR)/%: %
	install -m 0644 $< $@


