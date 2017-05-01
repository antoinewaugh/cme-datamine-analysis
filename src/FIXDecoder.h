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

extern const std::string Field_Asset;
extern const std::string Field_Symbol;
extern const std::string Field_TransactTime;
extern const std::string Field_TradeDate;
extern const std::string Field_MatchEventIndicator;
extern const std::string Field_NoMDEntries;
extern const std::string Field_MDUpdateAction;
extern const std::string Field_MDEntryType;
extern const std::string Field_SecurityID;
extern const std::string Field_SecurityGroup;
extern const std::string Field_SecurityTradingEvent;
extern const std::string Field_SecurityTradingStatus;
extern const std::string Field_RptSeq;
extern const std::string Field_MDEntryPx;
extern const std::string Field_MDEntrySize;
extern const std::string Field_NumberOfOrders;
extern const std::string Field_MDPriceLevel;
extern const std::string Field_OpenCloseSettlFlag;
extern const std::string Field_AggressorSide;
extern const std::string Field_TradingReferenceDate;
extern const std::string Field_HighLimitPrice;
extern const std::string Field_LowLimitPrice;
extern const std::string Field_MaxPriceVariation;
extern const std::string Field_ApplID;
extern const std::string Field_NoOrderIDEntries;
extern const std::string Field_OrderID;
extern const std::string Field_LastQty;
extern const std::string Field_SettlPriceType;
extern const std::string Field_HaltReason;

constexpr int Status_NotAvailableForTrading = 18;

constexpr char MDEntryType_BID = '0';
constexpr char MDEntryType_OFFER = '1';
constexpr char MDEntryType_TRADE = '2';
constexpr char MDEntryType_IMPLIED_BID = 'E';
constexpr char MDEntryType_IMPLIED_OFFER = 'F';

constexpr char MDEntryAction_NEW = '0';
constexpr char MDEntryAction_CHANGE = '1';
constexpr char MDEntryAction_DELETE = '2';

void string_split_optim(std::vector<std::string> &, const std::string &s, const char);

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

    void update(const std::string& );

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

    void update(const std::string &);
    void clear();

};

#endif //FIXDECODER_H
