g++ -Wall -g -fPIC -shared ./src/cluster_manager.cpp -o ./lib/libcluster.so -DTHREADED -I./include -I./src/zookeeper -L./lib -lzookeeper_mt --std=c++11
g++ -Wall -g ./test/main.cpp -o ./test/main -DTHREADED -I./include -L./lib -lzookeeper_mt -lcluster --std=c++11
