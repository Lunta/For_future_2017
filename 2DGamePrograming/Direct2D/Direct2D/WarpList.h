#pragma once
template<typename Ty> // !! �ٷ� ���⿡
class WarpList;

template<typename Ty> // �����ݷ� ������ ����
class WarpNode {
private:
	friend class WarpList<Ty>;

	WarpNode* m_pNext{ nullptr };
	WarpNode* m_pPrev{ nullptr }; // �ʱⰪ�� nullptr �� �߰�ȣ �ʱ�ȭ

public:
	Ty m_Elem; // Ty�� Ŭ����

	//������
	WarpNode() = default;
	WarpNode(Ty& elem) : m_Elem(elem) {};
	WarpNode(Ty&& elem) : m_Elem(move(elem)) {};

	WarpNode* GetNext() { return m_pNext; }
	WarpNode* GetPrev() { return m_pPrev; }
};

template<typename Ty> // !! �ٷ� ���⿡
class WarpList {

public:
	using type = Ty;
	using Node = WarpNode<Ty>; // using �� ����.

public:

	//WarpNode m_wnhead -> �׳� �̷��� �ϸ� ��������
	//WarpNode<Ty> m_wnhead; -> �ص� ������ ���ϱ� !!�κп� (���� 11����) ���ø� �ѹ� �� ����
	Node m_wnHead;
	Node m_wnTail; // �̰� �����Ͱ� �ƴϴ�. �׻� ��� ������ �����ִ�.

	//������
	WarpList() 
	{
		m_wnHead.m_pNext = &m_wnTail;
		m_wnTail.m_pPrev = &m_wnHead; // ���� ������ ��Ų��. 
	}

	//ù��° ��ȿ���
	Node* begin() { return m_wnHead.m_pNext; }
	//return TailNode
	const Node* end() const { return &m_wnTail; }
	
	void push_front(Ty elem) // �ڷ� �о�ֱ�
	{
		Node *newNode = new Node(elem);
		Node *& oldFirst = m_wnHead.m_pNext; // ����� ������

		oldFirst->m_pPrev = newNode;
		newNode->m_pNext = oldFirst;

		newNode->m_pPrev = &m_wnHead;
		m_wnHead.m_pNext = newNode;
	}

	void push_back(Ty elem) // �ڷ� �о�ֱ�
	{
		Node *newNode = new Node(elem);
		Node *& oldLast = m_wnTail.m_pPrev; //������ ������

		oldLast->m_pNext = newNode;
		newNode->m_pPrev = oldLast;

		newNode->m_pNext = &m_wnTail;
		m_wnTail.m_pPrev = newNode;
	}

	
	bool insert(Ty elem, Node* Where) /*   // Node* �� �ϳ��ϱ� �ߺ��� �� �� ����*/
	{ // where�� �տ� ����, ���а��ɼ� ������ bool �� ��ȯ
		Node*p = &m_wnHead;

		while (p->m_pNext != nullptr)
		{
			if (p->m_pNext == Where) 
			{
				Node *newNode = new Node(elem);
				
				p->m_pNext = newNode;
				newNode->m_pPrev = p;

				newNode->m_pNext = Where;
				Where->m_pPrev = newNode;


				return true; // ���������� true;
			}

			p = p->m_pNext;
		}
		return false;
	}

	bool remove(Ty elem, bool AllRemove = true)
	{//elem ���� �Ȱ��� ��쿡�� �ڸ���
		Node*p = m_wnHead.m_pNext;
		size_t szdeleteCount = 0;
		while (p->m_pNext != nullptr)
		{
			if (p->m_Elem == elem)
			{
				Node* oldNext = p->m_pNext;
				Node* oldPrev = p->m_pPrev;

				oldNext->m_pPrev = oldPrev;
				oldPrev->m_pNext = oldNext;

				delete p;

				if (AllRemove == true)
				{
					p = oldNext;
					szdeleteCount++;
					continue;
				}
				else return true;
				
			}
			p = p->m_pNext;
		}
		return (szdeleteCount > 0); // ��ȣ�� ���� --> bool
	}
	
	template<typename Fn>
	bool remove_if(Fn& fn)//bool �Լ������Ͱ� �ִٰ� ��������
	{//elem ���� �Ȱ��� ��쿡�� �ڸ���
		Node* p = m_wnHead.m_pNext;
		size_t szDeleteCount = 0;

		while (p->m_pNext != nullptr)
		{
			if (fn(p->m_Elem))
			{
				Node* oldNext = p->m_pNext;
				Node* oldPrev = p->m_pPrev;

				oldNext->m_pPrev = oldPrev;
				oldPrev->m_pNext = oldNext;

				p->m_pPrev = nullptr;
				p->m_pNext = nullptr;
				delete p;
				p = nullptr;
				szDeleteCount++;
				p = oldNext;

				continue;
			}
			p = p->m_pNext;
		}
		return (szDeleteCount > 0);
	}

	Node* find(Ty elem)
	{
		for (Node* p = begin(); p->m_pNext != nullptr; p = p->m_pNext)
		{
			if (elem == p->m_Elem) return p;
		}
		return nullptr;
	}

	Ty pop_front()
	{
		Ty retval = move(begin()->m_Elem);
		 
		Node* oldBegin = begin();
		Node* oldNext = begin()->m_pNext;
		Node* oldPrev = &m_wnHead;

		oldNext->m_pPrev = oldPrev;
		oldPrev->m_pNext = oldNext;

		delete oldBegin;

		return move(retval);
	}

	Ty pop_back()
	{
		Ty retval = move(end()->m_pPrev->m_Elem);

		Node* oldEnd = end()->m_pPrev;
		Node* oldNext = &m_wnTail;
		Node* oldPrev = oldEnd->m_pPrev;

		oldNext->m_pPrev = oldPrev;
		oldPrev->m_pNext = oldNext;

		delete oldEnd;

		return move(retval);
	}
};