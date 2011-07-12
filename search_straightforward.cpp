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


struct SearchPattern
{
    char const* _pattern;
    std::size_t _length;

    SearchPattern() = default;
    SearchPattern(char const* pattern)
        : _pattern(pattern), _length(strlen(pattern))
    {}
};

int SearchFatLobyte::seek( char const * filename )
{
    std::ofstream found_file(filename);
    if (!found_file)
    {
        perror("Failed to open output file");
        std::cerr<<"Aborting search\n";
        return 0;
    }

    std::size_t num_patterns = _patterns.size();
    SearchPattern* patterns = new SearchPattern[num_patterns];
    std::transform(
        _patterns.begin(), _patterns.end(), // range
        patterns, // to
        [](char const* p_str) { return SearchPattern(p_str); } //
    );


    for (auto& cur_text: _texts)
    {
        for (auto& cur_pat : _patterns)
        {
            char const* text_ptr = std::get<1>(cur_text);

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
                    ++std::get<2>(cur_text); // increment hit counter
                    continue; //, we allready incremented the text_ptr
                }

                ++text_ptr;
            }
        }

        if (std::get<2>(cur_text))
            found_file<<std::get<0>(cur_text)<<'\t'<<+std::get<2>(cur_text)
            <<'\n';

    }

    delete[] patterns;

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, decltype(*_texts.begin()) el) // binary_op
            { return acc + std::get<2>(el); }
    );
}



