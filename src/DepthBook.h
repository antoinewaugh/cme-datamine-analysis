//
// Created by Antoine Waugh on 12/04/2017.
//

#ifndef DEPTHBOOK_H
#define DEPTHBOOK_H

#include <vector>
#include <map>
#include <ostream>
#include "FIXDecoder.h"

struct PriceEntry
{
    double price;
    int quantity;

    friend std::ostream &operator<<(std::ostream &os, const PriceEntry &entry)
    {
        os << entry.price << ',' << entry.quantity;
        return os;
    }
};

class DepthList
{

private:

    std::vector<std::shared_ptr<PriceEntry> > entries;
    std::map<double, std::shared_ptr<PriceEntry>> byPrice;

    int maxDepthSupported = 10; // mve to constructor
    int maxDepthKnown = 0;

    bool isBuy;

    int updateCount;

    void remove(int, double);

public:

    DepthList( const bool & ); // simple constructor
    DepthList( const DepthList & ); // copy constructor
    DepthList & operator=(const DepthList &rhs); // assignment operator

    void insert(double , int , int , char );
    PriceEntry getBestEntry();

    const std::vector<std::shared_ptr<PriceEntry>> &getEntries() const
    {
        return entries;
    }

    int getMaxDepthSupported() const
    {
        return maxDepthSupported;
    }

    void clear()
    {
        updateCount = 0;
    }

    int getUpdateCount() const
    {
        return updateCount;
    }

    friend std::ostream &operator<<(std::ostream &os, const DepthList &list)
    {
        // Output in CSV
        // Column count needs to be constant to avoid misaligns

        int count = 0;
        for (auto &&entry : list.getEntries())
        {
            os << *entry << ',';
            ++count;
        }

        // Fill blank columns if book size < max depth
        for(int i=list.getMaxDepthSupported()-count-1; i >= 0;--i)
        {
            os << ",,";
        }

        os << list.getUpdateCount();
        return os;
    }
};

class TradeList
{
private:

    int aggressorSide;
    double min;
    double max;
    int totalVolume;
    int count;
    std::vector<PriceEntry> trades;

public:

    const std::vector<PriceEntry> &getTrades() const
    {
        return trades;
    }

    void insert(double, int , int );
    void clear();

    double getMinPrice() const
    {
        return min;
    }

    double getMaxPrice() const
    {
        return max;
    }

    int getCount() const
    {
        return count;
    }

    int getTotalVolume() const
    {
        return totalVolume;
    }

    int getAggressorSide() const
    {
        return aggressorSide;
    }

    friend std::ostream &operator<<(std::ostream &os, const TradeList &list)
    {
        os << list.getAggressorSide() << ','
           << list.getCount() << ','
           << list.getTotalVolume() << ','
           << list.getMinPrice() << ','
           << list.getMaxPrice() << ',';

        int remaining = list.getTrades().size();

        for (auto &&item : list.getTrades())
        {
            os << item.quantity << '@' << item.price;
            --remaining;
            if (remaining>0) { os << '|'; }
        }
        return os;
    }

};

class DepthBook
{

private:

    std::string timestamp;

    std::string symbol;
    std::string securityGroup;

    DepthList bids;
    DepthList asks;

    double bid1pDelta;
    double ask1pDelta;

    double bid1vDelta;
    double ask1vDelta;

    TradeList trades;

    long lastRptSeq;

    int securityTradingStatus;

    MDIncrementalRefresh mdRefresh;
    MDSecurityStatus mdStatus;

public:

    DepthBook(const std::string &symbol, const std::string &securityGroup) : symbol(symbol)
            , securityGroup(securityGroup)
            , bids(true)
            , asks(false)
            , bid1pDelta(0)
            , ask1pDelta(0)
            , bid1vDelta(0)
            , ask1vDelta(0)
    {

    }

    const std::string &getTimestamp() const
    {
        return timestamp;
    }

    int getSecurityTradingStatus() const
    {
        return securityTradingStatus;
    }

    long getLastRptSeq() const
    {
        return lastRptSeq;
    }

    bool handleMessage(const std::string& );

    friend std::ostream &operator<<(std::ostream &os, const DepthBook &book)
    {

        os << book.getTimestamp() << ','
           << book.getSecurityTradingStatus() << ','
           << book.bids << ','
           << book.asks << ','
           << book.bid1pDelta << ','
           << book.ask1pDelta << ','
           << book.bid1vDelta << ','
           << book.ask1vDelta << ','
//           << book.bids.getVolumeDeltas(book.previousBids)
//           << book.asks.getVolumeDeltas(book.previousAsks)
           << book.trades << ','
           << book.getLastRptSeq();

        return os;
    }

};


#endif //DEPTHBOOK_H
