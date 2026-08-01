# =============================================================================
#  DriveSim - Makefile
#  Build:   make            (produces ./drivesim)
#  Run:     make run
#  Clean:   make clean
# =============================================================================

CXX      := g++
CXXSTD   := -std=c++17
WARN     := -Wall -Wextra -Wpedantic
CXXFLAGS := $(CXXSTD) $(WARN) -O2
LDFLAGS  :=

SRC_DIR  := src
BUILD_DIR:= build
TARGET   := drivesim

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
