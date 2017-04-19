
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "DepthBook.h"

// TODO:
// * validate start on sunday?
// * support command line(s)
// * support maxDepth parameter


int main()
{

    // Return non-zero exit code if warnings occur
    // Move this to configuration
    std::vector<std::string> filenames = {
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170402-r-00365",
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170403-r-00365",
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170404-r-00365",
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170405-r-00365",
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170406-r-00365",
            "/Users/antoinewaugh/ClionProjects/untitled/raw/xcme_md_es_fut_20170407-r-00365"
    };

    DepthBook depthBook("ESM7", "ES"); // construct with a symbol which messages will be matched against

    std::string message;

    for (std::string filename : filenames)
    {

        std::ifstream file(filename);

        if (file.is_open())
        {
            while (std::getline(file, message))
            {
                {
                    if (depthBook.handleMessage(message))
                    {
                        // write separate file(s) to disk?
                        std::cout << depthBook << std::endl;
                    }
                }
            }
            file.close();
        }
    }
    return 0;
} // main


