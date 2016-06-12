# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -pthread flag will call the pthread library for the linking
CFLAGS  = -g -Wall -pthread
  
OBJS = main.o Debug.o Socket.o tftpServerProtocol.o

RM = rm -f

# Creating the  executable
ttftps: $(OBJS)
	$(CC) $(CFLAGS) -o ttftps $(OBJS)

# Creating the object files
main.o: main.c Debug.h main.h Socket.h tftpServerProtocol.h

Debug.o: Debug.c Debug.h

Socket.o: Socket.c Socket.h Debug.h 

tftpServerProtocol.o: Socket.c tftpServerProtocol.h Debug.h 

# Cleaning old files before new make
clean:
	$(RM) ttftps *.o *~ "#"* core.*
