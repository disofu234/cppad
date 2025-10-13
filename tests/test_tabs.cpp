#include <gtest/gtest.h>
#include "tabs.h"

class TabsTest : public ::testing::Test
{
protected:
    TABS tabs;
};

TEST_F(TabsTest, TabsConstructor)
{
    EXPECT_EQ(tabs.begin()->prev_chars, 0);
    EXPECT_EQ(tabs.spaces(tabs.begin()), 0);
    EXPECT_EQ(tabs.spaces(), 0);
    EXPECT_EQ(tabs.tab_count(), 0);
}

TEST_F(TabsTest, InsertTab)
{
    auto it = tabs.begin();
    tabs.add_prev_chars(it, 5);
    EXPECT_EQ(tabs.spaces(), 0);
    
    int before = tabs.spaces();
    tabs.insert(it, 3);
    int spaces = tabs.spaces() - before;
    auto second = std::next(it);
    EXPECT_EQ(spaces, 5);
    EXPECT_EQ(it->prev_chars, 3);
    EXPECT_EQ(second->prev_chars, 2);
    EXPECT_EQ(tabs.spaces(second), 0);
    EXPECT_EQ(tabs.spaces(), 5);

    int before2 = tabs.spaces();
    tabs.insert(second, 1);
    int spaces2 = tabs.spaces() - before2;
    EXPECT_EQ(spaces2, 7);
    EXPECT_EQ(second->prev_chars, 1);
    EXPECT_EQ(tabs.spaces(second), 7);
    EXPECT_EQ(tabs.spaces(), 12);

    auto third = std::next(second);
    EXPECT_EQ(third->prev_chars, 1);
    EXPECT_EQ(tabs.spaces(third), 0);

    tabs.insert(it, 1);
    EXPECT_EQ(it->prev_chars, 1);
    EXPECT_EQ(std::next(it)->prev_chars, 2);
    EXPECT_EQ(tabs.spaces(), 20);

    EXPECT_EQ(tabs.tab_count(), 3);
}

TEST_F(TabsTest, InsertAtEndThrows)
{
    auto end_it = tabs.end();
    EXPECT_THROW(tabs.insert(end_it, 5), std::runtime_error);
}

TEST_F(TabsTest, InsertPastPrevCharsThrows)
{
    auto it = tabs.begin();
    tabs.add_prev_chars(it, 5);
    EXPECT_THROW(tabs.insert(it, 6), std::runtime_error);
}

TEST_F(TabsTest, RemoveTab) {
    auto it = tabs.begin();
    tabs.add_prev_chars(it, 5);
    
    tabs.insert(it, 3);
    auto second = std::next(it);
    tabs.insert(second, 1);
    auto third = std::next(second);

    EXPECT_EQ(tabs.tab_count(), 2);
    EXPECT_EQ(tabs.spaces(), 12);

    int before3 = tabs.spaces();
    tabs.remove(second);
    int spaces = tabs.spaces() - before3;
    EXPECT_EQ(spaces, -8);
    EXPECT_EQ(it->prev_chars, 4);
    EXPECT_EQ(third->prev_chars, 1);
    EXPECT_EQ(tabs.spaces(), 4);
    EXPECT_EQ(tabs.tab_count(), 1);
}

TEST_F(TabsTest, RemoveAtBeginThrows) {
    EXPECT_THROW(tabs.remove(tabs.begin()), std::runtime_error);
}

TEST_F(TabsTest, IsTabStart)
{
    auto it = tabs.begin();
    tabs.add_prev_chars(it, 7);
    
    tabs.insert(it, 2);
    auto second = std::next(it);
    tabs.insert(second, 5);
    auto third = std::next(second);
    
    EXPECT_TRUE(tabs.is_tab_start(it, 2));
    EXPECT_FALSE(tabs.is_tab_start(it, 1));

    EXPECT_TRUE(tabs.is_tab_start(second, 5));
    EXPECT_FALSE(tabs.is_tab_start(second, 2));

    EXPECT_FALSE(tabs.is_tab_start(third, 0));
}

TEST_F(TabsTest, IsTabEnd)
{
    auto it = tabs.begin();
    tabs.add_prev_chars(it, 7);
    
    tabs.insert(it, 2);
    auto second = std::next(it);
    tabs.insert(second, 5);
    auto third = std::next(second);
    
    EXPECT_FALSE(tabs.is_tab_end(it, 0));
    EXPECT_FALSE(tabs.is_tab_end(it, 1));

    EXPECT_TRUE(tabs.is_tab_end(second, 0));
    EXPECT_FALSE(tabs.is_tab_end(second, 4));

    EXPECT_TRUE(tabs.is_tab_end(third, 0));
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
