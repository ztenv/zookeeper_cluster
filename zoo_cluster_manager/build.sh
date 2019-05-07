g++ -Wall -g -fPIC -shared cluster_manager.cpp -o libcluster.so -DTHREADED -I./include/zookeeper -L./lib -lzookeeper_mt --std=c++11
mv libcluster.so ./lib
g++ -Wall -g main.cpp -o main -DTHREADED -I./include/zookeeper -L./lib -lzookeeper_mt -lcluster --std=c++11
