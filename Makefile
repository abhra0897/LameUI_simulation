#####################################################################
# Created by Avra Mitra											    #
# Created on Apr 30, 2020										    #
# Last modified on May 2, 2020										#
#####################################################################

# Project name
PROJ_NAME=Sim_LameUI
# Executable name
TARGET=Sim

######################################################################
#                         SETUP SOURCES                              #
######################################################################

#------------ [SOURCE DIRECTORIES] -----------------------------------
# User source directory
SRCDIR_USER	 	 =  src/
# LameUI source directory
SRCDIR_LAMEUI	 =  LameUI/src/
# Objects directory
OBJDIR		 =  obj/
# Target directory
BINDIR		 =  bin/
#--------------------------------------------------------------------

#------------ [C/CPP SOURCE FILES] ----------------------------------
# User source files (c/cpp)
SRCS   	 	 =  main.c
# LameUI source file
SRCS	 	+=  lame_ui.c
#--------------------------------------------------------------------

#------------ [COMPILED OBJECT FILES] -------------------------------
# Object file names for user source files (source_name.o)
OBJ			 =  $(SRCS:.c=.o)
#--------------------------------------------------------------------

#------------ [INCLUDE DIRECTORIES] ---------------------------------
# Includes for user files(headers)
INCLS		 =  -Iinc
# Include for lameui files
INCLS 		+=  -ILameUI/inc
# Includes for Font and bitmap related files
INCLS 		+=  -IFontsEmbedded
#--------------------------------------------------------------------

#------------ [USED LIBRARIES] --------------------------------------
# Libraries used for linking
LIBS  	 	 =  -lGL
LIBS  		+=  -lGLU
LIBS  		+=  -lglut
LIBS  		+=  -lm

LIBPATH		 =  -L.
#--------------------------------------------------------------------

#------------ [DON'T EDIT BELOW PART] -------------------------------
# Add the directory with the file names
OBJ_MAIN	 =  $(addprefix $(OBJDIR),$(OBJ))
TARGET_MAIN	 =  $(addprefix $(BINDIR),$(TARGET))
#--------------------------------------------------------------------

######################################################################
#                         SETUP TOOLS                                #
######################################################################

# Cross compiler information
CC			 =  gcc
DEBUG		 =  gdb

# Compiler flags used to create .o files from .c/cpp files
CFLAGS  	 =   -g
CFLAGS  	+=  -O0
CFLAGS  	+=  -Wall -Wextra -Warray-bounds


######################################################################
#                         SETUP TARGETS                              #
######################################################################

# Rule to make all
.PHONY: all
all: $(TARGET_MAIN)



# Rule to build lameui library using its own makefile
.PHONY: make_lameui
make_lameui:
		cd LameUI && $(MAKE) all



# Rule to clean user application (.o and executable) and LameUI (.a, .o)
.PHONY: clean
clean:
		@echo [Cleaning user application...]
		rm -f $(OBJ_MAIN) $(TARGET_MAIN)


# NOTE: If both user application and lameUI (or other lib) has files with same name, wrong file may be compiled in wrong place
#       In such case, instead of using pattern rule, use exact file name

# Rule to make object files of user c/cpp (in src folder) by compiling source file
$(OBJDIR)%.o: $(SRCDIR_USER)%.c
		@echo [Compiling user code...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)


# Rule to make object files of lameui
$(OBJDIR)%.o: $(SRCDIR_LAMEUI)%.c
		@echo [Compiling LameUI...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)


# Rule to make target executable by linking object files and libraries
$(TARGET_MAIN): $(OBJ_MAIN)
		@echo [Linking all objects...]
		$(CC) -o $(TARGET_MAIN) $^ $(LIBS)



# Rule to fireup gdb for debugging the executable binary
.PHONY: debug
debug:
		$(DEBUG) $(TARGET_MAIN)

