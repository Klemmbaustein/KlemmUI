TARGET_EXEC := libKlemmUI.a

BUILD_DIR := ./Build
SRC_DIRS := ./Source ./Include

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_DIRS := $(SRC_DIRS)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
SDL2_PATH := $(shell ldconfig -p | grep libSDL2)

CPPFLAGS := $(INC_FLAGS) -DGLEW_STATIC -MMD -MP -Wdelete-incomplete -std=c++2a `sdl2-config --cflags --libs`

EXAMPLE_ARGS = -std=c++2a -lKlemmUI -l:libGLEW.a -lGL `sdl2-config --cflags --libs`

libKlemmUI: $(BUILD_DIR)/$(TARGET_EXEC)

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

ifeq ($(PREFIX),)
PREFIX = /usr/local
endif

.PHONY: configure
configure:
ifeq ($(SDL2_PATH),)
	cd SDL;\
	./configure;\
	cmake -S . -B Build;\
	cd Build/;\
	make SDL2;\
	sudo make install
endif
	cd glew-cmake;\
	make;\
	sudo make install

install: $(BUILD_DIR)/libKlemmUI.a
	$(info Installing to $(PREFIX))
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(BUILD_DIR)/libKlemmUI.a $(DESTDIR)$(PREFIX)/lib/
	install -d $(DESTDIR)$(PREFIX)/include/KlemmUI/
	cp -r ./Include/KlemmUI/. $(DESTDIR)$(PREFIX)/include/KlemmUI/

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
