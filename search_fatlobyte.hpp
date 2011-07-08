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

#include <vector>
#include <utility>
#include "searchbase.hpp"

class SearchFatLobyte : public SearchBase
{
    std::vector<std::pair<char const *,char const *> > _texts;
    std::vector<char const *> _patterns;

public:
    void addText( char const * id, char const * text )
    { _texts.push_back(std::make_pair(id,text)); }

    void addPattern( char const * pattern )
    { _patterns.push_back(pattern); }

    void clearPatterns( void )
    { _patterns.clear(); }

    int seek( char const * filename );


    virtual ~SearchFatLobyte() {}
};

