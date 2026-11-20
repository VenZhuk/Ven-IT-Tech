#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <functional>
#include <climits>
using boost::property_tree::ptree;
namespace fs=std::filesystem;
std::string root="../frontend";
std::string json(const ptree& p){std::ostringstream o;boost::property_tree::write_json(o,p,false);return o.str();}
ptree response(const std::string& s){ptree p;p.put("ok",true);p.put("result",s);p.put("engine","C++17");return p;}
ptree error(const std::string& s){ptree p;p.put("ok",false);p.put("error",s);return p;}
long long number(const ptree& p,const std::string& key,long long lo,long long hi){std::string s=p.get<std::string>(key,"");size_t end=0;long long n=std::stoll(s,&end);if(end!=s.size()||n<lo||n>hi)throw std::runtime_error("Некоректне число або значення поза допустимими межами: "+key);return n;}
std::string compute(const ptree& p){std::string day=p.get<std::string>("day", ""),mode=p.get<std::string>("mode", "");std::ostringstream o;
if(day=="3"){
 if(mode=="fact"){auto n=number(p,"a",0,20);unsigned long long r=1;for(int i=2;i<=n;i++)r*=i;o<<n<<"! = "<<r;}
 else if(mode=="fib"){auto n=number(p,"a",0,40),k=number(p,"b",0,40);unsigned long long a=0,b=1,f=0;std::vector<unsigned long long> seq;for(int i=0;i<=std::max(n,k-1);i++){seq.push_back(a);auto t=a+b;a=b;b=t;}f=seq[n];o<<"F("<<n<<") = "<<f<<"\nПерші "<<k<<" чисел: ";for(int i=0;i<k;i++)o<<(i?", ":"")<<seq[i];}
 else if(mode=="digits"){auto n=number(p,"a",0,LLONG_MAX);long long sum=0,t=n;do{sum+=t%10;t/=10;}while(t);o<<"Число: "<<n<<"\nСума цифр: "<<sum;}
 else if(mode=="gcd"){auto a=number(p,"a",0,LLONG_MAX),b=number(p,"b",0,LLONG_MAX);o<<"НСД("<<a<<", "<<b<<") = "<<std::gcd(a,b);}
 else if(mode=="bin"){auto n=number(p,"a",0,LLONG_MAX);std::string bits;do{bits=char('0'+n%2)+bits;n/=2;}while(n);o<<"Двійкове представлення: "<<bits;}
 else if(mode=="pow"){double a=std::stod(p.get<std::string>("a")),v=1;auto b=number(p,"b",-1000,1000);if(a==0&&b<0)throw std::runtime_error("Ділення на нуль");v=std::pow(a,b);if(!std::isfinite(v))throw std::runtime_error("Переповнення");o<<a<<" ^ "<<b<<" = "<<v;}
 else if(mode=="hanoi"){auto n=number(p,"a",1,15);unsigned long long count=(1ULL<<n)-1;o<<"Дисків: "<<n<<"\nХодів: "<<count<<"\n";std::function<void(int,char,char,char)> move=[&](int k,char a,char b,char c){if(!k)return;move(k-1,a,c,b);static unsigned long long step=0;o<<++step<<". "<<a<<" → "<<c<<"\n";move(k-1,b,a,c);};move(n,'A','B','C');}
 else if(mode=="pal"){auto s=p.get<std::string>("text","");std::string r=s;std::reverse(r.begin(),r.end());o<<"Реверс (байтовий UTF-8 для ASCII): "<<r<<"\nПаліндром: "<<(s==r?"так":"ні");}
 else throw std::runtime_error("Ця операція поки не реалізована на сервері");
}else if(day=="1"){
 auto s=p.get<std::string>("text","");if(s.empty())throw std::runtime_error("Введіть текст");
 if(mode=="stats"){int words=0;bool in=false;for(unsigned char c:s){if(std::isspace(c)){in=false;}else if(!in){words++;in=true;}}o<<"Байтів UTF-8: "<<s.size()<<"\nСлів: "<<words;}
 else if(mode=="find"){auto q=p.get<std::string>("find","");if(q.empty())throw std::runtime_error("Введіть текст пошуку");size_t pos=0,count=0;o<<"Позиції байтів (від 0): ";while((pos=s.find(q,pos))!=std::string::npos){o<<(count?", ":"")<<pos;count++;pos+=q.size();}o<<"\nЗбігів: "<<count;}
 else if(mode=="replace"){auto q=p.get<std::string>("find","");if(q.empty())throw std::runtime_error("Введіть текст пошуку");auto repl=p.get<std::string>("replace","");size_t pos=0,count=0;while((pos=s.find(q,pos))!=std::string::npos){s.replace(pos,q.size(),repl);pos+=repl.size();count++;}o<<"Замінено: "<<count<<"\nРезультат: "<<s;}
 else if(mode=="spaces"){bool prev=false;for(unsigned char c:s){if(std::isspace(c)){if(!prev)o<<' ';prev=true;}else{o<<char(c);prev=false;}}}
 else throw std::runtime_error("Ця операція поки не реалізована на сервері");
}else throw std::runtime_error("Невідомий день");return o.str();}
std::string mime(const std::string& path){if(path.ends_with(".js"))return "application/javascript; charset=utf-8";if(path.ends_with(".css"))return "text/css; charset=utf-8";if(path.ends_with(".html"))return "text/html; charset=utf-8";return "application/octet-stream";}
void serve(int fd){std::string req;char buf[8192];size_t header=std::string::npos;while((header=req.find("\r\n\r\n"))==std::string::npos&&req.size()<65536){ssize_t n=recv(fd,buf,sizeof(buf),0);if(n<=0)return;req.append(buf,n);}if(header==std::string::npos)return;std::istringstream line(req.substr(0,req.find("\r\n")));std::string method,path;line>>method>>path;size_t len=0;auto cl=req.find("Content-Length:");if(cl!=std::string::npos){try{len=std::stoul(req.substr(cl+15));}catch(...){}}if(len>1048576)return;std::string body=req.substr(header+4);while(body.size()<len){ssize_t n=recv(fd,buf,sizeof(buf),0);if(n<=0)return;body.append(buf,n);}std::string data,ct="application/json; charset=utf-8";int status=200;
try{if(method=="GET"&&path=="/api/health"){ptree p;p.put("ok",true);p.put("backend","C++17");data=json(p);}else if(method=="POST"&&path=="/api/compute"){std::istringstream in(body);ptree p;boost::property_tree::read_json(in,p);data=json(response(compute(p)));}else if(method=="GET"){if(path=="/")path="/index.html";if(path.find("..")!=std::string::npos)throw std::runtime_error("Forbidden");fs::path f=fs::path(root)/path.substr(1);if(!fs::is_regular_file(f))throw std::runtime_error("Not found");std::ifstream in(f,std::ios::binary);data.assign(std::istreambuf_iterator<char>(in),{});ct=mime(path);}else{status=405;data=json(error("Method not allowed"));}}catch(const std::exception& e){status=path.starts_with("/api/")?400:404;data=json(error(e.what()));}std::string head="HTTP/1.1 "+std::to_string(status)+(status==200?" OK":" Error")+"\r\nContent-Type: "+ct+"\r\nContent-Length: "+std::to_string(data.size())+"\r\nConnection: close\r\nAccess-Control-Allow-Origin: *\r\n\r\n";send(fd,head.data(),head.size(),0);send(fd,data.data(),data.size(),0);}
int main(int argc,char** argv){if(argc>1)root=argv[1];int s=socket(AF_INET,SOCK_STREAM,0),opt=1;setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=htonl(INADDR_LOOPBACK);a.sin_port=htons(18080);if(bind(s,(sockaddr*)&a,sizeof(a))<0){perror("bind");return 1;}listen(s,16);std::cout<<"Practice C++ backend: http://127.0.0.1:18080\n"<<std::flush;while(true){int c=accept(s,nullptr,nullptr);if(c>=0){serve(c);close(c);}}}
