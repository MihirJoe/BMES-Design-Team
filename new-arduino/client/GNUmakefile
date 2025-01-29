CC ?= clang
CFLAGS += -I../include -std=c17 -O2 -pthread -Wall -Wextra -D_XOPEN_SOURCE=600
LDFLAGS += -pthread

build_path ?= build

srcs := interact.c main.c serial.c tty.c
obj_paths := $(patsubst %,$(build_path)/%.o,$(srcs))

$(build_path)/client: $(obj_paths) | $(build_path)
	$(CC) -o$@ $^ $(LDFLAGS)

$(build_path)/%.c.o: src/%.c | $(build_path)
	$(CC) -c -o$@ $< $(CFLAGS)

$(build_path):
	mkdir $@
