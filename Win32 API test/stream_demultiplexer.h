#pragma once

// From https://stackoverflow.com/a/13978717/68086

#include <vector>
#include <ostream>

template<typename StreamType>
class stream_demultiplexer
{
private:
    typedef std::vector<StreamType*> str_cont;
    str_cont d;
public:
    stream_demultiplexer& put(typename StreamType::char_type ch)
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (*it)->put(ch);
        return *this;
    }

    stream_demultiplexer& write(typename const StreamType::char_type* s, std::streamsize count)
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (*it)->write(s, count);
        return *this;
    }

    stream_demultiplexer& flush()
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (*it)->flush();
        return *this;
    }


    template<typename T>
    stream_demultiplexer& operator<<( const T& obj )
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (**it) << obj;
        return *this;
    }

    stream_demultiplexer& operator<<(std::ios_base& (*func)(std::ios_base&))
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (**it) << func;
        return *this;
    }

    template<typename CharT, typename Traits>
    stream_demultiplexer& operator<<(std::basic_ios<CharT,Traits>& (*func)(std::basic_ios<CharT,Traits>&) )
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (**it) << func;
        return *this;
    }

    stream_demultiplexer& operator<<(StreamType& (*func)(StreamType&) )
    {
        for(typename str_cont::iterator it = d.begin(); it != d.end(); ++it)
            (**it) << func;
        return *this;
    }

    void add_stream(StreamType& ss)
    {
        d.push_back(&ss);
    }
};
