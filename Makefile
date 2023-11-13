
# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

###############################################################################
# Directories

ROOT_DIR		= ./
INSTALL_DIR		= /usr/bin/
SRC_DIR			= source
INC_DIR			= include
OBJ_DIR			= obj
BIN_DIR			= bin
TEST_DIR		= test

BINARY_NAME		= mbuddy

###############################################################################
# Basic Device Setup

DEFS			+= -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600
ARFLAGS 		= rcs
ARCH_FLAGS		= 

###############################################################################
# Linkerscript

#LDSCRIPT		=
LDLIBS			+= -lncursesw -ltinfo
#LDFLAGS		+= -L

###############################################################################
# Includes

DEFS			+= -I$(INC_DIR)

###############################################################################
# Executables

#PREFIX		?= arm-none-eabi-

CC				:= $(PREFIX)gcc
CXX				:= $(PREFIX)g++
LD				:= $(PREFIX)gcc
AR				:= $(PREFIX)ar
AS				:= $(PREFIX)as
OBJCOPY			:= $(PREFIX)objcopy
OBJDUMP			:= $(PREFIX)objdump
#GDB			:= $(PREFIX)gdb
#STFLASH		= $(shell which st-flash)
OPT				:= -Os
#DEBUG			:= -ggdb3
CSTD			?= -std=gnu99


###############################################################################
# Source files

SRCS			+= $(SRC_DIR)/$(BINARY_NAME).c
SRCS			+= $(SRC_DIR)/nix_serial.c
SRCS			+= $(SRC_DIR)/gui.c

OBJS 			+= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

SERIALTEST_OBJS	+= $(OBJ_DIR)/serialtest.o
SERIALTEST_OBJS	+= $(OBJ_DIR)/nix_serial.o

###############################################################################
# C flags

TGT_CFLAGS		+= $(OPT) $(CSTD) $(DEBUG)
TGT_CFLAGS		+= $(ARCH_FLAGS)
TGT_CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS		+= -fno-common -ffunction-sections -fdata-sections
TGT_CFLAGS		+= -Wno-unused-value -Wno-unused-parameter -Wno-unused-function

###############################################################################
# C++ flags

TGT_CXXFLAGS	+= $(OPT) $(CXXSTD) $(DEBUG)
TGT_CXXFLAGS	+= $(ARCH_FLAGS)
TGT_CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
TGT_CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

TGT_CPPFLAGS	+= -MD
TGT_CPPFLAGS	+= -Wall -Wundef
TGT_CPPFLAGS	+= $(DEFS)

###############################################################################
# Used libraries

LDLIBS			+= -Wl,--start-group -lc -lgcc  -Wl,--end-group

###############################################################################
###############################################################################
###############################################################################
.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: $(BIN_DIR)/$(BINARY_NAME)
	$(Q)echo "\nBuild complete\n\tout: $(BIN_DIR)/$(BINARY_NAME)\n"

install: $(BIN_DIR)/$(BINARY_NAME)
	$(Q)cp $(BIN_DIR)/$(BINARY_NAME) $(INSTALL_DIR)
	$(Q)echo "\nInstall complete, run with the command $(BINARY_NAME)\n"

debug: DEFS += -DDEBUG
debug: all
	$(Q)echo "\nBuild completed in DEBUG mode\n"

serialtest: $(BIN_DIR)/serialtest

$(BIN_DIR)/serialtest:
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(OBJ_DIR)/serialtest.o -c $(TEST_DIR)/serialtest.c
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(OBJ_DIR)/nix_serial.o -c $(SRC_DIR)/nix_serial.c
	$(Q)$(CC) $(SERIALTEST_OBJS) -o $(BIN_DIR)/serialtest


GENERATED_BINARIES = $(BIN_DIR)/$(BINARY_NAME) $(BIN_DIR)/serialtest

# Define a helper macro for debugging make errors online
# you can type "make print-OPENCM3_DIR" and it will show you
# how that ended up being resolved by all of the included
# makefiles.

print-%:
	$(Q)echo $*=$($*)

$(BIN_DIR)/$(BINARY_NAME): $(OBJS)
	$(Q)#printf "  LD	  $(OBJS)\n"
	$(Q)$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(BIN_DIR)/$(BINARY_NAME)

$(OBJS): $(SRCS)
	$(Q)#printf "  CC      $(*:obj/%=source/%).c\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*:obj/%=source/%).c

%.o: %.c
	$(Q)#printf "  CC      $(*).c\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o: %.cxx
	$(Q)#printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cxx

%.o: %.cpp
	$(Q)#printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

clean:
	$(Q)#printf "  CLEAN\n"
	$(Q)$(RM) $(GENERATED_BINARIES) $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d


.PHONY: install debug images clean elf bin hex srec list

-include $(OBJS:.o=.d)
