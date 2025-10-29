#include <gtest/gtest.h>
#include "screen.h"

class ScreenTest : public ::testing::Test
{
protected:
	CONTENT content;
};

TEST_F(ScreenTest, Constructor)
{
	std::istringstream input("abc\ndefg\nhijkl");
	initialize_content(content, input);
	CSCREEN screen(content, 3, 4);
	EXPECT_EQ(screen.print(), "abc\ndefg\nhijk");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevRow)
{
	std::istringstream input("abcde");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	screen.first.next_row();
	EXPECT_EQ(screen.print(), "e");

	screen.first.prev_row();
	EXPECT_EQ(screen.print(), "abcde");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevRowLine)
{
	std::istringstream input("abc\nd\n");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	screen.first.next_row();
	EXPECT_EQ(screen.print(), "d\n");

	screen.first.prev_row();
	EXPECT_EQ(screen.print(), "abc\nd");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevRowEmpty)
{
	std::istringstream input("abcd");
	initialize_content(content, input);
	CSCREEN screen(content, 1, 4);

	screen.first.next_row();
	EXPECT_EQ(screen.print(), "");

	screen.first.prev_row();
	EXPECT_EQ(screen.print(), "abcd");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevRowEmptyLine)
{
	std::istringstream input("\nabc");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 4);

	screen.first.next_row();
	EXPECT_EQ(screen.print(), "abc");

	screen.first.prev_row();
	EXPECT_EQ(screen.print(), "\nabc");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevChar)
{
	std::istringstream input("abcdef");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 3);

	screen.first.next();
	EXPECT_EQ(screen.print(), "bcdef");

	screen.first.prev();
	EXPECT_EQ(screen.print(), "abcdef");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevCharLine)
{
	std::istringstream input("\ndef");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 3);

	screen.first.next();
	EXPECT_EQ(screen.print(), "def");

	screen.first.prev();
	EXPECT_EQ(screen.print(), "\ndef");
}

TEST_F(ScreenTest, MoveFirstNextAndPrevCharEmpty)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CSCREEN screen(content, 1, 3);

	screen.first.next();
	EXPECT_EQ(screen.print(), "");

	screen.first.prev();
	EXPECT_EQ(screen.print(), "a");
}

TEST_F(ScreenTest, MoveFirstPrevAtStart)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CSCREEN screen(content, 1, 3);
	EXPECT_EQ(screen.print(), "abc");

	screen.first.prev();
	EXPECT_EQ(screen.print(), "abc");

	screen.first.prev_row();
	EXPECT_EQ(screen.print(), "abc");
}

TEST_F(ScreenTest, EmptyContent)
{
	std::istringstream input("");
	initialize_content(content, input);
	CSCREEN screen(content, 2, 3);
	EXPECT_EQ(screen.print(), "");

	screen.first.next();
	EXPECT_EQ(screen.print(), "");
}
