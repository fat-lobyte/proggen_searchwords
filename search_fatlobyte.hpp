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

class SearchFatLobyte : public SearchBase
{
    std::vector<std::unique_ptr<TextInfo>> _texts;
    std::vector<std::unique_ptr<PatternInfo>> _patterns;

public:
    SearchFatLobyte();
    virtual ~SearchFatLobyte();

    void addText( char const * id, char const * text );

    void addPattern( char const * pattern );

    void clearPatterns( void );

    int seek( char const * filename );
};

