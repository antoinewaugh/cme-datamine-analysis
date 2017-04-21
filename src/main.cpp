
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "DepthBook.h"



// TODO:
// * validate start on sunday?
// * support command line(s)
// * support maxDepth parameter

void writeHeaders(std::ostream& o)
{
    o 
    << "timestamp, "
    << "securityStatus, "
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
    << "bid1p delta, "
    << "ask1p delta, "
    << "tradeAggressorSide, "
    << "tradeCount, "
    << "tradeTotal, "
    << "tradeMin, "
    << "tradeMax, "
    << "tradeDetails, "
    << "lastRptSeq, "
    << std::endl;
}

int main()
{
    std::ios_base::sync_with_stdio(false);

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

    std::string outfilename("/Users/antoinewaugh/Downloads/ESM7_20170402.csv");
    std::ofstream outfile(outfilename);

    if (outfile.is_open())
    {
        writeHeaders(outfile);

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
                            outfile << depthBook << '\n'; // prefer \n to std::endl to prevent flush for speed
                          //  outfile << message.substr(0,1) << '\n'; // this line demonstrates the delay is in the length of writing to the file. processing the depthbook but not writing to file takes significantly less time..
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
} // main


