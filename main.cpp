#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstdlib>

using namespace std;

// ---------- CSV ----------
unordered_map<string,string> loadMap(const string& f){
    unordered_map<string,string> m;
    ifstream in(f);
    string l;
    while(getline(in,l)){
        if(l.empty()) continue;
        auto p=l.find(',');
        if(p!=string::npos)
            m[l.substr(0,p)] = l.substr(p+1);
    }
    return m;
}

// ---------- HEX -> UTF8 ----------
string hexToUtf8(const string& hex){
    vector<uint32_t> u;
    for(size_t i=0;i+3<hex.size();i+=4)
        u.push_back(strtoul(hex.substr(i,4).c_str(),nullptr,16));

    string out;
    for(auto c:u){
        if(c<0x80) out.push_back((char)c);
        else if(c<0x800){
            out.push_back(0xC0|(c>>6));
            out.push_back(0x80|(c&0x3F));
        }else{
            out.push_back(0xE0|(c>>12));
            out.push_back(0x80|((c>>6)&0x3F));
            out.push_back(0x80|(c&0x3F));
        }
    }
    return out;
}

// ---------- STEP decode ----------
string decodeSTEP(const string& s){
    string r;
    for(size_t i=0;i<s.size();){
        if(i+3<s.size() && s[i]=='\\' && s[i+1]=='X' && s[i+2]=='2'){
            i+=4;
            string hex;
            while(i+3<s.size() && !(s[i]=='\\' && s[i+1]=='X' && s[i+2]=='0'))
                hex += s[i++];
            i+=4;
            r += hexToUtf8(hex);
        }else r += s[i++];
    }
    return r;
}

string trimQ(string s){
    if(!s.empty() && s.front()=='\'') s.erase(s.begin());
    if(!s.empty() && s.back()=='\'') s.pop_back();
    return s;
}

// ---------- PROCESS ----------
string process(const string& s, unordered_map<string,string>& mp){
    string out;
    size_t pos=0;

    while(true){
        size_t p=s.find("PRODUCT(",pos);
        if(p==string::npos){ out+=s.substr(pos); break; }

        out+=s.substr(pos,p-pos);
        size_t q=s.find(");",p);
        if(q==string::npos) break;

        string block=s.substr(p,q-p+2);

        size_t l=block.find('(');
        size_t r=block.rfind(')');
        string inner=block.substr(l+1,r-l-1);

        vector<string> f;
        string cur;
        bool inq=false;

        for(char c:inner){
            if(c=='\'') inq=!inq;
            if(c==',' && !inq){ f.push_back(cur); cur.clear(); }
            else cur+=c;
        }
        f.push_back(cur);

        if(f.size()>1){
            string name=trimQ(f[1]);
            string dec=decodeSTEP(name);

            if(mp.count(name)) f[1]="'"+mp[name]+"'";
            else if(mp.count(dec)) f[1]="'"+mp[dec]+"'";
            else f[1]="'"+dec+"'";
        }

        string rebuilt="PRODUCT(";
        for(int i=0;i<f.size();i++){
            rebuilt+=f[i];
            if(i+1<f.size()) rebuilt+=",";
        }
        rebuilt+=");";

        out+=rebuilt;
        pos=q+2;
    }
    return out;
}

// ---------- MAIN ----------
int main(int argc,char** argv){
    if(argc<3){
        cout<<"usage: step_fix input.step name_map.csv\n";
        return 0;
    }

    string inFile=argv[1];
    string mapFile=argv[2];
    string outFile=inFile.substr(0,inFile.find_last_of('.'))+"_fixed.step";

    auto mp=loadMap(mapFile);

    ifstream in(inFile,ios::binary);
    string content((istreambuf_iterator<char>(in)),{});

    string out=process(content,mp);

    ofstream o(outFile,ios::binary);
    o<<out;

    cout<<"done -> "<<outFile<<"\n";
    return 0;
}
