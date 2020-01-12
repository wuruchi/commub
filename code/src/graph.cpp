
#include "graph.h"
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <random>
#include <numeric>

// Use namespace std here so it is not included in the main code
using namespace std;

std::knuth_b rand_engine; // replace knuth_b with one of the engines listed below

bool random_bool_with_prob(double prob) // probability between 0.0 and 1.0
{
  std::bernoulli_distribution d(prob);
  return d(rand_engine);
}

// Adds a vertex to the graph indexed to the end of the list
// Returns: index
int Graph::add_vertex(const string &label)
{
  //cout << label_dict.size() << endl;
  if (label_dict.find(label) == label_dict.end())
  {
    int id = label_dict.size();
    label_dict[label] = id;
    label_index.push_back(label);
    adj_list.push_back(vector<int>());
    adj_list_in.push_back(vector<int>());
    //cout << "determined " << id << endl;
    return id;
  }
  else
  {
    int id = label_dict[label];
    if (DEBUG)
      cerr << "Vertex with label '" << label << "' already exists with id " << id << endl;
    return id;
  }
}

// This assumes that the target vertices exist and it references vertex ids directly
// The graph is undirected so we need to add it in both directions
int Graph::add_edge(int v1, int v2)
{
  //cout << v1 << " ~ " << v2 << endl;
  if (is_neighbour(v1, v2))
    return -1;
  adj_list[v1].push_back(v2);
  adj_list_in[v2].push_back(v1);
  //adj_list[v2].push_back(v1);
  ++n_edges;
  return 0;
}

Graph::Graph()
{
  n_edges = 0;
}

// Auxiliary function to tokenize a string to facilitate parsing.
// WARNING: Replaces all contents inside the vector 'tokens'
// size_t tokenize_line(const string &line, vector<size_t> &token_pos, const string &sep = " ")
// {
//   token_pos.clear(); // Clear the vector before pushing in the new tokens
//   int counter = 0;
//   size_t tail = 0, head = line.find(sep);
//   while (counter <= 1)//while (tail < line.size())
//   {
//     if (head > tail)
//     {
//       token_pos.push_back(tail);
//       token_pos.push_back(head - tail);
//     }
//     tail = head + 1;
//     head = line.find(sep, tail);
//     if (head > line.size())
//       head = line.size();
//     counter += 1;
//   }
//   return token_pos.size() / 2;
// }

// This assumes that the first line in the file indicates the number of vertices
// and edges in the graph described by the file. Those numbers are used as a
// reference and not actually included in the graph.
int read_graph_from_file(const string &path, Graph &g)
{
  int lines_read = 1, vx_read, exp_v, exp_e, vx[2], loops = 0, multiedges = 0;
  string line, w, delimiter = "\t";
  std::vector<std::string> tokens;
  ifstream f(path); // Create a file stream for the specified path
  if (!f.is_open())
  {
    cerr << "Error: Unable to open file '" << path << "'" << endl;
    return -1;
  }
  cerr << "***********************************************" << endl;
  cerr << "Reading graph from file '" << path << "'..." << endl;
  // getline(f, line);
  // if (tokenize_line(line, token_pos) != 2)
  // {
  //   cerr << "Error: Malformed first row. Aborting parse" << endl;
  //   return -1;
  // }
  // Parse the numbers to integers and store them to check later
  // exp_v = stoi(line.substr(token_pos[0], token_pos[1]));
  // exp_e = stoi(line.substr(token_pos[2], token_pos[3]));

  cerr << "-----------------------------------------------" << endl;
  // cerr << " - Expected number of vertices: " << exp_v << endl;
  // cerr << " - Expected number of edges: " << exp_e << endl;

  while (getline(f, line))
  {
    ++lines_read;
    tokenize_line(line, delimiter, tokens);
    cout << tokens[0] << " " << tokens[1] << endl;
    if (tokens.size() < 2)
    {
      cerr << " - Bad word token found at file row " << lines_read << endl;
    }
    else
    {
      vx[0] = g.add_vertex(tokens[0]);
      vx[1] = g.add_vertex(tokens[1]);
    }
  
    //cout << vx[0] << " # " << vx[1] << endl;
    //cout << line << endl;
    if (vx[0] == vx[1])
    {
      ++loops;
    }
    else if (g.add_edge(vx[0], vx[1]))
      ++multiedges;
  }
  cerr << "-----------------------------------------------" << endl;
  cerr << "Graph succesfully read!" << endl;
  cerr << " - Vertices succesfully read: " << g.get_n_vertices() << endl;
  cerr << " - Edges succesfully read: " << g.get_n_edges() << endl;
  cerr << " - Loops omitted: " << loops << endl;
  cerr << " - Duplicate edges omitted: " << multiedges << endl;
  cerr << "***********************************************" << endl
       << endl;
  return 0;
}

