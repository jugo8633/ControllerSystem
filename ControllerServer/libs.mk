#####################################
# build library
# Created on: Oct 16, 2015
# Author: Louis Ju
#####################################

# set your lib folder name here.
#LIB_NAMES := httpServerHandler \
#			 socketHandler
			 
LIB_NAMES := 
				   
LIB_INCLUDE := $(addprefix -I,$(LIB_NAMES))

LIB_LINK := $(foreach bdir,$(LIB_NAMES),-L$(DIR_APP_ROOT)$(bdir)/lib -l$(bdir))

