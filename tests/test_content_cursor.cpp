#include <gtest/gtest.h>
#include "content_cursor.h"

class ContentCursorTest : public ::testing::Test
{
protected:
	CONTENT content;
};

TEST_F(ContentCursorTest, Constructor)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_char_it(), content.begin()->chars.begin());
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 0);
	EXPECT_EQ(get_content_string(content), "");
}

TEST_F(ContentCursorTest, InsertNormal)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
	INSERT ins = cursor.insert('a');
	EXPECT_EQ(ins.width, 1);
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(content.begin()->size(), 1);
	EXPECT_EQ(get_content_string(content), "a");
}

TEST_F(ContentCursorTest, LeftNormal)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);
	std::cout << cursor.get_x();
	LEFT l = cursor.left();
	EXPECT_EQ(l.ch, 'a');
	EXPECT_EQ(l.width, 1);
	EXPECT_EQ(cursor.get_char(), 'a');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(get_content_string(content), "a");
}

TEST_F(ContentCursorTest, RightNormal)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
    RIGHT r = cursor.right();
    EXPECT_EQ(r.ch, 'a');
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(get_content_string(content), "a");
}

TEST_F(ContentCursorTest, BackspaceNormal)
{
	std::istringstream input("a");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 1);
	char ch = cursor.backspace();
	EXPECT_EQ(ch, 'a');
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 0);
	EXPECT_EQ(get_content_string(content), "");
}

TEST_F(ContentCursorTest, InsertLine)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
	INSERT ins = cursor.insert('\n');
	LINE_IT exp_line_it = std::next(content.begin());
	EXPECT_EQ(ins.width, 0);
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), exp_line_it);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(exp_line_it->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 0);
	EXPECT_EQ(std::next(content.begin())->size(), 0);
	EXPECT_EQ(get_content_string(content), "\n");
}

TEST_F(ContentCursorTest, LeftLine)
{
	std::istringstream input("\n");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, std::next(content.begin()));
	LEFT l = cursor.left();
	EXPECT_EQ(l.ch, '\n');
	EXPECT_EQ(l.width, 0);
	LINE_IT exp_line_it = content.begin();
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), exp_line_it);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(exp_line_it->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(get_content_string(content), "\n");
}

TEST_F(ContentCursorTest, RightLine)
{
	std::istringstream input("\nbc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
    RIGHT r = cursor.right();
    EXPECT_EQ(r.ch, '\n');
	LINE_IT exp_line_it = std::next(content.begin());
	EXPECT_EQ(cursor.get_char(), 'b');
	EXPECT_EQ(cursor.get_line_it(), exp_line_it);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(exp_line_it->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(get_content_string(content), "\nbc");
}

TEST_F(ContentCursorTest, BackspaceLine)
{
	std::istringstream input("a\nbc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, std::next(content.begin()));
	char ch = cursor.backspace();
	EXPECT_EQ(ch, '\n');
	EXPECT_EQ(cursor.get_char(), 'b');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(content.begin()->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(content.begin()->size(), 3);
	EXPECT_EQ(get_content_string(content), "abc");
}

TEST_F(ContentCursorTest, InsertLineMid)
{
	std::istringstream input("abcdef");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 3);
	INSERT ins = cursor.insert('\n');
	LINE_IT exp_line_it = std::next(content.begin());
	EXPECT_EQ(ins.width, 0);
	EXPECT_EQ(cursor.get_char(), 'd');
	EXPECT_EQ(cursor.get_line_it(), exp_line_it);
	EXPECT_EQ(cursor.get_x(), 0);
	EXPECT_EQ(cursor.get_tabs_it(), std::prev(exp_line_it->tabs.end()));
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 3);
	EXPECT_EQ(std::next(content.begin())->size(), 3);
	EXPECT_EQ(get_content_string(content), "abc\ndef");
}

TEST_F(ContentCursorTest, InsertTab)
{
	std::istringstream input("");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
	INSERT ins = cursor.insert('\t');
	EXPECT_EQ(ins.width, TAB_SIZE);
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	TABS_IT exp_tabs_it = std::prev(content.begin()->tabs.end());
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 0);
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 8);
	EXPECT_EQ(get_content_string(content), "\t");
}

TEST_F(ContentCursorTest, InsertTabWithText)
{
	std::istringstream input("abc");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 3);
	INSERT ins = cursor.insert('\t');
	EXPECT_EQ(ins.width, TAB_SIZE - 3);
	EXPECT_EQ(cursor.get_char(), '\0');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 4);
	TABS_IT exp_tabs_it = std::prev(content.begin()->tabs.end());
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 0);
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 8);
	EXPECT_EQ(get_content_string(content), "abc\t");
}

