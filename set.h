#include <vector>
#include <iostream>


template <class T>
struct Node{
    Node(): parent(nullptr){}

    Node (const T& elem):parent(nullptr), max_of_subtree(elem){};

    Node(Node* t);

    Node* parent;
    std :: vector <Node*> sons;
    std :: vector <T*> keys;
    T max_of_subtree;
};



template<class T>
class Set;

template<class T>
class iterator{

    friend Set<T>;
    using Node = Node<T>;
    using Set = Set<T>;
public:

    iterator() = default;

    T operator*();

    const T* operator ->();

    bool operator!=(iterator b) const;

    bool operator ==(iterator b) const;

    iterator& operator =(iterator it);

    iterator& operator++();

    iterator operator++(int);

    iterator operator--(int);

    iterator& operator--();

private:
    const Node* ptr;
    const Set* st;

    iterator(const Node* t, const Set* set_) :ptr (t),st(set_) {};

    const Node* GoUpForNext(const Node* t) const;

    const Node* GoUpForPrevious(const Node* t) const;

};

template<class T>
class Set {
public:
    friend iterator<T>;

    using iterator = iterator<T>;
    using Node = Node<T>;



    Set();

    Set(const Set& s);

    Set& operator = (const Set& s);

    template<typename Iterator>
    Set(Iterator first, Iterator last);

    Set(std::initializer_list<T> elems);

    ~Set();

    size_t size() const;

    bool empty() const;

    void insert(const T& elem);

    void erase(const T& elem);

    iterator begin() const;

    iterator end() const;

    iterator find(const T& elem) const;

    iterator lower_bound(const T& elem) const;
private:
    Node* Copy(Node* t);

    void Delete(Node* t);

    Node* FindNode(const T& x) const;

    void FixChildren(Node* t);

    void SortSons(Node* t);

    void UpdateKeys(Node* t);

    void Split(Node* t);

    Node* FindBrother(Node* t);

    size_t data_size_;
    Node* root_ = nullptr;
    const Node* END_NODE = nullptr;

};

template<class T>
Node<T>::Node(Node *t) {
    parent = t->parent;
    sons.push_back(t->sons[2]);
    sons.push_back(t->sons[3]);
    max_of_subtree = sons.back()->max_of_subtree;
    keys.push_back(&(t->sons[2]->max_of_subtree));
}

template<class T>
const Node<T> * iterator<T>::GoUpForNext(const Node *t) const {
    if (t == nullptr || t->parent == nullptr) {
        return nullptr;
    }
    Node* p = t->parent;
    int32_t ind = 0;
    while (p->sons[ind]->max_of_subtree < t->max_of_subtree) {
        ++ind;
    }
    if (ind != p->sons.size() - 1) {
        p = p->sons[ind + 1];
        while (!p->sons.empty()){
            p = p->sons[0];
        }
        return p;
    }
    return GoUpForNext(p);
}

template<class T>
const Node<T> *iterator<T>::GoUpForPrevious(const Node *t) const {
    if (t == nullptr || t->parent == nullptr) {
        return nullptr;
    }
    Node* p = t->parent;
    int32_t ind = 0;
    while (p->sons[ind]->max_of_subtree < t->max_of_subtree) {
        ++ind;
    }
    if (ind != 0) {
        p = p->sons[ind - 1];
        while(!p->sons.empty()){
            p = p->sons.back();
        }
        return p;
    }
    return GoUpForPrevious(p);
}

template<class T>
iterator<T> &iterator<T>::operator--() {
    if (ptr == nullptr){
        Node* root = st->root_;
        while (!root->sons.empty()){
            root = root->sons.back();
        }
        ptr = root;
        return *this;
    }
    ptr = GoUpForPrevious(ptr);
    return *this;
}

template<class T>
iterator<T> iterator<T>::operator--(int) {
    iterator temp = *this;
    --*this;
    return temp;
}

template<class T>
iterator<T> iterator<T>::operator++(int) {
    iterator temp = *this;
    ++*this;
    return temp;
}

template<class T>
iterator<T> &iterator<T>::operator++() {
    ptr = GoUpForNext(ptr);
    return *this;
}

template<class T>
iterator<T> &iterator<T>::operator=(iterator it) {
    ptr = it.ptr;
    return *this;
}

template<class T>
bool iterator<T>::operator==(iterator b) const {
    if(b.ptr == nullptr) {
        return ptr == nullptr;
    }
    return !(ptr->max_of_subtree < b.ptr->max_of_subtree || b.ptr->max_of_subtree < ptr->max_of_subtree || st != b.st);
}

