#ifndef __TOOLS_MESSAGEQUEUE_HPP__
#define __TOOLS_MESSAGEQUEUE_HPP__

#include "tools/AbstractMessageQueue.hpp"

namespace Tools {

    template<typename T>
    class MessageQueue : public AbstractMessageQueue
    {
    public:
        typedef std::function<void(void)> Message;

    private:
        template<typename SelfMethod, typename ResponseMethod>
        struct ResponseFunctor
        {
        private:
            template<typename _T, typename _MR>  struct Caller  {
                static inline void call(ResponseFunctor& _this) {
                    _this.responseMethod(_this.selfMethod());
                }
            };

            template<typename _T, typename _MR>  struct Caller<std::unique_ptr<_T>, _MR>  {
                static inline void call(ResponseFunctor& _this) {
                    _this.responseMethod(std::move(_this.selfMethod()));
                }
            };

            template<typename _MR>  struct Caller<void, _MR>  {
                static inline void call(ResponseFunctor& _this) {
                    _this.selfMethod();
                    _this.responseMethod();
                }
            };

            typedef Caller<typename SelfMethod::result_type, typename ResponseMethod::result_type> ThisCaller;

        public:
            SelfMethod      selfMethod;
            ResponseMethod  responseMethod;

        public:
            ResponseFunctor(SelfMethod selfMethod,
                                  ResponseMethod responseMethod) :
                selfMethod(selfMethod),
                responseMethod(responseMethod)
            {}

            void operator ()()
            {
                ThisCaller::call(*this);
            }
        };

    private:
        T* _self;

    protected:
        void PushCall(Message message)
        { _PushMessage(message); }


