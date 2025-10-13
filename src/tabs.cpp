#include "tabs.h"

TAB::TAB(int prev_chars) : prev_chars(prev_chars) {}

TABS::TABS() : tabs({TAB(0)}) {}

void TABS::insert(TABS_IT it, int x)
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    if (x > it->prev_chars)
    {
        throw std::runtime_error("cannot insert at position larger than previous characters");
    }
    
    int new_prev_chars = it->prev_chars - x;
    it->prev_chars = x;
    TABS_IT new_tab = tabs.insert(std::next(it), TAB(new_prev_chars));
}

void TABS::remove(TABS_IT it)
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    if (it == tabs.begin())
    {
        throw std::runtime_error("cannot remove at the first tab");
    }
    
    TABS_IT prev_it = std::prev(it);
    prev_it->prev_chars += it->prev_chars;
    tabs.erase(it);
}

bool TABS::is_tab_start(TABS_CIT it, int x) const
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    if (x > it->prev_chars)
    {
        throw std::runtime_error("cannot check if tab start at position larger than previous characters");
    }
    
    return it != std::prev(tabs.end()) && x == it->prev_chars;
}

bool TABS::is_tab_end(TABS_CIT it, int x) const
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    if (x > it->prev_chars)
    {
        throw std::runtime_error("cannot check if tab end at position larger than previous characters");
    }

    return it != tabs.begin() && x == 0;
}

bool TABS::is_tab(TABS_CIT it) const
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    return it != std::prev(tabs.end());
}

TABS_IT TABS::end()
{
    return tabs.end();
}

TABS_IT TABS::begin()
{
    return tabs.begin();
}

int TABS::tab_count() const
{
    return tabs.size() - 1;
}

int TABS::spaces(TABS_CIT it) const
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    if (it == std::prev(tabs.end()))
    {
        return 0;
    }

    return TAB_SIZE - (it->prev_chars % TAB_SIZE);
}

int TABS::spaces() const
{
    int total = 0;
    if (tabs.empty()) return 0;
    for (auto it = tabs.begin(); it != std::prev(tabs.end()); ++it)
    {
        total += spaces(it);
    }
    return total;
}

void TABS::add_prev_chars(TABS_IT it, int delta)
{
    if (it == tabs.end())
    {
        throw std::runtime_error("iterator is at the end");
    }

    it->prev_chars += delta;
}
