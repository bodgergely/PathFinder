#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <set>
#include <fstream>
#include <queue>
#include <memory>
#include <limits>
#include <utility>


template<class NodeTy>
class NeighborGenerator
{
public:
	NeighborGenerator() {}
	virtual ~NeighborGenerator() {}
	virtual std::vector<NodeTy> generateNeighbors(NodeTy node) = 0;
};

class StringNeighborGenerator : public NeighborGenerator < std::string >
{
public:
	StringNeighborGenerator() {}
	virtual ~StringNeighborGenerator() {}
	virtual std::vector<std::string> generateNeighbors(std::string node) = 0;
};


class DictionaryDashNeighborGenerator : public StringNeighborGenerator
{
public:
	DictionaryDashNeighborGenerator(const std::unordered_set<std::string>& dict_) : _dict(dict_) {}
	virtual ~DictionaryDashNeighborGenerator() {}
	virtual std::vector<std::string> generateNeighbors(std::string node)
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

protected:
	const std::unordered_set<std::string>& _dict;
};

enum class Algo
{
	AStar,
	BreadthFirstSearch
};

template <class NodeTy>
class PathFinder
{
public:
	class Path
	{
	public:
		Path(const std::vector<NodeTy>& path_) : _path(path_){}
		int pathLen() const {return _path.size()-1;}
		const std::vector<NodeTy>& getPath() const {return _path;}
		std::string toString() const
		{
			std::stringstream ret;
			for(int i=_path.size()-1;i>=0;i--)
			{
				ret<<_path[i];
				if(i!=0)
					ret << "->";
			}
			return ret.str();
		}
	private:
		std::vector<NodeTy> _path;
	};
	PathFinder(const std::shared_ptr<NeighborGenerator<NodeTy>>& gen_) : _neighborGenerator(gen_){}
	virtual ~PathFinder() {}
	virtual typename PathFinder<NodeTy>::Path findPath(NodeTy start, NodeTy target) = 0;
protected:
	std::shared_ptr<NeighborGenerator<NodeTy>> _neighborGenerator;
};

namespace astar
{

	template<class NodeTy>
	struct Node
	{
		/*
		class Hash
		{
		public:
			unsigned operator()(const Node& node)
			{
				return std::hash<std::string>()(node.value);
			}
		};
		 */
		class Comparator
		{
		public:
			bool operator()(const Node<NodeTy>& lhs, const Node<NodeTy>& rhs)
			{
				if (lhs.f <= rhs.f)
					return true;
				else
					return false;
			}
		};


		Node() {}
		Node(NodeTy val_, unsigned g_, unsigned f_) : value(val_), g(g_), f(f_) {}
		Node(const Node& other) : value(other.value), g(other.g), f(other.f)
		{

		}
		Node& operator=(const Node& rhs)
		{
			this-> value = rhs.value;
			this->g = rhs.g;
			this->f = rhs.f;
			return *this;
		}
		NodeTy value;
		unsigned g;
		unsigned f;
	};



	template <class NodeTy, class Heuristic>
	class AStar : public PathFinder < NodeTy >
	{
		using CNode = Node < NodeTy > ;
		using Path = typename PathFinder<NodeTy>::Path;
	public:
		AStar(const std::shared_ptr<NeighborGenerator<NodeTy>>& gen_) : PathFinder<NodeTy>(gen_){}
		virtual ~AStar() {}
		virtual Path findPath(NodeTy start, NodeTy target)
		{
			std::unordered_set<NodeTy>				closedSet;
			std::unordered_map<NodeTy, CNode>		openSetLookup;
			std::set<CNode, typename CNode::Comparator>						openSetOrdered;
			std::unordered_map<NodeTy, NodeTy>   cameFrom;

			CNode startN(start, 0, Heuristic()(start, target));
			openSetOrdered.insert(startN);
			openSetLookup[start] = startN;

			if(startN.value == target)
			{
				return Path(std::vector<NodeTy>{start});
			}

			while (!openSetOrdered.empty())
			{
				const CNode currentN = *(openSetOrdered.begin());
				if (currentN.value == target)
					return reconstructPath(cameFrom, currentN.value);
				openSetOrdered.erase(openSetOrdered.begin());
				openSetLookup.erase(currentN.value);
				closedSet.insert(currentN.value);
				std::vector<NodeTy> nbs = this->_neighborGenerator->generateNeighbors(currentN.value);
				for (const NodeTy& n : nbs)
				{
					if (closedSet.find(n) != closedSet.end())
						continue;
					unsigned tentativeScore = currentN.g + 1;	// we add 1 as the distance from current to neighbor is always one
					const auto& it = openSetLookup.find(n);
					if (it != openSetLookup.end() && tentativeScore >= it->second.g)
						continue;

					CNode neighborNode(n, tentativeScore, tentativeScore + Heuristic()(n, target));
					cameFrom[n] = currentN.value;
					openSetOrdered.insert(neighborNode);
					openSetLookup[n] = neighborNode;
				}
			}

			return Path(std::vector<NodeTy>());

		}
	private:
		Path reconstructPath(const std::unordered_map<NodeTy, NodeTy>& cameFrom, const NodeTy& value)
		{
			std::vector<NodeTy> res;
			res.push_back(value);
			NodeTy current = value;
			auto it = cameFrom.find(current);
			while (it != cameFrom.end())
			{
				res.push_back(it->second);
				it = cameFrom.find(it->second);
			}
			return Path(res);
		}
	};

}

namespace bfs
{
	template<class NodeTy>
	struct Node
	{
		class Hash
		{
			unsigned operator()(const Node& node)
			{
				return std::hash<std::string>()(node.value);
			}
		};

		Node(NodeTy val_, Node* parent_, unsigned distance_) : value(val_), parent(parent_), distance(distance_) {}
		Node(const NodeTy& n) : value(n), parent(nullptr), distance(std::numeric_limits<unsigned>::max())
		{

		}
		NodeTy value;
		Node* parent;
		unsigned distance;
	};

	template <class NodeTy>
	class BreadthFirstSearch : public PathFinder < NodeTy >
	{
		using CNode = Node < NodeTy > ;
		using Path = typename PathFinder<NodeTy>::Path;
	public:
		BreadthFirstSearch(const std::shared_ptr<NeighborGenerator<NodeTy>>& gen_) : PathFinder<NodeTy>(gen_){}
		virtual ~BreadthFirstSearch() {}
		virtual Path findPath(NodeTy start, NodeTy target)
		{
			CNode* root = new CNode(start, nullptr, 0);
			std::queue<CNode*> q;
			q.push(root);

			std::unordered_set<NodeTy> seen;
			seen.insert(start);
			while (!q.empty())
			{
				CNode* current = q.front();
				q.pop();

				if(current->value == target)
				{
					return Path(std::vector<NodeTy>{start});
				}

				std::vector<NodeTy> nbs = this->_neighborGenerator->generateNeighbors(current->value);
				for (const NodeTy& n : nbs)
				{
					if (seen.find(n) == seen.end())
					{
						CNode* newnode = new CNode(n, current, current->distance + 1);
						if (n == target)
						{
							return reconstructPath(newnode);
						}
						q.push(newnode);
						seen.insert(n);
					}
				}
			}
			return Path(std::vector<NodeTy>());
		}
	private:
		Path reconstructPath(CNode* target)
		{
			std::vector<NodeTy> path;
			CNode* curr = target;
			while (curr)
			{
				path.push_back(curr->value);
				curr = curr->parent;
			}

			return Path(path);
		}
	};
}

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


