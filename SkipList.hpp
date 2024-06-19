


#ifndef ___SKIP_LIST_HPP
#define ___SKIP_LIST_HPP

#include <cmath> // for log2
#include <string>
#include <vector>
#include "runtimeexcept.hpp"
#include <iostream>

/**
 * flipCoin -- NOTE: Only read if you are interested in how the
 * coin flipping works.
 * 
 * @brief Uses the bitwise representation of the key to simulate
 * the flipping of a deterministic coin.
 * 
 * This function looks at the bitwise representation to determine
 * how many layers it occupies in the skip list. It takes the bitwise
 * XOR of each byte in a 32-bit unsigned number and uses the index `previousFlips`
 * to determine the truth value. It's best to look at an example and
 * to start with values that fit into one byte.
 * 
 * Let's start with the function call `flipCoin(0, 0)`. This call is
 * asking whether or not we should insert the key `0` into layer `1`
 * (because `previousFlips` represents the number of previous flips).
 * 
 * This function will get the bitwise representation of 0: 
 * 
 *        00000000 00000000 00000000 00000000
 * 
 * All bytes are XOR'd together: 
 * 
 * 
 *  c = 0 ^ 0 ^ 0 ^ 0
 * 
 * Then the index `previousFlips` is used to obtain the bit in the `previousFlips % 8` position.
 * 
 * So the return value is `0 & 1 = 0`, since the value in the zeroth position 
 * (obtained by bitmasking the value (1 << 0) at the bottom of the function
 * is 0.
 * Thus, this value `0` should never get added beyond the bottom-most layer.
 * 
 * Before:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after 0 is inserted
 * 
 * S_1: -inf --------> inf
 * S_0: -inf --> 0 --> inf
 * 
 * Let's look at something more interesting, like the call `flipCoin(5, 0)`.
 * Remember the binary representation for 5 is 00000101.
 * 
 * c = 0 ^ 0 ^ 0 ^ 00000101 = 00000101
 * 
 * Now we get the bit at 0th position (from `previousFlips`).
 * 
 * 00000101 & 1 = 1. Thus, `5` DOES get inserted into the next layer,
 * layer 1.
 * 
 * So the skip list before this insertion might look like:
 * 
 * S_1: -inf ----> inf
 * S_0: -inf ----> inf
 * 
 * And after it would look like:
 * 
 * S_2: -inf --------> inf
 * S_1: -inf --> 5 --> inf
 * S_0: -inf --> 5 --> inf
 * 
 * Observe that a new layer had to get created, because there should always
 * be an empty layer at the top.
 * 
 * Since 5 got inserted into the next layer, we need to flip again to see if
 * it should get propagated once more.
 * 
 * `flipCoin(5, 1)` this will produce a result of `0`, since 00000101 & (1 << 1) = 0.
 * 
 * Thus, `5` will not be pushed up to the next layer.
 * 
 * @param key The integer key which will be added to the skip list
 * @param previousFlips The number of previous flips for this key
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(unsigned key, unsigned previousFlips)
{
	char c;
	unsigned first8Bits = (key & 0xFF000000) / 0x01000000 ; 
	unsigned next8Bits =  (key & 0x00FF0000) / 0x00010000;
	unsigned andThen =	 (key & 0x0000FF00) / 0x00000100;
	unsigned lastBits =   (key & 0x000000FF);
	c = first8Bits ^ next8Bits ^ andThen ^ lastBits;
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;
}

/**
 * @brief Works the same as the integer version above, except
 * it XORs chars in a string instead of the first four
 * bytes of an integer.
 * 
 * @param key key that will be inserted into the skip list
 * @param previousFlips number of previous flips for this key 
 * @return true simulates a "heads" from a coin flip
 * @return false simulates a "tails" from a coin flip
 */
