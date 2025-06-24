#ifndef __LY_RENDER_UTILS_HPP__
#define __LY_RENDER_UTILS_HPP__

namespace ly::render {
void set_raw_mode();
void unset_raw_mode();

void reset_cursor();
void enter_alternate_screen();
void leave_alternate_screen();
} // namespace ly::render

#endif
