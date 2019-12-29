import networkx as nx
import infomap
import math
import statistics

adjacency = dict()
adjacency_community = dict()
node_to_community = dict()
community_sizes = list()
participation_vectors = dict()
participation_index = dict()
dispersion_index = dict()
edge_list = list()
global_hubness = dict()
local_hubness = dict()


def density_community(nodes_in_community, edge_list):
    '''
    Calculates density of a community based on a given edge_list and the nodes in the community
    '''
    counter = 0
    for u in nodes_in_community:
        for v in adjacency[node]:
            if (u, v) in edge_list and v in nodes_in_community:
                counter += 1
    N = len(nodes_in_community)
    potential_connections = (N * (N - 1))/2
    return counter/potential_connections


g = nx.read_edgelist("C-elegans-frontal.txt",
                     create_using=nx.DiGraph(), nodetype=int)

# Building an adjacency list for easy query
for (u, v) in g.edges:
    if u not in adjacency.keys():
        adjacency[u] = list()
    adjacency[u].append(v)
    edge_list.append((u, v))

# Source in same folder
filename = "C-elegans-frontal.txt"
infomapWrapper = infomap.Infomap("--directed")

infomapWrapper.readInputData(filename)
# Running infomap
infomapWrapper.run()

# Building adjacency list of community -> nodes
# Building node to community dictionary
for node in infomapWrapper.iterTree():
    if node.isLeaf():
        if node.moduleIndex() not in adjacency_community.keys():
            adjacency_community[node.moduleIndex()] = list()
        adjacency_community[node.moduleIndex()].append(node.physicalId)
        node_to_community[node.physicalId] = node.moduleIndex()

# Captures the size of each community, not really necessary but good to have
for community in adjacency_community.keys():
    community_sizes.append(len(adjacency_community[community]))

# Calculating participation vectors
for node in adjacency.keys():
    in_community = 0.0
    if node not in participation_vectors.keys():
        participation_vectors[node] = list()
    for community in adjacency_community.keys():
        P_i = 0.0
        # Setting numerator y denominator according to definitions
        # numerator is the out-degree of the node inside the community
        # denominator is the size of the community
        numerator = len(list(filter(lambda x: x in adjacency_community[community],
                                    adjacency[node])))
        denominator = (community_sizes[community])
        P_i = numerator/denominator
        in_community += P_i
        participation_vectors[node].append(P_i)
    for i in range(1, len(participation_vectors[node])):
        # If the node does not have out-degree (or degree in general given that we are considering it directed by default)
        # Then it is 0
        participation_vectors[node][i] = (
            participation_vectors[node][i]/in_community if in_community > 0 else 0)

# Calculating participation index
for node in participation_vectors.keys():
    M = len(participation_vectors[node])
    participation_index[node] = 1 - (M/math.sqrt(M-1)) * \
        statistics.stdev(participation_vectors[node])
# Calculating dispersion
for node in participation_vectors.keys():
    # Only considering the values > 0
    reduced_vector = list(filter(lambda x: x > 0, participation_vectors[node]))
    M = len(reduced_vector)
    # If dimension <= 1, assume disperion equals 0
    if M > 1:
        dispersion_index[node] = 1 - (M/math.sqrt(M-1)) * \
            statistics.stdev(reduced_vector)
    else:
        dispersion_index[node] = 0

# Density of the whole graph
p = nx.density(g)
# Number of nodes
N = g.number_of_nodes()
# Calculating global hubness
for u in adjacency.keys():
    global_hubness[u] = (len(adjacency[u]) - (N - 1) * p) / \
        math.sqrt((N - 1)*p*(1 - p))

# Calculating local hubness
for u in adjacency.keys():
    p = density_community(adjacency_community[node_to_community[u]], edge_list)
    N = community_sizes[node_to_community[u]]
    if N > 1 and p > 0:
        local_hubness[u] = (len(adjacency[u]) - (N - 1) * p) / \
            math.sqrt((N - 1)*p*(1 - p))
    else:
        local_hubness[u] = N


def main():
    print("Applying infomap")
    print("Found {} modules with codelength: {}".format(
        infomapWrapper.numTopModules(), infomapWrapper.codelength()))
    print("Result")
    # print("\n#node module")
    # for node in infomapWrapper.iterTree():
    #     if node.isLeaf():
    #         print("{} {}".format(node.physicalId, node.moduleIndex()))
    print(N)
    summ = 0
    for community in adjacency_community.keys():
        summ += len(adjacency_community[community])
        print(str(community) + " = " + str(adjacency_community[community]))
    print(summ)
    for u in global_hubness.keys():
        print("\nInformation of " + str(u))
        print("Community : " + str(node_to_community[u]))
        print("Participation index: " + str(participation_index[u]))
        print("Dispersion index: " + str(dispersion_index[u]))
        print("Participation vector: " + str(participation_vectors[u]))
        print("Global Hubness : " + str(global_hubness[u]))
        print("Local Hubness : " + str(local_hubness[u]))


if __name__ == "__main__":
    main()
