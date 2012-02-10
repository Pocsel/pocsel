#ifndef __TOOLS_LOGGER_WRITER_HPP__
#define __TOOLS_LOGGER_WRITER_HPP__

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "tools/logger/Token.hpp"
#include "tools/ToString.hpp"

namespace Tools { namespace Logger {

    template<class T> struct Writer;

    namespace {
        template<class TWriter>
        struct Buffer
        {
        private:
            TWriter& _writer;
            std::stringstream _stream;

        public:
            template<class T>
            Buffer(TWriter& writer)
                : _writer(writer)
            {
            }

            template<class T>
            Buffer(TWriter& writer, T value)
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
    }

    template<class TLog>
    struct Writer
    {
    private:
        typedef Writer<TLog> thisType;
        TLog& _parent;
        std::ostream* _output;
        std::ostream* _fileOutput;

    protected:
        Writer(TLog& parent, std::string const& file)
            : _parent(parent),
            _fileOutput(new std::ofstream(file.c_str()))
        {
        }

        Writer(TLog& parent, std::ostream& stream, std::string const& file)
            : _parent(parent), 
            _output(&stream),
            _fileOutput(new std::ofstream(file.c_str()))
        {
        }

        ~Writer()
        {
            Delete(this->_fileOutput);
        }

    public:
        void WriteFile(std::string const& data)
        {
            this->_parent.WriteFile(data);
            if (this->_fileOutput)
                *this->_fileOutput << data << std::flush;
        }

        void Write(std::string const& data)
        {
            this->_parent.WriteFile(data);
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
        Buffer<thisType> operator <<(T value)
        {
            return Buffer<thisType>(*this, value);
        }
    };


    template<>
    struct Writer<void>
    {
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

        void _Write(std::string const& data)
        {
            if (this->_fileOutput)
                *this->_fileOutput << data << std::flush;
        }

    public:
        void WriteFile(std::string const& data)
        {
            if (this->_fileOutput)
                *this->_fileOutput << data << std::flush;
        }

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
        Buffer<Writer<void>> operator <<(T value)
        {
            return Buffer<Writer<void>>(*this, value);
        }
    };
}}

#endif
