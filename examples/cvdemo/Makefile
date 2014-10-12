CC=g++
CXXFLAGS=-O2 -Wall
LDFLAGS=-lopencv_core -lopencv_imgproc -lopencv_highgui
INCLUDES=-I/usr/include/opencv -I../../API

SOURCES=../../API/dimage.cpp \
		../../API/cube.cpp \
		../../API/face.cpp

OBJECTS=$(patsubst ../../API/%.cpp,%.o,$(SOURCES))
OBJECTS+=main.o
EXECUTABLE=dice

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

main.o: main.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: ../../API/%.cpp
	$(CC) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
