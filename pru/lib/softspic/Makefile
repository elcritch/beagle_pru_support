# PRU_CGT environment variable must point to the TI PRU code gen tools directory. E.g.:
#(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
#(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru
#
# *ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
# order to use the same Makefile
#(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin

ifndef PRU_CGT
define ERROR_BODY

*******************************************************************************
PRU_CGT environment variable is not set. Examples given:
(Desktop Linux) export PRU_CGT=/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(Windows) set PRU_CGT=C:/path/to/pru/code/gen/tools/ti-cgt-pru_2.1.2
(ARM Linux*) export PRU_CGT=/usr/share/ti/cgt-pru

*ARM Linux also needs to create a symbolic link to the /usr/bin/ directory in
order to use the same Makefile
(ARM Linux) ln -s /usr/bin/ /usr/share/ti/cgt-pru/bin
*******************************************************************************

endef
$(error $(ERROR_BODY))
endif

MKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CURRENT_DIR := $(notdir $(patsubst %/,%,$(dir $(MKFILE_PATH))))
PROJ_NAME=$(CURRENT_DIR)
#-----------------------------------
INCLUDE=--include_path=../../include --include_path=../../include/am335x
#-----------------------------------
GEN_DIR=gen
TEST_DIR=test

#Common compiler flags (Defined in 'PRU Optimizing C/C++ Compiler User's Guide)
CFLAGS=-v3 -O2 --c99 --display_error_number --endian=little --hardware_mac=on --obj_directory=$(GEN_DIR) --pp_directory=$(GEN_DIR) -ppd -ppa

TARGET=$(GEN_DIR)/$(PROJ_NAME).lib
SOURCES=$(wildcard *.c)
#Using .object instead of .obj in order to not conflict with the CCS build process
OBJECTS=$(patsubst %,$(GEN_DIR)/%,$(SOURCES:.c=.object))
TOBJECTS=$(patsubst %,$(TEST_DIR)/%,$(SOURCES:.c=.o)) 

TEST_FLAGS=-O3 -g -I. -I.. -I../../include/ -I../../include/am335x/ -DTEST_BUILD -DSPI_CUSTOM_TIMING_CALLBACK -DPRU_SUPPORT_OVERRIDE_GPIO_FUNCS


all: printStart $(TARGET) printEnd

test: test/test

test/%.o: test/%.c
	$(CC) -c -o $@ $< $(TEST_FLAGS)

test/%.o: %.c
	$(CC) -c -o $@ $< $(TEST_FLAGS)

test/test: $(TOBJECTS)
	$(CC) $(TEST_FLAGS) $@.c -o $@ $^

printStart:
	@echo ''
	@echo '************************************************************'
	@echo 'Building project: $(PROJ_NAME)'

printEnd:
	@echo ''
	@echo 'Finished building project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Invokes the archiver to make the .lib file
$(TARGET): $(OBJECTS)
	@echo ''
	@echo 'Building target: $@'
	@echo 'Invoking: PRU Archiver'
	$(PRU_CGT)/bin/arpru r $(TARGET) $(OBJECTS)
	@cp $(TARGET) ../../../$(ARTIFACT_DIR)/$(PROJ_NAME).lib
	@echo 'Finished building target: $@'
	@echo ''
	@echo 'Output files can be found in the "$(GEN_DIR)" directory'

# Invokes the compiler on all c files in the directory to create the object files
$(GEN_DIR)/%.object: %.c
	@mkdir -p $(GEN_DIR)
	@echo ''
	@echo 'Building file: $<'
	@echo 'Invoking: PRU Compiler'
	$(PRU_CGT)/bin/clpru --include_path=$(PRU_CGT)/include $(INCLUDE) $(CFLAGS) -fe $@ $<

.PHONY: all clean

# Remove the $(GEN_DIR) directory
clean:
	@echo ''
	@echo '************************************************************'
	@echo 'Cleaning project: $(PROJ_NAME)'
	@echo ''
	@echo 'Removing files in the "$(GEN_DIR)" directory'
	@rm -rf $(GEN_DIR)
	@rm -rf $(TEST_DIR)/*.a $(TEST_DIR)/*.o
	@rm -rf ../../../$(ARTIFACT_DIR)/$(PROJ_NAME).lib
	@echo ''
	@echo 'Finished cleaning project: $(PROJ_NAME)'
	@echo '************************************************************'
	@echo ''

# Includes the dependencies that the compiler creates (-ppd and -ppa flags)
-include $(OBJECTS:%.object=%.pp)
