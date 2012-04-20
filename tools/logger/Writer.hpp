#ifndef __TOOLS_LOGGER_WRITER_HPP__
#define __TOOLS_LOGGER_WRITER_HPP__

namespace Tools { namespace Logger {

    template<class T> struct Writer;

    namespace {
        template<class TWriter>
        struct Buffer : public std::stringstream
        {
        private:
            TWriter& _writer;

        public:
            //template<class T>
            Buffer(TWriter& writer)
                : _writer(writer)
            {
            }

            template<class T>
            Buffer(TWriter& writer, T value)
                : std::stringstream(),
                _writer(writer)
            {
                *this << value;
            }

            Buffer(Buffer&& buffer) :
#ifdef _MSC_VER
                std::stringstream(std::move(buffer)),
#else
                std::stringstream(buffer.str()),
#endif
                _writer(buffer._writer)
            {
#ifndef _MSC_VER
                this->flags(buffer.flags());
                this->precision(buffer.precision());
                this->width(buffer.width());
                buffer.str("");
#endif
            }

            ~Buffer()
            {
                this->_writer.Write(this->str());
            }

            Buffer& operator <<(std::basic_ostream<char>& (*manip)(std::basic_ostream<char>&))
            {
                manip(*this);
                return *this;
            }

            Buffer& operator <<(std::basic_ios<char>& (*manip)(std::basic_ios<char>&))
            {
                manip(*this);
                return *this;
            }

            Buffer& operator <<(std::ios_base& (*manip)(std::ios_base&))
            {
                manip(*this);
                return *this;
            }

            template<class T>
            Buffer& operator <<(T&& value)
            {
                *(std::stringstream*)this << value;
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
        std::list<std::function<void(std::string const&)>> _callbacks;

    public:
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
            for (auto it = this->_callbacks.begin(), ite = this->_callbacks.end(); it != ite; ++it)
                (*it)(data);
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

        template<class T>
        Buffer<thisType> operator <<(T& (*manip)(T&))
        {
            Buffer<thisType> tmp(*this);
            tmp << manip;
            return tmp;
        }

        void RegisterCallback(std::function<void(std::string const&)>&& callback)
        {
            this->_callbacks.push_back(callback);
        }
    };


    template<>
    struct Writer<void>
    {
    private:
        std::ostream* _output;
        std::ostream* _fileOutput;
        std::list<std::function<void(std::string const&)>> _callbacks;

    public:
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
            for (auto it = this->_callbacks.begin(), ite = this->_callbacks.end(); it != ite; ++it)
                (*it)(data);
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

        template<class T>
        Buffer<Writer<void>> operator <<(T& (*manip)(T&))
        {
            Buffer<Writer<void>> tmp(*this);
            tmp << manip;
            return tmp;
        }

        void RegisterCallback(std::function<void(std::string const&)>&& callback)
        {
            this->_callbacks.push_back(callback);
        }
    };

}}

#endif
