#include <ncurses.h>
#include "headers.h"

class CPPAD
{
public:
	CPPAD(std::ifstream& file, int max_y, int max_x) : 
		screen(file, max_y, max_x - 1),
		max_x(max_x),
		max_y(max_y) { };

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
	CSCREEN screen;
	int max_x;
	int max_y;

	void handle_append(int ch)
	{
		std::cerr << "handle_append(" << char(ch) << ")\n";
		PrintOperation op = screen.handle_append(ch);
		if (op == Tab)
		{
			for (int i = 0; i < 4; i++)
			{
				addch_wrln(' ');
			}
			return;
		}

		addch_wrln(ch);
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
	}

	void handle_left()
	{
		PrintOperation op = screen.handle_left();
		if (op == Screen) {
			print_screen();
			return;
		}
		move(screen.get_y(), screen.get_x());
	}

	void handle_right()
	{
		PrintOperation op = screen.handle_right();
		if (op == Screen) {
			print_screen();
			return;
		}
		move(screen.get_y(), screen.get_x());
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
			addch_wrln(ch);
		}
	}

	void addch_wrln(char ch)
	{
		int x, y;
		getyx(stdscr, y, x);
		if (x == max_x - 1 && ch != '\n')
		{
			addch(' ');
		}
		getyx(stdscr, y, x);
		addch(ch);
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

	CPPAD cppad(file, max_y, max_x);
	cppad.start();
}
