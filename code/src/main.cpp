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

  // clearing last results
  std::ofstream ofs;
  std::string path = "data/C-elegans-frontal.txt";
  std::string input_path = "result/C-elegans-frontal-link-list";
  Graph gB = Graph();
  read_graph_from_file(path, gB);
  gB.print_edges(input_path);
  gB.calculate_community(input_path);
  // double cc_x;
  // int cont_ER, cont_sw;
  // int tnumber = 10000;
  // Graph gBA1 = Graph(); // Create a new graph object
  // Graph gBA2 = Graph();
  // Graph gBA3 = Graph(tnumber);
  //std::cout << gBA1.get_n_vertices() << std::endl;
  // generate_BA_graph(tnumber, gBA1, eng);
  // generate_BA_random_attachment(tnumber, gBA2, eng);
  // generate_BA_no_growth(tnumber, gBA3, eng);
  // // writing to txt files
  // gBA1.print_edges("data/model1.txt");
  // gBA2.print_edges("data/model2.txt");
  // gBA3.print_edges("data/model3.txt");

  //std::cout << gBA3.get_n_edges() << std::endl;
  //print_summary(gBA3);
  // gBA.print_edges();
}
