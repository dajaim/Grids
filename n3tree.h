//author: Daniel Jan Imiela

#include<map>
#include<queue>
#include<vector>

#include"grid.h"
#include"n3node.h"

#ifndef N3TREE
#define N3TREE

namespace magi {

	// v 1.0 ( seems to work)

	// N3tree is a dynamic grid, which is expandable to n ( n > 0) dimensions.
	// It is probably the best option if you work with few widely separated coordinates.
	// It is probably the worst option if you work with many closely aligned coordinates.

	// T is the type of the object, which you insert
	// C is the type of the coordinates, default is int
	template<class T, class C = int>
	class N3tree;
}


template<class T, class C>
class magi::N3tree : public magi::Grid<T, C> {
public:
	N3tree() { this->setup(); }
	~N3tree() { this->clear(); }
	// checks if the given object is available
	// returns true if so, else false
	bool available(const T & obj);
	// inserts a given object at a given position,
	// returns 0 if insertion is successfull
	// returns 1 if the position is not valid( e.g. dimension is 2
	// and you try to insert at position { 0, 0, 0})
	// does not insert the object
	// returns 2 if the object is already available,
	// does not insert the object
	// returns 3 if there already is an object at the given position,
	// does not insert the object
	// returns 4 at unknown error,
	// does not insert the object
	int add(const std::vector<C>& position, const T & obj);
	// deletes the given object,
	// returns true if successfull, else false
	bool del(const T & obj);
	// deletes the tree, sets settings to default
	void clear();
	// returns n3tree's dimension
	// returns 0 if dimension is not set yet
	// else returns dimension
	unsigned getDimension() { return this->dimension; }
	// sets the n3tree's dimension
	// returns 0 if the given dimension is not valid
	// else returns the given dimension as confirmation
	// if it is set once you have to call this->clear() to reset this->dimension to 0
	unsigned setDimension(const unsigned & dimension);
	//returns obj's position, if obj is not available returns a vector with size == 0
	std::vector<C> where(const T & obj);
	//advanced methods
	std::vector<std::vector<char > >appPos(const std::vector<C> & vector);
	// checks if given position is occupied 
	// returns true if so else false
	bool isPosOcc(const std::vector<C>& vec);
private:
	// doc
	unsigned scalar;
	// ...
	unsigned dimension;
	// ...
	magi::N3node<T, C> * entry, *nullNode;
	// zero in type C
	C zero;
	// approimation of numbers > (-1)
	std::map<C, unsigned >approx;
	//magi::N3node<T,C> * currentPosition; <- optimization
	std::map<T, magi::N3node<T, C> * >reference;
	void setup();
	void base3ApproxSetup();
	void shrinkVec(std::vector<std::vector<char>> & vec);
	void push0End(std::vector<std::vector<char>> & vec);
	void shrink();
	magi::N3node<T, C>* link(const std::vector<std::vector<char> > & mat);
	magi::N3node<T, C>* merge(const std::vector<std::vector<char> > & mat);
	bool legalVec(const std::vector<C> & vec) { return (vec.size() == this->dimension); }
	bool zeroVecCheck(const std::vector<char> & vec);
	bool posOcc(const std::vector<std::vector<char> > & mat);
	bool delAll();
	std::vector<char> appLoop(const C & app, const unsigned & c);
	std::vector<char> appC(const C & c);
	std::vector<char> neg(const std::vector<char> & vec);
	std::vector<std::vector<char> >translate(const std::vector<std::vector<char> >& vec);
	C convertCharVec(const std::vector<char>& cvec);
	C abs(const C & c) { return c < 0 ? ((-1)* c) : c; }
	C byx3(unsigned exp) { C res = 1; for (unsigned i = 0; i < exp; ++i) res *= 3; return res; }
	magi::N3node<T, C>* getNode(const std::vector<std::vector<char > > & vec);
	magi::N3node<T, C>* getN3Node(const std::vector<char> & vec);
	magi::N3node<T, C>* firstEntry(const std::vector<std::vector<char> > & mat);
	magi::N3node<T, C>* simpleEntry(const std::vector<std::vector<char> > & vec);
	magi::N3node<T, C>* getStage(const std::vector<char> & vec);
};

template<class T, class C>
void magi::N3tree<T, C>::push0End(std::vector<std::vector<char>> & vec) {
	std::vector<char>cVec(vec[0].size());
	while (vec.size() < this->scalar)
		vec.push_back(cVec);
}

