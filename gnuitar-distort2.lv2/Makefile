all:
	gcc `pkg-config lv2-plugin --libs -cflags` -g -w -fvisibility=hidden -fPIC -Wl,-Bstatic -Wl,-Bdynamic -Wl,--as-needed -shared -pthread -I/usr/include/lv2 -lm distort.c -o distort.so
