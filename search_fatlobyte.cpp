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
#include <tuple>
#include <unordered_map>

#ifndef INDEX_TYPE
    typedef std::uint32_t index_type;
#else
    typedef INDEX_TYPE index_type;
#endif

#ifndef MAX_BLOCKSIZE
#   define MAX_BLOCKSIZE 4
#endif


template <unsigned> struct blocktype {};

template <> struct blocktype<1> { typedef char type; };
template <> struct blocktype<2> { typedef std::int16_t type; };
template <> struct blocktype<4> { typedef std::int32_t type; };
template <> struct blocktype<8> { typedef std::int64_t type; };

template <unsigned Blocksize>
using indexmap_t = std::unordered_map<typename blocktype<Blocksize>::type, std::vector<index_type>> ;

typedef std::tuple<
        indexmap_t<1>[1],
        indexmap_t<2>[2],
        indexmap_t<4>[4],
        indexmap_t<8>[8]
> indexmap_tuple_type;

struct TextInfo
{
    char const* id;
    char const* text;
    indexmap_tuple_type indexmaps;
    index_type hit_count;
};

struct PatternInfo {
    char const* pattern;
    index_type length;
};


constexpr unsigned power_of_two(unsigned x) // sorta log_2()
{
    return
        (x) ? // if (x != 0) {
            ((x-1) ? // if (x-1 != 0) {
                1+power_of_two(x >> 1)
            : // } else {
            0) // }
        : // } else {
            0
        // }
        ;
}


template <unsigned Blocksize>
void create_indexmaps(indexmap_tuple_type& indexmap_tuple, const char* text)
{
    constexpr unsigned indexmap_index = power_of_two(Blocksize);

    index_type pos = 0;

    while (*(text+pos+Blocksize))
    {
        std::get<indexmap_index>(indexmap_tuple)[pos % Blocksize][
            *reinterpret_cast<const typename blocktype<Blocksize>::type*>(text+pos)
        ].push_back(pos);
        ++pos;
    }

    create_indexmaps<Blocksize/2>(indexmap_tuple, text);
}


template <>
void create_indexmaps<0>(indexmap_tuple_type&, const char*) {}

void SearchFatLobyte::addText( char const * id, char const * text )
{
    std::unique_ptr<TextInfo> textinfo{new TextInfo{
        id, text,
        {},
        0
    }};

    create_indexmaps<MAX_BLOCKSIZE>(textinfo->indexmaps, text);

    _texts.emplace_back(std::move(textinfo));
}

void SearchFatLobyte::addPattern( char const * pattern )
{
    _patterns.emplace_back(
        new PatternInfo{pattern, static_cast<index_type>(strlen(pattern))}
    );
}

void SearchFatLobyte::clearPatterns(void)
{ _patterns.clear(); }

inline bool expand_match(
    const char* text, index_type match_index,
    const char* pattern, index_type pattern_index,
    index_type matchwidth
)
{

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


template <unsigned Blocksize>
void do_search(TextInfo& textinfo, const PatternInfo& patterninfo)
{
    constexpr unsigned indexmap_index = power_of_two(Blocksize);

    // If the Pattern is smaller than one block, we need to search with
    // smaller blocks
    if (patterninfo.length < Blocksize)
    {
        do_search<Blocksize/2>(textinfo, patterninfo);
        return;
    }

    // Get a block that is most characteristic for the pattern
    typename blocktype<Blocksize>::type char_block =
        *reinterpret_cast<const typename blocktype<Blocksize>::type*>(
            patterninfo.pattern
        );

    // iterate over all frames
    for (index_type cur_frame = 0; cur_frame < Blocksize; ++cur_frame)
    {
        // find the first character in the index map
        auto index_vec_it = std::get<indexmap_index>(textinfo.indexmaps)[cur_frame]
            .find(char_block);

        // if the currect character is unknown, we didn't find it
        if(index_vec_it ==
            std::get<indexmap_index>(textinfo.indexmaps)[cur_frame].end())
            continue;
        assert(!index_vec_it->second.empty()); // this shouldn't be possible


        // iterate over ALL the indices available for the first character
        for(index_type offset: index_vec_it->second)
        {
            if(expand_match(textinfo.text, offset, patterninfo.pattern, 0, Blocksize))
                ++textinfo.hit_count;
        }

    }

}

// specialize for leaf node
template <>
void do_search<0>(TextInfo& textinfo, const PatternInfo& patterninfo) {}


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
            do_search<MAX_BLOCKSIZE>(*cur_text, *cur_pat);


        if (cur_text->hit_count)
            found_file<<cur_text->id<<'\t'<<cur_text->hit_count<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), index_type(0), // begin, end, init
        [](index_type& acc, std::unique_ptr<TextInfo>& el) { return acc + el->hit_count; }
    );
}

SearchFatLobyte::SearchFatLobyte() {}
SearchFatLobyte::~SearchFatLobyte() {}
