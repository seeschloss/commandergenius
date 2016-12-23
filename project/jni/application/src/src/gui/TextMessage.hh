/*
 * Copyright (C) 2002,2003 Daniel Heck, Ralf Westram
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ENIGMA_TEXT_MESSAGE_HH
#define ENIGMA_TEXT_MESSAGE_HH

#include "gui/Menu.hh"
#include "ecl_font.hh"

namespace enigma { namespace gui {
    class TextMessage : public Menu {
    public:
        TextMessage (const char *text_, int timeout_);
    private:
        bool on_event           (const SDL_Event &e);
        void draw_background    (ecl::GC &gc);

        const char      *text;
        int timeout;
        bool displayed;
        Uint32 enterTickTime;
    };

    void displayText (const char *text, int timeout);
    std::vector<std::string> wrapText (const std::string text, ecl::Font *f, int width);

}} // namespace enigma::gui
#endif // ENIGMA_TEXT_MESSAGE_HH

