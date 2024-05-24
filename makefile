# kill_process_port:
# 	kill -9 `lsof -t -i:7800`
all:
	clang++ -std=c++17 -lpthread main.cpp && ./a.out