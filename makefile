CFLAGS=-Wall -ansi 


TARGET1=main
TARGET2=pipe
TARGET3=parallel
OBJS1=main.o
OBJS2=pipe.o
OBJS3=parallel.o 

$(TARGET1):	$(TARGET1).c $(OBJS1)
	cc $(CFLAGS) -o ./bin/$(TARGET1) ./src/$(TARGET1).c $(OBJS1)

$(TARGET2):	$(TARGET2).c $(OBJS2)
	cc $(CFLAGS) -o $(TARGET2) $(TARGET2).c $(OBJS2)

$(TARGET3):	$(TARGET3).c $(OBJS3)
	cc $(CFLAGS) -c $(TARGET3).c $(OBJS3)
