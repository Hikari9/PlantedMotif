#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <set>
#include <vector>
#include <algorithm>
#include "suffix.hpp"
#include "hamming.hpp"
#include "double_int.hpp"
#define GENERATE time(NULL)

using namespace std;
#define N 20
#define M 600
#define L 12
#define D 6
typedef unsigned long long ull;
typedef double_int<unsigned, ull> term;
typedef set<term> Set;
#define foreach(iter, collection) for (Set::iterator iter = (collection).begin(), iter != (collection).end(); ++iter)
#define oneach(iter, collection) for (Set::iterator iter = (collection).begin(), iter != (collection).end();)

char s[N][M+1], *S[N];
char acgt_ch[] = "ACGT", acgt_4[16];
int order[N], acgt[128];
long long suffix_score[N];

bool score_compare(int i, int j) {
	return suffix_score[i] > suffix_score[j];
}

char getchar(const term& b, int i) {
	return acgt_4[b.get(i, 2)];
}

term merge(const term& a, const term& b) {
	term ham(a | b), ans = ham;
	for (int i = 0; i < L; ++i)
		if (getchar(ham, i) == '.')
			ans.set(i, 15, 2);
	return ans;
}

string operator string(const term& b) {
	string str;
	for (int i = 0; i < L; ++i)
		str.push_back(getchar(b, i));
	return str;
}

ostream& operator << (ostream& out, const term& b) {
	return out << string(b);
}

ull choose[L + 1][D + 1] = {1};
Set coll[D + 1];

bool check_patterns_below(int blanks, const term& pattern) {
	for (int i = 0; i < blanks; ++i) {
		foreach (it, coll[i]) {
			((*it & pattern) == *it) {
				return true;
			}
		}
	}
	return false;
}

bool remove_patterns_above(int blans, const term& pattern) {
	bool match = false;
	for (int i = D; i > blanks; --i) {
		oneach (it, coll[i]) {
			if ((*it & pattern) == pattern) {
				coll[i].erase(it++);
				match = true;
			} else {
				++it;
			}
		}
	}
	return match;
}

void add_pattern(int blanks, const term& pattern) {
	
	// check if current pattern is already there
	if (coll[blanks].count(pattern)) return;

	int coll_below = 0;
	for (int i = 0; i < blanks; ++i)
		coll_below += coll[i].size();

	int coll_above = 0;
	for (int i = blanks + 1; i <= D; ++i)
		coll_above += coll[i].size();

	// heuristic ensures us that there is only one subset/superset pair at a given time
	if (coll_below < coll_above) {
		if (!check_patterns_below(blanks, pattern)) {
			remove_patterns_above(blanks, pattern);
			coll[blanks].insert(pattern);
		}
	} else {
		if (remove_patterns_above(blanks, pattern) || !check_patterns_below(blanks, pattern)) {
			coll[blanks].insert(pattern);
		}
	}

}

int _underscore(const term& b) {
	int c = 0;
	for (int i = 0; i < L; ++i)
		if (getchar(b, i) == '_')
			++c;
	return c;
}

int _underscore(const string& s) {
	int c = 0;
	for (int i = 0; i < L; ++i)
		if (s[i] == '_')
			++c;
	return c;
}

int _hamming(const term& a, const term& b) {
	int c = 0;
	for (int i = 0; i < L; ++i)
		if (getchar(a, i) != getchar(b, i))
			++c;
	return c;
}

int _hamming(const string& a, const string& b) {
	int c = 0;
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i] && a[i] != '_' && b[i] != '_')
			++c;
	return c;
}

int _hamming_all(const string& a, const string& b) {
	int c = 0;
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i])
			++c;
	return c;
}

int count_bits(const term& t) {
	return hamming::count_bits(t.first) + hamming::count_bits(t.second);
}

