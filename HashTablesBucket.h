#ifndef __HASHTABLESBUCKET_H__
#define __HASHTABLESBUCKET_H__

template<class K, class V>
struct HashTablesBucketNode
{
	K _key;
	V _value;
	HashTablesBucketNode<K, V>* _next;

	HashTablesBucketNode(const K& key, const V& value)
		:_key(key)
		, _value(value)
		, _next(NULL)
	{}
};

template<class K>
struct DefaultHashFunce
{
	size_t operator()(const K& key)
	{
		return key;
	}
};

template<>
struct DefaultHashFunce<string>
{
	static size_t BKDRHash(const char * str)
	{
		unsigned int seed = 131; // 31 131 1313 13131 131313
		unsigned int hash = 0;
		while (*str)
		{
			hash = hash * seed + (*str++);
		}
		return (hash & 0x7FFFFFFF);
	}

	size_t operator()(const string& key)
	{
		return BKDRHash(key.c_str());
	}

};

template<class K,class V,class HashFuncer = DefaultHashFunce<K>>
class HashTablesBucket
{
	typedef HashTablesBucketNode<K, V> Node;
public:
	HashTablesBucket()
		:_size(0)
	{}

	bool Insert(const K& key,const V& value)
	{
		_CheckExpend();

		size_t index = HashKey(key);
		Node* begin = _tables[index];
		//判断有没有插入重复的数据
		while (begin)
		{
			if (begin->_key == key)
			{
				return false;
			}
			begin = begin->_next;
		}

		Node* tmp = new Node(key, value);
		tmp->_next = _tables[index];
		_tables[index] = tmp;
		_size++;

		return true;
	}

	Node* Find(const K& key)
	{
		for (int i = 0; i < _tables.size(); i++)
		{
			Node* cur = _tables[i];
			while (cur)
			{
				if (cur->_key == key)
				{
					return cur;
				}
				cur = cur->_next;
			}
		}
		return NULL;
	}

	bool Remove(const K& key)
	{
		int index = HashKey(key);
		Node* cur = _tables[index];
		Node* prev = NULL;
		while (cur)
		{
			if (cur->_key != key)
			{
				prev = cur;
				cur = cur->_next;
			}
			else
			break;
		}

		if (cur != NULL)
		{
			if (_tables[index] == cur)
			_tables[index] = cur->_next;

			else
			prev->_next = cur->_next;

			delete cur;
			cur = NULL;
		}

		return false;
	}

	size_t HashKey(const K& key)
	{
		return HashFuncer()(key)%_tables.size();
	}

	void PrintHashTables()
	{
		for (int i = 0; i < _tables.size(); i++)
		{
			Node* cur = _tables[i];
			while (cur)
			{
				cout << cur->_key << "->";
				cur = cur->_next;
			}
			cout << "Null" << endl;
		}
	}

protected:
	size_t GetPrimeSize(size_t size)
	{
		static const int _PrimeSize = 28;
		static const unsigned long _PrimeList[_PrimeSize] =
		{
			53ul, 97ul, 193ul, 389ul, 769ul,
			1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
			49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
			1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
			50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
			1610612741ul, 3221225473ul, 4294967291ul
		};

		for (size_t i = 0; i < _PrimeSize; i++)
		{
			if (_PrimeList[i] > size)
			{
				return _PrimeList[i];
			}
			if (_PrimeList[_PrimeSize] == size)
				return _PrimeList[_PrimeSize];
		}
		return _PrimeList[_PrimeSize];
	}
	void _CheckExpend()
	{
		if (_size == _tables.size())
		{
			//得到素数的size
			size_t newSize = GetPrimeSize(_size);
			//开辟空间
			vector<Node*> newTables;
			newTables.resize(newSize);
			//摘节点插入新的哈希桶中
			for (int i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				Node* tmp = NULL;
				while (cur)
				{
					tmp = cur;
					cur = cur->_next;

					size_t index = HashKey(tmp->_key);
					tmp->_next = newTables[index];
					newTables[index] = tmp;

				}
				_tables[i] = NULL;
			}
			_tables.swap(newTables);
		}
	}

protected:
	vector<Node*> _tables;
	size_t _size;
};



void TestHashTables()
{
	HashTablesBucket<int, char> ht;
	for (int i = 0; i < 53; i++)
	{
		ht.Insert(i, i);
	}

	ht.Insert(54, 54);
	ht.Insert(151, 151);
	ht.Insert(55, 55);
	
	cout << ht.Find(54)->_key << endl;
	ht.Remove(54);
	ht.Remove(52);

	ht.PrintHashTables();
}

void TestHashKV()
{
	//常规写法
	//HashTablesBucket<string, string> dict;
	//dict.Insert("助手", "assist");
	//dict.Insert("删除", "delete、removed、erase");
	//dict.Insert("土豪", "peter");
	//dict.Insert("有钱人", "yang");
	//cout << dict.Find("删除")->_value << endl;
	///*dict.PrintHashTables();*/

	//为了解决删除有相同的词的排版问题，用vector将其装起来
	HashTablesBucket<string, vector<string>> dict;
	vector<string> v;
	v.push_back("assist");
	dict.Insert("助手",v);
	v.clear();
	v.push_back("delete");
	v.push_back("remove");
	dict.Insert("删除",v);
	dict.Find("删除")->_value.push_back("erase");
	vector<string>& words = dict.Find("删除")->_value;
	for (int i = 0; i < words.size(); i++)
	{
		cout << words[i] << endl;
	}

}

#endif //__HASHTABLESBUCKET_H__