TEST_F(ContentCursorTest, InsertBeforeTab)
{
	std::istringstream input("\t");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
	INSERT ins = cursor.insert('a');
	EXPECT_EQ(ins.width, 1);
	EXPECT_EQ(cursor.get_char(), '\t');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	TABS_IT exp_tabs_it = content.begin()->tabs.begin();
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 1);
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(content.begin()->size(), 8);
	EXPECT_EQ(get_content_string(content), "a\t");
}

TEST_F(ContentCursorTest, LeftTab)
{
	std::istringstream input("a\tde");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);
	LEFT l = cursor.left();
	EXPECT_EQ(l.ch, '\t');
	EXPECT_EQ(l.width, TAB_SIZE - 1);
	EXPECT_EQ(cursor.get_char(), '\t');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	TABS_IT exp_tabs_it = content.begin()->tabs.begin();
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 1);
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(get_content_string(content), "a\tde");
}

TEST_F(ContentCursorTest, RightTab)
{
	std::istringstream input("\tde");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content);
    RIGHT r = cursor.right();
    EXPECT_EQ(r.ch, '\t');
	EXPECT_EQ(cursor.get_char(), 'd');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	TABS_IT exp_tabs_it = std::prev(content.begin()->tabs.end());
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 2);
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(get_content_string(content), "\tde");
}

TEST_F(ContentCursorTest, BackspaceTab)
{
	std::istringstream input("abc\tde");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 4);
	char ch = cursor.backspace();
	EXPECT_EQ(ch, '\t');
	EXPECT_EQ(cursor.get_char(), 'd');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 3);
	TABS_IT exp_tabs_it = content.begin()->tabs.begin();
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 5);
	EXPECT_EQ(cursor.get_tabs_x(), 3);
	EXPECT_EQ(content.begin()->size(), 5);
	EXPECT_EQ(get_content_string(content), "abcde");
}

TEST_F(ContentCursorTest, BackspaceBeforeTab)
{
	std::istringstream input("ab\tde");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 2);
	char ch = cursor.backspace();
	EXPECT_EQ(ch, 'b');
	EXPECT_EQ(cursor.get_char(), '\t');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 1);
	TABS_IT exp_tabs_it = content.begin()->tabs.begin();
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 1);
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(content.begin()->size(), 10);
	EXPECT_EQ(get_content_string(content), "a\tde");
}

TEST_F(ContentCursorTest, BackspaceLineTabs)
{
	std::istringstream input("ab\te\naa\tba\t");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, std::next(content.begin()));
	char ch = cursor.backspace();
	EXPECT_EQ(ch, '\n');
	EXPECT_EQ(cursor.get_char(), 'a');
	EXPECT_EQ(cursor.get_line_it(), content.begin());
	EXPECT_EQ(cursor.get_x(), 4);
	TABS_IT exp_tabs_it = std::next(content.begin()->tabs.begin());
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 3);
	EXPECT_EQ(cursor.get_tabs_x(), 1);
	EXPECT_EQ(content.begin()->size(), 24);
	EXPECT_EQ(get_content_string(content), "ab\teaa\tba\t");
}

TEST_F(ContentCursorTest, InsertLineTabs)
{
	std::istringstream input("abc\tee\ta");
	initialize_content(content, input);
	CONTENT_CURSOR cursor(content, content.begin(), 4);
	INSERT ins = cursor.insert('\n');
	EXPECT_EQ(ins.width, 0);
	EXPECT_EQ(cursor.get_char(), 'e');
	LINE_IT exp_line_it = std::next(content.begin());
	EXPECT_EQ(cursor.get_line_it(), exp_line_it);
	EXPECT_EQ(cursor.get_x(), 0);
	TABS_IT exp_tabs_it = std::next(content.begin())->tabs.begin();
	EXPECT_EQ(cursor.get_tabs_it(), exp_tabs_it);
	EXPECT_EQ(exp_tabs_it->prev_chars, 2);
	EXPECT_EQ(cursor.get_tabs_x(), 0);
	EXPECT_EQ(content.begin()->size(), 8);
	EXPECT_EQ(std::next(content.begin())->size(), 9);
	EXPECT_EQ(get_content_string(content), "abc\t\nee\ta");
}
