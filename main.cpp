#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <set>
#include <algorithm>
#include "suffix.hpp"
#include "hamming.hpp"
#define GENERATE 0

using namespace std;
#define N 20
#define M 600
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

inline BitSet bitset_mask(BitSet& b, int i, unsigned mask) {
	if (i == 16) b.first = mask;
	else b.second |= ((ull)mask << (i * 4));
}

char acgt_convert[16];

inline int bitset_get(const BitSet& b, int i) {
	return i == 16 ? acgt_convert[b.first] : acgt_convert[(unsigned) ((b.second >> (i << 2)) & 15)];
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

inline bool add_pattern_below(int blanks) {
	for (int i = 0; i < blanks; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			if ((*it & pattern) == *it) {
				// match found
				coll[i].erase(it);
				coll[blanks].insert(pattern);
				return true;
			}
	return false;
}

inline bool add_pattern_above(int blanks) {
	for (int i = D; i > blanks; --i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			if ((*it & pattern) == pattern) {
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

string convert(const BitSet& b) {
	string str;
	for (int i = 0; i < L; ++i) {
		str.push_back(bitset_get(b, i));
	}
	return str;
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
	srand(GENERATE);
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

	ull lmer1 = 0;
	ull mask = (1LL << (L * 2)) - 1;
	for (int j = 0; j < M;) {
		lmer1 = ((lmer1 << 2) | acgt[S[0][j]]) & mask;
		if (++j >= L) {
			ull lmer2 = 0;
			for (int k = 0; k < M;) {
				lmer2 = ((lmer2 << 2) | acgt[S[1][k]]) & mask;
				if (++k >= L) {
					ull x = lmer1 ^ lmer2;
					int h = hamming::distance(x);
					if (h <= D) {
						pattern.first = pattern.second = 0;
						for (int i = 0; i < L; ++i) {
							bitset_mask(pattern, i, S[0][j-L+i] == S[1][k-L+i] ? (1 << acgt[S[0][j-L+i]]) : 15);
						}
						// cout << h << ' ' << string(S[0] + (j - L), S[0] + j) << ' ' << string(S[1] + (k - L), S[1] + k) << ' ' << convert(pattern) << endl;
						add_pattern(h);
					}
				}
			}
		}
	}

	// test output:
	for (int i = 0; i <= D; ++i)
		for (Set::iterator it = coll[i].begin(); it != coll[i].end(); ++it)
			cout << convert(*it) << endl;

}