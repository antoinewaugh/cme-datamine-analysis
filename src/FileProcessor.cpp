//
// Created by antoine on 4/08/17.
//

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <vector>
#include "FileProcessor.h"

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
      << '\n';
}


std::ofstream& FileProcessor::getOutfile(const std::string& symbol) {
  if (!outfiles_.count(symbol)) {
      outfiles_.emplace(symbol, std::ofstream(symbol + ".csv"));
      auto &out = outfiles_[symbol];
      if (out.is_open()) {
        writeHeaders(out);
      }
  }
  return outfiles_[symbol];
}

// maybe this is automatically handled by map's destructor?
FileProcessor::~FileProcessor() {
  for(auto &o : outfiles_) {
    o.second.close();
  }
}

void FileProcessor::process(const std::vector<std::string>& filenames) {

    std::string message;

    for (const auto &filename : filenames) {

      std::ifstream file(filename);
      if (file.is_open()) {

        std::cout << "Opening : " << filename << std::endl;

        while (std::getline(file, message)) {

          if (message.find("35=d") != std::string::npos) {
            // create instrument books & outfiles
            MDInstrument instrument;
            instrument.update(message);
            depthBooks_.emplace(instrument.Symbol, DepthBook(instrument.Symbol, instrument.SecurityGroup));
          }

          for (auto& i : depthBooks_) {
            if (i.second.handleMessage(message)) {
              auto &file = getOutfile(i.first);
              file << i.second << '\n';
            }
          }

        }
      }
  }
}
