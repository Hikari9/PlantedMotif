#ifndef INCLUDE_SUFFIX
#define INCLUDE_SUFFIX 1

#include <cstring>

// uses radix sort
namespace suffix {
	struct suffix_cmp {
		int n, gap, *pos;
		suffix_cmp(int n, int pos[]): n(n), pos(pos) {}
		inline bool operator() (int i, int j) const {
			if (pos[i] != pos[j])
				return pos[i] < pos[j];
			i += gap;
			j += gap;
			return i < n && j < n ? pos[i] < pos[j] : j < i;
		}
	};
	long long get_suffix_score(char s[]) {
		int n = std::strlen(s) + 1;
		int *sa = new int[n];
		int *pos = new int[n];
		int *tmp = new int[n];
		tmp[0] = 0;
		for (int i = 0; i < n; ++i)
			sa[i] = i, pos[i] = s[i];
		suffix_cmp cmp(n, pos);
		for (cmp.gap = 1;; cmp.gap <<= 1) {
			sort(sa, sa + n, cmp);
			for (int i = 1; i < n; ++i)
				tmp[i] = tmp[i - 1] + cmp(sa[i - 1], sa[i]);
			for (int i = 0; i < n; ++i)
				pos[sa[i]] = tmp[i];
			if (tmp[n - 1] == n - 1)
				break;
		}
		long long score = 0;
		for (int i = 0, k = 0; i < n; ++i)
			if (pos[i] + 1 < n) {
				for (int j = sa[pos[i] + 1]; s[i + k] == s[j + k]; ++k);
				score += k;
				if (k) --k;
			}
		delete[] sa, pos, tmp;
		return score;
	}
}

#endif /* INCLUDE_SUFFIX */