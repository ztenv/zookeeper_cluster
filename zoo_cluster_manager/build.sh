g++ -Wall -g -fPIC -shared cluster_manager.cpp -o ./lib/libcluster_t.so -DTHREADED -I./include/zookeeper -L./lib -lzookeeper_mt --std=c++11
g++ -Wall -g main.cpp -o main -DTHREADED -I./include/zookeeper -L./lib -lzookeeper_mt -lcluster_t --std=c++11
