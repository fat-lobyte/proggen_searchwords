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

#include "experiment.hpp"

#include "search_fatlobyte.hpp"



int main(int argc, char **argv)
{
    Experiment<SearchFatLobyte> experiment(
        {// std::vector<std::pair<const char* /*id*/, const char* /*filename*/>>
            {"Shakespeare - Macbeth", "txt/en-macbeth.txt"},
            {"Kafka - Die Verwandlung", "txt/ger-dieverwandlung.txt"},
            {"Goethe - Faust", "txt/ger-faust.txt"},
            {"Goethe - Iphigenie auf Tauris", "txt/ger-iphigenie.txt"},
            {"Hegel - Wissenschaft der Logik", "txt/ger-logik.txt"}
        },
        {// std::map<const char* /*outfilename*/, std::vector<const char* /*pattern*/>>
            {"test_output1.txt", {"Hallo", "Welt" } },
            {"test_output2.txt", {"Hello", "World"} }
        }
    );

    experiment.conductExperiment(1);
#if 0
    std::cout.precision(3);
    if (duration < 1e3)
        std::cout<<"Search operation took "<<duration<<" microseconds.\n";
    else if(duration < 1e6)
        std::cout<<"Search operation took "<<duration/1e3<<" milliseconds.\n";
    else
        std::cout<<"Search operation took "<<duration/1e6<<" seconds.\n";
#endif

    return EXIT_SUCCESS;
   //Destruktor läuft
}

