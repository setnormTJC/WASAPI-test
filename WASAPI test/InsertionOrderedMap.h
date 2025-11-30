#pragma once

#include<unordered_map>
#include<unordered_set>
#include<vector> 

#include"MyException.h"

/*a wrapper over unordered map that preserves insertion order by using extra space to track insertion order*/
template<typename Key, typename Value> 
class InsertionOrderedMap
{
private: 
	std::unordered_map<Key, Value> data; 
	std::vector<Key> insertionOrder; 

public: 
	void insert(const Key& key, const Value& value)
	{
		if (data.find(key) == data.end())
		{
			insertionOrder.push_back(key); 
		}

		data[key] = value; 
	}
	/*Just a wrapper over std::unordered_map::find that throws exception if not found*/
	Value& at(const Key& key)
	{
		if (data.find(key) == data.end())
		{
			throw MyException(__FILE__, __LINE__, "could not find key");
		}

		return data.at(key); 
	}
};

template<typename Key>
class InsertionOrderedSet
{
private:
	std::unordered_set<Key> data;
	std::vector<Key> insertionOrder;

public:
	void insert(const Key& key)
	{
		if (data.find(key) == data.end())
		{
			insertionOrder.push_back(key);  
		}

		data.insert(key);
	}

	bool find(const Key& key)
	{
		return (data.find(key) == data.end());
	}
};
