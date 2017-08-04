//
// Created by antoine on 4/08/17.
//

#ifndef DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H
#define DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H

#include <fstream>
#include <map>
#include <vector>
#include "DepthBook.h"

struct FileProcessor {
  void process(const std::vector<std::string>&);
  ~FileProcessor();
private:
    std::map<std::string, DepthBook > depthBooks_;
    std::map<std::string, std::ofstream> outfiles_;

    std::ofstream& getOutfile(const std::string&);

};


#endif //DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H
