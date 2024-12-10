#include "FileUtils.h"
#include "pesukarhu/core/Debug.h"
#include <cstdio>


namespace pk
{
    std::string load_text_file(const std::string& filepath)
    {
        FILE* pFile = pFile = fopen(filepath.c_str(), "rb");
        if (!pFile)
        {
            Debug::log(
                "@load_text_file "
                "Failed to open file from: " + filepath,
                Debug::MessageType::PK_ERROR
            );
            return "";
        }

        fseek(pFile, 0, SEEK_END);
        long fileLength = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        size_t bufferSize = fileLength + 1;
        char* pBuffer = new char[bufferSize];
        memset(pBuffer, 0, sizeof(char) * bufferSize);

        size_t readBytes = 0;
        readBytes = fread(pBuffer, sizeof(char), fileLength, pFile);

        if (readBytes != fileLength)
        {
            Debug::log(
                "@load_text_file "
                "Error while reading file from: " + filepath + " "
                "(read byte count: " + std::to_string(readBytes) + " "
                "different from file size: " + std::to_string(fileLength) + ")",
                Debug::MessageType::PK_ERROR
            );
        }
        std::string sourceStr(pBuffer, fileLength-1);
        delete[] pBuffer;
        return sourceStr;
    }
}
