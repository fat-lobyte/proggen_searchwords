// search_straightforward.cpp

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
#include <algorithm>

void SearchFatLobyte::addText( char const * id, char const * text )
{ _texts.push_back({id, text, {}, 0}); }

void SearchFatLobyte::addPattern( char const * pattern )
{ _patterns.push_back(pattern); }

void SearchFatLobyte::clearPatterns( void )
{ _patterns.clear(); }


int SearchFatLobyte::seek( char const * filename )
{
    std::ofstream found_file(filename);
    if (!found_file)
    {
        perror("Failed to open output file");
        std::cerr<<"Aborting search\n";
        return 0;
    }


    #pragma omp paralell for
    for (auto& cur_text: _texts)
    {
        for (auto& cur_pat : _patterns)
        {
            char const* text_ptr = cur_text.text;

            while (*text_ptr != '\0')
            {
                char const* pattern_ptr = cur_pat;

                while(*pattern_ptr == *text_ptr)
                {
                    ++pattern_ptr;
                    ++text_ptr;
                }

                // if we reached this point and pattern_ptr points to '\0', we have a hit
                if (!*pattern_ptr)
                {
                    ++cur_text.hit_count; // increment hit counter
                    continue; //, we allready incremented the text_ptr
                }

                ++text_ptr;
            }
        }

        if (cur_text.hit_count)
            found_file<<cur_text.id<<'\t'<<cur_text.hit_count<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, TextInfo& el) { return acc + el.hit_count; }
    );
}



