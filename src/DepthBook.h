//
// Created by Antoine Waugh on 12/04/2017.
//

#ifndef DEPTHBOOK_H
#define DEPTHBOOK_H

#include "FIXDecoder.h"
#include <map>
#include <memory>
#include <ostream>
#include <iostream>
#include <vector>

struct PriceEntry {
    double price;
    int quantity;

    friend std::ostream& operator<<(std::ostream& os, const PriceEntry& entry)
    {
        os << entry.price << ',' << entry.quantity;
        return os;
    }
};

class DepthList {

private:
    std::vector<std::shared_ptr<PriceEntry> > entries;
    std::map<double, std::shared_ptr<PriceEntry> > byPrice;

    int maxDepthSupported;
    int maxDepthKnown;

    bool isBuy;

    int updateCount;

    void remove(int, double);

public:
    DepthList(const bool&, int maxDepthSupported); // simple constructor
    DepthList(const DepthList&); // copy constructor
    DepthList& operator=(const DepthList& rhs); // assignment operator
    DepthList() = default;
    void insert(double, int, int, char);
    PriceEntry getBestEntry();

    const std::vector<std::shared_ptr<PriceEntry> >& getEntries() const
    {
        return entries;
    }

    int getMaxDepthSupported() const
    {
        return maxDepthSupported;
    }

    void reset()
    {
        entries.clear();
        maxDepthKnown = 0;
        clear();
    }

    void clear()
    {
        updateCount = 0;
    }

    int getUpdateCount() const
    {
        return updateCount;
    }

    friend std::ostream& operator<<(std::ostream& os, const DepthList& list)
    {
        auto &&entries = list.getEntries();
        for (int i = 0; i < list.getMaxDepthSupported(); ++i) {
            if (i < entries.size()) {
                os << *entries[i] << ",";
            }
            else {
                os << ",,";
            }
        }

        os << list.getUpdateCount();

        return os;
    }
};

class TradeList {
private:
    int aggressorSide;
    double min;
    double max;
    int totalVolume;
    int count;
    std::vector<PriceEntry> trades;

public:
    TradeList() = default;
    const std::vector<PriceEntry>& getTrades() const
    {
        return trades;
    }

    void insert(double, int, int);
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

    friend std::ostream& operator<<(std::ostream& os, const TradeList& list)
    {
        os << list.getAggressorSide() << ','
           << list.getCount() << ','
           << list.getTotalVolume() << ','
           << list.getMinPrice() << ','
           << list.getMaxPrice() << ',';

        int remaining = list.getTrades().size();

        for (auto&& item : list.getTrades()) {
            os << item.quantity << '@' << item.price;
            --remaining;
            if (remaining > 0) {
                os << '|';
            }
        }
        return os;
    }
};

class DepthBook {

private:
    std::string timestamp;
    std::string sendingtime;

    std::string symbol;
    std::string securityGroup;
    std::string matchEventIndicator;

    DepthList bids;
    DepthList asks;

    DepthList implBids;
    DepthList implAsks;

    double bid1pDelta;
    double ask1pDelta;

    double bid1vDelta;
    double ask1vDelta;

    TradeList trades;

    long lastRptSeq;

    int securityTradingStatus = 0;

    void resetState();

    void setSecurityStatus(int);
    MDIncrementalRefresh mdRefresh;
    MDSecurityStatus mdStatus;



public:
    DepthBook(std::string_view symbol, std::string_view securityGroup,
              int maxDepthSupported, int maxImplDepthSupported)
        : symbol(symbol)
        , securityGroup(securityGroup)
        , bids(true, maxDepthSupported)
        , asks(false, maxDepthSupported)
        , implBids(true, maxImplDepthSupported)
        , implAsks(false, maxImplDepthSupported)
        , bid1pDelta(0)
        , ask1pDelta(0)
        , bid1vDelta(0)
        , ask1vDelta(0)
    {
    }

    DepthBook() = default;

    void clearFlags();
    bool handleEntry(std::string_view transactTime,
                                std::string_view sendingTime,
                                std::string_view matchEventIndicator,
                                MDEntry const& entry);

    const std::string& getTimestamp() const
    {
        return timestamp;
    }

    const std::string& getSendingTime() const
    {
        return sendingtime;
    }

    int getSecurityTradingStatus() const
    {
        return securityTradingStatus;
    }

    long getLastRptSeq() const
    {
        return lastRptSeq;
    }

    bool handleMessage(std::string_view);

    const std::string& getMatchEventIndicator() const;

    friend std::ostream& operator<<(std::ostream& os, const DepthBook& book)
    {
        os << book.getTimestamp() << ','
           << book.getSendingTime() << ','
           << book.getSecurityTradingStatus() << ','
           << book.getMatchEventIndicator() << ','
           << book.bids << ','
           << book.asks << ','
           << book.bid1pDelta << ','
           << book.ask1pDelta << ','
           << book.bid1vDelta << ','
           << book.ask1vDelta << ','
           //           << book.bids.getVolumeDeltas(book.previousBids)
           //           << book.asks.getVolumeDeltas(book.previousAsks)
           << book.trades << ','
           << book.getLastRptSeq() << ','
           << book.implBids << ','
           << book.implAsks;
        return os;
    }
};

#endif //DEPTHBOOK_H