size_t Graph::get_n_vertices() const { return adj_list.size(); }
size_t Graph::get_n_edges() const { return n_edges; }
const std::vector<int> &Graph::get_neighbours(int id) const { return adj_list[id - 1]; }
int Graph::get_degree(int id) const { return adj_list[id].size(); }

bool Graph::is_neighbour(int v1, int v2)
{
  for (int vx : adj_list[v1])
    if (vx == v2)
      return true;
  return false;
}

bool Graph::is_neighbour_in(int v1, int v2)
{
  for (int vx : adj_list_in[v2])
    if (vx == v1)
      return true;
  return false;
}

// This returns a 0-based index
int Graph::find_neighbour_ix(int id, int n_id)
{
  for (size_t i = 0; i < adj_list[id].size(); ++i)
    if (adj_list[id][i] == n_id)
      return i;
  return -1;
}

int choose_edge_uar(const Graph &g, pair<int, int> &e, std::mt19937 &eng)
{
  uniform_int_distribution<int> dist_vx(1, g.get_n_vertices()), dist_neighbours;
  e.first = dist_vx(eng);
  if (!g.get_degree(e.first))
    return -1; // Discard isolated nodes
  dist_neighbours = uniform_int_distribution<int>(0, g.get_degree(e.first) - 1);
  e.second = dist_neighbours(eng); // Second value of the pair is the index on the neighbour list
  return 0;
}

void print_summary(const Graph &g)
{
  int N = g.get_n_vertices(), E = g.get_n_edges();
  cout << N << ","
       << E << ","
       << 2 * E / float(N) << ","
       << 2 * E / float(N * (N - 1))
       << endl;
}

void Graph::print_edges()
{
  cout << "Printing out-degree format." << endl;
  for (int i = 0; i < adj_list.size(); ++i)
  {
    cout << label_index[i] << " : ";
    for (int j = 0; j < adj_list[i].size(); ++j)
    {
      cout << label_index[adj_list[i][j]] << " ";
    }
    cout << endl;
  }

  cout << "\n\nPrinting in-degree format." << endl;
  for (int i = 0; i < adj_list_in.size(); ++i)
  {
    cout << label_index[i] << " : ";
    for (int j = 0; j < adj_list_in[i].size(); ++j)
    {
      cout << label_index[adj_list_in[i][j]] << " ";
    }
    cout << endl;
  }

}
void Graph::print_communities()
{
  cout << " Adjacency Community size: " << adjacency_community.size() << endl;
  std::vector<int> comm_content;
  for(auto& x: adjacency_community){
    std::cout << x.first;
    comm_content.clear();
    comm_content = x.second;
    cout << " : size " << comm_content.size() << " content : ";
    for(int i = 0; i < comm_content.size(); ++i){
      cout << comm_content[i] << ",";
    }
    cout << endl;
  }
  // for (int i = 1; i <= adjacency_community.size(); ++i)
  // {
  //   cout << i << " : ";
  //   for (int j = 0; j < adjacency_community[i].size(); ++j)
  //   {
  //     cout << adjacency_community[i][j] << " ";
  //   }
  //   cout << endl;
  // }
}

