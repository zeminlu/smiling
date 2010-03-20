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
CFLAGS = -O -Wall -Wuninitialized -pedantic -errors -ansi -fno-builtin -g -c -o
#Sets the location of the header files.
INCLUDES = -I $(INC_DIR)
#Set the libraries variables
LIBRARIES = -lxml2
#Sets the C compiler set up
COMPILE.c = $(CC) $(INCLUDES) $(CFLAGS) 
#
#Sets the linker for the project
LD = gcc
#
LDFLAGS = $(LIBRARIES) -g -o
#
TARGET1 = main.bin
OBJECTS1 = main.o
TARGET2 = parallel.bin
OBJECTS2 = parallel.o tpl.o serializable.o linearHashADT.o msqIPC.o
TARGET3 = pipe.bin
OBJECTS3 = pipe.o tpl.o serializable.o linearHashADT.o msqIPC.o
TARGET4 = fifaGen.bin
OBJECTS4 = fifaGen.o conditions.o
TARGET5 = fifa.bin
OBJECTS5 = fifa.o tpl.o serializable.o linearHashADT.o msqIPC.o
TARGET6 = grouph.bin
OBJECTS6 = grouph.o tpl.o conditions.o serializable.o linearHashADT.o msqIPC.o
TARGET7 = gates.bin
OBJECTS7 = gates.o tpl.o serializable.o msqIPC.o linearHashADT.o
TARGET8 = levels.bin
OBJECTS8 = levels.o msqIPC.o linearHashADT.o
###############################################################################
.SILENT:
.PHONY: clean

all: clean $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6) $(TARGET7) $(TARGET8)

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

$(TARGET6): $(OBJECTS6)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."

$(TARGET7): $(OBJECTS7)
	@echo "Linking" $@"..."
	$(LD) $(LDFLAGS) $(OUTPUT_DIR)$@ $^ 
	@echo "Done."

$(TARGET8): $(OBJECTS8)
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
fifaGen.o: fifaGen.c fifaGen.h
fifa.o: fifa.c fifa.h
conditions.o: conditions.c conditions.h
grouph.o: grouph.c grouph.h
gates.o: gates.c gates.h
tpl.o: tpl.c tpl.h
definitions.o: definitions.c definitions.h
serializable.o: serializable.c serializable.h
pipeIPC.o: pipeIPC.c ipcAPI.h
levels.o: levels.c levels.h
linearHashADT.o: linearHashADT.c linearHashADT.h
msqIPC.o: msqIPC.c msqIPC.h

cleanObjects:
	@echo "Clearing Objects..."
	@rm -f *.o
	@echo "Done."
clean:
	@echo "Clearing" $(OUTPUT_DIR) "directory..."
	@rm -f *.o
	@rm -f $(OUTPUT_DIR)*.bin
	@echo "Done."
