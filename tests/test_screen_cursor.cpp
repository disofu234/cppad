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

TEST_F(ScreenCursorTest, ConstructorNoScroll)
{
	std::istringstream input("abcd\nefgh\nijkl\n");
	initialize_content(content, input);
	CSCREEN screen(content, 3, 4);

	SCREEN_CURSOR cursor(screen, 0, 2, 1);

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcd\nefgh\nijkl");
}

TEST_F(ScreenCursorTest, ConstructorWithScroll)
{
	std::istringstream input("AAAA\nBBBB\nCCCC\nDDDD\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	SCREEN_CURSOR cursor(screen, 1, 2, 1);

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "BBBB\nCCCC");
}

TEST_F(ScreenCursorTest, ConstructorWithScrollBacktrack)
{
	std::istringstream input("AAAA\nBBBB\nCCCC\nDDDD\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	SCREEN_CURSOR cursor(screen, 2, 2, 0);

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "CCCC\nDDDD");
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

TEST_F(ScreenCursorTest, InsertTabWithText)
{
	std::istringstream input("ab");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen, 0, 2, 0);

	cursor.insert('\t');

	EXPECT_EQ(cursor.get_x(), TAB_SIZE);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "ab      ");
}

TEST_F(ScreenCursorTest, InsertTabWrapsAndScrolls)
{
	std::istringstream input("abcdefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);

	cursor.insert('\t');

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "      ");
}

TEST_F(ScreenCursorTest, InsertCharWrapsToNextRow)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 4, 0);

	cursor.insert('e');

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcde");
}

TEST_F(ScreenCursorTest, InsertCharWrapTriggersScroll)
{
	std::istringstream input("abcdefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 4, 1);

	std::string before = screen.print();
	cursor.insert('j');

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(before, "abcdefgh");
	EXPECT_EQ(screen.print(), "efghj");
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

TEST_F(ScreenCursorTest, InsertTabAtOriginWraps)
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

TEST_F(ScreenCursorTest, InsertTabAtOriginWrapsAndScrolls)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 3);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\t');

	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "     ");
}

TEST_F(ScreenCursorTest, InsertNewlineAtOriginOneRow)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CSCREEN screen(content, 1, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abc");
}

TEST_F(ScreenCursorTest, InsertNewlineAtOriginScrolls)
{
	std::istringstream input("abcde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 0, 0);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcd\ne");
}

TEST_F(ScreenCursorTest, InsertNewlineAtOriginScrollsAtLineStart)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "\nabc");
}

TEST_F(ScreenCursorTest, InsertNewlineAtRowStart)
{
	std::istringstream input("abcde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 1);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcd\ne");
}

TEST_F(ScreenCursorTest, InsertNewlineAtLastRowScrolls)
{
	std::istringstream input("abcdef");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 1, 1);

	cursor.insert('\n');

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "e\nf");
}

TEST_F(ScreenCursorTest, RightWithinRow)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, RightTab)
{
	std::istringstream input("\t");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), TAB_SIZE);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, RightTabWithText)
{
	std::istringstream input("ab\t");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen, 0, 2, 0);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), TAB_SIZE);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, RightTabWraps)
{
	std::istringstream input("a\tde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 1, 0);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, RightTabScrolls)
{
	std::istringstream input("abcdefgh\ta");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 2);
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "      a");
}

TEST_F(ScreenCursorTest, RightWraps)
{
	std::istringstream input("abcde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 3, 0);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, RightToLineEnd)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 3, 0);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 4);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, RightScrollsAtBottomRow)
{
	std::istringstream input("abcdefghi");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);
	std::string before = screen.print();

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(screen.print(), "efghi");
}

TEST_F(ScreenCursorTest, RightScrollsOnNewlineAtBottomRow)
{
	std::istringstream input("line1\nline2\nline3\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 5, 1);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(screen.print(), "line2\nline3");
}

TEST_F(ScreenCursorTest, RightNoOpAtEndOfContent)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	RIGHT result = cursor.right();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftWithinRow)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 1, 0);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftTab)
{
	std::istringstream input("\t");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen, 0, TAB_SIZE, 0);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftTabWithText)
{
	std::istringstream input("ab\t");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen, 0, TAB_SIZE, 0);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftTabWraps)
{
	std::istringstream input("a\tde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftTabScrolls)
{
	std::istringstream input("abcdefgh\ta");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 2, 1, 1);

	LEFT left = cursor.left();

	EXPECT_EQ(left.scrolls, 1);
	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "fgh       ");
}

