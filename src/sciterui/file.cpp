#include "file.h"
#include <Windows.h>

namespace SciterUI
{

File::File() :
    m_file(INVALID_HANDLE_VALUE)
{
}

File::~File()
{
    if (m_file != INVALID_HANDLE_VALUE)
    {
        Close();
    }
}

bool File::Open(const char* fileName, uint32_t openFlags)
{
    if (!Close())
    {
        return false;
    }

    if (fileName == nullptr || strlen(fileName) == 0)
    {
        return false;
    }

    m_file = INVALID_HANDLE_VALUE;

    ULONG dwAccess = 0;
    switch (openFlags & 3)
    {
    case modeRead:
        dwAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwAccess = GENERIC_READ | GENERIC_WRITE;
        break;
    default:
        return false;
    }

    ULONG shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    if ((openFlags & shareDenyWrite) == shareDenyWrite)
    {
        shareMode &= ~FILE_SHARE_WRITE;
    }
    if ((openFlags & shareDenyRead) == shareDenyRead)
    {
        shareMode &= ~FILE_SHARE_READ;
    }
    if ((openFlags & shareExclusive) == shareExclusive)
    {
        shareMode = 0;
    }

    // map modeNoInherit flag
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = nullptr;
    sa.bInheritHandle = (openFlags & modeNoInherit) == 0;

    // map creation flags
    ULONG createFlag = OPEN_EXISTING;
    if (openFlags & modeCreate)
    {
        createFlag = ((openFlags & modeNoTruncate) != 0) ? OPEN_ALWAYS : CREATE_ALWAYS;
    }

    HANDLE hFile = ::CreateFileA(fileName, dwAccess, shareMode, &sa, createFlag, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    m_file = hFile;
    return true;
}

bool File::Close()
{
    bool bError = true;
    if (m_file != INVALID_HANDLE_VALUE)
    {
        bError = !::CloseHandle(m_file);
    }
    m_file = INVALID_HANDLE_VALUE;
    return bError;
}

uint32_t File::GetLength() const
{
    DWORD hiWord = 0;
    return GetFileSize(m_file, &hiWord);
}

uint32_t File::Read(void* lpBuf, uint32_t nCount)
{
    if (nCount == 0)
    {
        return 0;
    }

    DWORD read = 0;
    if (!::ReadFile(m_file, lpBuf, nCount, &read, nullptr))
    {
        return 0;
    }
    return (uint32_t)read;
}

}