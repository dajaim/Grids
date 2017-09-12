//author: Daniel Jan Imiela

#include<map>
#include<queue>
#include<vector>

#include"grid.h"
#include"n3node.h"

#ifndef N3TREE
#define N3TREE

namespace magi {

	// v 1.3 ( seems to work)

	// N3tree is a dynamic grid, which is expandable to n ( n > 0) dimensions.
	// It is probably the best option if you work with few widely separated coordinates.
	// It is probably the worst option if you work with many closely aligned coordinates.

	// T is the type of the object, which you insert
	// C is the type of the coordinates, default is int, floating-point numbers are not supported
	template<class T, class C = int>
	class N3tree;
}


template<class T, class C>
class magi::N3tree : public magi::Grid<T, C> {
	friend class N3test;
public:
	N3tree();
	~N3tree();
	void clr();
	bool ava(const T & obj);
	bool del(const T & obj);
	bool ipo(const std::vector<C>& position);
	int add(const std::vector<C>& position, const T & obj);
	unsigned getDim()const ;
	unsigned setDim(const unsigned & dimension);
	std::vector<C> whr(const T & obj);
private:
	// doc
	int scalar, dimension;
	// ...
	magi::N3node<T, C> * entry;
	// 3^x, x > (-1)
	std::map<C, std::size_t >approx;
	//magi::N3node<T,C> * currentPosition; <- optimization for CG
	std::map<T, magi::N3node<T, C> * >reference;

	void setup();
	void approxSetup();
	void shrinkVec(std::vector<std::vector<char>> & vec);
	void push0End(std::vector<std::vector<char>> & vec);
	void shrink();
	magi::N3node<T, C>* link(const std::vector<std::vector<char> > & mat);
	magi::N3node<T, C>* merge(const std::vector<std::vector<char> > & mat);
	bool legalVec(const std::vector<C> & vec);
	bool zeroVecCheck(const std::vector<char> & vec);
	bool posOcc(const std::vector<std::vector<char> > & mat);
	std::vector<char> appLoop(const C & app, const unsigned & c);
	std::vector<char> appC(const C & c);
	std::vector<char> neg(const std::vector<char> & vec);
	std::vector<std::vector<char > >appPos(const std::vector<C> & vector);
	std::vector<std::vector<char> >translate(const std::vector<std::vector<char> >& vec);
	C convertCharVec(const std::vector<char>& cvec);
	C abs(const C & c) { return c < 0 ? ((-1)* c) : c; }
	C byx3(unsigned &exp);
	magi::N3node<T, C>* firstEntry(const std::vector<std::vector<char> >& vec);
	magi::N3node<T, C>* getNode(const std::vector<std::vector<char > > & vec);
	magi::N3node<T, C>* getN3Node(const std::vector<char> & vec);
	magi::N3node<T, C>* simpleEntry(const std::vector<std::vector<char> > & vec);
	magi::N3node<T, C>* getStage(const std::vector<char> & vec, const std::size_t & index = 0);
};

template<class T,class C>
C magi::N3tree<T,C>::byx3(unsigned &exp) { 
	C res = 1;
	for (unsigned i = 0; i < exp; ++i) 
		res *= 3; 
	return res; 
}

template<class T,class C>
bool magi::N3tree<T,C>::legalVec(const std::vector<C> & vec) { 
	return (vec.size() == this->dimension); 
}

template<class T, class C>
unsigned magi::N3tree<T,C>::getDim()const { 
	return this->dimension; 
}

template<class T, class C>
magi::N3tree<T, C>::~N3tree() {
	this->clr(); delete this->entry;
}

template<class T, class C>
magi::N3tree<T, C>::N3tree() {
	this->setup();
}

template<class T, class C>
void magi::N3tree<T, C>::push0End(std::vector<std::vector<char>> & vec) {
	std::vector<char>cVec(vec[0].size());
	while (vec.size() < (this->scalar + 1))
		vec.push_back(cVec);
}

