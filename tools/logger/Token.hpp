#ifndef __TOOLS_LOGGER_TOKEN_HPP__
#define __TOOLS_LOGGER_TOKEN_HPP__

namespace Tools {

    namespace Logger {

        struct Token
        {
            private:
                char _;

                Token(Token const&);
                Token& operator =(Token const&);
            public:
                Token() {}
        };

        extern const Token flush;
        extern const Token endl;

    }


    using Logger::flush;
    using Logger::endl;
}

#endif
