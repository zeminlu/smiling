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
CFLAGS = -O -Wall -Wuninitialized -pedantic -fno-builtin -c -o
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

all: clean $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5)

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
	$(LD) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."

%.o: %.c
	@echo "Compiling" $< "into" $@...
	$(COMPILE.c) $@ $<
	@echo "Done."

main.o: main.c
pipe.o: pipe.c
parallel.o: parallel.c
fifaGen.o: fifaGen.c types.h conditions.h
luGen.o: luGen.c
conditions.o: conditions.c types.h

clean:
	@echo "Clearing" $(OUTPUT_DIR) "directory..."
	@rm -f *.o
	@rm -f $(OUTPUT_DIR)*.bin
	@echo "Done."
