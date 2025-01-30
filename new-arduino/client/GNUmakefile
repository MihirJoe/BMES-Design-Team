build_path ?= build

BEAR := bear
CC := clang

BEARFLAGS := --append --output $(build_path)/compile_commands.json
CFLAGS := -I../include -std=c17 -g -O2 -pthread -Wall -Wextra -D_XOPEN_SOURCE=600
LDFLAGS := -pthread

srcs := interact.c listen.c main.c proto.c serial.c
obj_paths := $(patsubst %,$(build_path)/%.o,$(srcs))

$(build_path)/client: $(obj_paths) | $(build_path)
	$(CC) -o$@ $^ $(LDFLAGS)

$(build_path)/%.c.o: src/%.c | $(build_path)
	$(BEAR) $(BEARFLAGS) -- $(CC) -c -o$@ $< $(CFLAGS)

$(build_path):
	mkdir $@
