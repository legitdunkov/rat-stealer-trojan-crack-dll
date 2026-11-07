#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int ves(int x) {
    int result = 0;

    while (x > 0) {
        result += x & 1;
        x >>= 1;
    }

    return result;
}

int scal(int a, int b) {
    return ves(a & b) % 2;
}

void BPF(vector<int>& f) {
    int N = f.size();

    for (int len = 1; len < N; len *= 2) {
        for (int i = 0; i < N; i += 2 * len) {
            for (int j = 0; j < len; ++j) {
                int a = f[i + j];
                int b = f[i + j + len];
                f[i + j] = a + b;
                f[i + j + len] = a - b;
            }
        }
    }
}

bool isPodstanovka(const vector<int>& S) {
    int N = S.size();
    vector<int> used(N, 0);

    for (int value : S) {
        if (value < 0 || value >= N || used[value]) return false;
        used[value] = 1;
    }

    return true;
}

int deltaS(const vector<int>& S) {
    int N = S.size();
    int delta = 0;

    for (int a = 1; a < N; ++a) {
        vector<int> count(N, 0);

        for (int x = 0; x < N; ++x) {
            int b = S[x] ^ S[x ^ a];
            count[b]++;
        }

        for (int value : count) delta = max(delta, value);
    }

    return delta;
}

int lambdaS(const vector<int>& S) {
    int N = S.size();
    int lambda = 0;

    for (int b = 1; b < N; ++b) {
        vector<int> f(N);

        for (int x = 0; x < N; ++x) {
            f[x] = scal(b, S[x]) ? -1 : 1;
        }

        BPF(f);
        for (int value : f) lambda = max(lambda, abs(value));
    }

    return lambda;
}

int stepenF(const vector<int>& S, int b, int n) {
    int N = S.size();
    vector<int> anf(N);

    for (int x = 0; x < N; ++x) anf[x] = scal(b, S[x]);

    for (int i = 0; i < n; ++i) {
        for (int mask = 0; mask < N; ++mask) {
            if (mask & (1 << i)) {
                anf[mask] ^= anf[mask ^ (1 << i)];
            }
        }
    }

    int degree = 0;
    for (int mask = 0; mask < N; ++mask) {
        if (anf[mask]) degree = max(degree, ves(mask));
    }

    return degree;
}

int stepenS(const vector<int>& S, int n) {
    int degree = 0;

    for (int i = 0; i < n; ++i) {
        degree = max(degree, stepenF(S, 1 << i, n));
    }

    return degree;
}

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
