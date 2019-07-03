# Module: VersaSafe Makefile
# Author: Gary Conway <gary.conway@fireking.com>
# Created: 12-5-17
# Language: make, shell, c, c++, sed
# Version: 1.0
# Notice: Copyright 2017 FireKing Security Group, All Rights Reserved
#

DT := $(shell /bin/date "+%Y-%m-%d--%H-%M-%S")


#compile with no stripping set to ON
#FEATURES="nostrip" make
export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/
PROJECT="VSafe"
EXECUTABLE=vsafe


# set the compiler options
SHELL=/bin/bash
DYNAMIC=-export-dynamic
#DEBUG=-O1 -pipe -ggdb -Q -dm
#for valgrind
#DEBUG=-O0 -ggdb
#CFLAGS=`pkg-config --cflags --libs gtk+-2.0 libglade-2.0 gmodule-2.0`
#CFLAGS=`pkg-config --cflags --libs gtk+-2.0 gtkimageviewer-21.0`
CFLAGS=`pkg-config --cflags --libs gtk+-3.0 libusb-1.0  libserial`
CFLAGS+=`mysql_config --cflags --libs`
#CFLAGS+=-lmodbus
#CFLAGS+=-llibusb
#CFLAGS+=llibserial
#enable the following flag to compile with optimizations (takes longer, but exe is smaller)
CFLAGS+=-Os
CFLAGS+=-std=c++11
#CFLAGS+=$(DEBUG)
# use Wall to find all warnings and fix
#DFLAGS=-Wall
CC=g++
SOURCEDIR=../
TARFILE=$(SOURCEDIR)$(EXECUTABLE).tar
ZIPFILE=zipfiles/$(EXECUTABLE)_$(DT).zip
BACKUP=/home/vsafe_backup/

# set the source modules
SOURCES=$(EXECUTABLE).cpp \
trim.cpp \
common.cpp \
ip_module.cpp \
segfault.cpp \
locale/locale.cpp \
logs.cpp \
socket_class.cpp \
help.cpp \
queue.cpp \
tinyxml/tinyxml.cpp \
tinyxml/tinyxmlerror.cpp \
tinyxml/tinyxmlparser.cpp \
tinyxml/tinystr.cpp \
buslogic.cpp \
pid.cpp \
dbf.cpp \
usb.cpp \
usb_gateway.cpp \
net_gateway.cpp \
drivers/jcm_driver.cpp \
api.cpp


OBJECTS=$(SOURCES:.cpp=.o)
PGM_VERSION=-DPGM_FLAVOR=200

#.PRECIOUS: $(OBJECTS)
#.SECONDARY: $(OBJECTS)


# various compiler options
# -o <output file>
# -c compile but do not link
# -g debugging information


help:
	@echo
	@echo $(PROJECT) Makefile HELP
	@echo ======================
	@echo "$(PROJECT) Makefile created by Gary Conway"
	@echo make help:: to display this help
	@echo make all:: to build $(PROJECT) completely
	@echo make clean:: to delete all object files
	@echo make cleanup:: remove all temp files *.*~
	@echo make deps:: to show Makefile rule suggestions
	@echo make tar:: to build a tar file of all source files recursively from source root
	@echo make count:: to display a count of all source files
	@echo make size:: show total source file size
	@echo make lines:: show total lines of source code
	@echo make install:: build installation files
	@echo make back:: backup all source files
	@echo




all:
#	@rm -rf *o $(EXECUTABLE)
	@echo
	@echo
	$(CC) $(PGM_VERSION) $(DFLAGS) -o $(EXECUTABLE) $(SOURCES) $(DYNAMIC) $(CFLAGS) $(DEBUG)
#	$(CC)   $(DFLAGS) -o $(EXECUTABLE) $(SOURCES) $(DYNAMIC) `pkg-config gtk+-2.0 libglade-2.0 --cflags --libs`


#
# &< = automatic var, name of the dependency
# $@ = automatic var, name of the target
#all: $(SOURCES) $(EXECUTABLE)
#$(EXECUTABLE): $(OBJECTS)
#	$(CC) $(CFLAGS) $(LFLAGS)  $(OBJECTS) -o $@
#
#.cpp.o: $(CC) $(CFLAGS) $< -o $@





