#include "cluster_interface.h"
#include <iostream>
#include <memory>

using namespace std;
using namespace uvframe::baseutils;

/**
 * @brief 主从变动通知事件
 */
class EventHandler final:public IEventHandler
{
protected:
    void OnMasterEvent(bool isMaster) override
    {
        if(isMaster)
        {
            cout<<"you are the master node!"<<endl;
        }else{
            cout<<"you are one of slave nodes"<<endl;
        }
    }
};

int main(int argc,char *argv[])
{
    std::string path="/xunce/adc";
    std::string node="0000";
    std::string hosts="192.168.0.223:2181,192.168.0.224:2181,192.168.0.225:2181";
    if(argc==4)
    {
        hosts=argv[1];
        path=argv[2];
        node=argv[3];
    }else{
        cout<<"usage:"<<argv[0]<<" zk_hosts path node"<<endl<<
            "such as:"<<argv[0]<<" "<<hosts<<" "<<path<<" "<<node<<endl;
        return 0;
    }

    IClusterManagerPtr icmPtr=ClusterManagerFactory::create();
    IEventHandlerPtr ehPtr=IEventHandlerPtr(new EventHandler());

    icmPtr->Initialize(hosts,path,node,100);
    icmPtr->Register(ehPtr);
    icmPtr->Start();

    getchar();
    icmPtr->Stop();
    icmPtr->DeInitialize();

    return 0;
}
