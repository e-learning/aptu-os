CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		04.o

LIBS =

TARGET =	04

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
