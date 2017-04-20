//
// Created by Antoine Waugh on 18/04/2017.
//

#ifndef FIXDECODER_H
#define FIXDECODER_H

#include <string>
#include <vector>

constexpr char FIX_FIELD_DELIMITER = '\x01';
constexpr char FIX_KEY_DELIMITER = '=';

constexpr int STR_TO_CHAR = 0;
constexpr int KEY = 0;
constexpr int VALUE = 1;

const std::string Field_Asset = "6937";
const std::string Field_Symbol = "55";
const std::string Field_TransactTime = "60";
const std::string Field_TradeDate = "75";
const std::string Field_MatchEventIndicator = "5799";
const std::string Field_NoMDEntries = "268";
const std::string Field_MDUpdateAction = "279";
const std::string Field_MDEntryType = "269";
const std::string Field_SecurityID = "48";
const std::string Field_SecurityGroup = "1151";
const std::string Field_SecurityTradingEvent = "1174";
const std::string Field_SecurityTradingStatus = "326";
const std::string Field_RptSeq = "83";
const std::string Field_MDEntryPx = "270";
const std::string Field_MDEntrySize = "271";
const std::string Field_NumberOfOrders = "346";
const std::string Field_MDPriceLevel = "1023";
const std::string Field_OpenCloseSettlFlag = "286";
const std::string Field_AggressorSide = "5797";
const std::string Field_TradingReferenceDate = "5796";
const std::string Field_HighLimitPrice = "1149";
const std::string Field_LowLimitPrice = "1148";
const std::string Field_MaxPriceVariation = "1143";
const std::string Field_ApplID = "1180";
const std::string Field_NoOrderIDEntries = "37705";
const std::string Field_OrderID = "37";
const std::string Field_LastQty = "32";
const std::string Field_SettlPriceType = "731";
const std::string Field_HaltReason = "327";

constexpr char MDEntryType_BID = '0';
constexpr char MDEntryType_OFFER = '1';
constexpr char MDEntryType_TRADE = '2';

constexpr char MDEntryAction_NEW = '0';
constexpr char MDEntryAction_CHANGE = '1';
constexpr char MDEntryAction_DELETE = '2';

void string_split_optim(std::vector<std::string> & output, const std::string &s, const char delimiter)
{
    size_t start = 0;
    size_t end = s.find_first_of(delimiter);

//    std::vector<std::string> output;

    while (end <= std::string::npos)
    {
        output.emplace_back(s.substr(start, end - start));

        if (end == std::string::npos)
            break;

        start = end + 1;
        end = s.find_first_of(delimiter, start);
    }

//    return output;
}

class OrderIdEntry
{
public:
    std::string OrderID;
    int LastQty;
};

struct MDEntry
{
public:
    // necessary for defaults?
    char MDUpdateAction;
    char MDEntryType;
    std::string Symbol;
    int SecurityID;
    int RptSeq;
    double MDEntryPx;
    int MDEntrySize;
    int NumberOfOrders = 0;
    int MDPriceLevel = 0;
    int OpenCloseSettlFlag = 0;
    std::string SettlPriceType = "";
    int AggressorSide = 0;
    std::string TradingReferenceDate = "";
    double HighLimitPrice = 0.0;
    double LowLimitPrice = 0.0;
    double MaxPriceVariation = 0.0;
    int ApplID = 0;

};

struct MDSecurityStatus
{
public:

    std::string TransactTime;
    std::string TradeDate;
    std::string MatchEventIndicator;
    std::string SecurityGroup;
    std::string Asset;

    int SecurityID;
    int SecurityTradingStatus;
    int HaltReason;
    int SecurityTradingEvent;

    std::vector<std::string> fieldssplit;
    std::vector<std::string> kv;

    void update(const std::string& message)
    {

        string_split_optim(fieldssplit,message, FIX_FIELD_DELIMITER);

        for (auto fields : fieldssplit)
        {
            string_split_optim(kv, fields, FIX_KEY_DELIMITER);

            // :: MDSecurityStatus

            if (kv[KEY] == Field_TransactTime) this->TransactTime = kv[VALUE];
            else if (kv[KEY] == Field_TradeDate) this->TradeDate = kv[VALUE];
            else if (kv[KEY] == Field_MatchEventIndicator) this->MatchEventIndicator = kv[VALUE];
            else if (kv[KEY] == Field_SecurityGroup) this->SecurityGroup = kv[VALUE];
            else if (kv[KEY] == Field_Asset) this->Asset = kv[VALUE];
            else if (kv[KEY] == Field_SecurityID) this->SecurityID = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_SecurityTradingStatus) this->SecurityTradingStatus = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_HaltReason) this->HaltReason= stoi(kv[VALUE]);
            else if (kv[KEY] == Field_SecurityTradingEvent) this->SecurityTradingEvent= stoi(kv[VALUE]);

