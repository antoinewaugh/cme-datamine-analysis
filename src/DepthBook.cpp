
#include <iostream>
#include "DepthBook.h"


DepthList::DepthList(const bool &isBuy) : maxDepthKnown(0)
        ,isBuy(isBuy)
        ,updateCount(0)
{
    entries.reserve(maxDepthSupported);
}

DepthList::DepthList(const DepthList &rhs)
{
    // create *new* entries
    // default would simply increment reference count on shared_pointer
    for (auto e : rhs.getEntries())
    {
        e = std::make_shared<PriceEntry>(*e);
    }
}


DepthList & DepthList::operator=(const DepthList &rhs)
{
    // create *new* entries
    // default would simply increment reference count on shared_pointer
    for (auto e : rhs.getEntries())
    {
        e = std::make_shared<PriceEntry>(*e);
    }
    return *this;
}

void DepthList::remove(int level, double price)
{
    if ((level < maxDepthKnown)
        && (byPrice.find(price) != byPrice.end())) // validate price exists in map
    {
        entries.erase(entries.begin() + level);
        byPrice.erase(price);
        --maxDepthKnown;
    } else
    {
        std::cout << "Problem detected in depth cache - price level " << price << " does not exist" << std::endl;
    }
}

void DepthList::insert(double price, int quantity, int levelIndex, char updateType)
{
    int level = levelIndex - 1; // internal level representation has offset of 1

    // REMOVE
    if (updateType == MDEntryAction_DELETE)
    {
        remove(level, price);
        ++updateCount;
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

                // update book entry
                // is this inefficient , first creating the entry object and passing it to entries & byPrice
                auto entry = std::make_shared<PriceEntry>();
                entry->price = price;
                entry->quantity = quantity;

                entries.insert(entries.begin() + level, entry);
                byPrice[price] = entry;

                // implicit removal of last level in book
                // if size of book has exceeded max due to recent insert
                if (entries.size() == maxDepthSupported+1)
                {
                    byPrice.erase(entries[maxDepthSupported]->price);
                    entries.erase(entries.begin() + maxDepthSupported);
                    --maxDepthKnown;
                }
                ++updateCount;
                ++maxDepthKnown;
            }
        } else
        {
            std::cout << "Trying to insert Depth Item outside the maximum known level of depth: "
                      << "Level=" << levelIndex << " - "
                      << "Price=" << price
                      << std::endl;
        }
    }
        // CHANGE
    else if (updateType == MDEntryAction_CHANGE)
    {
        if (level < maxDepthKnown)
        {

            // update book entry
            if (entries[level]->price == price)
            {
                // implicit update of entry in byPrice map due to shared pointer
                entries[level]->quantity = quantity;
                ++updateCount;
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

PriceEntry DepthList::getBestEntry()
{
    if (this->entries.size() > 0)
    {
        return *(entries[0]);
    }
    return PriceEntry{0,0};
}

void TradeList::insert(double price, int quantity, int aggressorSide)
{
    if (min == 0 || price < min) { min = price; }
    if (max == 0 || price > max) { max = price; }
    if (aggressorSide != 0) { this->aggressorSide = aggressorSide; }
    totalVolume += quantity;
    trades.push_back(PriceEntry{price, quantity});

    ++count;
}

void TradeList::clear()
{
    aggressorSide = 0;
    min = 0;
    max = 0;
    count = 0;
    totalVolume = 0;
    trades.clear();
}


bool DepthBook::handleMessage(const std::string& s)
{
    // Clear previous state
    trades.clear();
    bids.clear();
    asks.clear();

    // Update book
    bool bookUpdated = false;

    // FIXME: Move to decoder
    if (s.find("35=f") != std::string::npos) {
        mdStatus.update(s);
        if (mdStatus.SecurityGroup == this->securityGroup) {
            this->timestamp = mdStatus.TransactTime;
            this->securityTradingStatus = mdStatus.SecurityTradingStatus;
            bookUpdated = true;
        }
    }
    else if (s.find("35=X") != std::string::npos)
    {
        mdRefresh.update(s);

        double prevBid1p = bids.getBestEntry().price;
        double prevBid1v = bids.getBestEntry().quantity;
        double prevAsk1p = asks.getBestEntry().price;
        double prevAsk1v = asks.getBestEntry().quantity;

        // replace with deep copy of book,
        // then bids.getDeltas(previousBids)
        // where price at top level can represent bid1pdelta
        // and each of the volumes equal the volume deltas

        for (auto entry : mdRefresh.MDEntries)
        {
            if (entry.Symbol == this->symbol)
            {
                this->timestamp = mdRefresh.TransactTime;
                switch (entry.MDEntryType)
                {
                    case MDEntryType_BID:
                        bids.insert(entry.MDEntryPx, entry.MDEntrySize, entry.MDPriceLevel, entry.MDUpdateAction);
                        this->lastRptSeq = entry.RptSeq;
                        bookUpdated = true;
                        break;

                    case MDEntryType_OFFER:
                        asks.insert(entry.MDEntryPx, entry.MDEntrySize, entry.MDPriceLevel, entry.MDUpdateAction);
                        this->lastRptSeq = entry.RptSeq;
                        bookUpdated = true;
                        break;

                    case MDEntryType_TRADE:
                        trades.insert(entry.MDEntryPx, entry.MDEntrySize, entry.AggressorSide);
                        this->lastRptSeq = entry.RptSeq;
                        bookUpdated = true;
                        break;
                }
            }
        }

        // update top of book deltas
        // this needs to be done external to the depthbook as the book us unaware of
        // how many mdEntries are in a single MDIncrementalRefresh

        if ( prevBid1p > 0.0 and prevAsk1p > 0.0 )
        {
            this->bid1pDelta = bids.getBestEntry().price - prevBid1p;
            this->bid1vDelta = bids.getBestEntry().quantity - prevBid1v;
            this->ask1pDelta = asks.getBestEntry().price - prevAsk1p;
            this->ask1vDelta = asks.getBestEntry().quantity - prevAsk1v;
        }

        mdRefresh.clear();
    }
    return bookUpdated;
}

