#include <string>
#include <fstream>
#include <iostream>
#include "DictionaryDash.h"

using namespace std;

int main(int argc, char** argv)
{
	string start;
	string target;
	std::ifstream in;
	std::streambuf *cinbuf = std::cin.rdbuf(); //save old buf
	if (argc > 1)
	{
		in.open(argv[1]);
		std::cin.rdbuf(in.rdbuf());
	}

	cin >> start;
	cin >> target;
	vector<string> words;

	string n;
	while (cin >> n)
	{
		words.push_back(n);
	}

	DictionaryDash dd_astar(words, start.size(), Algo::AStar);
	DictionaryDash::Path path_astar = dd_astar.path(start, target);
	cout << "Min num steps with astar: " << path_astar.pathLen()<< endl;

	DictionaryDash dd_bfs(words, start.size(), Algo::BreadthFirstSearch);
	DictionaryDash::Path path_bfs = dd_bfs.path(start, target);
	cout << "Min num steps with bfs: " << path_bfs.pathLen() << endl;


	return 0;
}
