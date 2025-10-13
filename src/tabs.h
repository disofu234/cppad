#ifndef TABS_H
#define TABS_H

#include <list>
#include <iterator>
#include <stdexcept>

constexpr int TAB_SIZE = 8;

class TAB
{
public:
	TAB(int prev_chars);
	int prev_chars;
};

typedef std::list<TAB>::iterator TABS_IT;
typedef std::list<TAB>::const_iterator TABS_CIT;

class TABS
{
public:
    TABS();
    
    void insert(TABS_IT it, int x);
    void remove(TABS_IT it);

	bool is_tab_start(TABS_CIT it, int x) const;
	bool is_tab_end(TABS_CIT it, int x) const;

	bool is_tab(TABS_CIT it) const;

	TABS_IT end();
	TABS_IT begin();

	int tab_count() const;
	int spaces(TABS_CIT it) const;
    int spaces() const;

    void add_prev_chars(TABS_IT it, int delta);

private:
    std::list<TAB> tabs;
};

#endif
