# Grids
Grids with different concepts for storing objects at coordinates.

N3tree is a ternary tree, which is comparable to an n-dimensional array.
# Default constructor:
The N3tree default constructor creates an array with dimension == 0.
# Change dimension:
You can change the dimension with the unsigned setDim(const unsigned & dimension)-method.
You can also change the dimension by using the int add(const std::vector<C>& position, const T & obj)-method.
If you have set the dimension once, you can reset it by using the void clr()-method : all inserted objects are deleted and the dimension will become 0.
# Insert an object:
You can insert an object at a position by calling the int add(const std::vector<C>& position, const T & obj)-method.
by calling this directly after calling the default constructor you set the dimension automatically. Provided the size of position is greater than 0.
If position's size is not equal to N3tree's dimension you will get the return value int(1).
If the object, you want to insert, is already available in the N3tree, you will get the return value int(2).
If the given position, you want to insert the object at, is already occupied, you will get the return value int(3).
In all previous cases neither the position has been occupied nor the object has been inserted.
If the position is valid and not occupied and the object is not available yet, you will get the return value int(0).
The position is occupied and the object is available now.
# Check, if a position is occupied:
You can call the bool ipo(const std::vector<C>& position)-method, and check if the given position is occupied.
you will get the return value bool(true) if the given position is occupied.
you will get the return value bool(true) if the given position is not occupied.
# Getting an object's position:
you can call the std::vector<C> magi::N3tree<T, C>::whr(const T & obj)-method if you want to know an object's position.
you will get the return value std::vector<C>, which size is equal to N3tree's dimension, if the requested object is available and represents the position of the object.
you will get the return value std::vector<C>, which size is equal to 0, if the requested object is not available.
# Deleting an object:
you can delete an object by using the bool del(const T & obj)-method. This method deletes an object in the N3tree.
you will get the return value bool(false), if the object is not available.
you will get the return value bool(true), if the object was available, but is not existing anymore.