clean:
	@echo Removing all obj and executable files
	@rm -rf $(OBJECTS) $(EXECUTABLE)
	@rm -rf *.h~
	@rm -rf *.cpp~

cleanup:
	@echo Removing all *.*~ files
	@rm -rf *.*~
	@echo Removing all *.bak files
	@rm -rf *.bak


#stub:
#	$(CC) $(pkg-config --cflags --libs gtk+-2.0 libglade-2.0 gmodule-2.0) -c stub.cpp flowform.c
#
#.c.o:   $(CC) $(pkg-config --libs gtk+-2.0 libglade-2.0 gmodule-2.0) -o stub stub.o flowform.o



deps:
	$(CC) -M $(DFLAGS) $(SOURCES) $(CFLAGS)


tar:
# to exclude a folder from the backup use: touch /folder/.exclude_from_backup
	@echo VersaSafe tar file of all source files/folders
	tar cfv $(TARFILE) * --exclude-tag-all=.exclude_from_backup
# list all files in the archive
	tar -tf $(TARFILE)
# count the files
	@tar -tf $(TARFILE) | wc -l
	cp $(TARFILE) $(BACKUP)

count:
	@echo Total Source Files
#TODO:the following sed statement doesn't seem to work
	@`TOTFILES=`tar -tf $(TARFILE) | wc -l | sed "s/\r\n/xx/"``
	@echo "$(TOTFILES) files/folders"
#find . -name '*.[ch]' -print0 | wc -L --files0-from=- | tail -n1


size:
	@echo Total Source File Size
	du -h ../



lines:
	@echo
	@echo
	@echo Total lines of source code
	@echo
	@echo -n .cpp files
	@find ../  -name '*.cpp' | xargs wc -l | grep total
	@echo
	@echo -n .inc files
	@find ../  -name '*.inc' | xargs wc -l | grep total
	@echo
	@echo -n .h files
	@find ../  -name '*.h' | xargs wc -l | grep total
	@echo
	@echo -n .txt files
	@find ../  -name '*.txt' | xargs wc -l | grep total
	@echo
	@echo -n .mrs files
	@find ../  -name '*.mrs' | xargs wc -l | grep total
	@echo
	@echo -n .mcd files
	@find ../  -name '*.mcd' | xargs wc -l | grep total
	@echo
	@echo -n .xml files
	@find ../  -name '*.xml' | xargs wc -l | grep total
	@echo
	@echo -n .glade files
	@find ../  -name '*.glade' | xargs wc -l | grep total
	@echo
	@echo


install:
	@echo
	@echo Building installation files for installation of all FireKing suite products
	@echo
	@.././build_install.sh



zip:
	@echo
	@echo Zipping up all source files
	@echo
	@chown -R garyc:garyc *
	@chmod -R 755 *
	@echo "DT $(DT)"
#	ZIPFILE=$(EXECUTABLE)_$(DT)
	@echo $(ZIPFILE)
	@zip -r $(ZIPFILE) * -x *.zip *shared* *_archive* *template* *keyboard* *function* -x "zipfiles/*" "NewDebug/*" "tinyxml/*"
	@echo "Zip done [$(ZIPFILE)"
	@cp $(ZIPFILE) $(BACKUP)



back:
	@echo
	@echo Backing up all source files
	@echo
	@chown -R garyc:garyc *
	@chmod -R 755 *
	@cp -frpv hdr/ $(BACKUP)hdr
	@cp -frpv dbf/ $(BACKUP)dbf
	@cp -frpv xml/ $(BACKUP)xml
	@cp -frpv drivers/ $(BACKUP)server
	@cp -frpv drivers/ $(BACKUP)drivers
#	@cp -frpv installer/ $(BACKUP)installer
	@cp -frpv images/ $(BACKUP)images
	@cp -frpv locale/ $(BACKUP)locale
#	@cp -frpv tinyxml $(BACKUP)tinyxml
	@cp -pv * $(BACKUP)


