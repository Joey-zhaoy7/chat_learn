#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

/*
 * 一个类的static变量一定要被初始化的，而目前 只是声明
 * 如果这不是一个模板类 初始化应该放在cpp中
 * 如果这是一个模板类 就需要放到.h文件中
 */

template <typename T>
class Singleton{
protected:
    /*
     * 为什么构造函数设置成protected？ 希望子类能够构造基类
     * 为什么是default？
     *
     */
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>& st) = delete;

    //不希望手动进行回收 使用智能指针
    //为什么是静态的， 所有对象使用同一个实例
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        // static?
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
            // 为什么不用make_shared()？make_shared无法访问私有的构造函数
            _instance = std::shared_ptr<T>(new T);
        });

        return _instance;
    }

    void PrintAddress(){
        std::cout << _instance.get() << std::endl;
    }

    ~Singleton(){
        std::cout<<"this is singleton destruct" << std::endl;
    }

};

template<typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;
#endif // SINGLETON_H
