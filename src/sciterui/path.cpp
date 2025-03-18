#include "path.h"
#include "std_string.h"
#include <Windows.h>

namespace SciterUI
{
const char DRIVE_DELIMITER = ':';
const char * const DIR_DOUBLEDELIM = "\\\\";
const char DIRECTORY_DELIMITER = '\\';
const char DIRECTORY_DELIMITER2 = '/';

Path::Path()
{
}

Path::Path(const char * path)
{
    m_path = path ? path : "";
    CleanPath(m_path);
}

Path::Path(const char * path, const char * fileName)
{
    SetDriveDirectory(path);
    SetNameExtension(fileName);
}

Path::operator const char *() const
{
    return (const char *)m_path.c_str();
}

Path::operator const std::string &()
{
    return m_path;
}

void Path::GetDirectory(std::string & directory) const
{
    GetComponents(nullptr, &directory);
}

void Path::GetComponents(std::string * drive, std::string * directory, std::string * name, std::string * extension) const
{
    char driveBuff[_MAX_DRIVE + 1] = {0}, dirBuff[_MAX_DIR + 1] = {0}, nameBuff[_MAX_FNAME + 1] = {0}, extBuff[_MAX_EXT + 1] = {0};

    const char * basePath = m_path.c_str();
    const char * driveDir = strrchr(basePath, DRIVE_DELIMITER);
    if (driveDir != nullptr)
    {
        size_t len = sizeof(dirBuff) < (driveDir - basePath) ? sizeof(driveBuff) : driveDir - basePath;
        strncpy(driveBuff, basePath, len);
        basePath += len + 1;
    }

    const char * last = strrchr(basePath, DIRECTORY_DELIMITER);
    if (last != nullptr)
    {
        size_t len = sizeof(dirBuff) < (last - basePath) ? sizeof(dirBuff) : last - basePath;
        if (len > 0)
        {
            strncpy(dirBuff, basePath, len);
        }
        else
        {
            dirBuff[0] = DIRECTORY_DELIMITER;
            dirBuff[1] = '\0';
        }
        strncpy(nameBuff, last + 1, sizeof(nameBuff));
    }
    else
    {
        strncpy(dirBuff, basePath, sizeof(dirBuff));
    }
    char * ext = strrchr(nameBuff, '.');
    if (ext != nullptr)
    {
        strncpy(extBuff, ext + 1, sizeof(extBuff));
        *ext = '\0';
    }

    if (drive)
    {
        *drive = driveBuff;
    }
    if (directory)
    {
        *directory = dirBuff;
    }
    if (name)
    {
        *name = nameBuff;
    }
    if (extension)
    {
        *extension = extBuff;
    }
}

void Path::SetDriveDirectory(const char * driveDirectory)
{
    std::string driveDirectoryStr = driveDirectory;
    if (driveDirectoryStr.length() > 0)
    {
        EnsureTrailingBackslash(driveDirectoryStr);
        CleanPath(driveDirectoryStr);
    }

    std::string name, extension;
    GetComponents(nullptr, nullptr, &name, &extension);
    SetComponents(nullptr, driveDirectoryStr.c_str(), name.c_str(), extension.c_str());
}

void Path::SetDirectory(const char * directory, bool ensureAbsolute)
{
    std::string newDirectory = directory;
    if (ensureAbsolute)
    {
        EnsureLeadingBackslash(newDirectory);
    }
    if (newDirectory.length() > 0)
    {
        EnsureTrailingBackslash(newDirectory);
    }
    std::string drive, name, extension;
    GetComponents(&drive, nullptr, &name, &extension);
    SetComponents(drive.c_str(), newDirectory.c_str(), name.c_str(), extension.c_str());
}

void Path::SetNameExtension(const char * nameExtension)
{
    std::string directory, drive;
    GetComponents(&drive, &directory);
    SetComponents(drive.c_str(), directory.c_str(), nameExtension, nullptr);
}

void Path::SetComponents(const char * drive, const char * directory, const char * name, const char * extension)
{
    char fullname[MAX_PATH];

    memset(fullname, 0, sizeof(fullname));
    if (directory == nullptr || strlen(directory) == 0)
    {
        static char emptyDir[] = {DIRECTORY_DELIMITER, '\0'};
        directory = emptyDir;
    }

    _makepath(fullname, drive, directory, name, extension);
    m_path = fullname;
}

void Path::AppendDirectory(const char * subDirectory)
{
    std::string newSubDirectory = subDirectory;
    if (newSubDirectory.empty())
    {
        return;
    }

    StripLeadingBackslash(newSubDirectory);
    EnsureTrailingBackslash(newSubDirectory);

    std::string drive, directory, name, extension;
    GetComponents(&drive, &directory, &name, &extension);
    EnsureTrailingBackslash(directory);
    directory += newSubDirectory;
    SetComponents(drive.c_str(), directory.c_str(), name.c_str(), extension.c_str());
}

bool Path::FileExists() const
{
    WIN32_FIND_DATA FindData;
    HANDLE hFindFile = FindFirstFile(stdstr(m_path).ToUTF16().c_str(), &FindData);
    bool bSuccess = (hFindFile != INVALID_HANDLE_VALUE);

    if (hFindFile != nullptr) // Make sure we close the search
    {
        FindClose(hFindFile);
    }

    return bSuccess;
}

bool Path::DirectoryExists() const
{
    Path path(m_path.c_str());

    std::string directory;
    path.DirectoryUp(&directory);
    path.SetNameExtension(directory.c_str());

    WIN32_FIND_DATA FindData;
    HANDLE findFile = FindFirstFile(stdstr((const char *)path).ToUTF16().c_str(), &FindData);
    bool res = (findFile != INVALID_HANDLE_VALUE);
    if (findFile != INVALID_HANDLE_VALUE)
    {
        FindClose(findFile);
    }
    return res;
}

void Path::DirectoryUp(std::string * lastDir)
{
    std::string directory;
    GetDirectory(directory);
    StripTrailingBackslash(directory);
    if (directory.empty())
    {
        return;
    }
    std::string::size_type delimiter = directory.rfind(DIRECTORY_DELIMITER);
    if (lastDir != nullptr)
    {
        *lastDir = directory.substr(delimiter);
        StripLeadingBackslash(*lastDir);
    }

    if (delimiter != std::string::npos)
    {
        directory = directory.substr(0, delimiter);
    }
    SetDirectory(directory.c_str());
}

void Path::CleanPath(std::string & path) const
{
    std::string::size_type pos = path.find(DIRECTORY_DELIMITER2);
    while (pos != std::string::npos)
    {
        path.replace(pos, 1, &DIRECTORY_DELIMITER);
        pos = path.find(DIRECTORY_DELIMITER2, pos + 1);
    }

    bool appendEnd = !_strnicmp(path.c_str(), DIR_DOUBLEDELIM, 2);
    pos = path.find(DIR_DOUBLEDELIM);
    while (pos != std::string::npos)
    {
        path.replace(pos, 2, &DIRECTORY_DELIMITER);
        pos = path.find(DIR_DOUBLEDELIM, pos + 1);
    }
    if (appendEnd)
    {
        path.insert(0, stdstr_f("%c", DIRECTORY_DELIMITER).c_str());
    }
}

void Path::EnsureLeadingBackslash(std::string & directory) const
{
    if (directory.empty() || (directory[0] != DIRECTORY_DELIMITER))
    {
        directory = stdstr_f("%c%s", DIRECTORY_DELIMITER, directory.c_str());
    }
}

void Path::EnsureTrailingBackslash(std::string & directory) const
{
    std::string::size_type length = directory.length();

    if (directory.empty() || (directory[length - 1] != DIRECTORY_DELIMITER))
    {
        directory += DIRECTORY_DELIMITER;
    }
}

void Path::StripLeadingBackslash(std::string & directory) const
{
    if (directory.length() <= 1)
    {
        return;
    }

    if (directory[0] == DIRECTORY_DELIMITER)
    {
        directory = directory.substr(1);
    }
}

void Path::StripTrailingBackslash(std::string & directory) const
{
    for (;;)
    {
        std::string::size_type length = directory.length();
        if (length <= 1)
        {
            break;
        }

        if (directory[length - 1] == DIRECTORY_DELIMITER || directory[length - 1] == DIRECTORY_DELIMITER2)
        {
            directory.resize(length - 1);
            continue;
        }
        break;
    }
}

}
