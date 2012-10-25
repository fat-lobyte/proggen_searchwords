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
#include <cstdint>
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>


typedef std::uint32_t index_type;

template <std::size_t>
struct blocktype
{
};

template <> struct blocktype<1> { typedef char type; };
template <> struct blocktype<2> { typedef std::int16_t type; };
template <> struct blocktype<4> { typedef std::int32_t type; };
template <> struct blocktype<8> { typedef std::int64_t type; };

template <std::size_t Blocksize>
using indexmap_t = std::map<typename blocktype<Blocksize>::type, std::vector<index_type>> ;


struct TextInfo
{
    char const* id;
    char const* text;
    indexmap_t<1> indexmap1[1];
    indexmap_t<2> indexmap2[2];
    indexmap_t<4> indexmap4[4];
    index_type hit_count;
};

struct PatternInfo {
    char const* pattern;
};


template <std::size_t Blocksize>
void create_indexmap(indexmap_t<Blocksize> indexmap_array[], const char* text)
{
    index_type pos = 0;

    while (*(text+pos+Blocksize))
    {
        indexmap_array[pos % Blocksize][
            *reinterpret_cast<const typename blocktype<Blocksize>::type*>(text+pos)
        ].push_back(pos);
        ++pos;
    }
}


void SearchFatLobyte::addText( char const * id, char const * text )
{
    std::unique_ptr<TextInfo> textinfo{new TextInfo{
        id, text,
        {{}},
        {{}, {}},
        {{}, {}, {}, {}},
        0
    }};


    create_indexmap<1>(textinfo->indexmap1, text);
    create_indexmap<2>(textinfo->indexmap2, text);
    create_indexmap<4>(textinfo->indexmap4, text);

    _texts.emplace_back(std::move(textinfo));
}

void SearchFatLobyte::addPattern( char const * pattern )
{
    _patterns.emplace_back(new PatternInfo{pattern});
}

void SearchFatLobyte::clearPatterns(void)
{ _patterns.clear(); }

inline bool expand_match(
    const char* text, index_type match_index,
    const char* pattern, index_type pattern_index
)
{
    constexpr index_type matchwidth = 1;

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
    auto indexmap_end = textinfo.indexmap1[0].end();


    // find the first character in the index map
    auto index_vec_it = textinfo.indexmap1[0].find(patterninfo.pattern[0]);

    // if the currect character is unknown, we didn't find it
    if(index_vec_it == indexmap_end) return;
    assert(!index_vec_it->second.empty()); // this shouldn't be possible


    // iterate over ALL the indices available for the first character
    for(index_type offset: index_vec_it->second)
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

    return std::accumulate(_texts.begin(), _texts.end(), index_type(0), // begin, end, init
        [](index_type& acc, std::unique_ptr<TextInfo>& el) { return acc + el->hit_count; }
    );
}

SearchFatLobyte::SearchFatLobyte() {}
SearchFatLobyte::~SearchFatLobyte() {}
