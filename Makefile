CXX = g++

TARGET = flappy-bird

SOURCES = \
	main.cpp \
	Game.cpp \
	Bird.cpp \
	PipeSystem.cpp \
	HighScore.cpp

OBJECTS = $(SOURCES:.cpp=.o)

CXXFLAGS = -std=c++20 -Wall -Wextra -O2 $(shell pkg-config --cflags sfml-all)
LDFLAGS = $(shell pkg-config --libs sfml-all)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET) flappy-bird.exe