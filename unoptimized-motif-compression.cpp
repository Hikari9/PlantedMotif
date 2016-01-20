#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;
typedef set<string> Set;
const int n = 600;
const int m = 20;
const int L = 17;
const int D = 10;
#define GENERATE time(NULL)
#define all(collection) (collection).begin(), (collection).end()
void pregen();

char s[m][n+1], ACGT[] = "ACGT?";
int acgt[128];

ostream& operator << (ostream& out, const Set& v) {
	out << '[';
	bool first = true;
	for (const auto& x : v) {
		if (first) first = false;
		else out << ',';
		out << x;
	}
	return out << ']';
}

string operator ^ (const string& a, string b) {
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i])
			b[i] = '?';
	return b;
}

int questions(const string& s) {
	int c = 0;
	for (char ch : s)
		if (ch == '?')
			++c;
	return c;
}

int main() {
	pregen();
	set<string> *u = new set<string>[D+1];
	set<string> *v = new set<string>[D+1];
	set<string> buffer;
	// get first genome motif set
	for (int i = L; i <= n; ++i)
		v[0].insert(string(s[0]+i-L, s[0]+i));
	// intersect with other motif sets
	for (int i = 1; i < 20; ++i) {
		for (int j = 0; j <= D; ++j)
			u[j].clear();
		buffer.clear();
		for (int j = L; j <= n; ++j)
			buffer.insert(string(s[i]+j-L, s[i]+j));
		int size = 0;
		for (const auto& a : buffer) {
			for (int j = D; j >= 0; --j) {
				for (const auto& b : v[j]) {
					string c = a ^ b;
					int changes = questions(c);
					if (changes <= D) {
						// cout << a << ' ' << b << ' ' << c << endl;
						if (u[changes].count(c)) continue; // already in the set
						bool add_flag = true;
						for (int l = 0; l < changes; ++l) {
							for (const auto& d : u[l]) {
								if (c == (c ^ d)) {
									// c is a subcompression of d
									add_flag = false;
									break;
								}
							}
						}
						if (add_flag) {
							// add to the collection, check for subcompressions to remove
							u[changes].insert(c);
							++size;
							for (int l = changes + 1; l <= D; ++l) {
								for (Set::iterator it = u[l].begin(); it != u[l].end();) {
									const auto& d = *it;
									if (d == (c ^ d)) {
										// d is a subcompression of c
										u[l].erase(it++);
										--size;
									} else {
										++it;
									}
								}
							}
						}
					}
				}
			}
		}
		if (size == 0) break;
		swap(u, v);
	}
	// for (int i = D; i >= 0; --i)
		// cout << v[i] << endl;
	cout << (float) clock() / CLOCKS_PER_SEC << endl;
}

void pregen() {
	for (int i = 0; ACGT[i]; ++i)
		acgt[ACGT[i]] = i;
	srand(GENERATE);
	for (int i = 0; i < m; ++i)
		for (int j = 0; j < n; ++j)
			s[i][j] = ACGT[rand() % 4];
}