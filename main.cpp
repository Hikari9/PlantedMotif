#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <set>
#include <algorithm>
#include "suffix.hpp"
#include "hamming.hpp"
#define GENERATE 1453218765

using namespace std;
#define N 5
#define M 3500
#define L 17
#define D 6

typedef unsigned long long ull;
typedef pair<unsigned, ull> BitSet;
typedef set<BitSet> Set;

inline BitSet operator ^ (const BitSet& a, const BitSet& b) {
	return BitSet(a.first ^ b.first, a.second ^ b.second);
}

inline BitSet operator & (const BitSet& a, const BitSet& b) {
	return BitSet(a.first & b.first, a.second & b.second);
}
inline BitSet operator | (const BitSet& a, const BitSet& b) {
	return BitSet(a.first | b.first, a.second | b.second);
}

inline void bitset_mask(BitSet& b, int i, unsigned mask) {
	if (i == 16) b.first = mask;
	else b.second |= ((ull)mask << (i * 4));
}

inline void bitset_append(BitSet& b, unsigned x) {
	b.second = (b.second >> 4) | ((ull) b.first << 60);
	b.first >>= 4;
	bitset_mask(b, L - 1, x);
}

char acgt_convert[16];

inline char bitset_get(const BitSet& b, int i) {
	return i == 16 ? acgt_convert[b.first] : acgt_convert[(unsigned) ((b.second >> (i << 2)) & 15)];
}

inline BitSet bitset_merge(const BitSet& a, const BitSet& b) {
	BitSet ham(a.first | b.first, a.second | b.second), ans = ham;
	for (int i = 0; i < L; ++i)
		if (bitset_get(ham, i) == '.')
			bitset_mask(ans, i, 15);
	return ans;
}

ostream& operator << (ostream& out, const BitSet& b) {
	for (int i = 0; i < L; ++i)
		out << bitset_get(b, i);
	return out;
}

char s[N][M+1];
char *S[N];
int order[N], acgt[128];
long long suffix_score[N];

bool score_compare(int i, int j) {
	return suffix_score[i] > suffix_score[j];
}

ull choose[L + 1][D + 1] = {1};
Set coll[D + 1];
BitSet pattern;
char pattern_buffer[L + 1];

inline bool add_pattern_below(int blanks, const BitSet& b = pattern) {
	bool added = false;
	for (int i = 0; i < blanks; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end();)
			if ((*it & b) == *it) {
				// match found
				coll[i].erase(it++);
				coll[blanks].insert(b);
				added = true;
			} else {
				++it;
			}
	return added;
}

inline bool add_pattern_above(int blanks, const BitSet& b = pattern) {
	for (int i = D; i > blanks; --i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			if ((*it & pattern) == b) {
				// match found
				return true;
			}
	return false;
}

// add pattern to collection
void add_pattern(int blanks) {
	
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
		if (!add_pattern_below(blanks) && !add_pattern_above(blanks))
			coll[blanks].insert(pattern);
	} else {
		if (!add_pattern_above(blanks) && !add_pattern_below(blanks))
			coll[blanks].insert(pattern);
	}

}

// TESTING

string convert(const BitSet& b) {
	ostringstream os;
	os << b;
	return os.str();
}

int underscore(const BitSet& b) {
	int c = 0;
	for (int i = 0; i < L; ++i)
		if (bitset_get(b, i) == '_')
			++c;

	return c;
}

int _hamming(const BitSet& a, const BitSet& b) {
	int c = 0;
	for (int i = 0; i < L; ++i)
		if (bitset_get(a, i) != bitset_get(b, i))
			++c;
	return c;
}

int str_hamming(const string& a, const string& b) {
	int c = 0;
	for (int i = 0; i < a.length(); ++i)
		if (a[i] != b[i] && a[i] != '_' && b[i] != '_')
			++c;

	return c;
}

int main() {
	acgt['A'] = 0;
	acgt['C'] = 1;
	acgt['G'] = 2;
	acgt['T'] = 3;
	memset(acgt_convert, '.', sizeof acgt_convert);
	acgt_convert[1] = 'A';
	acgt_convert[2] = 'C';
	acgt_convert[4] = 'G';
	acgt_convert[8] = 'T';
	acgt_convert[15] = '_';

	for (int i = 1; i <= L; ++i) {
		choose[i][0] = 1;
		for (int j = 1; j <= D; ++j)
			choose[i][j] = choose[i - 1][j - 1] + choose[i - 1][j];
	}

	// 0) generate input
	unsigned seed = GENERATE;
	srand(seed);
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < M; ++j)
			switch (rand() % 4) {
				case 0: s[i][j] = 'A'; break;
				case 1: s[i][j] = 'C'; break;
				case 2: s[i][j] = 'G'; break;
				case 3: s[i][j] = 'T'; break;
			}

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
						pattern.first = pattern.second = 0;
						for (int i = 0; i < L; ++i)
							bitset_mask(pattern, i, S[0][j-L+i] == S[1][k-L+i] ? (1 << acgt[S[0][j-L+i]]) : 15);
						// cout << h << ' ' << string(S[0] + (j - L), S[0] + j) << ' ' << string(S[1] + (k - L), S[1] + k) << ' ' << convert(pattern) << endl;
						add_pattern(h);
					}
				}
			}
		}
	}

	// 3) intersect all other motifs
	for (int i = 2; i < N; ++i) {
		for (int I = D; I >= 0; --I) {
			for (Set::iterator it = coll[I].begin(); it != coll[I].end();) {
				pattern.first = pattern.second = 0;
				bool found = false;
				for (int j = 0; j < M;) {
					bitset_append(pattern, (1 << acgt[S[i][j]]));
					if (++j >= L) {
						int h = L - hamming::count_bits(pattern.first & it->first) - hamming::count_bits(pattern.second & it->second) + I;
						if (h <= D) {
							if (h == I) ++it;
							else {
								BitSet b = bitset_merge(pattern, *it);
								coll[I].erase(it++);
								for (Set::iterator it2 = it; it2 != coll[I].end();) {
									if ((*it2 & b) == *it2) {
										// match found
										if (it == it2) {
											coll[I].erase(it2++);
											it = it2;
										} else {
											coll[I].erase(it2++);
										}
									} else {
										++it2;
									}
								}
								for (int J = I + 1; J < h; ++J) {
									for (Set::iterator it2 = coll[J].begin(); it2 != coll[J].end();) {
										if ((*it2 & b) == *it2) {
											// match found
											coll[I].erase(it2++);
										} else {
											++it2;
										}
									}
								}
								coll[h].insert(b);
							}
							found = true;
							break;
						}
					}
				}
				if (!found) coll[I].erase(it++);
			}
		}
		break;
	}

	// output motifs
	for (int i = 0; i <= D; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			cout << *it << endl;

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
					cout << "WTF man " << *it << " " << ' ' << i << ' ' << seed << endl;
					return 1;
				}
			}
		}

}