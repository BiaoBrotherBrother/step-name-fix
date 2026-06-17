#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

// ===============================
// STEP 修复核心
// ===============================
string fixLine(const string& line) {

    string out = line;

    // STEP PRODUCT 匹配
    regex productRegex("#\\d+\\s*=\\s*PRODUCT\\s*\\(\\s*'([^']*)'");

    smatch match;

    if (regex_search(line, match, productRegex)) {

        string oldName = match[1];
        string newName = oldName;

        // ===========================
        // 🔥 简单映射规则（可扩展）
        // ===========================
        if (oldName == "0001_ASM") newName = "装配体";
        else if (oldName == "SKEL") newName = "骨架";
        else if (oldName.find("STEP") != string::npos) newName = "导入模型";

        // ===========================
        // ✔ 安全字符串替换（关键修复）
        // ===========================
        size_t pos = out.find(oldName);
        if (pos != string::npos) {
            out.replace(pos, oldName.length(), newName);
        }
    }

    return out;
}

// ===============================
// 自动找输入文件
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
// 主程序
// ===============================
int main() {

    string inputFile = findInputFile();

    if (inputFile.empty()) {
        cout << "❌ 找不到 STEP 文件(input/input.step/input.stp)" << endl;
        system("pause");
        return 1;
    }

    ifstream in(inputFile);
    if (!in.is_open()) {
        cout << "❌ 文件打开失败" << endl;
        system("pause");
        return 1;
    }

    string outputFile = "input_fixed.step";
    ofstream out(outputFile);

    if (!out.is_open()) {
        cout << "❌ 输出文件创建失败" << endl;
        system("pause");
        return 1;
    }

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    in.close();
    out.close();

    cout << "✅ 完成：input_fixed.step 已生成" << endl;

    system("pause");
    return 0;
}
