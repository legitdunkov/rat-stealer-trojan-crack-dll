#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

int degreePoly(int a) {
    int degree = -1;

    while (a > 0) {
        a >>= 1;
        ++degree;
    }

    return degree;
}

int remainderPoly(int a, int b) {
    int degreeB = degreePoly(b);

    while (degreePoly(a) >= degreeB) {
        int shift = degreePoly(a) - degreeB;
        a ^= b << shift;
    }

    return a;
}

bool irreduciblePoly(int p, int m) {
    for (int degree = 1; degree <= m / 2; ++degree) {
        int first = 1 << degree;
        int last = 1 << (degree + 1);

        for (int q = first; q < last; ++q) {
            if (remainderPoly(p, q) == 0) return false;
        }
    }

    return true;
}

int sumGF(int a, int b) {
    return a ^ b;
}

int multGF(int a, int b, int m, int p) {
    int result = 0;
    int highBit = 1 << m;

    while (b > 0) {
        if (b & 1) result ^= a;

        b >>= 1;
        a <<= 1;

        if (a & highBit) a ^= p;
    }

    return result;
}

int powerGF(int a, int k, int m, int p) {
    int result = 1;

    while (k > 0) {
        if (k & 1) result = multGF(result, a, m, p);
        a = multGF(a, a, m, p);
        k >>= 1;
    }

    return result;
}

int inverseGF(int a, int m, int p) {
    if (a == 0) return -1;
    return powerGF(a, (1 << m) - 2, m, p);
}

int countBits(int mask) {
    int count = 0;

    while (mask > 0) {
        count += mask & 1;
        mask >>= 1;
    }

    return count;
}

int determinantGF(vector<vector<int>> B, int m, int p) {
    int n = B.size();
    int result = 1;

    for (int col = 0; col < n; ++col) {
        int pivot = col;

        while (pivot < n && B[pivot][col] == 0) ++pivot;
        if (pivot == n) return 0;

        if (pivot != col) B[pivot].swap(B[col]);

        result = multGF(result, B[col][col], m, p);
        int inverse = inverseGF(B[col][col], m, p);

        for (int row = col + 1; row < n; ++row) {
            int factor = multGF(B[row][col], inverse, m, p);

            for (int j = col; j < n; ++j) {
                B[row][j] = sumGF(B[row][j],
                                  multGF(factor, B[col][j], m, p));
            }
        }
    }

    return result;
}

bool checkMDS(const vector<vector<int>>& A, int m, int p) {
    int n = A.size();
    int limit = 1 << n;

    for (int size = 1; size <= n; ++size) {
        for (int rowMask = 1; rowMask < limit; ++rowMask) {
            if (countBits(rowMask) != size) continue;

            for (int colMask = 1; colMask < limit; ++colMask) {
                if (countBits(colMask) != size) continue;

                vector<vector<int>> minor;

                for (int i = 0; i < n; ++i) {
                    if ((rowMask & (1 << i)) == 0) continue;

                    vector<int> row;
                    for (int j = 0; j < n; ++j) {
                        if (colMask & (1 << j)) row.push_back(A[i][j]);
                    }
                    minor.push_back(row);
                }

                if (determinantGF(minor, m, p) == 0) return false;
            }
        }
    }

    return true;
}

vector<vector<int>> makeCauchy(int n, int p, mt19937_64& generator,
                               vector<int>& x, vector<int>& y) {
    vector<int> elements(256);
    for (int i = 0; i < 256; ++i) elements[i] = i;

    shuffle(elements.begin(), elements.end(), generator);
    x.assign(elements.begin(), elements.begin() + n);
    y.assign(elements.begin() + n, elements.begin() + 2 * n);

    vector<vector<int>> A(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = inverseGF(x[i] ^ y[j], 8, p);
        }
    }

    return A;
}

void printMatrix(const vector<vector<int>>& A, int p) {
    cout << "8 " << uppercase << hex << p << '\n';
    cout << dec << A.size() << '\n';
    cout << uppercase << hex << setfill('0');

    for (const vector<int>& row : A) {
        for (int j = 0; j < (int)row.size(); ++j) {
            if (j > 0) cout << ' ';
            cout << setw(2) << row[j];
        }
        cout << '\n';
    }

    cout << dec << setfill(' ');
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

    if (!irreduciblePoly(p.polynomial, 8)) {
        cerr << "Error: polynomial is reducible\n";
        return 1;
    }

    if (p.construction == "cauchi") {
        if (!p.seedSet) p.seed = randomSeed();

        mt19937_64 generator(p.seed);
        vector<int> x, y;
        vector<vector<int>> A = makeCauchy(p.n, p.polynomial,
                                           generator, x, y);

        if (!checkMDS(A, 8, p.polynomial)) {
            cerr << "Error: generated matrix is not MDS\n";
            return 1;
        }

        printMatrix(A, p.polynomial);
        return 0;
    }

    cout << "construction: " << p.construction << '\n';
    cout << "n: " << p.n << '\n';
    cout << "polynomial: " << uppercase << hex << p.polynomial << dec << '\n';

    if (p.seedSet) cout << "seed: " << p.seed << '\n';
    else cout << "seed: random\n";

    cout << "involutory: " << (p.involutory ? "yes" : "no") << '\n';
    return 0;
}
