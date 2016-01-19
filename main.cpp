#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "suffix.hpp"
#include "hamming.hpp"
#define GENERATE 0

using namespace std;
const int N = 20, M = 600;
const int L = 17, D = 6;


char s[N][M+1];
char *S[N];
int order[N];
long long suffix_score[N];

bool score_compare(int i, int j) {
	return suffix_score[i] > suffix_score[j];
}

int main() {

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


}