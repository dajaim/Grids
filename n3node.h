// author: Daniel Jan Imiela

#include<map>
#include<vector>

#ifndef N3NODE_H
#define N3NODE_H

namespace magi {

	// v 1.2 ( seems to work)

	template<class T, class C>
	class N3node;
	template<class T, class C>
	class N3tree;
}

template<class T, class C>
class magi::N3node {
	friend class magi::N3tree<T, C>;
	N3node();
	~N3node();
	void setDirection(const char & c);
	void setNext(magi::N3node<T, C>*  n3n, const char &pos);
	void setNext(const std::vector<magi::N3node<T, C> * > & vec);
	void setPar(magi::N3node<T, C> *  n3n);
	void setEnd();
	void setMid();
	void setTarget(typename std::map<T, magi::N3node<T, C>*>::iterator* it);
	void resetDirection();
	bool isMid()const;
	bool isTarget()const;
	char getDirection()const;
	std::vector<magi::N3node<T, C>*>&getNext();
	magi::N3node<T, C>*& getNext(const char & c);
	typename std::map<T, magi::N3node<T, C>*>::iterator*& getIt();
	magi::N3node<T, C>* & getPar();
	/*
	* XXXXXX11 = -1 \
	* XXXXXX00 = 0 - direction in relation to parent
	* XXXXXX01 = 1 /
	* XXXXXX10 = no direction, will return nothing in request but has to return char => error
	* XXXXX0XX = end-bit, 0 = isNext, 1 = isEnd default is 0, guess it is unnecessary
	* 5 bits free !!!
	*/
	magi::N3node<T, C>*parent;
	unsigned char status;
	typename std::map<T, magi::N3node<T, C>*>::iterator * target;
	std::vector<magi::N3node<T, C>*> * next;
};

template<class T, class C>
magi::N3node<T, C>::~N3node() {
	if (this->target != NULL)
		delete this->target;
	if (this->next != NULL)
		delete this->next;
}

template<class T, class C>
magi::N3node<T, C>*& magi::N3node<T, C>::getNext(const char & c) {
	return this->next->operator[](c + 1);
}

template<class T, class C>
std::vector<magi::N3node<T, C>*>& magi::N3node<T, C>::getNext() {
	return (*this->next);
}

template<class T, class C>
void magi::N3node<T, C>::setEnd() {
	this->status |= 4;
}

template<class T, class C>
void magi::N3node<T, C>::setMid() {
	this->status &= 251;
	this->next = new std::vector<magi::N3node<T, C>*>;
	this->next->resize(3);
}

template<class T, class C>
void magi::N3node<T, C>::setTarget(typename std::map<T, magi::N3node<T, C>*>::iterator * it) {
	this->target = it;
}

template<class T, class C>
magi::N3node<T, C>*& magi::N3node<T, C>::getPar() {
	return this->parent;
}

template<class T, class C>
bool magi::N3node<T, C>::isMid()const {
	return (this->target == NULL);
}

template<class T, class C>
bool magi::N3node<T, C>::isTarget()const {
	return (this->target != NULL);
}

template<class T, class C>
void magi::N3node<T, C>::setPar(magi::N3node<T, C>* n3n) {
	this->parent = n3n;
}

template<class T, class C>
magi::N3node<T, C>::N3node() {
	this->next = NULL;
	this->target = NULL;
	this->status = 2;
}

template<class T, class C>
void magi::N3node<T, C>::setNext(magi::N3node<T, C>* n3n, const char &pos) {
	this->next->operator[](pos + 1) = n3n;
}

template<class T, class C>
void magi::N3node<T, C>::setNext(const std::vector<magi::N3node<T, C>*>& vec) {
	(*this->next) = vec;
}

template<class T, class C>
char magi::N3node<T, C>::getDirection() const {
	unsigned char res = 3;
	res &= this->status;
	switch (res) {
	case 0: return 0;
	case 1: return 1;
	case 3: return -1;
	}
}

template<class T, class C>
typename std::map<T, magi::N3node<T, C>*>::iterator *& magi::N3node<T, C>::getIt() {
	return this->target;
}

template<class T, class C>
void magi::N3node<T, C>::resetDirection() {
	this->status &= 252;
}

template<class T, class C>
void magi::N3node<T, C>::setDirection(const char &c) {
	this->resetDirection();
	if (c == (-1))
		this->status |= 3;
	else
		this->status |= c;
}

#endif // N3NODE_H