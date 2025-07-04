#include <string>
using namespace std;

class Node // RBT node
{
public:
    int data; // this is the product id
    string name;
    int stock;

    Node* left;
    Node* right;
    Node* parent;
    bool color;

    Node(int data,string name,int stock)
    {
        this -> data = data;
        left = right = parent = nullptr;
        this->color = 1;

        this->name = name;
        this->stock = stock;
    }
};


