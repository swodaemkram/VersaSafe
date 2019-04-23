#!/bin/bash

# NOTE: you must change tutorial.glade  file. change <interface> to <glade-interface>...
# remember to change both the open and close tags
# you must change the tag back to <interface>


#change <interface> into <glade-interface>
#first: just to make sure everything is set to an initial point
sed -i 's/glade-interface>/interface>/g' ../xml/flowform.glade

sed -i 's/<interface>/<glade-interface>/g' ../xml/flowform.glade
sed -i 's/<\/interface>/<\/glade-interface>/g' ../xml/flowform.glade

gtk-builder-convert ../xml/flowform.glade ../xml/flowform.xml

##below is the original compile line for the minimal app
##gcc -Wall -g -o tutorial main.c `pkg-config --cflags --libs gtk+-2.0` -export-dynamic

gcc -Wall -o flowform flowform.c -export-dynamic `pkg-config gtk+-2.0 libglade-2.0 gmodule-2.0 --cflags --libs`

sed -i 's/glade-interface>/interface>/g' ../xml/flowform.glade

