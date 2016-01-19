#ifndef INCLUDE_HAMMING
#define INCLUDE_HAMMING

namespace hamming {
	int dp[1 << 16] = {-1};
	int dpc[1 << 16] = {-1};

	inline int distance(unsigned x) {
		if (dp[0] == -1) {
			dp[0] = 0;
			for (int i = 1; i < (1 << 16); ++i)
				dp[i] = dp[i >> 2] + ((i & 3) != 0);
		}
		return dp[x >> 16] + dp[x & ((1 << 16) - 1)];
	}

	inline int distance(unsigned long long x) {
		return distance((unsigned) (x >> 32)) + distance((unsigned) (x & ((1ULL << 32) - 1)));
	}

	inline int count_bits(unsigned x) {
		if (dpc[0] == -1) {
			dpc[0] = 0;
			for (int i = 1; i < (1 << 16); ++i)
				dpc[i] = dpc[i >> 1] + (i & 1);
		}
		return dpc[x >> 16] + dpc[x & ((1 << 16) - 1)];
	}

	inline int count_bits(unsigned long long x) {
		return count_bits((unsigned) (x >> 32)) + count_bits((unsigned) (x & ((1ULL << 32) - 1)));
	}
}

#endif /* INCLUDE_HAMMING */