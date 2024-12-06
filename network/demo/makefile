all:test_server test_client
test_server:test_server.cpp server.cpp
	g++ -o test_server test_server.cpp server.cpp

test_client:test_client.cpp client.cpp
	g++ -o test_client test_client.cpp client.cpp

clean:
	rm -f test_server test_client 