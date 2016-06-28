// The HashTable class is an open hash table with a specified number of buckets and a
// specified capacity. When adding an association, one can designate whether or not it
// is permanent. If it is permanent, it cannot be overwritten. Otherwise, once the hash
// table reaches capacity, it will automatically replace the oldest assocation. I keep
// track of the recent assocations by using an embedded doubly-linked list within the
// hash table nodes.

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
using namespace std;

/////////////////////////////////////
//	HashTable Declaration
/////////////////////////////////////

template <typename KeyType, typename ValueType>
class HashTable
{
public:
	HashTable(unsigned int numBuckets, unsigned int capacity);
	~HashTable();
	bool isFull() const;
	bool set(const KeyType& key, const ValueType& value, bool permanent = false);
	bool get(const KeyType& key, ValueType& value) const;
	bool touch(const KeyType& key);
	bool discard(KeyType& key, ValueType& value);

private:
	struct Node
	{
		KeyType key;
		ValueType value;

		Node* next_hash;
		Node* next_rlist, *prev_rlist; // Embedded linked list variables
	};

	Node** m_hash;
	unsigned int m_numBuckets;
	unsigned int m_capacity;
	int m_size_hash;
	  // Embedded linked list variables
	Node* m_head_rlist, *m_tail_rlist;
	int m_size_rlist;

private:
	// We prevent a HashTable from being copied or assigned by declaring the
	// copy constructor and assignment operator privatge and not implementing them.
	HashTable(const HashTable&);
	HashTable& operator=(const HashTable&);
	unsigned int getBucketFromHash(unsigned int hash) const;
	unsigned int getBucketFromKey(const KeyType& key) const;
	bool isPermanent(Node* src) const;
	void makeMostRecentInRecentList(Node* src);
	void addToEndOfRecentList(Node* src);
};


/////////////////////////////////////
//	Function Implementations
/////////////////////////////////////

/////////////////////////////////////
//	Helper Functions
/////////////////////////////////////

template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::getBucketFromHash(unsigned int hash) const
{
	return hash % m_numBuckets;
}

template <typename KeyType, typename ValueType>
unsigned int HashTable<KeyType, ValueType>::getBucketFromKey(const KeyType& key) const
{
	unsigned int computeHash(KeyType);	// prototype. Why not const KeyType&?
	unsigned int hash = computeHash(key);
	unsigned int bucket = getBucketFromHash(hash);
	return bucket;
}

template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::makeMostRecentInRecentList(Node* src)
{
	if (isPermanent(src))
		return;
	  // Remove Node from current position
	m_size_rlist--;
	if (src->prev_rlist != nullptr)	// there is a preceding Node
		src->prev_rlist->next_rlist = src->next_rlist;
	else
		m_head_rlist = src->next_rlist;
	if (src->next_rlist != nullptr)	// there is a following Node
		src->next_rlist->prev_rlist = src->prev_rlist;
	else
		m_tail_rlist = src->prev_rlist;
	  // Add Node to end of list
	addToEndOfRecentList(src);
}

template <typename KeyType, typename ValueType>
void HashTable<KeyType, ValueType>::addToEndOfRecentList(Node* src)
{
	src->prev_rlist = m_tail_rlist;
	src->next_rlist = nullptr;
	m_tail_rlist = src;	// src is now most recent
	m_size_rlist++;
	if (src->prev_rlist != nullptr)	// has preceding Node
		src->prev_rlist->next_rlist = src;
	else
		m_head_rlist = src;	// first linked list item
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isPermanent(Node* src) const
{
	  // Simply checks to see if Node is part of embedded linked list
	return src->prev_rlist == nullptr && src->next_rlist == nullptr
		&& src != m_head_rlist;
}

/////////////////////////////////////
//	Public Member Functions
/////////////////////////////////////

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::HashTable(unsigned int numBuckets, unsigned int capacity)
{
	m_hash = new Node*[numBuckets]();
	m_head_rlist = m_tail_rlist = nullptr;
	m_numBuckets = numBuckets;
	m_capacity = capacity;
	m_size_hash = m_size_rlist = 0;
}

template <typename KeyType, typename ValueType>
HashTable<KeyType, ValueType>::~HashTable()
{
	Node* p = nullptr;
	for (unsigned int i = 0; i < m_numBuckets; i++)
	{
		p = m_hash[i];	// whatever linked list the bucket points to
		while (p != nullptr)	// delete linked list at each bucket
		{
			Node* killMe = p;
			p = p->next_hash;
			delete killMe;
		}
	}
	delete[] m_hash;	// delete dynamic array
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::isFull() const
{
	return m_size_hash == m_capacity;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::set(const KeyType& key, const ValueType& value, bool permanent)
{
	  // If key exists, update it
	int bucket = getBucketFromKey(key);
	Node* p = m_hash[bucket];
	Node* q = nullptr;
	while (p != nullptr)
	{
		if (p->key == key)	// key exists
		{
			p->value = value;
			makeMostRecentInRecentList(p);
			return true;
		}
		q = p;
		p = p->next_hash;
	}

	  // Key does not exist, add it to the hash table
	if (m_size_hash == m_capacity)
		return false;

	Node* n = new Node;
	n->key = key;
	n->value = value;
	n->next_hash = nullptr;
	if (q != nullptr)	// there are pre-existing items in the bucket
		q->next_hash = n;
	else  // no items in the bucket
		m_hash[bucket] = n;
	if (permanent)
		n->next_rlist = n->prev_rlist = nullptr;
	else  // need to add to embedded linked list
		addToEndOfRecentList(n);
	m_size_hash++;
	return true;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::get(const KeyType& key, ValueType& value) const
{
	int bucket = getBucketFromKey(key);
	Node* p = m_hash[bucket];
	while (p != nullptr)
	{
		if (p->key == key)
		{
			value = p->value;
			return true;
		}
		p = p->next_hash;
	}
	return false;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::touch(const KeyType& key)
{
	int bucket = getBucketFromKey(key);
	Node* p = m_hash[bucket];
	while (p != nullptr)
	{
		if (!isPermanent(p) && p->key == key)
		{
			makeMostRecentInRecentList(p);
			return true;
		}
		p = p->next_hash;
	}
	return false;
}

template <typename KeyType, typename ValueType>
bool HashTable<KeyType, ValueType>::discard(KeyType& key, ValueType& value)
{
	if (m_head_rlist == nullptr)	// no non-permanent items
		return false;

	key = m_head_rlist->key;
	value = m_head_rlist->value;

	int bucket = getBucketFromKey(key);
	Node* p = m_hash[bucket];	// guaranteed to be non-empty
	Node* q = nullptr;
	while (p != nullptr)
	{
		if (p == m_head_rlist)
			break;
		q = p;
		p = p->next_hash;
	}

	  // Update the hash structure
	if (q != nullptr)
		q->next_hash = p->next_hash;
	else
		m_hash[bucket] = p->next_hash;

	  // Update the embedded linked list structure
	m_head_rlist = p->next_rlist;
	if (m_head_rlist != nullptr)
		m_head_rlist->prev_rlist = nullptr;
	else
		m_tail_rlist = nullptr;

	  // Delete the target node, update sizes, return true
	delete p;
	m_size_hash--;
	m_size_rlist--;
	return true;
}

#endif // HASHTABLE_H
