//
// Created by antoine on 4/08/17.
//

#include "FileProcessor.h"
#include "Timer.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_set>

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

void FileProcessor::createOutfile(std::string symbol, int maxDepth, int maxImplDepth)
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

    Decoder decoder;
    for (const auto& filename : filenames) {

        std::ifstream file(filename);
        if (file.is_open()) {
            std::cout << "Opening : " << filename << std::endl;

            while(getline(file, message)) {
                if (message.find("35=d") != std::string::npos) {

                    MDInstrument instrument;
                    instrument.update(message);
                    createOutfile(instrument.Symbol, instrument.MaxDepthSupported, instrument.MaxImplDepthSupported);
                    depthBooks_.emplace(instrument.Symbol, DepthBook(instrument.Symbol, instrument.SecurityGroup, instrument.MaxDepthSupported, instrument.MaxImplDepthSupported));
                }

                std::unordered_set<std::string> updated;

                auto type = decoder.headerType(message);

                if(type == Decoder::Type::IncrementalRefresh) {
                    auto refresh = decoder.parseIncremental(message);
                    for(auto& entry: refresh.MDEntries) {
                        bool changed = depthBooks_[entry.Symbol].handleEntry(
                                refresh.TransactTime,
                                refresh.SendingTime,
                                refresh.MatchEventIndicator,
                                entry);
                        if(changed)
                            updated.insert(entry.Symbol);
                    }
                } else if(type == Decoder::Type::SecurityStatus) {

                    auto status = decoder.parseStatus(message);

                    for(auto& [symbol, book]: depthBooks_) {
                       bool changed = book.handleStatus(status.TransactTime,
                               status.SendingTime,
                               status.MatchEventIndicator,
                               status);
                       if(changed)
                           updated.insert(symbol);
                    }
                }

                // one decode
                for (auto& symbol : updated) {

                    auto &book = depthBooks_[symbol];

                    if(filename.find(filter) != std::string::npos) {
                         auto &file = outfiles_[symbol];
                         book.commit();
                         file << book << '\n';
                    }
                    // need to clear irrespective of whether filename in filter
                    book.clearFlags();
                }
            }
        }
    }
}
