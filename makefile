TARGET_EXEC := libUISystem.a

BUILD_DIR := ./Build
SRC_DIRS := ./Source ./Headers

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(SRC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -DGLEW_STATIC -MMD -MP -O2 -Wdelete-incomplete -std=c++2a `sdl2-config --cflags --libs`

# Package all C++ object files into a static library
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	ar rcs $(BUILD_DIR)/$(TARGET_EXEC) $(OBJS)

# Build C++ object files
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD_DIR)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
