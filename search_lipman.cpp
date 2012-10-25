// search_std-strstr.cpp

/*
 *   proggen_searchwords
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
#include <cassert>
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>


typedef std::map<char, std::vector<std::size_t>> indexmap_t;

struct TextInfo
{
    char const* id;
    indexmap_t indexmap;
    std::size_t hit_count;
};

struct PatternInfo {
    char const* pattern;
};


std::vector<std::pair<char, std::size_t>> reorder_pattern(
    const indexmap_t& indexmap,const char* pattern
)
{
    std::vector<std::pair<char, std::size_t>> reordered;

    // copy pattern verbatim
    std::size_t pos = 0;
    while(pattern[pos])
    {
        reordered.push_back(std::make_pair(pattern[pos], pos));
        ++pos;
    }

    // sort by lowest number of occurances in the index map
    std::sort(reordered.begin(), reordered.end(),
        [&indexmap](std::pair<char, std::size_t> lhs, std::pair<char, std::size_t> rhs)
        {
            // Get index vector for lhs and rhs
            auto lhs_index_vec_it = indexmap.find(lhs.first);
            auto rhs_index_vec_it = indexmap.find(rhs.first);

            // If the LHS is empty, it definitely goes before the RHS
            if (lhs_index_vec_it == indexmap.end()) return true;

            // If the RHS is empty, it definitely goes before the LHS
            if (rhs_index_vec_it == indexmap.end()) return false;


            // if none of those were empty, we check for the sizes of the index
            // vectors.
            return (lhs_index_vec_it->second.size() < rhs_index_vec_it->second.size());

        }
    );

    return reordered;
}


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
    _texts.emplace_back(new TextInfo{id, preparse(text), 0});
}

void SearchFatLobyte::addPattern( char const * pattern )
{
    _patterns.emplace_back(new PatternInfo{pattern});
}

void SearchFatLobyte::clearPatterns(void)
{ _patterns.clear(); }

#if 0
void print_indexmap(const indexmap_t& indexmap)
{
    for(const auto &p: indexmap)
    {
        std::cout<<p.first<<", "<<p.second.size()<<": ";
        for(auto &i: p.second) std::cout<<i<<' ';
        std::cout<<std::endl;
    }
}
#endif

void do_search(TextInfo& textinfo, const PatternInfo& patterninfo)
{
    // convenience pointer so we don't have to call the end() function each time
    auto indexmap_end = textinfo.indexmap.end();

    // create reordered pattern
    std::vector<std::pair<char, std::size_t>> pattern =
        reorder_pattern(textinfo.indexmap, patterninfo.pattern);

    // "first" pattern character
    const auto pattern_begin_it = pattern.begin();

    // find the character in the index map
    auto index_vec_it = textinfo.indexmap.find(pattern_begin_it->first);

    // if the currect character is unknown, we didn't find it
    if(index_vec_it == indexmap_end) return;
    assert(!index_vec_it->second.empty()); // this shouldn't be possible



    // iterate over ALL the indices available for the first character
    for(std::size_t first_index: index_vec_it->second)
    {
        std::size_t offset = first_index - pattern_begin_it->second;

        // create loop iterator starting at the second character
        auto pattern_ch_it = pattern_begin_it; ++pattern_ch_it;

        // iterate through all available characters in the pattern
        for (;pattern_ch_it != pattern.end(); ++pattern_ch_it)
        {
            // find character in the index map
            auto index_vec = textinfo.indexmap.find(pattern_ch_it->first);

            // if the currect character is unknown, we didn't find it
            if(index_vec == indexmap_end) goto NOTFOUND;

            // do a binary search through our available indices, check if the required
            // position is present in the indices for this letter
            auto it = std::lower_bound(
                index_vec->second.begin(), index_vec->second.end(),
                offset + pattern_ch_it->second
            );

            // if the position was not found, we definitely don't have a match.
            if (it == index_vec->second.end()) goto NOTFOUND;

            if (*it != offset + pattern_ch_it->second) goto NOTFOUND;
        }

        // If we finished this loop, we've found something
        ++textinfo.hit_count;

    NOTFOUND:
        ;
    }
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
        for (auto& cur_pat : _patterns)
            do_search(*cur_text, *cur_pat);


        if (cur_text->hit_count)
            found_file<<cur_text->id<<'\t'<<cur_text->hit_count<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, std::unique_ptr<TextInfo>& el) { return acc + el->hit_count; }
    );
}

SearchFatLobyte::SearchFatLobyte() {}
SearchFatLobyte::~SearchFatLobyte() {}
