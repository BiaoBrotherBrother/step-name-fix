#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

// ===============================
// 🧠 中文映射表（可扩展）
// ===============================
unordered_map<string, string> nameMap = {
    {"0001_ASM", "电机支架装配体"},
    {"SKEL", "骨架系统"},
    {"STG6X20", "螺钉M6x20"},
    {"STG6X20-1", "螺钉M6x20-1"},
    {"FR_THIGH", "左大腿"},
    {"FL_THIGH", "右大腿"}
};

// ===============================
// 🚨 STEP非法字符清理（Creo/SW安全）
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
// 🚨 SolidWorks Unicode修复 (\X2\)
// ===============================
string fixSWUnicode(string line) {

    size_t start = 0;

    while ((start = line.find("\\X2\\")) != string::npos) {

        size_t end = line.find("\\X0\\", start);

        if (end == string::npos) break;

        // 👉 提取中间内容
        string block = line.substr(start, end - start + 4);

        // 👉 简化处理（工程安全策略）
        line.replace(start, end - start + 4, "中文零件");

        start += 6;
    }

    return line;
}

// ===============================
// 🚨 提取 STEP 名称（简单安全版）
// ===============================
string extractName(string line) {

    size_t p = line.find("'");

    if (p == string::npos) return "";

    size_t q = line.find("'", p + 1);

    if (q == string::npos) return "";

    return line.substr(p + 1, q - p - 1);
}

// ===============================
// 🚨 替换 STEP 名称
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
// 🚀 核心修复逻辑
// ===============================
string fixLine(string line) {

    // ① SW Unicode修复
    line = fixSWUnicode(line);

    // ② 提取旧名字
    string name = extractName(line);

    if (!name.empty()) {

        // ③ 映射中文
        string newName = name;

        if (nameMap.count(name)) {
            newName = nameMap[name];
        }

        // ④ 安全字符过滤
        newName = sanitize(newName);

        // ⑤ 替换回 STEP
        line = replaceName(line, newName);
    }

    return line;
}

// ===============================
// 🚀 输入文件自动识别
// ===============================
string findInputFile() {

    ifstream f1("input");
    if (f1.good()) return "input";

    ifstream f2("input.step");
    if (f2.good()) return "input.step";

    ifstream f3("input.stp");
    if (f3.good()) return "input.stp";

    return "";
}

// ===============================
// 🚀 MAIN
// ===============================
int main() {

    string file = findInputFile();

    if (file.empty()) {
        cout << "❌ 未找到 STEP 文件 (input / input.step / input.stp)" << endl;
        return 1;
    }

    ifstream in(file);

    if (!in.is_open()) {
        cout << "❌ 文件打开失败" << endl;
        return 1;
    }

    string outputFile = "input_fixed.step";
    ofstream out(outputFile);

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

    cout << "🚀 完成：STEP修复成功 -> " << outputFile << endl;

    return 0;
}
