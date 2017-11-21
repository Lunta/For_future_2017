#pragma once

class CObject
{
public:
	
	CObject(D2D_POINT_2F pt = Point2F(), D2D_RECT_F rc = RectF());

	virtual ~CObject();

	virtual void Update(float fTimeElapsed) {}
	virtual void Draw(ID2D1HwndRenderTarget* RenderTarget) = 0;
	virtual void DrawUI(ID2D1HwndRenderTarget* RenderTarget) {}

	void SetPosition(D2D_POINT_2F pt) { m_ptPoisition = pt; }
	void Move(D2D_POINT_2F pt){ m_ptPoisition = m_ptPoisition + pt; }
	D2D_POINT_2F GetPosition() const { return m_ptPoisition; }

	void SetSize(D2D_RECT_F pt) { m_rcSize = pt; }
	D2D_RECT_F GetSize() const { return m_rcSize; }

protected:

	D2D_POINT_2F	m_ptPoisition;
	D2D_RECT_F		m_rcSize;
};