inline bool flipCoin(std::string key, unsigned previousFlips)
{
	char c = key[0];
	for(unsigned j = 1; j < key.length(); j++)
	{
		c = c ^ key[j];
	}
	previousFlips = previousFlips % 8;
	return ( c & (1 << previousFlips) ) != 0;	
}

template<typename Key, typename Value>
class SkipList
{


private:
	// private variables go here.
	class Node{
    public:
        bool sentinel;
        unsigned height;
        
        Key key;
        Value val;
        Node* next;
        Node* prev;
        Node* up;
        Node* down;
        
        std::string name;

        Node(): sentinel(true), next(nullptr), prev(nullptr), up(nullptr), down(nullptr){}

        Node(const Key& k, const Value& v):
                sentinel(false), height(0), key(k), val(v), 
                next(nullptr), prev(nullptr), up(nullptr), down(nullptr){}

        
    };

    Node* top_l = new Node();
    Node* top_tail = new Node();
    Node* bottom_l = new Node();
    Node* bottom_tail = new Node();
    size_t sl_size; // num of keys
    unsigned sl_layers;


public:

	SkipList();

	// You DO NOT need to implement a copy constructor or an assignment operator.

	~SkipList();

	// How many distinct keys are in the skip list?
	size_t size() const noexcept;

	// Does the Skip List contain zero keys?
	bool isEmpty() const noexcept;

	// How many layers are in the skip list?
	// Note that an empty Skip List has two layers by default,
	// the "base" layer S_0 and the top layer S_1.
	//
	// [S_1: Top layer]    -inf ------> inf
	// [S_0: Bottom layer] -inf ------> inf
	//
	// This "empty" Skip List has two layers and a height of one.
	unsigned numLayers() const noexcept;

	// What is the height of this key, assuming the "base" layer S_0
	// contains keys with a height of 1?
	// For example, "0" has a height of 1 in the following skip list.
	//
	// [S_1]  -inf --------> inf
	// [S_0]  -inf --> 0 --> inf
	//
	// Throw an exception if this key is not in the Skip List.
	unsigned height(const Key & k) const;

	// If this key is in the SkipList and there is a next largest key
	// return the next largest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no subsequent key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *largest* key in the Skip List.
	Key nextKey(const Key & k) const;

	// If this key is in the SkipList and a next smallest key exists,
	// return the next smallest key.
	// This function should throw a RuntimeException if either the key doesn't exist
	// or there is no previous key. 
	// A consequence of this is that this function will
	// throw a RuntimeException if *k* is the *smallest* key in the Skip List.
	Key previousKey(const Key & k) const;

	// These return the value associated with the given key.
	// Throw a RuntimeException if the key does not exist.
	Value & find(const Key & k);
	const Value & find(Key k) const;

	// Return true if this key/value pair is successfully inserted, false otherwise.
	// See the project write-up for conditions under which the key should be "bubbled up"
	// to the next layer.
	// If the key already exists, do not insert one -- return false.
	bool insert(const Key & k, const Value & v);

	// Return a vector containing all inserted keys in increasing order.
	std::vector<Key> allKeysInOrder() const;

	// Is this the smallest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isSmallestKey(const Key & k) const;

	// Is this the largest key in the SkipList? Throw a RuntimeException
	// if the key *k* does not exist in the Skip List. 
	bool isLargestKey(const Key & k) const;

	// I am not requiring you to implement remove.
	const bool search(const Key & k, Node *& n = nullptr) const;

    const void bottomMost(Node *& n) const;

};

template<typename Key, typename Value>
const void SkipList<Key, Value>::bottomMost(SkipList::Node *&n) const {
    // should update the passed in node and make it the bottomMost node in the layer.
    // returning would be the bottom most node with that key/node
    Node* temp = n;

    while (temp->down != nullptr){
        temp = temp->down;
    }
    n = temp;
}


