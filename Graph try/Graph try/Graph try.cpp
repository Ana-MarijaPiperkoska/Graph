#include <random>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stack>
#include <chrono>
#include <array>

using namespace std;

constexpr int MaxArrayCount = 400;
constexpr int MinNumber = 0;
constexpr int MaxNumber = 1;

using UINT = unsigned int;

struct Node {
    int m_x;
    int m_y;
    bool m_checked;
    vector<UINT> m_neighbours;
    int m_parent;
};

class Graph {
public:
    Graph(const vector<Node>& nodes) : m_nodes(nodes) {}
    virtual ~Graph() {}
    virtual bool FindPath(UINT start, UINT end) = 0;
   
    const vector<Node>& GetGraph() const { return m_nodes; }  
protected:
    vector<Node> m_nodes;
};

class BreadthFirst : public Graph {
public:
    BreadthFirst(const vector<Node>& nodes) : Graph(nodes) {}
    bool FindPath(UINT start, UINT end);
    const vector<Node>& GetGraph() const { return m_nodes; }
};

class DepthFirst : public Graph {
public:
    DepthFirst(const vector<Node>& nodes) : Graph(nodes) {}
    bool FindPath(UINT start, UINT end);
    const vector<Node>& GetGraph() const { return m_nodes; }
};

bool BreadthFirst::FindPath(UINT start, UINT end) {
    queue<UINT> queue;
    queue.push(start);

    while (!queue.empty()) {
        UINT current = queue.front();
        queue.pop();

        if (current == end) {
            cout << "BFS: Goal found at position " << current << "\n";
            return true;
        }

        for (UINT neighbor : m_nodes[current].m_neighbours) {
            if (!m_nodes[neighbor].m_checked) {
                m_nodes[neighbor].m_checked = true;
                m_nodes[neighbor].m_parent = current;
                queue.push(neighbor);
            }
        }
    }

    cout << "BFS: Goal not found!\n";
    return false;
}

bool DepthFirst::FindPath(UINT start, UINT end) {
    stack<UINT> stack;
    stack.push(start);

    while (!stack.empty()) {
        UINT current = stack.top();
        stack.pop();

        if (current == end) {
            cout << "DFS: Goal found at position " << current << "\n";
            return true;
        }

        for (UINT neighbor : m_nodes[current].m_neighbours) {
            if (!m_nodes[neighbor].m_checked) {
                m_nodes[neighbor].m_checked = true;
                m_nodes[neighbor].m_parent = current;
                stack.push(neighbor);
            }
        }
    }

    cout << "DFS: Goal not found!\n";
    return false;
}

enum SEARCH_ALGORITHMS {
    BREADTH_SEARCH,
    DEPTH_SEARCH,
};

array<Graph*, 3> graphs; 

float RunSearchAlgorithms(char* arr, SEARCH_ALGORITHMS algorithm) {
    
    UINT startNode = 0;
    UINT endNode = 0;

    vector<Node> nodes;
    
    graphs[BREADTH_SEARCH] = new BreadthFirst(nodes);
    graphs[DEPTH_SEARCH] = new DepthFirst(nodes);

    chrono::steady_clock::time_point begin = chrono::steady_clock::now();

    bool found = false;

    
    switch (algorithm) {
    case BREADTH_SEARCH:
        found = graphs[BREADTH_SEARCH]->FindPath(startNode, endNode);
        break;
    case DEPTH_SEARCH:
        found = graphs[DEPTH_SEARCH]->FindPath(startNode, endNode);
        break;
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();

    if (found) {
        const vector<Node>& resultNodes = graphs[algorithm]->GetGraph();
        
    }

    return static_cast<float>(chrono::duration_cast<chrono::microseconds>(end - begin).count());
}

void GetArrayFromFile(char* arr, const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        char c;
        UINT i = 0;
        while (file.get(c) && i < MaxArrayCount) {
            if (c == 'o' || c == 'X' || c == 'S' || c == 'G') {
                arr[i] = c;
                i++;
            }
        }
    }
    else {
        cout << "Failed to open file: " << fileName << "\n";
    }

    file.close();
}

void ConstructGraph(char* arr, UINT startNode, UINT endNode, vector<Node>& nodes) {
    
    int gridSize = static_cast<int>(sqrt(MaxArrayCount));
    nodes.resize(MaxArrayCount);

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            int index = i * gridSize + j;
            nodes[index].m_x = i;
            nodes[index].m_y = j;
            nodes[index].m_checked = false;
            nodes[index].m_parent = -1;

           
            if (j + 1 < gridSize)
                nodes[index].m_neighbours.push_back(index + 1);
            if (i + 1 < gridSize)
                nodes[index].m_neighbours.push_back(index + gridSize);
        }
    }
}

void DrawPath(const vector<Node>& nodes, UINT startNode, UINT endNode) {
    
    int gridSize = static_cast<int>(sqrt(nodes.size()));

    
    vector<char> pathGrid(nodes.size(), '_');

    
    pathGrid[startNode] = 'S';
    pathGrid[endNode] = 'G';

    UINT current = endNode;
    while (current != startNode) {
        pathGrid[current] = '*'; 
        current = nodes[current].m_parent;
    }

    
    cout << "\nPath:\n";
    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            int index = i * gridSize + j;
            cout << pathGrid[index] << " ";
        }
        cout << "\n";
    }
}

int main() {
    cout << "Graph Search Algorithms Start: \n \n";

    array<char, MaxArrayCount> charNodes;
    GetArrayFromFile(&charNodes[0], "AssignmentNodes.txt");

    vector<Node> nodes;
    ConstructGraph(&charNodes[0], 0, 0, nodes);

    graphs[BREADTH_SEARCH] = new BreadthFirst(nodes);
    graphs[DEPTH_SEARCH] = new DepthFirst(nodes);

    
    {
        cout << "Breadth Search: \n";
        float duration = RunSearchAlgorithms(&charNodes[0], BREADTH_SEARCH);
        cout << "Time taken for search: " << duration << "ms \n";
    }

    cout << "\n";

    
    {
        cout << "Depth Search: \n";
        float duration = RunSearchAlgorithms(&charNodes[0], DEPTH_SEARCH);
        cout << "Time taken for search: " << duration << "ms \n";
    }

    cout << "\n";
    cout << "Graph Search Algorithms End: \n \n";
    delete graphs[BREADTH_SEARCH];
    delete graphs[DEPTH_SEARCH];

    return 0;
}

