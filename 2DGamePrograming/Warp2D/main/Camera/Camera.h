#pragma once

class CCamera
{
protected:
	D2D_POINT_2F					m_d2dptPosition;
	D2D_MATRIX_3X2_F				m_d2dmtxWorld;
	D2D_MATRIX_3X2_F				m_d2dmtxView;

	D2D_POINT_2F					m_d2dptClientSize;
	bool							m_bControlLock;
	float							m_fScale;

	D2D_POINT_2F					m_d2dptAnchor {0,0};

private:
	void SetMatrix();
	D2D1_POINT_2F GetCameraPosition() const;

public:
	CCamera();
	virtual ~CCamera();

	D2D1_POINT_2F GetAnchor() const { return m_d2dptAnchor; }
	D2D1_POINT_2F GetPosition() const { return m_d2dptPosition; }

	D2D_RECT_F GetViewRect() const;
	virtual void Move(D2D1_POINT_2F& d2dptShift) { m_d2dptPosition = m_d2dptPosition + d2dptShift; }
	virtual void SetPosition(D2D1_POINT_2F& ptPosition) { m_d2dptPosition = ptPosition; }
	// (-1, -1) ¡Â ptAnchor ¡Â (+1, +1)
	virtual void SetAnchor(D2D1_POINT_2F& ptAnchor) { m_d2dptAnchor = ptAnchor; }

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	virtual void Scale(float Scale){ m_fScale = Scale; }
	float GetScale(){ return m_fScale; }
	void AgjustScale(float Scale){ m_fScale += Scale; }
	virtual void SetLookAt(D2D1_POINT_2F& d2dptLookAt) { }
	D2D_MATRIX_3X2_F GetCameraMatrix(){ SetMatrix(); return m_d2dmtxWorld; }
	D2D_MATRIX_3X2_F RegenerateViewMatrix();

	void SetClientSize(D2D_POINT_2F ptClientSize);
	bool CheckControlLock() const { return m_bControlLock; }

};