template<class T, class C>
void magi::N3tree<T, C>::shrink() {
	magi::N3node<T, C>* node, *tmp;
	bool oneWay = true;
	node = this->entry;

	while (oneWay && (this->scalar > 1)) {// this->scalar : {0, 0, 0, ...}
		for (unsigned xnth = 0; xnth < this->dimension; ++xnth) {
			if ((node == NULL) ||(node->getNext().size() > 1)) {// <- compiler optimization must be active
				oneWay = false;
				break;
			}
			node = node->relations[2];
		}
		if (!(oneWay))
			return;
		tmp = this->entry->relations[2];
		while (tmp != node) {
			tmp = tmp->relations[2];
			delete tmp->getPar();
		}
		this->entry->relations = node->relations;
		this->entry->setPar(NULL);
		node->setPar(this->entry);
		--this->scalar;
	}
	node = NULL;
	return;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::link(const std::vector<std::vector<char>>& mat) {
	auto revIt = mat.rbegin();
	std::vector<char> vec;
	magi::N3node<T, C>* run, *last, *down, *upper;
	bool secondPhase = false;
	unsigned i = 0;

	last = this->entry;

	// looking for link
	while ((revIt != mat.rend()) && (!secondPhase)) {
		i = 0;
		vec = (*revIt);
		for (; i < vec.size(); ++i) {
			run = last->relations[vec[i] + 2];
			if (run == NULL) {
				secondPhase = true;
				break;
			}
			last = run;
		}
		++revIt;
	}
	// secondPhase.1 complete a stage
	while (i < this->dimension) {
		run = new magi::N3node<T, C>;
		run->setPar(last);
		run->setDirection(vec[i]);
		last->setNext(run, vec[i]);
		last = run;
		++i;
	}

	if (revIt == mat.rend()) {
		while (last->relations.size() != 1)
			last->relations.pop_back();
		return last;
	}
	//secondPhase.2, like firstEntry fill stage after stage
	// maybe todo
	while (revIt != mat.rend()) {
		down = this->getStage(*revIt);
		upper = down;
		while (upper->relations[0] != NULL)
			upper = upper->relations[0];
		upper->setPar(last);
		last->setNext(upper, upper->getDirection());
		last = down;
		++revIt;
	}
	while (last->relations.size() != 1)
		last->relations.pop_back();
	return last;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::merge(const std::vector<std::vector<char>>& mat) {
	std::vector<char> nullStage(this->dimension);
	magi::N3node<T, C>* lower, *upper, *next;

	lower = this->getStage(nullStage);
	upper = lower;
	while (upper->getPar() != NULL)
		upper = upper->getPar();
	for (unsigned i = 1; i < 4; ++i) {
		next = this->entry->relations[i];
		if (next != NULL) {
			next->setPar(lower);
			lower->setNext(next, next->getDirection());
		}
	}
	upper->setPar(this->entry);
	for (magi::N3node<T, C>* & node : this->entry->relations)
		node = NULL;
	this->entry->relations[2] = upper;
	++this->scalar;

	while (this->scalar != mat.size()) {
		lower = this->getStage(nullStage);
		upper = lower;
		while (upper->getPar() != NULL)
			upper = upper->getPar();
		next = this->entry->relations[2];
		next->setPar(lower);
		lower->setNext(next, next->getDirection());
		upper->setPar(this->entry);
		this->entry->setNext(upper, 0);
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
	magi::N3node<T, C> * run;

	this->push0End(cVec);
	if (cVec.size() > this->scalar)
		return false;

	revIt = cVec.rbegin();
	run = this->entry;

	while ((revIt != cVec.rend()) && (run != NULL)) {
		scope = *(revIt);
		for (unsigned i = 0; (i < (*revIt).size()) && (run != NULL); ++i)
			run = run->relations[scope[i] + 2];
		++revIt;
	}
	if (run == NULL)
		return false;
	if (run->relations.size() == 1)
		return true;
	return false;
}

//returns last xn
template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getStage(const std::vector<char> & vec) {
	magi::N3node<T, C>* node, *last;
	node = NULL;
	last = NULL;
	for (const char & c : vec) {
		node = new magi::N3node<T, C>;
		node->setDirection(c);
		if (last != NULL)
			last->setNext(node, c);
		node->setPar(last);
		node->setDirection(c);
		last = node;
	}
	return node;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::firstEntry(const std::vector<std::vector<char> > & mat) {
	magi::N3node<T, C>* last, *down, *upper;
	std::vector<char> stage;
	auto vec = this->translate(mat);
	auto revIt = vec.rbegin();

	this->scalar = vec.size();
	down = NULL;
	last = NULL;

	while (revIt != vec.rend()) {
		down = this->getStage(*revIt);
		upper = down;
		while (upper->getPar() != NULL)
			upper = upper->getPar();
		if (revIt == vec.rbegin()) {
			this->entry = new magi::N3node<T, C>;
			this->entry->setNext(upper, upper->getDirection());
			upper->setPar(this->entry);
		}
		else {
			last->setNext(upper, upper->getDirection());
			upper->setPar(last);
		}
		last = down;
		++revIt;
	}
	while (down->relations.size() != 1)
		down->relations.pop_back();
	return down;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getN3Node(const std::vector<char> & vec) {
	return 0;
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::getNode(const std::vector<std::vector<char > > & vec) {
	if (this->entry == NULL)
		return this->firstEntry(vec);
	return this->simpleEntry(vec);
}

template<class T, class C>
magi::N3node<T, C>* magi::N3tree<T, C>::simpleEntry(const std::vector<std::vector<char> > & vec) {
	std::vector<std::vector<char> > cVec;
	if (this->posOcc(vec))
		return NULL;
	cVec = this->translate(vec);
	this->push0End(cVec);
	// link (cVec.size()<= this->scalar) merge ( cVec.size()> this->scalar)
	return (cVec.size() <= this->scalar) ? this->link(cVec) : this->merge(cVec);
}

template<class T, class C>
std::vector<std::vector<char > > magi::N3tree<T, C>::appPos(const std::vector<C> & vector) {
	std::vector<std::vector<char> > res;
	bool positive;

	for (const C & elem : vector) {
		positive = (elem > this->zero);
		if (positive)
			res.push_back(this->appC(elem));
		else
			res.push_back(this->neg(this->appC(elem)));
	}
	this->shrinkVec(res);
	return res;
}

template<class T, class C>
bool magi::N3tree<T, C>::isPosOcc(const std::vector<C>& vec) {
	return (this->posOcc(this->appPos(vec)));
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

	if (cNum == this->zero)
		return{ 0 };

	for (unsigned i = c - 1; i < c; --i) {
		test = this->byx3(i);

		if (target == this->zero)
			return res;

		if (target > this->zero) {
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
	C lookFor = c < this->zero ? (c * (-1)) : c;
	typename std::map<C, unsigned >::iterator upper;

	upper = this->approx.upper_bound(lookFor);
	return this->appLoop(lookFor, upper->second);

}

template<class T, class C>
bool magi::N3tree<T, C>::available(const T &obj) {
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

	//check, obj available
	if (this->available(obj))
		return 2;
	//if this->dimension == 0
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
	node = this->getNode(appedPos);
	// position occupied
	if (node == NULL)
		return 3;
	this->reference[obj] = node;
	pit = new std::map<T, magi::N3node<T, C>*>::iterator;
	(*pit) = this->reference.find(obj);
	node->itptr = pit;
	return 0;
}

template<class T, class C>
bool magi::N3tree<T, C>::delAll() {
	magi::N3node<T, C>* node = this->reference.begin()->second, *par;

	par = node->getPar();
	delete node->itptr;
	while (par != NULL) {
		delete node;
		node = par;
		par = par->getPar();
	}
	delete node;
	this->entry = NULL;
	this->scalar = 0;
	this->reference.clear();
	this->dimension = 0;
	return true;
}

template<class T, class C>
bool magi::N3tree<T, C>::del(const T & obj) {
	magi::N3node<T, C>* node, *par;
	std::vector<magi::N3node<T, C>*>vec;
	typename std::map<T, magi::N3node<T, C>*>::iterator it;

	if (!(this->available(obj)))
		return false;

	it = this->reference.find(obj);
	if (it != this->reference.end() && this->reference.size() == 1)
		return this->delAll();
	node = it->second;
	par = node->getPar();
	vec = par->getNext();
	delete node->itptr;
	this->reference.erase(it);
	while ((vec.size() == 1) && (par->getPar() != NULL)) {
		par->relations[node->getDirection() + 2] = NULL;
		delete node;
		node = par;
		par = par->getPar();
		vec = par->getNext();
	}
	par->relations[node->getDirection() + 2] = NULL;
	delete node;
	this->shrink();
	return true;
}

template<class T, class C>
unsigned magi::N3tree<T, C>::setDimension(const unsigned & dimension) {
	if (dimension != this->dimension) {
		this->clear();
		this->dimension = dimension;
		return this->dimension;
	}
	return this->dimension;
}

template<class T, class C>
std::vector<C> magi::N3tree<T, C>::where(const T & obj) {
	std::vector<C> res;
	magi::N3node<T, C>*run, * test;
	C tmp;
	unsigned exp = 0;
	typename std::map<T, magi::N3node<T, C>*>::iterator it;

	if (!this->available(obj))
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
			run = run->relations[0];
		}
		++exp;
	}
	return res;
}

template<class T, class C>
void magi::N3tree<T, C>::base3ApproxSetup() {
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
	this->base3ApproxSetup();
	this->dimension = 0;
	this->entry = NULL;
	this->zero = 0;
}

template<class T, class C>
void magi::N3tree<T, C>::clear() {
	std::queue<magi::N3node<T, C> * >queue;
	magi::N3node<T, C>* node;
	if (this->entry == NULL)return;
	queue.push(this->entry);
	while (!queue.empty()) {
		node = queue.front();
		if (node->itptr != NULL)
			delete node->itptr;
		for (magi::N3node<T, C>*& elem : node->getNext())
			queue.push(elem);
		queue.pop();
	}
	this->reference.clear();
	this->entry = 0;
	this->dimension = 0;
}
#endif