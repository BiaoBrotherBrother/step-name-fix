#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

// ===============================
// 🔥 STEP修复核心函数
// ===============================
string fixLine(const string& line) {

    string out = line;

    // ===============================
    // 🎯 1. 修复 PRODUCT 名称（核心）
    // ===============================
    regex productRegex("#\\d+\\s*=\\s*PRODUCT\\s*\\(\\s*'([^']*)'");

    smatch match;
    if (regex_search(line, match, productRegex)) {

        string oldName = match[1];

        // 🔥 这里是核心：后续接 CSV 映射
        string newName = oldName;

        // 示例：简单规则（你后面可以换成中文映射表）
        if (oldName == "0001_ASM") newName = "装配体";
        if (oldName == "SKEL") newName = "骨架";
        if (oldName.find("STEP") != string::npos) newName = "导入模型";

        // 替换
        out = regex_replace(line,
                            regex(oldName),
                            newName);
    }

    return out;
}

// ===============================
// 🔥 自动找输入文件
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
        cout << "❌ 找不到STEP输入文件(input/input.step/input.stp)" << endl;
        system("pause");
        return 1;
    }

    ifstream in(inputFile);
    if (!in.is_open()) {
        cout << "❌ 无法打开输入文件" << endl;
        system("pause");
        return 1;
    }

    // ===============================
    // 🔥 输出固定文件
    // ===============================
    string outputFile = "input_fixed.step";
    ofstream out(outputFile);

    if (!out.is_open()) {
        cout << "❌ 无法创建输出文件" << endl;
        system("pause");
        return 1;
    }

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    in.close();
    out.close();

    cout << "✅ STEP修复完成：" << outputFile << endl;
    system("pause");

    return 0;
}
