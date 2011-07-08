
#ifndef SEARCH_BASE_HPP_INCLUDED
#define SEARCH_BASE_HPP_INCLUDED

class SearchBase
{
  public:
     virtual void addText( char const * id, char const * text ) = 0;
     virtual void addPattern( char const * pattern ) = 0;
     virtual void clearPatterns( void ) = 0;
     virtual int seek( char const * filename ) = 0;

     virtual ~SearchBase() {}
};

#endif // ifndef SEARCH_BASE_HPP_INCLUDED
