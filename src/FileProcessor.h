//
// Created by antoine on 4/08/17.
//

#ifndef DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H
#define DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H

#include "DepthBook.h"
#include <fstream>
#include <map>
#include <vector>

struct FileProcessor {
    void process(const std::vector<std::string>&, const std::string&);
    ~FileProcessor();

private:
    std::map<std::string, DepthBook> depthBooks_;
    std::map<std::string, std::ofstream> outfiles_;

    void createOutfile(const std::string&, int, int);
};

#endif //DEPTHBOOK_CONSTRUCTOR_FILEPROCESSOR_H
