#####################################################################
# Created by Avra Mitra											    #
# Created on Apr 30, 2020										    #
# Last modified on April 24, 2022									#
#####################################################################

# Project name
PROJ_NAME=LameUI_simulator
# Executable name
TARGET=big_example

######################################################################
#                         SETUP SOURCES                              #
######################################################################

#------------ [SOURCE DIRECTORIES] -----------------------------------
# User source directory
SRCDIR_USER	 	 =  ./
RESDIR_USER		 =	./res/
# LameUI source directory
SRCDIR_LAMEUI	 =  ../../LameUI/
# Objects directory
OBJDIR		 =  ../../obj/
# Target directory
BINDIR		 =  ../../bin/
#--------------------------------------------------------------------

#------------ [C/CPP SOURCE FILES] ----------------------------------
# User source files (c/cpp)
SRCS   	 	 =  big_example.c
SRCS		+=  grad.c
SRCS		+=  grad_color1_bg.c

# LameUI source file
SRCS	 	+=  lame_ui.c
# Extra fonts (in LameUI/fonts directory)
SRCS		+=  montserrat_regular_32.c
SRCS		+=  montserrat_regular_48.c
SRCS		+=  ubuntu_regular_32.c
SRCS		+=  ubuntu_regular_20.c
#--------------------------------------------------------------------

#------------ [COMPILED OBJECT FILES] -------------------------------
# Object file names for user source files (source_name.o)
OBJ			 =  $(SRCS:.c=.o)
#--------------------------------------------------------------------

#------------ [INCLUDE DIRECTORIES] ---------------------------------
# Includes for user files(headers)
INCLS		 =  -I.
INCLS		+=  -Ires
# Include for lameui files
INCLS 		+=  -I../../LameUI/
# Includes for Font and bitmap related files
INCLS 		+=  -I../../LameUI/fonts/
#--------------------------------------------------------------------

#------------ [USED LIBRARIES] --------------------------------------
# Libraries used for linking
LIBS  	 	 =  -lGL
LIBS  		+=  -lGLU
LIBS  		+=  -lglut
LIBS  		+=  -lm

LIBPATH		 =  #-Llibs
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
CC			 =  gcc #tcc
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


# Rule to make all and then run it
.PHONY: run
run: $(TARGET_MAIN)
	$(TARGET_MAIN)


# Rule to clean user application (.o and executable) and LameUI (.a, .o)
.PHONY: clean
clean:
		@echo [Cleaning user application...]
		rm -f $(OBJ_MAIN) $(TARGET_MAIN)


# NOTE: If both user application and lameUI (or other lib) has files with same name, wrong file may be compiled in wrong place
#       In such case, instead of using pattern rule, use exact file name

# Rule to make object files of user c/cpp (in src folder) by compiling source file
$(OBJDIR)%.o: $(SRCDIR_USER)%.c
		@echo [Compiling user code _src_...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)
$(OBJDIR)%.o: $(RESDIR_USER)%.c
		@echo [Compiling user code _res_...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)


# Rule to make object files of lameui
$(OBJDIR)%.o: $(SRCDIR_LAMEUI)%.c  
		@echo [Compiling LameUI...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)

# Rule to make object files of lameui fonts (font c files must be in LameUI/fonts/)
$(OBJDIR)%.o: $(SRCDIR_LAMEUI)/fonts/%.c
		@echo [Compiling LameUI Fonts...]
		$(CC) -c -o $@ $< $(INCLS) $(CFLAGS)


# Rule to make target executable by linking object files and libraries
$(TARGET_MAIN): $(OBJ_MAIN)
		@echo [Linking all objects...]
		$(CC) -o $(TARGET_MAIN) $^ $(LIBPATH) $(LIBS)



# Rule to fireup gdb for debugging the executable binary
.PHONY: debug
debug:
		$(DEBUG) $(TARGET_MAIN)

