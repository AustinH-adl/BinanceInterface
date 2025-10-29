TARGET       := BinanceInterface
SRC_DIR      := src
INC_DIR      := include
OBJ_DIR      := build/obj

CXX          := g++
CXXFLAGS     := -std=c++17 -Wall -Wextra -I$(INC_DIR)
LIBS         := -lcurl -lssl -lcrypto -lboost_system -lpthread

SRC_FILES := $(shell find $(SRC_DIR) -name "*.cpp")
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

$(TARGET): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	@echo "Linking $(TARGET)"
	$(CXX) $(OBJ_FILES) -o $(TARGET) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all run clean

all: $(TARGET)

run: $(TARGET)
	@echo "Running $(TARGET)..."
	./$(TARGET)

clean:
	@echo "Cleaning build directory..."
	rm -rf build