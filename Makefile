all: LD_PRELOAD_va.so

CC = clang
LDFLAGS += $(shell pkg-config --cflags --libs libva)

LD_PRELOAD_%.so: LD_PRELOAD_%.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< -ldl -Wl,--no-as-needed $(LDFLAGS)
