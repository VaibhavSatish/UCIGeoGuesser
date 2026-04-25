CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -Wfatal-errors -pedantic
INCLUDES := -Icrow -Iasio-1.30.2/include

main: main.cpp
	mkdir -p bin
	g++ $(CXXFLAGS) main.cpp -o bin/main

server_connection: server_connection.cpp
	mkdir -p bin
	g++ $(CXXFLAGS) $(INCLUDES) server_connection.cpp -o bin/server_connection

clean:
	rm -f bin/*