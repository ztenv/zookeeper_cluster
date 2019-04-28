#include "cluster_manager.h"

#include <iostream>
#include <vector>
#include <exception>


#include "StringHelper.h"


using namespace std;
namespace xc{
    namespace common{

        namespace impl{
            CClusterManager::CClusterManager():m_runState(EN_State::UnKnown),
            m_nodeMode(EN_NodeMode::Other),m_tag("xunce tech adc")
            {
                cout<<"CClusterManager ctor"<<endl;
            }

            CClusterManager::~CClusterManager()
            {
                m_runState=EN_State::UnKnown;
                cout<<"CClusterManager dtor"<<endl;
            }

            int CClusterManager::Initialize(const std::string &hosts,const std::string &path,
                                            const std::string &node,unsigned int timeout)
            {
                int res=0;
                do{
                    if(m_runState==EN_State::Initialized||m_runState==EN_State::Running||
                       m_runState==EN_State::Started)
                    {//bug is here
                        break;
                    }

                    if((hosts.length()==0)||(path.length()==0)||(m_zkhandlePtr.get()!=nullptr))
                    {
                        res=-1;
                        break;
                    }

                    m_hosts=hosts;
                    m_path=path;
                    m_node=node;

                    zhandle_t *zk_handle=zookeeper_init(hosts.c_str(),CClusterManager::watcher,
                                                        timeout,(clientid_t*)NULL,(void*)this,0);
                    if(zk_handle==nullptr)
                    {
                        res=-2;
                        break;
                    }
                    this->m_zkhandlePtr.reset();
                    this->m_zkhandlePtr=std::shared_ptr<zhandle_t>(zk_handle,zk_handle_deleter());
                    m_runState=EN_State::Initialized;

                }while(0);
                return res;
            }

            int CClusterManager::Start()
            {
                int res=0;
                if(m_runState==EN_State::Initialized)
                {
                    res=this->createNode(this->m_path,this->m_node);
                    if(res==ZOK)
                    {
                        m_runState=EN_State::Started;
                    }
                    cout<<errorCode2String(res)<<endl;
                }else{
                    res=-1;
                    cout<<"Need initialize first."<<endl;
                }
                return res;
            }

            int CClusterManager::Register(common::IEventHandlerPtr eventHandlerPtr)
            {
                std::lock_guard<std::mutex> lock_guard(m_ehMutex);
                m_eventHandlers.push_back(eventHandlerPtr);
                return 0;
            }

            int CClusterManager::Stop()
            {
                this->clearEventHandlers();
                m_zkhandlePtr.reset();
                return 0;
            }

            int CClusterManager::DeInitialize()
            {
                this->clearEventHandlers();
                m_zkhandlePtr.reset();
                return 0;
            }

            void CClusterManager::watcher(zhandle_t *zk_handle,int eventType,int state,
                                       const char *path,void *context)
            {
                cout<<"**********watcher event***********************"
                    <<"event:"<<eventType<<" state:"<<state<<" path:"<<path<<endl;

                CClusterManager* pthis=static_cast<CClusterManager*>(context);
                if(eventType==ZOO_CHILD_EVENT)
                {
                    pthis->onZOO_CHILD_EVENT(zk_handle,eventType,state,path);
                }
                cout<<pthis->m_node<<" node work mode:"<<(int)pthis->m_nodeMode<<endl;
            }

            void CClusterManager::onZOO_CHILD_EVENT( zhandle_t *zk_handle,int eventType,
                                                    int state, const char *path)
            {
                String_vector *pchild_nodes=(String_vector*)malloc(sizeof(String_vector)*100);
                if(pchild_nodes!=NULL)
                {
                    int res=zoo_get_children(zk_handle,path,1,pchild_nodes);
                    if(res==ZOO_ERRORS::ZOK)
                    {
                        cout<<"path:"<<path<<" children changed:"<<endl;
                        if (pchild_nodes->count>0)
                        {
                            bool isMaster=(m_node==pchild_nodes->data[0]);
                            if(isMaster)
                            {
                                m_nodeMode=EN_NodeMode::Leader;
                            }else{
                                m_nodeMode=EN_NodeMode::Follower;
                            }

                            this->triggerMasterEvent(isMaster);
                        }
                        for(int i=0;i<pchild_nodes->count;++i)
                        {
                            cout<<pchild_nodes->data[i]<<endl;
                        }
                        deallocate_String_vector(pchild_nodes);
                    }
                }
            }

