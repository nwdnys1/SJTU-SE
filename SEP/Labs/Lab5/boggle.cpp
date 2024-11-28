#include "lexicon.h"
#include <iostream>
#include <string>
#include <stack> 
#include <set>
using namespace std;
Lexicon dictionary("./EnglishWords.txt");

class Boggle {
public:
	string* lines = NULL;
	char* letters = NULL;
	int n;
	int player_score = 0;
	int computer_score = 0;
	string player_words = " ";
	string player_wordslower=" ";
	set<string> computer_words;
	set<string> all_words;

	Boggle(){
		cin >> n;
		lines = new string[n];
		for (int i = 0; i < n; i++) {
			cin >> lines[i];
		}
		letters = new char[n * n];
		for (int i = 0; i < n ; i++) {
			for (int j = 0; j < n ; j++) {
				letters[i * n + j] = lines[i][j];
			}
		}
	}

	void search(int i,int j, string s, int* visited) {		
		if (i < 0 || i >= n || j < 0 || j >= n || visited[i * n + j])	return;
		s += letters[i * n + j];
		if (s.size() >= 4 && computer_words.find(s) == computer_words.end() && dictionary.contains(s)) {
			computer_words.insert(s);
			computer_score += (s.size() - 3);
		}
		if (dictionary.containsPrefix(s)) {
			for (int m = -1; m <= 1; ++m) {
				for (int k = -1; k <= 1; ++k) {
					if (m != 0 || k != 0) {
						visited[i * n + j] = true;
						search(i + m, j + k, s , visited);
						visited[i * n + j] = false;
					}
				}
			}
		}		
	}
};
int main() {
	// TODO
	Boggle b;
	int* visited = new int[b.n * b.n]();
	string in = "";
	string in_low = "";
	string in_up = "";
	for (int i = 0; i < b.n; ++i) {
		for (int j = 0; j < b.n; ++j) {
			string s = "";
			b.search(i, j, s, visited);
		}
	}
	b.all_words = b.computer_words;
	while (1) {
		for (int i = 0; i < b.n; i++) {
			cout << b.lines[i] << endl;
		}
		cout << "Your Score: " << b.player_score << endl;
		cout << "Your Words: " << b.player_words << endl;
		
		cin >> in;
		if (in == "???") break;
		in_low = in;
		in_up = in;
		for (int i = 0; i < in.size(); i++) {
			in_up[i] = toupper(in[i]);
			in_low[i] = tolower(in[i]);
		}
		if (in.size() >= 4) {
			if (dictionary.contains(in)) {
				if (b.all_words.find(in_up)!= b.all_words.end()) {
					if (b.player_wordslower.find(" "+in_low+" ") == string::npos) {
						b.player_words += in + " ";
						b.player_wordslower += in_low + " ";
						b.player_score += in.size() - 3;
						b.computer_words.erase(in_up);
						b.computer_score -= in.size() - 3;
					}
					else cout << in_low << " is already found." << endl;
				}
				else cout << in_low << " is not on board." << endl;
			}
			else cout << in_low << " is not a word." << endl;
		}
		else cout << in_low << " is too short." << endl;	
	}
	cout << "Computer Score: " << b.computer_score << endl;
	cout << "Computer Words: " ;
	for (string word : b.computer_words) {
		cout << word << ' ';
	}
	

	return 0;
}
