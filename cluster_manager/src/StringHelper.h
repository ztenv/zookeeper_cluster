/**
 * @file StringHelper.h
 * @brief 字符串辅助类
 * @author shlian
 * @version 1.0
 * @date 2018-10-19
 */

#pragma once
#include <vector>
#include <string>
#include <cstring>

namespace uvframe{
    namespace baseutils{
        using std::string;
        using std::vector;

        /**
        * @brief 字符串辅助类
        * @author shlian
        */
        class StringHelper final
        {
        public:
            StringHelper()=delete;
            ~StringHelper()=delete;

            /**
            * @brief 按token分割字符串
            *
            * @tparam func 函数的类型:可以是任何支持operator()(const char *)的类型，如：仿函数、普通函数、lambda表达式
            * @param[in] str 待分割的字符串
            * @param[in] f 回调的(仿)函数,函数签名: void (const char * value);
            * @param[in] token 分割符,必须为非NULL
            * @param[in] ignoreEmpty 是否忽略结果集中空的字符串
            *
            * @return 0:成功，-1：token非法
            */
            template<typename func>
            inline static int Split(const std::string &str,func f,const char *token=",",bool ignoreEmpty=false)
            {
                int res=-1;
                if(token!=NULL)
                {
                    size_t pos=0,index=str.find(token),tokenLen=std::string(token).length();
                    char *pdata=new (std::nothrow)char[str.size()+1];
                    if(pdata!=NULL)
                    {
                        strcpy(pdata,str.c_str());
                        do{
                            if(index!=std::string::npos)
                            {
                                //f(str.substr(pos,index-pos).c_str());
                                pdata[index]=0;
                                if((!ignoreEmpty)||((ignoreEmpty)&&((pdata+pos)[0]!=0)))
                                {
                                    f(pdata+pos);
                                }
                            }
                            else
                            {
                                //f(str.substr(pos).c_str());
                                if((!ignoreEmpty)||((ignoreEmpty)&&((pdata+pos)[0]!=0)))
                                {
                                    f(pdata+pos);
                                }
                                break;
                            }
                            pos=index+tokenLen;
                            index=str.find(token,pos);
                        } while(true);
                        delete[]pdata;
                        pdata=0;
                        res=0;
                    }
                }
                return res;
            }

            /**
            * @brief 按token分割字符串
            *
            * @param[in] str 待分隔的字符串
            * @param[in] result 按token分割后的子串的结果集
            * @param[in] token 分割符
            * @param[in] ignoreEmpty 是否忽略结果集中空的字符串
            *
            * @return 0:成功，-1：token非法
            */
            inline static int Split(const std::string &str,std::vector<std::string> &result,const char *token=",",bool ignoreEmpty=false)
            {
                result.reserve(100);
                int res=ignoreEmpty?Split(str,split_ignore_empty(result),token):Split(str,split_functor(result),token);
                return res;
            }

            /**
            * @brief 删除str左边的token指定的任意字符，默认删除str左边的空格字符、\r字符、\n字符、\t字符
            *
            * @param[in] str 待处理的字符串
            * @param[in] token  需要从str中删除的字符的集合
            *
            * @return 删除token指定字符之后的字符串
            */
            inline static std::string & LeftStrip(std::string &str,const char *token=" \r\n\t")
            {
                if(token==NULL)
                {
                    return str;
                }
                return str.erase(0,str.find_first_not_of(token));
            }

            /**
            * @brief 删除str右边的token指定的任意字符，默认删除str右边的空格字符、\r字符、\n字符、\t字符
            *
            * @param[in] str 待处理的字符串
            * @param[in] token  需要从str中删除的字符的集合
            *
            * @return 删除token指定字符之后的字符串
            */
            inline static std::string & RightStrip(std::string &str,const char *token=" \r\n\t")
            {
                if(token==NULL)
                {
                    return str;
                }
                return str.erase(str.find_last_not_of(token)+1);
            }

            /**
            * @brief 删除str左边、右边的token指定的任意字符，默认删除str左边、右边的空格字符、\r字符、\n字符、\t字符
            *
            * @param[in] str 待处理的字符串
            * @param[in] token  需要从str中删除的字符的集合
            *
            * @return 删除token指定字符之后的字符串
            */
            inline static std::string & Strip(std::string &str,const char *token=" \r\n\t")
            {
                if(token==NULL)
                {
                    return str;
                }
                return LeftStrip(RightStrip(str,token),token);
            }

        private:
            typedef std::vector<std::string> StringVector;
            /**
            * @brief split使用的仿函数，将分割的子串存储到vector中
            */
            class split_functor
            {
            public:
                split_functor(StringVector &result):m_sv(result)
                {
                }
                void operator()(const char *value)
                {
                    m_sv.push_back(std::string(value));
                }
            private:
                StringVector &m_sv;
            };

            /**
            * @brief split使用的仿函数，将分割的非空的子串存储到vector中
            */
            class split_ignore_empty
            {
            public:
                split_ignore_empty(StringVector &result):m_sv(result)
                {
                }
                void operator()(const char *value)
                {
                    if((value!=nullptr)&&(strlen(value)>0))
                    {
                        m_sv.push_back(value);
                    }
                }
            private:
                StringVector & m_sv;
            };
        };
    }
}

