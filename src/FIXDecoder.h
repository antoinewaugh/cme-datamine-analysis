#pragma once

#include <string>
#include <vector>
#include <variant>

constexpr char FIX_FIELD_DELIMITER = '\x01';
constexpr char FIX_KEY_DELIMITER = '=';

constexpr int STR_TO_CHAR = 0;
constexpr int KEY = 0;
constexpr int VALUE = 1;

constexpr auto Field_Asset = "6937";
constexpr auto Field_Symbol = "55";
constexpr auto Field_TransactTime = "60";
constexpr auto Field_SendingTime = "52";
constexpr auto Field_TradeDate = "75";
constexpr auto Field_MatchEventIndicator = "5799";
constexpr auto Field_NoMDEntries = "268";
constexpr auto Field_MDUpdateAction = "279";
constexpr auto Field_MDEntryType = "269";
constexpr auto Field_SecurityID = "48";
constexpr auto Field_SecurityGroup = "1151";
constexpr auto Field_SecurityTradingEvent = "1174";
constexpr auto Field_SecurityTradingStatus = "326";
constexpr auto Field_RptSeq = "83";
constexpr auto Field_MDEntryPx = "270";
constexpr auto Field_MDEntrySize = "271";
constexpr auto Field_NumberOfOrders = "346";
constexpr auto Field_MDPriceLevel = "1023";
constexpr auto Field_OpenCloseSettlFlag = "286";
constexpr auto Field_AggressorSide = "5797";
constexpr auto Field_TradingReferenceDate = "5796";
constexpr auto Field_HighLimitPrice = "1149";
constexpr auto Field_LowLimitPrice = "1148";
constexpr auto Field_MaxPriceVariation = "1143";
constexpr auto Field_ApplID = "1180";
constexpr auto Field_NoOrderIDEntries = "37705";
constexpr auto Field_OrderID = "37";
constexpr auto Field_LastQty = "32";
constexpr auto Field_SettlPriceType = "731";
constexpr auto Field_HaltReason = "327";
constexpr auto Field_MarketDepth = "264";
constexpr int Status_NotAvailableForTrading = 18;

constexpr char MDEntryType_BID = '0';
constexpr char MDEntryType_OFFER = '1';
constexpr char MDEntryType_TRADE = '2';
constexpr char MDEntryType_IMPLIED_BID = 'E';
constexpr char MDEntryType_IMPLIED_OFFER = 'F';

constexpr char MDEntryAction_NEW = '0';
constexpr char MDEntryAction_CHANGE = '1';
constexpr char MDEntryAction_DELETE = '2';

void string_split_optim(std::vector<std::string_view>&, std::string_view s, const char);

class OrderIdEntry {
public:
    std::string OrderID;
    int LastQty;
};

struct MDEntry {
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

struct MDSecurityStatus {
public:
    std::string_view TransactTime;
    std::string_view SendingTime;
    std::string_view TradeDate;
    std::string_view MatchEventIndicator;
    std::string_view SecurityGroup;
    std::string_view Asset;

    int SecurityID;
    int SecurityTradingStatus;
    int HaltReason;
    int SecurityTradingEvent;

    std::vector<std::string_view> fieldssplit;
    std::vector<std::string_view> kv;

    void update(std::string_view);
};

struct MDInstrument {
public:
    std::string_view SecurityGroup;
    std::string Symbol;

    int MaxDepthSupported = 0;
    int MaxImplDepthSupported = 0;

    std::vector<std::string_view> fieldssplit;
    std::vector<std::string_view> kv;

    void update(std::string_view);
};

class MDIncrementalRefresh {

public:
    std::string_view TransactTime;
    std::string_view SendingTime;
    std::string_view MatchEventIndicator;
    int NoMDEntries;
    int NoOrderIDEntries = 0;
    std::vector<MDEntry> MDEntries;
    std::vector<OrderIdEntry> OrderIdEntries;

    std::vector<std::string_view> fieldssplit;
    std::vector<std::string_view> kv;

    void update(std::string_view);
    void clear();
};

static bool isStatusUpdate(std::string_view message) {
   return message.find("35=f")  != std::string::npos;
}

static bool isIncrementalUpdate(std::string_view message) {
    return message.find("35=X")  != std::string::npos;
}

class Decoder {
public:
    enum class Type {
        IncrementalRefresh,
        SecurityStatus,
        Unknown
    };

    Type headerType(std::string_view message) {
       if(isIncrementalUpdate(message)) {
           return Type::IncrementalRefresh;
       }
       if(isStatusUpdate(message)) {
           return Type::SecurityStatus;
       }
       return Type::Unknown;
    }

    MDIncrementalRefresh const&
    parseIncremental(std::string_view message) {
        m_refresh.clear();
        m_refresh.update(message);
        return m_refresh;
    }

    MDSecurityStatus const&
    parseStatus(std::string_view message) {
        m_status.update(message);
        return m_status;
    }

private:
    MDIncrementalRefresh m_refresh;
    MDSecurityStatus m_status;
};

