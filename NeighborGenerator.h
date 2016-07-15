#ifndef NEIGHBORGENERATOR_H
#define NEIGHBORGENERATOR_H

#include <string>

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


#endif
