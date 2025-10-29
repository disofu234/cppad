#include "position.h"
#include "screen.h"

#ifndef FIRST_POSITION_H
#define FIRST_POSITION_H

class CSCREEN;

class FIRST_POSITION : public POSITION
{
public:
	FIRST_POSITION(CSCREEN& screen);

	char next() override;
	char prev() override;

	void next_row();
	void prev_row();
	void set(LINE_IT line_it, int x = 0);

private:
	CSCREEN& screen;

	int max_tabs_x() const;
};

#endif