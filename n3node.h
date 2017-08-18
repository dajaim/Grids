#include<map>
#include<vector>

#ifndef N3NODE_H
#define N3NODE_H

namespace magi {

	template<class T, class C = int>
	class N3node;
	//enum Direction { LEFT = 1, MID, RIGHT };// <- not necessary i think
	template<class T, class C = int>
	class N3tree;
}

template<class T, class C>
class magi::N3node {
	friend magi::N3tree<T, C>;
	N3node() { this->setMid(true); this->setup(); }
	void setDirection(const char & c);
	void setNext( magi::N3node<T, C>*  n3n, const char &pos) { this->relations[pos + 2] = n3n; }
	void setPar( magi::N3node<T, C> *  n3n) { this->relations[0] = n3n; }
	void setIt(typename std::map<T, magi::N3node<T, C>*>::iterator* it) { this->itptr = it; }
	bool isMid();
	bool isEnd();
	char getDirection();
	std::vector<magi::N3node<T, C>*>getNext();
	typename std::map<T, magi::N3node<T, C>*>::iterator* getIt() { return this->itptr; }
	magi::N3node<T, C>* getPar() { return this->relations[0]; }

	/*
	* XXXXXX11 = -1 \
	* XXXXXX00 = 0 - direction in relation to parent
	* XXXXXX01 = 1 /
	* XXXXX0XX = isMid-Flag
	* 5 bits free !!!
	*  maybe multithread support in next release...
	*	- ( lock nodes?)
	*/
	unsigned char status;

	/*
	* parent @ 0
	* // left, mid right @ 1 to @ 3 (only if isMid - flag = 1)
	* // else @ 1 std::map<T,magi::N3node<T,C>*>::iterator *
	*/
	std::vector<magi::N3node<T,C>* >relations;// parent, left, mid, right
	typename std::map<T, magi::N3node<T, C>*>::iterator* itptr;// != NULL if(this->isEnd())
	void resetDirection();
	void setMid(const bool & mid);
	void setup();
};

template<class T, class C>
std::vector<magi::N3node<T, C>*>magi::N3node<T, C>::getNext() {
	std::vector<magi::N3node<T, C>*>res;
	for (unsigned i = 1; i< this->relations.size(); ++i)
		if (this->relations[i] != NULL)
			res.push_back(this->relations[i]);
	return res;
}

template<class T, class C>
void magi::N3node<T, C>::setMid(const bool &mid) {
	unsigned char tmp = mid ? 4 : 0;
	this->status |= tmp;
}

template<class T, class C>
void magi::N3node<T, C>::setup() {
	this->status = 0;
	this->relations.resize(4);
}

template<class T, class C>
void magi::N3node<T, C>::resetDirection() {
	this->status &= 252;
}

template<class T, class C>
char magi::N3node<T, C>::getDirection() {
	unsigned char tmp = 3;
	tmp &= this->status;
	if (tmp == 3)
		return (-1);
	return tmp;
}

template<class T, class C>
void magi::N3node<T, C>::setDirection(const char &c) {
	if (c< (-1) || c >1)
		throw std::exception();
	this->resetDirection();
	if (c == (-1))
		this->status |= 3;
	else
		this->status |= c;
}

template<class T, class C>
bool magi::N3node<T, C>::isMid() {
	unsigned char tmp = 4;
	tmp &= this->status;
	return (tmp != 0);
}

template<class T, class C>
bool magi::N3node<T, C>::isEnd() {
	return (!this->isMid());
}

#endif // N3NODE_H
