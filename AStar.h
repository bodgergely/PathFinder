#ifndef ASTAR_H
#define ASTAR_H

#include "PathFinder.h"
#include <unordered_set>
#include <unordered_map>

namespace astar
{

	template<class NodeTy>
	struct Node
	{
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


	/*
	 * Informed shortest path finder - pruning the search tree
	 * */

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
			std::unordered_set<NodeTy>					    closedSet;
			std::unordered_map<NodeTy, CNode>				openSetLookup;
			std::set<CNode, typename CNode::Comparator>		openSetOrdered;
			std::unordered_map<NodeTy, NodeTy>   		    cameFrom;

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

#endif
