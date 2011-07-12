#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>


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
        close(fd);
        return NULL;
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
    struct timeval start_tv, end_tv;

    // Texte werden geladen
    char const *textfilenames[] = {
        "txt/en-macbeth.txt",
        "txt/ger-dieverwandlung.txt",
        "txt/ger-faust.txt",
        "txt/ger-iphigenie.txt",
        "txt/ger-logik.txt"
    };

    // array with pointers to the texts, initialize them with NULL
    char const *texts[5];
    memset(reinterpret_cast<void*>(texts), 0, 5);

    for (unsigned i = 0; i < 5; ++i)
    {
        texts[i] = getFile(textfilenames[i]);
        if (!texts[i]) return EXIT_FAILURE;
    }

  SearchFatLobyte search;

  //Zeitmessung startet
  gettimeofday(&start_tv, 0);

   search.addText( "Shakespeare - Macbeth", texts[0] );
   search.addText( "Kafka - Die Verwandlung", texts[1] );
   search.addText( "Goethe - Faust", texts[2] );
   search.addText( "Goethe - Iphigenie auf Tauris", texts[3] );
   search.addText( "Hegel - Wissenschaft der Logik", texts[4] );

   search.addPattern( "Hallo" );
   search.addPattern( "Welt" );
   search.seek( "test_output1.txt" );
   search.clearPatterns();
   search.addPattern( "Hello" );
   search.addPattern( "World" );
   search.seek( "test_output2.txt" );

   // Zeitmessung endet
  gettimeofday(&end_tv, 0);

    double duration = (end_tv.tv_sec - start_tv.tv_sec)*1e6 +
        (end_tv.tv_usec - start_tv.tv_usec);

    std::cout<<"Search operation took "<<duration<<" microseconds.\n";

    return EXIT_SUCCESS;
   //Destruktor läuft
}

