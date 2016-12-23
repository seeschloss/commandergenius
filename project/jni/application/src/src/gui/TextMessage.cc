/*
 * Copyright (C) 2003,2004 Daniel Heck, Ralf Westram
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

#include "gui/TextMessage.hh"
#include "ecl_font.hh"
#include "ecl_video.hh"
#include "resource_cache.hh"
#include "video.hh"
#include "nls.hh"
#include "display.hh"

#include <cassert>

#include <android/log.h>

#include <sstream>

using namespace ecl;
using namespace std;

namespace enigma { namespace gui {

    /* -------------------- TextMessage -------------------- */

    TextMessage::TextMessage (const char *text_, int timeout_) : 
        displayed(false),
        text     (text_),
        timeout  (timeout_),
        enterTickTime (0)
    {
        enterTickTime = SDL_GetTicks();

        display::RedrawAll(video::GetScreen());

        const video::VMInfo* vminfo = video::GetInfo();

        Font *f = enigma::GetFont("messagefont");
        std::vector<std::string> lines = wrapText(text, f, vminfo->sb_textarea.w);

        int line_height = f->get_height();
        int y_offset = (vminfo->sb_textarea.h - (line_height * lines.size())) / 2;
        for (size_t i = 0; i < lines.size(); i++) {
            gui::Label *textLabel = new gui::Label(lines[i], HALIGN_CENTER, VALIGN_TOP);
            textLabel->set_font(f);
            add(textLabel, Rect(vminfo->sb_textarea.x, y_offset + (line_height * i), vminfo->sb_textarea.w, line_height));
        }

        if (timeout <= 0) {
            Font *f_small = enigma::GetFont("messagedismissfont");
            gui::Label *textLabel = new gui::Label("tap to dismiss", HALIGN_CENTER, VALIGN_TOP);
            textLabel->set_font(f_small);
            add(textLabel, Rect(vminfo->sb_textarea.x, y_offset + (line_height * (lines.size() + 1)), vminfo->sb_textarea.w, line_height));
        }
    }

    bool TextMessage::on_event (const SDL_Event &e) 
    {
        if (e.type == SDL_MOUSEBUTTONDOWN) 
        {
            Menu::quit();
            return true;
        }
        return false;
    }

    std::vector<std::string> wrapText (std::string text, Font *f, int width)
    {
        int space_width = f->get_width(" ");

        std::vector<std::string> lines;
        std::string line = "";
        int line_width = 0;

        std::vector<std::string> tokens;
        std::string word;
        std::istringstream iss(text);
        while (iss >> word) {
            int word_width = f->get_width(word.c_str());

            if (line_width + word_width > width) {
                lines.push_back(line);
                line = "";
                line_width = 0;
            }

            line += word + " ";
            line_width += word_width + space_width;
        }

        lines.push_back(line);

        return lines;
    }

    void TextMessage::draw_background (ecl::GC &gc) 
    {
        if (timeout > 0 && SDL_GetTicks() - enterTickTime >= timeout * 1000) {
            Menu::quit();
            return;
        }
    }

    /* -------------------- Functions -------------------- */

    void displayText(const char *text, int timeout) 
    {
        TextMessage message(text, timeout);
        message.draw_all();
        message.manage();
    }

}} // namespace enigma::gui
