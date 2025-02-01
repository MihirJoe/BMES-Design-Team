build_path ?= build

BEAR := bear
CC := clang

sanitize_flags := -fsanitize=address,undefined

BEARFLAGS := --append --output $(build_path)/compile_commands.json
CFLAGS := -I../include -std=c17 -g -O1 -pthread -Wall -Wextra -D_XOPEN_SOURCE=600  $(sanitize_flags)
LDFLAGS := -pthread $(sanitize_flags)

srcs := cli.c console.c listen.c main.c monitor.c proto.c result.c serial.c support.c
obj_paths := $(patsubst %,$(build_path)/%.o,$(srcs))

$(build_path)/client: $(obj_paths) | $(build_path)
	$(CC) -o$@ $^ $(LDFLAGS)

$(build_path)/%.c.o: src/%.c | $(build_path)
	$(BEAR) $(BEARFLAGS) -- $(CC) -c -o$@ $< $(CFLAGS)

$(build_path):
	mkdir $@
