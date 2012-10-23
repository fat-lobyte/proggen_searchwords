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
#include <algorithm>


indexmap_t preparse(const char* text)
{
    indexmap_t indexmap;

    std::size_t pos=0;
    while(text[pos])
    {
        indexmap[text[pos]].push_back(pos);
        ++pos;
    }

    return indexmap;
}

void SearchFatLobyte::addText( char const * id, char const * text )
{
    _texts.push_back({id, nullptr, preparse(text), 0});
}

void SearchFatLobyte::addPattern( char const * pattern )
{ _patterns.push_back(pattern); }

void SearchFatLobyte::clearPatterns( void )
{ _patterns.clear(); }


void print_indexmap(const indexmap_t& indexmap)
{
    for(const auto &p: indexmap)
    {
        std::cout<<p.first<<", "<<p.second.size()<<": ";
        for(auto &i: p.second) std::cout<<i<<' ';
        std::cout<<std::endl;
    }
}


bool subsearch(
    indexmap_t& indexmap,
    std::size_t offset,
    std::size_t pat_position,
    const char* pattern
)
{
    // std::cout<<offset<<':'<<pat_position<<'\n';
    char next_char;
NEXT_POS:
    // get next character
    next_char = pattern[pat_position];

    // if this is the end, we were successful
    if (next_char == '\0') return true;

    // if the currect character is unknown, return false
    if(!indexmap.count(next_char)) return false;

    // do a binary search through our available indices, find the index greater
    // than the offset
    auto it = std::upper_bound(
        indexmap[next_char].begin(), indexmap[next_char].end(), offset + pat_position
    );

    if (it != indexmap[next_char].end())
    {
        ++pat_position; // go to the next position in the pattern
        goto NEXT_POS;
    }
#if 0
    // loop through indices for this character
    for (std::size_t char_index: indexmap[next_char])
    {
        // only bother to continue search if the offset is still valid
        if(char_index - pat_position == offset)
        {

            //return subsearch(indexmap, offset, pat_position, pattern);
        }
    }
#endif

    // if this loop finished, we ran out of viable indices at this level
    return false;
}

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
        //std::cout<<" ###### Index for "<<cur_text.id<<" ###### ";
        //print_indexmap(cur_text.indexmap);

        for (auto& cur_pat : _patterns)
        {
            //std::cout<<" ------- pattern: \""<<cur_pat<<"\" -------\n";

            // if the first character is not part of the text,
            // we didn't find anything!
            if (!cur_text.indexmap.count(cur_pat[0])) continue;

            // otherwise, check all indices for the first character
            for (std::size_t offset: cur_text.indexmap[cur_pat[0]])
            {
                // perform the subsearch, report positives
                if (subsearch(cur_text.indexmap, offset, 1, cur_pat))
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



