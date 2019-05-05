/**
 * @file cluster_interface.h
 * @brief 集群管理器接口
 * @author shlian
 * @version 1.0
 * @date 2019-04-23
 */
#include <memory>
#include <string>

#ifdef _WIN32
    #define DECL_IMPORTS __declspec(dllimport)
    #define DECL_EXPORTS __declspec(dllexport)
#else
    #define DECL_IMPORTS __attribute__((visibility("default")))
    #define DECL_EXPORTS __attribute__((visibility("default")))
#endif

#ifdef CLUSTER_LIB
    # define CLUSTER_EXPORT DECL_EXPORTS
#else
    # define CLUSTER_EXPORT DECL_IMPORTS
#endif

namespace uvframe{
    namespace baseutils{

        /**
         * @brief 事件处理器
         */
        class CLUSTER_EXPORT IEventHandler{
        public:
            IEventHandler(){}
            virtual ~IEventHandler(){}

            /**
             * @brief 主从变动事件
             *
             * @param[in] isMaster :true,当前结点是集群的主结点；false:当前结点是集群的从
             * 结点
             *
             * @note 此方法不能抛出异常，此方法是被回调线程调用的，注意线程安全。
             */
            virtual void OnMasterEvent(bool isMaster)=0;
        };
        typedef std::shared_ptr<IEventHandler> IEventHandlerPtr;

        /**
         * @brief 结点的模式
         */
        enum class EN_NodeMode{
            /**
             * @brief 主结点工作模式
             */
            Leader=0,
            /**
             * @brief 从结点工作模式
             */
            Follower=1,
            /**
             * @brief 其他模式,如：初始、失联等
             */
            Other=2
        };

        /**
         * @brief 集群管理器接口
         */
        class CLUSTER_EXPORT IClusterManager{
        public:
            IClusterManager(){}
            virtual ~IClusterManager(){}

            /**
             * @brief 初始化集群资源
             *
             * @param[in] hosts   集群的地址和端口，形如：ip:port,ip1:port1,ip2:port2
             * @param[in] path    结点的注册路径，同一集群所有结点的注册路径必须相同，
             *形如：/xunce/adc
             * @param[in] nodeName 当前结点的名称，同一集群所有结点的名称必须唯一
             * @param[in] timeout 超时时间，单位为秒
             *
             * @return 0:成功，非0:失败
             */
            virtual int Initialize(const std::string &hosts,const std::string &path,
                                   const std::string &nodeName,unsigned int timeout)=0;

            /**
             * @brief 注册事件处理器
             *
             * @param[in] eventHandlerPtr 事件处理器的shared_ptr
             *
             * @return 0:成功，非0:失败
             */
            virtual int Register(IEventHandlerPtr eventHandlerPtr)=0;

            /**
             * @brief 启动集群管理
             *
             * @return 0:成功，非0:失败
             */
            virtual int Start()=0;;

            /**
             * @brief 退出集群（当业务出现异常需要退出集群时需要手动调用此方法）
             *
             * @return 0:成功，非0:失败
             */
            virtual int Stop()=0;

            /**
             * @brief 释放集群资源
             *
             * @return 0:成功，非0:失败
             */
            virtual int DeInitialize()=0;

            /**
             * @brief 结点的工作模式
             *
             * @return EN_NodeMode
             */
            virtual EN_NodeMode getMode()=0;
        };
        typedef std::shared_ptr<IClusterManager> IClusterManagerPtr;

        /**
         * @brief ClusterManager工厂
         */
        class CLUSTER_EXPORT  ClusterManagerFactory final
        {
        public:
            ClusterManagerFactory()=default;
            ~ClusterManagerFactory()=default;

            ClusterManagerFactory(const ClusterManagerFactory&)=delete;
            ClusterManagerFactory(ClusterManagerFactory &&)=delete;

            ClusterManagerFactory & operator=(const ClusterManagerFactory &)=delete;
            ClusterManagerFactory & operator=(ClusterManagerFactory &&)=delete;

            /**
             * @brief 创建ClusterManager的实例，并以IClusterManagerPtr的形式返回
             *
             * @return IClusterManagerPtr的智能指针
             */
            static IClusterManagerPtr create();
        };
    }
}

