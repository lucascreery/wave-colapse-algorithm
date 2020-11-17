CC = g++

MAIN = wfc

CFLAGS = -g

INCLUDES = -I./include

LFLAGS = -L./lib

LIBS = -lsfml-system -lsfml-graphics -lsfml-window

SRCS = wfc.cpp lib/pattern.cpp lib/wave.cpp

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(SRCS) $(LFLAGS) $(LIBS)

clean:
	$(RM) *.o *~ $(TARGET)

depend: $(SRCS)
	makedepend $(INCLUDES) $^