        ///////////////////////////////////////////////////////////////////////
        /// PushCall(SelfMethod, SelfArgs..., ResponseQueueMethod)
        template<typename ReturnType, typename MethodReturnType>
        void PushCall(ReturnType (T::*method)(), std::function<MethodReturnType(ReturnType)> response)
        {
            auto selfMethod = std::bind(method, _self);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename ReturnType, typename P1, typename MethodReturnType>
        void PushCall(ReturnType (T::*method)(P1), P1 p1, std::function<MethodReturnType(ReturnType)> response)
        {
            auto selfMethod = std::bind(method, _self, p1);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename ReturnType, typename P1, typename P2, typename MethodReturnType>
        void PushCall(ReturnType (T::*method)(P1, P2), P1 p1 , P2 p2, std::function<MethodReturnType(ReturnType)> response)
        {
            auto selfMethod = std::bind(method, _self, p1, p2);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename ReturnType, typename P1, typename P2, typename P3, typename MethodReturnType>
        void PushCall(ReturnType (T::*method)(P1, P2, P3), P1 p1 , P2 p2, P3 p3, std::function<MethodReturnType(ReturnType)> response)
        {
            auto selfMethod = std::bind(method, _self, p1, p2, p3);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }


        ///////////////////////////////////////////////////////////////////////
        /// PushCall(SelfMethod, SelfArgs..., ResponseQueueFunctor)
        template<typename MethodReturnType>
        void PushCall(void (T::*method)(), std::function<MethodReturnType(void)> response)
        {
            auto selfMethod = std::bind(method, _self);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename P1, typename MethodReturnType>
        void PushCall(void (T::*method)(P1), P1 p1, std::function<MethodReturnType(void)> response)
        {
            auto selfMethod = std::bind(method, _self, p1);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename P1, typename P2, typename MethodReturnType>
        void PushCall(void (T::*method)(P1, P2), P1 p1 , P2 p2, std::function<MethodReturnType(void)> response)
        {
            auto selfMethod = std::bind(method, _self, p1, p2);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        template<typename P1, typename P2, typename P3, typename Methodvoid>
        void PushCall(void (T::*method)(P1, P2, P3), P1 p1 , P2 p2, P3 p3, std::function<Methodvoid(void)> response)
        {
            auto selfMethod = std::bind(method, _self, p1, p2, p3);
            typedef ResponseFunctor<decltype(selfMethod), decltype(response)> FunctorType;
            this->_PushMessage(FunctorType(selfMethod, response));
        }

        ///////////////////////////////////////////////////////////////////////
        // PushCall(SelfMethod, Args...)
        template<typename MethodReturnType>
        void PushCall(MethodReturnType (T::*method)())
        { _PushMessage(std::bind(method, _self)); }

        template<typename MethodReturnType, typename P1>
        void PushCall(MethodReturnType (T::*method)(P1), P1 p1)
        { _PushMessage(std::bind(method, _self, p1)); }

        template<typename MethodReturnType, typename P1, typename P2>
        void PushCall(MethodReturnType (T::*method)(P1, P2), P1 p1, P2 p2)
        { _PushMessage(std::bind(method, _self, p1, p2)); }

        template<typename MethodReturnType, typename P1, typename P2, typename P3>
        void PushCall(MethodReturnType (T::*method)(P1, P2, P3), P1 p1, P2 p2, P3 p3)
        { _PushMessage(std::bind(method, _self, p1, p2, p3)); }

        template<typename MethodReturnType, typename P1, typename P2, typename P3, typename P4>
        void PushCall(MethodReturnType (T::*method)(P1, P2, P3, P4), P1 p1, P2 p2, P3 p3, P4 p4)
        { _PushMessage(std::bind(method, _self, p1, p2, p3, p4)); }

    protected:
        MessageQueue(T* self, unsigned int nbThread = 1) :
            AbstractMessageQueue(nbThread),
            _self(self)
        {
        }
    };

}


/////////////////////////////////////////////////////////////////////////////
//
// Declare queued method with following macros :
//   TOOLS_MQ_WRAP(ReturnType, ClassName, MethodName)
//   TOOLS_MQ_WRAP1(ReturnType, ClassName, MethodName, Arg1Type, Arg1Name)
//   TOOLS_MQ_WRAPN(ReturnType, ClassName, MethodName, Arg1Type, Arg1Name, ..., ArgNType, ArgNName)
//
// For method that need a callback, you can use :
//  TOOLS_MQ_CBWRAP(ReturnType, ClassName, MethodName, CallbackType)
//  TOOLS_MQ_CBWRAP1(ReturnType, ClassName, MethodName, Arg1Type, Arg1Name, CallbackType)
//
// You can use these macros directly in your class déclaration,
// like this :
//
// class MyClass : protected Tools::MessageQueue<MyClass>
// {
//      MyClass() : Tools::MessageQueue<MyClass>(this, 1) {} // 1 thread/instance
//
//      // Simple int Myclass::simpleMethod(void) declaration
//      TOOLS_MQ_WRAP(int, MyClass, simpleMethod)
//      {
//          return 42;
//      }
//
//      // Simple int MyClass::simpleMethodWithParam(float) declaration
//      TOOLS_MQ_WRAP1(int, MyClass, simpleMethodWithParam, float, f)
//      {
//          std::cout << "got f = " << f << ".\n";
//      }
//
//      // Simple int Myclass::notImplemetedHereMethod(void) declaration
//      TOOLS_MQ_WRAP(int, MyClass, notImplementedHereMethod);
//
// };
//
//
//  // Note the underscore before the function name
//  int MyClass::_notImplementedHereMethod(void)
//  {
//      return 0;
//  }


/////////////////////////////////////////////////////////////////////////////
#define TOOLS_MQ_WRAP(return_type, class, name) \
public: \
    void name() \
    { \
        this->PushCall(&class::_##name); \
    } \
private: \
    return_type _##name()

/////////////////////////////////////////////////////////////////////////////
#define TOOLS_MQ_WRAP1(return_type, class, name, var1_type, var1_name) \
public: \
    void name(var1_type var1_name) \
    { \
        this->PushCall(&class::_##name, var1_name); \
    } \
private: \
    return_type _##name(var1_type var1_name)

/////////////////////////////////////////////////////////////////////////////
#define TOOLS_MQ_WRAP2(return_type, class, name, var1_type, var1_name, var2_type, var2_name) \
public: \
    void name(var1_type var1_name, var2_type var2_name) \
    { \
        this->PushCall(&class::_##name, var1_name, var2_name); \
    } \
private: \
    return_type _##name(var1_type var1_name, var2_type var2_name)

/////////////////////////////////////////////////////////////////////////////
#define TOOLS_MQ_CBWRAP(return_type, class, name, cb_type) \
public: \
    void name(std::function<cb_type> response) \
    { \
        this->PushCall(&class::_##name, response); \
    } \
private: \
    return_type _##name()

/////////////////////////////////////////////////////////////////////////////
#define TOOLS_MQ_CBWRAP1(return_type, class, name, arg1_type, arg1_name, cb_type) \
public: \
    void name(arg1_type arg1_name, std::function<cb_type> response) \
    { \
        this->PushCall(&class::_##name, arg1_name, response); \
    } \
private: \
    return_type _##name(arg1_type arg1_name)

#endif
