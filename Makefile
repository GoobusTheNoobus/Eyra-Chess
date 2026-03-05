CXX = g++
CXXFLAGS = -std=c++20 -O3 -march=native -mbmi2 \
           -fno-rtti \
           -DNDEBUG \
           -funroll-loops \
           -fno-semantic-interposition \
           -flto \
           -fomit-frame-pointer 

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

TARGET = eyra

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)