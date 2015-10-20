#####################################
# global variable
# Created on: Oct 16, 2015
# Author: Louis Ju
#####################################

export VERSION=v0.0.0.2
export DIR_APP_ROOT=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#####################################
# Version module controle
# Build Debug version use: -DDEBUG -DTRACE
# Build Release version use: -DRELEASE
#####################################
export LOG=-DDEBUG