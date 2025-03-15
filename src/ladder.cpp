#include "ladder.h"
#include <cassert>
#include <unordered_set>

using namespace std;

void error(string word1, string word2, string msg) {
    if (word1.empty() && word2.empty()) {
        cout << "Error: " << msg << endl;
    } else {
        cout << "Error (" << word1 << " -> " << word2 << "): " << msg << endl;
    }
    exit(1);
}

bool edit_distance_within(const string& str1, const string& str2, int d) {
    int len1 = str1.length();
    int len2 = str2.length();
    if (abs(len1 - len2) > d) return false;

    vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, 0));
    
    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0) dp[i][j] = j;
            else if (j == 0) dp[i][j] = i;
            else if (str1[i-1] == str2[j-1]) dp[i][j] = dp[i-1][j-1];
            else dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    }
    return dp[len1][len2] <= d;
}

// Adjacency check (required by tests)
bool is_adjacent(const string& word1, const string& word2) {
    return edit_distance_within(word1, word2, 1);
}

// Optimized BFS implementation
vector<string> generate_word_ladder(const string& begin_word, const string& end_word, const set<string>& word_list) {
    queue<vector<string>> ladder_queue;
    ladder_queue.push({begin_word});
    unordered_set<string> visited;
    visited.insert(begin_word);
    unordered_set<string> dict(word_list.begin(), word_list.end());

    while (!ladder_queue.empty()) {
        auto ladder = ladder_queue.front();
        ladder_queue.pop();
        string current = ladder.back();

        // Generate 1-edit neighbors directly
        vector<string> neighbors;
        
        // Substitutions
        for (size_t i = 0; i < current.size(); ++i) {
            string temp = current;
            for (char c = 'a'; c <= 'z'; ++c) {
                temp[i] = c;
                if (temp != current && dict.count(temp)) {
                    neighbors.push_back(temp);
                }
            }
        }

        // Insertions
        for (size_t i = 0; i <= current.size(); ++i) {
            for (char c = 'a'; c <= 'z'; ++c) {
                string temp = current.substr(0, i) + c + current.substr(i);
                if (dict.count(temp)) {
                    neighbors.push_back(temp);
                }
            }
        }

        // Deletions
        for (size_t i = 0; i < current.size(); ++i) {
            string temp = current.substr(0, i) + current.substr(i + 1);
            if (dict.count(temp)) {
                neighbors.push_back(temp);
            }
        }

        sort(neighbors.begin(), neighbors.end());

        // Process neighbors
        for (const auto& neighbor : neighbors) {
            if (!visited.count(neighbor)) {
                visited.insert(neighbor);
                vector<string> new_ladder = ladder;
                new_ladder.push_back(neighbor);
                if (neighbor == end_word) return new_ladder;
                ladder_queue.push(new_ladder);
            }
        }
    }
    return {};
}


// Load words (matches header declaration exactly)
void load_words(set<string>& word_list, const string& file_name) {
    ifstream file(file_name);
    if (!file) error("", "", "Can't open dictionary file");
    
    string word;
    while (file >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        word_list.insert(word);
    }
}

void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No word ladder found." << endl;
    } else {
        cout << "Word ladder found: ";
        for (const string& word : ladder) {
            cout << word << " ";
        }
        cout << endl;
    }
}

void verify_word_ladder() {
    set<string> word_list;
    load_words(word_list, "words.txt");
    assert(generate_word_ladder("cat", "dog", word_list).size() == 4);
    assert(generate_word_ladder("marty", "curls", word_list).size() == 6);
    assert(generate_word_ladder("code", "data", word_list).size() == 6);
    assert(generate_word_ladder("work", "play", word_list).size() == 6);
    assert(generate_word_ladder("sleep", "awake", word_list).size() == 8);
    assert(generate_word_ladder("car", "cheat", word_list).size() == 4);
}