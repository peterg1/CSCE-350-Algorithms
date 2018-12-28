#pragma once
#include <cstddef>
#include<memory>
#include<algorithm>

/******************************
Dynamic array / vector
supports the following properties standard library vector:
-dynamic growth/memory allocation and random access operator[]
-push_back, begin, end, and size member functions
-Default constructor, fill constructor, copy constructor, destructor
-random access iterators
-back_inserter
*******************************/

template<class T>
class Vec
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef size_t size_type;
	typedef T value_type;

	size_type size() const {return avail - data;}
	T& operator[](size_type i) {return data[i];}
	const T& operator[] (size_type i) const {return data[i];}

	iterator begin() { return data; }
	const_iterator begin() const { return data; }
	
	iterator end() { return avail; }
	const_iterator end() const{ return limit; }

	Vec() { create(); }
	explicit Vec(size_type n, const T& val = T()) { create(n, val); }

	Vec(const Vec& v) {	create(v.begin(), v.end());}

	Vec& operator=(const Vec&);
	~Vec() { uncreate(); }

	void push_back(const T& val) {
		if (avail == limit)
			grow();
		unchecked_append(val);
	}

	T* vec_erase(T*);
	void vec_clear() { uncreate(); };
private:
	iterator data;
	iterator limit;
	iterator avail;

	std::allocator<T> alloc;
	
	void create();
	void create(size_type, const T&);
	void create(const_iterator, const_iterator);
	
	void uncreate();
	
	void grow();
	void unchecked_append(const T&);
};



template<class T>
void Vec<T>::create()
{
	data = limit = avail = 0;
}


template<class T> void Vec<T>::create(size_type n, const T& val)
{
	data = alloc.allocate(n);
	limit = avail = data + n;
	std::uninitialized_fill(data, limit, val);
}

template<class T>
void Vec<T>::create(const_iterator i, const_iterator j)
{
	data = alloc.allocate(j - i);
	limit = avail = std::uninitialized_copy(i, j, data);
}

template<class T>
Vec<T>& Vec<T>::operator=(const Vec& rhs)
{
	if (&rhs != this)
	{
		uncreate();
		create(rhs.begin(), rhs.end());
	}
	return *this;
}

template<class T>
void Vec<T>::uncreate()
{
	if (data)
	{
		iterator it = avail;
		while (it != data)
		{
			alloc.destroy(--it);
		}
		alloc.deallocate(data, limit - data);
	}
	data = limit = avail = 0;
}

template<class T>
void Vec<T>::grow()
{
	size_type new_size = std::max(2 * (limit - data), ptrdiff_t(1));
	iterator new_data = alloc.allocate(new_size);
	iterator new_avail = std::uninitialized_copy(data, avail, new_data);

	uncreate();
	data = new_data;
	avail = new_avail;
	limit = data+new_size;
}

template<class T>
void Vec<T>::unchecked_append(const T& val)
{
	alloc.construct(avail++, val);
}


template<class T>
T* Vec<T>::vec_erase(T* iter)
{
	T* ret = iter;
	T* replace = iter;
	while (replace != avail)
		*replace++ = *++iter;
	alloc.destroy(avail);
	avail--;
	return ret;
}
