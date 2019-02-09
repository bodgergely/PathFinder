#include "pch.h"

#include "../DictionaryDash.h"
#include <random>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <cstring>
#include <algorithm>


TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}



using namespace std;

using Path = DictionaryDash::Path;



string generateRandomWord(int len)
{
	string s(len, 'a');
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::minstd_rand0 generator(seed);
	std::uniform_int_distribution<int> distribution(0, 'z' - 'a');
	for (int i = 0; i < len; i++)
	{
		int num = distribution(generator);
		char c = 'a' + num;
		s[i] = c;
	}

	return s;
}

void populateDictionary(unordered_set<string>& wordSet, int i)
{
#ifndef _WIN32
	const char* dic = "/usr/share/dict/words";
#else
	const char* dic = "C://Users///bodge///dict/words.txt";
#endif
	ifstream dictFile(dic, std::ifstream::in);
	string word;
	while (getline(dictFile, word))
	{
		if (word.size() == i && !strchr(word.c_str(), '\''))
		{
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
			wordSet.insert(word);
		}
	}
}

TEST(DictionaryDash, sample)
{
	string start = "hit";
	string target = "cog";
	vector<string> words{ "hit","dot","dog","cog","hot" };
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(4, astarPath.pathLen());
	ASSERT_EQ(4, bfsPath.pathLen());
}

TEST(DictionaryDash, unreachable)
{
	string start = "hit";
	string target = "cog";
	vector<string> words{ "hit","dog","cog" };
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(-1, astarPath.pathLen());
	ASSERT_EQ(-1, bfsPath.pathLen());
}

TEST(DictionaryDash, reachItself)
{
	string start = "hit";
	string target = "hit";
	vector<string> words{ "hit","dot","dog","cog","hot" };
	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	Path astarPath = dd_astar.path(start, target);
	Path bfsPath = dd_bfs.path(start, target);
	ASSERT_EQ(0, astarPath.pathLen());
	ASSERT_EQ(0, bfsPath.pathLen());
}


struct TimingAvgs
{
	TimingAvgs() : astar(0.0), bfs(0.0) {}
	TimingAvgs(double astar_, double bfs_) : astar(astar_), bfs(bfs_) {}
	double astar;
	double bfs;
};

TimingAvgs testWordSize(int wordlen, bool verbose)
{
	unordered_set<string> wordSet;
	vector<string> words;
	if (verbose)
		cout << "Word size: " << wordlen << endl;
	populateDictionary(wordSet, wordlen);
	for (const string& w : wordSet)
	{
		words.push_back(w);
	}
	wordSet.clear();

	double astarDurAvg = 0;
	double bfsDurAvg = 0;

	int attempts = 0;
	while (true)
	{
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::minstd_rand0 generator(seed);
		std::uniform_int_distribution<int> distribution(0, words.size() - 1);
		string start = words[distribution(generator)];
		string target = words[distribution(generator)];


		DictionaryDash dd_astar(words, start.size(), Algo::AStar);
		DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);

		auto startAStar = std::chrono::system_clock::now();
		Path astarPath = dd_astar.path(start, target);
		auto endAStar = std::chrono::system_clock::now();
		Path bfsPath = dd_bfs.path(start, target);
		auto endBFS = std::chrono::system_clock::now();


		auto astarDur = std::chrono::duration_cast<std::chrono::microseconds>(endAStar - startAStar);
		auto bfsDur = std::chrono::duration_cast<std::chrono::microseconds>(endBFS - endAStar);
		++attempts;
		astarDurAvg += (double)astarDur.count(); astarDurAvg /= (double)attempts;
		bfsDurAvg += (double)bfsDur.count(); bfsDurAvg /= (double)attempts;

		EXPECT_EQ(astarPath.pathLen(), bfsPath.pathLen());
		if (astarPath.pathLen() != -1)
		{
			if (verbose)
			{
				cout << "start: " << start << " target: " << target << endl;
				cout << "AStar steps: " << astarPath.pathLen() << " Path: " << astarPath.toString() << endl;
				cout << "BFS steps: " << bfsPath.pathLen() << " Path: " << bfsPath.toString() << endl;
			}
			break;
		}
	}

	return TimingAvgs(astarDurAvg, bfsDurAvg);
}

map<int, TimingAvgs> testWordSizes(int minLen, int maxLen, bool verbose)
{
	map<int, TimingAvgs> timings;
	for (int wordlen = minLen; wordlen <= maxLen; wordlen++)
	{
		TimingAvgs times = testWordSize(wordlen, verbose);
		timings[wordlen] = times;
	}
	return timings;
}

TEST(DictionaryDash, realDictionary)
{
	testWordSizes(3, 8, true);
}

TEST(DictionaryDash, realDictionaryStressTest)
{
	int totalTests = 15;
	map<int, TimingAvgs> stressTimes;
	for (int i = 1; i <= totalTests; i++)
	{
		cout << "Test: " << i << " of " << totalTests << endl;
		map<int, TimingAvgs> times = testWordSizes(3, 8, false);
		for (const pair<int, TimingAvgs>& time : times)
		{
			TimingAvgs& avg = stressTimes[time.first];
			avg.astar += time.second.astar; avg.astar /= (double)i;
			avg.bfs += time.second.bfs; avg.bfs /= (double)i;
		}
	}

	cout << "Avg calculation times (microsecs): \n";
	for (const auto& p : stressTimes)
	{
		cout << "Word len : " << p.first << "\tAStar: " << p.second.astar << " BFS: " << p.second.bfs << " Diff: " << abs(p.second.astar - p.second.bfs) << endl;
	}


}


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
