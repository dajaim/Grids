// author: Daniel Jan Imiela

#include<map>
#include<vector>

#ifndef N3NODE_H
#define N3NODE_H

namespace magi {

	// v 1.3 ( seems to work)

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
	void setEoN();
	void setTarget(typename std::map<T, magi::N3node<T, C>*>::iterator* it);
	void resetDirection();
	bool isEoN()const;
	bool isMid()const;
	bool isTarget()const;
	char getDirection()const;
	std::vector<magi::N3node<T, C>*>&getNext();
	magi::N3node<T, C>* &getNext(const char & c);
	typename std::map<T, magi::N3node<T, C>*>::iterator*& getIt();
	magi::N3node<T, C>* &getPar();

	// parent
	magi::N3node<T, C>*parent;
	// std::vector<magi::N3node<T,C>*>* XOR typename std::map<T,magi::N3node<T,C>*>*
	void * data;
	/*
	* XXXXXX11 = -1 \
	* XXXXXX00 = 0  - direction in relation to parent
	* XXXXXX01 = 1  /
	* XXXXXX10 = no direction, will return nothing in request but has to return char => error
	* XXXXX0XX = end-bit, 0 = isNext, 1 = isEnd
	* 5 bits free !!!
	*/
	unsigned char status;
};

template<class T,class C>
void magi::N3node<T, C>::setEoN() {
	this->status |= 8;
}

template<class T, class C>
bool magi::N3node<T, C>::isEoN()const {
	unsigned char res = 8;
	res &= this->status;
	return (res == 0);
}

template<class T, class C>
magi::N3node<T, C>::~N3node() {
	if (this->data == NULL)
		return;
	if (this->isMid())
		delete (std::vector<magi::N3node<T, C>*>*)this->data;
	else
		delete (typename std::map<T, magi::N3node<T, C>*>::iterator*)this->data;
}

template<class T, class C>
magi::N3node<T, C>*& magi::N3node<T, C>::getNext(const char & c) {
	std::vector<magi::N3node<T, C>*> & ref = (*(std::vector<magi::N3node<T, C>*>*)this->data);
	return ref[c + 1];
}

template<class T, class C>
std::vector<magi::N3node<T, C>*>& magi::N3node<T, C>::getNext() {
	return (*(std::vector<magi::N3node<T, C>*>*)this->data);
}

template<class T, class C>
void magi::N3node<T, C>::setEnd() {
	if (this->isMid())
		delete (std::vector<magi::N3node<T, C>*>*)this->data;
	this->status |= 4;
}

template<class T, class C>
void magi::N3node<T, C>::setMid() {
	this->status &= 251;
	this->data = new std::vector<magi::N3node<T, C>*>(3);
}

template<class T, class C>
void magi::N3node<T, C>::setTarget(typename std::map<T, magi::N3node<T, C>*>::iterator * it) {
	this->setEoN();
	this->data = (typename std::map<T, magi::N3node<T, C>*>::iterator *)it;
}

template<class T, class C>
magi::N3node<T, C>* &magi::N3node<T, C>::getPar() {
	return this->parent;
}

template<class T, class C>
bool magi::N3node<T, C>::isMid() const{
	unsigned char res = 4;
	res &= this->status;
	return (res == 0);
}

template<class T, class C>
bool magi::N3node<T, C>::isTarget()const {
	unsigned char res = 4;
	res &= this->status;
	return (res == 4);
}

template<class T, class C>
void magi::N3node<T, C>::setPar(magi::N3node<T, C>* n3n) {
	this->parent = n3n;
}

template<class T, class C>
magi::N3node<T, C>::N3node() {
	this->status = 2;
}

template<class T, class C>
void magi::N3node<T, C>::setNext(magi::N3node<T, C>* n3n, const char &pos) {
	std::vector<magi::N3node<T, C>*> & ref = (*(std::vector<magi::N3node<T, C>*>*) this->data);
	ref[pos + 1] = n3n;
}

template<class T, class C>
void magi::N3node<T, C>::setNext(const std::vector<magi::N3node<T, C>*>& vec) {
	std::vector<magi::N3node<T, C>*> & ref = (*(std::vector<magi::N3node<T, C>*>*)this->data);
	ref = vec;
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
	return (typename std::map<T, magi::N3node<T, C>*>::iterator *)this->data;
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