template<class T>
bool iterator<T>::operator!=(iterator b) const {
    if(b.ptr == nullptr) {
        return ptr != nullptr;
    }
    return ptr->max_of_subtree < b.ptr->max_of_subtree || b.ptr->max_of_subtree < ptr->max_of_subtree || st != b.st;
}

template<class T>
const T *iterator<T>::operator->() {
    const T* point = &(ptr->max_of_subtree);
    return point;
}

template<class T>
T iterator<T>::operator*() {
    return ptr->max_of_subtree;
}


template <class T>
Set<T>::Set() {
    root_ = nullptr;
    data_size_ = 0;
}

template <class T>
Set<T>::Set(const Set<T> &s) {
    if (s.root_ != nullptr){
        root_ = Copy(s.root_);
    } else {
        root_ = nullptr;
    }
    data_size_ = s.data_size_;
}

template <class T>
Set<T> &Set<T>::operator=(const Set<T> &s) {
    if (this == &s) {
        return *this;
    }
    if (root_ != nullptr){
        Delete(root_);
    }
    if (s.root_ != nullptr){
        delete root_;
        root_ = Copy(s.root_);
    }
    data_size_ = s.data_size_;
    return *this;
}

template <class T>
template<typename Iterator>
Set<T>::Set(Iterator first, Iterator last) {
    data_size_ = 0;
    while (first != last) {
        insert(*first);
        ++first;
    }
}

template <class T>
Set<T>::Set(std::initializer_list<T> elems) {
    data_size_ = 0;
    for(auto it : elems) {
        insert(it);
    }
}

template <class T>
Set<T>::~Set(){
    Delete(root_);
    delete root_;
}

template <class T>
size_t Set<T>::size() const {
    return data_size_;
}

template <class T>
bool Set<T>::empty() const {
    return (data_size_ == 0);
}

template <class T>
void Set<T>::insert(const T &elem) {
    Node* new_Node = new Node(elem);
    if (data_size_ == 0) {
        root_ = new_Node;
        ++data_size_;
        return;
    }
    Node* found_Node = FindNode(elem);
    if (!(found_Node->max_of_subtree < elem || elem < found_Node->max_of_subtree)) {
        delete new_Node;
        return;
    }
    if (found_Node->parent == nullptr) {
        Node* new_root = new Node();
        new_root->sons.push_back(root_);
        new_root->sons.push_back(new_Node);
        root_->parent = new_root;
        new_Node->parent = new_root;
        SortSons(new_root);
        UpdateKeys(new_root);
        root_ = new_root;
    } else {
        Node* p = found_Node->parent;
        p->sons.push_back(new_Node);
        new_Node->parent = p;
        SortSons(p);
        UpdateKeys(p);
        Split(p);
    }
    UpdateKeys(new_Node);
    ++data_size_;
}


template <class T>
void Set<T>::erase(const T &elem) {
    if (data_size_ == 0) {
        return;
    }
    Node* found_Node = FindNode(elem);
    if (found_Node->max_of_subtree < elem || elem < found_Node->max_of_subtree) {
        return;
    }
    --data_size_;
    if (found_Node->parent == nullptr) {
        delete root_;
        root_ = nullptr;
    } else {
        Node* p = found_Node->parent;
        for (size_t i = 0; i < p->sons.size(); ++i) {
            if (p->sons[i] == found_Node) {
                p->sons.erase(p->sons.begin() + i);
                break;
            }
        }
        UpdateKeys(p);
        delete found_Node;
        found_Node = nullptr;
        FixChildren(p);
    }
}

template <class T>
iterator<T> Set<T>::begin() const {
    if(data_size_ == 0) {
        iterator it(END_NODE, this);
        return it;
    }
    Node* t = root_;
    while (!(t->sons.empty())){
        t = t->sons[0];
    }
    iterator it(t, this);
    return it;
}

template <class T>
iterator<T> Set<T>::end() const {
    iterator it(END_NODE, this);
    return it;
}

template <class T>
typename Set<T>::iterator Set<T>::find(const T &elem) const {
    if (data_size_ == 0) {
        iterator it(END_NODE, this);
        return it;
    }
    Node *t = FindNode(elem);
    if (t->max_of_subtree < elem || elem < t->max_of_subtree) {
        return this->end();
    }
    iterator it(t, this);
    return it;
}

template <class T>
typename Set<T>::iterator Set<T>::lower_bound(const T &elem) const {
    if (data_size_ == 0) {
        iterator it(END_NODE, this);
        return it;
    }
    Node* t = FindNode(elem);
    if(t->max_of_subtree < elem) {
        return this->end();
    }
    iterator it(t, this);
    return it;
}

