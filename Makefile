
CXXFLAGS += -std=c++0x
OUTFILES = test_output1.txt test_output2.txt

all: searchwords

searchwords: main.o search_fatlobyte.o
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean clean_output

clean: clean_output
	rm -rf searchwords *.o

clean_output:
	rm -rf $(OUTFILES)

