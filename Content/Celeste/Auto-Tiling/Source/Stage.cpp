#include "pch.h"

#include "CTileSetTile.h"

#include "Spike.h"
#include "Wall.h"
#include "JumpPad.h"
#include "CrystalSpike.h"
#include "Bumper.h"
#include "SemiBlock.h"

#include "DashCrystal.h"
#include "FlipSwitch.h"
#include "Feather.h"

#include "RespawnTrigger.h"

void FStage::Init(pos<int> _MapSize)
{
	MapSize = _MapSize;

	TileBlockCount = TileCount = BGTileCount = DecalCount = TriggerCount = ObjectCount = BlockCount = 0;

	vecBGTiles.clear();
	vecTiles.clear();
	vecTileBlocks.clear();
	vecDecals.clear();
	vecBlocks.clear();
	vecObjects.clear();
	vecTriggers.clear();
}

void FStage::LoadStage(const wchar_t* _fileName)
{

	HANDLE		hFile = CreateFile(_fileName,			// 파일 이름을 포함한 경로
		GENERIC_READ,											// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
		NULL,													// 공유 방식(파일이 열려 있는 상태에서 다른 프로그램에서 오픈하는 것을 허가할 것인가)
		NULL,													// 보안 속성(기본 값)
		OPEN_EXISTING,											// 생성 방식(파일이 없으면 생성, 있으면 덮어 쓰기) // OPEN_EXISTING(파일이 있을 경우에만 연다)
		FILE_ATTRIBUTE_NORMAL,									// 파일 속성(아무런 속성이 없는 일반 파일)
		NULL);													// 생성될 파일의 속성을 제공할 템플릿(사용하지 않기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Load Map File", _T("Fail"), MB_OK);
		return;
	}

	DWORD	dwByte(0);

	//ReadMapSize
	ReadFile(hFile, &MapSize, sizeof(pos<int>), &dwByte, NULL);
	//ReadRespawnPos
	ReadFile(hFile, &RespawnPos, sizeof(pos<int>), &dwByte, NULL);

	Init(MapSize);


	//ReadBGTile
	ReadFile(hFile, &BGTileCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < BGTileCount; i++)
	{
		FTileInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FTileInfo), &dwByte, NULL);
		vecBGTiles.push_back(_FInfo);
	}


	//ReadDecal
	ReadFile(hFile, &DecalCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < DecalCount; i++)
	{
		FDecalInfo _FTileInfo;
		ReadFile(hFile, &_FTileInfo, sizeof(FTileInfo), &dwByte, NULL);
		vecDecals.push_back(_FTileInfo);
	}

	//ReadTile
	ReadFile(hFile, &TileCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < TileCount; i++)
	{
		FTileInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FTileInfo), &dwByte, NULL);
		vecTiles.push_back(_FInfo);
	}

	//ReadTileBlock
	ReadFile(hFile, &TileBlockCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < TileBlockCount; i++)
	{
		FTileBlockInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FTileBlockInfo), &dwByte, NULL);
		vecTileBlocks.push_back(_FInfo);
	}

	//ReadBlock
	ReadFile(hFile, &BlockCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < BlockCount; i++)
	{
		FBlockInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FBlockInfo), &dwByte, NULL);
		vecBlocks.push_back(_FInfo);
	}

	//ReadObject
	ReadFile(hFile, &ObjectCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < ObjectCount; i++)
	{
		FObjectInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FObjectInfo), &dwByte, NULL);
		vecObjects.push_back(_FInfo);
	}

	//ReadTriggeer
	ReadFile(hFile, &TriggerCount, sizeof(int), &dwByte, NULL);
	for (int i = 0; i < TriggerCount; i++)
	{
		FTriggerInfo _FInfo;
		ReadFile(hFile, &_FInfo, sizeof(FTriggerInfo), &dwByte, NULL);
		vecTriggers.push_back(_FInfo);
	}

	CloseHandle(hFile);
}

