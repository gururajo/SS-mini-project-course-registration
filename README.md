run init.cpp first
g++ init.cpp -o init && ./init

then start server and open as many clients required
g++ server.cpp -o server && ./server
g++ client.cpp -o client && ./client
