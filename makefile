CC= g++
cflags= -Wall -g
clibs= -lm -lpthread
targets= p2p_server
objs= server/communicationserver.o server/peerholder.o main.o

all: ${targets}

p2p_server: $(objs)
	$(CC) $(cflags) $(objs) -o p2p_server $(clibs)

%.o: %.cpp
	$(CC) -std=c++11 -g -c $< -o $@ $(clibs)

clean:
	rm -f $(objs) $(targets) *~
