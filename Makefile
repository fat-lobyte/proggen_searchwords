
all: searchwords

searchwords: main.o search_fatlobyte.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf searchwords *.o