void pregen() {
	memset(acgt_4, '.', sizeof acgt_4);
	acgt['A'] = 0;
	acgt['C'] = 1;
	acgt['G'] = 2;
	acgt['T'] = 3;
	acgt_4[1] = 'A';
	acgt_4[2] = 'C';
	acgt_4[4] = 'G';
	acgt_4[8] = 'T';
	acgt_4[15] = '_';
	for (int i = 1; i <= L; ++i) {
		choose[i][0] = 1;
		for (int j = 1; j <= D; ++j)
			choose[i][j] = choose[i - 1][j - 1] + choose[i - 1][j];
	}
}

int main() {

	// 0) generate input
	pregen();
	unsigned seed = GENERATE;
	srand(seed);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			s[i][j] = acgt_ch[rand() % 4];

	// 1) sort suffixes by score
	for (int i = 0; i < N; ++i)
		suffix_score[i] = suffix::get_suffix_score(s[i]),
		order[i] = i;
	sort(order, order + N, score_compare);
	for (int i = 0; i < N; ++i)
		S[i] = s[order[i]];

	// 2) collect first two pair's motif pattern
	ull lmer = 0;
	ull mask = (1ULL << (L * 2)) - 1;
	for (int j = 0; j < M;) {
		lmer = ((lmer << 2) | acgt[S[0][j]]) & mask;
		if (++j >= L) {
			ull lmer2 = 0;
			for (int k = 0; k < M;) {
				lmer2 = ((lmer2 << 2) | acgt[S[1][k]]) & mask;
				if (++k >= L) {
					int h = hamming::distance(lmer ^ lmer2);
					if (h <= D) {
						term pattern;
						for (int i = 0; i < L; ++i) {
							int val = S[0][j-L+i] == S[1][k-L+i] ? (1 << acgt[S[0][j-L+i]]) : 15;
							pattern.set(i, val, 2);
						}
						add_pattern(h, pattern);
					}
				}
			}
		}
	}

	// 3) intersect all other motifs
	for (int i = 2; i < N; ++i) {
		for (int I = D; I >= 0; --I) {
			oneach (it, coll[I]) {
				term pattern;
				int flag = 0;
				vector< pair<int, pattern> > to_add;
				for (int j = 0; j < M;) {
					pattern >>= 4;
					pattern.set(L - 1, (1 << acgt[S[i][j]]), 2);
					if (++j >= L) {
						int h = 
					}
				}
			}
		}
	}
	for (int i = 2; i < 4; ++i) {
		for (int I = D; I >= 0; --I) {
			oneach (it, coll[I]) {
				pattern.first = pattern.second = 0;
				vector<term> v;
				bool keep = false;
				for (int j = 0; j < M;) {
					bitset_append(pattern, (1 << acgt[S[i][j]]));
					if (++j >= L) {
						int h = L + I - count_bits(pattern & *it);
						if (h <= D) {
							if (h == I) {
								v.clear();
								keep = true;
								break;
							}
							else {
								v.push_back(merge(pattern, *it));
								flag = true;
							}
						}
					}
				}
				if (v.size()) 
				if (flag) {
					coll[I].erase(it++);
					to_ad
				}
				if (!(flag & 2)) coll[I].erase(it++);
				else if (flag & 1)
				if (!found) {
					coll[I].erase(it++);
				}
			}
		}
	}

	// output motifs
	for (int i = 0; i <= D; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			cout << i << ' ' << *it << endl;

	// test
	for (int i = 0; i <= D; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it) {
			ostringstream os;
			os << *it;
			string a = os.str();
			for (int i = 0; i < N; ++i) {
				bool ok = false;
				for (int j = L; j <= M; ++j) {
					string b(S[i] + j - L, S[i] + j);
					if (str_hamming(a, b) <= D) {
						ok = true;
						break;
					}
				}
				if (!ok) {
					cout << "WTF man " << *it << ' ' << S[i] << ' ' << i << ' ' << seed << endl;
					return 1;
				}
			}
		}

}