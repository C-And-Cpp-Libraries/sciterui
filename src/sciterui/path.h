#pragma once
#include <string>

namespace SciterUI
{

class Path
{
public:
    enum DIR_MODULE_DIRECTORY
    {
        MODULE_DIRECTORY = 2
    };

    enum
    {
        FIND_ATTRIBUTE_ALLFILES = 0xFFFF, // Search include all files
        FIND_ATTRIBUTE_FILES = 0x0000,    // File can be read or written to without restriction
        FIND_ATTRIBUTE_SUBDIR = 0x0010,   // Subdirectories
    };

    Path();
    Path(const char * path);
    Path(const char * path, const char * fileName);

    Path(DIR_MODULE_DIRECTORY sdt, const char * nameExten = nullptr);
    ~Path();

    operator const char *() const;
    operator const std::string &();

    void GetDriveDirectory(std::string & directory) const;
    std::string GetDriveDirectory(void) const;
    void GetDirectory(std::string & directory) const;
    std::string GetDirectory(void) const;
    void GetNameExtension(std::string & nameExtension) const;
    std::string GetNameExtension(void) const;
    void GetExtension(std::string & rExtension) const;
    std::string GetExtension(void) const;
    void GetComponents(std::string * drive = nullptr, std::string * directory = nullptr, std::string * name = nullptr, std::string * extension = nullptr) const;
    bool IsRelative() const;

    void SetDriveDirectory(const char * driveDirectory);
    void SetDirectory(const char * directory, bool ensureAbsolute = false);
    void SetExtension(const char * extension);
    void SetNameExtension(const char * nameExtension);
    void SetComponents(const char * drive, const char * directory, const char * name, const char * extension);
    void AppendDirectory(const char * subDirectory);

    bool FileDelete(bool evenIfReadOnly = true) const;
    bool FileExists() const;

    bool IsDirectory() const;
    bool DirectoryExists() const;
    Path & DirectoryNormalize(Path baseDir);
    void DirectoryUp(std::string * lastDir = nullptr);

    bool FindFirst(uint32_t attributes = 0);
    bool FindNext();

private:
    void SetToModuleDirectory();

    bool AttributesMatch(uint32_t targetAttributes, uint32_t fileAttributes);
    void CloseFindHandle();

    void CleanPath(std::string & path) const;
    void EnsureLeadingBackslash(std::string & directory) const;
    void EnsureTrailingBackslash(std::string & directory) const;
    void StripLeadingBackslash(std::string & path) const;
    void StripTrailingBackslash(std::string & rDirectory) const;

    void * m_findHandle = nullptr;
    static void * m_hInst;
    std::string m_path;
    uint32_t m_findAttributes = 0;
};

} // namespace SciterUI