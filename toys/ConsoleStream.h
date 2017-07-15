#pragma once

#define NOMINMAX

#include <Windows.h>
#include <cassert>
#include <string>

template<bool VCanRead, bool VCanWrite>
class ConsoleStream
{
protected:
    static constexpr int DEFAULT_BUFFER_SIZE = 128;

private:
    static constexpr int _ERROR_BROKEN_PIPE = ERROR_BROKEN_PIPE;

    static constexpr int _ERROR_NO_DATA = ERROR_NO_DATA;

    HANDLE _handle;
    
public:
    ConsoleStream(HANDLE handle)
        : _handle(handle)
    {
    }
    ConsoleStream(const ConsoleStream&) = delete;

    ConsoleStream& operator=(const ConsoleStream&) = delete;

    ConsoleStream(ConsoleStream&& other)
        : _handle(other._handle)
    {
        other._handle = INVALID_HANDLE_VALUE;
    }

    ConsoleStream& operator=(ConsoleStream&& other)
    {
        _handle = other._handle;
        other._handle = INVALID_HANDLE_VALUE;

        return *this;
    }
public:
    ~ConsoleStream()
    {
        CloseHandle(_handle);
    }

    constexpr bool can_read() const { return VCanRead; }

    constexpr bool can_write() const { return VCanWrite; }

    void flush()
    {
        assert(_handle != nullptr);
        assert(VCanWrite == true);
    }

    size_t read(byte* buffer, size_t count)
    {
        assert(buffer != nullptr);
        static_assert(VCanRead == true, "Read is not supported");

        int errCode = 0;
        size_t result = read_file_internal(_handle, buffer, count, errCode);
        if(result == -1)
        {
            throw std::runtime_error(std::to_string(errCode));
        }
        return result;
    }

    void write(byte* buffer, size_t count)
    {
        assert(buffer != nullptr);
        static_assert(VCanWrite == true, "Write is not supported");

        int errCode = 0;
        size_t result = write_file_internal(_handle, buffer, count, errCode);
        if (result == -1)
        {
            throw std::runtime_error(std::to_string(errCode));
        }
    }
private:
    static size_t read_file_internal(HANDLE handle, byte* buffer, size_t count, int& outErrorCode)
    {
        if(count == 0)
        {
            return 0;
        }
        DWORD numBytesRead;
        static_assert(sizeof(DWORD) <= sizeof(size_t), "size_t should be greater equal than DWORD");
        int r = ReadFile(handle, buffer, count, &numBytesRead, nullptr);
        if(r == 0)
        {
            outErrorCode = GetLastError();
            if(outErrorCode == _ERROR_BROKEN_PIPE)
            {
                return 0;
            }
            return -1;
        }
        outErrorCode = 0;
        return numBytesRead;
    }

    static size_t write_file_internal(HANDLE handle, byte* buffer, size_t count, int& outErrorCode)
    {
        if(count == 0)
        {
            outErrorCode = 0;
            return 0;
        }

        DWORD numBytesWritten = 0;
        static_assert(sizeof(DWORD) <= sizeof(size_t), "size_t should be greater equal than DWORD");
        int r = WriteFile(handle, buffer, count, &numBytesWritten, nullptr);
        if(r == 0)
        {
            outErrorCode = GetLastError();
            if(outErrorCode == _ERROR_NO_DATA || outErrorCode == _ERROR_BROKEN_PIPE)
            {
                return 0;
            }
            return -1;
        }
        outErrorCode = 0;
        
        return numBytesWritten;
    }
};
