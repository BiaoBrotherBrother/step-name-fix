#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

// ===============================
// 中文映射表（工程配置）
// ===============================
unordered_map<string, string> chineseMap = {
    {"ASM_001", "电机支架"},
    {"SKEL", "骨架系统"},
    {"BRKT", "支架"},
    {"GEARBOX", "减速器"}
};

// ===============================
// ASCII安全转换（SW用）
// ===============================
string toASCII(const string& s) {
    string out;

    for (char c : s) {
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_' || c == '-') {
            out += c;
        } else {
            out += '_';
        }
    }

    return out;
}

// ===============================
// STEP Unicode修复（SW专用）
// ===============================
string fixUnicode(string line) {

    size_t pos;

    while ((pos = line.find("\\X2\\")) != string::npos) {

        size_t end = line.find("\\X0\\", pos);

        if (end == string::npos) break;

        line.replace(pos, end - pos + 4, "Part");

    }

    return line;
}

// ===============================
// 提取STEP名称
// ===============================
string extractName(const string& line) {

    size_t p = line.find("'");

    if (p == string::npos) return "";

    size_t q = line.find("'", p + 1);

    if (q == string::npos) return "";

    return line.substr(p + 1, q - p - 1);
}

// ===============================
// 替换STEP字段（核心）
// ===============================
string buildTripleName(const string& id) {

    string ascii = toASCII(id);

    string chinese = id;

    if (chineseMap.count(id)) {
        chinese = chineseMap[id];
    }

    return ascii + "|" + chinese;
}

// ===============================
// 替换逻辑
// ===============================
string fixLine(string line) {

    line = fixUnicode(line);

    string name = extractName(line);

    if (!name.empty()) {

        string ascii = toASCII(name);
        string chinese = name;

        if (chineseMap.count(name))
            chinese = chineseMap[name];

        // 安全字符
        for (char &c : chinese) {
            if (c == '\'' || c == '"' || c == '\\')
                c = '_';
        }

        // ⚠️ 关键：写回 ASCII（保证SW）
        size_t p = line.find("'");

        if (p != string::npos) {
            size_t q = line.find("'", p + 1);
            if (q != string::npos) {
                line.replace(p + 1, q - p - 1, ascii);
            }
        }
    }

    return line;
}

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
        cout << "❌ 未找到STEP文件" << endl;
        return 1;
    }

    ifstream in(file);
    ofstream out("input_fixed.step");

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    in.close();
    out.close();

    cout << "✅ STEP双CAD兼容V2完成" << endl;

    return 0;
}
