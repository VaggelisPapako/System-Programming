# Compiler options
CXX := g++
CXXFLAGS := -g -Wall -Wextra
LDFLAGS := -pthread

# Directories
LIB_DIR := lib
SRC_DIR := src
POLLER_DIR := $(SRC_DIR)/poller
POLL_SWAYER_DIR := $(SRC_DIR)/pollSwayer
BUILD_DIR := build

# Files
LIB_SOURCES := $(wildcard $(LIB_DIR)/*.cpp)
LIB_HEADERS := $(wildcard $(LIB_DIR)/*.hpp)
LIB_OBJECTS := $(patsubst $(LIB_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(LIB_SOURCES))

POLLER_SOURCES := $(wildcard $(POLLER_DIR)/*.cpp)
POLLER_HEADERS := $(wildcard $(POLLER_DIR)/*.hpp)
POLLER_OBJECTS := $(patsubst $(POLLER_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(POLLER_SOURCES))

POLL_SWAYER_SOURCES := $(wildcard $(POLL_SWAYER_DIR)/*.cpp)
POLL_SWAYER_HEADERS := $(wildcard $(POLL_SWAYER_DIR)/*.hpp)
POLL_SWAYER_OBJECTS := $(patsubst $(POLL_SWAYER_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(POLL_SWAYER_SOURCES))

# Targets
.PHONY: all clean

all: $(BUILD_DIR)/poller $(BUILD_DIR)/pollSwayer

$(BUILD_DIR)/poller: $(LIB_OBJECTS) $(POLLER_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/pollSwayer: $(LIB_OBJECTS) $(POLL_SWAYER_OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.cpp $(LIB_HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(POLLER_DIR)/%.cpp $(POLLER_HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(POLL_SWAYER_DIR)/%.cpp $(POLL_SWAYER_HEADERS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) -r $(BUILD_DIR)