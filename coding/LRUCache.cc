#include <bits/stdc++.h>
using namespace std;

class Node {
    int key;
    int value;
    Node* next;
    Node* pre;
    Node(int k, int v) : key(k), value(v), next(nullptr), pre(nullptr) {}
};

class LRUCache {
private:
    unordered_map<int, Node*> mp;
    Node* dummy;
    int capacity;
    
    void remove(Node* node) {
        node -> next -> pre = node -> pre;
        node -> pre -> next = node -> next;
    }

    void push_front(Node* node) {
        dummy -> next -> pre = node;
        node -> next = dummy -> next;
        dummy -> next = node;
        node -> pre = dummy;
    }


    Node* get_node(int key) {
        if(mp.find(key) == mp.end()) {
            return nullptr;
        }
        Node* x = mp[key];
        remove(mp[key]);
        push_front(x);
        return x;
    }

public:
    LRUCache(int cap) : capacity(cap) {
        dummy -> next = dummy;
        dummy -> pre = dummy;
    }

    int get(int key) {
        Node* x = get_node(key);
        return x ? x->value : 0; 
    }

    void put(int key, int value) {
        Node* node = get_node(key);
        //存在该节点就修改值
        if(node) {
            node -> value = value;
            return;
        }
        //没有该节点就加入
        Node* x = new Node(key, value);
        mp[key] = x;
        push_front(x);
        if(mp.size() > capacity) {
            Node* tmp = dummy -> pre;
            remove(tmp);
            mp.erase(tmp -> key);
            delete(tmp);
        }

    }

};