#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

// ===============================
// 🔥 中文映射表（你后面可以扩展CSV）
// ===============================
unordered_map<string, string> nameMap = {
    {"0001_ASM", "电机支架装配体"},
    {"SKEL", "骨架系统"},
    {"STEP_PART", "导入零件"},
    {"STG6X20", "螺钉M6x20"}
};

// ===============================
string CN(const string& s) {
    auto it = nameMap.find(s);
    if (it != nameMap.end()) return it->second;
    return s;
}

// ===============================
// 🔥 修复 STEP Unicode（稳定版，无regex炸点）
// ===============================
string fixUnicode(string line) {

    // 简化处理：直接替换 SW/STEP Unicode标记
    size_t pos = 0;

    while ((pos = line.find("\\X2\\", pos)) != string::npos) {

        size_t end = line.find("\\X0\\", pos);
        if (end == string::npos) break;

        line.replace(pos, end - pos + 4, "中文零件");
        pos += 12;
    }

    return line;
}

// ===============================
// 🔥 STEP核心修复
// ===============================
string fixLine(string line) {

    line = fixUnicode(line);

    // ==========================
    // ① PRODUCT
    // ==========================
    size_t p1 = line.find("PRODUCT('");
    if (p1 != string::npos) {

        size_t s = p1 + 9;
        size_t e = line.find("'", s);

        if (e != string::npos) {
            string oldName = line.substr(s, e - s);
            string newName = CN(oldName);

            line.replace(s, oldName.length(), newName);
        }
    }

    // ==========================
    // ② PRODUCT_DEFINITION
    // ==========================
    if (line.find("PRODUCT_DEFINITION") != string::npos) {

        size_t q1 = line.find("'");
        if (q1 != string::npos) {
            size_t q2 = line.find("'", q1 + 1);

            if (q2 != string::npos) {
                string oldName = line.substr(q1 + 1, q2 - q1 - 1);
                string newName = CN(oldName);

                line.replace(q1 + 1, oldName.length(), newName);
            }
        }
    }

    // ==========================
    // ③ ASSEMBLY / INSTANCE（关键）
    // ==========================
    if (line.find("NEXT_ASSEMBLY_USAGE_OCCURRENCE") != string::npos) {

        size_t q1 = line.find("'");
        if (q1 != string::npos) {
            size_t q2 = line.find("'", q1 + 1);

            if (q2 != string::npos) {
                string oldName = line.substr(q1 + 1, q2 - q1 - 1);
                string newName = CN(oldName);

                line.replace(q1 + 1, oldName.length(), newName);
            }
        }
    }

    return line;
}

// ===============================
// 🔥 自动找输入文件
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
        cout << "❌ 找不到 input / input.step / input.stp" << endl;
        system("pause");
        return 1;
    }

    ifstream in(file);
    ofstream out("input_fixed.step");

    if (!in || !out) {
        cout << "❌ 文件打开失败" << endl;
        system("pause");
        return 1;
    }

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << "\n";
    }

    in.close();
    out.close();

    cout << "✅ STEP修复完成 -> input_fixed.step" << endl;
    system("pause");

    return 0;
}
