#ifndef __COMMON_ENV_HPP__
#define __COMMON_ENV_HPP__

namespace Common {

    class Env : boost::noncopyable
    {
    public:
        struct Variable;
        typedef boost::function<void(std::string const& key, Variable& val)> OnNewCallbackType;
        typedef boost::function<void(std::string const& key, Variable& val)> OnChangeCallbackType;

        struct Variable : private boost::noncopyable
        {
        public:
            enum Type {String = 0, Int, Float};

        private:
            std::list<OnChangeCallbackType> _onChangeCallbacks;
            std::string _name;
            bool _doNotFire;

        public:
            Variable(std::string const& name) :
                _onChangeCallbacks(), _name(name), _doNotFire(false) {}
            virtual ~Variable() {}

            virtual Type GetType() const = 0;
            virtual operator std::string const&() = 0;
            virtual operator Int32() = 0;
            virtual operator float() = 0;

            virtual Variable& operator =(std::string const&) = 0;
            virtual Variable& operator =(Int32) = 0;
            virtual Variable& operator =(float) = 0;
            virtual bool operator ==(std::string const&) const = 0;
            virtual bool operator ==(Int32) const = 0;
            virtual bool operator ==(float) const = 0;

            // non virtual functions
            inline bool operator !=(std::string const& s) const     { return !this->operator ==(s); }
            inline bool operator !=(Int32 i) const                  { return !this->operator ==(i); }
            inline bool operator !=(float f) const                  { return !this->operator ==(f); }
            inline void PushCallback(OnChangeCallbackType cb)       { this->_onChangeCallbacks.push_back(cb); }
            inline void ClearCallbacks()                            { this->_onChangeCallbacks.clear(); }

        protected:
            virtual void _Write(std::ostream& out) const = 0;
            void _FireCallbacks();

            friend std::ostream& operator <<(std::ostream& out, Variable const& var);
        };

    private:
        OnNewCallbackType _onNew;
        std::unordered_map<std::string, Variable*> _map;

    public:
        Env(OnNewCallbackType onNew = 0);
        virtual ~Env();

        Variable& Get(std::string const& key);
        template<typename T> void Set(std::string const& key, T const& val);
        void Set(std::string const& key, char const* val);
        bool HasKey(std::string const& key);
    };

}

namespace Tools {

    template<>
    struct Stringify< Common::Env::Variable::Type >
    {
        static inline std::string MakeString(Common::Env::Variable::Type type)
        {
            static std::string const strings[] = {"String", "Int", "Float"};
            return strings[type];
        }
    };

}

#endif
