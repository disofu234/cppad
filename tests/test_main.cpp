#include <gtest/gtest.h>
#include "test_helpers.h"

/* TEST(FileContentsReturned, OversizedFile) */
/* { */
/* 	std::string screen_content = ""; */
/* 	screen_content.reserve(MAX_SCREEN_CHARS); */
/* 	std::list<std::list<char>> contents = std::list<std::list<char>>{std::list<char>()}; */
/* 	EXPECT_THROW( */
/* 		ensure_file_and_read_contents("../assets/test_oversized", screen_content, contents, MAX_SCREEN_CHARS), */
/* 		std::runtime_error); */
/* } */
/*  */
/* TEST(FileContentsReturned, LargeFile) */
/* { */
/* 	std::string screen_content = ""; */
/* 	screen_content.reserve(MAX_SCREEN_CHARS); */
/* 	std::list<std::list<char>> contents = std::list<std::list<char>>{std::list<char>()}; */
/* 	EXPECT_NO_THROW( */
/* 		ensure_file_and_read_contents("../assets/test_large", screen_content, contents, MAX_SCREEN_CHARS)); */
/* } */
/*  */
/* TEST(FileContentsReturned, FileLargerThanScreen) */
/* { */
/* 	std::ofstream file("test"); */
/* 	file.clear(); */
/* 	file << "abcde"; */
/* 	file.close(); */
/*  */
/* 	std::list<std::list<char>> contents{std::list<char>()}; */
/* 	std::string screen_contents; */
/* 	ensure_file_and_read_contents("test", screen_contents, contents, 4); */
/* 	EXPECT_EQ(screen_contents, "abcd"); */
/* } */
/*  */
/* TEST(FileContentsReturned, FileSmallerThanScreen) */
/* { */
/* 	std::ofstream file("test"); */
/* 	file.clear(); */
/* 	file << "abcde"; */
/* 	file.close(); */
/*  */
/* 	std::list<std::list<char>> contents{std::list<char>()}; */
/* 	std::string screen_contents; */
/* 	ensure_file_and_read_contents("test", screen_contents, contents, 6); */
/* 	EXPECT_EQ(screen_contents, "abcde"); */
/* } */
/*  */
/* TEST(FileContentsList, NoNewLines) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("ab3#a", contents); */
/* 	std::list<std::list<char>> list = content_state.contents; */
/*  */
/* 	EXPECT_EQ(list.size(), 1); */
/*  */
/* 	std::list<char>& line = *content_state.y_it; */
/* 	EXPECT_EQ(line.size(), 5); */
/*  */
/* 	std::list<char>::iterator it = line.begin(); */
/* 	EXPECT_EQ('a', *(it++)); */
/* 	EXPECT_EQ('b', *(it++)); */
/* 	EXPECT_EQ('3', *(it++)); */
/* 	EXPECT_EQ('#', *(it++)); */
/* 	EXPECT_EQ('a', *(it++)); */
/* } */
/*  */
/* TEST(FileContentsList, NewLines) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("ab3#a\ndf;\nq12", contents); */
/* 	std::list<std::list<char>> list = content_state.contents; */
/*  */
/* 	EXPECT_EQ(list.size(), 3); */
/*  */
/* 	std::list<std::list<char>>::iterator list_it = list.begin(); */
/* 	std::list<char>* line = &*(list_it++); */
/* 	EXPECT_EQ(line->size(), 5); */
/*  */
/* 	std::list<char>::iterator it = line->begin(); */
/* 	EXPECT_EQ('a', *(it++)); */
/* 	EXPECT_EQ('b', *(it++)); */
/* 	EXPECT_EQ('3', *(it++)); */
/* 	EXPECT_EQ('#', *(it++)); */
/* 	EXPECT_EQ('a', *(it++)); */
/* 	 */
/* 	line = &*(list_it++); */
/* 	it = line->begin(); */
/* 	EXPECT_EQ('d', *(it++)); */
/* 	EXPECT_EQ('f', *(it++)); */
/* 	EXPECT_EQ(';', *(it++)); */
/*  */
/* 	line = &*list_it; */
/* 	it = line->begin(); */
/* 	EXPECT_EQ('q', *(it++)); */
/* 	EXPECT_EQ('1', *(it++)); */
/* 	EXPECT_EQ('2', *(it++)); */
/* } */
/*  */
/* TEST(FileContentsList, Empty) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("", contents); */
/* 	std::list<std::list<char>> list = content_state.contents; */
/* 	EXPECT_EQ(list.size(), 1); */
/*  */
/* 	std::list<char>* line = &*list.begin(); */
/* 	EXPECT_EQ(line->size(), 0); */
/* } */
/*  */
/* TEST(SaveFile, Empty) */
/* { */
/* 	std::string filename = "test"; */
/* 	std::string contents = "abc"; */
/* 	save_contents_to_file(filename, contents); */
/*  */
/* 	std::ifstream file(filename); */
/* 	std::stringstream buffer; */
/* 	buffer << file.rdbuf(); */
/* 	std::string file_contents = buffer.str(); */
/* 	EXPECT_EQ(file_contents, contents); */
/* } */
/*  */
/* TEST(SaveFile, NonEmpty) */
/* { */
/* 	std::string filename = "test"; */
/* 	std::string contents = "abc"; */
/* 	save_contents_to_file(filename, contents); */
/*  */
/* 	std::ifstream file(filename); */
/* 	std::stringstream buffer; */
/* 	buffer << file.rdbuf(); */
/* 	std::string file_contents = buffer.str(); */
/* 	EXPECT_EQ(file_contents, contents); */
/* } */
/*  */
/* TEST(GetStringFromIterator, Empty) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("", contents); */
/* 	std::list<std::list<char>>::iterator y_it = content_state.contents.end(); */
/* 	std::list<char>::iterator x_it_start = (*(--y_it)).end(); */
/* 	EXPECT_EQ( */
/* 		get_string_from_it(content_state, content_state.contents.end(), x_it_start, MAX_SCREEN_CHARS), */
/* 		""); */
/*  */
/* 	EXPECT_EQ( */
/* 		get_string_from_it(content_state, content_state.contents.begin(), x_it_start, MAX_SCREEN_CHARS), */
/* 		""); */
/* } */
/*  */
/* TEST(GetStringFromIterator, NonEmpty) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\ndef", contents); */
/* 	content_state.y_it--; */
/* 	content_state.x_it = (*content_state.y_it).begin(); */
/* 	content_state.x_it++; */
/* 	EXPECT_EQ( */
/* 		get_string_from_it(content_state, content_state.y_it, content_state.x_it, MAX_SCREEN_CHARS), */
/* 		"bc\ndef"); */
/* } */
/*  */
/* TEST(GetStringFromIterator, NonEmptyMax) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\ndef", contents); */
/* 	content_state.y_it--; */
/* 	content_state.x_it = (*content_state.y_it).begin(); */
/* 	content_state.x_it++; */
/* 	int maxSize = 5; */
/* 	EXPECT_EQ( */
/* 		get_string_from_it(content_state, content_state.y_it, content_state.x_it, maxSize), */
/* 		"bc\nde"); */
/* } */


