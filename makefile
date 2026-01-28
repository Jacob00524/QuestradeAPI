CC = gcc
MODE ?= release

TARGET = libquestrade.so
EXAMPLE_TARGET = example.out
BUILD_FOLDER = build

CPPFLAGS = -Iexternal/curlwrapper/include -Iexternal/cJSON -Iinclude
LDFLAGS = -shared -Wl,-soname,$(TARGET)
DEBUG_CFLAGS   = -g3 -O0 -Wall -Wextra -fPIC -fsanitize=address,undefined
RELEASE_CFLAGS = -O4 -Wall -Wextra -fPIC

CURLWRAPPER_SO = external/curlwrapper/libcurlwrapper.so
CJSON_SO = external/cJSON/libcjson.so

ifeq ($(MODE),debug)
  CFLAGS = $(DEBUG_CFLAGS)
else
  CFLAGS = $(RELEASE_CFLAGS)
endif

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c, $(BUILD_FOLDER)/%.o, $(SRC))

default: $(TARGET) $(EXAMPLE_TARGET)

$(TARGET): $(OBJ) $(CURLWRAPPER_SO) $(CJSON_SO)
	$(CC) $(OBJ) -o $(BUILD_FOLDER)/$@ $(LDFLAGS) \
	-L. -lcurlwrapper -lcjson \
	-Wl,-rpath,'$$ORIGIN'
	cp $(BUILD_FOLDER)/$(TARGET) .

$(CURLWRAPPER_SO):
	$(MAKE) -C external/curlwrapper MODE=$(MODE)
	cp $(CURLWRAPPER_SO) .

$(CJSON_SO):
	git submodule update --init --recursive
	$(MAKE) -C external/cJSON
	cp -L $(CJSON_SO) .

$(BUILD_FOLDER)/%.o: src/%.c | $(BUILD_FOLDER)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_FOLDER):
	mkdir $@

$(EXAMPLE_TARGET): example.c $(TARGET)
	$(CC) -g3 -O0 -Wall -Wextra -fsanitize=address,undefined \
	-Iinclude $< -o $@ \
	-L. -lquestrade \
	-Wl,-rpath,\$$ORIGIN

clean:
	$(MAKE) -C external/curlwrapper clean
	$(MAKE) -C external/cJSON clean
	rm -rf $(BUILD_FOLDER)
	rm -f libcurlwrapper.so
	rm -f libquestrade.so
	rm -f libcjson.so
	rm -f $(EXAMPLE_TARGET)
