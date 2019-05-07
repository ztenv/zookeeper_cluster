g++ -Wall -g main.cpp cluster_manager.cpp -o main -DTHREADED -I./include/zookeeper -L./lib -lzookeeper_mt --std=c++11
