malloc_test: my_malloc.h my_malloc.c main.c
	gcc -ggdb -o malloc_test -pedantic -std=gnu99 my_malloc.h my_malloc.c main.c
