SRCDIR 	= src
BINDIR  = bin
OBJDIR  = bin/obj

CPPSRC 	= $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC 	= $(call rwildcard,$(SRCDIR),*.asm)
LDS		= src/mnk.lds

OBJS 	= $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.cc.o, $(CPPSRC))
OBJS 	+= $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.as.o, $(ASMSRC))
SYMS	= $(OBJDIR)/syms.o
KERNEL	= $(BINDIR)/mnk.elf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

CC 		= x86_64-elf-gcc
AS 		= nasm
GS		= ./gensyms.sh

# Flags to enable all (reasonable) warnings possible
# and treat them as errors to ensure code quality
WRFLAGS	=\
-Werror \
-Wall \
-Wno-int-to-pointer-cast

CCFLAGS 	= \
-ffreestanding \
-I$(SRCDIR) \
-fno-pic \
-fpie \
-std=c++20 \
-gdwarf \
-O0 \
-mno-red-zone \
-msse3 \
-fno-threadsafe-statics \
-fno-omit-frame-pointer \
$(WRFLAGS)

LDFLAGS  = -ffreestanding \
-nostdlib \
-fno-pic \
-fpie \
-static-pie \
-pie \
-z max-page-size=0x1000 \
-lgcc \
-T $(LDS)

ASFLAGS	= \
-f elf64 \
-g \
-F dwarf 

.DEFAULT-GOAL 	= all
.PHONY			= all kernel clean

$(OBJDIR)/%.cc.o: $(SRCDIR)/%.cpp
	@ echo "]	CC $^"
	@ mkdir -p $(@D)
	@ $(CC) $(CCFLAGS) -c $^ -o $@

$(OBJDIR)/%.as.o: $(SRCDIR)/%.asm
	@ echo "]	AS $^"
	@ mkdir -p $(@D)
	@ $(AS) $(ASFLAGS) $^ -o $@

$(KERNEL): $(OBJS)
	@ echo "]	LD $@"
	@ $(CC) $(LDFLAGS) $(OBJS) -o $@


	@ echo "]	GS $@"
	@ $(GS) $@

	@ echo "]	CC syms.gen"
	@ $(CC) -fno-pic -fpie -xc -c syms.gen -o $(SYMS)

	@ echo "]	RM syms.gen"
	@ rm syms.gen

	@ echo "]	LD $@"
	@ $(CC) $(LDFLAGS) $(OBJS) $(SYMS) -o $@

all: $(KERNEL)
kernel: $(KERNEL)

clean:
	@ rm -rf bin