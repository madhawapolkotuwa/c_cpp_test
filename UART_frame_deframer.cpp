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
#include <string>
#include <stdint.h>
#include <vector>

static void get_packets_form_str(const std::string& str_packets, int len, std::vector<uint8_t>& packets){
    
    std::string str_val;
    str_val.clear();
    uint8_t val{0};

    for(char c : str_packets){
        if(c == ' '){
            val = static_cast<uint8_t>(std::stoul(str_val, nullptr, 16));
            packets.push_back(val);
            str_val.clear();
        }else{
            str_val += c;
        }
    }
    val = static_cast<uint8_t>(std::stoul(str_val, nullptr, 16));
    packets.push_back(val);

    if(len != packets.size()){
        throw std::invalid_argument("Enterd packet lenth and the string packets length not matched");
    }
}

int main(){

    int packets_length{0};
    std::string str_packets;
    try{

        std::cin >> packets_length;
        std::cin.ignore();
        getline(std::cin, str_packets);

        std::vector<uint8_t> packets;
        get_packets_form_str(str_packets, packets_length, packets);

        for(uint8_t packet : packets){
            printf("0x%X ", packet);
        }
        std::cout << '\n';

    }catch(std::exception& e){
        std::cout << "Expection: " << e.what() << '\n';
    }

    return 0;
}




