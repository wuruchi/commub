#include <iostream>
#include <fstream>
#include <vector>
#include "graph.h"

#define DEBUG true

int main()
{
  const std::string DATA_DIR = "data/";
  const std::string LANGUAGE_LIST_FILENAME = "language_list.txt";

  std::vector<std::string> languages;
  std::string line, langs_path = DATA_DIR + LANGUAGE_LIST_FILENAME, lang_path;

  std::ifstream f(langs_path);
  if (!f.is_open())
  {
    std::cerr << "Error: Unable to open file '" << langs_path << "'" << std::endl;
    return -1;
  }

  while (getline(f, line))
  {
    languages.push_back(line);
  }

  for (std::string lang : languages)
  {
    lang_path = DATA_DIR + lang + "_syntactic_dependency_network.txt";
    Graph g; // Create a new graph object
    //read_graph_from_file(lang_path, g);
    std::cout << lang << ",";
    print_summary(g);
  }
}
