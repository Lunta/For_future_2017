#pragma once

#include <ostream>
#include <vector>
#include <cassert>

template<class KeyType>
class IndexedPriorityQLow
{
private:
	std::vector<KeyType>&  m_vecKeys;
	std::vector<int>       m_Heap;
	std::vector<int>       m_invHeap;
	int                    m_iSize, m_iMaxSize;

	void Swap(int a, int b)
	{
		int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

		//change the handles too
		m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
	}

	void ReorderUpwards(int nd)
	{
		//move up the heap swapping the elements until the heap is ordered
		while ((nd > 1) && (m_vecKeys[m_Heap[nd / 2]] > m_vecKeys[m_Heap[nd]]))
		{
			Swap(nd / 2, nd);

			nd /= 2;
		}
	}

	void ReorderDownwards(int nd, int HeapSize)
	{
		//move down the heap from node nd swapping the elements until
		//the heap is reordered
		while (2 * nd <= HeapSize)
		{
			int child = 2 * nd;

			//set child to smaller of nd's two children
			if ((child < HeapSize) &&
				(m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child + 1]]))
			{
				++child;
			}

			//if this nd is larger than its child, swap
			if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
			{
				Swap(child, nd);

				//move the current node down the tree
				nd = child;
			}
			else
			{
				break;
			}
		}
	}

public:
	//you must pass the constructor a reference to the std::vector the PQ
	//will be indexing into and the maximum size of the queue.
	IndexedPriorityQLow(std::vector<KeyType>& keys,
		int              MaxSize) :m_vecKeys(keys),
		m_iMaxSize(MaxSize),
		m_iSize(0)
	{
		m_Heap.assign(MaxSize + 1, 0);
		m_invHeap.assign(MaxSize + 1, 0);
	}

	bool empty()const { return (m_iSize == 0); }

	void insert(const int idx)
	{
		assert(m_iSize + 1 <= m_iMaxSize);

		++m_iSize;

		m_Heap[m_iSize] = idx;

		m_invHeap[idx] = m_iSize;

		ReorderUpwards(m_iSize);
	}

	int Pop()
	{
		Swap(1, m_iSize);

		ReorderDownwards(1, m_iSize - 1);

		return m_Heap[m_iSize--];
	}

	void ChangePriority(const int idx)
	{
		ReorderUpwards(m_invHeap[idx]);
	}
};
