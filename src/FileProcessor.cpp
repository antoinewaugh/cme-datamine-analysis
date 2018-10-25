//
// Created by antoine on 4/08/17.
//

#include "FileProcessor.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <unordered_set>

struct Timer {
    Timer() {
        m_start = std::chrono::steady_clock::now();
    }

    auto seconds_elapsed() {
        auto end = std::chrono::steady_clock::now();
        auto elapsed = end - m_start;
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;

};

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
////    for (auto& o : outfiles_) {
////        o.second.close();
//    }
}

void FileProcessor::process(const std::vector<std::string>& filenames, const std::string& filter)
{

    std::cout << "Filter: " << filter << std::endl;
    std::string message;

    std::stringstream o;
    Decoder decoder;
    for (const auto& filename : filenames) {

        std::ifstream file(filename);
        if (file.is_open()) {

            Timer timer;
            std::cout << "Opening : " << filename << std::endl;
            std::stringstream content;
            content << file.rdbuf();
            std::string content_str = content.str();
            std::string_view  content_view(content_str);
            std::cout << "Opened file in: " << timer.seconds_elapsed() << std::endl;

            int begin = 0, end = 0;
            do {
                end = content_view.find('\n', begin);
                std::string_view message = content_view.substr(begin,end-begin);
                begin = end+1;

                // one decode
                for (auto& kv: depthBooks_) {
                    kv.second.clearFlags();
                }

                if (message.find("35=d") != std::string::npos) {
//                     create instrument books & outfiles
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
                        depthBooks_[entry.Symbol].handleEntry(
                                refresh.TransactTime,
                                refresh.SendingTime,
                                refresh.MatchEventIndicator,
                                entry);
                        updated.insert(entry.Symbol);
                    }
                } else if(type == Decoder::Type::SecurityStatus) {
                    auto status = decoder.parseStatus(message);
                    // todo: update all entires
                }

                // one decode
                for (auto& symbol : updated) {
                     if(filename.find(filter) == std::string::npos) { continue;}
                    // auto &file = outfiles_[symbol];
                     auto &book= depthBooks_[symbol];
                     //file << book << '\n';
                     o << book;
                }
            } while(end != std::string::npos);
            std::cout << "Decoded file in: " << timer.seconds_elapsed() << std::endl;
        }
    }
}
