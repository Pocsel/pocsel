#include "server/rcon/ToJsonStr.hpp"

namespace Server { namespace Rcon {

    std::string ToJsonStr(std::string const& str)
    {
        std::string ret;
        auto it = str.begin();
        auto itEnd = str.end();
        for (; it != itEnd; ++it)
            switch (*it)
            {
                // standard JSON
                case '"':
                    ret += "\\u0022";
                    break;
                case '\\':
                    ret += "\\u005c";
                    break;
                case '/':
                    ret += "\\u002f";
                    break;
                case '\b':
                    ret += "\\u0008";
                    break;
                case '\f':
                    ret += "\\u000c";
                    break;
                case '\n':
                    ret += "\\u000a";
                    break;
                case '\r':
                    ret += "\\u000d";
                    break;
                case '\t':
                    ret += "\\u0009";
                    break;
                // pas standard mais cool pour pas faire chier le client
                case '\'':
                    ret += "&apos;";
                    break;
                case '&':
                    ret += "&amp;";
                    break;
                case '<':
                    ret += "&lt;";
                    break;
                case '>':
                    ret += "&gt;";
                    break;
                default:
                    ret += *it;
            }
        return ret;
    }

}}
