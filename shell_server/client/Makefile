CFLAGS=-Wall -Wextra -Werror -Wvla -std=c99 -pedantic
CPPFLAGS=-Isrc

TARGET=shell_client
OBJ_MAIN=src/main.o
OBJ=src/utils/sockutils.o

TARGET_UNIT=tests/unit
OBJ_UNIT=tests/unit.o

.PHONY: all check dev coverage clean

all: $(TARGET)

$(TARGET): $(OBJ_MAIN) $(OBJ)
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

	# coverage clean
	$(RM) report.*
	find . -type f -name *.gcno -exec $(RM) {} +
	find . -type f -name *.gcda -exec $(RM) {} +
