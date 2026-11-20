#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <cstring>
#include <cstdint>
#include <cctype>
#include <algorithm>
using namespace std;

struct Student { int id; string name; int day, month, year, faculty; double grade; };
struct StudentRec { int32_t id; char name[50]; int32_t day, month, year, faculty; double grade; };
static_assert(sizeof(StudentRec) > 0, "record");

string trim(const string& s) { size_t a=s.find_first_not_of(" \t\r\n"), b=s.find_last_not_of(" \t\r\n"); return a==string::npos?"":s.substr(a,b-a+1); }
int readInt(const string& prompt, int lo, int hi) {
    while(true) { cout<<prompt; string s; if(!getline(cin,s)) return lo; try { size_t pos; int v=stoi(trim(s),&pos); if(pos==trim(s).size()&&v>=lo&&v<=hi)return v; }catch(...){} cout<<"Invalid number. Try again.\n"; }
}
double readGrade() { while(true) { cout<<"Average grade (0..100): "; string s; if(!getline(cin,s))return 0; try {size_t p; double x=stod(trim(s),&p); if(p==trim(s).size()&&x>=0&&x<=100)return x;}catch(...){} cout<<"Invalid grade.\n"; } }
string readLine(const string& prompt) { cout<<prompt; string s; getline(cin,s); return s; }
bool leap(int y){return y%400==0||(y%4==0&&y%100!=0);}
bool valid(const Student& s){ int md[]={0,31,28+(leap(s.year)?1:0),31,30,31,30,31,31,30,31,30,31}; return s.id>0&&!s.name.empty()&&s.name.find(';')==string::npos&&s.day>=1&&s.month>=1&&s.month<=12&&s.year>=1900&&s.year<=2100&&s.day<=md[s.month]&&s.faculty>=0&&s.faculty<=9999&&s.grade>=0&&s.grade<=100; }
Student inputStudent(){ Student s; while(true){s.id=readInt("ID (1..1000000): ",1,1000000);s.name=trim(readLine("Full name (max 49 bytes for binary): "));s.day=readInt("Day: ",1,31);s.month=readInt("Month: ",1,12);s.year=readInt("Year: ",1900,2100);s.faculty=readInt("Faculty number (0..9999): ",0,9999);s.grade=readGrade();if(valid(s)&&s.name.size()<50)return s;cout<<"Invalid student data or name too long. Retry.\n";} }
string toText(const Student& s){ostringstream o;o<<s.id<<';'<<s.name<<';'<<setfill('0')<<setw(2)<<s.day<<'.'<<setw(2)<<s.month<<'.'<<setw(4)<<s.year<<';'<<s.faculty<<';'<<fixed<<setprecision(2)<<s.grade;return o.str();}
bool parseText(const string& line, Student& s){ try {vector<string> f;istringstream in(line);string t;while(getline(in,t,';'))f.push_back(trim(t));if(f.size()!=5)return false;size_t p=0;s.id=stoi(f[0],&p);if(p!=f[0].size())return false;s.name=f[1];int d,m,y;char c1,c2;istringstream date(f[2]);if(!(date>>d>>c1>>m>>c2>>y)||c1!='.'||c2!='.')return false;string rest;if(date>>rest)return false;s.day=d;s.month=m;s.year=y;s.faculty=stoi(f[3],&p);if(p!=f[3].size())return false;s.grade=stod(f[4],&p);return p==f[4].size()&&valid(s)&&s.name.size()<50; }catch(...){return false;} }
StudentRec pack(const Student& s){StudentRec r{};r.id=s.id;memcpy(r.name,s.name.data(),min(s.name.size(),sizeof(r.name)-1));r.day=s.day;r.month=s.month;r.year=s.year;r.faculty=s.faculty;r.grade=s.grade;return r;}
bool unpack(const StudentRec& r,Student& s){if(memchr(r.name,'\0',sizeof(r.name))==nullptr)return false;s={r.id,string(r.name),r.day,r.month,r.year,r.faculty,r.grade};return valid(s);}
void show(const vector<Student>& a){if(a.empty()){cout<<"List is empty.\n";return;}for(size_t i=0;i<a.size();++i)cout<<i+1<<") "<<toText(a[i])<<'\n';}
bool saveText(const string& path,const vector<Student>& a){ofstream out(path);if(!out.is_open()){cout<<"Cannot open text file for writing.\n";return false;}for(const auto& s:a)out<<toText(s)<<'\n';if(!out){cout<<"Text write error.\n";return false;}cout<<"Saved "<<a.size()<<" records.\n";return true;}
void loadText(const string& path,vector<Student>& a){ifstream in(path);if(!in.is_open()){cout<<"Cannot open text file.\n";return;}vector<Student> tmp;string line;int n=0;while(getline(in,line)){++n;Student s;if(parseText(line,s))tmp.push_back(s);else cout<<"Skipping damaged line "<<n<<".\n";}if(in.bad()){cout<<"Text read error; list unchanged.\n";return;}a=move(tmp);cout<<"Loaded "<<a.size()<<" records.\n";}
void appendText(const string& path,vector<Student>& a){Student s=inputStudent();ofstream out(path,ios::app);if(!out.is_open()){cout<<"Cannot open file for append.\n";return;}out<<toText(s)<<'\n';if(!out){cout<<"Append error.\n";return;}a.push_back(s);cout<<"Appended.\n";}
void stats(const string& path){ifstream in(path);if(!in.is_open()){cout<<"Cannot open text file.\n";return;}size_t lines=0,words=0,chars=0;string line;while(getline(in,line)){++lines;chars+=line.size();istringstream ss(line);string word;while(ss>>word)++words;}if(in.bad()){cout<<"Read error.\n";return;}cout<<"Lines: "<<lines<<", words: "<<words<<", characters (excluding newlines): "<<chars<<'\n';}
void searchWord(const string& path){string term=readLine("Word to find: ");if(term.empty()){cout<<"Empty search term.\n";return;}ifstream in(path);if(!in.is_open()){cout<<"Cannot open text file.\n";return;}string line;int n=0,hits=0;while(getline(in,line)){++n;istringstream ss(line);string token;bool found=false;while(ss>>token)if(token==term)found=true;if(found){cout<<"Line "<<n<<'\n';++hits;}}cout<<"Matching lines: "<<hits<<'\n';}
bool saveBinary(const string& path,const vector<Student>& a){ofstream out(path,ios::binary|ios::trunc);if(!out.is_open()){cout<<"Cannot open binary file for writing.\n";return false;}for(const auto& s:a){StudentRec r=pack(s);out.write(reinterpret_cast<const char*>(&r),sizeof(r));}if(!out){cout<<"Binary write error.\n";return false;}cout<<"Saved "<<a.size()<<" binary records.\n";return true;}
bool countBinary(ifstream& in,size_t& count){in.seekg(0,ios::end);streamoff bytes=in.tellg();if(bytes<0||bytes%static_cast<streamoff>(sizeof(StudentRec))!=0){cout<<"Damaged binary file size.\n";return false;}count=static_cast<size_t>(bytes/sizeof(StudentRec));in.seekg(0,ios::beg);return true;}
void readBinary(const string& path){ifstream in(path,ios::binary);if(!in.is_open()){cout<<"Cannot open binary file.\n";return;}size_t n;if(!countBinary(in,n))return;for(size_t i=0;i<n;++i){StudentRec r{};if(!in.read(reinterpret_cast<char*>(&r),sizeof(r))){cout<<"Read error.\n";return;}Student s;if(unpack(r,s))cout<<i+1<<") "<<toText(s)<<'\n';else cout<<"Damaged record "<<i+1<<'\n';}cout<<"Total records: "<<n<<'\n';}
void readAt(const string& path){ifstream in(path,ios::binary);if(!in.is_open()){cout<<"Cannot open binary file.\n";return;}size_t n;if(!countBinary(in,n)||n==0){cout<<"No readable records.\n";return;}int k=readInt("Record number (starting at 1): ",1,static_cast<int>(min(n,static_cast<size_t>(1000000))));in.seekg(static_cast<streamoff>(k-1)*sizeof(StudentRec),ios::beg);StudentRec r{};if(!in.read(reinterpret_cast<char*>(&r),sizeof(r))){cout<<"Read error.\n";return;}Student s;if(unpack(r,s))cout<<toText(s)<<'\n';else cout<<"Damaged record.\n";}
void editAt(const string& path){ifstream check(path,ios::binary);if(!check.is_open()){cout<<"Cannot open binary file.\n";return;}size_t n;if(!countBinary(check,n)||n==0){cout<<"No readable records.\n";return;}check.close();int k=readInt("Record number to replace: ",1,static_cast<int>(min(n,static_cast<size_t>(1000000))));Student s=inputStudent();fstream io(path,ios::binary|ios::in|ios::out);if(!io.is_open()){cout<<"Cannot open binary file for editing.\n";return;}io.seekp(static_cast<streamoff>(k-1)*sizeof(StudentRec),ios::beg);StudentRec r=pack(s);io.write(reinterpret_cast<const char*>(&r),sizeof(r));cout<<(io?"Record updated.\n":"Write error.\n");}
void compare(const string& txt,const string& bin){ifstream a(txt,ios::binary|ios::ate),b(bin,ios::binary|ios::ate);if(!a.is_open()||!b.is_open()){cout<<"Both files must exist. Save the same student list to both first.\n";return;}cout<<"Text bytes: "<<a.tellg()<<", binary bytes: "<<b.tellg()<<'\n';}
int main(){
#ifdef _WIN32
    // UTF-8 console: run `chcp 65001` in cmd if Cyrillic text is entered.
#endif
    vector<Student> students;string txt="students.txt",bin="students.bin";
    cout<<"DAY 4 - TEXT AND BINARY FILES\n";
    while(true){cout<<"\nWorking files: "<<txt<<" | "<<bin<<"\n0 Exit | 1 Select files | 2 Add student to memory | 3 Show students | 4 Text submenu | 5 Binary submenu\n";int c=readInt("Choice: ",0,5);if(c==0)break;
        if(c==1){string t=trim(readLine("Text file path: ")),b=trim(readLine("Binary file path: "));if(!t.empty())txt=t;if(!b.empty())bin=b;}
        if(c==2){students.push_back(inputStudent());cout<<"Student added.\n";}
        if(c==3)show(students);
        if(c==4)while(true){cout<<"\nTEXT: 1 Save | 2 Load | 3 Append | 4 Statistics | 5 Find word | 0 Back\n";int x=readInt("Choice: ",0,5);if(!x)break;if(x==1)saveText(txt,students);if(x==2)loadText(txt,students);if(x==3)appendText(txt,students);if(x==4)stats(txt);if(x==5)searchWord(txt);}
        if(c==5)while(true){cout<<"\nBINARY: 6 Save | 7 Read all | 8 Read by number | 9 Edit by number | 10 Compare sizes | 11 Back\n";int x=readInt("Choice: ",6,11);if(x==11)break;if(x==6)saveBinary(bin,students);if(x==7)readBinary(bin);if(x==8)readAt(bin);if(x==9)editAt(bin);if(x==10)compare(txt,bin);}
    }
    return 0;
}
