#include "dicdash.h"
#include <gtest/gtest.h>
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <cstring>
#include <algorithm>

using namespace std;

using Path = DictionaryDash::Path;

string generateRandomWord(int len)
{
	string s(len,'a');
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand0 generator (seed);
	std::uniform_int_distribution<int> distribution(0,'z'-'a');
	for(int i=0;i<len;i++)
	{
		int num = distribution(generator);
		char c = 'a' + num;
		s[i] = c;
	}

	return s;
}

#ifndef _WIN32
void populateDictionary(unordered_set<string>& wordSet, int i)
{
	ifstream dictFile("/usr/share/dict/words", std::ifstream::in);
	string word;
	while(getline(dictFile, word))
	{
		if(word.size()== i && !strchr(word.c_str(), '\''))
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
			wordSet.insert(word);
		}
	}
}
#endif

TEST(DictionaryDash,sample)
{
	string start = "hit";
	string target = "cog";
	vector<string> words {"hit","dot","dog","cog","hot"};
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(4,astarPath.pathLen());
	ASSERT_EQ(4,bfsPath.pathLen());
}

TEST(DictionaryDash,unreachable)
{
	string start = "hit";
	string target = "cog";
	vector<string> words {"hit","dog","cog"};
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(-1,astarPath.pathLen());
	ASSERT_EQ(-1,bfsPath.pathLen());
}

TEST(DictionaryDash,reachItself)
{
	string start = "hit";
	string target = "hit";
	vector<string> words {"hit","dot","dog","cog","hot"};
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(0,astarPath.pathLen());
	ASSERT_EQ(0,bfsPath.pathLen());
}

#ifndef _WIN32

TEST(DictionaryDash, realDictionary)
{
	unordered_set<string> wordSet;
	vector<string> words;
	for(int i=3;i<9;i++)
	{
		cout << "Word size: " << i << endl;
		populateDictionary(wordSet, i);
		for(const string& w : wordSet)
		{
			words.push_back(w);
		}
		wordSet.clear();

		int attempts = 0;
		while(true)
		{
			//cout << "Attempts to reach: " << ++attempts << endl;
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
			std::minstd_rand0 generator (seed);
			std::uniform_int_distribution<int> distribution(0,words.size()-1);
			string start = words[distribution(generator)];
			string target = words[distribution(generator)];

			//cout << "start: " << start << " target: " << target << endl;

			DictionaryDash dd_astar(words, start.size(), Algo::AStar);
			DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
			Path astarPath = dd_astar.path(start, target);
			Path bfsPath = dd_bfs.path(start, target);
			EXPECT_EQ(astarPath.pathLen(),bfsPath.pathLen());
			if(astarPath.pathLen()!=-1)
			{
				cout << "start: " << start << " target: " << target << endl;
				cout << "AStar steps: " << astarPath.pathLen() << " Path: " << astarPath.toString() << endl;
				cout << "BFS steps: " << bfsPath.pathLen() << " Path: " << bfsPath.toString() << endl;
				break;
			}
		}

		words.clear();


	}

}

#endif

/*
 * Combinatorial explosion makes the below test useless
 * */

/*
TEST(DictionaryDash, random_8)
{
	unordered_set<string> wordSet;
	vector<string> words;
	int wordNum = 100000;
	while(true)
	{
		wordSet.insert(generateRandomWord(8));
		if(wordSet.size() == wordNum)
			break;
	}

	for(const string& w : wordSet)
	{
		words.push_back(w);
	}

	wordSet.clear();

	int attempts = 0;
	while(true)
	{
		cout << "Attempts to reach: " << ++attempts << endl;
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::minstd_rand0 generator (seed);
		std::uniform_int_distribution<int> distribution(0,wordNum-1);
		string start = words[distribution(generator)];
		string target = words[distribution(generator)];

		cout << "start: " << start << " target: " << target << endl;

		DictionaryDash dd_astar(words, start.size(), Algo::AStar);
		DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
		int stepsAstar = dd_astar.path(start, target);
		int stepsBfs = dd_bfs.path(start, target);
		ASSERT_EQ(stepsAstar,stepsBfs);
		if(stepsAstar!=-1)
		{
			cout << "AStar steps: " << stepsAstar << endl;
			cout << "BFS steps: " << stepsBfs << endl;
			break;
		}
	}
}
*/
