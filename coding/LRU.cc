class Node {
    int key;
    int value;
    Node* pre;
    Node* next;
    Node(int k = 0, int v = 0) : key(k), value(v) {}
};

class LRU {
private:
    Node* dummy;
    unordered_map<int, Node*> mp;
    int capacity;
    void remove_node(Node* node) {
        node -> pre -> next = node -> next -> next;
        node -> next -> pre = node -> pre;
    }

    void push_front(Node *node) {
        node -> next = dummy -> next;
        dummy -> next = node;
        node -> next -> pre = node;
        node -> pre = dummy;
    }

    Node *get_node(int key) {
        auto it = mp.find(key);
        if( it == mp.end()) {
            return nullptr;
        }
        //找到了就删掉当前节点，添加到头部
        Node* node = it -> second();
        remove_node(node);
        push_front(node);
        return node;
    }

public:
    LRU(int cap) : capacity(cap), dummy(new Node()) {
        dummy -> next = dummy;
        dummy -> pre = dummy;
    }

    int get(int key) {
        Node* node = get_node(key);
        return node ? node -> value : -1;
    }

    void put(int key, int val) {
        auto it = mp.find(key);
        if(it != mp.end()) {
            it -> second -> value = val;
            return;
        }
        mp[key] = new Node(key, val);
        push_front(mp[key]);
        if(mp.size() > capacity) {
            Node* tmp = dummy -> pre;
            remove_node(tmp -> key);
            mp.erase(tmp -> key);
            delete tmp;
        }
    }
};



