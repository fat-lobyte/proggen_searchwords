// search_blockwise.cpp

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


template<typename BlockType>
struct Pattern
{
    // array with the first chunks of the pattern, one for each alignment
    BlockType first_chunks[sizeof(BlockType)];

    // array with the bitmasks of the first chunks, one for each alignment
    BlockType first_chunks_masks[sizeof(BlockType)];

/*
    // number of next chunks minus one,
    // since we allways have two chunks if strlen(pattern) > 1
    std::size_t num_next_chunks_minus1;

    // array with the next chunks
    BlockType* next_chunks[sizeof(BlockType)];
*/
};


template <typename BlockType>
Pattern<BlockType>* make_pattern_struct(char const* pattern)
{
    Pattern<BlockType>* pattern_struct = new Pattern<BlockType>;
    std::size_t len =  strlen(pattern);

    // for now we only care for the first block
    if (len  > sizeof(BlockType))
        len = sizeof(BlockType);

    // memset masks to zero
    memset(
        reinterpret_cast<void*>(pattern_struct->first_chunks_masks),
        0, sizeof(BlockType) * sizeof(BlockType)
    );

    for (
        std::size_t align_index = 0;
        align_index <  sizeof(BlockType);
        ++align_index)
    {
        char* byte_ptr = reinterpret_cast<char*>(
            &pattern_struct->first_chunks_masks[align_index]
        );

        for (
            std::size_t char_index = 0; // start at first index of pattern
                char_index + align_index < len;
            ++char_index)
        {
            *byte_ptr++ = 0xFF;
        }
    }

    // since we will AND with the masks anyway, we will not initialize the
    // pattern blocks but only set the bytes we need

    for (
        std::size_t align_index = 0;
        align_index <  sizeof(BlockType);
        ++align_index)
    {
        char* byte_ptr = reinterpret_cast<char*>(&pattern_struct->first_chunks[align_index]);

        for (
            std::size_t char_index = 0; // start at first index of pattern
            char_index + align_index < len;
            ++char_index)
        {
            *byte_ptr++ = pattern[char_index];
        }
    }

    return pattern_struct;
}

template<typename BlockType>
inline std::size_t block_has_first_match(
    BlockType block, const Pattern<BlockType>& pat
)
{
    BlockType sum_block;
    memset(&sum_block, 0, sizeof(BlockType)); // set to zero somehow

    // unroll me, end condition is known at compile time
    for (std::size_t i = 0; i < sizeof(BlockType); ++i)
    {
        // SUBtract, matching bits will be zero
        // NEGate, matching bits will be one
        // AND with bitmask to set only the bits we care about

        sum_block +=
            ~(block - pat.first_chunks[i])
            &  pat.first_chunks_masks[i]
        ;
    }

    // when we sum up the result and it is still zero, then no match was found

    return sum_block; // somehow convert to boolean type
}

/*
// Return value:
// 0 to sizeof(BlockType)-1: index of the first hit
// sizeof(BlockType) no match!!
template<typename BlockType>
inline std::size_t find_first_match_index(
    BlockType block, const Pattern<BlockType>& pat
)
{
    // unroll me, end condition is known at compile time
    for (std::size_t i = 0; i < sizeof(BlockType); ++i)
    {
        // SUBtract, matching bits will be zero
        // NEGate, matching bits will be one
        // AND with bitmask to set only the bits we care about

        if(~(block - pat.first_chunks[i]) &  pat.first_chunks_masks[i])
            break;
    }


    return i; // somehow convert to boolean type
}
*/

template<typename BlockType>
inline bool block_has_zero(BlockType block)
{
    // ok, this _will_ perform crappy
    return memchr(reinterpret_cast<void*>(&block), '\0', sizeof(BlockType)) != NULL;
}

typedef long long LargeBlock;

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
            Pattern<LargeBlock>* cur_pat_struct =
                make_pattern_struct<LargeBlock>(cur_pat);
            const LargeBlock* text_ptr =
                reinterpret_cast<const LargeBlock*>(cur_text.text);

            while (!block_has_zero(*text_ptr))
            {
                if(block_has_first_match(*text_ptr, *cur_pat_struct))
                {
                    ++cur_text.hit_count;

                    // ok, for now we just increment. In the proper algorithm we want to check
                    // for the next chunks in the search patterns
                }

                ++text_ptr;
            }

            // here we have to check if we have a match on the last block
            // and if the index of the first character is smaller than the index
            // of the null byte

            delete cur_pat_struct;
        }

        if (cur_text.hit_count)
            found_file<<cur_text.id<<'\t'<<cur_text.hit_count<<'\n';

    }

    return std::accumulate(_texts.begin(), _texts.end(), std::size_t(0), // begin, end, init
        [](std::size_t& acc, TextInfo& el) { return acc + el.hit_count; }
    );
}



