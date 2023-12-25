#
# Portable Makefile
# 
# Main platform differences and important points
#
# On Windows, signature of the 'main()' function matters - it has to be like
# this:
# int main(int argc, char *argv[])
# Also when linking manually and not using `sdl-config --cflags --libs` you
# need to pass in some extra linker flags:
# -lmingw32 -lSDLmain -lSDL -mwindows
# When packaging you need to include mingw32 and SDL .dll runtime libraries
# in the game's directory.
# 
# On Linux make sure to enable commented out sections for much improved
# programming experience.
#
# In general, the best development experience is on Linux.
# 
# Other instructions
#
# a) Compiling for different platforms
# Select the corresponding PLATFORM, this will select all correct compiler and
# linker flags.
#
# b) Debug vs Release builds
# When developing and debugging always set TARGET to DEVEL - this will enable
# elevated compiler warning flags and add debug info to the binary.
#
# RELEASE is for the final binary release. It is also a good practice to strip
# the resulting binaries using: 'strip--strip-unneeded <binary>'
#
# c) Profile Guided Optimizations
# Profiled builds are a bit trickier, one has to compile twice:
# 1. first compile with TARGET := $(GENERATE_PROFILE)
# 2. run your application to collect peformance samples (.gcda files in the
# build directory)
# 3. run make clean to delete old .o object files
# 4. now run make again with TARGET := $(PROFILED_RELEASE) to compile a new
# binary using .gcda files for extra level of optimization
#
# d) CPU specific optimizations
# Setting compiler flags to -march=native and -mtune=native will generate
# faster code for the particular machine we are building on. The drawback is
# it will make the binary non backwards compatible CPU-wise.
#
# e) Static Code Analysis
# Run 'make check' rule to make sure the code is squeaky clean.
# 
# f) Code formatting
# Run 'make fmt' rule to make sure the code has consistent formatting.
# 
# g) Code coverage
# Run 'make cov' rule to see which parts of your code were executed. Very
# helpful when doing test first development.
# 
# h) Running in a debugger
# Run 'make run' to start the debugger. If the application segfaults just
# type 'bt full' to see where the problem happened.
#
# i) Release
# This has to be done manually for each platform really.
#
# j) Duplicated code
# Run 'make dup' to identify copy and paste areas.
#
# Notes on gcc flags
# gcc -Q --help=target --help=optimizers - this will print which optimizations 
# are enabled
# CXXFLAGS += -fopt-info-vec-all - this will print info on loops that were 
# auto vectorized (SIMD)
#

SHELL := /usr/bin/env bash
CC := gcc

#
# Colorize output
#
PRINT = @echo -e "\e[1;32mBuilding $@\e[0m"

#
# PLATFORM can be only one of the below
#
LINUX := 1
WINDOWS := 2
MAC := 3

#
# Select one only at a time
#
# PLATFORM := $(LINUX)
PLATFORM := $(WINDOWS)
# PLATFORM := $(MACOS)

#
# TARGET can be only one of the below
#
DEVEL := 1
RELEASE := 2
GENERATE_PROFILE := 3
PROFILED_RELEASE := 4

#
# Select one only at a time
#
# TARGET := $(DEVEL)
TARGET := $(RELEASE)
# TARGET := $(GENERATE_PROFILE)
# TARGET := $(PROFILED_RELEASE)

#
# Name of the resulting binary file
#
BUILD_ARTIFACT := plasma
ifeq ($(PLATFORM), $(WINDOWS))
	BUILD_ARTIFACT := $(BUILD_ARTIFACT).exe
endif

#
# -I, -D preprocessor options
#
CPPFLAGS = -I. `sdl-config --cflags`
ifneq ($(TARGET), $(DEVEL))
	CPPFLAGS += -DNDEBUG
endif

#
# Debugging and optimization options for the C and C++ compilers
#
ifeq ($(TARGET), $(DEVEL))

	#
	# Basic warnings
	#
	CFLAGS := -std=c89 -pedantic -Wall -Wextra
	# CFLAGS += -Werror

	#
	# Extended warning and debug options
	#
	CFLAGS += -Wlogical-op -Wdouble-promotion -Wshadow -Wformat=2
	CFLAGS += -Wcast-align -Winvalid-pch -Wmissing-include-dirs
	CFLAGS += -Wredundant-decls -Wswitch-default -Wswitch-enum
	CFLAGS += -Wmissing-prototypes -Wconversion -Wcast-qual
	CFLAGS += -Wmissing-declarations -Wodr -Wjump-misses-init
	#
	# Those warnings require gcc newer than 5.1
	#
	# CFLAGS += -Wduplicated-cond -Wduplicated-branches -Wrestrict
	# CFLAGS += -Wnull-dereference

	#
	# Debugger experience
	#
	CFLAGS += -O0 -g3 -fno-omit-frame-pointer -fno-strict-aliasing
	CFLAGS += -fno-optimize-sibling-calls -fasynchronous-unwind-tables
	CFLAGS += -fexceptions

	#
	# Code coverage and profiling
	#
	CFLAGS += --coverage -pg

	#
	# Security options, Linux only - comment out for Windows
	#
	# CPPFLAGS += -D_FORTIFY_SOURCE=2
	# CFLAGS += -fstack-protector-strong
	# CFLAGS += -fPIE -fPIC -pie -Wl,-pie -Wl,-z,defs -Wl,-z,now -Wl,-z,relro

	#
	# Sanitizers, Linux only - comment out for Windows
	#
	# CFLAGS += -fsanitize=leak
	# CFLAGS += -fsanitize=undefined
	#
	# Run with: ASAN_OPTIONS=detect_invalid_pointer_pairs=2: ./<binary>
	#
	# CFLAGS += -fsanitize=address
	# CFLAGS += -fsanitize=address -fsanitize=pointer-compare
	# CFLAGS += -fsanitize=pointer-subtract
	#
	# Those two only seem to work with clang on x86-64
	#
	# CFLAGS += -fsanitize=thread
	# CFLAGS += -fsanitize=memory
