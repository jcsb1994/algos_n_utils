# This makefile builds the algos_n_utils static library
# run `make help` for more info

# Check if make version is recent enough
required_version := 3.80
is_version_ok := $(filter $(required_version),$(firstword $(sort $(MAKE_VERSION) $(required_version))))
ifeq ($(is_version_ok),) # blank if too old
$(error Make version not supported)
endif

# ========================================================
# User Settings Variables
# ========================================================

# Verbose print info
VERBOSE?=0
ifeq ($(VERBOSE),0)
Q = @
else
Q =
endif

# Build directory
# default is for when we build inside the lib repo's dir, will outut the  build to
# algos_n_utils/build/anu, otherwise you can specify PATH_TO_BUILD to your build path
# so that you get project/build/anu along with your other build artifacts
# Note: make clean will delete default build dir only, use your own clean for custom build dirs
DEFAULT_BUILD_DIR := build
PATH_TO_BUILD ?= $(DEFAULT_BUILD_DIR)
ifeq ($(PATH_TO_BUILD),/)
$(error Specified build directory is root, abort)
endif

ALGOS_N_UTILS_OUT_FOLDER :=anu
BUILD_DIR := $(PATH_TO_BUILD)/$(ALGOS_N_UTILS_OUT_FOLDER)


# ========================================================
# Unit Test Secondary Makefile variables
# ========================================================

# This repository provides unit tests for the Telink chips through this makefile.
# Unit tests may provide a secondary makefile to override or add environment vars to the build.
# Here, we initialize variables needed to determine if we are calling this makefile to build a
# test, and if there is a secondary makefile to include

# Test Initialization
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# If the make file is called with any 'test_foobar' target, it will build a binary and run it
# In such case, TEST_NAME and following variables will be populated
TEST_NAME := $(filter test%,$(MAKECMDGOALS))
# ANU_TEST_MAIN_SRCS := $(shell find . -name 'test*.cpp')
ANU_TEST_MAIN_SRCS := $(wildcard test*.cpp) $(wildcard */test*.cpp) $(wildcard */*/test*.cpp) $(wildcard */*/*/test*.cpp)

# Hand-pick test files, must be excluded from the library files
# Reminder: cannot be called test_*, this is reserved for test main files
# Add to this variable in a test secondary makefile if custom source files are needed
TEST_FILES := anu_test.cpp
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Unit Test Secondary Makefile
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# It must be included before all the other test configuration parts so that it can override.
# The makefile must have the same name as the test's main c file (must start with test_)
GOAL_CPP_FILE := $(patsubst %,%.cpp,$(TEST_NAME))

ifneq ($(filter %.cpp,$(GOAL_CPP_FILE)),) # if there is a .cpp matching the make target, we launch a test!
GOAL_MAKEFILE := $(patsubst %.cpp,%.mk,$(filter %$(GOAL_CPP_FILE),$(ANU_TEST_MAIN_SRCS)))
GOAL_SH_SCRIPT := $(GOAL_MAKEFILE:%.mk=%.sh)
endif

ifneq ("$(wildcard $(GOAL_MAKEFILE))","")
$(info Test supplies additional make settings, including them)
$(info Note: Use ?= if you want these settings to be overwritten from the terminal!)
include $(GOAL_MAKEFILE)
endif
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# ========================================================
# Custom API (T8A) Variables
# ========================================================

# Find all directories except for the out-of-source build folder
ANU_DIRS := $(sort $(dir $(wildcard */)))
ANU_DIRS := $(filter-out $(DEFAULT_BUILD_DIR)/, $(ANU_DIRS))

# Library files
ANU_CPP_FILES := $(wildcard *.cpp) $(foreach dir,$(ANU_DIRS), $(wildcard $(dir)*.cpp))
# Remove test-related files
ANU_CPP_FILES := $(filter-out test_%.cpp, $(ANU_CPP_FILES))
ANU_CPP_FILES := $(filter-out $(TEST_FILES), $(ANU_CPP_FILES))

ANU_SRCS := $(ANU_CPP_FILES) # $(ANU_C_FILES), ...

# ========================================================
# Library Variables
# ========================================================

