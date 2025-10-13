#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include "screen_cursor.h"
#include "tabs.h"

class ScreenCursorTest : public ::testing::Test
{
protected:
	CONTENT content;
};

TEST_F(ScreenCursorTest, PositionsCursorWithinView)
{
	std::istringstream input("abcd\nefgh\nijkl\n");
	initialize_content(content, input);
	CSCREEN screen(content, 3, 4);

	SCREEN_CURSOR cursor(screen, 0, 2, 1);

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcd\nefgh\nijkl");
}

TEST_F(ScreenCursorTest, ScrollsToRequestedFirstRow)
{
	std::istringstream input("AAAA\nBBBB\nCCCC\nDDDD\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	SCREEN_CURSOR cursor(screen, 1, 2, 1);

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "BBBB\nCCCC");
}

TEST_F(ScreenCursorTest, ThrowsWhenArgumentsOutOfBounds)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	EXPECT_THROW(SCREEN_CURSOR(screen, -1, 0, 0), std::out_of_range);
	EXPECT_THROW(SCREEN_CURSOR(screen, 0, -1, 0), std::out_of_range);
	EXPECT_THROW(SCREEN_CURSOR(screen, 0, 0, -1), std::out_of_range);
	EXPECT_THROW(SCREEN_CURSOR(screen, 0, 4, 0), std::out_of_range);
	EXPECT_THROW(SCREEN_CURSOR(screen, 0, 0, 2), std::out_of_range);
}

TEST_F(ScreenCursorTest, ThrowsWhenTargetColumnPastLineEnd)
{
	std::istringstream input("ab\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 3);

	EXPECT_THROW(SCREEN_CURSOR(screen, 0, 4, 0), std::out_of_range);
}

TEST_F(ScreenCursorTest, ThrowsWhenTargetColumnInsideTab)
{
	std::istringstream input("\t\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 8);

	EXPECT_THROW(SCREEN_CURSOR(screen, 0, 1, 0), std::out_of_range);
}

TEST_F(ScreenCursorTest, ThrowsWhenRequestedRowBeyondDocument)
{
	std::istringstream input("abc\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	EXPECT_THROW(SCREEN_CURSOR(screen, 5, 0, 0), std::out_of_range);
}

TEST_F(ScreenCursorTest, InsertCharAdvancesPosition)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('a');

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "a");
}

TEST_F(ScreenCursorTest, InsertTabAtOriginAdvancesToTabStop)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\t');

	EXPECT_EQ(cursor.get_x(), TAB_SIZE);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "        ");
}

TEST_F(ScreenCursorTest, InsertCharWrapsToNextRow)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('a');
	cursor.insert('b');
	cursor.insert('c');
	cursor.insert('d');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcd");
}

TEST_F(ScreenCursorTest, InsertCharWrapTriggersScroll)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	for (char ch : std::string("abcdefg"))
	{
		cursor.insert(ch);
	}
	std::string before = screen.print();

	cursor.insert('h');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(before, "abcdefg");
	EXPECT_EQ(screen.print(), "efgh");
}

TEST_F(ScreenCursorTest, InsertNewlineAdvancesRow)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 3, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('x');
	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "x\n");
}

TEST_F(ScreenCursorTest, InsertNewlineAtOrigin)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "\n");
}

TEST_F(ScreenCursorTest, InsertTabAtOriginWrapsAndScrolls)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\t');

	EXPECT_EQ(cursor.get_x(), 4);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "        ");
}
