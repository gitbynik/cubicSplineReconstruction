CC := gcc

TARGET := main

SRCS := main.c render.c update.c canvas.c widgets.c app.c spline.c
OBJS := $(SRCS:.c=.o)

ifeq ($(OS),Windows_NT)
	PLATFORM := windows
else
	PLATFORM := linux
endif

ifeq ($(PLATFORM),windows)

	BASELIBS := -lmingw32 -lSDL3 -lSDL3_image -lSDL3_ttf -lm
	STATLIBS := -static -Wl,--start-group -lsetupapi -lwinmm -lgdi32 -lusp10 -limm32 -lole32 -loleaut32 -lversion -luuid -lfreetype -lharfbuzz -lpng -lz -lbz2 -lbrotlidec -lgraphite2 -ldwrite -lbrotlicommon -lrpcrt4 -lstdc++ -Wl,--end-group

else

	BASELIBS := -lSDL3 -lSDL3_image -lSDL3_ttf -lm

endif

CFLAGS :=
LDFLAGS :=
LIBS := $(BASELIBS)

# Static linking Windows only

ifeq ($(PLATFORM),windows)
	ifeq ($(STATIC),true)
		LIBS += $(STATLIBS)
	endif
endif

ifeq ($(DEBUG),true)
	CFLAGS += -g -DDEBUG
endif

ifeq ($(ASAN),true)
	CFLAGS += -fsanitize=address -fno-omit-frame-pointer
	LDFLAGS += -fsanitize=address
endif

ifeq ($(PLATFORM),windows)
	ifeq ($(DEBUG),false)
		LIBS += -mwindows
	endif
endif

.PHONY: all static debug asan staticdebug clean

all: $(TARGET)

static:
	@$(MAKE) --no-print-directory STATIC=true

debug:
	@$(MAKE) --no-print-directory DEBUG=true

asan:
	@$(MAKE) --no-print-directory DEBUG=true ASAN=true

staticdebug:
	@$(MAKE) --no-print-directory STATIC=true DEBUG=true

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(PLATFORM),windows)

clean:
	-del /Q $(OBJS) $(TARGET).exe 2>nul

else

clean:
	rm -f $(OBJS) $(TARGET)

endif