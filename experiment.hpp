// experiment.hpp

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

#ifndef EXPERIMENT_HPP_INCLUDED_
#define EXPERIMENT_HPP_INCLUDED_

// C-Library
#include <cstring>
#include <cmath>

// C++-Library
#include <iostream>
#include <utility>
#include <vector>
#include <valarray>
#include <map>
#include <memory>
#include <numeric>

// POSIX-Library
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>



// for use with std::shared_ptr
struct MMapDeleter
{
    const std::size_t _filesize;

    MMapDeleter(std::size_t filesize)
        : _filesize(filesize)
    {}

    void operator () (void* mapping) const
    {
        if (mapping != nullptr && mapping != MAP_FAILED)
        {
            munmap(mapping, _filesize);
        }
    }
};

// for use with std::shared_ptr
template<typename T>
struct ArrayDeleter
{
    void operator () (T* d) const
    {
        delete [] d;
    }
};

std::shared_ptr<char> readFile(char const *filename);

struct ExperimentStatistics
{
    ExperimentStatistics(const std::vector<double>& data, const std::string& name_ = std::string())
        : name(name_)
    {
        N = data.size();
        mean = std::accumulate(data.begin(), data.end(), 0.0);
        mean /= N;

        std_dev = sqrt(
            std::accumulate(data.begin(), data.end(), 0.0,
                [this](double sum, double val)
                {return sum +(mean - val)*(mean - val);}
            )
        );
    }

    ExperimentStatistics(const ExperimentStatistics& other) = default;

    void display()
    {
        std::cout.precision(3);
        std::cout<<"Experiment"<<name<<" with N = "<<N<<":\n"<<"\tAverage: ";
        if (mean < 1e3)
            std::cout<<mean<<" microseconds.\n";
        else if(mean < 1e6)
            std::cout<<mean/1e3<<" milliseconds.\n";
        else
            std::cout<<mean/1e6<<" seconds.\n";

        std::cout<<"\tStandard deviation: ";
        if (std_dev < 1e3)
            std::cout<<std_dev<<" microseconds.\n";
        else if(std_dev < 1e6)
            std::cout<<std_dev/1e3<<" milliseconds.\n";
        else
            std::cout<<std_dev/1e6<<" seconds.\n";

        std::cout<<"\tRelat. standard deviation: "<<std_dev/mean*100<<"%\n";
    }

    std::string name;
    std::size_t N;
    double mean;
    double std_dev;
};


// This is the "ugly" but parsable version of displaying the statistical data
inline
std::ostream& operator << (std::ostream& os, const ExperimentStatistics& stats)
{
    return os<<
        "N:"<<stats.N<<';'<<
        "Mean:"<<stats.mean<<';'<<
        "StdDev:"<<stats.std_dev;
}


template <typename Candidate>
class Experiment
{
public:
    typedef std::pair<
        const char* /*text id*/,
        const char* /*text file name*/
    > input_t;

    typedef std::pair<
        const char* /*text id*/,
        std::shared_ptr<char>  /*text file data*/
    > text_t;

    typedef std::map<
        const char*, // file name
        std::vector<const char*> // search patterns
    > searches_t;

    Experiment(std::vector<input_t> input, searches_t searches);

    void conductExperiment(std::size_t N);

    ExperimentStatistics getInitStatistics()
    {
        return ExperimentStatistics(_durations_init, "Adding Texts");
    }

    ExperimentStatistics getSearchStatistics()
    {
        return ExperimentStatistics(_durations_search, "Searching Patterns");
    }
private:
    std::vector<text_t> _input_data;
    searches_t _searches;
    std::vector<double> _durations_init;
    std::vector<double> _durations_search;
};

template <typename Candidate>
Experiment<Candidate>::Experiment(
    std::vector<input_t> input, searches_t searches)
    : _searches(searches)
{
    // load files that were possible to open
    for (auto& i: input)
    {
        auto filedata = readFile(i.second);
        if (filedata) _input_data.push_back({i.first, filedata});
    }
}

template <typename Candidate>
void Experiment<Candidate>::conductExperiment(std::size_t N)
{
    Candidate* candidate = nullptr;
    struct timeval start_tv, interm_tv, end_tv;

    for (std::size_t current_run = 0; current_run < N; ++current_run)
    {
        candidate = new Candidate;

        //Zeitmessung startet
        gettimeofday(&start_tv, 0);

        // add texts
        for (auto& text : _input_data)
        {
            candidate->addText(text.first, text.second.get());
        }

        gettimeofday(&interm_tv, 0);

        // perform search operations
        for (auto& outfile : _searches)
        {
            for (const char* pattern : outfile.second)
                candidate->addPattern(pattern);

            // GO!
            candidate->seek(outfile.first);
        }


        // Zeitmessung endet
        gettimeofday(&end_tv, 0);

        // push back microseconds
        _durations_init.push_back(
            (interm_tv.tv_sec - start_tv.tv_sec) * 1e6 +
            (interm_tv.tv_usec - start_tv.tv_usec)
        );
        _durations_search.push_back(
            (end_tv.tv_sec - interm_tv.tv_sec) * 1e6 +
            (end_tv.tv_usec - interm_tv.tv_usec)
        );

        //Destruktor läuft
        delete candidate;
    }
}

std::shared_ptr<char> readFile(char const *filename)
{
    off_t filesize = 0;

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("Failed to open file for reading");
        return nullptr;
    }

    filesize = lseek(fd, 0, SEEK_END);

    std::shared_ptr<void> filebuf(
        mmap(0, filesize, PROT_READ, MAP_SHARED, fd, 0), MMapDeleter(filesize));
    if (filebuf.get() == MAP_FAILED)
    {
        perror("Failed to map file");
        close(fd);
        return nullptr;
    }

    std::shared_ptr<char> textbuf(new char[filesize + 1], ArrayDeleter<char>());
    if (textbuf)
    {
        memcpy(textbuf.get(), filebuf.get(), filesize);
        textbuf.get()[filesize] = '\0';
    }

    close(fd);
    return textbuf;
}



#endif
