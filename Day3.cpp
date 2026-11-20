#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <system_error>
#include <limits>
#include <cmath>
#include <cstdint>
using namespace std;
namespace fs = std::filesystem;
// Base: n <= 1. Recursive step: n * factorial(n - 1).
uint64_t factorial(int n) { return n <= 1 ? 1ULL : (uint64_t)n * factorial(n - 1); }
uint64_t factorialIter(int n) { uint64_t r=1; for(int i=2;i<=n;i++) r*=i; return r; }
// Base: n <= 1. Recursive step: fib(n-1) + fib(n-2).
uint64_t fib(int n, uint64_t &calls) { ++calls; if(n<=1) return n; return fib(n-1,calls)+fib(n-2,calls); }
uint64_t fibIter(int n) { if(n<=1) return n; uint64_t a=0,b=1; for(int i=2;i<=n;i++){uint64_t c=a+b;a=b;b=c;}return b; }
// Base: n < 10. Recursive step: last digit + digitSum(n/10).
uint64_t digitSum(uint64_t n) { return n<10 ? n : n%10+digitSum(n/10); }
// Base: exponent 0. Recursive step: exponentiation by squaring.
long double power(long double x, unsigned n) { if(n==0) return 1; long double half=power(x,n/2); return n%2 ? half*half*x : half*half; }
// Base: b == 0. Recursive step: gcd(b,a%b).
uint64_t gcdRec(uint64_t a,uint64_t b) { return b==0 ? a : gcdRec(b,a%b); }
// Base: left >= right. Recursive step: swap endpoints and recurse inward.
void reverseRec(string &s,size_t l,size_t r) { if(l>=r)return; swap(s[l],s[r]); reverseRec(s,l+1,r-1); }
// Base: left >= right. Recursive step: compare endpoints and recurse inward.
bool palindromeRec(const string &s,size_t l,size_t r) { if(l>=r)return true; return s[l]==s[r] && palindromeRec(s,l+1,r-1); }
// Base: n < 2. Recursive step: convert n/2, append n%2.
string binaryRec(uint64_t n) { if(n<2)return to_string(n); return binaryRec(n/2)+char('0'+n%2); }
// Base: n == 0. Recursive step: move n-1, move largest, move n-1.
void hanoi(int n,char from,char aux,char to,uint64_t &moves) { if(n==0)return; hanoi(n-1,from,to,aux,moves); cout<<"Move disk "<<n<<": "<<from<<" -> "<<to<<'\n'; ++moves; hanoi(n-1,aux,from,to,moves); }
// Base: depth limit or inaccessible directory. Recursive step: visit each subdirectory.
void treeRec(const fs::path &p,int depth,int maxDepth,uint64_t &count) {
if(depth>=maxDepth)return;
error_code ec;
fs::directory_iterator it(p,fs::directory_options::skip_permission_denied,ec), end;
if(ec){cout<<string(depth*2,' ')<<"[Cannot access: "<<ec.message()<<"]\n";return;}
for(;it!=end;it.increment(ec)){
if(ec){cout<<"[Directory read error: "<<ec.message()<<"]\n";break;}
if(++count>2000){cout<<"[Entry limit reached]\n";return;}
auto item=*it; cout<<string(depth*2,' ')<<"|-- "<<item.path().filename().string()<<'\n';
error_code typeEc;
if(item.is_directory(typeEc) && !item.is_symlink(typeEc))treeRec(item.path(),depth+1,maxDepth,count);
if(count>2000)return;
}
}
int readInt(const string &prompt,int low,int high) {
while(true){cout<<prompt;long long n; if(cin>>n && n>=low && n<=high)return (int)n;
cout<<"Invalid input. Enter a number from "<<low<<" to "<<high<<".\n";
cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
}
uint64_t readUnsigned(const string &prompt){
while(true){cout<<prompt;string s;cin>>s;
if(s.empty()||s[0]=='-'){cout<<"Enter a non-negative integer.\n";continue;}
try {size_t pos;auto x=stoull(s,&pos);if(pos==s.size())return x;}catch(...){}
cout<<"Invalid unsigned 64-bit integer.\n";
}
}
int main(){
cout<<fixed<<setprecision(6);
while(true){
cout<<"\n=== DAY 3: RECURSION ===\n"
<<"1. Factorial\n2. Fibonacci\n3. Sum of digits\n4. Power\n"
<<"5. GCD (Euclid)\n6. Reverse / palindrome\n7. Decimal to binary\n"
<<"8. Tower of Hanoi\n9. Directory tree\n10. Back / Exit\n";
int choice=readInt("Choice: ",1,10);
if(choice==10)break;
switch(choice){
case 1:{int n=readInt("n (0..20): ",0,20);cout<<"Recursive: "<<factorial(n)<<"; iterative: "<<factorialIter(n)<<'\n';break;}
case 2:{int n=readInt("Index n (0..40): ",0,40);uint64_t calls=0;
auto a=chrono::steady_clock::now();auto value=fib(n,calls);auto b=chrono::steady_clock::now();
auto iterative=fibIter(n);auto c=chrono::steady_clock::now();
cout<<"F("<<n<<") = "<<value<<"; calls = "<<calls<<'\n';
cout<<"Naive recursion: "<<chrono::duration<double,milli>(b-a).count()<<" ms\n";
cout<<"Iteration: "<<chrono::duration<double,milli>(c-b).count()<<" ms; result "<<iterative<<'\n';
int k=readInt("Print first k Fibonacci numbers (0..40): ",0,40);
for(int i=0;i<k;i++){cout<<fibIter(i)<<(i+1==k?'\n':' ');} break;}
case 3:{auto n=readUnsigned("Non-negative integer: ");cout<<"Sum = "<<digitSum(n)<<'\n';break;}
case 4:{long double x;cout<<"Base x: ";while(!(cin>>x)||!isfinite(x)){cout<<"Invalid base. Try again: ";cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
int n=readInt("Integer exponent (-1000..1000): ",-1000,1000);
if(x==0&&n<0){cout<<"Undefined: division by zero.\n";break;}
long double result=n<0?1.0L/power(x,(unsigned)(-n)):power(x,(unsigned)n);
if(!isfinite(result))cout<<"Overflow: result cannot be represented.\n";
else {cout<<"Result = "<<result<<'\n';} break;}
case 5:{auto a=readUnsigned("a: "),b=readUnsigned("b: ");cout<<"GCD = "<<gcdRec(a,b)<<'\n';break;}
case 6:{cout<<"Text (up to 1000 characters): ";cin.ignore(numeric_limits<streamsize>::max(),'\n');string s;getline(cin,s);if(s.size()>1000){cout<<"Text too long.\n";break;}
cout<<"Palindrome (exact character match): "<<(palindromeRec(s,0,s.size()?s.size()-1:0)?"yes":"no")<<'\n';
if(!s.empty()){reverseRec(s,0,s.size()-1);} cout<<"Reversed: "<<s<<'\n';break;}
case 7:{auto n=readUnsigned("Decimal integer: ");cout<<"Binary: "<<binaryRec(n)<<'\n';break;}
case 8:{int n=readInt("Number of disks (1..15): ",1,15);uint64_t moves=0;hanoi(n,'A','B','C',moves);cout<<"Total moves: "<<moves<<'\n';break;}
case 9:{cout<<"Directory path: ";cin.ignore(numeric_limits<streamsize>::max(),'\n');string s;getline(cin,s);fs::path p(s);error_code ec;
if(!fs::is_directory(p,ec)){cout<<"Directory does not exist or is inaccessible.\n";break;}
int depth=readInt("Maximum depth (1..8): ",1,8);uint64_t count=0;cout<<p.string()<<'\n';treeRec(p,0,depth,count);cout<<"Entries visited: "<<min<uint64_t>(count,2000)<<'\n';break;}
}
}
return 0;
}
