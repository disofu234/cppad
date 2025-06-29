#include <gtest/gtest.h>
#include "test_helpers.h"

TEST(ContentRight, NotEnd)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(1);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abc");
	EXPECT_EQ(content_state.get_x(), 1);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentRight, End)
{
	CONTENT_STATE content_state = initialize_content_state("a\nbc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(2);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), std::next(initial_y_it));
	EXPECT_EQ(content_state.get_contents_string(), "a\nbc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 1);
}

TEST(ContentRight, Tab)
{
	CONTENT_STATE content_state = initialize_content_state("a\tbc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(2);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "a\tbc");
	EXPECT_EQ(content_state.get_x(), 5);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentLeft, NotBeginning)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(1);
	content_state.left(1);
	EXPECT_EQ(*(content_state.get_char_it()), 'a');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentLeft, Beginning)
{
	CONTENT_STATE content_state = initialize_content_state("a\nbc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(2);
	content_state.left(1);
	EXPECT_EQ(content_state.get_char_it(), initial_y_it->end());
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "a\nbc");
	EXPECT_EQ(content_state.get_x(), 1);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentLeft, Tab)
{
	CONTENT_STATE content_state = initialize_content_state("a\tbc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	content_state.right(2);
	content_state.left(2);
	EXPECT_EQ(*(content_state.get_char_it()), 'a');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "a\tbc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentAppendChar, MidLine)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	AppendCase acase = content_state.handle_append('d');
	EXPECT_EQ(acase, AppendNormal);
	EXPECT_EQ(*std::prev(content_state.get_char_it()), 'd');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "dabc");
	EXPECT_EQ(content_state.get_x(), 1);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentAppendChar, EndOfLine)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	content_state.right(3);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	AppendCase acase = content_state.handle_append('d');
	EXPECT_EQ(acase, AppendNormal);
	EXPECT_EQ(*std::prev(content_state.get_char_it()), 'd');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abcd");
	EXPECT_EQ(content_state.get_x(), 4);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentAppendChar, NewLineEnd)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	content_state.right(3);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	AppendCase acase = content_state.handle_append('\n');
	EXPECT_EQ(acase, NewLine);
	CLINES::const_iterator next_y_it = std::next(initial_y_it);
	EXPECT_EQ(content_state.get_char_it(), next_y_it->begin());
	EXPECT_EQ(content_state.get_line_it(), next_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abc\n");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 1);
}

TEST(ContentAppendChar, NewLineMidLine)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	content_state.right(1);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	AppendCase acase = content_state.handle_append('\n');
	EXPECT_EQ(acase, NewLine);
	CLINES::const_iterator next_y_it = std::next(initial_y_it);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), next_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "a\nbc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 1);
}

TEST(ContentAppendChar, Tab)
{
	CONTENT_STATE content_state = initialize_content_state("ab\tc");
	content_state.right(1);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	AppendCase acase = content_state.handle_append('\t');
	EXPECT_EQ(acase, AppendTab);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "a\tb\tc");
	EXPECT_EQ(content_state.get_x(), 5);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentBackspace, MidLine)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	content_state.right(1);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	BackspaceCase bcase = content_state.handle_backspace();
	EXPECT_EQ(bcase, BackspaceNormal);
	EXPECT_EQ(*(content_state.get_char_it()), 'b');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "bc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentBackspace, NoContent)
{
	CONTENT_STATE content_state = initialize_content_state("abc");
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	BackspaceCase bcase = content_state.handle_backspace();
	EXPECT_EQ(bcase, NoContent);
	EXPECT_EQ(*(content_state.get_char_it()), 'a');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentBackspace, LineStart)
{
	CONTENT_STATE content_state = initialize_content_state("ab\nc");
	content_state.right(3);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	CLINES::const_iterator prev_y_it = std::prev(initial_y_it);
	BackspaceCase bcase = content_state.handle_backspace();
	EXPECT_EQ(bcase, LineStart);
	EXPECT_EQ(*(content_state.get_char_it()), 'c');
	EXPECT_EQ(content_state.get_line_it(), prev_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "abc");
	EXPECT_EQ(content_state.get_x(), 2);
	EXPECT_EQ(content_state.get_y(), 0);
}

TEST(ContentBackspace, Tab)
{
	CONTENT_STATE content_state = initialize_content_state("ab\n\tc");
	content_state.right(4);
	CLINES::const_iterator initial_y_it = content_state.get_line_it();
	BackspaceCase bcase = content_state.handle_backspace();
	EXPECT_EQ(bcase, BackspaceTab);
	EXPECT_EQ(*(content_state.get_char_it()), 'c');
	EXPECT_EQ(content_state.get_line_it(), initial_y_it);
	EXPECT_EQ(content_state.get_contents_string(), "ab\nc");
	EXPECT_EQ(content_state.get_x(), 0);
	EXPECT_EQ(content_state.get_y(), 1);
}
