//
//! Demo4.fx :스프라이트 셰이더. 
//!
//! \author	김기홍 Kihong Kim / onlysonim@gmail.com / mad_dog@hanmail.net
//! \date   2003.11.04. Updated. DX9.x 
//! \date   2010.12.01. Updated. DX11, Jun.2010
//! \date   2016.12.27. Updated. DX11/12, Window SDK 8.1 / Window 10 SDK 10.0.18362
//! \date   2018.12.30. Updated. DX11.x/12.x, Windows 10 SDK 10.0.18362
//! \date   2020.08.22. Updated. DX11.x/12.x, Windows 10 SDK 10.0.19041 
//! \date   2024.12.10. Updated. DX11.x/12.x, Windows 10 SDK 10.0.22621 (VS22)
//! \date   2025.09.01. Updated. DX11.x/12.x, Windows 10 SDK 10.0.26100 (VS22)
//
// < 지원기능 >
// 0.스프라이트 그리기용 
// 1.조명 없음 
// 2.단일 텍스처 멥핑
//   - 텍스처 변환 (UV 애니)
// 3.외부 지정색 혼합 지원.
//
#include "ConstantBuffers.hlsli"
#include "TexturesSampler.hlsli"


 
//VS 출력 구조체.
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv : TEXCOORD0;
};




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main : 정점 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main(
				  float4 pos : POSITION, //[입력] 정점좌표. Vertex Position (Model Space, 3D)
                  float2 uv : TEXCOORD0 //[입력] 텍스처 좌표 Texture Coordiates.
				)
{
	//정점 정리
    pos.w = 1;

	//월드 변환.(World Transform) 
    pos = mul(pos, mTM); //pos = pos * mTM
	
    //시야-뷰 변환 (View Transform)
    pos = mul(pos, mView);
 
    //원근 투영 변환 (Projection Transform)
    pos = mul(pos, mProj);

 
    //정보 출력.
    VSOutput o = (VSOutput) 0;
    o.pos = pos;
    o.col = Diffuse; //외부 지정 색으로 출력.
    o.uv = uv;


    return o;
}




////////////////////////////////////////////////////////////////////////////// 
//
// Vertex Shader Main2 : 정점 셰이더 메인 함수.(+Tex.Transform) 
//
////////////////////////////////////////////////////////////////////////////// 

VSOutput VS_Main2(
				  float4 pos : POSITION, //[입력] 정점좌표. Vertex Position (Model Space, 3D)
                  float2 uv : TEXCOORD0 //[입력] 텍스처 좌표 Texture Coordiates.
				)
{
	//* 아래의 테스트를 수행하기 전에  
    //* VS 에 상수 버퍼가 설정되어 있어야 합니다.    
    pos.w = 1;

	//월드 변환.(World Transform) 
    pos = mul(pos, mTM); //pos = pos * mTM
	
    //시야-뷰 변환 (View Transform)
    pos = mul(pos, mView);
 
    //원근 투영 변환 (Projection Transform)
    pos = mul(pos, mProj);

    
    //텍스쳐변환.
    float4 uvt = float4(uv.x, uv.y, 1, 0);
    uv = mul(uvt, mTex);
 
 
    //정보 출력.
    VSOutput o = (VSOutput) 0;
    o.pos = pos;
    o.col = Diffuse; //외부 지정 색으로 출력.
    o.uv = uv;


    return o;
}



 



////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader 전역 데이터.
//

//텍스처 객체 변수: 엔진에서 공급됨.
Texture2D texDiffuse;

//레지스터 직접 지정. (기본값은 t0)
//Texture2D texDiffuse : register(t0);     

//텍스처 셈플러. (엔진지정)
SamplerState smpLinear;

/*//텍스처 셈플러. (Effect Framework 사용시)
SamplerState smpLinear
{
    //Filter = MIN_MAG_MIP_LINEAR; //삼중선형 필터링 (Tril-Linear Filter)
    Filter = ANISOTROPY; //삼중선형 필터링 (Tril-Linear Filter)
    MaxAnisotropy = 8;
    AddressU = Wrap;
    AddressV = Wrap;
};
*/


////////////////////////////////////////////////////////////////////////////// 
//
// Pixel Shader Main : 픽셀 셰이더 메인 함수.
//
////////////////////////////////////////////////////////////////////////////// 


float4 PS_Main(
				float4 pos : SV_POSITION, //[입력] (보간된) 픽셀별 좌표. (Screen, 2D)
				float4 diff : COLOR0, //[입력] (보간된) 픽셀별 색상 
				float2 uv : TEXCOORD0 //[입력] 텍스처 좌표
				) : SV_TARGET //[출력] 색상.(필수), "렌더타겟" 으로 출력합니다.
{
      
     //텍스쳐 셈플링.
    float4 tex = texDiffuse.Sample(smpLinear, uv);
     
    //알파테스트 Alpha-Test
    //clip(tex.a == 0.0f ? -1 : 1);      //Alpha 0% 이하의 픽셀 버리기.
    //clip(tex.a < 0.01f ? -1 : 1);      //Alpha 1% 이하의 픽셀 버리기.
    clip(tex.a < 0.30f ? -1 : 1); //Alpha 30% 이하의 픽셀 버리기.
    //clip(tex.a < 0.70f ? -1 : 1);      //Alpha 70% 이하의 픽셀 버리기.
    //clip(tex.a < 0.85f ? -1 : 1);      //Alpha 85% 이하의 픽셀 버리기.

    
    //텍스처 혼합 출력 (곱셈, 기본)
    float4 col = tex * diff;
    //float4 col = tex;
    

    //색상 보정.
    //col.rgb *= 5.0f;
    //알파 보정.
    //col.a = tex.a * diff.a;

    /*//알파선행곱셈 : Pre-multiplyed Alpha 처리.
    // 참조1 : https://microsoft.github.io/Win2D/html/PremultipliedAlpha.htm
    // 참조2 : https://blogs.msdn.microsoft.com/shawnhar/2009/11/07/premultiplied-alpha-and-image-composition/
    col *= tex.a;
    col.a = tex.a;
	*/
    
    return col;
}




/**************** end of file "Demo4.fx" ***********************/
