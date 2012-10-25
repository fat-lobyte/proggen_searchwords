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
    char const* text;
    indexmap_t indexmap;
    std::size_t hit_count;
};

struct PatternInfo {
    char const* pattern;
};


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
    _texts.emplace_back(new TextInfo{id, text, preparse(text), 0});
}

void SearchFatLobyte::addPattern( char const * pattern )
{
    _patterns.emplace_back(new PatternInfo{pattern});
}

void SearchFatLobyte::clearPatterns(void)
{ _patterns.clear(); }

inline bool expand_match(
    const char* text, std::size_t match_index,
    const char* pattern, std::size_t pattern_index
)
{
    constexpr std::size_t matchwidth = 1;

    const char* text_p;
    const char* pattern_p;

    // Search forward
    text_p = text + match_index + matchwidth;
    pattern_p = pattern + pattern_index + matchwidth;

    while(*pattern_p)
        if(*text_p++ != *pattern_p++) return false;

#if 1
    // search backwards
    text_p = text + match_index - 1;
    pattern_p = pattern + pattern_index - 1;

    while(pattern_index)
    {
        if(*text_p-- != *pattern_p--) return false;
        --pattern_index;
    }
#endif

    return true;
}


void do_search(TextInfo& textinfo, const PatternInfo& patterninfo)
{
    // convenience pointer so we don't have to call the end() function each time
    auto indexmap_end = textinfo.indexmap.end();


    // find the first character in the index map
    auto index_vec_it = textinfo.indexmap.find(patterninfo.pattern[0]);

    // if the currect character is unknown, we didn't find it
    if(index_vec_it == indexmap_end) return;
    assert(!index_vec_it->second.empty()); // this shouldn't be possible


    // iterate over ALL the indices available for the first character
    for(std::size_t offset: index_vec_it->second)
    {
        if(!expand_match(textinfo.text, offset, patterninfo.pattern, 0))
            goto NOTFOUND;


        // If we finished this loop, we've found something
        ++textinfo.hit_count;

    NOTFOUND:;
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
