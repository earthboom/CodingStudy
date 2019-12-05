#pragma once

namespace ECS_tool
{
	class Math_Helper
	{
	public:
		explicit Math_Helper(void) = default;
		Math_Helper(const Math_Helper&) = delete;
		Math_Helper& operator=(const Math_Helper&) = delete;
		~Math_Helper(void) {}

	public:
		static DirectX::XMMATRIX FbxMatrix_to_XMMATRIX(FbxAMatrix _m)
		{
			DirectX::XMMATRIX _mat = DirectX::XMMATRIX(_m.Get(0, 0), _m.Get(0, 1), _m.Get(0, 2), _m.Get(0, 3),
										_m.Get(1, 0), _m.Get(1, 1), _m.Get(1, 2), _m.Get(1, 3),
										_m.Get(2, 0), _m.Get(2, 1), _m.Get(2, 2), _m.Get(2, 3),
										_m.Get(3, 0), _m.Get(3, 1), _m.Get(3, 2), _m.Get(3, 3));

			return _mat;
		};
	};
}