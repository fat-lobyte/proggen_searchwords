#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>


#include "search_fatlobyte.hpp"

char const * getFile(char const *filename)
{
    off_t filesize = 0;
    void *filebuf = NULL;
    char* textbuf = NULL;

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open file for reading");
        return NULL;
    }

    filesize = lseek(fd, 0, SEEK_END);

    filebuf = mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0);
    if (filebuf == MAP_FAILED)
    {
        perror("Failed to map file");
        return NULL;
        close(fd);
    }

    textbuf = (char*) malloc(filesize + 1);
    if (textbuf != NULL)
    {
        memcpy(textbuf, filebuf, filesize);
        textbuf[filesize] = '\0';
    }
    else
        perror("Failed to allocate memory");

    munmap (filebuf, filesize);
    close(fd);

    return textbuf;
}


int main(int argc, char **argv)
{
    char const *textfilenames[] = {
        "main.cpp",
        "searchbase.hpp",
        "search_fatlobyte.cpp",
        "search_fatlobyte.hpp",
        "Makefile"
    };

    char const *texts[5];
    memset(reinterpret_cast<void*>(texts), 0, 5);

    for (unsigned i = 0; i < 5; ++i)
    {
        texts[i] = getFile(textfilenames[i]);
        if (!texts[i])
            return EXIT_FAILURE;
    }

  // Texte werden geladen
  SearchFatLobyte search;

  //Zeitmessung startet
   search.addText( "text 1", texts[1] );
   search.addText( "text 2", texts[2] );
   search.addText( "text 3", texts[3] );
   search.addText( "text 4", texts[4] );
   search.addText( "text 5", texts[5] );

   search.addPattern( "Hallo" );
   search.addPattern( "Welt" );
   search.seek( "xin/test1.txt" );
   search.clearPatterns();
   search.addPattern( "Hello" );
   search.addPattern( "World" );
   search.seek( "xin/test2.txt" );

   // Zeitmessung endet


    return EXIT_SUCCESS;
   //Destruktor läuft
}

