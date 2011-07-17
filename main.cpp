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

    experiment.conductExperiment(30);

    ExperimentStatistics stats(experiment.getStatistics());
    stats.display();

    return 0;
}