void Graph::print_node_data(const std::string &output_path, bool is_out_degree)
{
  std::string path = output_path + "_out.graphml";
  inner_adj_list = adj_list;
  if (is_out_degree == false){
    path = output_path + "_in.graphml";
    inner_adj_list = adj_list_in;
  }
  ofstream myfile(path);
  if (myfile.is_open()){
    myfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    myfile << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\">\n";
    // myfile << "\txmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
    // myfile << "xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns\n";
    // myfile << "http://graphml.graphdrawing.org/xmlns/1.0/graphml.xsd\">\n";
    myfile << "\t<key id=\"p_i\" for=\"node\" attr.name=\"participation index\" attr.type=\"double\">\n";
    myfile << "\t\t<default>0.0</default>\n";
    myfile << "\t</key>\n";
    myfile << "\t<key id=\"d_i\" for=\"node\" attr.name=\"dispersion index\" attr.type=\"double\">\n";
    myfile << "\t\t<default>0.0</default>\n";
    myfile << "\t</key>\n";
    myfile << "\t<key id=\"g_h\" for=\"node\" attr.name=\"global hubness\" attr.type=\"double\">\n";
    myfile << "\t\t<default>0.0</default>\n";
    myfile << "\t</key>\n";
    myfile << "\t<key id=\"l_h\" for=\"node\" attr.name=\"local hubness\" attr.type=\"double\">\n";
    myfile << "\t\t<default>0.0</default>\n";
    myfile << "\t</key>\n";
    myfile << "\t<key id=\"comm\" for=\"node\" attr.name=\"community\" attr.type=\"int\">\n";
    myfile << "\t\t<default>1</default>\n";
    myfile << "\t</key>\n";
    myfile << "<graph id=\"G\" edgedefault=\"directed\">\n";
    for (int i = 0; i < inner_adj_list.size(); ++i)
    {
      myfile << "\t<node id=\"" << label_index[i] << "\">\n";
      myfile << "\t\t<data key=\"p_i\">" << participation_index[i] << "</data>\n";
      myfile << "\t\t<data key=\"d_i\">" << dispersion_index[i] << "</data>\n";
      myfile << "\t\t<data key=\"g_h\">" << global_hubness[i] << "</data>\n";
      myfile << "\t\t<data key=\"l_h\">" << local_hubness[i] << "</data>\n";
      myfile << "\t\t<data key=\"comm\">" << id_to_community[i] << "</data>\n";
      myfile << "\t</node>\n";
      // cout << label_index[i] << '\t' << participation_index[i] << '\t' << dispersion_index[i] << '\t' << global_hubness[i] << '\t' << local_hubness[i] << "\t [";
      // for(int j = 0; j < participation_vectors[i].size(); ++j){
      //   cout << ' ' << participation_vectors[i][j];
      // }
      // cout << " ]" << endl;
    }
    for (int i = 0; i < inner_adj_list.size(); ++i){
      for (int j = 0; j < inner_adj_list[i].size(); ++j){
        myfile << "<edge source=\"" << label_index[i] << "\" target=\"" << label_index[inner_adj_list[i][j]] << "\"/>\n";
      }
    }
    myfile << "</graph>\n</graphml>";
    myfile.close();
  }
}

void Graph::print_edges(const std::string &txtfile)
{
  const std::string out_degree_path = txtfile + "_out.txt";
  const std::string in_degree_path = txtfile + "_in.txt";
  ofstream myfile(out_degree_path);
  if (myfile.is_open())
  {
    for (int i = 0; i < adj_list.size(); ++i)
    {
      //cout << label_index[i] << " : ";
      for (int j = 0; j < adj_list[i].size(); ++j)
      {
        myfile << i + 1 << " " << adj_list[i][j] + 1 << " "
               << "1"
               << "\n";
      }
    }
    myfile.close();
    cout << "Graph written in format link-list in " << out_degree_path << endl;
  }
  else
    cout << "Unable to open file";

  ofstream myfile2(in_degree_path);
  if (myfile2.is_open())
  {
    for (int i = 0; i < adj_list_in.size(); ++i)
    {
      //cout << label_index[i] << " : ";
      for (int j = 0; j < adj_list_in[i].size(); ++j)
      {
        myfile2 << i + 1 << " " << adj_list_in[i][j] + 1 << " "
               << "1"
               << "\n";
      }
    }
    myfile2.close();
    cout << "Graph written in format link-list in " << in_degree_path << endl;
  }
  else
    cout << "Unable to open file";
}

void Graph::tokenize_line_plus(std::string &line, std::string &delimiter, std::vector<std::string> &tokens)
{
  tokens.clear();
  size_t pos = 0;
  std::string token;
  while ((pos = line.find(delimiter)) != std::string::npos)
  {
    token = line.substr(0, pos);
    //std::cout << line << std::endl;
    tokens.push_back(token);
    line.erase(0, pos + delimiter.length());
  }
  tokens.push_back(line);
}

