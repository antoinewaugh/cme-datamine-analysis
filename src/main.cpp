
#include "FileProcessor.h"
#include <iostream>
#include <string>
#include <vector>
#include "string.h"

void usage()
{
    {
        std::cout << "Usage: depthbook-constructor [--filter YYYYMMDD] sourcefile1 [sourcefile2...] \n\n"
                     "Creates a compiled CME depthbook given a series of raw fix files.\n"
                     "To ensure the book is well constructed, the first file needs to be week beginning (Sunday).\n\n"
                     "ls /path/to/cme-datapoint-files | xargs ./depthbook-constructor\n\n";
    }
}

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);

    if (argc < 2) {
        usage();
        return 1;
    }


    std::string filter;
    if(strcmp(argv[1],"--filter") == 0) {
       filter = argv[2];
    }

    std::vector<std::string> filenames;
    for (int i = 1; i < argc; ++i) {
        filenames.push_back(std::string(argv[i]));
    }

    FileProcessor fileProcessor;
    fileProcessor.process(filenames, filter);

} // main