            int CClusterManager::createNode(const std::string &path,const std::string &node)
            {
                int res=ZOK;
                if((m_zkhandlePtr.get()!=NULL)&&(path.length()>0)&&(node.length()>0))
                {
                    res=zoo_exists(m_zkhandlePtr.get(),path.c_str(),0,NULL);
                    cout<<this->errorCode2String(res)<<endl;
                    std::string zkPath;
                    if (res==ZNONODE)
                    {
                        std::vector<std::string> nodes;
                        StringHelper::Split(path,nodes,"/",true);
                        for(unsigned int i=0;i<nodes.size();++i)
                        {
                            zkPath.append("/");
                            zkPath.append(nodes[i]);
                            res=zoo_exists(m_zkhandlePtr.get(),zkPath.c_str(),0,NULL);
                            if (res==ZNONODE)
                            {
                                res=zoo_create(m_zkhandlePtr.get(),zkPath.c_str(),m_tag.c_str(),m_tag.length(),
                                                &ZOO_OPEN_ACL_UNSAFE,0,NULL,0);
                                cout<<"create:"<<zkPath<<":"<<errorCode2String(res)<<endl;
                                if(res!=ZOK)
                                {
                                    res=-2;
                                    break;
                                }
                            }
                        }
                    }
                    res|=zoo_get_children(m_zkhandlePtr.get(),m_path.c_str(),1,NULL);

                    zkPath.clear();
                    zkPath.append(path);
                    zkPath.append("/");
                    zkPath.append(node);
                    res=zoo_exists(m_zkhandlePtr.get(),zkPath.c_str(),0,NULL);
                    if(res==ZNONODE)
                    {
                        res=zoo_create(m_zkhandlePtr.get(),zkPath.c_str(),m_tag.c_str(),
                                       m_tag.length(),&ZOO_OPEN_ACL_UNSAFE,ZOO_EPHEMERAL,NULL,0);
                        cout<<"create:"<<zkPath<<":"<<errorCode2String(res)<<endl;
                    }
                }else{
                    res=-1;
                }
                return res;
            }

            void CClusterManager::clearEventHandlers()
            {
                std::lock_guard<std::mutex> lock_guard(m_ehMutex);
                m_eventHandlers.clear();
            }

            void CClusterManager::triggerMasterEvent(bool isMaster)
            {
                std::lock_guard<std::mutex> lock_guard(m_ehMutex);
                std::vector<common::IEventHandlerPtr>::const_iterator citer=m_eventHandlers.begin();
                while(citer!=m_eventHandlers.end())
                {
                    try{
                        (*citer)->OnMasterEvent(isMaster);
                        ++citer;
                    }catch(std::exception &e)
                    {
                        cout<<e.what()<<endl;
                    }
                    catch(...)
                    {
                    }
                }
            }

            const char *CClusterManager::errorCode2String(int ec)
            {
                 if (ec == ZOK) {
                     return "OK";
                 }
                 if (ec == ZSYSTEMERROR) {
                     return "System error";
                 }
                 if (ec == ZRUNTIMEINCONSISTENCY) {
                     return "Runtime inconsistency";
                 }
                 if (ec == ZDATAINCONSISTENCY) {
                     return "Data inconsistency";
                 }
                 if (ec == ZCONNECTIONLOSS) {
                     return "Connection to the server has been lost";
                 }
                 if (ec == ZMARSHALLINGERROR) {
                     return "Error while marshalling or unmarshalling data ";
                 }
                 if (ec == ZUNIMPLEMENTED) {
                     return "Operation not implemented";
                 }
                 if (ec == ZOPERATIONTIMEOUT) {
                     return "Operation timeout";
                 }
                 if (ec == ZBADARGUMENTS) {
                     return "Invalid argument";
                 }
                 if (ec == ZINVALIDSTATE) {
                     return "Invalid zhandle state";
                 }
                 if (ec == ZAPIERROR) {
                     return "API error";
                 }
                 if (ec == ZNONODE) {
                     return "Znode does not exist";
                 }
                 if (ec == ZNOAUTH) {
                     return "Not authenticated";
                 }
                 if (ec == ZBADVERSION) {
                     return "Version conflict";
                 }
                 if (ec == ZNOCHILDRENFOREPHEMERALS) {
                     return "Ephemeral nodes may not have children";
                 }
                 if (ec == ZNODEEXISTS) {
                     return "Znode already exists";
                 }
                 if (ec == ZNOTEMPTY) {
                     return "The znode has children";
                 }
                 if (ec == ZSESSIONEXPIRED) {
                     return "The session has been expired by the server";
                 }
                 if (ec == ZINVALIDCALLBACK) {
                     return "Invalid callback specified";
                 }
                 if (ec == ZINVALIDACL) {
                     return "Invalid ACL specified";
                 }
                 if (ec == ZAUTHFAILED) {
                     return "Client authentication failed";
                 }
                 if (ec == ZCLOSING) {
                     return "ZooKeeper session is closing";
                 }
                 if (ec == ZNOTHING) {
                     return "No response from server";
                 }
                 if (ec == ZSESSIONMOVED) {
                     return "Session moved to a different server";
                 }
                 return "UNKNOWN_EVENT_TYPE";
            }

        }
    }
}