TEST_F(ScreenCursorTest, LeftWraps)
{
	std::istringstream input("abcde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 1);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, LeftToLineEnd)
{
	std::istringstream input("abdefa\ncde");
	initialize_content(content, input);
	CSCREEN screen(content, 3, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 2);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 2);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, LeftScrollsAtTopRow)
{
	std::istringstream input("abcdefghi");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 0, 0);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 1);
	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcdefgh");
}

TEST_F(ScreenCursorTest, LeftScrollsAtNewlineOnTopRow)
{
	std::istringstream input("abcd\nefghi");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 0, 0);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 1);
	EXPECT_EQ(cursor.get_x(), 4);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcd\nefgh");
}

TEST_F(ScreenCursorTest, LeftNoOpAtStartOfContent)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	LEFT result = cursor.left();

	EXPECT_EQ(result.scrolls, 0);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, BackspaceWithinRow)
{
	std::istringstream input("ab");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 1, 0);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "b");
}

TEST_F(ScreenCursorTest, BackspaceAtOriginScrolls)
{
	std::istringstream input("abcdefghi");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 0, 0);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcefghi");
}

TEST_F(ScreenCursorTest, BackspaceToOriginScrollsLineEnd)
{
	std::istringstream input("abcde\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 1, 0);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 4);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcd\n");
}

TEST_F(ScreenCursorTest, BackspaceTab)
{
	std::istringstream input("abc\tde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 16);
	SCREEN_CURSOR cursor(screen, 0, TAB_SIZE, 0);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcde");
}

TEST_F(ScreenCursorTest, BackspaceTabWraps)
{
	std::istringstream input("a\tde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "ade");
}

TEST_F(ScreenCursorTest, BackspaceTabScrolls)
{
	std::istringstream input("abcdefgh\ta");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 5);
	SCREEN_CURSOR cursor(screen, 2, 1, 1);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "fgha");
}

TEST_F(ScreenCursorTest, BackspaceLineToPreviousRow)
{
	std::istringstream input("abc\ndef");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 1);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 3);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcdef");
}

TEST_F(ScreenCursorTest, BackspaceLineStaysInPlace)
{
	std::istringstream input("abcd\ne");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 1);

	cursor.backspace();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "abcde");
}

TEST_F(ScreenCursorTest, UpWithinRows)
{
	std::istringstream input("abcdefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 1);

	cursor.up();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, UpToTab)
{
	std::istringstream input("\tabcdefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 8);
	SCREEN_CURSOR cursor(screen, 0, 3, 1);

	cursor.up();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, UpToLine)
{
	std::istringstream input("a\nefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 2, 1);

	cursor.up();

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, UpAtTop)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.up();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, UpScrolls)
{
	std::istringstream input("abcd\na\nefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 1, 1, 0);

	cursor.up();

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 0);
	EXPECT_EQ(screen.print(), "abcd\na");
}

TEST_F(ScreenCursorTest, DownWithinRows)
{
	std::istringstream input("abcdefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 0, 0);

	cursor.down();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, DownToTab)
{
	std::istringstream input("abcdefgh\t");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 8);
	SCREEN_CURSOR cursor(screen, 0, 3, 0);

	cursor.down();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, DownToLine)
{
	std::istringstream input("efgh\na");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 2, 0);

	cursor.down();

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
}

TEST_F(ScreenCursorTest, DownAtBottom)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen);

	cursor.down();

	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_y(), 0);
}

TEST_F(ScreenCursorTest, DownScrolls)
{
	std::istringstream input("abcd\na\nefgh");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);
	SCREEN_CURSOR cursor(screen, 0, 1, 1);

	cursor.down();

	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_y(), 1);
	EXPECT_EQ(screen.print(), "a\nefgh");
}