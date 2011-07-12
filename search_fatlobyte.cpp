// search_fatlobyte.cpp

#include "search_fatlobyte.hpp"

#include <cstring>
#include <iostream>
#include <fstream>

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
    int num_hits = 0;

    std::ofstream found_file(filename);
    if (!found_file)
    {
        perror("Failed to open output file");
        std::cerr<<"Aborting search\n";
        return 0;
    }

    for (auto text_it = _texts.begin(); text_it != _texts.end(); ++text_it)
    {
        bool was_found = false;

        for (auto pat_it = _patterns.begin();
            pat_it != _patterns.end(); ++pat_it)
        {
            const char *found_pointer = std::get<1>(*text_it);
            while (found_pointer = std::strstr(found_pointer, *pat_it))
            {
                ++found_pointer;
                ++num_hits;
                was_found = true;
            }
        }

        if(was_found) found_file<<std::get<0>(*text_it)<<'\n';
    }
    return num_hits;
}



