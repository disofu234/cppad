#include "screen.h"

CSCREEN::CSCREEN(CONTENT& content, int rows, int cols) :
	content(content),
	rows(rows),
	cols(cols),
	first(*this) {}

std::string CSCREEN::print()
{
    std::string output;
    FIRST_POSITION pos = first;
    int curr_row = 0;
    int curr_col = 0;
    while (!pos.is_at_contents_end())
    {
        char ch = pos.next();
        if (ch == '\n')
        {
            if (curr_row + 1 == rows)
                break;

            output.push_back(ch);
            curr_row++;
            curr_col = 0;
            continue;
        }

        curr_col++;
        if (curr_col > cols)
        {
            curr_row++;
            if (curr_row == rows)
                break;
            curr_col = 1;
        }
        output.push_back(ch);
    }
    return output;
}

int CSCREEN::get_rows() const
{
	return rows;
}

int CSCREEN::get_cols() const
{
	return cols;
}

int CSCREEN::get_spaces_in_last_row(LINE_CIT line_it) const
{
    int line_size = line_it->size();
    if (line_size == 0)
    {
        return 0;
    }

    int remainder = line_size % cols;
    return remainder == 0 ? cols : remainder;
}

void CSCREEN::scroll_up()
{
	first.prev_row();
}

void CSCREEN::scroll_down()
{
	first.next_row();
}
