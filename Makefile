
# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

###############################################################################
# Directories

ROOT_DIR		= ./
SRC_DIR			= source
INC_DIR			= include
OBJ_DIR			= obj

BINARY_NAME		= mbuddy

###############################################################################
# Basic Device Setup

#DEFS			+= -D
ARFLAGS 		= rcs
ARCH_FLAGS		= 

###############################################################################
# Linkerscript

#LDSCRIPT		=
LDLIBS			+= -lncurses
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
CSTD			?= -std=c99


###############################################################################
# Source files


OBJS			+= $(BINARY_NAME).o


###############################################################################
# C flags

TGT_CFLAGS		+= $(OPT) $(CSTD) $(DEBUG)
TGT_CFLAGS		+= $(ARCH_FLAGS)
TGT_CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
TGT_CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
TGT_CFLAGS		+= -fno-common -ffunction-sections -fdata-sections

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

all: $(BINARY_NAME) 

GENERATED_BINARIES = $(BINARY_NAME)

# Define a helper macro for debugging make errors online
# you can type "make print-OPENCM3_DIR" and it will show you
# how that ended up being resolved by all of the included
# makefiles.

print-%:
	$(Q)echo $*=$($*)

$(BINARY_NAME): $(OBJ_DIR)/$(OBJS)
	$(Q)printf "  LD		$(OBJ_DIR)/$(OBJS)\n"
	$(Q)$(LD) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJ_DIR)/$(OBJS) $(LDLIBS) -o $(BINARY_NAME)

$(OBJ_DIR)/$(OBJS): $(SRC_DIR)/$(OBJS)
	$(Q)mv $(SRC_DIR)/*.o $(OBJ_DIR)/
	$(Q)mv $(SRC_DIR)/*.d $(OBJ_DIR)/

#$(OBJS): $(SRC_DIR)/%.o

%.o: %.c
	$(Q)printf "  CC      $(*).c\n"
	$(Q)$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o: %.cxx
	$(Q)printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cxx

%.o: %.cpp
	$(Q)printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

clean:
	@#printf "  CLEAN\n"
	$(Q)$(RM) $(GENERATED_BINARIES) generated.* $(OBJ_DIR)/$(OBJS) $(OBJ_DIR)/$(OBJS:%.o=%.d)


.PHONY: images clean elf bin hex srec list

-include $(OBJS:.o=.d)
