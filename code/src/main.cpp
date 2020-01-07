#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>
#include <cmath>
#include "graph.h"

int main()
{
  std::random_device rd;  // obtain a random number from hardware
  std::mt19937 eng(rd()); // seed the generator

  // Clearing last results
  std::string data_path = "data/C-elegans-frontal.txt";
  std::string input_path = "result/C-elegans-frontal-link-list";

  Graph gB = Graph();
  read_graph_from_file(data_path, gB);
  gB.print_edges(input_path);
  //gB.print_edges();
  std::cout << "\n###### Processing out-degree ######\n" << std::endl;
  gB.calculate_community(input_path, true);
  gB.print_communities();
  gB.print_node_data(input_path, true);

  std::cout << "\n###### Processing in-degree ######\n" << std::endl;
  gB.calculate_community(input_path, false);
  gB.print_communities();
  gB.print_node_data(input_path, false);
  // double cc_x;
  // int cont_ER, cont_sw;


  //std::cout << gBA3.get_n_edges() << std::endl;
  //print_summary(gBA3);
  // gBA.print_edges();
}
