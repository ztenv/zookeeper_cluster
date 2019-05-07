#include "cluster_manager.h"
#include <iostream>
#include <memory>

using namespace std;
using namespace xc::common;

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
    if(argc==3)
    {
        path=argv[1];
        node=argv[2];
    }
    //CClusterManagerPtr cmPtr=std::make_shared<impl::CClusterManager>();
    //IClusterManagerPtr icmPtr=cmPtr;
    IClusterManagerPtr icmPtr=ClusterManagerFactory::create();

    IEventHandlerPtr ehPtr=IEventHandlerPtr(new EventHandler());

    //icmPtr->Initialize("192.168.0.227:2181,192.168.0.227:2182,192.168.0.227:2183",
    icmPtr->Initialize("192.168.0.223:2181,192.168.0.224:2181,192.168.0.225:2181",
                       path,node,100);
    icmPtr->Register(ehPtr);
    icmPtr->Start();

    getchar();
    icmPtr->Stop();
    icmPtr->DeInitialize();

    return 0;
}
