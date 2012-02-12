#ifndef __CLIENT_OPTIONS_HPP__
#define __CLIENT_OPTIONS_HPP__

namespace Client {

    class Options
    {
        public:
            std::string port;
            std::string host;
            boost::filesystem::path confdir;

        public:
            Options(int ac, char** av);
    };

}

#endif
