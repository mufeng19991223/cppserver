ALL: server client threadtest1 threadtest2
server: server.cpp
	g++ -g server.cpp -o server -lpthread http_conn.cpp
client1: client.cpp
	g++ -g client.cpp -o client
threadtest1: threadtest1.cpp
	g++ -g threadtest1.cpp -o threadtest1 -lpthread
threadtest2: threadtest2.cpp
	g++ -g threadtest2.cpp -o threadtest2 -lpthread


.PHONY:clean
clean:
	-rm server
	-rm client
	-rm threadtest1
	-rm threadtest2