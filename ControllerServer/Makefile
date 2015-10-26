#####################################
# Wireless Manager Server make file
# Created on: Dec 02,2014
# Author: jugo
#####################################

include env.mk
include modules.mk

TARGET    := controlCenter
CC        := g++
LD        := g++

SRC_DIR   := $(addprefix $(DIR_APP_ROOT),$(MODULES))
BUILD_DIR := $(addprefix $(DIR_APP_ROOT)build/,$(MODULES))
SRC       := $(filter-out %/main.cpp, $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c*))) $(DIR_APP_ROOT)main/main.cpp
OBJ       := $(patsubst $(DIR_APP_ROOT)%.cpp,$(DIR_APP_ROOT)build/%.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR)) 
CXXFLAGS  := -Wall -Wno-unused-variable -g -fno-strict-aliasing $(LOG)
EXT_INCLUDES := $(addprefix -I,$(addprefix $(DIR_APP_ROOT),$(EXT_MODULES))) 

LDLIBS    :=-L$(DIR_APP_ROOT)libs \
			-L/lib64 \
			-L/usr/lib64 \
			-L/usr/lib64/x86_64-linux-gnu \
			-lpthread \
			-lm -lrt -ldl \
			-lmongoclient \
			-lboost_thread \
			-lboost_system \
			-lboost_filesystem \
			-lsqlite3
			
vpath %.cpp $(SRC_DIR)

define make-goal
$1/%.o: %.cpp
	$(CC) $(EXT_INCLUDES) $(INCLUDES) -c $$< -o $$@ $(CXXFLAGS) 
endef

.PHONY: all bin/$(TARGET) lib checkdirs clean cleanAll

all: checkdirs package-u lib bin/$(TARGET)
	@cp $(DIR_APP_ROOT)center.conf $(DIR_APP_ROOT)bin
	@echo -ne '\n###### All Application Build Done. ######\n'

bin/$(TARGET): $(OBJ)
	$(LD) $^ -o $@ $(CXXFLAGS) $(LDLIBS)
	strip -s $@

lib:
	@for libName in ${LIB_NAMES} ; do \
		$(MAKE) V=99 -C $$libName static; \
	done
	@echo -ne '\n###### All Library Build Done. ######\n'
		
checkdirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p bin
	@mkdir -p libs
	
clean:
	@for libName in ${LIB_NAMES} ; do \
		$(MAKE) V=99 -C $$libName clean; \
	done
	@rm -rf $(DIR_APP_ROOT)build
	@rm -rf $(DIR_APP_ROOT)bin
	@rm -rf $(DIR_APP_ROOT)log

cleanAll: clean
	@rm -rf $(DIR_APP_ROOT)userDB/*
	@rm -rf $(DIR_APP_ROOT)deviceDB/*
	@rm -rf $(DIR_APP_ROOT)configDB/*
	@rm -rf $(DIR_APP_ROOT)libs/*
	
mk_debug:
	@echo "\n"
	@echo 'Program Name    : '$(TARGET)
	@echo 'Program Version : '$(VERSION)"\n"
	@echo 'Source Director : '$(SRC_DIR)"\n"
	@echo 'Build Director  : '$(BUILD_DIR)"\n"
	@echo 'Link Library    : '$(LDLIBS)"\n"
	@echo 'Extend Include :'$(EXT_INCLUDES)"\n"
	
package-s:
	yum -y install sqlite-devel.x86_64
	yum -y install boost boost-devel
	yum -y install gcc-c++
	@echo -ne '\n###### Package for CentSO install completed. ######\n'
	
package-u:
	apt-get -y install sqlite3 libsqlite3-dev
	apt-get -y install libboost-all-dev libboost-thread-dev
	apt-get -y install build-essential
	@echo -ne '\n###### Package for Ubuntu install completed. ######\n'
	

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
