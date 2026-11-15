#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <random>
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

int fieldPolynomial(int n) {
    int polynomials[] = {0, 0, 0, 0xB, 0x13, 0x25,
                         0x43, 0x83, 0x11B};
    return polynomials[n];
}

int multGF(int a, int b, int n, int p) {
    int result = 0;
    int highBit = 1 << n;

    while (b > 0) {
        if (b & 1) result ^= a;

        b >>= 1;
        a <<= 1;

        if (a & highBit) a ^= p;
    }

    return result;
}

int powerGF(int a, int degree, int n, int p) {
    int result = 1;

    while (degree > 0) {
        if (degree & 1) result = multGF(result, a, n, p);
        a = multGF(a, a, n, p);
        degree >>= 1;
    }

    return result;
}

vector<int> baseSbox(int n, const string& optimal) {
    int N = 1 << n;
    int p = fieldPolynomial(n);
    vector<int> S(N);

    for (int x = 0; x < N; ++x) {
        if (optimal == "lc" && n % 2 == 1) {
            S[x] = multGF(multGF(x, x, n, p), x, n, p);
        } else if (x == 0) {
            S[x] = 0;
        } else {
            S[x] = powerGF(x, N - 2, n, p);
        }
    }

    return S;
}

vector<int> randomMatrix(int n, mt19937_64& generator) {
    vector<int> rows(n);

    for (int i = 0; i < n; ++i) rows[i] = 1 << i;

    uniform_int_distribution<int> index(0, n - 1);
    uniform_int_distribution<int> operation(0, 1);

    for (int k = 0; k < 8 * n; ++k) {
        int i = index(generator);
        int j = index(generator);

        if (i == j) {
            --k;
            continue;
        }

        if (operation(generator) == 0) swap(rows[i], rows[j]);
        else rows[i] ^= rows[j];
    }

    return rows;
}

int linearMap(int x, const vector<int>& rows) {
    int result = 0;

    for (int i = 0; i < (int)rows.size(); ++i) {
        if (scal(x, rows[i])) result |= 1 << i;
    }

    return result;
}

vector<int> affineSbox(const vector<int>& base, int n,
                       mt19937_64& generator) {
    int N = 1 << n;
    vector<int> A = randomMatrix(n, generator);
    vector<int> B = randomMatrix(n, generator);
    vector<int> S(N);
    uniform_int_distribution<int> value(0, N - 1);
    int a = value(generator);
    int b = value(generator);

    for (int x = 0; x < N; ++x) {
        int y = linearMap(x, A) ^ a;
        S[x] = linearMap(base[y], B) ^ b;
    }

    return S;
}

struct Characteristics {
    int delta;
    int lambda;
    int degree;
};

Characteristics checkSbox(const vector<int>& S, int n) {
    return {deltaS(S), lambdaS(S), stepenS(S, n)};
}

bool correctCriterion(const Characteristics& c, const string& optimal, int n) {
    int lambdaLimit[] = {0, 0, 0, 4, 8, 8, 16, 16, 32};

    if (optimal == "dc") return c.delta <= 6;
    if (optimal == "lc") return c.lambda <= lambdaLimit[n];
    return c.degree == n - 1;
}

bool alreadyGenerated(const vector<vector<int>>& all, const vector<int>& S) {
    for (const vector<int>& current : all) {
        if (current == S) return true;
    }

    return false;
}

void printSbox(const vector<int>& S, int n, const string& optimal,
               const Characteristics& c) {
    int width = (n + 3) / 4;

    cout << "n = " << n << ", criterion: " << optimal << '\n';
    cout << uppercase << hex << setfill('0');

    for (int i = 0; i < (int)S.size(); ++i) {
        if (i > 0) cout << ' ';
        cout << setw(width) << S[i];
    }

    cout << dec << setfill(' ') << '\n';
    cout << "differential uniformity: " << c.delta << '\n';
    cout << "linear characteristic: " << c.lambda << '\n';
    cout << "degree: " << c.degree << '\n';
}

unsigned long long randomSeed() {
    random_device device;
    unsigned long long timeValue = chrono::high_resolution_clock::now()
                                       .time_since_epoch()
                                       .count();

    return (static_cast<unsigned long long>(device()) << 32) ^
           static_cast<unsigned long long>(device()) ^ timeValue;
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

    if (!p.seedSet) p.seed = randomSeed();

    mt19937_64 generator(p.seed);
    vector<int> base = baseSbox(p.n, p.optimal);
    vector<vector<int>> generated;

    while ((int)generated.size() < p.count) {
        vector<int> S = affineSbox(base, p.n, generator);

        if (alreadyGenerated(generated, S)) continue;
        if (!isPodstanovka(S)) continue;

        Characteristics c = checkSbox(S, p.n);
        if (!correctCriterion(c, p.optimal, p.n)) continue;

        if (!generated.empty()) cout << '\n';
        printSbox(S, p.n, p.optimal, c);
        generated.push_back(S);
    }

    return 0;
}
