/* Copyright (C) 2005-2020, UNIGINE. All rights reserved.
 *
 * This file is a part of the UNIGINE 2.11.0.1 SDK.
 *
 * Your use and / or redistribution of this software in source and / or
 * binary form, with or without modification, is subject to: (i) your
 * ongoing acceptance of and compliance with the terms and conditions of
 * the UNIGINE License Agreement; and (ii) your inclusion of this notice
 * in any version of this software that you use or redistribute.
 * A copy of the UNIGINE License Agreement is available by contacting
 * UNIGINE. at http://unigine.com/
 */


#pragma once

#include <UnigineHash.h>

namespace Unigine
{

template<typename Key, typename HashType>
struct HashSetData
{
	const HashType hash;
	const Key key;
	HashSetData(HashType h, const Key &k)
		: hash(h)
		, key(k)
	{}
	HashSetData(HashType h, Key &&k)
		: hash(h)
		, key(std::move(k))
	{}

	static UNIGINE_INLINE void *operator new(size_t size) { return Memory::allocate(size); }
	static UNIGINE_INLINE void operator delete(void *ptr) { Memory::deallocate(ptr); }
	static UNIGINE_INLINE void operator delete(void *ptr, size_t size) { Memory::deallocate(ptr,size); }

};

template <typename Key, typename Counter = unsigned int>
class HashSet : public Hash<Key, HashSetData<Key, typename Hasher<Key>::HashType>, typename Hasher<Key>::HashType, Counter>
{
public:

	using HashType = typename Hasher<Key>::HashType;
	using Parent = Hash<Key, HashSetData<Key, typename Hasher<Key>::HashType>, typename Hasher<Key>::HashType, Counter>;
	using Iterator = typename Parent::Iterator;
	using ConstIterator = typename Parent::ConstIterator;

	using iterator = typename Parent::iterator;
	using const_iterator = typename Parent::const_iterator;

	HashSet()
	{
		Parent::data = nullptr;
		Parent::length = 0;
		Parent::capacity = 0;
	}

	~HashSet()
	{
		if (Parent::data == nullptr)
			return;
		for (Counter i = 0; i < Parent::capacity; ++i)
			delete Parent::data[i];
		delete [] Parent::data;
	}

	HashSet(const HashSet &o)
	{
		Parent::data = nullptr;
		Parent::length = 0;
		Parent::capacity = 0;
		Parent::rehash(o.capacity);
		for (const auto &it : o)
			Parent::do_append(it.hash, it.key);
	}

	HashSet(HashSet &&o)
	{
		Parent::length = o.length;
		Parent::capacity = o.capacity;
		Parent::data = o.data;

		o.length = 0;
		o.capacity = 0;
		o.data = nullptr;
	}

	HashSet(std::initializer_list<Key> list)
	{
		Parent::data = nullptr;
		Parent::length = 0;
		Parent::capacity = 0;
		for (const auto &v : list)
			append(v);
	}

	HashSet &operator=(const HashSet &o)
	{
		if (this == &o)
			return *this;
		Parent::clear();
		Parent::reserve(static_cast<Counter>(o.capacity * HASH_LOAD_FACTOR));
		for (const auto &it : o)
			Parent::do_append(it.hash, it.key);
		return *this;
	}

	HashSet &operator=(HashSet &&o)
	{
		if (this == &o)
			return *this;
		for (Counter i = 0; i < Parent::capacity; ++i)
			delete Parent::data[i];

		delete [] Parent::data;
		Parent::length = o.length;
		Parent::capacity = o.capacity;
		Parent::data = o.data;

		o.length = 0;
		o.capacity = 0;
		o.data = nullptr;
		return *this;
	}

	UNIGINE_INLINE void append(const Key &key) { Parent::do_append(key); }
	UNIGINE_INLINE void append(Key &&key) { Parent::do_append(std::move(key)); }

	UNIGINE_INLINE void append(const HashSet &o)
	{
		for (const auto &it : o)
			Parent::do_append(it.hash, it.key);
	}

	UNIGINE_INLINE void append(HashSet &&o) {
		for (const auto &it : o)
			Parent::do_append(it.hash, std::move(it.key));
		o.clear();
	}

	UNIGINE_INLINE void insert(const Key &key) { Parent::do_append(key); }
	UNIGINE_INLINE void insert(Key &&key) { Parent::do_append(std::move(key)); }
	UNIGINE_INLINE void insert(const HashSet &o) { append(o); }
	UNIGINE_INLINE void insert(HashSet &&o) { append(std::move(o)); }

};

} // namespace Unigine
