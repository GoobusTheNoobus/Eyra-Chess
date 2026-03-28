CXX = g++
CXXFLAGS = -std=c++20 -g -O3 -march=native -mbmi2 \
           -fno-rtti \
           -DNDEBUG \
           -funroll-loops \
           -fno-semantic-interposition \
           -flto \
           -fomit-frame-pointer \
           -Isrc \
           

SRC = $(wildcard src/*.cpp) \
      $(wildcard src/*/*.cpp) \
      $(wildcard src/*/*/*.cpp) \
      $(wildcard src/*/*/*/*.cpp)

OBJ = $(patsubst src/%.cpp, bin/%.o, $(SRC))

TARGET = eyra

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

bin/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf bin $(TARGET)