#include "FileManager.h"

FileManager::FileManager() {}

void FileManager::listFiles()
{
    Dir dir = LittleFS.openDir("/");
    Serial.println("Listing files:");
    while (dir.next())
    {
        Serial.print(dir.fileName());
        if (dir.fileSize())
        {
            File f = dir.openFile("r");
            Serial.print("   ");
            Serial.print(f.size());
            time_t creation = f.getCreationTime();
            time_t lastWrite = f.getLastWrite();
            struct tm *tmstruct = localtime(&creation);
            Serial.printf("    CREATION: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            tmstruct = localtime(&lastWrite);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min, tmstruct->tm_sec);
            f.close();
        }
    }
}

void FileManager::readFile(const char *path)
{
    Serial.printf("Reading file: %s\n", path);

    File file = LittleFS.open(path, "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    while (file.available())
    {
        Serial.write(file.read());
    }

    file.close();
    Serial.println();
}

void FileManager::createFile(const char *path)
{
    if (!LittleFS.exists(path))
    {
        FileManager::writeFile(path, "");
    }
}

boolean FileManager::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);

    File file = LittleFS.open(path, "w");
    if (!file)
    {
        Serial.println("[FileManager] Failed to open file for writing");
        return false;
    }
    if (!file.print(message))
    {
        Serial.println("[FileManager] Write failed");
        return false;
    }
    delay(2000); // Make sure the CREATE and LASTWRITE times are different
    file.close();
    return true;
}

boolean FileManager::appendFile(const char *path, const char *message)
{
    File file = LittleFS.open(path, "a");
    if (!file)
    {
        Serial.println("Failed to open file for appending");
        return false;
    }
    file.print(message);
    file.close();
    return true;
}

boolean FileManager::deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if (LittleFS.remove(path))
    {
        Serial.println("File deleted");
        return true;
    }
    return false;
}