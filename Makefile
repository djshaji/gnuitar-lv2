all:
	gcc -fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread -I/usr/include/lv2 -lm amp.c -o amp.o
