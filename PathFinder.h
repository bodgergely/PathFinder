#ifndef PATHFINDER_H
#define PATHFINDER_H
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <set>
#include "NeighborGenerator.h"

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


#endif
