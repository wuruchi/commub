
#include "graph.h"
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <random>

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
size_t tokenize_line(const string &line, vector<size_t> &token_pos, const string &sep = " ")
{
  token_pos.clear(); // Clear the vector before pushing in the new tokens
  size_t tail = 0, head = line.find(sep);
  while (tail < line.size())
  {
    if (head > tail)
    {
      token_pos.push_back(tail);
      token_pos.push_back(head - tail);
    }
    tail = head + 1;
    head = line.find(sep, tail);
    if (head > line.size())
      head = line.size();
  }
  return token_pos.size() / 2;
}



// This assumes that the first line in the file indicates the number of vertices
// and edges in the graph described by the file. Those numbers are used as a
// reference and not actually included in the graph.
int read_graph_from_file(const string &path, Graph &g)
{
  int lines_read = 1, vx_read, exp_v, exp_e, vx[2], loops = 0, multiedges = 0;
  string line, w;
  vector<size_t> token_pos;
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
    vx_read = 0;
    for (size_t i = 0; i < tokenize_line(line, token_pos); ++i)
    {
      if (!line.compare(token_pos[2 * i], token_pos[2 * i + 1], " "))
      {
        cerr << " - Bad word token found at file row " << lines_read << endl;
      }
      else if (++vx_read > 2)
      {
        cerr << " - More than 2 vertices found at file row " << lines_read
             << ". Omitting additional vertices" << endl;
      }
      else
      {
        vx[i] = g.add_vertex(line.substr(token_pos[2 * i], token_pos[2 * i + 1]));
      }
    }
    //cout << vx[0] << " # " << vx[1] << endl;
    //cout << line << endl;
    if (vx_read != 2)
    {
      cerr << " - Malformed edge representation at file row " << lines_read << endl;
    }
    else if (vx[0] == vx[1])
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
  for (int i = 0; i < adj_list.size(); ++i)
  {
    cout << label_index[i] << " : ";
    for (int j = 0; j < adj_list[i].size(); ++j)
    {
      cout << label_index[adj_list[i][j]] << " ";
    }
    cout << endl;
  }
}

void Graph::print_edges(const std::string &txtfile)
{
  ofstream myfile(txtfile);
  if (myfile.is_open())
  {
    for (int i = 0; i < adj_list.size(); ++i)
      {
        //cout << label_index[i] << " : ";
        for (int j = 0; j < adj_list[i].size(); ++j)
        {
          myfile << i + 1 << " " << adj_list[i][j] + 1 << " " << "1" << "\n";
        }
      }
    myfile.close();
    cout << "Graph written in format link-list in " << txtfile << endl;
  }
  else
    cout << "Unable to open file";
}

void Graph::tokenize_line_plus(std::string &line, std::string &delimiter, std::vector<std::string> &tokens)
{
  tokens.clear();
  size_t pos = 0;
  std::string token;
  while ((pos = line.find(delimiter)) != std::string::npos) {
      token = line.substr(0, pos);
      //std::cout << line << std::endl;
      tokens.push_back(token);
      line.erase(0, pos + delimiter.length());
  }
  tokens.push_back(line);
}

int Graph::calculate_community(std::string &input_path){
  const std::string pre = "Infomap ../code/" + input_path + ".txt ../code/result/ -N 10 --directed";
  const std::string result_path = "../code/" + input_path + ".tree";
  std::string delimiter = " ";
  std::string second_delimiter = ":";
  std::vector<std::string> tokens;
  std::vector<std::string> inner_tokens;
  std::string line, division;
  int id, division_id;
  const char *command = pre.c_str();
  system(command);
  
  ifstream f(result_path); // Create a file stream for the specified path
  if (!f.is_open())
  {
    cerr << "Error: Unable to open file '" << result_path << "'" << endl;
    return -1;
  }
  cerr << "***********************************************" << endl;
  cerr << "Reading Infomap result from file '" << result_path << "'..." << endl;
  while(getline(f,line)){
    Graph::tokenize_line_plus(line, delimiter, tokens);
    division = tokens[0];
    if (division != "#"){
      id = stoi(tokens[3]);
      Graph::tokenize_line_plus(division, second_delimiter, inner_tokens);
      division_id = stoi(inner_tokens[0]);
      // For zero-based counter
      id = id - 1;
      id_to_community[id] = division_id;
      cout << "Result read " << id << " in " << id_to_community[id] << endl;
    }

  }
}
