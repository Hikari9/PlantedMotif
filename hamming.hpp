#ifndef INCLUDE_HAMMING
#define INCLUDE_HAMMING

namespace hamming {
	int dp[1 << 16] = {-1};

	inline int distance(unsigned x) {
		if (dp[0] == -1) {
			dp[0] = 0;
			for (int i = 1; i < (1 << 16); ++i)
				dp[i] = dp[i >> 2] + ((i & 3) != 0);
		}
		return dp[x >> 16] + dp[x & ((1 << 16) - 1)];
	}

	inline int distance(unsigned long long x) {
		return distance((unsigned) (x >> 32)) + distance((unsigned) (x & ((1LL << 32) - 1)));
	}
}

#endif /* INCLUDE_HAMMING */