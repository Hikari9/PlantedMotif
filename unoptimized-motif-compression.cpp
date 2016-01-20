#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <set>
#include <algorithm>
#define GENERATE time(NULL)
#define all(collection) (collection).begin(), (collection).end()

using namespace std;
typedef set<string> Set;

char ACGT[] = "ACGT?";
int acgt[128];

ostream& operator << (ostream&, const Set&);
string operator & (const string&, string);
string operator ^ (const string&, string);
int questions(const string&);

int main() {
	#ifdef INPUT
		int n, m, L, D;
		scanf("%d%d%d%d", &m, &n, &L, &D);
		int pos[m];
		for (int i = 0; i < m; ++i)
			scanf("%d", pos + i);
		char s[m][n+1];
		char in[L+1];
		scanf("%s", in);
		for (int i = 0; i < m; ++i)
			scanf("%s", s[i]);
		cout << "Confirming if test data is correct..." << endl;
		bool correct_data = true;
		for (int i = 0; i < m; ++i) {
			string test(s[i]+pos[i], s[i]+pos[i]+L);
			if (questions(test & in)) {
				cout << i << ": OK " << test << " ~ " << (test & in) << " has distance " << questions(test & in) << endl;
			} else {
				correct_data = false;
				cout << i << ": fail! " << test << " ~ " << (test & in) << " has distance " << questions(test & in) << endl; 
			}
		}
		if (!correct_data) return 0;
		cout << "Performing algorithm..." << endl;
	#else
		int n = 600, m = 20, L = 17, D = 6;
		char s[m][n+1];
		srand(GENERATE);
		for (int i = 0; i < m; ++i)
			for (int j = 0; j < n; ++j)
				s[i][j] = ACGT[rand() & 3];
	#endif /* INPUT */
	string as = string(s[0]+pos[0], s[0]+pos[0]+L);
	string bs = string(s[1]+pos[1], s[1]+pos[1]+L);
	#define quest(Q) (Q) << ' ' << questions(Q)
	cout << as << ' ' << bs << ' ' << in << ' ' << quest(as & in) << ' ' << quest(bs & in) << ' ' << quest(as ^ bs) << ' ' << quest((as&in)^(bs&in)) << endl;
	set<string> *u = new set<string>[D+1];
	set<string> *v = new set<string>[D+1];
	set<string> buffer;
	// get first genome motif set
	for (int i = L; i <= n; ++i)
		v[0].insert(string(s[0]+i-L, s[0]+i));
	// intersect with other motif sets
	for (int i = 1; i < 2; ++i) {
		for (int j = 0; j <= D; ++j)
			u[j].clear();
		buffer.clear();
		for (int j = L; j <= n; ++j)
			buffer.insert(string(s[i]+j-L, s[i]+j));
		int size = 0;
		for (const auto& a : buffer) {
			bool FLAG = (a == "GTGTCCACCCTGTAAGT");
			for (int j = D; j >= 0; --j) {
				for (const auto& b : v[j]) {
					string c = a & b;
					int changes = questions(c);
					// if (FLAG) cout << c << ' ' << changes << endl;
					if (changes <= D) {
						if (FLAG) cout << a << ' ' << b << ' ' << c << endl;
						if (u[changes].count(c)) continue; // already in the set
						bool add_flag = true;
						for (int l = 0; l < changes; ++l) {
							for (const auto& d : u[l]) {
								if (c == (c & d)) {
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
									if (d == (c & d)) {
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
	#ifdef INPUT
	bool found = false;
	int mn = 2147483647;
	for (int i = 0; i <= D; ++i) {
		for (const auto& str : v[i]) {
			// cout << str << ' ' << in << ' ' << (str & in) << endl;
			mn = min(mn, questions(str & in));
			if (questions(str & in) <= D) {
				found = true;
				cout << "Found pattern " << str << endl;
			}
		}
	}
	if (!found) cout << "Not found :(" << endl;
	#endif /* INPUT */
	cerr << "Time: " << (float) clock() / CLOCKS_PER_SEC << endl;
}

ostream& operator << (ostream& out, const Set& s) {
	out << '[';
	bool first = true;
	for (const auto& str : s) {
		if (first) first = false;
		else out << ',';
		out << str;
	}
	return out << ']';
}

string operator & (const string& a, string b) {
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i])
			b[i] = '?';
	return b;
}

string operator ^ (const string& a, string b) {
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i]) {
			if (a[i] == '?');
			else if (b[i] == '?') b[i] = a[i];
			else b[i] = '?';
		}
	return b;
}

int questions(const string& s) {
	int c = 0;
	for (int i = 0; i < s.length(); ++i)
		if (s[i] == '?')
			++c;
	return c;
}