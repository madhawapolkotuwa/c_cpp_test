/*
* A gateway routes published MQTT messages to subscribers. Implement MQTT 3.1.1 topic filter matching.   
* Topic names and filters are split into levels by '/'.
* a level may be empty (for example 'a//b' has three levels, '/x' has two, the first empty). 
* In a filter, '+' is a single-level wildcard: it must occupy an entire level and matches exactly one level (which may be empty). 
* '#' is a multi-level wildcard: it must occupy an entire level and must be the last level of the filter; 
* it matches its parent level and any number of following levels (so 'a/#' matches 'a', 'a/b' and 'a/b/c'). 
* Any other use of '+' or '#' makes the filter INVALID. 
* All other characters match literally and case-sensitively. 
* A topic name whose first character is '$' is never matched by a filter whose first level is '+' or '#' (so '#' does not match '$SYS/x', but '$SYS/#' does).

* Input/output: Line 1: N. Next N lines: one filter each (filter indices are 0-based in input order). 
* Next line: M. Next M lines: one topic name each. Output: first, for every invalid filter in input order, a line INVALID <index>. Then, for each topic in order, one line with the indices of all valid filters that match it, ascending, separated by single spaces, or - if none match.

* Constraints: 1 <= N <= 200; 1 <= M <= 2000. Filters and topics are 1..256 printable ASCII characters with no spaces. Topic names never contain '+' or '#'.

Input:
```
4
sport/tennis/+
sport/#
#
+/+
3
sport/tennis/player1
sport
news/today
```
Expected output:
```
0 1 2
1 2
2 3
```
*/

#include <iostream>
#include <string>
#include <vector>

// Split on '/', keeping empty levels: "a//b" -> {"a","","b"}, "a/" -> {"a",""}.
static std::vector<std::string> split_levels(const std::string &s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == '/') {
            out.push_back(cur);
            cur.clear();
        } else {
            cur += c;
        }
    }
    out.push_back(cur);
    return out;
}

// Return true if f is a valid MQTT topic filter.
static bool is_valid_filter(const std::string &f) {
    std::vector<std::string> lv = split_levels(f);
    for (size_t i = 0; i < lv.size(); i++) {
        const std::string &l = lv[i];
        bool has_wild = l.find('+') != std::string::npos ||
                        l.find('#') != std::string::npos;
        if (!has_wild) continue;
        if (l == "+") continue;                          // '+' must be a whole level
        if (l == "#" && i + 1 == lv.size()) continue;    // '#' must be whole and last
        return false;
    }
    return true;
}

// Return true if the valid filter f matches the topic name.
static bool matches(const std::string &f, const std::string &topic) {
    std::vector<std::string> fl = split_levels(f);
    std::vector<std::string> tl = split_levels(topic);

    // '$' topics are never matched by a filter starting with '+' or '#'.
    if (topic[0] == '$' && (fl[0] == "+" || fl[0] == "#")) return false;

    size_t i = 0;
    for (; i < fl.size(); i++) {
        if (fl[i] == "#") return true;        // matches parent and all descendants
        if (i >= tl.size()) return false;     // topic ran out of levels
        if (fl[i] == "+") continue;           // exactly one level (may be empty)
        if (fl[i] != tl[i]) return false;     // literal, case-sensitive
    }
    return i == tl.size();                    // all topic levels consumed
}

int main() {
    std::ios::sync_with_stdio(false);

    int n, m;

    if (!(std::cin >> n)) return 0;

    std::vector<std::string> filters(n);

    std::vector<bool> valid(n);

    for (int i = 0; i < n; i++) {

        std::cin >> filters[i];
        valid[i] = is_valid_filter(filters[i]);

        if (!valid[i]) std::cout << "INVALID " << i << "\n";
    }

    if (!(std::cin >> m)) return 0;
    
    for (int j = 0; j < m; j++) {
        std::string topic;
        if (!(std::cin >> topic)) break;
        bool any = false;
        for (int i = 0; i < n; i++) {
            if (valid[i] && matches(filters[i], topic)) {
                std::cout << (any ? " " : "") << i;
                any = true;
            }
        }
        std::cout << (any ? "" : "-") << "\n";
    }
    return 0;
}