            kv.clear();
        }
        fieldssplit.clear();
    }


};

class MDIncrementalRefresh
{

public:
    std::string TransactTime;
    std::string MatchEventIndicator;
    int NoMDEntries;
    int NoOrderIDEntries = 0;
    std::vector<MDEntry> MDEntries;
    std::vector<OrderIdEntry> OrderIdEntries;

    std::vector<std::string> fieldssplit;
    std::vector<std::string> kv;

    void update(const std::string &message)
    {
        // Parse String

        MDEntry *currentMDEntry = nullptr;
        OrderIdEntry *currentOrderIDEntry = nullptr;

        string_split_optim(fieldssplit,message, FIX_FIELD_DELIMITER);

        for (auto fields : fieldssplit)
        {
            string_split_optim(kv,fields, FIX_KEY_DELIMITER);

            // Header :: MDIncrementalRefresh

            if (kv[KEY] == Field_TransactTime) this->TransactTime = kv[VALUE];

            else if (kv[KEY] == Field_MatchEventIndicator) this->MatchEventIndicator = kv[VALUE];
            else if (kv[KEY] == Field_NoMDEntries) this->NoMDEntries = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_NoOrderIDEntries) this->NoOrderIDEntries = stoi(kv[VALUE]);

                // Repeating Group :: MDEntry

            else if (kv[KEY] == Field_MDUpdateAction)
            {
                MDEntries.push_back(MDEntry());
                currentMDEntry = &MDEntries.back(); // use pointer for fast lookup on subsequent repeating group fields
                currentMDEntry->MDUpdateAction = kv[VALUE][STR_TO_CHAR];
            }
            else if (kv[KEY] == Field_MDEntryType) currentMDEntry->MDEntryType = kv[VALUE][STR_TO_CHAR];
            else if (kv[KEY] == Field_SecurityID) currentMDEntry->SecurityID = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_Symbol) currentMDEntry->Symbol = kv[VALUE];
            else if (kv[KEY] == Field_RptSeq) currentMDEntry->RptSeq = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_MDEntryPx) currentMDEntry->MDEntryPx = stod(kv[VALUE]);
            else if (kv[KEY] == Field_MDEntrySize) currentMDEntry->MDEntrySize = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_NumberOfOrders) currentMDEntry->NumberOfOrders = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_MDPriceLevel) currentMDEntry->MDPriceLevel = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_OpenCloseSettlFlag) currentMDEntry->OpenCloseSettlFlag = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_SettlPriceType) currentMDEntry->SettlPriceType = kv[VALUE];
            else if (kv[KEY] == Field_AggressorSide) currentMDEntry->AggressorSide = stoi(kv[VALUE]);
            else if (kv[KEY] == Field_TradingReferenceDate) currentMDEntry->TradingReferenceDate = kv[VALUE];
            else if (kv[KEY] == Field_HighLimitPrice) currentMDEntry->HighLimitPrice = stod(kv[VALUE]);
            else if (kv[KEY] == Field_LowLimitPrice) currentMDEntry->LowLimitPrice = stod(kv[VALUE]);
            else if (kv[KEY] == Field_MaxPriceVariation) currentMDEntry->MaxPriceVariation = stod(kv[VALUE]);
            else if (kv[KEY] == Field_ApplID) currentMDEntry->ApplID = stoi(kv[VALUE]);

                // Repeating Group :: OrderIDEntry
            else if (kv[KEY] == Field_OrderID)
            {
                OrderIdEntries.push_back(OrderIdEntry());
                currentOrderIDEntry = &OrderIdEntries.back();
                currentOrderIDEntry->OrderID = kv[VALUE];
            } else if (kv[KEY] == Field_LastQty) currentOrderIDEntry->LastQty = stol(kv[VALUE]);

            kv.clear();
        }
        fieldssplit.clear();
    }

    void clear()
    {
        this->MDEntries.clear();
        this->OrderIdEntries.clear();
    }
};

#endif //FIXDECODER_H
