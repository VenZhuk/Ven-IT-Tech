#pragma once
#include <algorithm>
#include <cmath>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <iterator>

namespace day6 {
template<class T> class LinkedList {
 std::list<T> values_;
public:
 LinkedList()=default; LinkedList(const LinkedList&)=default; LinkedList(LinkedList&&) noexcept=default;
 LinkedList& operator=(const LinkedList&)=default; LinkedList& operator=(LinkedList&&) noexcept=default; ~LinkedList()=default;
 void push_front(const T& x){values_.push_front(x);} void push_back(const T& x){values_.push_back(x);}
 T pop_front(){if(values_.empty())throw std::out_of_range("empty list");T x=values_.front();values_.pop_front();return x;}
 T pop_back(){if(values_.empty())throw std::out_of_range("empty list");T x=values_.back();values_.pop_back();return x;}
 void insert(size_t i,const T& x){if(i>size())throw std::out_of_range("insert index");auto it=values_.begin();std::advance(it,i);values_.insert(it,x);}
 void erase(size_t i){if(i>=size())throw std::out_of_range("erase index");auto it=values_.begin();std::advance(it,i);values_.erase(it);}
 T& operator[](size_t i){if(i>=size())throw std::out_of_range("list index");auto it=values_.begin();std::advance(it,i);return *it;}
 size_t size()const{return values_.size();} void clear(){values_.clear();} void reverse(){values_.reverse();}
 void reverseRecursive(){if(values_.size()<2)return;auto first=values_.front();values_.pop_front();reverseRecursive();values_.push_back(first);}
 auto begin(){return values_.begin();}auto end(){return values_.end();}auto begin()const{return values_.begin();}auto end()const{return values_.end();}
};
template<class T> class BST {
 struct Node {T value;std::unique_ptr<Node> left,right;explicit Node(T v):value(std::move(v)){} };
 std::unique_ptr<Node> root_;size_t size_=0;
 static std::unique_ptr<Node> clone(const std::unique_ptr<Node>& n){if(!n)return {};auto p=std::make_unique<Node>(n->value);p->left=clone(n->left);p->right=clone(n->right);return p;}
 static bool add(std::unique_ptr<Node>& n,const T& x){if(!n){n=std::make_unique<Node>(x);return true;}if(x<n->value)return add(n->left,x);if(n->value<x)return add(n->right,x);return false;}
 static bool remove(std::unique_ptr<Node>& n,const T& x){if(!n)return false;if(x<n->value)return remove(n->left,x);if(n->value<x)return remove(n->right,x);if(!n->left){n=std::move(n->right);return true;}if(!n->right){n=std::move(n->left);return true;}Node* m=n->right.get();while(m->left)m=m->left.get();T v=m->value;n->value=v;remove(n->right,v);return true;}
 template<class F>static void walk(const Node* n,int mode,F& f){if(!n)return;if(mode==1)f(n->value);walk(n->left.get(),mode,f);if(mode==0)f(n->value);walk(n->right.get(),mode,f);if(mode==2)f(n->value);}
 static int h(const Node* n){return n?1+std::max(h(n->left.get()),h(n->right.get())):0;}
public:
 BST()=default;BST(const BST& x):root_(clone(x.root_)),size_(x.size_){} BST(BST&&) noexcept=default;
 BST& operator=(const BST& x){if(this!=&x){root_=clone(x.root_);size_=x.size_;}return *this;} BST& operator=(BST&&) noexcept=default;~BST()=default;
 bool insert(const T& x){bool b=add(root_,x);size_+=b;return b;}bool erase(const T& x){bool b=remove(root_,x);size_-=b;return b;}
 bool contains(const T& x)const{auto p=root_.get();while(p){if(x<p->value)p=p->left.get();else if(p->value<x)p=p->right.get();else return true;}return false;}
 size_t size()const{return size_;}int height()const{return h(root_.get());}
 template<class F>void inorder(F f)const{walk(root_.get(),0,f);}template<class F>void preorder(F f)const{walk(root_.get(),1,f);}template<class F>void postorder(F f)const{walk(root_.get(),2,f);}
 std::vector<T> values()const{std::vector<T> v;inorder([&](const T& x){v.push_back(x);});return v;}
};
template<class T>class RingBuffer {
 std::vector<std::optional<T>> data_;size_t start_=0,count_=0;
public:
 explicit RingBuffer(size_t cap):data_(cap){if(!cap)throw std::invalid_argument("capacity must be positive");}
 void push(T x){size_t i=(start_+count_)%data_.size();if(count_==data_.size()){data_[start_]=std::move(x);start_=(start_+1)%data_.size();}else{data_[i]=std::move(x);++count_;}}
 T pop(){if(!count_)throw std::out_of_range("empty ring");T x=std::move(*data_[start_]);data_[start_].reset();start_=(start_+1)%data_.size();--count_;return x;}
 const T& at(size_t i)const{if(i>=count_)throw std::out_of_range("ring index");return *data_[(start_+i)%data_.size()];}
 const T& front()const{return at(0);}const T& back()const{return at(count_-1);}size_t size()const{return count_;}size_t capacity()const{return data_.size();}bool empty()const{return !count_;}bool full()const{return count_==capacity();}
 std::vector<T> values()const{std::vector<T> v;for(size_t i=0;i<count_;++i)v.push_back(at(i));return v;}
};
template<class T>class Matrix {
 size_t rows_,cols_;std::vector<T> data_;
public:
 Matrix(size_t r,size_t c,T init=T{}):rows_(r),cols_(c),data_(r*c,init){if(!r||!c)throw std::invalid_argument("empty dimensions");}
 T& operator()(size_t r,size_t c){if(r>=rows_||c>=cols_)throw std::out_of_range("matrix index");return data_[r*cols_+c];}
 const T& operator()(size_t r,size_t c)const{if(r>=rows_||c>=cols_)throw std::out_of_range("matrix index");return data_[r*cols_+c];}
 size_t rows()const{return rows_;}size_t cols()const{return cols_;}
 Matrix transpose()const{Matrix m(cols_,rows_);for(size_t i=0;i<rows_;++i)for(size_t j=0;j<cols_;++j)m(j,i)=(*this)(i,j);return m;}
 Matrix operator*(const Matrix& b)const{if(cols_!=b.rows_)throw std::invalid_argument("matrix sizes");Matrix m(rows_,b.cols_);for(size_t i=0;i<rows_;++i)for(size_t j=0;j<b.cols_;++j)for(size_t k=0;k<cols_;++k)m(i,j)+=(*this)(i,k)*b(k,j);return m;}
 Matrix operator+(const Matrix& b)const{if(rows_!=b.rows_||cols_!=b.cols_)throw std::invalid_argument("matrix sizes");Matrix m=*this;for(size_t i=0;i<data_.size();++i)m.data_[i]+=b.data_[i];return m;}
 T determinant()const{if(rows_!=cols_)throw std::invalid_argument("not square");if(rows_==1)return data_[0];T d{};for(size_t j=0;j<cols_;++j){Matrix minor(rows_-1,cols_-1);for(size_t r=1;r<rows_;++r){size_t k=0;for(size_t c=0;c<cols_;++c)if(c!=j)minor(r-1,k++)=(*this)(r,c);}d+=(j%2?-1:1)*(*this)(0,j)*minor.determinant();}return d;}
};
template<class T,class Compare=std::less<T>>class PriorityQueue {
 std::vector<T> heap_;Compare cmp_;
 void down(size_t i){size_t n=heap_.size(),best=i,l=i*2+1,r=l+1;if(l<n&&cmp_(heap_[best],heap_[l]))best=l;if(r<n&&cmp_(heap_[best],heap_[r]))best=r;if(best!=i){std::swap(heap_[i],heap_[best]);down(best);}}
public:
 void push(T x){heap_.push_back(std::move(x));size_t i=heap_.size()-1;while(i){size_t p=(i-1)/2;if(!cmp_(heap_[p],heap_[i]))break;std::swap(heap_[p],heap_[i]);i=p;}}
 T pop(){if(heap_.empty())throw std::out_of_range("empty heap");T x=heap_.front();heap_[0]=heap_.back();heap_.pop_back();if(!heap_.empty())down(0);return x;}
 const T& top()const{if(heap_.empty())throw std::out_of_range("empty heap");return heap_.front();}size_t size()const{return heap_.size();}bool empty()const{return heap_.empty();}
};
template<class K,class V,class Hash=std::hash<K>>class HashMap {
 std::vector<std::list<std::pair<K,V>>> buckets_;size_t size_=0;Hash hash_;
 void rehash(size_t n){auto old=std::move(buckets_);buckets_.resize(n);for(auto& b:old)for(auto& p:b)buckets_[hash_(p.first)%n].push_back(std::move(p));}
public:
 explicit HashMap(size_t n=8):buckets_(n){if(!n)throw std::invalid_argument("buckets");}
 V& operator[](const K& k){auto& b=buckets_[hash_(k)%buckets_.size()];for(auto& p:b)if(p.first==k)return p.second;b.emplace_back(k,V{});++size_;if(double(size_)/buckets_.size()>.75){rehash(buckets_.size()*2);return (*this)[k];}return b.back().second;}
 bool contains(const K& k)const{auto& b=buckets_[hash_(k)%buckets_.size()];for(auto& p:b)if(p.first==k)return true;return false;}
 bool erase(const K& k){auto& b=buckets_[hash_(k)%buckets_.size()];for(auto it=b.begin();it!=b.end();++it)if(it->first==k){b.erase(it);--size_;return true;}return false;}
 size_t size()const{return size_;}size_t bucketCount()const{return buckets_.size();}double loadFactor()const{return double(size_)/buckets_.size();}
 size_t longestChain()const{size_t m=0;for(auto& b:buckets_)m=std::max(m,b.size());return m;}
 std::vector<std::pair<K,V>> entries()const{std::vector<std::pair<K,V>> v;for(auto& b:buckets_)for(auto& p:b)v.push_back(p);return v;}
};
template<class T>class Graph {
 std::map<T,std::vector<T>> adj_;
 void dfsVisit(const T& x,std::set<T>& seen,std::vector<T>& out)const{seen.insert(x);out.push_back(x);for(const auto& y:adj_.at(x))if(!seen.count(y))dfsVisit(y,seen,out);}
public:
 void addVertex(T x){adj_.try_emplace(std::move(x));}
 void addEdge(const T& a,const T& b){if(!adj_.count(a)||!adj_.count(b))throw std::out_of_range("unknown vertex");auto& x=adj_.at(a);if(std::find(x.begin(),x.end(),b)==x.end())x.push_back(b);auto& y=adj_.at(b);if(a!=b&&std::find(y.begin(),y.end(),a)==y.end())y.push_back(a);}
 std::vector<T> bfs(const T& x)const{if(!adj_.count(x))throw std::out_of_range("unknown start");std::vector<T> q{x};std::set<T> seen{x};for(size_t i=0;i<q.size();++i)for(const auto& y:adj_.at(q[i]))if(seen.insert(y).second)q.push_back(y);return q;}
 std::vector<T> dfs(const T& x)const{if(!adj_.count(x))throw std::out_of_range("unknown start");std::set<T> seen;std::vector<T> v;dfsVisit(x,seen,v);return v;}
 std::vector<T> shortest(const T& a,const T& b)const{if(!adj_.count(a))throw std::out_of_range("unknown start");std::map<T,T> parent;std::set<T> seen{a};std::queue<T> q;q.push(a);while(!q.empty()){T x=q.front();q.pop();for(const auto& y:adj_.at(x))if(seen.insert(y).second){parent[y]=x;q.push(y);}}if(!seen.count(b))return {};std::vector<T> path{b};while(path.back()!=a)path.push_back(parent.at(path.back()));std::reverse(path.begin(),path.end());return path;}
 size_t components()const{std::set<T> seen;size_t n=0;for(auto& p:adj_)if(!seen.count(p.first)){++n;for(auto& x:bfs(p.first))seen.insert(x);}return n;}
 bool hasCycle()const{std::set<T> seen;std::function<bool(const T&,const T*)> visit=[&](const T& x,const T* parent){seen.insert(x);for(auto& y:adj_.at(x)){if(y==x)return true;if(!seen.count(y)){if(visit(y,&x))return true;}else if(!parent||y!=*parent)return true;}return false;};for(auto& p:adj_)if(!seen.count(p.first)&&visit(p.first,nullptr))return true;return false;}
 size_t vertexCount()const{return adj_.size();}
};
class Trie {
 struct Node {bool terminal=false;std::map<char32_t,std::unique_ptr<Node>> children;};Node root_;size_t count_=0;
 static std::unique_ptr<Node> clone(const Node& n){auto p=std::make_unique<Node>();p->terminal=n.terminal;for(auto& x:n.children)p->children[x.first]=clone(*x.second);return p;}
 static void collect(const Node& n,std::u32string& cur,std::vector<std::u32string>& out){if(n.terminal)out.push_back(cur);for(auto& p:n.children){cur.push_back(p.first);collect(*p.second,cur,out);cur.pop_back();}}
 static bool del(Node& n,const std::u32string& s,size_t i,bool& removed){if(i==s.size()){if(n.terminal){n.terminal=false;removed=true;}}else{auto it=n.children.find(s[i]);if(it!=n.children.end()&&del(*it->second,s,i+1,removed))n.children.erase(it);}return !n.terminal&&n.children.empty();}
public:
 Trie()=default;Trie(const Trie& t):count_(t.count_){auto p=clone(t.root_);root_=std::move(*p);}Trie(Trie&&) noexcept=default;Trie& operator=(const Trie& t){if(this!=&t){Trie x(t);*this=std::move(x);}return *this;}Trie& operator=(Trie&&) noexcept=default;
 void insert(const std::u32string& s){Node* p=&root_;for(char32_t c:s){auto& next=p->children[c];if(!next)next=std::make_unique<Node>();p=next.get();}if(!p->terminal){p->terminal=true;++count_;}}
 bool contains(const std::u32string& s)const{auto p=&root_;for(auto c:s){auto it=p->children.find(c);if(it==p->children.end())return false;p=it->second.get();}return p->terminal;}
 void remove(const std::u32string& s){bool removed=false;del(root_,s,0,removed);count_-=removed;}
 std::vector<std::u32string> autocomplete(const std::u32string& s)const{auto p=&root_;for(auto c:s){auto it=p->children.find(c);if(it==p->children.end())return {};p=it->second.get();}std::vector<std::u32string> out;auto cur=s;collect(*p,cur,out);return out;}
 size_t size()const{return count_;}
};
template<class T>class Polynomial {
 std::map<int,T> terms_;
public:
 void set(int degree,T value){if(degree<0||degree>1000)throw std::out_of_range("degree");if(std::abs(value)<1e-12)terms_.erase(degree);else terms_[degree]=value;}
 Polynomial operator+(const Polynomial& p)const{Polynomial r=*this;for(auto& x:p.terms_)r.set(x.first,r.terms_[x.first]+x.second);return r;}
 Polynomial operator*(const Polynomial& p)const{Polynomial r;for(auto& a:terms_)for(auto& b:p.terms_)r.set(a.first+b.first,r.terms_[a.first+b.first]+a.second*b.second);return r;}
 Polynomial derivative()const{Polynomial r;for(auto& x:terms_)if(x.first)r.set(x.first-1,x.first*x.second);return r;}
 T evaluate(T x)const{if(terms_.empty())return T{};auto it=terms_.rbegin();int last=it->first;T v=it->second;++it;for(;it!=terms_.rend();++it){while(last>it->first){v*=x;--last;}v+=it->second;}while(last-->0)v*=x;return v;}
 const auto& terms()const{return terms_;}
};
template<class K,class V>class LRUCache {
 size_t cap_;std::list<std::pair<K,V>> order_;std::unordered_map<K,typename std::list<std::pair<K,V>>::iterator> index_;
 void rebuild(){index_.clear();for(auto it=order_.begin();it!=order_.end();++it)index_[it->first]=it;}
public:
 size_t hits=0,misses=0;explicit LRUCache(size_t cap):cap_(cap){if(!cap)throw std::invalid_argument("capacity");}
 LRUCache(const LRUCache& x):cap_(x.cap_),order_(x.order_),hits(x.hits),misses(x.misses){rebuild();}
 LRUCache(LRUCache&& x) noexcept:cap_(x.cap_),order_(std::move(x.order_)),hits(x.hits),misses(x.misses){rebuild();x.index_.clear();}
 LRUCache& operator=(const LRUCache& x){if(this!=&x){cap_=x.cap_;order_=x.order_;hits=x.hits;misses=x.misses;rebuild();}return *this;}
 LRUCache& operator=(LRUCache&& x) noexcept{if(this!=&x){cap_=x.cap_;order_=std::move(x.order_);hits=x.hits;misses=x.misses;rebuild();x.index_.clear();}return *this;}
 std::optional<V> get(const K& k){auto it=index_.find(k);if(it==index_.end()){++misses;return {}; }++hits;order_.splice(order_.begin(),order_,it->second);return it->second->second;}
 void put(K k,V v){auto it=index_.find(k);if(it!=index_.end()){it->second->second=std::move(v);order_.splice(order_.begin(),order_,it->second);return;}order_.emplace_front(std::move(k),std::move(v));index_[order_.front().first]=order_.begin();if(order_.size()>cap_){index_.erase(order_.back().first);order_.pop_back();}}
 bool contains(const K& k)const{return index_.count(k)>0;}size_t size()const{return order_.size();}size_t capacity()const{return cap_;}
 auto begin()const{return order_.begin();}auto end()const{return order_.end();}
};
}
