#include "FIXDecoder.h"
#include <string_view>


void string_split_optim(std::vector<std::string_view>& output, std::string_view s, const char delimiter)
{
    size_t start = 0;
    size_t end = s.find_first_of(delimiter);

    while (end <= std::string::npos) {
        output.emplace_back(s.substr(start, end - start));

        if (end == std::string::npos)
            break;

        start = end + 1;
        end = s.find_first_of(delimiter, start);
    }
}

void MDSecurityStatus::update(std::string_view message)
{

    string_split_optim(fieldssplit, message, FIX_FIELD_DELIMITER);

    for (auto fields : fieldssplit) {
        string_split_optim(kv, fields, FIX_KEY_DELIMITER);

        // :: MDSecurityStatus

        if (kv[KEY] == Field_TransactTime)
            this->TransactTime = kv[VALUE];
        else if (kv[KEY] == Field_SendingTime)
            this->SendingTime = kv[VALUE];
        else if (kv[KEY] == Field_TradeDate)
            this->TradeDate = kv[VALUE];
        else if (kv[KEY] == Field_MatchEventIndicator)
            this->MatchEventIndicator = kv[VALUE];
        else if (kv[KEY] == Field_SecurityGroup)
            this->SecurityGroup = kv[VALUE];
        else if (kv[KEY] == Field_Asset)
            this->Asset = kv[VALUE];
        else if (kv[KEY] == Field_SecurityID)
            this->SecurityID = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_SecurityTradingStatus)
            this->SecurityTradingStatus = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_HaltReason)
            this->HaltReason = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_SecurityTradingEvent)
            this->SecurityTradingEvent = stoi(std::string(kv[VALUE]));

        kv.clear();
    }
    fieldssplit.clear();
}

void MDIncrementalRefresh::update(std::string_view message)
{
    // Parse String

    MDEntry* currentMDEntry = nullptr;
    OrderIdEntry* currentOrderIDEntry = nullptr;

    string_split_optim(fieldssplit, message, FIX_FIELD_DELIMITER);

    for (auto fields : fieldssplit) {
        string_split_optim(kv, fields, FIX_KEY_DELIMITER);

        // Header :: MDIncrementalRefresh

        if (kv[KEY] == Field_TransactTime)
            this->TransactTime = kv[VALUE];
        else if (kv[KEY] == Field_SendingTime)
            this->SendingTime = kv[VALUE];
        else if (kv[KEY] == Field_MatchEventIndicator)
            this->MatchEventIndicator = kv[VALUE];
        else if (kv[KEY] == Field_NoMDEntries)
            this->NoMDEntries = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_NoOrderIDEntries)
            this->NoOrderIDEntries = stoi(std::string(kv[VALUE]));

        // Repeating Group :: MDEntry

        else if (kv[KEY] == Field_MDUpdateAction) {
            MDEntries.push_back(MDEntry());
            currentMDEntry = &MDEntries.back(); // use pointer for fast lookup on subsequent repeating group fields
            currentMDEntry->MDUpdateAction = kv[VALUE][STR_TO_CHAR];
        } else if (kv[KEY] == Field_MDEntryType)
            currentMDEntry->MDEntryType = kv[VALUE][STR_TO_CHAR];
        else if (kv[KEY] == Field_SecurityID)
            currentMDEntry->SecurityID = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_Symbol)
            currentMDEntry->Symbol = kv[VALUE];
        else if (kv[KEY] == Field_RptSeq)
            currentMDEntry->RptSeq = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_MDEntryPx)
            currentMDEntry->MDEntryPx = stod(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_MDEntrySize)
            currentMDEntry->MDEntrySize = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_NumberOfOrders)
            currentMDEntry->NumberOfOrders = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_MDPriceLevel)
            currentMDEntry->MDPriceLevel = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_OpenCloseSettlFlag)
            currentMDEntry->OpenCloseSettlFlag = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_SettlPriceType)
            currentMDEntry->SettlPriceType = kv[VALUE];
        else if (kv[KEY] == Field_AggressorSide)
            currentMDEntry->AggressorSide = stoi(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_TradingReferenceDate)
            currentMDEntry->TradingReferenceDate = kv[VALUE];
        else if (kv[KEY] == Field_HighLimitPrice)
            currentMDEntry->HighLimitPrice = stod(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_LowLimitPrice)
            currentMDEntry->LowLimitPrice = stod(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_MaxPriceVariation)
            currentMDEntry->MaxPriceVariation = stod(std::string(kv[VALUE]));
        else if (kv[KEY] == Field_ApplID)
            currentMDEntry->ApplID = stoi(std::string(kv[VALUE]));

        // Repeating Group :: OrderIDEntry
        else if (kv[KEY] == Field_OrderID) {
            OrderIdEntries.push_back(OrderIdEntry());
            currentOrderIDEntry = &OrderIdEntries.back();
            currentOrderIDEntry->OrderID = kv[VALUE];
        } else if (kv[KEY] == Field_LastQty)
            currentOrderIDEntry->LastQty = stol(std::string(kv[VALUE]));

        kv.clear();
    }
    fieldssplit.clear();
}

void MDIncrementalRefresh::clear()
{
    this->MDEntries.clear();
    this->OrderIdEntries.clear();
}

void MDInstrument::update(std::string_view message)
{
    // Parse String
    string_split_optim(fieldssplit, message, FIX_FIELD_DELIMITER);

    for (auto fields : fieldssplit) {
        string_split_optim(kv, fields, FIX_KEY_DELIMITER);

        if (kv[KEY] == Field_Symbol)
            this->Symbol = kv[VALUE];
        else if (kv[KEY] == Field_SecurityGroup)
            this->SecurityGroup = kv[VALUE];
        else if(kv[KEY] == Field_MarketDepth) {
            // default to assigning MaxDepth , otherwise Impl.
            if (MaxDepthSupported == 0) {
               this->MaxDepthSupported = stoi(std::string(kv[VALUE]));
            } else {
               this->MaxImplDepthSupported= stoi(std::string(kv[VALUE]));
            }
        }

        kv.clear();
    }
    fieldssplit.clear();
}