template <class T>
typename Set<T>::Node *Set<T>::Copy(Node *t) {
    if (t == nullptr) {
        return nullptr;
    }
    Node* new_Node = new Node();
    if (t->sons.empty()){
        new_Node->max_of_subtree = t->max_of_subtree;
        return new_Node;
    }
    for (auto son: t->sons) {
        Node* new_Node_son = Copy(son);
        new_Node->sons.push_back(new_Node_son);
        new_Node_son->parent = new_Node;
    }
    UpdateKeys(new_Node);
    return new_Node;
}

template <class T>
void Set<T>::Delete(Node *t) {
    if (t == nullptr) {
        return;
    }
    for (auto son: t->sons) {
        Delete(son);
    }
    if (t != root_){
        delete t;
    }
}

template <class T>
typename Set<T>::Node *Set<T>::FindNode(const T &x) const {
    Node* t = root_;
    if(t->sons.empty()){
        return t;
    }
    while (!t->sons.empty()){
        if (t->sons.size() == 2){
            if (*t->keys[0] < x) {
                t = t->sons[1];
            } else {
                t = t->sons[0];
            }
        } else {
            if (*t->keys[1] < x) {
                t = t->sons[2];
            } else {
                if (*t->keys[0] < x) {
                    t = t->sons[1];
                } else {
                    t = t->sons[0];
                }
            }
        }
    }
    return t;
}

template <class T>
void Set<T>::FixChildren(Node *t) {
    if (t->sons.size() >= 2) {
        return;
    }
    if (t->parent == nullptr) {
        root_ = t->sons[0];
        delete t;
        t = nullptr;
        root_->parent = nullptr;
        return;
    }
    Node* brother = FindBrother(t);
    brother->sons.push_back(t->sons[0]);
    t->sons[0]->parent = brother;
    for (size_t i = 0; i < t->parent->sons.size(); ++i) {
        if (t->parent->sons[i] == t) {
            t->parent->sons.erase(t->parent->sons.begin() + i);
            break;
        }
    }
    UpdateKeys(t->parent);
    SortSons(brother);
    UpdateKeys(brother);
    Split(brother);
    Node* p = t->parent;
    delete t;
    FixChildren(p);
}

template <class T>
void Set<T>::SortSons(Node *t) {
    for(int i = t->sons.size() - 2; i >= 0; i--) {
        if (t->sons[i + 1]->max_of_subtree < t->sons[i]->max_of_subtree) {
            std::swap(t->sons[i], t->sons[i + 1]);
        }
    }
}

template <class T>
void Set<T>::UpdateKeys(Node *t) {
    while(t != nullptr){
        t->keys.clear();
        if (t->sons.empty()) {
            t = t->parent;
            continue;
        }
        for (size_t i = 0; i < t->sons.size() - 1; ++i) {
            t->keys.push_back(&(t->sons[i]->max_of_subtree));
        }
        t->max_of_subtree = t->sons.back()->max_of_subtree;
        t = t->parent;
    }
}


template <class T>
void Set<T>::Split(Node *t) {
    if (t->sons.size() > 3) {
        Node* new_Node = new Node(Node(t));
        t->sons[2]->parent = new_Node;
        t->sons[3]->parent = new_Node;
        t->sons.pop_back();
        t->sons.pop_back();
        t->max_of_subtree = t->sons.back()->max_of_subtree;
        t->keys = {&(t->sons[0]->max_of_subtree)};
        if (t->parent != nullptr){
            t->parent->sons.push_back( new_Node);
            SortSons(t->parent);
            UpdateKeys(t->parent);
            t->parent->max_of_subtree = t->parent->sons.back()->max_of_subtree;
            Split(t->parent);
        } else {
            Node* new_root = new Node();
            new_root->sons.push_back(root_);
            new_root->sons.push_back(new_Node);
            root_->parent = new_root;
            new_Node->parent = new_root;
            SortSons(new_root);
            root_ = new_root;
            UpdateKeys(root_);
        }

    }
}

template <class T>
typename Set<T>::Node *Set<T>::FindBrother(Node *t) {
    int32_t ind = 0;
    for (size_t i = 0; i < t->parent->sons.size(); ++i) {
        if (t->parent->sons[i] == t) {
            ind = i;
        }
    }
    if (ind == 1) {
        return t->parent->sons[0];
    } else {
        return t->parent->sons[1];
    }
}


