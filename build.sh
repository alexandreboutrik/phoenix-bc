gcc -I./include -I./src src/main.c \
	src/check.c src/data.c \
	-o exe $*
