/***
# UART Frame Deframer with Resynchronisation

>> Language: C++. Hidden tests: 12, plus the 2 examples below.

## Problem
A sensor board streams frames over UART. Each frame is laid out like this:


| Field   | Size       |           Value                      |
|---------|------------|--------------------------------------|
| SOF     | 1 byte     | `0xAA`                                 |
| LEN     | 1 byte     | number of payload bytes              |
| Payload | LEN bytes  | data                                 |
| CHK     | 1 byte     | `LEN + sum of payload bytes mod 256` |

Bytes between frames are line noise. Write a deframer that takes one byte at a time. 
It should be a state machine with fixed-size buffers and no dynamic allocation. 
Apply these rules in stream order:
1. **Hunting**: a byte equal to `0xAA` starts a frame. Any other byte is discarded and counted.
2. **Length**: the byte after `SOF` is `LEN`. If `LEN > 32`, print `ERR LEN` and go back to hunting. 
   That same `LEN` byte is then examined again as a hunting byte, so it is either discarded and counted, 
   or it starts a new frame if it is `0xAA`.
3. **Payload and checksum**: otherwise read exactly LEN payload bytes (payload bytes are never treated as SOF), then the `CHK` byte. 
   If `CHK` matches, print the frame. If not, print `ERR CHK`. Either way, go back to hunting starting with the byte after `CHK`.
4. **Truncation**: if the stream ends after an `SOF` but before that frame's `CHK` byte, print `ERR TRUNC`.
5. **Summary**: at the end, print `DISCARDED k`, where `k` is the number of bytes discarded while hunting.

## Input / Output
**Input**: the first token is `N`, the number of bytes. Then come `N` bytes as whitespace-separated two-digit hex tokens. They can be upper or lower case and can span several lines.

**Output**: one line per event, in stream order:

`FRAME L` followed by a space and two-digit uppercase hex for each payload byte (just `FRAME 0` for an empty payload)
`ERR LEN`, `ERR CHK` or `ERR TRUNC`
The last line is always `DISCARDED k`.

## Constraints
* `0 <= N <= 200000`
* A valid frame has `0 <= LEN <= 32`
* O(1) work per byte, and O(1) memory beyond a 32-byte payload buffer
* No `malloc` / `new`

## Examples
#### Example 1
```
6
55 AA 02 10 20 32
```
output:
```
FRAME 2 10 20
DISCARDED 1
```


#### Example 2
```
8
AA 01 05 07 AA 01 05 06
```

Output:
```
ERR CHK
FRAME 1 05
DISCARDED 0
```
*/

#include <iostream>
#include <cstdint>

constexpr std::uint8_t kSof = 0xAA;
constexpr int kMaxPayload = 32;

enum State : uint8_t { HUNT, LEN, PAYLOAD, CHK };

struct Diframer{

    State state{HUNT};
    uint8_t len{0}, got{0}, sum{0};
    uint8_t buf[32]; // payload buffer
    uint32_t discarded{0};

    void feed(uint8_t b){

        switch (state)
        {
        case HUNT:
            if(b == kSof){
                state = LEN;
            }else{
                discarded++;
            }
            break;

        case LEN:
            if(b > kMaxPayload){
                std::puts("ERR LEN");
                state = HUNT;
                feed(b);  // re-examine this byte as a hunting byte
            }else{
                len = b; got = 0; sum = b; // add length 1st
                state = (len == 0) ? CHK : PAYLOAD;
            }
            break;

        case PAYLOAD:
            buf[got++] = b;
            sum =  (uint8_t)(sum + b); // sum of payload bytes mod 256
            if(got == len) state = CHK;
            break;

        case CHK:
            if(b == sum){
                std::printf("FRAME %u", (unsigned)len);
                for(uint8_t i=0; i<len; i++) std::printf(" %02X", buf[i]);
                std::putchar('\n');
            }else{
                std::puts("ERR CHK");
            }
            state = HUNT;
            break;

        }
    }

    void finish() {
        if(state != HUNT) std::puts("ERR TRUNC");
        std::printf("DISCARDED %u\n", (unsigned)discarded);
    }

};

int main(){

    int n;
    if(std::scanf("%d", &n) != 1) n = 0;

    {
        Diframer d;

        for(int i=0; i < n; i++){
            unsigned v;
            if(std::scanf("%x", &v) != 1) break;
            d.feed((uint8_t)v);
        }
        d.finish();
    }
    return 0;
}

