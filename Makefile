# linux makefile, graciously provided by strtok (https://github.com/taurreco on GitHub)
# you need to run as 'make all', plain 'make' won't build it properly

ifeq ($(DEBUG),0)
CFLAGS = -Oz -ffast-math -flto
LIBS += -lc
else
CFLAGS = -g
LIBS += -lc-dbg
endif

CC = clang --target=wasm32 --sysroot=../../wasmlite/libc -nodefaultlibs
LIBS += -lm -Wl,--export-table -Wl,--allow-undefined # allow undefined needed bc of the wasm-ld calls?
# LIBS += -Wl,--error-limit=0

BIN_DIR = bin

EXECS = main
OUT = PL3D-KC.wasm
# EXECS_DEPS: $(BIN_DIR)/libpl.o $(BIN_DIR)/libfw.o

LIBFW = $(BIN_DIR)/libfw.o
LIBPL = $(BIN_DIR)/libpl.o

LIBFW_DEPS = $(addprefix $(BIN_DIR)/, pkb.o sys.o wvid.o xvid.o hvid.o)
LIBPL_DEPS = $(addprefix $(BIN_DIR)/, clip.o gfx.o imode.o importer.o math.o pl.o)

all: $(BIN_DIR) $(EXECS)

$(BIN_DIR):
	echo "[make] making bin directory"
	mkdir -p $@

$(LIBFW_DEPS): $(BIN_DIR)/%.o: fw/%.c fw/fw.h
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBPL_DEPS): $(BIN_DIR)/%.o: %.c pl.h
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBFW): $(LIBFW_DEPS)
	wasm-ld -r -o $@ $(LIBFW_DEPS)

$(LIBPL): $(LIBPL_DEPS)
	wasm-ld -r -o $@ $(LIBPL_DEPS)

$(EXECS): %: %.c pl.h fw/fw.h $(LIBPL) $(LIBFW)
	$(CC) $(CFLAGS) $< -o $(OUT) $(LIBPL) $(LIBFW) $(LIBS)
ifeq ($(DEBUG),0)
	wasm-strip $(OUT) && wasm-opt $(OUT) -o $(OUT) -Oz --enable-sign-ext
else
	../../emscripten/tools/wasm-sourcemap.py $(OUT) -w $(OUT) -p $(CURDIR) -s -u ./$(OUT).map -o $(OUT).map --dwarfdump=/usr/bin/llvm-dwarfdump
endif
	python3 -m http.server


clean:
	echo "[make] deleting bin directory"
	rm -r $(BIN_DIR)
	echo "[make] deleting executables"
	for exec in $(EXECS) ; do \
	rm $$exec ; \
	done
