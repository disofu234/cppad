#include <ncurses.h>
#include "headers.h"

class CPPAD
{
public:
	CPPAD(CSCREEN& screen) : screen(screen) { };

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
				// handle_action_char(content_state, ch, 0);
			}
		}
		
		// std::string scontents = get_string_content(content_state.contents);
		// save_contents_to_file(filename, scontents);
		
		endwin();
	}

private:
	CSCREEN& screen;

	int get_max_chars_screen()
	{
		int y, x;
		getmaxyx(stdscr, y, x);
		return y * (x - 1);
	}

	void handle_append(int ch)
	{
		std::cerr << "handle_append(" << char(ch) << ")\n";
		PrintOperation op = screen.handle_append(ch);
		addch(ch);
		if (op == AfterCursor)
		{
			print_after_cursor();
			return;
		}
		if (op == Screen)
		{
			print_screen();
			return;
		}
	}
	
	void handle_action_char(CONTENT_STATE& content_state, int ch, int max_size)
	{
		switch (ch)
		{
			case KEY_BACKSPACE:
				handle_backspace(content_state, max_size);
				return;
			case KEY_LEFT:
				handle_left(content_state, max_size);
				return;
		}
	}

	void handle_backspace(CONTENT_STATE& content_state, int max_size)
	{
	}

	void handle_left(CONTENT_STATE& content_state, int max_size)
	{
	}

	void print_screen()
	{
		move(0, 0);
		clrtobot();
		print_iterator(screen.get_iterator_at_start());
		move(screen.get_y(), screen.get_x());
	}

	void print_after_cursor()
	{
		clrtobot();
		print_iterator(screen.get_iterator_at_cursor());
		move(screen.get_y(), screen.get_x());
	}

	void print_iterator(SCREEN_ITERATOR it)
	{
		int max_x, max_y;
		getmaxyx(stdscr, max_y, max_x);
		int x, y;
		while (!it.is_at_end())
		{
			char ch = it.next();
			getyx(stdscr, y, x);
			if (x == max_x - 1 && ch != '\n')
			{
				wrln();
			}
			addch(ch);
		}
	}

	void wrln()
	{
		addch('>');
	}

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

	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	// Leave one column at the end of each row as a placeholder for a '>'
	// if the line in the row wraps to the next row
	CSCREEN screen(file, max_y, max_x - 1);

	CPPAD cppad(screen);
	cppad.start();
}
