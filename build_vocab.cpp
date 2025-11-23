#include <bits/stdc++.h>

using namespace std;

int get_utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

int main() {
    ifstream sf("shakespeare.txt");
    if (!sf.is_open()) {
        cerr << "Error: unable to open shakespeare.txt" << endl;
        return 1;
    }

    ifstream emo("emojis.txt");
    if (!emo.is_open()) {
        cerr << "Error: unable to open emojis.txt" << endl;
        return 1;
    }

    unordered_set<string> unique_chars;
    unordered_map<int, string> unique_char_map;

    string line;

    while (getline(sf, line)) {
        for (size_t i = 0; i < line.length(); ) {
            int len = get_utf8_char_len(line[i]);
            if (i + len > line.length()) break;

            string utf8_char = line.substr(i, len);
            unique_chars.insert(utf8_char);

            i += len;
        }
    }
    sf.close();

    while (getline(emo, line)) {
        for (size_t i = 0; i < line.length(); ) {
            int len = get_utf8_char_len(line[i]);
            if (i + len > line.length()) break;

            string utf8_char = line.substr(i, len);
            unique_chars.insert(utf8_char);

            i += len;
        }
    }
    emo.close();

    // now i need to map these unique strings to ids
    int id = 0;
    for (const auto& c : unique_chars) {
        unique_char_map[id++] = c;
    }

    ofstream out("unique_char_map.txt");
    for (auto& pair : unique_char_map) {
        out << pair.first << " : " << pair.second << endl;
    }

    return 0;
}
