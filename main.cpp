#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

// ===============================
// 中文映射（可选）
// ===============================
unordered_map<string, string> nameMap = {
    {"ASM_001", "电机支架"},
    {"SKEL", "骨架"},
    {"BRACKET", "支架"},
    {"GEARBOX", "减速器"}
};

// ===============================
// STEP非法字符清理
// ===============================
string sanitize(string s) {
    string out;
    for (char c : s) {
        if (c=='\'' || c=='"' || c=='#' || c==';' || c==',' ||
            c=='\\' || c=='(' || c==')' || c=='\t' || c=='\r' || c=='\n')
            out += '_';
        else
            out += c;
    }
    return out;
}

// ===============================
// SW Unicode修复
// ===============================
string fixUnicode(string line) {

    size_t p;

    while ((p = line.find("\\X2\\")) != string::npos) {

        size_t q = line.find("\\X0\\", p);

        if (q == string::npos) break;

        line.replace(p, q - p + 4, "Part");
    }

    return line;
}

// ===============================
// 提取 STEP 名称
// ===============================
string getName(const string& line) {

    size_t p = line.find("'");

    if (p == string::npos) return "";

    size_t q = line.find("'", p + 1);

    if (q == string::npos) return "";

    return line.substr(p + 1, q - p - 1);
}

// ===============================
// 替换名称
// ===============================
string replaceName(string line, const string& newName) {

    size_t p = line.find("'");

    if (p == string::npos) return line;

    size_t q = line.find("'", p + 1);

    if (q == string::npos) return line;

    line.replace(p + 1, q - p - 1, newName);

    return line;
}

// ===============================
// 核心逻辑
// ===============================
string fixLine(string line) {

    line = fixUnicode(line);

    string name = getName(line);

    if (!name.empty()) {

        string newName = name;

        if (nameMap.count(name))
            newName = nameMap[name];

        newName = sanitize(newName);

        line = replaceName(line, newName);
    }

    return line;
}

// ===============================
// 找输入文件
// ===============================
string findInput() {

    ifstream f1("input");
    if (f1.good()) return "input";

    ifstream f2("input.step");
    if (f2.good()) return "input.step";

    ifstream f3("input.stp");
    if (f3.good()) return "input.stp";

    return "";
}

// ===============================
int main() {

    string file = findInput();

    if (file.empty()) {
        cout << "❌ 找不到STEP文件(input/input.step/input.stp)" << endl;
        return 1;
    }

    ifstream in(file);
    ofstream out("input_fixed.step");

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    cout << "✅ 完成：input_fixed.step 已生成" << endl;

    return 0;
}
