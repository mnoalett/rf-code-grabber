#include "LittleFS.h"

class FileManager
{
public:
    static FileManager &getInstance()
    {
        static FileManager instance;
        return instance;
    }

    void listFiles();
    void readFile(const char *path);
    boolean writeFile(const char *path, const char *message);
    void createFile(const char *path);
    boolean appendFile(const char *path, const char *message);
    boolean deleteFile(const char *path);

private:
    FileManager();
};