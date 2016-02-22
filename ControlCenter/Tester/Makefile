#####################################
# httpHandler make file
# Created on: Nor 02,2015
# Author: Louis Ju
#####################################
TARGET    := tester
CC        := g++
LD        := g++

DIR_APP_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR   := $(DIR_APP_ROOT)
BUILD_DIR := $(DIR_APP_ROOT)
SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)*.c*))
OBJ       := $(patsubst %.cpp, %.o,$(SRC))
INCLUDES  := $(addprefix -I,$(SRC_DIR)) -I../global_inc
CXXFLAGS  := -Wall -Wno-unused-variable -g -fno-strict-aliasing -DDEBUG
			
vpath %.cpp $(SRC_DIR)

define make-goal
$1%.o: %.cpp
	$(CC) $(INCLUDES) -c $$< -o $$@ $(CXXFLAGS) 
	@echo -ne '###### $1 Object Build Done. ######\n'
	@echo "\n"
endef

.PHONY: all bin/$(TARGET) checkdirs clean cleanAll

.IGNORE: clean

all: checkdirs bin/$(TARGET)
	@echo -ne '\n###### $(TARGET) Build Done. ######\n'
	@echo "\n"

bin/$(TARGET): $(OBJ)
	@echo -ne '\n###### Start Build $(TARGET) ######\n'
	$(LD) $^ -o $@ $(CXXFLAGS)
	strip -s $@
	@echo "\n"
		
checkdirs:
	@mkdir -p bin
	
clean:
	@rm -rf *.o
	@rm -rf $(DIR_APP_ROOT)bin
	
mk_debug:
	@echo "\n"
	@echo 'Program Name    : '$(TARGET)
	@echo 'Source Director : '$(SRC_DIR)"\n"
	@echo 'Build Director  : '$(BUILD_DIR)"\n"
	@echo 'Source  : '$(SRC)"\n"
	@echo 'Build   : '$(OBJ)"\n"
	@echo 'INCLUDE   : '$(INCLUDES)"\n"
		
$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
