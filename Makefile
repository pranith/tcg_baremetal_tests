#
# Global variables, sources and tools
#
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld

S_OBJS    = start.o
C_OBJS    = main.o printf.o helpers.o
H_DEPS    = helpers.h
LD_SCRIPT = link.ld.S

LIBS      = $(shell $(CC) $(CCFLAGS) -print-libgcc-file-name)
CPPFLAGS  += -gdwarf-2 -fno-stack-protector -nostdinc -fno-builtin

# Set to ATOMIC to implement spinlock test with real atomic instructions
TEST = ATOMIC

#
# Target specific variables
#
clean: export DIRS = build-virt build-virt64 build-vexpress

virt vexpress:   export CPPFLAGS += -march=armv7-a
virt64: export CPPFLAGS += -march=armv8-a -mgeneral-regs-only -mstrict-align

virt vexpress:   export CROSS_COMPILE ?= arm-none-eabi-
virt64: export CROSS_COMPILE ?= aarch64-linux-gnu-

virt vexpress:   export ARCH = ARCH_ARM
virt64: export ARCH = ARCH_AARCH64

virt virt64: export UART_PHYS = 0x09000000
virt virt64: export ENTRY_POINT = 0x40000000

vexpress: export UART_PHYS = 0x1c090000
vexpress: export ENTRY_POINT = 0x80000100

virt virt64 vexpress: export O_DIR = build-$@/
virt virt64 vexpress: export IMAGE = $(O_DIR)image-$@.axf

# Export machine name
virt:     export BOARD_MODEL = VIRT
virt64:   export BOARD_MODEL = VIRT64
vexpress: export BOARD_MODEL = VEXPRESS

#
# Target build rules
#
all: virt virt64 vexpress

clean:
	rm -rf $(DIRS)

virt virt64 vexpress:
	mkdir -p $(O_DIR)
	@$(MAKE) $(IMAGE) --no-print-directory

$(IMAGE): $(addprefix $(O_DIR), $(S_OBJS)) \
          $(addprefix $(O_DIR), $(C_OBJS)) $(H_DEPS) $(O_DIR)link.ld Makefile
	$(LD) -o $@ $(addprefix $(O_DIR), $(S_OBJS)) \
          $(addprefix $(O_DIR), $(C_OBJS)) $(LIBS) \
          --script=$(O_DIR)link.ld -Map $(O_DIR)system.map

$(O_DIR)link.ld: $(LD_SCRIPT)
	$(CC) -DENTRY_POINT=$(ENTRY_POINT) -D$(ARCH) $(CPPFLAGS) -E -P -C -o $@ $<

$(O_DIR)%.o: %.c $(H_DEPS)
	$(CC) -DENTRY_POINT=$(ENTRY_POINT) -D$(BOARD_MODEL) \
          -DUART_PHYS=$(UART_PHYS) -D$(ARCH) $(CPPFLAGS) -c -o $@ $<

$(O_DIR)%.o: %.S $(H_DEPS)
	$(CC) -D$(ARCH) $(CPPFLAGS) -c -o $@ $<

.PHONY: all clean virt virt64 vexpress
