#include <bits/stdc++.h>
using namespace std;

// ================= CSV MAP =================
map<string,string> loadMap(const string& file){
    map<string,string> mp;
    ifstream in(file);
    string line;

    while(getline(in,line)){
        if(line.find(',')==string::npos) continue;

        string a=line.substr(0,line.find(','));
        string b=line.substr(line.find(',')+1);

        mp[a]=b;
    }
    return mp;
}

// ================= STEP UTF16 decode =================
string decodeSTEP(const string& s){
    string out;

    for(size_t i=0;i<s.size();){
        if(i+2<s.size() && s[i]=='\\' && s[i+1]=='X' && s[i+2]=='2'){
            i+=4;

            string hexStr;

            while(i+3<s.size() && !(s[i]=='\\' && s[i+1]=='X')){
                hexStr.push_back(s[i++]);
            }

            i+=4; // skip \X0\

            for(size_t j=0;j+3<hexStr.size(); j+=4){
                string h = hexStr.substr(j,4);
                int val = stoi(h,nullptr,16);

                if(val < 0x80){
                    out.push_back(val);
                }
                else if(val < 0x800){
                    out.push_back(0xC0 | (val>>6));
                    out.push_back(0x80 | (val & 0x3F));
                }
                else{
                    out.push_back(0xE0 | (val>>12));
                    out.push_back(0x80 | ((val>>6)&0x3F));
                    out.push_back(0x80 | (val&0x3F));
                }
            }
        }
        else{
            out.push_back(s[i++]);
        }
    }

    return out;
}

// ================= replace =================
string process(const string& s, map<string,string>& mp){
    string out = s;

    for(auto &kv: mp){
        size_t pos = 0;

        while((pos = out.find(kv.first, pos)) != string::npos){
            cout << "replace: " << kv.first << " -> " << kv.second << endl;
            out.replace(pos, kv.first.size(), kv.second);
            pos += kv.second.size();
        }
    }

    return decodeSTEP(out);
}

// ================= main =================
int main(int argc, char** argv){

    if(argc < 3){
        cout << "usage: step_fix input.step map.csv\n";
        return 0;
    }

    string file = argv[1];
    string mapfile = argv[2];

    auto mp = loadMap(mapfile);

    ifstream in(file, ios::binary);
    string content((istreambuf_iterator<char>(in)), {});

    string out = process(content, mp);

    ofstream o("input_fixed.step", ios::binary);
    o << out;

    cout << "done -> input_fixed.step" << endl;

    return 0;
}
