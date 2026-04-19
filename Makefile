COMMON_FLAGS = -DEFINE_DEF=static -pedantic-errors -Werror=implicit-function-declaration -Werror=vla -Wall -Wextra -Wpedantic -Wshadow

char_hist: main.c efine_helpers.h efine_fsync.h efine_portability_base.h
	$(CC) $(COMMON_FLAGS) -DNDEBUG -fstrict-aliasing -Os -Wstrict-aliasing -Wno-unused-function -Wno-unused-parameter -Wno-unused-label -Wno-parentheses -o $@ main.c

char_hist_test: main.c efine_helpers.h efine_fsync.h efine_portability_base.h
	$(CC) $(COMMON_FLAGS) -fsanitize=address,undefined -Og -g -fno-strict-aliasing -o $@ main.c
