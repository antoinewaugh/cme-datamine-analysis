
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "DepthBook.h"
#include <gtest/gtest.h>

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
    << std::endl;
}

void usage()
{
    {
        std::cout << "depthbook-constructor v1.0 \n\n" \
                     "usage: depthbook-constructor symbol product sourcefile1 sourcefile2... \n\n" \
                     "output: symbol.csv \n\n" \
                     "sample: ls /path/to/cme-datapoint-files | xargs ./depthbook-constructor ESM7 ES" << std::endl;
    }
}


int process(std::string symbol, std::string product, std::vector<std::string> filenames )
{
    // make map: product, vector<DepthBook>
    // if 55 = chain
    DepthBook depthBook(symbol, product); // construct with a symbol which messages will be matched against

    std::string outfilename(symbol + ".csv");
    std::ofstream outfile(outfilename);

    if (outfile.is_open())
    {
        writeHeaders(outfile);

        std::string message;
        for (auto filename : filenames)
        {
            std::ifstream file(filename);

            if (file.is_open())
            {
                std::cout << "Opening : " << filename << std::endl;
                while (std::getline(file, message))
                {
                    {
                        if (depthBook.handleMessage(message))
                        {
                            outfile << depthBook << '\n'; // prefer \n to std::endl to prevent flush for speed
                        }
                    }
                }
            }
            file.close();
        }
        outfile.flush();
        outfile.close();
    }
    return 0;
}

int main(int argc, char* argv[])
{
    std::ios_base::sync_with_stdio(false);

    if (argc < 4)
    {
        usage();
        return 1;
    }

    std::string symbol(argv[1]);
    std::string product(argv[2]);

    std::vector<std::string> filenames(argc-3);
    for(int i = 3; i < argc; ++i )
    {
        filenames.push_back(std::string(argv[i]));
    }

    return process(symbol, product, filenames);

} // main

