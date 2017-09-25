//
// Created by Antoine Waugh on 24/04/2017.
//

#include "../src/DepthBook.h"
#include <gtest/gtest.h>

namespace {

class DepthListTest : public testing::Test {
public:
    DepthList list;

    DepthListTest()
        : list(true)
    {
    }

    void SetUp()
    {
        list = DepthList(true);
    }
};

TEST_F(DepthListTest, insert)
{
    list.insert(1300.25, 100, 1, MDEntryAction_NEW);

    std::vector<std::shared_ptr<PriceEntry> > entries = list.getEntries();

    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(entries[0]->price, 1300.25);
    ASSERT_EQ(entries[0]->quantity, 100);
}

TEST_F(DepthListTest, modifyValidIndex)
{
    list.insert(1300.25, 100, 1, MDEntryAction_NEW);
    list.insert(1300.25, 1012, 1, MDEntryAction_CHANGE);

    std::vector<std::shared_ptr<PriceEntry> > entries = list.getEntries();

    ASSERT_EQ(entries.size(), 1);
    ASSERT_EQ(entries[0]->price, 1300.25);
    ASSERT_EQ(entries[0]->quantity, 1012);
}

TEST_F(DepthListTest, modifyInvalidIndex)
{
    testing::internal::CaptureStdout();
    list.insert(1300.25, 100, 1, MDEntryAction_CHANGE);
    ASSERT_EQ("Depth Item at given price for modification does not exist : 1300.25\n", testing::internal::GetCapturedStdout());
}

TEST_F(DepthListTest, removeValidIndex)
{
    list.insert(1300.25, 100, 1, MDEntryAction_NEW);
    list.insert(1300.25, 100, 1, MDEntryAction_DELETE);

    std::vector<std::shared_ptr<PriceEntry> > entries = list.getEntries();

    ASSERT_EQ(entries.size(), 0);
}

TEST_F(DepthListTest, removeInvalidPrice)
{
    testing::internal::CaptureStdout();

    list.insert(1300.25, 100, 1, MDEntryAction_NEW);
    list.insert(1300.5, 100, 1, MDEntryAction_DELETE);

    ASSERT_EQ("Problem detected in depth cache - price level 1300.5 does not exist\n", testing::internal::GetCapturedStdout());
}

TEST_F(DepthListTest, removeInvalidIndex)
{
    testing::internal::CaptureStdout();
    list.insert(1300.25, 100, 1, MDEntryAction_DELETE);
    ASSERT_EQ("Problem detected in depth cache - price level 1300.25 does not exist\n", testing::internal::GetCapturedStdout());
}

TEST_F(DepthListTest, copyConstructor)
{
    DepthList copy(list);
    list.insert(1300.25, 100, 1, MDEntryAction_NEW);

    std::vector<std::shared_ptr<PriceEntry> > list_entries = list.getEntries();
    std::vector<std::shared_ptr<PriceEntry> > copy_entries = copy.getEntries();

    ASSERT_EQ(list_entries.size(), 1);
    ASSERT_EQ(copy_entries.size(), 0);

    // modify copy, assert original
}

TEST_F(DepthListTest, assignmentOperator)
{
    DepthList copy(true, 10);
    copy = list;

    copy.insert(1300.25, 100, 1, MDEntryAction_NEW);
    std::vector<std::shared_ptr<PriceEntry> > list_entries = list.getEntries();
    std::vector<std::shared_ptr<PriceEntry> > copy_entries = copy.getEntries();

    ASSERT_EQ(list_entries.size(), 0);
    ASSERT_EQ(copy_entries.size(), 1);
}
}
