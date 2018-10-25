#include "FileProcessor.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cstring>
#include "Timer.h"
#include "FIXDecoder.h"
#include "DepthBook.h"

void usage()
{
    std::cout << "Usage: depthbook-constructor [--filter YYYYMMDD] sourcefile1 [sourcefile2...] \n\n"
                 "Creates a compiled CME depthbook given a series of raw fix files.\n"
                 "To ensure the book is well constructed, the first file needs to be week beginning (Sunday).\n\n"
                 "ls /path/to/cme-datapoint-files | xargs ./depthbook-constructor\n\n";
}

// perf

int main() {

   Decoder decoder;

   Timer timer;

   std::string inst = "ESM7";
   std::string group = "ES";

   DepthBook book(inst, group, 10, 2);
   MDIncrementalRefresh last;

   std::vector<std::string> files;
   files.push_back("/home/twanas/workspace/antoinewaugh/cme-datamine-analysis/sample-input/xcme_md_es_fut_20170312-r-00369");
   files.push_back("/home/twanas/workspace/antoinewaugh/cme-datamine-analysis/sample-input/xcme_md_es_fut_20170313-r-00369");


   std::ofstream ofile("ESH7.csv");
    std::stringstream ss;
   for(auto& filename: files) {

       std::ifstream file(filename);
       if (file.is_open()) {
           std::string message;
           while (getline(file, message)) {
//                         auto type = decoder.headerType(message);
//                         if(type == Decoder::Type::IncrementalRefresh) {
               if(book.handleMessage(message)) {
                   ss << book << '\n';
               }
//                             last = decoder.parseIncremental(message);

//                         }
           }
       }
   }

   std::cout << timer.seconds_elapsed() << std::endl;
   ofile << ss.rdbuf();
    std::cout << timer.seconds_elapsed() << std::endl;
}



/*
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

 */

/*
 *  if no matches 3 seconds to run
 *
 * */