void FStage::SaveStage(const wchar_t* _fileName, const list<CObject*> const* _pObjList)
{
	HANDLE		hFile = CreateFile(_fileName,			// 파일 이름을 포함한 경로
		GENERIC_WRITE,											// 파일 접근 모드(GENERIC_WRITE : 쓰기 전용, GENERIC_READ : 읽기 전용)
		NULL,													// 공유 방식(파일이 열려 있는 상태에서 다른 프로그램에서 오픈하는 것을 허가할 것인가)
		NULL,													// 보안 속성(기본 값)
		CREATE_ALWAYS,											// 생성 방식(파일이 없으면 생성, 있으면 덮어 쓰기) // OPEN_EXISTING(파일이 있을 경우에만 연다)
		FILE_ATTRIBUTE_NORMAL,									// 파일 속성(아무런 속성이 없는 일반 파일)
		NULL);													// 생성될 파일의 속성을 제공할 템플릿(사용하지 않기 때문에 NULL)

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, L"Save Map File", _T("Fail"), MB_OK);
		return;
	}

	DWORD	dwByte(0);

	//WriteMapSize
	WriteFile(hFile, &MapSize, sizeof(pos<int>), &dwByte, NULL);
	//WriteRespawnPos]
	WriteFile(hFile, &RespawnPos, sizeof(pos<int>), &dwByte, NULL);

	//WriteBGTile
	int _iBGTileCnt = _pObjList[BGTILE].size();
	WriteFile(hFile, &_iBGTileCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[BGTILE])
	{
		auto _pTileSetTile = dynamic_cast<CTileSetTile*>(_pObject);
		FTileInfo _FTileInfo;
		_FTileInfo.Type			= _pTileSetTile->GetTileType();
		_FTileInfo.TileMapPos	= _pTileSetTile->GetTileIdx();
		_FTileInfo.TileSetPos	= _pTileSetTile->GetTileSetIdx();

		WriteFile(hFile, &_FTileInfo, sizeof(FTileInfo), &dwByte, NULL);
	}

	//WriteDecal
	int _iDecalCnt = _pObjList[DECAL].size();
	WriteFile(hFile, &_iDecalCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[DECAL])
	{
		//auto _pTileSetTile = dynamic_cast<CTileSetTile*>(_pObject);
		FDecalInfo _FInfo;

		WriteFile(hFile, &_FInfo, sizeof(FDecalInfo), &dwByte, NULL);
	}

	//WriteTile
	int _iTileCnt = _pObjList[TILE].size();
	WriteFile(hFile, &_iTileCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[TILE])
	{
		auto _pTileSetTile = dynamic_cast<CTileSetTile*>(_pObject);
		FTileInfo _FTileInfo;
		_FTileInfo.Type			= _pTileSetTile->GetTileType();
		_FTileInfo.TileMapPos	= _pTileSetTile->GetTileIdx();
		_FTileInfo.TileSetPos	= _pTileSetTile->GetTileSetIdx();

		WriteFile(hFile, &_FTileInfo, sizeof(FTileInfo), &dwByte, NULL);
	}

	//WriteTileBlock
	int _iTileBlockCnt = _pObjList[TILEBLOCK].size();
	WriteFile(hFile, &_iTileBlockCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[TILEBLOCK])
	{
		auto _FInfo = CreateTileBlockInfo(_pObject);
		WriteFile(hFile, &_FInfo, sizeof(FTileBlockInfo), &dwByte, NULL);
	}

	//WriteBlock
	int _iBlockCnt = _pObjList[BLOCK].size();
	WriteFile(hFile, &_iBlockCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[BLOCK])
	{
		//auto _pTileSetTile = dynamic_cast<CTileSetTile*>(_pObject);
		FBlockInfo _FInfo;

		WriteFile(hFile, &_FInfo, sizeof(FBlockInfo), &dwByte, NULL);
	}

	//WriteObject
	int _iObjectCnt = _pObjList[OBJECT].size();
	WriteFile(hFile, &_iObjectCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[OBJECT])
	{
		auto _FInfo = CreateObjectInfo(_pObject);
		WriteFile(hFile, &_FInfo, sizeof(FObjectInfo), &dwByte, NULL);
	}

	//WriteObject
	int _iTriggerCnt = _pObjList[TRIGGER].size();
	WriteFile(hFile, &_iTriggerCnt, sizeof(int), &dwByte, NULL);
	for (auto& _pObject : _pObjList[TRIGGER])
	{
		//auto _pTileSetTile = dynamic_cast<CTileSetTile*>(_pObject);
		auto _FInfo = CreateTriggerInfo(_pObject);
		WriteFile(hFile, &_FInfo, sizeof(FTriggerInfo), &dwByte, NULL);
	}

	CloseHandle(hFile);
}

FTileBlockInfo FStage::CreateTileBlockInfo(CObject* _pObject)
{
	FTileBlockInfo _FInfo;
	if (auto pObject = dynamic_cast<CSpike*>(_pObject); pObject) {
		_FInfo.Type			= 0;
		_FInfo.TileMapPos	= pObject->GetTileIdx();
		_FInfo.State		= pObject->GetDirection();
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CWall*>(_pObject); pObject) {
		_FInfo.Type			= 1;
		_FInfo.TileMapPos	= pObject->GetTileIdx();
		_FInfo.State		= pObject->GetState();
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CJumpPad*>(_pObject); pObject) {
		_FInfo.Type = 2;
		_FInfo.TileMapPos	= pObject->GetTileIdx();
		_FInfo.State		= pObject->GetDirection();
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CCrystalSpike*>(_pObject); pObject) {
		_FInfo.Type = 3;
		_FInfo.TileMapPos = pObject->GetTileIdx();
		_FInfo.State	  = pObject->GetState();
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CBumper*>(_pObject); pObject) {
		_FInfo.Type = 4;
		_FInfo.TileMapPos	= pObject->GetTileIdx();
		_FInfo.State		= 0;
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CSemiBlock*>(_pObject); pObject) {
		_FInfo.Type = 5;
		_FInfo.TileMapPos = pObject->GetTileIdx();
		_FInfo.State = pObject->GetState();
		return _FInfo;
	}

	return _FInfo;
}

FObjectInfo FStage::CreateObjectInfo(CObject* _pObject)
{
	FObjectInfo _FInfo;
	if (auto pObject = dynamic_cast<CDashCrystal*>(_pObject); pObject) {
		_FInfo.Type = 0;
		_FInfo.MapPos = pObject->GetTileIdx();
		_FInfo.State = 0;
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CFlipSwitch*>(_pObject); pObject) {
		_FInfo.Type = 1;
		_FInfo.MapPos = pObject->GetTileIdx();
		_FInfo.State = pObject->GetState();
		return _FInfo;
	}
	else if (auto pObject = dynamic_cast<CFeather*>(_pObject); pObject) {
		_FInfo.Type = 2;
		_FInfo.MapPos = pObject->GetTileIdx();
		_FInfo.State = 0;
		return _FInfo;
	}

	return _FInfo;
}

FTriggerInfo FStage::CreateTriggerInfo(CObject* _pObject)
{
	FTriggerInfo _FInfo;
	if (auto pObject = dynamic_cast<CRespawnTrigger*>(_pObject); pObject) {
		_FInfo.Type = 1;
		_FInfo.TileMapPos = pObject->GetTileIdx();
		return _FInfo;
	}

	return _FInfo;
}
