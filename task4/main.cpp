#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

struct Parameters {
    string construction;
    int n = 0;
    int polynomial = 0x11B;
    unsigned long long seed = 0;
    bool seedSet = false;
    bool involutory = false;
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

bool readHex(const string& text, int& value) {
    if (text.empty() || text[0] == '-') return false;

    try {
        size_t pos;
        value = stoi(text, &pos, 16);
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
    bool constructionSet = false;
    bool nSet = false;
    bool polySet = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg.rfind("-construction=", 0) == 0) {
            if (constructionSet) {
                cerr << "Error: construction is specified twice\n";
                return false;
            }
            p.construction = arg.substr(14);
            constructionSet = true;
        } else if (arg.rfind("-n=", 0) == 0) {
            if (nSet || !readInt(arg.substr(3), p.n)) {
                cerr << "Error: incorrect n\n";
                return false;
            }
            nSet = true;
        } else if (arg.rfind("-poly=", 0) == 0) {
            if (polySet || !readHex(arg.substr(6), p.polynomial)) {
                cerr << "Error: incorrect polynomial\n";
                return false;
            }
            polySet = true;
        } else if (arg.rfind("-seed=", 0) == 0) {
            if (p.seedSet || !readSeed(arg.substr(6), p.seed)) {
                cerr << "Error: incorrect seed\n";
                return false;
            }
            p.seedSet = true;
        } else if (arg == "-involutory") {
            if (p.involutory) {
                cerr << "Error: involutory is specified twice\n";
                return false;
            }
            p.involutory = true;
        } else {
            cerr << "Error: unknown parameter " << arg << '\n';
            return false;
        }
    }

    if (!constructionSet || !nSet) {
        cerr << "Error: construction and n are required\n";
        return false;
    }
    if (p.construction != "circl" && p.construction != "ffh" &&
        p.construction != "cauchi") {
        cerr << "Error: construction must be circl, ffh or cauchi\n";
        return false;
    }
    if (p.n < 4 || p.n > 8) {
        cerr << "Error: n must be from 4 to 8\n";
        return false;
    }
    if (p.polynomial < 0x100 || p.polynomial > 0x1FF) {
        cerr << "Error: polynomial degree must be 8\n";
        return false;
    }
    if (p.involutory &&
        (p.construction != "ffh" || (p.n != 4 && p.n != 8))) {
        cerr << "Error: involutory is allowed only for ffh with n=4 or n=8\n";
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    Parameters p;

    if (!readParameters(argc, argv, p)) {
        cerr << "Use: program.exe -construction=[circl|ffh|cauchi] "
             << "-n=[4..8] [-poly=p] [-seed=s] [-involutory]\n";
        return 1;
    }

    cout << "construction: " << p.construction << '\n';
    cout << "n: " << p.n << '\n';
    cout << "polynomial: " << uppercase << hex << p.polynomial << dec << '\n';

    if (p.seedSet) cout << "seed: " << p.seed << '\n';
    else cout << "seed: random\n";

    cout << "involutory: " << (p.involutory ? "yes" : "no") << '\n';
    return 0;
}
