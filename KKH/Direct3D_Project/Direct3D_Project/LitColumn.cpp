#include "stdafx.h"
#include "LitColumn.h"
#include "GeometryGenerator.h"
#include "d3dutil_Manager.h"
#include "Utility_Manager.h"
#include "MathHelper.h"
#include "Timer_Manager.h"

LitColumn::LitColumn(void)
{
}

LitColumn::~LitColumn(void)
{
}

bool LitColumn::Ready(void)
{
	BuildShapeGeometry();
	BuildSkullGeometry();
	BuildMaterials();
	BuildRenderItem();

	return TRUE;
}

bool LitColumn::Update(const float & dt)
{
	UTIL.Get_CurrFrameResourceIndex() = (UTIL.Get_CurrFrameResourceIndex() + 1) % NumFrameResources;
	UTIL.Get_CurrFrameResource() = UTIL.Get_Frameres()[UTIL.Get_CurrFrameResourceIndex()].get();

	if (UTIL.Get_CurrFrameResource()->Fence != 0 && GRAPHIC->Get_Fence()->GetCompletedValue() < UTIL.Get_CurrFrameResource()->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
		ThrowIfFailed(GRAPHIC->Get_Fence()->SetEventOnCompletion(UTIL.Get_CurrFrameResource()->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	AnimateMaterials(dt);
	UpdateObjectCBs(dt);
	UpdateMaterialCBs(dt);
	UpdateMainPassCB(dt);

	return TRUE;
}

bool LitColumn::Render(const float & dt)
{
	auto cmdListAlloc = UTIL.Get_CurrFrameResource()->CmdListAlloc;

	ThrowIfFailed(cmdListAlloc->Reset());
	ThrowIfFailed(COM_LIST->Reset(cmdListAlloc.Get(), UTIL.Get_PSOs("opaque").Get()));

	COM_LIST->RSSetViewports(1, &GRAPHIC->Get_ScreenViewport());
	COM_LIST->RSSetScissorRects(1, &GRAPHIC->Get_ScissorRect());

	COM_LIST->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	COM_LIST->ClearRenderTargetView(GRAPHIC->Get_CurrentBackBufferView_Handle(), Colors::LightSteelBlue, 0, nullptr);
	COM_LIST->ClearDepthStencilView(GRAPHIC->Get_DepthStencilView_Handle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0.0f, 0.0f, nullptr);

	COM_LIST->OMSetRenderTargets(1, &GRAPHIC->Get_CurrentBackBufferView_Handle(), TRUE, &GRAPHIC->Get_DepthStencilView_Handle());

	COM_LIST->SetGraphicsRootSignature(UTIL.Get_RootSignature().Get());

	auto passCB = UTIL.Get_CurrFrameResource()->PassCB->Resource();
	COM_LIST->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

	DrawRenderItems(COM_LIST.Get(), UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE));

	COM_LIST->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(GRAPHIC->Get_CurrentBackBuffer_Resource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(COM_LIST->Close());

	ID3D12CommandList* cmdsLists[] = { COM_LIST.Get() };
	GRAPHIC->Get_CommandQueue()->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	ThrowIfFailed(GRAPHIC->Get_SwapChain()->Present(0, 0));
	GRAPHIC->Get_Set_CurrBackBuffer() = (GRAPHIC->Get_Set_CurrBackBuffer() + 1) % GRAPHIC->Get_SwapChainBufferCount();

	UTIL.Get_CurrFrameResource()->Fence = ++GRAPHIC->Get_CurrentFence();

	GRAPHIC->Get_CommandQueue()->Signal(GRAPHIC->Get_Fence().Get(), GRAPHIC->Get_CurrentFence());

	return TRUE;
}

void LitColumn::DrawRenderItems(ID3D12GraphicsCommandList * cmdList, const std::vector<RenderItem*>& ritems)
{
	UINT objCBByteSize = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dutil_Mananger::CalcConstantBufferByteSize(sizeof(MaterialConstants));

	auto objectCB = UTIL.Get_CurrFrameResource()->ObjectCB->Resource();
	auto matCB = UTIL.Get_CurrFrameResource()->MaterialCB->Resource();

	// For each render item...
	for (size_t i = 0; i < ritems.size(); ++i)
	{
		auto ri = ritems[i];

		cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
		cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
		cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + ri->objCBIndex*objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex*matCBByteSize;

		cmdList->SetGraphicsRootConstantBufferView(0, objCBAddress);
		cmdList->SetGraphicsRootConstantBufferView(1, matCBAddress);

		cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
	}
}

void LitColumn::AnimateMaterials(const float & dt)
{
}

void LitColumn::UpdateObjectCBs(const float & dt)
{
	auto currObjectCB = UTIL.Get_CurrFrameResource()->ObjectCB.get();
	for (auto& e : UTIL.Get_Ritemvec())
	{
		if (e->NumFramesDirty > 0)
		{
			DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&e->World);
			DirectX::XMMATRIX texTransform = DirectX::XMLoadFloat4x4(&e->TexTransform);

			ObjectConstants objConstants;
			DirectX::XMStoreFloat4x4(&objConstants.World, DirectX::XMMatrixTranspose(world));
			DirectX::XMStoreFloat4x4(&objConstants.TexTransform, DirectX::XMMatrixTranspose(texTransform));

			currObjectCB->CopyData(e->objCBIndex, objConstants);

			e->NumFramesDirty--;
		}
	}
}

void LitColumn::UpdateMaterialCBs(const float & dt)
{
	auto currMaterialCB = UTIL.Get_CurrFrameResource()->MaterialCB.get();
	for (auto& e : UTIL.Get_Materials())
	{
		Material* mat = e.second.get();
		if (mat->NumFrameDirty > 0)
		{
			DirectX::XMMATRIX matTransform = DirectX::XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			DirectX::XMStoreFloat4x4(&matConstants.MatTransform, DirectX::XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			mat->NumFrameDirty--;
		}
	}
}

void LitColumn::UpdateMainPassCB(const float & dt)
{
	XMMATRIX view = XMLoadFloat4x4(&UTIL.Get_ViewMat());
	XMMATRIX proj = XMLoadFloat4x4(&g_Proj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&UTIL.Get_MainPassCB().View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&UTIL.Get_MainPassCB().InvViewProj, XMMatrixTranspose(invViewProj));
	UTIL.Get_MainPassCB().EyePosW = g_EyePos;
	UTIL.Get_MainPassCB().RenderTargetSize = XMFLOAT2((float)WINSIZE_X, (float)WINSIZE_Y);
	UTIL.Get_MainPassCB().InvRenderTargetSize = XMFLOAT2(1.0f / WINSIZE_X, 1.0f / WINSIZE_Y);
	UTIL.Get_MainPassCB().NearZ = 1.0f;
	UTIL.Get_MainPassCB().FarZ = 1000.0f;
	UTIL.Get_MainPassCB().TotalTime = TIME_MGR.Get_TotalTime(L"MainTimer");
	UTIL.Get_MainPassCB().DeltaTime = TIME_MGR.Get_TimeDelta(L"MainTimer");
	UTIL.Get_MainPassCB().AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	UTIL.Get_MainPassCB().Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	UTIL.Get_MainPassCB().Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	UTIL.Get_MainPassCB().Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	UTIL.Get_MainPassCB().Lights[1].Strength = { 0.3f, 0.3f, 0.3f };
	UTIL.Get_MainPassCB().Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	UTIL.Get_MainPassCB().Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	auto currPassCB = UTIL.Get_CurrFrameResource()->PassCB.get();
	currPassCB->CopyData(0, UTIL.Get_MainPassCB());
}

void LitColumn::BuildShapeGeometry(void)
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 0.5f, 1.5f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(20.0f, 30.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	auto totalVertexCount =	box.Vertices.size() + grid.Vertices.size() + sphere.Vertices.size() + cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Normal = grid.Vertices[i].Normal;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Normal = sphere.Vertices[i].Normal;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Normal = cylinder.Vertices[i].Normal;
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(),
		COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(),
		COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

void LitColumn::BuildSkullGeometry(void)
{
	std::ifstream fin("../Models/skull.txt");

	if (!fin)
	{
		MessageBox(0, L"../Models/skull.txt not found.", 0, 0);
		return;
	}

	UINT vcount = 0, tcount = 0;
	std::string ignore;

	fin >> ignore >> vcount;
	fin >> ignore >> tcount;
	fin >> ignore >> ignore >> ignore >> ignore;

	std::vector<Vertex> vertices(vcount);
	for (UINT i = 0; i < vcount; ++i)
	{
		fin >> vertices[i].Pos.x >> vertices[i].Pos.y >> vertices[i].Pos.z;
		fin >> vertices[i].Normal.x >> vertices[i].Normal.y >> vertices[i].Normal.z;
	}

	fin >> ignore;
	fin >> ignore;
	fin >> ignore;

	std::vector<std::int32_t> indices(3 * tcount);
	for (UINT i = 0; i < tcount; ++i)
	{
		fin >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];
	}

	fin.close();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::int32_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "skullGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), COM_LIST.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);
	geo->IndexBufferGPU = D3DUTIL.CreateDefaultBuffer(GRAPHIC->Get_Device().Get(), COM_LIST.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	geo->DrawArgs["skull"] = submesh;

	UTIL.Get_Geomesh()[geo->Name] = std::move(geo);
}

void LitColumn::BuildMaterials(void)
{
	auto brick0 = std::make_unique<Material>();
	brick0->Name = "bricks0";
	brick0->MatCBIndex = 0;
	brick0->DiffuseSrvHeapIndex = 0;
	brick0->DiffuseAlbedo = DirectX::XMFLOAT4(Colors::ForestGreen);
	brick0->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	brick0->Roughness = 0.1f;

	auto stone0 = std::make_unique<Material>();
	stone0->Name = "stone0";
	stone0->MatCBIndex = 1;
	stone0->DiffuseSrvHeapIndex = 1;
	stone0->DiffuseAlbedo = DirectX::XMFLOAT4(Colors::LightSteelBlue);
	stone0->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	stone0->Roughness = 0.2f;

	auto tile0 = std::make_unique<Material>();
	tile0->Name = "tile0";
	tile0->MatCBIndex = 2;
	tile0->DiffuseSrvHeapIndex = 2;
	tile0->DiffuseAlbedo = DirectX::XMFLOAT4(Colors::LightGray);
	tile0->FresnelR0 = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);
	tile0->Roughness = 0.2f;

	auto skullMat = std::make_unique<Material>();
	skullMat->Name = "skullMat";
	skullMat->MatCBIndex = 3;
	skullMat->DiffuseSrvHeapIndex = 3;
	skullMat->DiffuseAlbedo = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	skullMat->FresnelR0 = DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f);
	skullMat->Roughness = 0.3f;

	UTIL.Get_Materials()["bricks0"] = std::move(brick0);
	UTIL.Get_Materials()["stone0"] = std::move(stone0);
	UTIL.Get_Materials()["tile0"] = std::move(tile0);
	UTIL.Get_Materials()["skullMat"] = std::move(skullMat);
}

void LitColumn::BuildRenderItem(void)
{
	auto boxRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(2.0f, 2.0f, 2.0f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	XMStoreFloat4x4(&boxRitem->TexTransform, XMMatrixScaling(1.0f, 1.0f, 1.0f));
	boxRitem->objCBIndex = 0;
	boxRitem->Mat = UTIL.Get_Materials()["stone0"].get();
	boxRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	UTIL.Get_Ritemvec().push_back(std::move(boxRitem));

	auto gridRitem = std::make_unique<RenderItem>();
	gridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&gridRitem->TexTransform, XMMatrixScaling(8.0f, 8.0f, 1.0f));
	gridRitem->objCBIndex = 1;
	gridRitem->Mat = UTIL.Get_Materials()["tile0"].get();
	gridRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	UTIL.Get_Ritemvec().push_back(std::move(gridRitem));

	auto skullRitem = std::make_unique<RenderItem>();
	XMStoreFloat4x4(&skullRitem->World, XMMatrixScaling(0.5f, 0.5f, 0.5f)*XMMatrixTranslation(0.0f, 1.0f, 0.0f));
	skullRitem->TexTransform = MathHelper::Identity4x4();
	skullRitem->objCBIndex = 2;
	skullRitem->Mat = UTIL.Get_Materials()["skullMat"].get();
	skullRitem->Geo = UTIL.Get_Geomesh()["skullGeo"].get();
	skullRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	skullRitem->IndexCount = skullRitem->Geo->DrawArgs["skull"].IndexCount;
	skullRitem->StartIndexLocation = skullRitem->Geo->DrawArgs["skull"].StartIndexLocation;
	skullRitem->BaseVertexLocation = skullRitem->Geo->DrawArgs["skull"].BaseVertexLocation;
	UTIL.Get_Ritemvec().push_back(std::move(skullRitem));

	DirectX::XMMATRIX brickTexTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	UINT objCBIndex = 3;
	for (int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		DirectX::XMMATRIX leftCylWorld = DirectX::XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
		DirectX::XMMATRIX rightCylWorld = DirectX::XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

		DirectX::XMMATRIX leftSphereWorld = DirectX::XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
		DirectX::XMMATRIX rightSphereWorld = DirectX::XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		XMStoreFloat4x4(&leftCylRitem->TexTransform, brickTexTransform);
		leftCylRitem->objCBIndex = objCBIndex++;
		leftCylRitem->Mat = UTIL.Get_Materials()["bricks0"].get();
		leftCylRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		XMStoreFloat4x4(&rightCylRitem->TexTransform, brickTexTransform);
		rightCylRitem->objCBIndex = objCBIndex++;
		rightCylRitem->Mat = UTIL.Get_Materials()["bricks0"].get();
		rightCylRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->TexTransform = MathHelper::Identity4x4();
		leftSphereRitem->objCBIndex = objCBIndex++;
		leftSphereRitem->Mat = UTIL.Get_Materials()["stone0"].get();
		leftSphereRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->TexTransform = MathHelper::Identity4x4();
		rightSphereRitem->objCBIndex = objCBIndex++;
		rightSphereRitem->Mat = UTIL.Get_Materials()["stone0"].get();
		rightSphereRitem->Geo = UTIL.Get_Geomesh()["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		UTIL.Get_Ritemvec().push_back(std::move(leftCylRitem));
		UTIL.Get_Ritemvec().push_back(std::move(rightCylRitem));
		UTIL.Get_Ritemvec().push_back(std::move(leftSphereRitem));
		UTIL.Get_Ritemvec().push_back(std::move(rightSphereRitem));
	}

	for (auto& e : UTIL.Get_Ritemvec())
		UTIL.Get_Drawlayer((int)DrawLayer::DL_OPAUQE).push_back(e.get());
}

std::shared_ptr<LitColumn> LitColumn::Create(std::string _name, Object::COM_TYPE _type)
{
	LIT pLit = std::make_shared<LitColumn>();
	if (!pLit)	return nullptr;

	pLit->Get_Objname() = _name;
	pLit->Get_Comtype() = _type;

	return pLit;
}
