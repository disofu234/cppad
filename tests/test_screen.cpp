#include <gtest/gtest.h>
#include "test_helpers.h"

TEST(ScreenAppend, Normal)
{
	CSCREEN screen = initialize_screen("", MAX_ROWS, MAX_COLS);
	PrintOperation op = screen.handle_append('a');
	EXPECT_EQ(op, None);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "a");
	EXPECT_EQ(screen.get_x(), 1);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenAppend, NormalRow)
{
	CSCREEN screen = initialize_screen("abcdefgh", MAX_ROWS, MAX_COLS);
	screen.right(2);
	PrintOperation op = screen.handle_append('e');
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "cdefg");
	EXPECT_EQ(screen.get_x(), 3);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenAppend, NormalEnd)
{
	CSCREEN screen = initialize_screen("abcd\nefgh", MAX_ROWS, MAX_COLS);
	screen.right(9);
	PrintOperation op = screen.handle_append('e');
	EXPECT_EQ(op, Screen);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "efghe");
	EXPECT_EQ(screen.get_x(), 1);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, Tab)
{
	CSCREEN screen = initialize_screen("", MAX_ROWS, MAX_COLS);
	PrintOperation op = screen.handle_append('\t');
	EXPECT_EQ(op, None);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "    ");
	EXPECT_EQ(screen.get_x(), 4);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenAppend, TabRow)
{
	CSCREEN screen = initialize_screen("abcdefgh", MAX_ROWS, MAX_COLS);
	screen.right(2);
	PrintOperation op = screen.handle_append('\t');
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "ab    cd");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "cd");
	EXPECT_EQ(screen.get_x(), 2);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, TabEnd)
{
	CSCREEN screen = initialize_screen("abcd\nefgh", MAX_ROWS, MAX_COLS);
	screen.right(9);
	PrintOperation op = screen.handle_append('\t');
	EXPECT_EQ(op, Screen);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "efgh    ");
	EXPECT_EQ(screen.get_x(), 4);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, Line)
{
	CSCREEN screen = initialize_screen("abc", MAX_ROWS, MAX_COLS);
	screen.right(3);
	PrintOperation op = screen.handle_append('\n');
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abc\n");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, LineMid)
{
	CSCREEN screen = initialize_screen("abcdefgh", MAX_ROWS, MAX_COLS);
	screen.right(3);
	PrintOperation op = screen.handle_append('\n');
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abc\ndefg");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "defg");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, LineRow)
{
	CSCREEN screen = initialize_screen("abcdefgh", MAX_ROWS, MAX_COLS);
	screen.right(4);
	PrintOperation op = screen.handle_append('\n');
	EXPECT_EQ(op, RemoveWrap);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abcd\nefgh");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenAppend, LineEnd)
{
	CSCREEN screen = initialize_screen("abcd\nefgh", MAX_ROWS, MAX_COLS);
	screen.right(6);
	PrintOperation op = screen.handle_append('\n');
	EXPECT_EQ(op, Screen);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "e\nfgh");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 1);
}

TEST(ScreenBackspace, Normal)
{
	CSCREEN screen = initialize_screen("abc\nefgh\ndee", MAX_ROWS, MAX_COLS);
	screen.right(1);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "bc\nefgh");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "bc\nefgh");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, NormalRow)
{
	CSCREEN screen = initialize_screen("abcde\ndee", MAX_ROWS, MAX_COLS);
	screen.right(2);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "acde\ndee");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "cde\ndee");
	EXPECT_EQ(screen.get_x(), 1);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, NormalEnd)
{
	CSCREEN screen = initialize_screen("abcd\n", MAX_ROWS, MAX_COLS);
	screen.right(4);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, None);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abc\n");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "\n");
	EXPECT_EQ(screen.get_x(), 3);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, Tab)
{
	CSCREEN screen = initialize_screen("\t", MAX_ROWS, MAX_COLS);
	screen.right(1);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, None);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "");
	EXPECT_EQ(screen.get_x(), 0);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, TabRow)
{
	CSCREEN screen = initialize_screen("ab\t", MAX_ROWS, MAX_COLS);
	screen.right(3);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "ab");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "");
	EXPECT_EQ(screen.get_x(), 2);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, LineStartAfterFirstRow)
{
	CSCREEN screen = initialize_screen("abc\nd", MAX_ROWS, MAX_COLS);
	screen.right(4);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, AfterCursor);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abcd");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "d");
	EXPECT_EQ(screen.get_x(), 3);
	EXPECT_EQ(screen.get_y(), 0);
}

TEST(ScreenBackspace, LineStartAtFirstRow)
{
	CSCREEN screen = initialize_screen("abc\ndef\nghi", MAX_ROWS, MAX_COLS);
	screen.right(8);
	screen.left(4);
	PrintOperation op = screen.handle_backspace();
	EXPECT_EQ(op, Screen);
	EXPECT_EQ(print_iterator(screen.get_iterator_at_start()), "abcdef");
	EXPECT_EQ(print_iterator(screen.get_iterator_at_cursor()), "def");
	EXPECT_EQ(screen.get_x(), 3);
	EXPECT_EQ(screen.get_y(), 0);
}
