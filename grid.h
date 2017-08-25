#include<vector>

#ifndef GRID
#define GRID
namespace magi{
	template<class T, class C = int>
	class Grid;	
}

template<class T, class C >
class magi::Grid{
	public:
		//destructor
		virtual ~Grid(){}
		//returns a vector of objects, which are the closests to "position"
        //virtual std::vector<T> area(const std::vector<C>&position, const C & range = 1) = 0;
		//inserts an object at a position
        virtual int add(const std::vector<C>& position, const T & obj) = 0;
		//virtual int del(const std::vector<C>& vec) = 0;
		//deletes a position 
        virtual bool del(const T & obj) = 0;
		//returns position of the requested object
        virtual std::vector<C> where(const T & obj)= 0;
		//clears n3tree
        virtual void clear() = 0;
};
#endif
