#include <ncurses.h>
#include <fstream>
#include "content_cursor.h"
#include "screen.h"
#include "screen_cursor.h"

#define CTRL_X 24
#define CTRL_O 15

class CPPAD
{
public:
	CPPAD(
		CONTENT& content,
		CSCREEN& screen,
		SCREEN_CURSOR& cursor,
		const std::string& filename,
		WINDOW* text_win,
		WINDOW* status_win)
		: content(content),
		  screen(screen),
		  cursor(cursor),
		  filename(filename),
		  text_win(text_win),
		  status_win(status_win),
		  is_modified(false) {}

	void start()
	{
		print();

		int ch;
		while (true)
		{
			ch = wgetch(text_win);
			if (ch == CTRL_X)
			{
				break;
			}

			handle_key(ch);
			print();
		}

		endwin();
	}

private:
	CONTENT& content;
	CSCREEN& screen;
	SCREEN_CURSOR& cursor;
	const std::string& filename;
	WINDOW* text_win;
	WINDOW* status_win;
	bool is_modified;

	void handle_key(int ch)
	{
		if (is_append_char(ch))
		{
			cursor.insert(ch);
			is_modified = true;
			return;
		}

		switch (ch)
		{
			case KEY_BACKSPACE:
				cursor.backspace();
				is_modified = true;
				return;
			case KEY_LEFT:
				cursor.left();
				return;
			case KEY_RIGHT:
				cursor.right();
				return;
			case KEY_UP:
				cursor.up();
				return;
			case KEY_DOWN:
				cursor.down();
				return;
			case CTRL_O:
				save();
				is_modified = false;
				return;
		}
	}

	void print()
	{
		wmove(status_win, 0, 0);
		wclrtobot(status_win);
		mvwhline(status_win, 0, 0, ACS_HLINE, COLS);
		wmove(status_win, 1, 0);
		std::string status_str = filename;
		if (is_modified)
		{
			status_str += "*";
		}
		waddstr(status_win, status_str.c_str());
		wnoutrefresh(status_win);

		wmove(text_win, 0, 0);
		wclrtobot(text_win);

		std::string screen_content = screen.print();
		int max_x = getmaxx(text_win);
		int max_y = getmaxy(text_win);
		for (char ch : screen_content)
		{
			int x, y;
			getyx(text_win, y, x);
			if (x == max_x - 1 && ch != '\n')
			{
				wmove(text_win, y + 1, 0);
			}
			waddch(text_win, ch);
		}

		wmove(text_win, cursor.get_y(), cursor.get_x());
		wnoutrefresh(text_win);

		doupdate();
	}

	bool is_append_char(int ch)
	{
		if (ch >= 32 && ch <= 126 || ch == '\n' || ch == '\t') return true;
		return false;
	}

	void save()
	{
		std::ofstream outfile(filename);
		if (!outfile)
		{
			throw std::runtime_error("Failed to open file for saving.");
		}

		outfile << get_content_string(content);
		outfile.close();
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
	noecho();

	WINDOW* text_win = newwin(LINES - 2, COLS, 0, 0);
	keypad(text_win, TRUE);

	WINDOW* status_win = newwin(2, COLS, LINES - 2, 0);

	CSCREEN screen(content, LINES - 2, COLS - 1);
	SCREEN_CURSOR cursor(screen);

	CPPAD cppad(content, screen, cursor, filename, text_win, status_win);
	cppad.start();
}