template<typename Key, typename Value>
const bool SkipList<Key, Value>::search(const Key& k, Node *& n) const {
    // if return is false, then n = the node before where the insert would of taken place
    // if return is true, then n = the position in which the node was found.

    Node *temp = top_l;
    Node *no_null = temp;

    while (temp != bottom_tail && temp != nullptr) {
        if (temp->sentinel == true) {
            if (temp->next->sentinel == true) {
                temp = temp->down;
            } else {
                temp = temp->next;
            }

        } else {
            if (temp->key == k) {
                n = temp;
                bottomMost(n);
                return true;
            }
            if (temp->key > k) {
                no_null = temp;
                temp = temp->prev->down;
            } else {
                no_null = temp;
                if (temp->next->sentinel == true) {
                    if (temp == bottom_tail) {
                        temp = temp->next;
                    } else { temp = temp->down; }
                } else {
                    temp = temp->next;
                }
            }
        }
    }
    // if no key was found, then return a position thats less than the key
    if (no_null->sentinel == true) {
        n = no_null;
    } else {
        if (no_null->key < k) {
            n = no_null;
        } else { n = no_null->prev; }
        return false;
    }
    return false;
}


template<typename Key, typename Value>
SkipList<Key, Value>::SkipList() {
    // look into if bottom_l should be used or how to avoid using it accidentally when looping
    top_l->down = bottom_l;
    top_l->next = top_tail;
    top_l->name = "top_l";

    top_tail->prev = top_l;
    top_tail->down = bottom_tail;
    top_tail->next = nullptr;
    top_tail->name = "top_tail";

    bottom_tail->prev = bottom_l;
    bottom_tail->up = top_tail;
    bottom_tail->next = nullptr;
    bottom_tail->name = "bottom_tail";

    bottom_l->next = bottom_tail;
    bottom_l->up = top_l;
    bottom_l->name = "bottom_l";

    sl_size = 0;
    sl_layers = 2;

}

template<typename Key, typename Value>
SkipList<Key, Value>::~SkipList() {
    // you need to cycle through every node that was created and delete it
    Node* column = top_l;
    Node* row;
    Node* del;

    // reengineered
    while (column != nullptr){
        if (column->next->sentinel == true){
            column = column->down;
        } else {
            row = column->next;
            while (!row->sentinel){
                    del = row;
                    row = row->next;
                    delete del;
            }
            column = column->down;
        }
    }
    Node* col = top_l;
    Node* tail = top_tail;
    Node* del2;

    while (col != nullptr){
        del = col;
        del2 = tail;
        col = col->down;
        tail = tail->down;

        delete del;
        delete del2;

    }
}

