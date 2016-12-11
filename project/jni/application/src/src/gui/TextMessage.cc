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

using namespace ecl;
using namespace std;

namespace enigma { namespace gui {

    /* -------------------- TextMessage -------------------- */

    TextMessage::TextMessage (const char *text_, int xoffset_) : 
        displayed(false),
        text        (text_),
        cfg         (xoffset_)
    {
        const video::VMInfo &vminfo = *video::GetInfo();
        const int vshrink = vminfo.width < 640 ? 1 : 0;
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
    
    void TextMessage::draw_background (ecl::GC &gc) 
    {
        const video::VMInfo* vminfo = video::GetInfo();

        if (!displayed) {
            displayed = true;
            display::RedrawAll(video::GetScreen());

            Font *f = enigma::GetFont("messagefont");

            int line_height = f->get_height();
            int space_width = f->get_width(" ");

            char* to_split = (char *)malloc(strlen(text + 1));
            strcpy(to_split, text);
            char* pch;
            pch = strtok(to_split, " ");

            std::vector<int> line_lengths;
            std::vector<std::string> lines;
            std::string line = "";
            int line_width = 0;

            while (pch != NULL) {
                int word_width = f->get_width(pch);

                if (line_width + word_width > 960) {
                    lines.push_back(line);
                    line_lengths.push_back(line_width);
                    line = "";
                    line_width = 0;
                }

                line += pch;
                line += " ";
                line_width += word_width;
                line_width += space_width;

                pch = strtok(NULL, " ");
            }
            lines.push_back(line);
            line_lengths.push_back(line_width);

            int y_offset = (vminfo->height - (line_height * lines.size())) / 2;

            for (size_t i = 0; i < lines.size(); i++) {
                int x_offset = (960 - line_lengths[i]) / 2;
                f->render(gc, 160 + x_offset,  y_offset + (line_height * i), lines[i].c_str());
            }
        }
    }
    
    /* -------------------- Functions -------------------- */
    
    void displayText(const char *text, int xoffset) 
    {
    //    FX_Fade (video::FADEOUT);
        TextMessage menu(text, xoffset);
        menu.draw_all();
    //    FX_Fade (video::FADEIN);
        menu.manage();
    }

}} // namespace enigma::gui
