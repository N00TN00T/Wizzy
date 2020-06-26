#pragma once

#include "Rect.h"

#define INDEX_NO_FIT	   -1
#define INDEX_BOTTOM_LEFT	0
#define INDEX_BOTTOM_RIGHT	1
#define INDEX_TOP_LEFT		2
#define INDEX_TOP_RIGHT		3

template <class T>
class QuadNode
{
public:
	QuadNode<T>(Rect bounds, u32 capacity = 5, u32 maxDepth = 5, int32 depth = 0);
	inline ~QuadNode<T>()
	{
		for (auto _c : m_subNodes)
		{
			delete _c;
		}
	}
	void Clear();
	QuadNode<T>* Insert(T item, Rect itemBounds);
	std::vector<T> GetRelevantItems(Rect area);
	void RemoveItem(T item);

	std::vector<Rect> GetRects();

private:
	Rect m_bounds;
	int32 m_depth;
	std::vector<T> m_items;
	std::map<T, Rect> m_itemBounds;
	QuadNode<T>* m_subNodes[4];
	bool m_hasSubNodes;
	u32 m_capacity;
	u32 m_maxDepth;
private:
	void Split();
	void Migrate();
	int32 GetIndex(Rect itemBounds);
};

template<class T>
inline void QuadNode<T>::Split()
{
	fvec2 _subSize(m_bounds.w / 2.f, m_bounds.h / 2.f);
	fvec2 _bottomLeft(m_bounds.Left(), m_bounds.Bottom());

	m_subNodes[INDEX_BOTTOM_LEFT] = new QuadNode<T>(Rect(_bottomLeft.x, _bottomLeft.y, _subSize.x, _subSize.y), m_capacity, m_maxDepth, m_depth + 1);

	m_subNodes[INDEX_BOTTOM_RIGHT] = new QuadNode<T>(Rect(_bottomLeft.x + _subSize.x, _bottomLeft.y, _subSize.x, _subSize.y), m_capacity, m_maxDepth, m_depth + 1);

	m_subNodes[INDEX_TOP_LEFT] = new QuadNode<T>(Rect(_bottomLeft.x, _bottomLeft.y + _subSize.y, _subSize.x, _subSize.y), m_capacity, m_maxDepth, m_depth + 1);

	m_subNodes[INDEX_TOP_RIGHT] = new QuadNode<T>(Rect(_bottomLeft.x + _subSize.x, _bottomLeft.y + _subSize.y, _subSize.x, _subSize.y), m_capacity, m_maxDepth, m_depth + 1);

	m_hasSubNodes = true;
}

template<class T>
inline int32 QuadNode<T>::GetIndex(Rect itemBounds)
{
	int32 _index = -1;

	auto _center = fvec2(m_bounds.Left() + m_bounds.w / 2.f, m_bounds.Bottom() + m_bounds.h / 2.f);

	bool _isInTopNodes = itemBounds.Top() > _center.y&& itemBounds.Bottom() > _center.y;
	bool _isInBottomNodes = itemBounds.Top() < _center.y;

	if (itemBounds.Left() < _center.x && itemBounds.Right() < _center.x)
	{
		if (_isInTopNodes)
		{
			_index = INDEX_TOP_LEFT;
		}
		if (_isInBottomNodes)
		{
			_index = INDEX_BOTTOM_LEFT;
		}
	}
	else if (itemBounds.Left() > _center.x)
	{
		if (_isInTopNodes)
		{
			_index = INDEX_TOP_RIGHT;
		}
		if (_isInBottomNodes)
		{
			_index = INDEX_BOTTOM_RIGHT;
		}
	}

	return _index;
}

template<class T>
inline void QuadNode<T>::Migrate()
{
	for (int32 i = m_items.size() - 1; i >= 0; i--)
	{
		int32 _subIndex = GetIndex(m_itemBounds[m_items[i]]);
		if (_subIndex != INDEX_NO_FIT)
		{
			m_subNodes[_subIndex]->Insert(m_items[i], m_itemBounds[m_items[i]]);
			m_items.erase(m_items.begin() + i);
		}
	}
}

template<class T>
inline QuadNode<T>::QuadNode(Rect bounds, u32 capacity, u32 maxDepth, int32 depth)
	: m_bounds(bounds), m_capacity(capacity), m_maxDepth(maxDepth), m_depth(depth), m_hasSubNodes(false)
{ 
	for (int i = 0; i < 4; i++) m_subNodes[i] = nullptr;
}

template<class T>
inline void QuadNode<T>::Clear()
{
	m_items.clear();
	m_itemBounds.clear();
	if (m_hasSubNodes)
	{
		for (auto& _n : m_subNodes)
		{
			delete _n;
		}
		m_hasSubNodes = false;
	}
}

template<class T>
inline QuadNode<T>* QuadNode<T>::Insert(T item, Rect itemBounds)
{
	if (m_hasSubNodes)
	{
		int32 _index = GetIndex(itemBounds);
		if (_index != INDEX_NO_FIT)
		{
			return m_subNodes[_index]->Insert(item, itemBounds);
		}
	}

	m_items.push_back(item);
	m_itemBounds[item] = itemBounds;

	if (m_items.size() > m_capacity && m_depth < m_maxDepth && !m_hasSubNodes)
	{
		Split();
	}

	if (m_hasSubNodes)
	{
		Migrate();
	}

	return this;
}

template<class T>
inline std::vector<T> QuadNode<T>::GetRelevantItems(Rect bounds)
{
	std::vector<T> _items;

	if (m_hasSubNodes)
	{
		int32 _index = GetIndex(bounds);
		if (_index != INDEX_NO_FIT)
		{
			std::vector<T> _subItems = m_subNodes[_index]->GetRelevantItems(bounds);
			_items.insert(std::end(_items), std::begin(_subItems), std::end(_subItems));
		}
		else
		{
			for (auto& _n : m_subNodes)
			{
				std::vector<T> _subItems = _n->GetRelevantItems(bounds);
				_items.insert(std::end(_items), std::begin(_subItems), std::end(_subItems));
			}
		}
	}

	for (auto& _i : m_items)
	{
		if (bounds.Intersects(&m_itemBounds[_i]))
			_items.push_back(_i);
	}

	return _items;
}

template<class T>
inline void QuadNode<T>::RemoveItem(T item)
{
	if (m_hasSubNodes)
	{
		for (auto _subnode : m_subNodes)
		{
			_subnode->RemoveItem(item);
		}
	}
	auto _idx = vector_index_of(m_items, item);
	if (_idx >= 0)
	{
		m_itemBounds.erase(m_items[_idx]);
		m_items.erase(m_items.begin() + _idx);
	}
}

template<class T>
inline std::vector<Rect> QuadNode<T>::GetRects()
{
	std::vector<Rect> vec;

	vec.push_back(m_bounds);

	if (m_hasSubNodes)
	{
		for (auto subnode : m_subNodes)
		{
			auto subVec = subnode->GetRects();
			vec.insert(vec.end(), subVec.begin(), subVec.end());
		}
	}

	return vec;
}

