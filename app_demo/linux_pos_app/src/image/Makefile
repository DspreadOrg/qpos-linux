PROJECT = image
platform = M3
O = ./Objects

# Startup code
#STARTUP = ./Startup/startup_thm36.c
topDir = ./

#APPINCLUDE += -I./Startup
APPINCLUDE += -I./

#APPINCLUDE += -I./lib

ALLSRC += $(wildcard ./*.c)

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

#startup_obj := $(STARTUP:.c=.o)

#OBJS = $(patsubst %.c, %.o, $(notdir $(ALLSRC))) $(startup_obj)
OBJS = $(patsubst %.c, %.o, $(notdir $(ALLSRC)))
#DEPS = $(patsubst %.c, %.d, $(OBJS))

ARCH_FLAGS = -mthumb -mcpu=cortex-m3 -std=gnu99 -fno-rtti -fno-exceptions 
CFLAGS += $(APPINCLUDE) $(ARCH_FLAGS)
CFLAGS += -fno-common -fdata-sections -ffunction-sections
CFLAGS += -msoft-float
CFLAGS += -Os -Wall
USE_SEMIHOST := 
USE_NOHOST := 
GC := 
#LDSCRIPTS := -T thm36_Flash_gcc.ld
MAP = -Wl,-Map,$(O)/$(PROJECT).map,--cref,--gc-section
#LDFLAGS += $(LDSCRIPTS)
LDFLAGS += $(MAP)
LIBS    :=

.PHONY: all
all: 
	mkdir -p $(O);	\
	for name in `echo $(ALLSRC) | sed -e 's/\.c//g'`; \
	do \
		echo "compiling $$name".c; \
		$(CC) $(CFLAGS) $(LDFLAGS) -c "$$name".c -o "$$name".o;	\
		mv -f "$$name".o ./$(O);	\
	done

	while true;	\
	do	\
		cd $(O);	\
		$(AR) -r ../$(O)/$(PROJECT).a $(OBJS);	\
		if [ -f "$(PROJECT).a" ]; then	\
			echo "Built $(PROJECT).a OK!";	\
		fi;	\
		break;	\
	done
	
.PHONY: print
print:
	@echo outfilename:  $(O)/$(PROJECT).a
	@echo CFLAGS:   $(CFLAGS)
	@echo CC:   $(CC)
	@echo AR:   $(AR)
	@echo ALLSRC:   $(ALLSRC)
	@echo OBJS:   $(OBJS)

.PHONY: clean
clean:
	-rm -f $(O)/*.*

