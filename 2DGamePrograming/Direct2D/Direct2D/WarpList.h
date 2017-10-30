#pragma once
template<typename Ty> // !! 바로 여기에
class WarpList;

template<typename Ty> // 세미콜론 붙이지 마라
class WarpNode {
private:
	friend class WarpList<Ty>;

	WarpNode* m_pNext{ nullptr };
	WarpNode* m_pPrev{ nullptr }; // 초기값은 nullptr 로 중괄호 초기화

public:
	Ty m_Elem; // Ty는 클래스

	//생성자
	WarpNode() = default;
	WarpNode(Ty& elem) : m_Elem(elem) {};
	WarpNode(Ty&& elem) : m_Elem(move(elem)) {};

	WarpNode* GetNext() { return m_pNext; }
	WarpNode* GetPrev() { return m_pPrev; }
};

template<typename Ty> // !! 바로 여기에
class WarpList {

public:
	using type = Ty;
	using Node = WarpNode<Ty>; // using 을 쓴다.

public:

	//WarpNode m_wnhead -> 그냥 이렇게 하면 오류나고
	//WarpNode<Ty> m_wnhead; -> 해도 오류가 나니까 !!부분에 (위에 11번줄) 템플릿 한번 더 선언
	Node m_wnHead;
	Node m_wnTail; // 이거 포인터가 아니다. 항상 헤드 테일은 남아있다.

	//생성자
	WarpList() 
	{
		m_wnHead.m_pNext = &m_wnTail;
		m_wnTail.m_pPrev = &m_wnHead; // 서로 연결을 시킨다. 
	}

	//첫번째 유효노드
	Node* begin() { return m_wnHead.m_pNext; }
	//return TailNode
	const Node* end() const { return &m_wnTail; }
	
	void push_front(Ty elem) // 뒤로 밀어넣기
	{
		Node *newNode = new Node(elem);
		Node *& oldFirst = m_wnHead.m_pNext; // 헤드의 다음꺼

		oldFirst->m_pPrev = newNode;
		newNode->m_pNext = oldFirst;

		newNode->m_pPrev = &m_wnHead;
		m_wnHead.m_pNext = newNode;
	}

	void push_back(Ty elem) // 뒤로 밀어넣기
	{
		Node *newNode = new Node(elem);
		Node *& oldLast = m_wnTail.m_pPrev; //테일의 이전꺼

		oldLast->m_pNext = newNode;
		newNode->m_pPrev = oldLast;

		newNode->m_pNext = &m_wnTail;
		m_wnTail.m_pPrev = newNode;
	}

	
	bool insert(Ty elem, Node* Where) /*   // Node* 는 하나니까 중복이 될 수 없다*/
	{ // where의 앞에 삽입, 실패가능성 있으니 bool 로 반환
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


				return true; // 성공했으니 true;
			}

			p = p->m_pNext;
		}
		return false;
	}

	bool remove(Ty elem, bool AllRemove = true)
	{//elem 값이 똑같을 경우에만 자르자
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
		return (szdeleteCount > 0); // 괄호는 보험 --> bool
	}
	
	template<typename Fn>
	bool remove_if(Fn& fn)//bool 함수포인터가 있다고 가정하자
	{//elem 값이 똑같을 경우에만 자르자
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