#include "day6_containers.h"
#include <cassert>
#include <iostream>
#include <random>
#include <deque>
int main(){
 std::mt19937 rng(12345);
 day6::LinkedList<int> a;std::list<int> ref;
 for(int i=0;i<10000;++i){int op=rng()%4,v=int(rng()%100);if(op==0){a.push_back(v);ref.push_back(v);}else if(op==1){a.push_front(v);ref.push_front(v);}else if(op==2&&!ref.empty()){assert(a.pop_front()==ref.front());ref.pop_front();}else if(op==3&&!ref.empty()){assert(a.pop_back()==ref.back());ref.pop_back();}assert(a.size()==ref.size());assert(std::equal(a.begin(),a.end(),ref.begin(),ref.end()));}
 day6::BST<int> tree;std::set<int> st;
 for(int i=0;i<10000;++i){int x=int(rng()%1000);if(rng()%2){assert(tree.insert(x)==st.insert(x).second);}else{assert(tree.erase(x)==bool(st.erase(x)));}assert(tree.values()==std::vector<int>(st.begin(),st.end()));}
 day6::RingBuffer<int> ring(17);std::deque<int> q;
 for(int i=0;i<10000;++i){int x=int(rng()%1000);if(rng()%3||q.empty()){ring.push(x);if(q.size()==17)q.pop_front();q.push_back(x);}else{assert(ring.pop()==q.front());q.pop_front();}assert(ring.values()==std::vector<int>(q.begin(),q.end()));}
 day6::HashMap<int,int> hash;std::map<int,int> hm;
 for(int i=0;i<10000;++i){int k=int(rng()%1000);if(rng()%3){hash[k]++;hm[k]++;}else{assert(hash.erase(k)==bool(hm.erase(k)));}assert(hash.size()==hm.size());for(auto& [key,value]:hm)assert(hash.contains(key));}
 day6::Matrix<double> m(2,2);m(0,0)=1;m(0,1)=2;m(1,0)=3;m(1,1)=4;assert(m.determinant()==-2);
 day6::PriorityQueue<int> heap;heap.push(5);heap.push(1);heap.push(8);assert(heap.pop()==8);
 day6::Graph<int> graph;for(int i=1;i<=3;++i)graph.addVertex(i);graph.addEdge(1,2);graph.addEdge(2,3);assert(graph.shortest(1,3).size()==3);
 day6::Trie trie;trie.insert(U"car");trie.insert(U"card");assert(trie.autocomplete(U"car").size()==2);trie.remove(U"card");assert(!trie.contains(U"card"));
 day6::Polynomial<double> poly;poly.set(2,3);poly.set(1,-2);poly.set(0,1);assert(poly.evaluate(2)==9);
 day6::LRUCache<int,int> cache(2);cache.put(1,1);cache.put(2,2);assert(cache.get(1)==1);cache.put(3,3);assert(!cache.contains(2));
 std::cout<<"Day 6: 40,000 randomized operations and smoke checks PASSED\n";
}