LIB_BASENAME:=anu
LIB_NAME :=$(LIB_BASENAME).a
# Our library will be composed of the ANU files exclusively
LIB_SRCS := $(ANU_SRCS)

LIB_OBJS := $(LIB_SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Dependencies are simply obj files as .d
DEPFILES := $(LIB_OBJS:%.o=%.d)

# Get an original source file's relative path from provided out-of-source object (ooso) file
get_src_from_ooso = $(patsubst $(BUILD_DIR)/%.o,%.cpp,$(1))


# ========================================================
# Flags and Implicit Variables
# ========================================================

# Default compiler, can be overwritten
COMPILER?=g++
CC:=$(COMPILER)

# Include paths (-I compiler flags)
I_PATHS := -I.
I_PATHS += $(patsubst %, -I%, $(ANU_DIRS))

# Provides a different set of compiler flags
ifndef NEW_CFLAGS
CFLAGS := -Wall
else
CFLAGS := $(NEW_CFLAGS)
endif
# Or just additional flags
EXTRA_CFLAGS ?=

# Compiler flags
CFLAGS += $(I_PATHS) $(EXTRA_CFLAGS)

DEPFLAGS ?= -MT $@ -MMD -MP -MF $*.d # Is not supported by all compilers

STATIC_LIB_FLAGS := rcs

# ========================================================
# Test Variables
# ========================================================

ifneq ($(filter %.cpp,$(GOAL_CPP_FILE)),) # if there is a .cpp, we are launching a test!
CFLAGS+=-DTEST_UNIT_NAME=\"$(TEST_NAME)\" # Pass the name to compiler so it prints out when running
endif

# Test sources to link with the library
TEST_CPP_FILES := $(filter %.cpp, $(TEST_FILES))
TEST_SRCS := $(TEST_CPP_FILES) # $(TEST_C_FILES), .s, ... if we ever add any
TEST_OBJS := $(TEST_SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Test main files
TEST_MAIN_OBJS := $(ANU_TEST_MAIN_SRCS:%.cpp=$(BUILD_DIR)/%.o) # e.g. build/anu/algo1/test/test_algo1.o

TEST_BUILD_DIRS := $(sort $(patsubst %/,%,$(dir $(TEST_MAIN_OBJS)))) # e.g. build/anu/algo1/test
PHONY_TEST_RULES := $(patsubst %.cpp,%,$(notdir $(ANU_TEST_MAIN_SRCS))) # e.g. test_algo1
# TEST_ELFS := $(TEST_MAIN_OBJS:%.o=%.elf) #$(patsubst ./%.cpp,$(BUILD_DIR)/%.elf,$(foreach file,$(PHONY_TEST_RULES),$(shell find -name $(patsubst %,%.cpp,$(file)))))
TEST_BINS := $(TEST_MAIN_OBJS:%.o=%.bin) # e.g. build/anu/algo1/test/test_algo1.o

# For the phony test targets, will turn into ooso, e.g. build/anu/algo1/test/run_test will gen build/anu/algo1/test/main.o
# get_test_ooso_from_elf = $(patsubst %.elf,%.o,$(1))
get_test_ooso_from_bin = $(patsubst %.bin,%.o,$(1))

# Takes a test phony rule (e.g. test_algo1) and get its associated test executable file
get_bin_from_phony_rule = $(patsubst ./%.cpp,$(BUILD_DIR)/%.bin,$(shell find -name $(patsubst %,%.cpp,$(1))))

# Optional files from the binary
get_elf_from_bin = $(patsubst %.bin,%.elf,$(1))
get_lst_from_bin = $(patsubst %.bin,%.lst,$(1))


# ========================================================
# Build Config Variables
# ========================================================
# Config files are stored along with the build files
# When make is called, those configs are checked and different configs will rebuild specific targets
CONFIG_DIR := $(BUILD_DIR)/config
CONFIG_TEST_NAME_FILEPATH := $(CONFIG_DIR)/test_name.txt

# Current build configuration
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ifneq ($(TEST_NAME),) # This config is ignored if we are not running a test
CONFIG_TEST_NAME := $(TEST_NAME)
endif

# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# Last build configuration
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# Configs below will only be checked if we built previously
# This means config files are available in the config dir

config_req_version := 4.2
support_config := $(filter $(config_req_version),$(firstword $(sort $(MAKE_VERSION) $(config_req_version))))

ifneq ($(support_config),)

ifneq ($(wildcard $(CONFIG_DIR)/*),)

LAST_CONFIG_TEST_NAME = $(file < $(CONFIG_TEST_NAME_FILEPATH))

# Comparing debug configs

# Comparing test names
ifneq ($(CONFIG_TEST_NAME), $(LAST_CONFIG_TEST_NAME))
$(shell $(RM) $(BUILD_DIR)/t8a_test.o)
endif

endif # ($(wildcard $(CONFIG_DIR)/*),)

endif # ifeq ($(support_config),)

# ========================================================
# Library Rules
# ========================================================

.PHONY: all

all: $(BUILD_DIR)/$(LIB_NAME)

# Archive the library from its obj files, out-of-source dir must be created
$(BUILD_DIR)/$(LIB_NAME): $(LIB_OBJS) save_config | $(BUILD_DIR)
	$(Q)$(AR) $(STATIC_LIB_FLAGS) $@ $(LIB_OBJS)

# Compilation rules
# Note: The 2 rules below produce the same targets, but make will only use the one that has
# its prerequisites met. If you have a .S and a .cpp with the same name in the same path,
# then and only then may this have adverse effects. The first rule (here the .cpp) will
# have precedence. This can be problematic as the .S will not be correctly built.

# C source
.SECONDEXPANSION:
%.o: $$(call get_src_from_ooso,$$@) | $$(@D)
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Create root out-of-source directory
$(BUILD_DIR):
	$(Q)mkdir -p $@

# Create out-of-source directories for lib's compiled sources
ANU_OOSD:=$(patsubst %/,%,$(addprefix $(BUILD_DIR)/,$(sort $(dir $(LIB_SRCS)))))
$(ANU_OOSD):
	$(Q)mkdir -p $@

# Dummy rule for every dependency
$(DEPFILES):

# ========================================================
# Test Rules
# ========================================================

.PHONY: $(PHONY_TEST_RULES)

# Generate the phony rules that will allow the user to call each test
# Those rules are simply the name of the tests, e.g. test_algo1
# Compiles the test, links with the lib, and runs the binary
.SECONDEXPANSION:
$(PHONY_TEST_RULES): $$(call get_bin_from_phony_rule,$$@) save_config
	@echo "Lauching: $<"
	$<

# Link the library, test main file and other test sources together
# clean_test is called before objects are built
# .SECONDEXPANSION:
# $(TEST_ELFS): $$(call get_test_ooso_from_elf,$$@) $$(TEST_OBJS) $$(BUILD_DIR)/$(LIB_NAME)
# 	$(Q)ld -o $@ $< $(TEST_OBJS)
# 	$(Q)$(CC) main.o other_file.o -L. -lmy_library -o $@ $< $(TEST_OBJS)

# Generate the executable binary from the linked elf file
.SECONDEXPANSION:
$(TEST_BINS): $$(call get_test_ooso_from_bin,$$@) $$(TEST_OBJS) $$(BUILD_DIR)/$(LIB_NAME)
	g++ $< $(TEST_OBJS) $(BUILD_DIR)/$(LIB_NAME) -o $@
# $(Q)objdump -x -D -l -S $< > "$(call get_lst_from_bin,$@)"
# $(Q)size -t $<
# $(Q)objcopy -O binary $< "$@"

# Create an out-of-source test/main.o from its test/main.cpp
.SECONDEXPANSION:
$(TEST_MAIN_OBJS):| $$(@D)
$(TEST_MAIN_OBJS): $$(call get_src_from_ooso,$$@)
	$(Q)$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Create out-of-source directories for test main files
# Filter-out old dir recipes to prevent overriding them
$(filter-out $(ANU_OOSD) $(BUILD_DIR), $(sort $(TEST_BUILD_DIRS))):
	$(Q)mkdir -p $@

# Create out-of-source directories for cstartup (more than one if using an sdk cstartup)
$(patsubst %/,%,$(addprefix $(BUILD_DIR)/,$(sort $(dir $(SDK_CTR0S))))):
	$(Q)mkdir -p $@

# ========================================================
# Support Rules
# ========================================================

.PHONY: clean debug help get_anu_paths save_config

# Clean the default build dir, if you archive/build in a specific project dir, use your own clean
clean:
	-$(Q)$(RM) -r $(DEFAULT_BUILD_DIR)


# Print all the -I compiler flags needed to compile your project code using the t8a library
# Only for the get_anu_paths rule, maybe shell has a way to evaluate this automatically
PATH_FROM_WD?=
# Will return an additional path -IPATH_FROM_WD if var is not empty (should not be when calling this rule)
get_anu_paths: I_PATHS := $(patsubst -I%, -I$(PATH_FROM_WD)/%, $(filter-out -I., $(I_PATHS)))
get_anu_paths: I_PATHS+=-I$(PATH_FROM_WD)
get_anu_paths: I_PATHS+=$(filter-out -I, $(I_PATHS)) #safety for empty -I
get_anu_paths:
	@echo "$(I_PATHS)"

ifeq ($(support_config),)
save_config:
	@echo "Config files functionality is not supported, Make version $(MAKE_VERSION) is too old"
else
# Create out-of-source directory for lib's config (how it was built in regard to passed flags)
$(CONFIG_DIR):
	$(Q)mkdir -p $@

# TODO: make a var that contains all configs instead of typing everyone
# Output current build's flags that have an effect on built objects, but are not tracked by dependency files
# These config files are then checked by future builds
save_config:| $(CONFIG_DIR) # Fun fact: cannot use $$(@D), it is a phony rule, does not have a dir!
	$(file >$(CONFIG_TEST_NAME_FILEPATH))
	$(file >>$(CONFIG_TEST_NAME_FILEPATH),$(CONFIG_TEST_NAME))

endif # ifeq ($(support_config),)

debug:
	@echo "CFLAGS=$(CFLAGS)"
	@echo "LIB_DIRS=$(ANU_DIRS)"
	@echo "LIB_SRCS=$(LIB_SRCS)"
	@echo "ANU_TEST_MAIN_SRCS=$(ANU_TEST_MAIN_SRCS)"
	@echo "TEST_MAIN_OBJS=$(TEST_MAIN_OBJS)"
	@echo "TEST_BUILD_DIRS=$(TEST_BUILD_DIRS)"
	@echo "PHONY_TEST_RULES=$(PHONY_TEST_RULES)"
	@echo "TEST_ELFS=$(TEST_ELFS)"
	@echo "BUILD_DIR=$(BUILD_DIR)"
	@echo "TEST_OBJS=$(TEST_OBJS)"


help:
	@echo "=== Parameters ==="
	@echo "VERBOSE - Adds compilation messages during build"
	@echo "PATH_TO_BUILD - where to send the compiled static lib and build artifacts."
	@echo "	Default is <algos_n_utils repo>/build/algos_n_utils"
	@echo "NEW_CFLAGS - Replace default compiler flags by your own"
	@echo "EXTRA_CFLAGS - Pass aditional compiler flags"
	@echo "COMPILER - Defaults to g++"
	@echo "DEPFLAGS - Dependency flags, defaults to generating deps for all sources."
	@echo "Can be set to nothing if a specified compiler does not support .d's"
	@echo "PATH_FROM_WD - Used with rule "make get_anu_paths", will add paths from wd so that"
	@echo "	-I flags have the proper paths from where it is called"
	@echo
	@echo "=== Rules ==="
	@echo "all (default) - builds $(LIB_NAME)"
	@echo "get_anu_paths - returns the -I flags needed to compile this lib. see PATH_FROM_WD param"
	@echo "	if paths need to be relative to another directory"
	@echo
	@echo "=== Test Parameters and Rules ==="
	@echo "test_* - builds, flashes and runs test for specified unit. e.g. make test_sleep_modes"

# Dependency files must be included, usually keep this at the bottom of a Makefile
include $(wildcard $(DEPFILES))