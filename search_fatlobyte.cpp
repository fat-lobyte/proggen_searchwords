// search_fatlobyte.cpp

#include "search_fatlobyte.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <numeric>

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
            const char *found_pointer = std::get<1>(cur_text);
            while (found_pointer = std::strstr(found_pointer, cur_pat))
            {
                ++found_pointer; // increment so we aren't stuck on first hit
                ++std::get<2>(cur_text); // increment hit counter
            }
        }

        if (std::get<2>(cur_text)) found_file<<std::get<0>(cur_text)<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, decltype(*_texts.begin()) el) // binary_op
            { return acc + std::get<2>(el); }
    );
}



