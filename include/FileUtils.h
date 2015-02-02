#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>

namespace FileUtils
{
    bool isFile( const std::string &filename );
    bool isDirectory( const std::string &directory );
    
    
    std::string getFileContent( const std::string &filename );
    
    std::vector<std::string> getFilesInDirectory( const std::string &directory );
}


#endif
