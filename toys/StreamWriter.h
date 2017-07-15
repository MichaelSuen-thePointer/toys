#pragma once

template<class TStream, bool VClosable>
class StreamWriter
{
private:
    static constexpr int DEFAULT_BUFFER_SIZE = 1024;
    static constexpr int DEFAULT_FILE_STREAM_BUFFER_SIZE = 4096;
    static constexpr int MIN_BUFFER_SIZE = 128;

    using byte = unsigned char;

    TStream _stream;
    byte* _byteBuffer;
    wchar_t* _charBuffer;
    int _charPos;
    int _charLen;
    bool _autoFlush;

public:
};