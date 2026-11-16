/*
# Streaming Q15 FIR filter

### Statement
A DSP front end filters a stream of signed 16-bit ADC samples with a K-tap FIR filter whose coefficients h[0..K-1] 
are in Q15 format (the integer c stands for c / 32768). 
Implement it as a streaming filter: one sample in, one sample out, with a fixed-size history buffer and no dynamic allocation.

For each input sample x[n] (n = 0, 1, ...), compute the exact integer sum
```
acc = h[0]*x[n] + h[1]*x[n-1] + ... + h[K-1]*x[n-K+1]
```

where x[i] = 0 for i < 0 (the history starts at zero). 
Then convert back to Q0 with round-half-up: y = floor((acc + 16384) / 32768), 
that is, add 2^14 and arithmetic-shift right by 15. 
Finally saturate y to the int16 range [-32768, 32767] and output it.

## Input / Output

* **Input**: (whitespace-separated tokens, may span lines): K N, then K coefficients, then N samples, all decimal integers.
* **Output**: N lines, the filtered output y[n] for each input sample, in order. Print nothing when N = 0.

## Constraints
* 1 <= K <= 64
* 0 <= N <= 100000
* Every coefficient and sample is in [-32768, 32767].
* O(K) time per sample and O(K) memory; no malloc/new.

### Example

#### Example 1: moving average
Input:
```
2 4
16384 16384
100 200 300 -50
```

Output:
```
50
150
250
125
```

#### Example 2: three tap

input:
```
3 5
8192 16384 8192
1000 0 0 0 -1000
```

Output:
```
3 5
8192 16384 8192
1000 0 0 0 -1000
```


*/

#include <cstdint>
#include <cstdio>
#include <iostream>

constexpr int kMaxTaps = 64;

class Fir {
public:
    // Store the K Q15 coefficients and clear the sample history.
    void init(const int16_t *coeffs, int k) 
    { 
        (void)coeffs; (void)k; /* TODO */ 
    }

    // Push one input sample and return the filtered int16 output.
    int16_t step(int16_t sample) 
    { 
        (void)sample; return 0; /* TODO */ 
    }

private:
    // TODO: coefficients and sample history, fixed size (no heap).
};

int main() {
    std::ios::sync_with_stdio(false);
    int k = 0, n = 0, v = 0;
    if (!(std::cin >> k >> n)) return 0;
    int16_t coeffs[kMaxTaps];
    for (int i = 0; i < k; ++i) {
        if (!(std::cin >> v)) return 0;
        coeffs[i] = static_cast<int16_t>(v);
    }
    static Fir fir;
    fir.init(coeffs, k);
    for (int i = 0; i < n; ++i) {
        if (!(std::cin >> v)) break;
        std::cout << fir.step(static_cast<int16_t>(v)) << '\n';
    }
    return 0;
}
