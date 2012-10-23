#include "experiment.hpp"
#include "search_fatlobyte.hpp"

int main(int argc, char **argv)
{
    std::size_t N = 30;

    for(std::size_t i=1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-n"))
        {
            if (!(i+1 < argc))
            {
                std::cerr<<"Missing argument for \"-n\"\n";
                exit(EXIT_FAILURE);
            }
            else if (!(N = atoi(argv[i+1])))
            {
                std::cerr<<"Invalid argument for \"-n\"\n";
                exit(EXIT_FAILURE);
            }
        }
    }

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

    experiment.conductExperiment(N);

    ExperimentStatistics(experiment._durations_init, "Adding Texts").display();


    ExperimentStatistics(experiment._durations_search, "Adding Texts").display();


    return 0;
}

