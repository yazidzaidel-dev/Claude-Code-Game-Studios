CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter
LDFLAGS  := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

INCLUDES := -I/usr/local/include -I/usr/include
LIBDIRS  := -L/usr/local/lib -L/usr/lib/aarch64-linux-gnu

TARGET   := ccgs_shooter
SRCS     := main.cpp game.cpp player.cpp enemy.cpp level.cpp hud.cpp audio.cpp minimap.cpp touch_controls.cpp
OBJS     := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBDIRS) $^ -o $@ $(LDFLAGS)
	@echo "Build complete: ./$(TARGET)"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
