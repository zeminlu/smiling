############################### Variables ######################################
#
#Sets working directories
INC_DIR = ./inc/
SRC_DIR = ./src/
OUTPUT_DIR = ./bin/
#
#- Global Variables
#Sets the location of the header files for make
vpath %.h $(INC_DIR)
#Set the location of the source files for make
vpath %.c $(SRC_DIR)
#
#
#- Compiler
#Sets the C compiler used to create object files from source code.
CC = gcc
#Sets compiler options.
CFLAGS = -O -Wall -Wuninitialized -pedantic -errors -ansi -fno-builtin -c -o
#Sets the location of the header files.
INCLUDES = -I $(INC_DIR)
#Sets the C compiler set up
COMPILE.c = $(CC) $(INCLUDES) $(CFLAGS) 
#
#Sets the linker for the project
LD = gcc
#
LDFLAGS = -O -Wall -Wuninitialized -pedantic -fno-builtin -o
#
TARGET1 = main.bin
OBJECTS1 = main.o
TARGET2 = parallel.bin
OBJECTS2 = parallel.o
TARGET3 = pipe.bin
OBJECTS3 = pipe.o
TARGET4 = fifaGen.bin
OBJECTS4 = fifaGen.o conditions.o
TARGET5 = luGen.bin
OBJECTS5 = luGen.o 
###############################################################################
.SILENT:
.PHONY: clean

all: clean $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) cleanObjects

$(TARGET1): $(OBJECTS1)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."
	
$(TARGET2): $(OBJECTS2)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."
	
$(TARGET3): $(OBJECTS3)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."
	
$(TARGET4): $(OBJECTS4)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^
	@echo "Done."
	
$(TARGET5): $(OBJECTS5)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."

%.o: %.c
	@echo "Compiling" $< "into" $@...
	$(COMPILE.c) $@ $<
	@echo "Done."

main.o: main.c main.h
pipe.o: pipe.c pipe.h 
parallel.o: parallel.c parallel.h
fifaGen.o: fifaGen.c fifaGen.h types.h conditions.h
luGen.o: luGen.c luGen.h
conditions.o: conditions.c conditions.h types.h

cleanObjects:
	@echo "Clearing Objects..."
	@rm -f *.o
	@echo "Done."
clean:
	@echo "Clearing" $(OUTPUT_DIR) "directory..."
	@rm -f *.o
	@rm -f $(OUTPUT_DIR)*.bin
	@echo "Done."