else ifeq ($(TARGET), $(RELEASE))
	CFLAGS := -O2
else ifeq ($(TARGET), $(GENERATE_PROFILE))
	CFLAGS := -O3 -march=native -mtune=native -flto -fprofile-generate
else ifeq ($(TARGET), $(PROFILED_RELEASE))
	CFLAGS := -O3 -march=native -mtune=native -flto -fprofile-use
	#
	# Turn it on for multithreaded applications
	#
	# CFLAGS += -fprofile-correction
endif

#
# -L options for the linker
#
LDFLAGS := -L.

#
# -l options to pass to the linker
#
LDLIBS := `sdl-config --libs` -lSDL_mixer -lSDL_image -lSDL_ttf -lm
ifeq ($(TARGET), $(DEVEL))
	#
	# Linux only, comment out for gcc on Windows
	#
	# LDLIBS += -fsanitize=address -static-libasan
	# LDLIBS += -fsanitize=undefined -static-libubsan
endif
ifeq ($(TARGET),$(filter $(TARGET),$(GENERATE_PROFILE) $(PROFILED_RELEASE)))
	LDLIBS += -lgcov
endif

#
# Linking
#
OBJECTS := main.o
all: $(OBJECTS)
	$(PRINT)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJECTS) $(LDFLAGS) $(LDLIBS) -o $(BUILD_ARTIFACT)

#
# Compiling
#  *.h 
main.o: main.c
	$(PRINT)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ $(LDFLAGS) $(LDLIBS)

cov:
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory out
	# chromium-browser ./out/index.html

prof:
	gprof $(BUILD_ARTIFACT) gmon.out

run: all
	echo "run" | gdb -quiet $(BUILD_ARTIFACT)

clean:
	rm -f *.o $(BUILD_ARTIFACT) || true
	rm *plist *gcno *gch gmon.out coverage.info profile.txt tests || true
	# rm *.gcda || true
	rm -f output.db || true
	rm -rf "infer-out" "out" || true
	rm -f *tar.gz || true
	rm -f *.orig *.uncrustify || true
	rm stderr.txt stdout.txt || true

release:
	strip --strip-unneeded $(BUILD_ARTIFACT)
	echo "<Copy required files here>"
	tar -czf $(BUILD_ARTIFACT).tar.gz $(BUILD_ARTIFACT)

SOURCE_FILES := *.c
# HEADER_FILES := *.h

fmt:
	uncrustify -c ~/c.cfg --replace --no-backup $(HEADER_FILES) $(SOURCE_FILES)
	astyle --style=otbs --max-code-length=120 -k3 $(HEADER_FILES) $(SOURCE_FILES)
	astyle --style=allman --max-code-length=120 -k3 $(HEADER_FILES) $(SOURCE_FILES)
	# clang-format -i -style=Google $(HEADER_FILES) $(SOURCE_FILES)

check:
	# @echo
	# @echo -e "\e[1;33mAnalazing: clang-analyze... \e[0m"	
	# clang --analyze $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33m Running clang-tidy... \e[0m"
	# #  -warnings-as-errors=*
	# clang-tidy -checks=boost-*,bugprone-*,cert-*,cppcoreguidelines-*,clang-analyzer-*,hicpp-*,modernize-*,performance-*,portability-*,readability-* $(HEADER_FILES) $(SOURCE_FILES)

	@echo
	@echo -e "\e[1;33mAnalazing: cppcheck... \e[0m"
	cppcheck --enable=all --platform=unix64 $(CPPFLAGS) std=c90 $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: infer... \e[0m"
	# infer run -- gcc -c $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: oclint... \e[0m"
	# oclint $(HEADER_FILES) $(SOURCE_FILES) -- -c

	@echo
	@echo -e "\e[1;33mAnalazing: flawfinder... \e[0m"
	flawfinder $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: rats... \e[0m"
	# rats $(HEADER_FILES) $(SOURCE_FILES)

	@echo
	@echo -e "\e[1;33mAnalazing: flint++... \e[0m"
	flint++ --cmode $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33m Running cpplint... \e[0m"
	# cpplint $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: ikos... \e[0m"
	# ikos main.c

	# @echo
	# @echo -e "\e[1;33mAnalazing: uno... \e[0m"
	# uno $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: splint... \e[0m"
	# splint --weak $(CPPFLAGS) $(HEADER_FILES) $(SOURCE_FILES)

	# @echo
	# @echo -e "\e[1;33mAnalazing: cbmc... \e[0m"
	# cbmc main.c --pointer-check --bounds-check --unwind 2

dup:
	# @echo
	# @echo -e "\e[1;33mAnalazing: cpd... \e[0m"
	# cpd --language c --minimum-tokens 50 --files $(SOURCE_FILES)

	@echo
	@echo -e "\e[1;33mAnalazing: lizard... \e[0m"
	python3 -m lizard -Eduplicate $(SOURCE_FILES)

