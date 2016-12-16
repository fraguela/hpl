# ACL:license
#  ----------------------------------------------------------------------
#  This software and ancillary information (herein called "SOFTWARE")
#  called PETE (Portable Expression Template Engine) is
#  made available under the terms described here.  The SOFTWARE has been
#  approved for release with associated LA-CC Number LA-CC-99-5.
#  
#  Unless otherwise indicated, this SOFTWARE has been authored by an
#  employee or employees of the University of California, operator of the
#  Los Alamos National Laboratory under Contract No.  W-7405-ENG-36 with
#  the U.S. Department of Energy.  The U.S. Government has rights to use,
#  reproduce, and distribute this SOFTWARE. The public may copy, distribute,
#  prepare derivative works and publicly display this SOFTWARE without 
#  charge, provided that this Notice and any statement of authorship are 
#  reproduced on all copies.  Neither the Government nor the University 
#  makes any warranty, express or implied, or assumes any liability or 
#  responsibility for the use of this SOFTWARE.
#  
#  If SOFTWARE is modified to produce derivative works, such modified
#  SOFTWARE should be clearly marked, so as not to confuse it with the
#  version available from LANL.
#  
#  For more information about PETE, send e-mail to pete-devel@lanl.gov,
#  or visit the PETE web page at http://www.acl.lanl.gov/pete/.
#  ----------------------------------------------------------------------
# ACL:license

ifndef PETEDIR
PETEDIR = /home/pete/build/pete-2.1.0
endif
ifndef PETEARCH
PETEARCH = LINUXEGCS
endif

PROJECTROOT = $(shell pwd)
include $(PROJECTROOT)/config/$(PETEARCH)

default: tools

tools:
	make -Csrc/Tools

example:
	make -Cexamples

run:
	make -Cexamples run

clean:
	make -Csrc/Tools clean
	make -Cexamples clean

install: tools
	if [ ! -d $(PETEDIR) ] ; then mkdir $(PETEDIR) ; fi
	if [ ! -d $(PETEDIR)/src ] ; then mkdir $(PETEDIR)/src ; fi
	if [ ! -d $(PETEDIR)/src/PETE ] ; then mkdir $(PETEDIR)/src/PETE ; fi
	cp src/PETE/*.h $(PETEDIR)/src/PETE
	if [ ! -d $(PETEDIR)/src/Tools ] ; then mkdir $(PETEDIR)/src/Tools ; fi
	if [ ! -d $(PETEDIR)/$(PETEARCHDIR) ] ; then mkdir $(PETEDIR)/$(PETEARCHDIR) ; fi
	if [ ! -d $(PETEDIR)/$(PETEARCHDIR)/bin ] ; then mkdir $(PETEDIR)/$(PETEARCHDIR)/bin ; fi
	cp src/Tools/MakeOperators $(PETEDIR)/$(PETEARCHDIR)/bin
	cp -r html $(PETEDIR)

# ACL:rcsinfo
#  ----------------------------------------------------------------------
#  $RCSfile: makefile,v $   $Author: sa_smith $
#  $Revision: 1.7 $   $Date: 2000/08/03 22:28:12 $
#  ----------------------------------------------------------------------
# ACL:rcsinfo
