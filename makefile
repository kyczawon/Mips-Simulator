###################################################
## MIPS Compilation path

MIPS_CC = mips-linux-gnu-gcc
MIPS_OBJCOPY = mips-linux-gnu-objcopy

# Turn on all warnings, and enable optimisations
MIPS_CPPFLAGS = -W -Wall -O3 -fno-builtin

# Avoid standard libraries etc. being brought in, and link statically
MIPS_LDFLAGS = -nostdlib -Wl,-melf32btsmip -march=mips1 -nostartfiles -mno-check-zero-division -Wl,--gpsize=0 -static -Wl,-Bstatic
MIPS_LDFLAGS += -Wl,--build-id=none

%.mips.s : %.c
	$(MIPS_CC) -S -O3 $< -o $@

# Compile a c file into a MIPS object file
%.mips.o : %.c
	$(MIPS_CC) $(MIPS_CPPFLAGS) -c $< -o $@
	make $*.mips.s

# Link a MIPS object file and place it at the locations required in the
# spec using linker.ld
%.mips.elf : %.mips.o
	$(MIPS_CC) $(MIPS_CPPFLAGS) $(MIPS_LDFLAGS) -T src/linker.ld $< -o $@

# Extract just the binary instructions from the object file
%.mips.bin : %.mips.elf
	$(MIPS_OBJCOPY) -O binary --only-section=.text $< $@

# For example, if you have testbench/test.c, you can do:
#
# make testbench/test.mips.bin

###################################################
## Simulator

# Build the simulation binary
bin/mips_simulator : src/simulator.cpp src/functions.cpp
	mkdir -p
	g++ -W -Wall src/simulator.cpp  bin/mips_simulator
	# g++ -W -Wall -c -std=c++11 src/simulator.cpp
	# g++ -W -Wall -c -std=c++11 src/functions.cpp
	# g++ simulator.o functions.o -o bin/mips_simulator

# In order to comply with spec
simulator : bin/mips_simulator

###################################################
## Testbench

testbench :
	echo "No testbench yet"
	exit 1
