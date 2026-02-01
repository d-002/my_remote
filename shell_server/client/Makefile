CFLAGS=-Wall -Wextra -Werror -Wvla -std=c99 -pedantic
CPPFLAGS=-Isrc

TARGET=shell_client
TARGET_WIN=shell_client.exe
OBJ_MAIN=src/main.o
OBJ=src/sock/sock.o src/sock/sockutils.o \
	src/utils/stringutils.o src/utils/stringbuilder.o \
	src/logger/logger.o \
	src/settings/settings.o src/settings/state.o \
	src/queue/queue.o \
	src/mainloop/mainloop.o src/mainloop/heartbeat.o src/mainloop/list_commands.o src/mainloop/run_commands.o src/mainloop/signals.o

TARGET_UNIT=tests/unit
OBJ_UNIT=tests/unit.o

.PHONY: all portable windows check dev coverage clean

all: $(TARGET)

portable: CC=musl-gcc
portable: LDFLAGS+=-static
portable: clean $(TARGET)
	[ -n "$$(ldd $(TARGET) 2>&1 | grep 'not a dynamic executable')" ]
	strip $(TARGET)

windows: $(TARGET_WIN)

$(TARGET): $(OBJ_MAIN) $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(TARGET_WIN): CC=cosmocc
$(TARGET_WIN): $(OBJ_MAIN) $(OBJ)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(TARGET_UNIT): CFLAGS+=-lcriterion -g
$(TARGET_UNIT): LDLIBS+=-lcriterion
$(TARGET_UNIT): $(OBJ) $(OBJ_UNIT)
	$(CC) -o $(TARGET_UNIT) $^ $(LDFLAGS) $(LDLIBS)

check: LDLIBS+=-lcriterion
check: $(TARGET) $(TARGET_UNIT)
	./$(TARGET_UNIT) -j 1

dev: CFLAGS+=-fsanitize=address -g
dev: LDLIBS+=-fsanitize=address
dev: check

coverage: CFLAGS+=--coverage -fPIC
coverage: LDLIBS+=-lgcov
coverage: LDFLAGS+=--coverage
coverage: dev
	gcovr --html --html-nested --output=report.html
	xdg-open report.html

clean:
	# main clean
	$(RM) $(TARGET) $(OBJ_MAIN) $(OBJ)
	$(RM) $(TARGET_UNIT) $(OBJ_UNIT)
	$(RM) $(TARGET_WIN) *.dbg *.elf

	# coverage clean
	$(RM) report.*
	find . -type f -name *.gcno -exec $(RM) {} +
	find . -type f -name *.gcda -exec $(RM) {} +
