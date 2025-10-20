#include <ncurses.h>
#include <fstream>
#include "content_cursor.h"
#include "screen.h"
#include "screen_cursor.h"

class CPPAD
{
public:
	CPPAD(CSCREEN& screen, SCREEN_CURSOR& cursor) :
		screen(screen), cursor(cursor) {}

	void start()
	{
		int ch;
		while ((ch = getch()) != 3) // CTRL-C
		{
			if (is_append_char(ch))
			{
				handle_append(ch);
			}
			else
			{
				handle_action_char(ch);
			}
		}
		
		// std::string scontents = get_string_content(content_state.contents);
		// save_contents_to_file(filename, scontents);
		
		endwin();
	}

private:
	CSCREEN& screen;
	SCREEN_CURSOR& cursor;

	void handle_append(int ch)
	{
		cursor.insert(ch);
		print();
		move(cursor.get_y(), cursor.get_x());
	}
	
	void handle_action_char(int ch)
	{
		switch (ch)
		{
			case KEY_BACKSPACE:
				handle_backspace();
				return;
			case KEY_LEFT:
				handle_left();
				return;
			case KEY_RIGHT:
				handle_right();
				return;
		}
	}

	void handle_backspace()
	{
		cursor.backspace();
		print();
		move(cursor.get_y(), cursor.get_x());
	}

	void handle_left()
	{
		cursor.left();
		print();
		move(cursor.get_y(), cursor.get_x());
	}

	void handle_right()
	{
		cursor.right();
		print();
		move(cursor.get_y(), cursor.get_x());
	}

	void print()
	{
		move(0, 0);
		clrtobot();

		std::string screen_content = screen.print();

		int max_x, max_y;
		getmaxyx(stdscr, max_y, max_x);
		for (char ch : screen_content)
		{
			int x, y;
			getyx(stdscr, y, x);
			if (x == max_x - 1 && ch != '\n')
			{
				move(y + 1, 0);
			}
			addch(ch);
		}
	}

	// void print_iterator(SCREEN_ITERATOR it)
	// {
	// 	int max_x, max_y;
	// 	getmaxyx(stdscr, max_y, max_x);
	// 	int x, y;
	// 	while (!it.is_at_end())
	// 	{
	// 		char ch = it.next();
	// 		addch_wrln(ch);
	// 	}
	// }

	// void addch_wrln(char ch)
	// {
	// 	int x, y;
	// 	getyx(stdscr, y, x);
	// 	if (x == max_x - 1 && ch != '\n')
	// 	{
	// 		addch(' ');
	// 	}
	// 	getyx(stdscr, y, x);
	// 	addch(ch);
	// }

	bool is_append_char(int ch)
	{
		if (ch >= 32 && ch <= 126 || ch == '\n' || ch == '\t') return true;
		return false;
	}
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		return 1;
	}

	std::string filename = argv[1];
	std::ifstream file(filename);
	if (!file)
	{
		std::ofstream outfile(filename);
		if (!outfile)
		{
			throw std::runtime_error("Failed to open or create file.");
		}
		outfile.close();
		file.open(filename);
	}

	CONTENT content{};
	initialize_content(content, file);

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	CSCREEN screen(content, max_y, max_x - 1);
	SCREEN_CURSOR cursor(screen);

	CPPAD cppad(screen, cursor);
	cppad.start();
}
