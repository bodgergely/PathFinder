#ifndef DICTIONARY_DASH_H
#define DICTIONARY_DASH_H

#include "PathFinder.h"
#include "NeighborGenerator.h"
#include "AStar.h"
#include "BFS.h"
#include <memory>
#include <string>
#include <unordered_set>



enum class Algo
{
	AStar,
	BreadthFirstSearch
};


class DictionaryDash
{
public:
	using Path = PathFinder<std::string>::Path;
	DictionaryDash(const std::vector<std::string>& dictionary, unsigned wordLen, Algo algo);
	~DictionaryDash() {}
	/*
	 * Return -1 if unreachable
	 * */
	Path path(const std::string& start, const std::string& target);
private:
	void _populateDictionary(const std::vector<std::string>& dictionary);

private:
	std::unordered_set<std::string> _dict;
	unsigned      _wordLen;
	const std::string  _start;
	const std::string  _target;
	std::unique_ptr<PathFinder<std::string>>	  _pathFinder;

};

class DictionaryDashNeighborGenerator : public StringNeighborGenerator
{
public:
	DictionaryDashNeighborGenerator(const std::unordered_set<std::string>& dict_);
	virtual ~DictionaryDashNeighborGenerator();
	virtual std::vector<std::string> generateNeighbors(std::string node);

protected:
	const std::unordered_set<std::string>& _dict;
};

/*
 * Heuristic function for informed exploration
 * */

class CharDifference
{
public:
	unsigned operator()(const std::string& current, const std::string& target)
	{
		unsigned count = 0;
		for (int i = 0; i < current.size(); i++)
		{
			if (current[i] != target[i])
			{
				count++;
			}
		}
		return count;
	}
};


#endif
