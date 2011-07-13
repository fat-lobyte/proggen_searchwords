// search_std-strstr.cpp

/*
 *   proggen_searchtexts
 *   Copyright (C) 2011  Alexander Korsunsky
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "search_fatlobyte.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <numeric>

int SearchFatLobyte::seek( char const * filename )
{
    std::ofstream found_file(filename);
    if (!found_file)
    {
        perror("Failed to open output file");
        std::cerr<<"Aborting search\n";
        return 0;
    }

    for (auto& cur_text: _texts)
    {
        for (auto& cur_pat : _patterns)
        {
            const char *found_pointer = cur_text.text;
            while (found_pointer = std::strstr(found_pointer, cur_pat))
            {
                ++found_pointer; // increment so we aren't stuck on first hit
                ++cur_text.hit_count;
            }
        }

        if (cur_text.hit_count)
            found_file<<cur_text.id<<'\t'<<cur_text.hit_count<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, TextInfo& el) { return acc + el.hit_count; }
    );
}



