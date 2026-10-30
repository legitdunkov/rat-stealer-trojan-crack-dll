#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Error: use program.exe input.txt\n";
        return 1;
    }

    ifstream in(argv[1]);
    if (!in) {
        cerr << "Error: cannot open input file\n";
        return 1;
    }

    cout << "Input file opened successfully\n";

    return 0;
}
