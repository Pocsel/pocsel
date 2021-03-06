#ifndef __TOOLS_FILESYSTEM_HPP__
#define __TOOLS_FILESYSTEM_HPP__

#define BOOST_FILESYSTEM_NO_DEPRECATED 1
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace Tools {

    class Filesystem
    {
    public:

        // Remove the quotes from a string
        static void RemoveQuotes(std::string& str)
        {
            size_t n;
            while ((n = str.find('\"')) != std::string::npos)
                str.erase(n, 1);
        }

        // Get's the size of the file in bytes.
        static std::streamoff GetFileLength(std::istream& file)
        {
            auto pos = file.tellg();
            file.seekg(0, std::ios::end);
            auto length = file.tellg();
            // Restore the position of the get pointer
            file.seekg(pos);

            return length;
        }

        // Ignore everything else that comes on the line, up to 'length' characters.
        static void IgnoreLine(std::istream& file, int length)
        {
            file.ignore(length, '\n');
        }

        static void ReplaceBackslashes(std::string& str)
        {
            char const* cStr = str.c_str();

            for (unsigned int i = 0; i < str.size(); ++i)
            {
                if (cStr[i] == '\\')
                    str.replace(i, 1, "/");
            }
        }

    };

}

#endif
