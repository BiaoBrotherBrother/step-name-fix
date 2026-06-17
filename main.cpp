#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string fixLine(const string& line) {
    // 示例：你后面再扩展 STEP PRODUCT 解析
    return line;
}

int main() {

    // 🔥 同时兼容 input 和 input.step
    string inputFile = "input";

    ifstream in(inputFile);

    // 如果打不开，再尝试 input.step
    if (!in.is_open()) {
        inputFile = "input.step";
        in.open(inputFile);
    }

    if (!in.is_open()) {
        cout << "❌ 找不到输入文件（input / input.step）" << endl;
        system("pause");
        return 1;
    }

    // 输出文件
    string outputFile = "input_fixed.step";
    ofstream out(outputFile);

    if (!out.is_open()) {
        cout << "❌ 无法创建输出文件" << endl;
        system("pause");
        return 1;
    }

    string line;

    while (getline(in, line)) {
        out << fixLine(line) << endl;
    }

    in.close();
    out.close();

    cout << "✅ 完成！输出：" << outputFile << endl;
    system("pause");

    return 0;
}
