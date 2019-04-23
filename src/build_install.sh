#!/bin/bash

# this is the script that builds the installation files for FlowForm OSL
# it will build the tar file that contains all executables and modules
# and place the tar file in the installer/ folder

AUTHOR="Gary Conway"
VERSION="1.0";
INSTALLDIR="installer";
TARFILE="flowform_install_files.tar";
INSTALL_SCRIPT="install.sh";
LOGFILE="build.log";
CREATEOPTIONS="cf";
OPTIONS="rf";

YEAR=`date +%Y`


cd ..
echo 
echo
echo "Building Installation Files in /$INSTALLDIR/ directory";
echo "Version $VERSION by $AUTHOR";
echo "Copyright 2013-$YEAR Flow International, All Rights Reserved";
echo "SCRIPT:$0";
echo "============================================================";
echo ..
echo "Removing Old Archive";
rm -rf $INSTALLDIR/$TARFILE

if [ "$?" == 0 ]; then
	echo "Successfully deleted old archive -or- old archive doesn't exist."
else
	echo "Error deleting Old archive."
	exit
fi



# if the file still exists, then something went wrong
if [ -e $INSTALLDIR/$TARFILE ]; then
	echo "ERROR::$INSTALLDIR/$TARFILE was not deleted successfully";
	echo
	echo
	exit
fi


echo ..
echo "Building $INSTALLDIR/$TARFILE";
tar $CREATEOPTIONS $INSTALLDIR/$TARFILE flowform_app/flowform

# if the file doesnt exist, then something went wrong

if [ -e $INSTALLDIR/$TARFILE ]; then
	echo "$INSTALLDIR/$TARFILE created successfully.";
	echo
	echo
else
    echo "ERROR::$INSTALLDIR/$TARFILE was not created successfully";
	echo
	echo
    exit
fi


function MakeTar()
{
	tar $OPTIONS $INSTALLDIR/$TARFILE $1
	if [ "$?" == 0 ]; then
	    echo "$1 added to archive";
	else
		echo "ERROR adding $1 files to archive";
	fi
} 


MakeTar "production_app/production";
MakeTar "locale/*.lang";
MakeTar "logs/ --exclude=*.*";
MakeTar "res/flowformrc";
MakeTar "recipe/teststand.mcd";
MakeTar "xml/*.xml";
MakeTar "recipe_editor_app/recipe_editor";
MakeTar "header_editor_app/header_editor";
MakeTar "data/*.sdf";
MakeTar "images/";
MakeTar ".flowformrc";
MakeTar "controller/maestro/lang/*.lang";
MakeTar "controller/maestro/user_program/*.img";
MakeTar "controller/maestro/user_program/*.mrs";
MakeTar "controller/maestro/user_program/*.dat";
MakeTar "controller/maestro/user_program/user_files.txt";
MakeTar "headers/teststand.blf";
MakeTar "jobs/ --exclude=*.*";

# log the build
DATE=`date`
echo "Build: $DATE" >> $INSTALLDIR/$LOGFILE

echo "Finished.";
echo
echo


