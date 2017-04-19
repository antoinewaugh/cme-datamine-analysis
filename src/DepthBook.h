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

    int maxDepthSupported = 10; // move to constructor
    int maxDepthKnown = 0;

    void remove(int levelIndex, double price)
    {
        if ((levelIndex < maxDepthKnown)
            && (byPrice.find(price) != byPrice.end()))
        {
            entries.erase(entries.begin() + levelIndex);
            byPrice.erase(price);
            maxDepthKnown--;
        } else
        {
            std::cout << "Problem detected in depth cache - price level " << price << " does not exist" << std::endl;
        }
    }


public:

    void insert(double price, int quantity, int levelIndex, char updateType)
    {
        int level = levelIndex - 1; // internal level representation has offset of 1

        // REMOVE
        if (updateType == MDEntryAction_DELETE)
        {
            remove(level, price);
        }
        // ADD
        else if (updateType == MDEntryAction_NEW)
        {
            if (level < maxDepthKnown + 1)
            {
                if ((level < maxDepthKnown) && (entries[level]->price == price))
                {
                    std::cout << "Depth Item at new price already exists: Level=" << level << " - Price=" << price
                         << std::endl;
                } else
                {

                    // is this inefficient , first creating the entry object and passing it to entries & byPrice
                    auto entry = std::make_shared<PriceEntry>();
                    entry->price = price;
                    entry->quantity = quantity;

                    entries.insert(entries.begin() + level, entry);
                    byPrice[price] = entry;

                    // implicit removal of last level in book
                    // if size of book has exceeded max due to recent insert
                    if (entries.size() == 11)
                    {
                        byPrice.erase(entries[10]->price);
                        entries.erase(entries.begin() + 10);
                        maxDepthKnown--;
                    }
                    maxDepthKnown++;
                }
            } else
            {
                std::cout << "Trying to insert Depth Item outside the maximum known level of depth: Level=" << levelIndex
                     << " - Price=" << price << std::endl;
            }
        }
        // CHANGE
        else if (updateType == MDEntryAction_CHANGE)
        {
            if (level < maxDepthKnown)
            {
                if (entries[level]->price == price)
                {
                    // implicit update of vector.
                    entries[level]->quantity = quantity;
                } else
                {
                    std::cout << "Depth Item at given price for modification does not exist : " << price << std::endl ;
                }
            } else
            {
                std::cout << "Depth Item at given price for modification does not exist : " << price << std::endl ;
            }
        } else
        {
            std::cout << "Depth Item update type is invalid :" << updateType << std::endl ;
        }
    }

    const std::vector<std::shared_ptr<PriceEntry>> &getEntries() const
    {
        return entries;
    }

    int getMaxDepthSupported() const
    {
        return maxDepthSupported;
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

        return os;
    }
};

class TradeList
{
private:
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

    void insert(double price, int quantity)
    {
        if (min == 0 || price < min) { min = price; }
        if (max == 0 || price > max) { max = price; }
        totalVolume += quantity;
        trades.push_back(PriceEntry{price, quantity});

        ++count;
    }

    void clear()
    {
        min = 0;
        max = 0;
        count = 0;
        totalVolume = 0;
        trades.clear();
    }

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

    friend std::ostream &operator<<(std::ostream &os, const TradeList &list)
    {
        os << list.getCount() << ','
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
    TradeList trades;

    int securityTradingStatus;

    std::string lastMsg; // for debugging

public:

    DepthBook(const std::string &symbol, const std::string &securityGroup) : symbol(symbol), securityGroup(securityGroup) {}

    const std::string &getTimestamp() const
    {
        return timestamp;
    }

    int getSecurityTradingStatus() const
    {
        return securityTradingStatus;
    }

    bool handleMessage(const std::string& s)
    {
        // Clear previous state
        trades.clear();

        // Update book
        bool bookUpdated = false;

        // FIXME: Move to decoder
        if (s.find("35=f") != std::string::npos) {
            MDSecurityStatus mdStatus(s);
            if (mdStatus.SecurityGroup == this->securityGroup) {
                this->timestamp = mdStatus.TransactTime;
                this->securityTradingStatus = mdStatus.SecurityTradingStatus;
                bookUpdated = true;
            }
        }
        else if (s.find("35=X") != std::string::npos)
        {
           MDIncrementalRefresh mdRefresh(s);
           for (auto entry : mdRefresh.MDEntries)
            {
                if (entry.Symbol == this->symbol)
                {
                    this->timestamp = mdRefresh.TransactTime;

                    switch (entry.MDEntryType)
                    {
                        case MDEntryType_BID:
                            bids.insert(entry.MDEntryPx, entry.MDEntrySize, entry.MDPriceLevel, entry.MDUpdateAction);
                            bookUpdated = true;
                            break;

                        case MDEntryType_OFFER:
                            asks.insert(entry.MDEntryPx, entry.MDEntrySize, entry.MDPriceLevel, entry.MDUpdateAction);
                            bookUpdated = true;
                            break;

                        case MDEntryType_TRADE:
                            trades.insert(entry.MDEntryPx, entry.MDEntrySize);
                            bookUpdated = true;
                            break;
                    }
                }
            }
            this->lastMsg = s;
        }
        return bookUpdated;
    }

    friend std::ostream &operator<<(std::ostream &os, const DepthBook &book)
    {

        os << book.getTimestamp() << ','
           << book.getSecurityTradingStatus() << ','
           << book.bids
           << book.asks
           << book.trades;

        return os;
    }

};


#endif //DEPTHBOOK_H
