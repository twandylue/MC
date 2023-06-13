CFLAGS=-Wall -Wextra -std=c11 -pedantic -ggdb
STYLE=llvm # google, llvm, chromium, mozilla, webkit

heap: main.c
	clang-format --style=$(STYLE) -i main.c
	cc $(CFLAGS) main.c -o heap