template<class T, class C>
void magi::N3tree<T, C>::shrink() {
	magi::N3node<T, C>* node, *tmp, *run;
	std::vector<char>nullStage(this->dimension);
	bool oneWay = true, stageBreak = false;

	node = this->entry;
	while (true) {
		for (const char & c : nullStage) {
			if ((node->getNext(-1) == NULL) && (node->getNext(1) == NULL) && (node->getNext(0) != NULL))// shrinkable
				node = node->getNext(0);
			else
				return;
		}
		run = this->entry->getNext(0);
		while (run != node) {
			delete run->getPar();
			run = run->getNext(0);
		}
		delete run->getPar();
		run->setPar(NULL);
		this->entry = run;
		--this->scalar;
		if (this->scalar - 1 < 0)
			return;
	}
	return;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::link(const std::vector<std::vector<char>>& mat) {
	magi::N3node<T, C>* run, *node, *tmp;
	std::vector<char> scope;
	std::size_t index = 0;
	bool jump = false;
	auto revIt = mat.rbegin();

	this->scalar = (this->scalar < mat.size() - 1) ? mat.size() - 1 : this->scalar;
	run = this->entry;
	// detect
	while (revIt != mat.rend()) {
		scope = (*revIt);
		index = 0;
		for (; index < this->dimension; ++index) {
			if (run->getNext(scope[index]) == NULL) {
				jump = true;
				break;
			}
			run = run->getNext(scope[index]);
		}
		if (jump)
			break;
		++revIt;
	}

	// complete stage
	if (index < this->dimension) {
		node = this->getStage(scope, index);
		tmp = node->getPar();
		run->setNext(node, node->getDirection());
		node->setPar(run);
		run = tmp;
	}
	++revIt;

	while (revIt != mat.rend()) {
		scope = (*revIt);
		node = this->getStage(scope);
		tmp = node->getPar();
		run->setNext(node, node->getDirection());
		node->setPar(run);
		run = tmp;
		++revIt;
	}
	return run;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::merge(const std::vector<std::vector<char>>& mat) {
	magi::N3node<T, C> * node, *last, *tmp;
	std::vector<char> nullStage(this->dimension);

	node = this->getStage(nullStage);
	last = node->getPar();
	last->setNext(this->entry->getNext());
	for (magi::N3node<T, C>* & elem : last->getNext())
		if (elem != NULL)
			elem->setPar(last);

	this->entry->setNext({ NULL, NULL, NULL });
	this->entry->setNext(node, node->getDirection());
	node->setPar(this->entry);
	++this->scalar;
	last = node;
	while (mat.size() != (this->scalar + 1)) {
		node = this->getStage(nullStage);
		tmp = node->getPar();
		this->entry->getNext(0)->setPar(tmp);
		tmp->setNext(this->entry->getNext(0), 0);
		this->entry->setNext(node, 0);
		node->setPar(this->entry);
		++this->scalar;
	}
	return this->link(mat);
}

template<class T, class C>
std::vector<char> magi::N3tree<T, C>::neg(const std::vector<char> & vec) {
	std::vector<char> res;
	for (const char & c : vec)
		res.push_back((-1)* c);
	return res;
}

template<class T, class C>
std::vector<std::vector<char>> magi::N3tree<T, C>::translate(const std::vector<std::vector<char>>& vec) {
	std::vector<std::vector<char>> res;
	std::vector<char>tmp;
	for (unsigned inner = 0; inner < vec[0].size(); ++inner) {
		for (unsigned outer = 0; outer < vec.size(); ++outer)
			tmp.push_back(vec[outer][inner]);
		res.push_back(tmp);
		tmp.clear();
	}
	return res;
}

template<class T, class C>
bool magi::N3tree<T, C>::zeroVecCheck(const std::vector<char> & vec) {
	for (const char & c : vec)
		if (c != char(0))
			return false;
	return true;
}

template<class T, class C>
bool magi::N3tree<T, C>::posOcc(const std::vector<std::vector<char>>& mat) {
	std::vector<std::vector<char> > cVec = this->translate(mat);
	typename std::vector<std::vector<char> >::reverse_iterator revIt;
	std::vector<char> scope;
	bool jump = false;
	magi::N3node<T, C> * run;

	this->push0End(cVec);
	if (cVec.size() > (this->scalar + 1))
		return false;

	revIt = cVec.rbegin();
	run = this->entry;

	while ((revIt != cVec.rend())) {
		scope = *(revIt);
		for (unsigned i = 0; i < scope.size(); ++i)
			if ((run != NULL))
				run = run->getNext(scope[i]);
		++revIt;
	}
	if (run == NULL)
		return false;
	if (run->isMid())
		return false;
	return true;
}

//returns last xn
template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getStage(const std::vector<char> & vec, const std::size_t & index) {
	magi::N3node<T, C>* node, *last, *first;

	first = NULL;
	last = NULL;

	for (std::size_t i = index; i < this->dimension; ++i) {
		node = new magi::N3node<T, C>;
		node->setMid();
		node->setDirection(vec[i]);
		node->setPar(last);
		if (first == NULL)
			first = node;
		else
			last->setNext(node, node->getDirection());
		last = node;
	}
	first->setPar(last);
	return first;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getN3Node(const std::vector<char> & vec) {
	return 0;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::firstEntry(const std::vector<std::vector<char>>& vec) {
	magi::N3node<T, C>* last, *stage, *tmp;
	auto revIt = vec.rbegin();

	last = this->entry;
	this->scalar = (vec.size() - 1);
	while (revIt != vec.rend()) {
		stage = this->getStage(*revIt);
		tmp = stage->getPar();
		stage->setPar(last);
		last->setNext(stage, stage->getDirection());
		last = tmp;
		++revIt;
	}
	return last;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getNode(const std::vector<std::vector<char > > & vec) {
	return this->simpleEntry(vec);
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::simpleEntry(const std::vector<std::vector<char> > & vec) {
	std::vector<std::vector<char> > cVec;
	cVec = this->translate(vec);
	this->push0End(cVec);
	return (cVec.size() > (this->scalar + 1)) ? this->merge(cVec) : this->link(cVec);
}

template<class T, class C>
std::vector<std::vector<char > > magi::N3tree<T, C>::appPos(const std::vector<C> & vector) {
	std::vector<std::vector<char> > res;
	bool positive;

	for (const C & elem : vector) {
		positive = (elem > 0);
		if (positive)
			res.push_back(this->appC(elem));
		else
			res.push_back(this->neg(this->appC(elem)));
	}
	this->shrinkVec(res);
	return res;
}

template<class T, class C>
bool magi::N3tree<T, C>::ipo(const std::vector<C>& position) {
	return (this->posOcc(this->appPos(position)));
}

template<class T, class C>
void magi::N3tree<T, C>::shrinkVec(std::vector<std::vector<char>> & vec) {
	unsigned max = 0;
	bool allEndZero = true;
	bool zeroVec = true;

	// {0,0,0,0, ...}, not shrinkable
	for (std::vector<char> & elem : vec)
		if (!(this->zeroVecCheck(elem)))
			zeroVec = false;

	if (zeroVec)
		return;

	for (std::vector<char> & elem : vec)
		if (elem.size() > max)
			max = elem.size();

	for (std::vector<char> & elem : vec)
		while (elem.size() < max)
			elem.push_back(0);

	for (std::vector<char> & elem : vec)
		if (elem[elem.size() - 1] != char(0))
			allEndZero = false;

	if (allEndZero)
		for (std::vector<char> & elem : vec)
			elem.pop_back();
}

template<class T, class C>
C magi::N3tree<T, C>::convertCharVec(const std::vector<char>& cvec) {
	C res = 0, run = 1;
	for (const char &elem : cvec) {
		res += (run * elem);
		run *= 3;
	}
	return res;
}

template<class T, class C>
std::vector<char> magi::N3tree<T, C>::appLoop(const C & cNum, const unsigned & c) {
	std::vector<char> res(c);
	C target, test;

	target = cNum;
	res[res.size() - 1] = 1;

	// do not know how to include to loop

	if (cNum == 0)
		return{ 0 };

	for (unsigned i = c - 1; i < c; --i) {
		test = this->byx3(i);

		if (target == 0)
			return res;

		if (target > 0) {
			if (this->abs(target - test) >= this->abs(target)) {
				res[i] = char(0);
				continue;
			}
			res[i] = char(1);
			target -= test;
			continue;
		}
		else {
			if (this->abs(target + test) >= this->abs(target)) {
				res[i] = char(0);
				continue;
			}
			res[i] = char(-1);
			target += test;
			continue;
		}
	}
	return res;
}

template<class T, class C>
std::vector<char> magi::N3tree<T, C>::appC(const C &c) {
	C lookFor = (c < 0) ? (c * (-1)) : c;
	auto upper = this->approx.begin();

	upper = this->approx.upper_bound(lookFor);
	return this->appLoop(lookFor, upper->second);

}

template<class T, class C>
bool magi::N3tree<T, C>::ava(const T &obj) {
	typename std::map<T, magi::N3node<T, C>* >::iterator it;
	if (this->reference.size() == 0)
		return false;
	it = this->reference.find(obj);
	return it == this->reference.end() ? false : true;
}

template<class T, class C>
int magi::N3tree<T, C>::add(const std::vector<C>& position, const T & obj) {
	std::vector<std::vector<char> > appedPos;
	magi::N3node<T, C>* node;
	typename std::map<T, magi::N3node<T, C>*>::iterator* pit;

	if (this->ava(obj))
		return 2;

	if (this->dimension == 0) {
		if (position.size() > 0)
			this->dimension = position.size();
		else
			return 1;
	}
	else
		if (!(this->legalVec(position)))
			return 1;
	appedPos = this->appPos(position);
	// position occupied
	if (this->posOcc(appedPos))
		return 3;
	if (this->reference.size() == 0)
		node = this->firstEntry(this->translate(appedPos));
	else
		node = this->getNode(appedPos);
	node->setEnd();
	this->reference[obj] = node;
	pit = new typename std::map<T, magi::N3node<T, C>*>::iterator;
	(*pit) = this->reference.find(obj);
	node->setTarget(pit);
	return 0;
}

template<class T, class C>
bool magi::N3tree<T, C>::del(const T & obj) {
	typename std::map<T, magi::N3node<T, C>*>::iterator it;
	magi::N3node<T, C>* node, *parent;
	bool hyper = true;

	if (!this->ava(obj))
		return false;
	it = this->reference.find(obj);
	node = it->second;
	parent = node->getPar();
	while (hyper && (parent != NULL)) {
		parent->setNext(NULL, node->getDirection());
		delete node;
		for (magi::N3node<T, C>* & elem : parent->getNext())
			if (elem != NULL) {
				hyper = false;
				break;
			}
		node = parent;
		parent = parent->getPar();
	}
	// shrink is an optimization, does not work correctly yet
	this->shrink();
	this->reference.erase(it);
	return true;
}

template<class T, class C>
unsigned magi::N3tree<T, C>::setDim(const unsigned & dimension) {
	if (dimension != this->dimension) {
		this->clr();
		this->dimension = dimension;
		return this->dimension;
	}
	return this->dimension;
}

template<class T, class C>
std::vector<C> magi::N3tree<T, C>::whr(const T & obj) {
	std::vector<C> res;
	magi::N3node<T, C>*run, *test;
	C tmp;
	unsigned exp = 0;
	typename std::map<T, magi::N3node<T, C>*>::iterator it;

	if (!this->ava(obj))
		return res;
	res.resize(this->dimension);
	it = this->reference.find(obj);
	run = it->second;
	while (run != this->entry) {
		for (unsigned xn = this->dimension - 1; xn < this->dimension; --xn) {
			tmp = ((run->getDirection())*(this->byx3(exp)));
			res[xn] += tmp;
			if (run == this->entry)
				break;
			run = run->getPar();
		}
		++exp;
	}
	return res;
}

template<class T, class C>
void magi::N3tree<T, C>::approxSetup() {
	C exp, run, flowCheck;
	exp = 0;
	run = 1;
	flowCheck = 3;
	while ((exp != 42) && (flowCheck == (run * 3)) && (flowCheck > run)) {
		this->approx[run] = exp + 1;
		++exp;
		run = flowCheck;
		flowCheck *= 3;
	}
}

template<class T, class C>
void magi::N3tree<T, C>::setup() {
	this->approxSetup();
	this->dimension = 0;
	this->entry = new magi::N3node<T, C>;
	this->entry->setMid();
	this->scalar = 0;
}

template<class T, class C>
void magi::N3tree<T, C>::clr() {
	std::queue<magi::N3node<T, C> * >queue;
	magi::N3node<T, C>* node;
	queue.push(this->entry);
	while (!queue.empty()) {
		node = queue.front();
		if (node->isMid()) {
			for (magi::N3node<T, C>*& elem : node->getNext())
				if (elem != NULL)
					queue.push(elem);
		}
		delete node;
		queue.pop();
	}
	this->reference.clear();
	this->dimension = 0;
	this->scalar = 0;
	this->entry = new magi::N3node<T, C>;
	this->entry->setMid();
}
#endif