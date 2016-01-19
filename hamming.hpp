#ifndef INCLUDE_HAMMING
#define INCLUDE_HAMMING

namespace hamming {
	short dp[1 << 16] = {-1};

	inline int xor_distance(int x) {
		if (dp[0] == -1) {
			dp[0] = 0;
			for (int i = 1; i < (1 << 16); ++i)
				dp[i] = dp[i >> 2] + ((i & 3) != 0);
		}
		return dp[x >> 16] + dp[x & ((1 << 16) - 1)];
	}

	inline int distance(int a, int b) {
		return xor_distance(a, b);
	}
}

#endif /* INCLUDE_HAMMING */