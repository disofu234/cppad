template<typename CHARS_T>
CONTENT_CURSOR_T<CHARS_T>::CONTENT_CURSOR_T(CONTENT_T<CHARS_T>& content)
    : POSITION_T<CHARS_T>(content) {}

template<typename CHARS_T>
CONTENT_CURSOR_T<CHARS_T>::CONTENT_CURSOR_T(CONTENT_T<CHARS_T>& content,
    typename CONTENT_T<CHARS_T>::iterator line_it, int chars_x)
    : POSITION_T<CHARS_T>(content, line_it, chars_x) {}

template<typename CHARS_T>
INSERT CONTENT_CURSOR_T<CHARS_T>::insert(char ch)
{
    INSERT result{};

    if (ch == '\n')
    {
        insert_line();
        result.width = 0;
        return result;
    }

    if (ch == '\t')
    {
        this->line_it->tabs.insert(this->tabs_it, this->tabs_x);
        result.width = this->line_it->tabs.spaces(this->tabs_it);
        this->tabs_x = 0;
        this->tabs_it++;
    }
    else
    {
        result.width = 1;
        this->line_it->tabs.add_prev_chars(this->tabs_it, 1);
        this->tabs_x++;
    }

    this->char_it = this->line_it->chars.insert(this->char_it, ch);
    this->chars_x++;
    this->spaces_x += result.width;
    return result;
}

template<typename CHARS_T>
BACKSPACE CONTENT_CURSOR_T<CHARS_T>::backspace()
{
    BACKSPACE result{};

    if (this->is_at_contents_start())
    {
        result.ch = '\0';
        result.width = 0;
        return result;
    }

    if (this->is_at_line_start())
    {
        result.ch = '\n';
        result.width = 0;
        backspace_line();
        return result;
    }

    if (this->is_after_tab())
    {
        TABS_IT prev_tabs_it = std::prev(this->tabs_it);
        result.width = this->line_it->tabs.spaces(prev_tabs_it);
        int saved_tabs_x = prev_tabs_it->prev_chars;
        this->line_it->tabs.remove(this->tabs_it);
        this->tabs_it = prev_tabs_it;
        this->tabs_x = saved_tabs_x;
    }
    else
    {
        result.width = 1;
        this->line_it->tabs.add_prev_chars(this->tabs_it, -1);
        this->tabs_x--;
    }

    auto prev_char_it = std::prev(this->char_it);
    result.ch = *prev_char_it;
    this->char_it = this->line_it->chars.erase(prev_char_it);
    this->chars_x--;
    this->spaces_x -= result.width;
    return result;
}

template<typename CHARS_T>
LEFT CONTENT_CURSOR_T<CHARS_T>::left()
{
    LEFT result{};
    result.ch = this->prev();
    result.width = 0;

    if (result.ch == '\0' || result.ch == '\n')
        return result;

    if (this->is_tab())
    {
        result.width = this->line_it->tabs.spaces(this->tabs_it);
        return result;
    }

    result.width = 1;
    return result;
}

template<typename CHARS_T>
RIGHT CONTENT_CURSOR_T<CHARS_T>::right()
{
    RIGHT result{};
    result.ch = this->next();
    result.width = 0;

    if (result.ch == '\0' || result.ch == '\n')
        return result;

    if (this->is_after_tab())
    {
        result.width = this->line_it->tabs.spaces(std::prev(this->tabs_it));
        return result;
    }

    result.width = 1;
    return result;
}

template<typename CHARS_T>
void CONTENT_CURSOR_T<CHARS_T>::insert_line()
{
    auto new_line_it = this->content.insert(
        std::next(this->line_it),
        LINE_T<CHARS_T>(this->line_it->chars.create_sibling()));

    CONTENT_CURSOR_T<CHARS_T> insert_cursor(this->content, new_line_it);
    while (!this->is_at_line_end())
    {
        right();
        BACKSPACE removed = backspace();
        insert_cursor.insert(removed.ch);
    }

    this->line_it = new_line_it;
    this->char_it = new_line_it->chars.begin();
    this->tabs_it = new_line_it->tabs.begin();

    this->chars_x = 0;
    this->spaces_x = 0;
    this->tabs_x = 0;
}

template<typename CHARS_T>
void CONTENT_CURSOR_T<CHARS_T>::backspace_line()
{
    auto prev_line_it = std::prev(this->line_it);
    if (prev_line_it->chars.empty())
    {
        this->content.erase(prev_line_it);
        return;
    }

    CONTENT_CURSOR_T<CHARS_T> insert_cursor(
        this->content, prev_line_it, (int)prev_line_it->chars.size());

    POSITION_T<CHARS_T> saved_position = insert_cursor;
    saved_position.prev();

    while (!this->is_at_line_end())
    {
        right();
        BACKSPACE removed = backspace();
        insert_cursor.insert(removed.ch);
    }

    this->content.erase(this->line_it);

    saved_position.next();
    this->reset(saved_position);
}