template<typename Key, typename Value>
size_t SkipList<Key, Value>::size() const noexcept {
	return sl_size;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isEmpty() const noexcept {
    if (sl_size == 0){return true;}
    return false;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::numLayers() const noexcept {
	return sl_layers;
}

template<typename Key, typename Value>
unsigned SkipList<Key, Value>::height(const Key & k) const {
    // search for the key, get key and return its height,
    // if false, then key didnt exist, raise exception
    Node* n;
    if (search(k,n)) {
        return n->height;
    } else {
        throw RuntimeException("No key.");
    }
    //failed, key doesnt exist
    // throw exception
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::nextKey(const Key & k) const {
    // search for the key, return k->next
    Node* n;
    if (search(k, n)){
        if (n->next != nullptr && n->next->sentinel != true){
            return n->next->key;
        } else {
            throw RuntimeException("failed to get next key.");
            // throw exception if this occurs
        }
    }
    // no key found, throw exception
}

template<typename Key, typename Value>
Key SkipList<Key, Value>::previousKey(const Key & k) const {
     // search for the key, return k->prev
    Node* n;
    if (search(k, n)){
        if (n->prev != nullptr && n->prev->sentinel != true){
            return n->prev->key;
        } else {
            throw RuntimeException("No previous key");
            // throw exception if this occurs
        }
    }
    throw RuntimeException("No previous key");
    // no key found, throw exception
}

template<typename Key, typename Value>
const Value & SkipList<Key, Value>::find(Key k) const {
    // search for the key, return k->val
    Node* n;
    if (search(k, n)){
        // Value v = n->val;
        // std::cout<<"found: key: "<<n->key<<" val: "<<v<<" height: "<<n->height<<std::endl;
        return n->val;
    }
    else {
        throw RuntimeException("find failed.");
//        throw std::runtime_error("did not find");
        // no key found, throw exception}
    }
}

template<typename Key, typename Value>
Value & SkipList<Key, Value>::find(const Key & k) {
    // search for the key, return k->val
    Node* n;
    if (search(k, n)){
        // Value v = n->val;
        // std::cout<<"found: key: "<<n->key<<" val: "<<v<<" height: "<<n->height<<std::endl;
        // return static_cast<unsigned int &>(v);
        return n->val;
    }
    else {
        throw RuntimeException("find failed.");
//        throw std::runtime_error("did not find");
        // no key found, throw exception}
    }
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::insert(const Key & k, const Value & v) {
	// TODO - your implementation goes here!
    Node* position;

    if (sl_size == 0){
        // if list is empty, position must start at bottom_l
        position = bottom_l;
    } else {
        if (!search(k,position)){
            // key didn't exist in list, position is now the node
            // that is prev to where the key should be placed

            // position = location
        } else {
            // key exists in list, we cannot insert
            // throw exception
            return false;}
    }
    Node* newNode = new Node(k,v);
    // placing new node where prev node was in 0 layer
    // position -> newNode -> position->next
    newNode->next = position->next;
    newNode->prev = position;
    position->next->prev = newNode;
    position->next = newNode;

    newNode->height = 1;

    // layer using coinflip
    unsigned int currLayer = 0;
    // while heads, increment upward from position
    Node* temp = newNode;
    unsigned int max = 3 * static_cast<int>(std::ceil(std::log2(sl_size + 1))) + 1;
    if (sl_size < 16){
        max = 13;
    }
    while(flipCoin(k,currLayer) && static_cast<unsigned int>(sl_layers) < max) {
        if (currLayer + 1 >= static_cast<unsigned int>(sl_layers) - 1) {
            Node* newTop = new Node();
            Node* newTail = new Node();
            newTop->next = newTail;
            newTail->prev = newTop;

            newTop->down = top_l;
            top_l->up = newTop;
            newTail->down = top_tail;
            top_tail->up = newTail;

            top_l = newTop;
            top_tail = newTail;

            sl_layers++;

        }
        // checks if the position we are replacing exists in that layer,
        // if not, then we look for the position prev to it, if all else fails
        // we obtain the bottom_l->up position
        temp = position->next;
        while (position->up == nullptr) {
            position = position->prev;
        }
        position = position->up;

        // finally pushing up the layer with desired insert
        Node* newUp = new Node(k,v);
        newUp->height = ++currLayer + 1;

        newUp->next = position->next;
        position->next->prev = newUp;
        newUp->prev = position;
        position->next = newUp;

        newUp->down = temp;
        temp->up = newUp;
        temp = newUp;

        newNode->height = newUp->height;

    }

    sl_size++;

    return true;
}

template<typename Key, typename Value>
std::vector<Key> SkipList<Key, Value>::allKeysInOrder() const {
    Node* temp = bottom_l->next;
    std::vector<Key> v;
    // remember you made a sentinel for the bottom_tail
    while (temp != bottom_tail){
        v.push_back(temp->key);
        temp = temp->next;
    }
    return v;
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isSmallestKey(const Key & k) const {
    // how can we check the whole list to see if the key exists with theta(1) time, at best it would be log(n)
    if (k == bottom_l->next->key){
        return true;
    } else {
        return false;
    }
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::isLargestKey(const Key & k) const {
    // how can we check the whole list to see if the key exists with theta(1) time
	if (k == bottom_tail->prev->key){
        return true;
    } else {
        return false;
    }
}



#endif

