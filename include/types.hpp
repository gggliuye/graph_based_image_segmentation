


struct pixel
{
    int i = -1;
    int j = -1;
};

struct edge
{
    pixel pixel_i;
    pixel pixel_j;
    float weight;
};

// compare the weight of edge
// define the operator for using std::sort
bool operator<(const edge &a, const edge &b) {
  return a.weight < b.weight;
}


struct component
{  
    bool isTop = true;
    int size = 1;
    int parentIdx;
    float mInt;
};

class ComponentTree
{
public:
    ComponentTree(int num_vertices, float c_);
    ~ComponentTree();
    void join(int i, int j, float weight);
    int findParent(int idx);
    float getMInt(int idx);
    int sizeOfComponent(int idx) const { return components[idx].size; }
    int size() const {return num_component;}

private:
    component *components;
    int num_component;
    float c;
};

ComponentTree::ComponentTree(int num_vertices, float c_)
{
    components = new component[num_vertices];
    num_component = num_vertices;
    c = c_;

    for(int i = 0; i < num_vertices; i++){
        // initialize the parent (the center) of the component as itself
        components[i].parentIdx = i;
        components[i].mInt = c;
    }
}

ComponentTree::~ComponentTree() {
  delete [] components;
}

void ComponentTree::join(int i, int j, float weight)
{
    if(components[i].isTop && components[j].isTop){
        // set the top vertex as jth vertex
        components[i].isTop = false;
        components[i].parentIdx = j;
        components[j].size += components[i].size;
        num_component--;
        if(weight > 0){
            components[j].mInt = weight + c / (components[j].size);
        }
    }else{
        fprintf(stderr, " All the input vertices should be top vertex. \n");
    }
}

int ComponentTree::findParent(int idx)
{
    int output = idx;
    while(!components[output].isTop){
        output = components[output].parentIdx;
    }
    // save the result to make later calculation more convenient
    components[idx].parentIdx = output;
    return output;
}

float ComponentTree::getMInt(int idx)
{
    return components[idx].mInt;
}

