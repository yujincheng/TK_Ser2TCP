ifndef CFLAGS
CFLAGS = -O2 -Wall -g -I ../src
endif
LIBS= -lpthread

#udhcpc.o
all: ser2TCP

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

ser2TCP: tcpclient.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf ser2TCP *~

