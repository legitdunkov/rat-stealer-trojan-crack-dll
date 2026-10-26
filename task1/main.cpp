#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int ves(int x) {
    int k = 0;
    while (x > 0) {
        k += x & 1;
        x >>= 1;
    }
    return k;
}

int scal(int a, int b) {
    return ves(a & b) % 2;
}

// Быстрое преобразование Фурье (Уолша-Адамара)
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
    vector<int> used(S.size(), 0);

    for (int x : S) {
        if (used[x]) return false;
        used[x] = 1;
    }
    return true;
}

int deltaS(const vector<int>& S, int& maxA, int& maxB) {
    int N = S.size();
    int delta = 0;

    for (int a = 1; a < N; ++a) {
        vector<int> count(N, 0);

        for (int x = 0; x < N; ++x) {
            int b = S[x] ^ S[x ^ a];
            count[b]++;
        }

        for (int b = 0; b < N; ++b) {
            if (count[b] > delta) {
                delta = count[b];
                maxA = a;
                maxB = b;
            }
        }
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

        for (int x : f) {
            lambda = max(lambda, abs(x));
        }
    }
    return lambda;
}

int stepenF(const vector<int>& S, int b, int n) {
    int N = S.size();
    vector<int> anf(N);

    for (int x = 0; x < N; ++x) {
        anf[x] = scal(b, S[x]);
    }

    for (int i = 0; i < n; ++i) {
        for (int mask = 0; mask < N; ++mask) {
            if (mask & (1 << i)) {
                anf[mask] ^= anf[mask ^ (1 << i)];
            }
        }
    }

    int d = 0;
    for (int mask = 0; mask < N; ++mask) {
        if (anf[mask]) d = max(d, ves(mask));
    }
    return d;
}

void stepenS(const vector<int>& S, int n, int& degS, int& degMin) {
    int N = S.size();
    degS = 0;
    degMin = n;

    for (int b = 1; b < N; ++b) {
        int d = stepenF(S, b, n);
        degMin = min(degMin, d);

        if (ves(b) == 1) {
            degS = max(degS, d);
        }
    }
}

int rankF2(vector<vector<int>> a) {
    if (a.empty()) return 0;

    int rows = a.size();
    int cols = a[0].size();
    int rank = 0;

    for (int col = 0; col < cols && rank < rows; ++col) {
        int p = rank;
        while (p < rows && a[p][col] == 0) p++;

        if (p == rows) continue;

        swap(a[p], a[rank]);

        for (int i = rank + 1; i < rows; ++i) {
            if (a[i][col]) {
                for (int j = col; j < cols; ++j) {
                    a[i][j] ^= a[rank][j];
                }
            }
        }
        rank++;
    }
    return rank;
}

int algebraicImmunost(const vector<int>& S, int n) {
    int N = S.size();
    int vars = 2 * n;

    for (int d = 0; d <= vars; ++d) {
        vector<int> monom;

        for (int mask = 0; mask < (1 << vars); ++mask) {
            if (ves(mask) <= d) monom.push_back(mask);
        }

        if ((int)monom.size() > N) return d;

        vector<vector<int>> E(monom.size(), vector<int>(N, 0));

        for (int i = 0; i < (int)monom.size(); ++i) {
            for (int x = 0; x < N; ++x) {
                int point = x | (S[x] << n);
                E[i][x] = ((point & monom[i]) == monom[i]);
            }
        }

        if (rankF2(E) < (int)monom.size()) return d;
    }
    return vars;
}

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

    int n;
    if (!(in >> n) || n < 2 || n > 8) {
        cerr << "Error: n must be from 2 to 8\n";
        return 1;
    }

    int N = 1 << n;
    vector<int> S(N);

    for (int i = 0; i < N; ++i) {
        if (!(in >> hex >> S[i])) {
            cerr << "Error: incorrect number of S-box values\n";
            return 1;
        }
        if (S[i] < 0 || S[i] >= N) {
            cerr << "Error: S-box value is outside the range\n";
            return 1;
        }
    }

    string extra;
    if (in >> extra) {
        cerr << "Error: too many S-box values\n";
        return 1;
    }

    int a = 0, b = 0;
    int delta = deltaS(S, a, b);
    int lambda = lambdaS(S);
    int deg, degMin;
    stepenS(S, n, deg, degMin);
    int ai = algebraicImmunost(S, n);

    cout << "n = " << dec << n << '\n';
    cout << "bijective: " << (isPodstanovka(S) ? "yes" : "no") << '\n';
    cout << "differential uniformity: " << delta
         << "    (a = " << uppercase << hex << a
         << ", b = " << b << dec << ")\n";
    cout << "linear characteristic: " << lambda
         << "    nonlinearity: " << N / 2 - lambda / 2 << '\n';
    cout << "algebraic degree: " << deg
         << "    min component degree: " << degMin << '\n';
    cout << "graph algebraic immunity: " << ai << '\n';

    return 0;
}