/* TEST(Backspace, Empty) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("", contents); */
/* 	std::list<std::list<char>> initial_contents = content_state.contents; */
/* 	std::list<std::list<char>>::iterator initial_y_it = content_state.y_it; */
/* 	std::list<char>::iterator initial_x_it = content_state.x_it; */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, MAX_SCREEN_CHARS); */
/* 	EXPECT_EQ(content_state.contents, initial_contents); */
/* 	EXPECT_EQ(content_state.x_it, initial_x_it); */
/* 	EXPECT_EQ(content_state.y_it, initial_y_it); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), ""); */
/* 	EXPECT_EQ(backspace_state.bcase, NoContent); */
/* 	EXPECT_EQ(buffer, ""); */
/* 	EXPECT_EQ(backspace_state.line_size, 0); */
/* } */
/*  */
/* TEST(Backspace, LineStart) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\ndef", contents); */
/* 	(--(--(--content_state.x_it))); */
/* 	std::list<std::list<char>>::iterator expected_y_it = std::prev(content_state.y_it); */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, MAX_SCREEN_CHARS); */
/* 	EXPECT_EQ(content_state.contents.size(), 1); */
/* 	EXPECT_EQ(content_state.y_it, expected_y_it); */
/* 	std::list<char>& curr_line = *content_state.y_it; */
/* 	EXPECT_EQ(content_state.x_it, std::prev(curr_line.end(), 3)); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), "abcdef"); */
/* 	EXPECT_EQ(backspace_state.bcase, LineStart); */
/* 	EXPECT_EQ(buffer, "def"); */
/* 	EXPECT_EQ(backspace_state.line_size, 3); */
/* } */
/*  */
/* TEST(Backspace, EmptyLine) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\n", contents); */
/* 	std::list<std::list<char>>::iterator expected_y_it = std::prev(content_state.y_it); */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, MAX_SCREEN_CHARS); */
/* 	EXPECT_EQ(content_state.contents.size(), 1); */
/* 	EXPECT_EQ(content_state.y_it, expected_y_it); */
/* 	std::list<char>& curr_line = *content_state.y_it; */
/* 	EXPECT_EQ(content_state.x_it, curr_line.end()); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), "abc"); */
/* 	EXPECT_EQ(backspace_state.bcase, LineStart); */
/* 	EXPECT_EQ(buffer, ""); */
/* 	EXPECT_EQ(backspace_state.line_size, 3); */
/* } */
/*  */
/* TEST(Backspace, Nonempty) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\ndef", contents); */
/* 	std::list<std::list<char>>::iterator initial_y_it = content_state.y_it; */
/* 	std::list<char>::iterator initial_x_it = content_state.x_it; */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, MAX_SCREEN_CHARS); */
/* 	EXPECT_EQ(content_state.contents.size(), 2); */
/* 	EXPECT_EQ(content_state.y_it, initial_y_it); */
/* 	EXPECT_EQ(content_state.x_it, initial_x_it); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), "abc\nde"); */
/* 	EXPECT_EQ(backspace_state.bcase, Normal); */
/* 	EXPECT_EQ(buffer, ""); */
/* 	EXPECT_EQ(backspace_state.line_size, 0); */
/* } */
/*  */
/* TEST(Backspace, Tab) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\nde\t", contents); */
/* 	std::list<std::list<char>>::iterator initial_y_it = content_state.y_it; */
/* 	std::list<char>::iterator initial_x_it = content_state.x_it; */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, MAX_SCREEN_CHARS); */
/* 	EXPECT_EQ(content_state.contents.size(), 2); */
/* 	EXPECT_EQ(content_state.y_it, initial_y_it); */
/* 	EXPECT_EQ(content_state.x_it, initial_x_it); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), "abc\nde"); */
/* 	EXPECT_EQ(backspace_state.bcase, Tab); */
/* 	EXPECT_EQ(buffer, ""); */
/* 	EXPECT_EQ(backspace_state.line_size, 0); */
/* } */
/*  */
/* TEST(Backspace, LineStartMax) */
/* { */
/* 	std::list<std::list<char>> contents{std::list<char>{}}; */
/* 	CONTENT_STATE content_state = initialize_content_state_test("abc\ndef", contents); */
/* 	(--(--(--content_state.x_it))); */
/* 	std::list<std::list<char>>::iterator expected_y_it = std::prev(content_state.y_it); */
/* 	int maxSize = 2; */
/* 	std::string buffer; */
/* 	BACKSPACE_STATE backspace_state = handle_backspace_state(content_state, buffer, maxSize); */
/* 	EXPECT_EQ(content_state.contents.size(), 1); */
/* 	EXPECT_EQ(content_state.y_it, expected_y_it); */
/* 	std::list<char>& curr_line = *content_state.y_it; */
/* 	EXPECT_EQ(content_state.x_it, std::prev(curr_line.end(), 3)); */
/* 	EXPECT_EQ(get_string_content(content_state.contents), "abcdef"); */
/* 	EXPECT_EQ(backspace_state.bcase, LineStart); */
/* 	EXPECT_EQ(buffer, "de"); */
/* 	EXPECT_EQ(backspace_state.line_size, 3); */
/* } */

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
    	return RUN_ALL_TESTS();
}
