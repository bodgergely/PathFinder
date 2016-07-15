#ifndef BFS_H
#define BFS_H

#include "PathFinder.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <limits>

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

	/*
	 * The brute force shortest path finder - we can test our A* algo against this one
	 * */

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

#endif
