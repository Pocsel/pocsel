#ifndef __TOOLS_LOGGER_WRITER_HPP__
#define __TOOLS_LOGGER_WRITER_HPP__

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tools/logger/Token.hpp"
#include "tools/ToString.hpp"

namespace Tools { namespace Logger {

    struct Writer
    {
    public:
        struct Buffer
        {
        private:
            Writer& _writer;
            std::stringstream _stream;

        public:
            template<class T>
            Buffer(Writer& writer, T value)
                : _writer(writer)
            {
                this->_stream << ToString(value);
            }

            Buffer(Buffer&& buffer)
                : _writer(buffer._writer)
            {
                this->_stream << buffer._stream.str();
                buffer._stream.clear();
            }

            Buffer(Buffer& buffer)
                : _writer(buffer._writer)
            {
                this->_stream << buffer._stream.str();
                buffer._stream.clear();
            }

            ~Buffer()
            {
                this->_writer.Write(this->_stream.str());
            }

            Buffer& operator <<(Token const& t)
            {
                if (&t == &flush)
                {
                    this->_writer.Write(this->_stream.str());
                    this->_stream.clear();
                }
                if (&t == &endl)
                    this->_stream << std::endl;
                return *this;
            }

            Buffer& operator <<(Int8 value)
            {
                this->_stream << ToString(value);
                return *this;
            }

            Buffer& operator <<(Uint8 value)
            {
                this->_stream << ToString(value);
                return *this;
            }

            Buffer& operator <<(int value)
            {
                this->_stream << value;
                return *this;
            }

            template<class T>
            Buffer& operator <<(T value)
            {
                this->_stream << ToString(value);
                return *this;
            }
        };

    private:
        std::ostream* _output;
        std::ostream* _fileOutput;

    protected:
        Writer(std::string const& file)
            : _fileOutput(new std::ofstream(file.c_str()))
        {
        }

        Writer(std::ostream& stream, std::string const& file)
            : _output(&stream),
            _fileOutput(new std::ofstream(file.c_str()))
        {
        }

        ~Writer()
        {
            Delete(this->_fileOutput);
        }

    public:
        void Write(std::string const& data)
        {
            if (this->_output)
                *this->_output << data  << std::flush;
            if (this->_fileOutput)
                *this->_fileOutput << data << std::flush;
        }

        void WriteLine(std::string const& line)
        {
            this->Write(line + "\n");
        }

        template<class T>
        Buffer operator <<(T value)
        {
            return Buffer(*this, value);
        }
    };

}}

#endif
