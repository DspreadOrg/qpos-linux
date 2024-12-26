PROJECT = lvgl_v8_GNU
platform = M3
O = ./Objects

# Startup code
#STARTUP = ./Startup/startup_thm36.c
topDir = ./

#APPINCLUDE += -I./Startup
APPINCLUDE += -I$(topDir)
APPINCLUDE += -I$(topDir)/src
APPINCLUDE += -I$(topDir)/src/core
APPINCLUDE += -I$(topDir)/src/draw
APPINCLUDE += -I$(topDir)/src/extra
APPINCLUDE += -I$(topDir)/src/extra/layouts
APPINCLUDE += -I$(topDir)/src/extra/layouts/flex
APPINCLUDE += -I$(topDir)/src/extra/layouts/grid
APPINCLUDE += -I$(topDir)/src/extra/others
APPINCLUDE += -I$(topDir)/src/extra/others/snapshot
APPINCLUDE += -I$(topDir)/src/extra/themes
APPINCLUDE += -I$(topDir)/src/extra/themes/basic
APPINCLUDE += -I$(topDir)/src/extra/themes/default
APPINCLUDE += -I$(topDir)/src/extra/themes/mono
APPINCLUDE += -I$(topDir)/src/extra/widgets
APPINCLUDE += -I$(topDir)/src/extra/widgets/animimg
APPINCLUDE += -I$(topDir)/src/extra/widgets/calendar
APPINCLUDE += -I$(topDir)/src/extra/widgets/chart
APPINCLUDE += -I$(topDir)/src/extra/widgets/colorwheel
APPINCLUDE += -I$(topDir)/src/extra/widgets/imgbtn
APPINCLUDE += -I$(topDir)/src/extra/widgets/keyboard
APPINCLUDE += -I$(topDir)/src/extra/widgets/led
APPINCLUDE += -I$(topDir)/src/extra/widgets/list
APPINCLUDE += -I$(topDir)/src/extra/widgets/meter
APPINCLUDE += -I$(topDir)/src/extra/widgets/msgbox
APPINCLUDE += -I$(topDir)/src/extra/widgets/span
APPINCLUDE += -I$(topDir)/src/extra/widgets/spinbox
APPINCLUDE += -I$(topDir)/src/extra/widgets/spinner
APPINCLUDE += -I$(topDir)/src/extra/widgets/tabview
APPINCLUDE += -I$(topDir)/src/extra/widgets/tileview
APPINCLUDE += -I$(topDir)/src/extra/widgets/win
APPINCLUDE += -I$(topDir)/src/font
APPINCLUDE += -I$(topDir)/src/gpu
APPINCLUDE += -I$(topDir)/src/hal
APPINCLUDE += -I$(topDir)/src/misc
APPINCLUDE += -I$(topDir)/src/widgets
#APPINCLUDE += -I./lib

ALLSRC += $(wildcard $(topDir)/src/core/*.c)
ALLSRC += $(wildcard $(topDir)/src/draw/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/layouts/flex/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/layouts/grid/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/themes/basic/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/themes/default/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/themes/mono/*.c)

ALLSRC += $(wildcard $(topDir)/src/extra/widgets/animimg/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/calendar/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/chart/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/colorwheel/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/imgbtn/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/keyboard/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/led/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/list/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/meter/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/msgbox/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/span/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/spinbox/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/spinner/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/tabview/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/tileview/*.c)
ALLSRC += $(wildcard $(topDir)/src/extra/widgets/win/*.c)
ALLSRC += $(wildcard $(topDir)/src/font/*.c)
ALLSRC += $(wildcard $(topDir)/src/gpu/*.c)
ALLSRC += $(wildcard $(topDir)/src/hal/*.c)
ALLSRC += $(wildcard $(topDir)/src/misc/*.c)
ALLSRC += $(wildcard $(topDir)/src/widgets/*.c)


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

