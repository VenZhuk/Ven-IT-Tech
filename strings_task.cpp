#include "strings_task.h"
#include "menu_utils.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <map>
// UTF-8 decoding keeps Ukrainian letters intact when reversing or counting characters.
static std::vector<std::string> chars(const std::string& s){std::vector<std::string> a;for(size_t i=0;i<s.size();){unsigned char c=s[i];size_t n=c<128?1:(c<224?2:(c<240?3:4));n=std::min(n,s.size()-i);a.push_back(s.substr(i,n));i+=n;}return a;}
static std::string join(const std::vector<std::string>& a,const std::string& sep=""){std::string r;for(size_t i=0;i<a.size();++i){if(i)r+=sep;r+=a[i];}return r;}
static std::vector<std::string> words(const std::string& s){std::istringstream in(s);std::vector<std::string> w;std::string x;while(in>>x)w.push_back(x);return w;}
static std::string lowerUa(std::string s){auto a=chars(s);const auto upper=chars("АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯABCDEFGHIJKLMNOPQRSTUVWXYZ");const auto lower=chars("абвгґдеєжзиіїйклмнопрстуфхцчшщьюяabcdefghijklmnopqrstuvwxyz");for(auto& c:a)for(size_t i=0;i<upper.size();++i)if(c==upper[i]){c=lower[i];break;}return join(a);}
static std::string caesar(const std::string& s,int key){auto a=chars(s);const auto alph=chars("абвгґдеєжзиіїйклмнопрстуфхцчшщьюя");const auto upper=chars("АБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯ");for(auto& c:a){for(const auto* alphabet:{&alph,&upper}){auto it=std::find(alphabet->begin(),alphabet->end(),c);if(it!=alphabet->end()){int k=(int)(it-alphabet->begin());c=(*alphabet)[(k+key%33+33)%33];break;}}if(c.size()==1&&std::isalpha((unsigned char)c[0])){char b=c[0],base=(b>='a'&&b<='z')?'a':'A';c=std::string(1,(char)(base+(b-base+(key%26+26)%26)%26));}}return join(a);}
void stringsMenu(){std::string s;int c;do{std::cout<<"\n--- ТЕКСТОВИЙ ПРОЦЕСОР ---\n1. Ввести рядок\n2. Статистика\n3. Найдовше і найкоротше слово\n4. Перевернути рядок і слова\n5. Паліндром\n6. Пошук підрядка\n7. Заміна підрядка\n8. Прибрати зайві пробіли\n9. Шифр Цезаря\n0. Назад\n";c=readInt("Ваш вибір: ",0,9);if(c>1&&s.empty()){std::cout<<"Спочатку введіть рядок (пункт 1).\n";continue;}switch(c){
case 1:s=readLine("Текст: ");break;
case 2:{auto a=chars(s);int vowel=0,cons=0,digits=0,punct=0;auto v=chars("аеєиіїоуюяАЕЄИІЇОУЮЯaeiouyAEIOUY");auto letters=chars("абвгґдеєжзиіїйклмнопрстуфхцчшщьюяАБВГҐДЕЄЖЗИІЇЙКЛМНОПРСТУФХЦЧШЩЬЮЯabcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");for(auto& ch:a){if(std::find(v.begin(),v.end(),ch)!=v.end())++vowel;else if(std::find(letters.begin(),letters.end(),ch)!=letters.end())++cons;else if(ch.size()==1&&std::isdigit((unsigned char)ch[0]))++digits;else if((ch.size()==1&&std::ispunct((unsigned char)ch[0]))||ch=="…"||ch=="—"||ch=="–"||ch=="«"||ch=="»"||ch=="„"||ch=="“"||ch=="”")++punct;}std::cout<<"Символів: "<<a.size()<<", слів: "<<words(s).size()<<", голосних: "<<vowel<<", приголосних: "<<cons<<", цифр: "<<digits<<", розділових знаків ASCII: "<<punct<<"\n";break;}
case 3:{auto w=words(s);if(w.empty()){std::cout<<"Слів немає.\n";break;}auto cmp=[](const std::string&a,const std::string&b){return chars(a).size()<chars(b).size();};std::cout<<"Найкоротше: "<<*std::min_element(w.begin(),w.end(),cmp)<<"; найдовше: "<<*std::max_element(w.begin(),w.end(),cmp)<<"\n";break;}
case 4:{auto a=chars(s);std::reverse(a.begin(),a.end());auto w=words(s);std::reverse(w.begin(),w.end());std::cout<<"Навпаки: "<<join(a)<<"\nСлова навпаки: "<<join(w," ")<<"\n";break;}
case 5:{auto a=chars(lowerUa(s));std::vector<std::string> b;for(auto& ch:a)if(ch!=" "&&ch!="\t")b.push_back(ch);auto rev=b;std::reverse(rev.begin(),rev.end());std::cout<<(b==rev?"Паліндром":"Не паліндром")<<" (ігноруються пробіли та регістр)\n";break;}
case 6:{auto q=readLine("Шукати: ");size_t pos=0,n=0;while((pos=s.find(q,pos))!=std::string::npos){std::cout<<"Позиція (байт, від 0): "<<pos<<"\n";++n;pos+=q.size();}std::cout<<"Знайдено: "<<n<<"\n";break;}
case 7:{auto q=readLine("Замінити що: ");auto to=readLine("На що: ",true);size_t pos=0;while((pos=s.find(q,pos))!=std::string::npos){s.replace(pos,q.size(),to);pos+=to.size();}std::cout<<"Результат: "<<s<<"\n";break;}
case 8:s=join(words(s)," ");std::cout<<"Результат: "<<s<<"\n";break;
case 9:{int k=readInt("Ключ (-1000..1000): ",-1000,1000);int d=readInt("1 - шифрувати, 2 - розшифрувати: ",1,2);std::cout<<"Результат: "<<caesar(s,d==1?k:-k)<<"\n";break;}
}}while(c!=0);}
