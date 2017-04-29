CC= g++
cflags= -Wall -g
clibs= -lm -lpthread
targets= p2p_client
objs= peerclient.o main.o encryption_methods.o

all: ${targets}

p2p_client: $(objs)
	$(CC) $(cflags) $(objs) -o p2p_client $(clibs)

%.o: %.cpp
	$(CC) -std=c++11 -g -c $< -o $@ $(clibs)

clean:
	rm -f $(objs) $(targets) *~
