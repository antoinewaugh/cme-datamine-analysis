//
// Created by antoine on 4/08/17.
//

#include "FileProcessor.h"
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void writeHeaders(std::ostream& o)
{
    o
        << "timestamp, "
        << "securityStatus, "
        << "matchEventIndicator, "
        << "bid1p, "
        << "bid1v, "
        << "bid2p, "
        << "bid2v, "
        << "bid3p, "
        << "bid3v, "
        << "bid4p, "
        << "bid4v, "
        << "bid5p, "
        << "bid5v, "
        << "bid6p, "
        << "bid6v, "
        << "bid7p, "
        << "bid7v, "
        << "bid8p, "
        << "bid8v, "
        << "bid9p, "
        << "bid9v, "
        << "bid10p, "
        << "bid10v, "
        << "bidUpdateCount, "
        << "ask1p, "
        << "ask1v, "
        << "ask2p, "
        << "ask2v, "
        << "ask3p, "
        << "ask3v, "
        << "ask4p, "
        << "ask4v, "
        << "ask5p, "
        << "ask5v, "
        << "ask6p, "
        << "ask6v, "
        << "ask7p, "
        << "ask7v, "
        << "ask8p, "
        << "ask8v, "
        << "ask9p, "
        << "ask9v, "
        << "ask10p, "
        << "ask10v, "
        << "askUpdateCount, "
        << "bid1pDelta, "
        << "ask1pDelta, "
        << "bid1vDelta, "
        << "ask1vDelta, "
        << "tradeAggressorSide, "
        << "tradeCount, "
        << "tradeTotal, "
        << "tradeMin, "
        << "tradeMax, "
        << "tradeDetails, "
        << "lastRptSeq, "
        << "ibid1p, "
        << "ibid1v, "
        << "ibid2p, "
        << "ibid2v, "
        << "ibid3p, "
        << "ibid3v, "
        << "ibid4p, "
        << "ibid4v, "
        << "ibid5p, "
        << "ibid5v, "
        << "ibid6p, "
        << "ibid6v, "
        << "ibid7p, "
        << "ibid7v, "
        << "ibid8p, "
        << "ibid8v, "
        << "ibid9p, "
        << "ibid9v, "
        << "ibid10p, "
        << "iask1p, "
        << "iask1v, "
        << "iask2p, "
        << "iask2v, "
        << "iask3p, "
        << "iask3v, "
        << "iask4p, "
        << "iask4v, "
        << "iask5p, "
        << "iask5v, "
        << "iask6p, "
        << "iask6v, "
        << "iask7p, "
        << "iask7v, "
        << "iask8p, "
        << "iask8v, "
        << "iask9p, "
        << "iask9v, "
        << "iask10p, "
        << "iask10v "
        << '\n';
}

std::ofstream& FileProcessor::getOutfile(const std::string& symbol)
{
    if (!outfiles_.count(symbol)) {
        outfiles_.emplace(symbol, std::ofstream(symbol + ".csv"));
        auto& out = outfiles_[symbol];
        if (out.is_open()) {
            writeHeaders(out);
        }
    }
    return outfiles_[symbol];
}

// maybe this is automatically handled by map's destructor?
FileProcessor::~FileProcessor()
{
    for (auto& o : outfiles_) {
        o.second.close();
    }
}

void FileProcessor::process(const std::vector<std::string>& filenames, const std::string& filter)
{

    std::cout << "Filter: " << filter << std::endl;
    std::string message;

    for (const auto& filename : filenames) {

        std::ifstream file(filename);
        if (file.is_open()) {

            std::cout << "Opening : " << filename << std::endl;

            while (std::getline(file, message)) {

                if (message.find("35=d") != std::string::npos) {
                    // create instrument books & outfiles
                    MDInstrument instrument;
                    instrument.update(message);
                    depthBooks_.emplace(instrument.Symbol, DepthBook(instrument.Symbol, instrument.SecurityGroup, instrument.MaxDepthSupported, instrument.MaxImplDepthSupported));
                }

                for (auto& i : depthBooks_) {
                    if (i.second.handleMessage(message)) {
                        if(filename.find(filter) == std::string::npos) { continue;}
                        auto &file = getOutfile(i.first);
                        file << i.second << '\n';
                    }
                }
            }
        }
    }
}
