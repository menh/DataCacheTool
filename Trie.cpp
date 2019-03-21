#include <iostream>
#include <vector>
#include <map>
using namespace std;

class Trie {
	private:
		struct TrieNode{
			TrieNode* next[26];
			string word;
		};
public:
	TrieNode* buidTrie(const vector<string>& words)
	{
		TrieNode* root = new TrieNode();
		
		for(int words_i = 0; words_i < words.size(); ++words_i)
		{
			
			TrieNode* tempNode = root;
			string word = words[words_i];
			for(int word_i = 0; word_i < word.length(); ++word_i)
			{
				if(tempNode->next[word[word_i] - 'a'] == nullptr)
				{
					tempNode->next[word[word_i] - 'a'] = new TrieNode();
				
				}
				tempNode = tempNode->next[word[word_i] - 'a'];
			}
			tempNode->word = word;
		}
		return root;
	}
};
int main()
{	
	return 0;
}

