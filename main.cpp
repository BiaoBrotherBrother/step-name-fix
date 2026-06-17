#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

// ===============================
// 🔥 中文映射（可扩展CSV）
// ===============================
unordered_map<string, string> nameMap = {
    {"0001_ASM", "电机支架装配体"},
    {"SKEL", "骨架系统"},
    {"STG6X20", "螺钉M6x20"},
    {"FR_THIGH", "左大腿"},
    {"FL_THIGH", "右大腿"}
};

// ===============================
// 🔥 安全字符过滤（Creo / SW兼容）
// ===============================
string sanitize(const string& s) {

    string out;

    for (char c : s) {
        switch (c) {
            case '\'':
            case '"':
            case '#':
            case ';':
            case ',':
            case '\\':
            case '(':
            case ')':
            case '\t':
            case '\r':
            case '\n':
                out += '_';
                break;
            default:
                out += c;
        }
    }

    return out;
}

// ===============================
// 🔥 SW STEP Unicode修复
// ===============================
string fixUnicode(string line) {

    size_t pos = 0;

    while ((pos = line.find("\\X2\\")) != string::npos) {

        size_t end = line.find("\\X0\\", pos);
        if (end == string::npos) break;

        // 👉 工业安全处理（不破坏结构）
        line.replace(pos, end - pos + 4, "零件");

        pos += 6;
    }

    return line;
}

// ===============================
// 🔥 名称映射
// ===============================
string mapName(const string& name) {

    auto it = nameMap.find(name);
    if (it != nameMap.end())
        return it->second;

    return name;
}

// ===============================
// 🔥 FINAL STEP修复核心
// ===============================
string fixLine(string line) {

    // ① Unicode安全修复
    line = fixUnicode(line);

    // ② 查找 NAME（只处理第一个 'xxx'）
    size_t p = line.find("'");

    if (p != string::npos) {

        size_t q = line.find("'", p + 1);

        if (q != string::npos) {

            string oldName = line.substr(p + 1, q - p - 1);

            string newName = sanitize(mapName(oldName));

            line.replace(p + 1, oldName.length(), newName);
        }
    }

    return line;
}

// ===============================
// 🔥 自动识别输入文件
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
// 🔥 MAIN
// ===============================
int main() {

    string file = findInput();

    if (file.empty()) {
        cout << "❌ 找不到 STEP 文件 (input / input.step / input.stp)" << endl;
        return 1;
    }

    ifstream in(file);

    if (!in.is_open()) {
        cout << "❌ 文件打开失败" << endl;
        return 1;
    }

    ofstream out("input_fixed.step");

    if (!out.is_open()) {
        cout << "❌ 输出文件创建失败" << endl;
        return 1;
    }

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    in.close();
    out.close();

    cout << "🚀 FINAL版完成：STEP安全修复成功 → input_fixed.step" << endl;

    return 0;
}
