
#include "DictionaryDash.h"

DictionaryDashNeighborGenerator::DictionaryDashNeighborGenerator(const std::unordered_set<std::string>& dict_) : _dict(dict_) {}
DictionaryDashNeighborGenerator::~DictionaryDashNeighborGenerator() {}

std::vector<std::string> DictionaryDashNeighborGenerator::generateNeighbors(std::string node)
{
	std::vector<std::string> nb;
	for (int i = 0; i < node.size(); i++)
	{
		char orr = node[i];
		for (char c = 'a'; c <= 'z'; c++)
		{
			node[i] = c;
			if (node[i] == orr)
				continue;
			if (_dict.find(node) != _dict.end())
			{
				nb.push_back(node);
			}
		}
		node[i] = orr;
	}

	return nb;
}

DictionaryDash::DictionaryDash(const std::vector<std::string>& dictionary, unsigned wordLen, Algo algo) :
_wordLen(wordLen)
{
	_populateDictionary(dictionary);

	std::shared_ptr<DictionaryDashNeighborGenerator> nb(new DictionaryDashNeighborGenerator(_dict));
	if (algo == Algo::AStar)
		_pathFinder = std::unique_ptr<PathFinder<std::string>>(new astar::AStar<std::string,CharDifference>(nb));
	else
		_pathFinder = std::unique_ptr<PathFinder<std::string>>(new bfs::BreadthFirstSearch<std::string>(nb));


}

void DictionaryDash::_populateDictionary(const std::vector<std::string>& dictionary)
{
	for (const std::string& s : dictionary)
	{
		if (s.size() != _wordLen)
		{
			std::stringstream err;
			err << "Word len is bad at: " << s;
			throw std::logic_error(err.str());
		}
		_dict.insert(s);
	}
}

DictionaryDash::Path DictionaryDash::path(const std::string& start, const std::string& target)
{
	DictionaryDash::Path path =  _pathFinder->findPath(start, target);
	return path;
}
