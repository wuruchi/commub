#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <random>
#include <utility>
#include <map>

class Graph
{
private:
  std::vector<std::vector<int>> adj_list;
  std::vector<std::vector<int>> adj_list_in;
  std::vector<std::vector<int>> inner_adj_list;
  // Label handling
  std::vector<std::string> label_index; // Allows to access word values by index
  std::unordered_map<std::string, int> label_dict;
  std::unordered_map<int, int> id_to_community;
  std::map<int, std::vector<int>> adjacency_community;
  std::map<int, std::vector<double>> participation_vectors;
  std::map<int, double> community_to_density;
  std::map<int, double> participation_index;
  std::map<int, double> dispersion_index;
  std::map<int, double> global_hubness;
  std::map<int, double> local_hubness;
  size_t n_edges;
  bool DEBUG;
  int find_neighbour_ix(int id, int n_id);
  double c_i(int id, bool pruning, double *cumm_c_i);

public:
  Graph(); // Default constructor
  // We only use default STL structures, do we need a copy constructor?
  void print_edges();
  void print_communities();
  void print_communities_sizes();
  void print_node_data(const std::string &output_path, bool is_out_degree = true);
  void print_graphml_format();
  void print_edges(const std::string &txtfile);
  void print_time_track(const std::string &txtfile, int node, int &time);
  int add_vertex(const std::string &label);
  int add_edge(int v1, int v2);
  bool is_neighbour(int v1, int v2);
  bool is_neighbour_in(int v1, int v2);
  void switch_edges(std::pair<int, int> e1, std::pair<int, int> e2);
  double closeness_centrality(bool pruning = false, bool sorting = false, bool montecarlo = false, int M = 0, double x = -1.0);
  void tokenize_line_plus(std::string &line, std::string &delimiter, std::vector<std::string> &tokens);
  //void tokenize_line(std::string &line, std::string &delimiter, std::vector<std::string> &tokens);
  // Getters & Setters
  std::size_t get_n_vertices() const;
  std::size_t get_n_edges() const;
  const std::vector<int> &get_neighbours(int id) const;
  int get_degree(int id) const;
  void get_degree_seq(std::vector<int> &deg_seq);
  void calculate_metrics(bool is_out_degree = true);
  void calculate_community_only(std::string &input_path, bool is_out_degree = true);  
  double density_of_community(std::vector<int> &nodes_in_community, std::vector<std::vector<int>> &adj_list, int &community_id);
};
int read_graph_from_file(const std::string &path, Graph &g);
void print_summary(const Graph &g);
int degree_vector_in_community(std::vector<int> &degree, std::vector<int> &community);
double standard_deviation(std::vector<double> &v);
std::vector<double> only_non_zero(std::vector<double> &original);
double density(int &n_nodes, int &n_edges);
void tokenize_line(std::string &line, std::string &delimiter, std::vector<std::string> &tokens);


#endif
