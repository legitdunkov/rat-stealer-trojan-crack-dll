#include <fstream>
#include <iomanip>
#include <iostream>
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

    int m, p, n;

    if (!(in >> m >> hex >> p) || m < 1 || m > 8) {
        cerr << "Error: incorrect field parameters\n";
        return 1;
    }

    if (degreePoly(p) != m) {
        cerr << "Error: polynomial degree must be equal to m\n";
        return 1;
    }

    if (!irreduciblePoly(p, m)) {
        cerr << "Error: polynomial is reducible\n";
        return 1;
    }

    if (!(in >> dec >> n) || n < 1 || n > 8) {
        cerr << "Error: n must be from 1 to 8\n";
        return 1;
    }

    vector<vector<int>> A(n, vector<int>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (!(in >> hex >> A[i][j])) {
                cerr << "Error: incorrect number of matrix elements\n";
                return 1;
            }

            if (A[i][j] < 0 || A[i][j] >= (1 << m)) {
                cerr << "Error: matrix element is not in the field\n";
                return 1;
            }
        }
    }

    string extra;
    if (in >> extra) {
        cerr << "Error: too many matrix elements\n";
        return 1;
    }

    cout << "field: GF(2^" << dec << m << "), p = 0x"
         << uppercase << hex << p << '\n';
    cout << "n = " << dec << n << '\n';
    cout << "matrix:\n";

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << uppercase << hex << A[i][j];
            if (j + 1 < n) cout << ' ';
        }
        cout << '\n';
    }

    return 0;
}
