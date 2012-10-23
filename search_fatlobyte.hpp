// search_fatlobyte.cpp

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

#include <memory>
#include <vector>
#include <map>
#include "searchbase.hpp"

struct TextInfo;
struct PatternInfo;

typedef std::map<char, std::vector<std::size_t>> indexmap_t;

struct TextInfo
{
    char const* id;
    char const* text;
    indexmap_t indexmap;
    std::size_t hit_count;
};


class SearchFatLobyte : public SearchBase
{
    std::vector<TextInfo> _texts;
    std::vector<char const *> _patterns;

public:
    void addText( char const * id, char const * text );

    void addPattern( char const * pattern );

    void clearPatterns( void );

    int seek( char const * filename );

    virtual ~SearchFatLobyte() {}
};

