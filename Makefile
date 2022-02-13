SRC_DIR = src
INC_DIR = $(SRC_DIR)/include

# directories
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj

CC = g++
CXXFLAGS = -Wall
CXXFLAGS = -DDBG_CODE
CXXFLAGS += -I $(INC_DIR)
CXXFLAGS += -I include/cxxopts
CXXFLAGS += -I include/elfio
CXXFLAGS += -I include/nlohmann_json
LDFLAGS = 

EXECUTABLE = rvsim
CSRCS = main.cpp memory.cpp util.cpp
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CSRCS))
SRCS = $(patsubst %,$(SRC_DIR)/%,$(CSRCS))

all: default
default: sim

directories: $(BUILD_DIR) $(BIN_DIR) $(OBJ_DIR)

$(BUILD_DIR):
	mkdir $@

# create bin directory
$(BIN_DIR): $(BUILD_DIR)
	mkdir $@

# create obj directory
$(OBJ_DIR): $(BUILD_DIR)
	mkdir $@


.PHONY: clean
clean: 
	rm -rf $(BIN_DIR)/*
	rm -rf $(OBJ_DIR)/*

.PHONY: super-clean
super-clean:
	rm -rf $(BUILD_DIR)


# build simulator
.PHONY: sim
sim: directories $(BIN_DIR)/$(EXECUTABLE)

# link
$(BIN_DIR)/$(EXECUTABLE): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

# Compile all cpp files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CXXFLAGS) $< -o $@

