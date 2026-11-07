#include <iostream>
#include <string>

using namespace std;

struct Parameters {
    string optimal;
    int n = 0;
    int count = 1;
    unsigned long long seed = 0;
    bool seedSet = false;
};

bool readInt(const string& text, int& value) {
    try {
        size_t pos;
        value = stoi(text, &pos);
        return pos == text.size();
    } catch (...) {
        return false;
    }
}

bool readSeed(const string& text, unsigned long long& value) {
    if (text.empty() || text[0] == '-') return false;

    try {
        size_t pos;
        value = stoull(text, &pos);
        return pos == text.size();
    } catch (...) {
        return false;
    }
}

bool readParameters(int argc, char* argv[], Parameters& p) {
    bool optimalSet = false;
    bool nSet = false;
    bool countSet = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg.rfind("-optimal=", 0) == 0) {
            if (optimalSet) {
                cerr << "Error: optimal is specified twice\n";
                return false;
            }
            p.optimal = arg.substr(9);
            optimalSet = true;
        } else if (arg.rfind("-n=", 0) == 0) {
            if (nSet || !readInt(arg.substr(3), p.n)) {
                cerr << "Error: incorrect n\n";
                return false;
            }
            nSet = true;
        } else if (arg.rfind("-count=", 0) == 0) {
            if (countSet || !readInt(arg.substr(7), p.count)) {
                cerr << "Error: incorrect count\n";
                return false;
            }
            countSet = true;
        } else if (arg.rfind("-seed=", 0) == 0) {
            if (p.seedSet || !readSeed(arg.substr(6), p.seed)) {
                cerr << "Error: incorrect seed\n";
                return false;
            }
            p.seedSet = true;
        } else {
            cerr << "Error: unknown parameter " << arg << '\n';
            return false;
        }
    }

    if (!optimalSet || !nSet) {
        cerr << "Error: optimal and n are required\n";
        return false;
    }
    if (p.optimal != "dc" && p.optimal != "lc" && p.optimal != "deg") {
        cerr << "Error: optimal must be dc, lc or deg\n";
        return false;
    }
    if (p.n < 3 || p.n > 8) {
        cerr << "Error: n must be from 3 to 8\n";
        return false;
    }
    if (p.count < 1) {
        cerr << "Error: count must be positive\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    Parameters p;

    if (!readParameters(argc, argv, p)) {
        cerr << "Use: program.exe -optimal=[dc|lc|deg] -n=[3..8] "
             << "[-count=k] [-seed=s]\n";
        return 1;
    }

    cout << "criterion: " << p.optimal << '\n';
    cout << "n: " << p.n << '\n';
    cout << "count: " << p.count << '\n';

    if (p.seedSet) cout << "seed: " << p.seed << '\n';
    else cout << "seed: random\n";

    return 0;
}
