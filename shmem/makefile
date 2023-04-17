ALL: server client read write socketpair sendfd signal mmapRead mmapWrite
server: server.cpp
	g++ server.cpp -o server -lrt
client: client.cpp
	g++ client.cpp -o client 
read: read.cpp
	g++ read.cpp -o read -lrt
write: write.cpp
	g++ write.cpp -o write -lrt
socketpair: socketpair.cpp
	g++ socketpair.cpp -o socketpair
sendfd: sendfd.cpp
	g++ sendfd.cpp -o sendfd
signal: signal.cpp
	g++ signal.cpp -o signal
mmapRead: mmapRead.cpp
	g++ mmapRead.cpp -o mmapRead
mmapWrite: mmapWrite.cpp
	g++ mmapWrite.cpp -o mmapWrite