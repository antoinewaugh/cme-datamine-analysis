//
// Created by antoine on 4/08/17.
//

#include "FileProcessor.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

void writeHeaders(std::ostream& o, int maxDepth, int maxImpDepth)
{
    o   << std::setprecision(12)
        << "timestamp, "
        << "sendingtime, "
        << "securityStatus, "
        << "matchEventIndicator, ";

    for(int i = 1; i<=maxDepth; ++i) {
        o << "bid" << i << "p, "
          << "bid" << i << "v, ";
    }

    o << "bidUpdateCount, ";

    for(int i = 1; i<=maxDepth; ++i) {
        o << "ask" << i << "p, "
          << "ask" << i << "v, ";
    }

    o << "askUpdateCount, ";

    o   << "bid1pDelta, "
        << "ask1pDelta, "
        << "bid1vDelta, "
        << "ask1vDelta, "
        << "tradeAggressorSide, "
        << "tradeCount, "
        << "tradeTotal, "
        << "tradeMin, "
        << "tradeMax, "
        << "tradeDetails, "
        << "lastRptSeq, ";

    for(int i = 1; i<=maxImpDepth ; ++i) {
        o << "ibid" << i << "p, "
          << "ibid" << i << "v, ";
    }

    o << "ibidUpdateCount, ";

    for(int i = 1; i<=maxImpDepth ; ++i) {
        o << "iask" << i << "p, "
          << "iask" << i << "v, ";
    }

    o << "iaskUpdateCount";

    o << '\n';
}

void FileProcessor::createOutfile(const std::string& symbol, int maxDepth, int maxImplDepth)
{
    if (!outfiles_.count(symbol)) {
        outfiles_.emplace(symbol, std::ofstream(symbol + ".csv"));
        auto& out = outfiles_[symbol];
        if (out.is_open()) {
            writeHeaders(out, maxDepth, maxImplDepth);
        }
    }

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
                    createOutfile(instrument.Symbol, instrument.MaxDepthSupported, instrument.MaxImplDepthSupported);
                    depthBooks_.emplace(instrument.Symbol, DepthBook(instrument.Symbol, instrument.SecurityGroup, instrument.MaxDepthSupported, instrument.MaxImplDepthSupported));
                }

                for (auto& i : depthBooks_) {
                    if (i.second.handleMessage(message)) {
                        if(filename.find(filter) == std::string::npos) { continue;}
                        auto &file = outfiles_[i.first];
                        file << i.second << '\n';
                    }
                }
            }
        }
    }
}
