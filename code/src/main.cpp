#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include "graph.h"

int main(int argc, char **argv)
{
  std::vector<std::string> tokens;
  std::string delimiter = ".";
  std::string data_name = "";
  Graph gB = Graph();
  bool not_proper = true;
  if (argc > 1)
  {
    data_name = argv[1];
    std::cout << data_name << std::endl;
    gB.tokenize_line_plus(data_name, delimiter, tokens);
    std::cout << tokens[0] << std::endl;
    if (tokens.size() == 2)
    {
      if (tokens[1] == "edges")
      {
        std::cout << " Token 0 " << tokens[0] << std::endl;
        data_name = tokens[0];
        not_proper = false;
      }
    }
  }

  if (not_proper == true)
  {
    std::cout << "Not a proper filename" << data_name << std::endl;
    return 0;
  }
  else
  {
    std::cout << data_name << std::endl;
  }
  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator

  // Clearing last results

  std::string data_path = "data_test/" + data_name + ".edges";
  std::string input_path = "result/" + data_name + "/" + data_name + "-link-list";

  read_graph_from_file(data_path, gB);
  gB.print_edges(input_path);
  //gB.print_edges();
  std::cout << "\n###### Calculating Communities ######\n"
            << std::endl;
  gB.calculate_community_only(input_path, data_name);
  // gB.print_communities();
  gB.print_communities_sizes();
  gB.calculate_metrics(true);
  gB.print_node_data(input_path, true);
  //gB.print_communities_sizes();
  gB.calculate_metrics(false);
  gB.print_node_data(input_path, false);
  // gB.print_node_data(input_path, true);
  return 0;
  // std::cout << "\n###### Processing in-degree ######\n" << std::endl;
  // gB.calculate_community(input_path, false);
  // gB.print_communities();
  // gB.print_node_data(input_path, false);
  // double cc_x;
  // int cont_ER, cont_sw;

  //std::cout << gBA3.get_n_edges() << std::endl;
  //print_summary(gBA3);
  // gBA.print_edges();
}
