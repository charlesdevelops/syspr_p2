CC = gcc
CFLAGS = -std=c11 -Wall -Werror

OBJS = mysync.o checkdir.o usage.o syncDir.o argPassing.o directoryInfo.o

mysync: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

mysync.o: mysync.c mysync.h
	$(CC) $(CFLAGS) -c $<

checkdir.o: checkdir.c mysync.h
	$(CC) $(CFLAGS) -c $<

usage.o: usage.c mysync.h
	$(CC) $(CFLAGS) -c $<

syncDir.o: syncDir.c mysync.h
	$(CC) $(CFLAGS) -c $<

argPassing.o: argPassing.c mysync.h
	$(CC) $(CFLAGS) -c $<

directoryInfo.o: directoryInfo.c mysync.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f mysync $(OBJS)
