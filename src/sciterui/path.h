#pragma once
#include <string>

namespace SciterUI
{

class Path
{
public:
    Path();
    Path(const char * path);
    Path(const char * path, const char * fileName);

    operator const char *() const;
    operator const std::string &();

    void GetDirectory(std::string & directory) const;
    void GetComponents(std::string * drive = nullptr, std::string * directory = nullptr, std::string * name = nullptr, std::string * extension = nullptr) const;

    void SetDriveDirectory(const char * driveDirectory);
    void SetDirectory(const char * directory, bool ensureAbsolute = false);
    void SetNameExtension(const char * nameExtension);
    void SetComponents(const char * drive, const char * directory, const char * name, const char * extension);
    void AppendDirectory(const char * subDirectory);

    bool FileExists() const;

    bool DirectoryExists() const;
    void DirectoryUp(std::string * lastDir = nullptr);

private:
    void CleanPath(std::string & path) const;
    void EnsureLeadingBackslash(std::string & directory) const;
    void EnsureTrailingBackslash(std::string & directory) const;
    void StripLeadingBackslash(std::string & path) const;
    void StripTrailingBackslash(std::string & rDirectory) const;

    std::string m_path;
};

}