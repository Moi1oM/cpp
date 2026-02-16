#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

struct Node{
    std::string nodeValue;
    std::vector<Node*> nextNodes;

    Node() = default;
    Node(std::string value) : nodeValue(value) {}
};
std::vector<Node*> starting;

void dfs(std::vector<std::string>& values,
    std::vector<Node*>& nextValues, int index) {
        if(values.size()<=index) return;
        std::string val = values[index];
        bool notExists = true;
        for (size_t i = 0; i < nextValues.size(); i++) {
            Node* nextValue = nextValues[i];
            if (nextValue->nodeValue == val) {
                notExists = false;
                dfs(values, nextValue->nextNodes, index+1);
            }
        }
        if(notExists) {
            Node* newNode = new Node(val);
            nextValues.push_back(newNode);
            dfs(values, newNode->nextNodes, index+1);
        }
}

void printDashWithString(std::string food, int depth) {
    for (size_t i = 0; i < depth; i++) {
        std::cout<<"--";
    }
    std::cout<<food<<"\n";
}

void printPretty(std::vector<Node*>& nodes, int depth) {
    //sort
    std::sort(nodes.begin(), nodes.end(), [](Node* a, Node* b) {
              return a->nodeValue < b->nodeValue;
          });
    for (size_t i = 0; i < nodes.size(); i++) {
        Node* node = nodes[i];
        printDashWithString(node->nodeValue, depth);
        printPretty(node->nextNodes, depth+1);
    }
}

int main() {
    int N;
    std::cin>>N;
    while(N--) {
        int num;
        std::string tmp;
        std::cin>>num;
        std::vector<std::string> foods(num);
        for (size_t i = 0; i < num; i++) {
            std::cin>>foods[i];
        }
        dfs(foods, starting, 0);
    }

    printPretty(starting, 0);
    return 0;
}