void tokenize_line(std::string &line, std::string &delimiter, std::vector<std::string> &tokens)
{
  tokens.clear();
  size_t pos = 0;
  std::string token;
  while ((pos = line.find(delimiter)) != std::string::npos)
  {
    token = line.substr(0, pos);
    //std::cout << line << std::endl;
    tokens.push_back(token);
    line.erase(0, pos + delimiter.length());
    //counter += 1;
  }
  //tokens.push_back(line);
}

void Graph::calculate_community(std::string &input_path, bool is_out_degree)
{
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  const std::string out_degree_path = input_path + "_out";
  const std::string in_degree_path = input_path + "_in";
  std::string pre = "Infomap ../code/" + out_degree_path + ".txt ../code/result/ -N 5 --directed";
  std::string result_path = "../code/" + out_degree_path + ".tree";
  inner_adj_list.clear();
  adjacency_community.clear();
  id_to_community.clear();
  participation_vectors.clear();
  participation_index.clear();
  dispersion_index.clear();
  global_hubness.clear();
  local_hubness.clear();
  community_to_density.clear();
  inner_adj_list = adj_list;
  if (is_out_degree == false){
    pre = "Infomap ../code/" + in_degree_path + ".txt ../code/result/ -N 10 --directed";
    result_path = "../code/" + in_degree_path + ".tree";
    inner_adj_list = adj_list_in;
  }
  
  std::string delimiter = " ";
  std::string second_delimiter = ":";
  std::vector<std::string> tokens;
  std::vector<std::string> inner_tokens;
  std::string line, division;
  int N = inner_adj_list.size(); // Number of nodes
  int M = n_edges;         // Number of edges
  int id, division_id;
  const char *command = pre.c_str();
  system(command);

  ifstream f(result_path); // Create a file stream for the specified path
  if (!f.is_open())
  {
    cerr << "Error: Unable to open file '" << result_path << "'" << endl;
    return;
  }
  cerr << "***********************************************" << endl;
  cerr << "Reading Infomap result from file '" << result_path << "'..." << endl;
  while (getline(f, line))
  {
    Graph::tokenize_line_plus(line, delimiter, tokens);
    division = tokens[0];
    if (division != "#")
    {
      id = stoi(tokens[3]);
      Graph::tokenize_line_plus(division, second_delimiter, inner_tokens);
      division_id = stoi(inner_tokens[0]);
      // For zero-based counter
      id = id - 1;
      id_to_community[id] = division_id;
      // Adjacency Community
      if (adjacency_community.count(division_id) > 0)
      {
        adjacency_community[division_id].push_back(id);
      }
      else
      {
        adjacency_community[division_id] = std::vector<int>();
        adjacency_community[division_id].push_back(id);
      }

      //cout << "Result read " << id << " in " << id_to_community[id] << endl;
    }
  }

  // Participation vectors
  cerr << "***********************************************" << endl;
  cerr << "Computing Participation Vectors for " << inner_adj_list.size() << endl;
  for (int i = 0; i < inner_adj_list.size(); ++i)
  {
    //cout << i << endl;
    double in_community = 0;
    if (participation_vectors.count(i) == 0)
    {
      participation_vectors[i] = std::vector<double>();
    }
    // Calculating participation vector for each community
    for (int j = 1; j <= adjacency_community.size(); ++j)
    {
      double P_i = 0;
      int numerator = degree_vector_in_community(inner_adj_list[i], adjacency_community[j]);
      int denominator = adjacency_community[j].size();
      P_i = static_cast<double>(numerator) / static_cast<double>(denominator);
      in_community += P_i;
      participation_vectors[i].push_back(P_i);
    }
    // Normalizing participation vectors
    for (int k = 0; k < participation_vectors[i].size(); ++k)
    {
      if (in_community > 0)
      {
        participation_vectors[i][k] = participation_vectors[i][k] / in_community;
      }
      else
      {
        participation_vectors[i][k] = 0.0;
      }
    }
  }

  // Participation index
  cerr << "***********************************************" << endl;
  cerr << "Computing Participation Index" << endl;
  for (int i = 0; i < participation_vectors.size(); ++i)
  {
    participation_index[i] = 1 - (participation_vectors[i].size() / sqrt(participation_vectors[i].size() - 1)) * standard_deviation(participation_vectors[i]);
  }

  // Dispersion
  cerr << "***********************************************" << endl;
  cerr << "Computing Dispersion" << endl;
  std::vector<double> reduced_vector;
  int dimension = 0;
  for (int i = 0; i < participation_vectors.size(); ++i)
  {
    reduced_vector.clear();
    reduced_vector = only_non_zero(participation_vectors[i]);
    dimension = reduced_vector.size();
    if (reduced_vector.size() > 1)
    {
      dispersion_index[i] = 1 - (dimension / sqrt(dimension - 1)) * standard_deviation(reduced_vector);
    }
    else
    {
      dispersion_index[i] = 0.0;
    }
  }

  // Global Hubness
  cerr << "***********************************************" << endl;
  cerr << "Computing Global Hubness" << endl;
  double p = density(N, M);
  for (int i = 0; i < inner_adj_list.size(); ++i)
  {
    global_hubness[i] = (inner_adj_list[i].size() - (N - 1) * p) / sqrt((N - 1) * p * (1 - p));
  }

  // Local Hubness
  cerr << "***********************************************" << endl;
  cerr << "Computing Local Hubness" << endl;
  int N_community = 0;
  double p_community = 0.0;
  double numerator = 0.0;
  double denominator = 0.0;
  for (int i = 0; i < inner_adj_list.size(); ++i)
  {
    N_community = adjacency_community[id_to_community[i]].size();
    p_community = Graph::density_of_community(adjacency_community[id_to_community[i]], inner_adj_list, id_to_community[i]);
    numerator = (inner_adj_list[i].size() - (N_community - 1) * p_community);
    denominator = sqrt((N_community - 1) * p_community * (1 - p_community));
    if (denominator > 0)
    {
      local_hubness[i] =  numerator/denominator;
    } else {
      local_hubness[i] = 0;
    }
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()/1.66667e-8 << "[ss]" << std::endl;

}

// Calculating density of community
double Graph::density_of_community(std::vector<int> &nodes_in_community, std::vector<std::vector<int>> &adj_list_set, int &community_id)
{
  double p = 0.0;
  int count = 0;
  int N = nodes_in_community.size();
  if (community_to_density.count(community_id) > 0)
  {
    return community_to_density[community_id];
  }
  if (N == 0)
  {
    return 0.0;
  }
  for (int i = 0; i < nodes_in_community.size(); ++i)
  {
    for (int j = 0; j < adj_list_set[nodes_in_community[i]].size(); ++j)
    {
      if (std::find(nodes_in_community.begin(), nodes_in_community.end(), adj_list_set[nodes_in_community[i]][j]) != nodes_in_community.end())
      {
        count += 1;
      }
    }
  }
  p = static_cast<double>(count) / static_cast<double>((N * (N - 1)) / 2);
  community_to_density[community_id] = p;
  //std::cout << "Local community " << p << std::endl;
  return community_to_density[community_id];
}

int degree_vector_in_community(std::vector<int> &degree, std::vector<int> &community)
{
  int result = 0;
  //std::vector<int> result;
  for (int i = 0; i < degree.size(); ++i)
  {
    if (std::find(community.begin(), community.end(), degree[i]) != community.end())
    {
      result += 1;
      //result.push_back(degree[i]);
    }
  }
  return result;
}

double standard_deviation(std::vector<double> &v)
{
  double sum = std::accumulate(v.begin(), v.end(), 0.0);
  double mean = sum / v.size();

  std::vector<double> diff(v.size());
  std::transform(v.begin(), v.end(), diff.begin(), [mean](double x) { return x - mean; });
  double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
  double stdev = std::sqrt(sq_sum / v.size());
  return stdev;
}

std::vector<double> only_non_zero(std::vector<double> &original)
{
  std::vector<double> result;
  for (int i = 0; i < original.size(); ++i)
  {
    if (original[i] > 0)
    {
      result.push_back(original[i]);
    }
  }
  return result;
}

double density(int &n_nodes, int &n_edges)
{
  if (n_nodes > 0)
  {
    return (static_cast<double>(n_edges) / ((static_cast<double>(n_nodes) * (static_cast<double>(n_nodes) - 1)) / 2));
  }
  else
  {
    return 0.0;
  }
}


