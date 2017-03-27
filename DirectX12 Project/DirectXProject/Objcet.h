#pragma once
class CObjcet
{
private:
	ID3D11InputLayout	*m_pLayout;				// the pointer to the input layout
	ID3D11VertexShader	*m_pVS;					// the pointer to the vertex shader
	ID3D11PixelShader	*m_pPS;					// the pointer to the pixel shader

public:
	CObjcet(ID3D11Device *pD3D_Device, ID3D11DeviceContext *pD3D_DC);
	~CObjcet();

	virtual void InitPipeline(ID3D11Device *pD3D_Device, ID3D11DeviceContext *pD3D_DC);
	virtual void Update(float fTimeElapsed) = 0;
	virtual void Render() = 0;

};

