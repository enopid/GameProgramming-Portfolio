#pragma once
#include "pch.h"

// BG
// BGTiLe
// Decal
// 
// Tile - 기본적으로 타일셋
// TileBlock
// Block
// Object
//
// Trigger

//기본적으로 0일시 빈칸

//기본적으로 사용하는 decal의 위치 저장
struct FDecalInfo
{
	pos<int>	MapPos = { 0,0 };			//맵내 타일셋 내 위치 인덱스
};

//기본적으로 사용하는 타일셋 정보와, 타일셋 내 위치 저장
struct FTileInfo
{
	int			Type = 0;			//어떤 타일셋인지
	pos<int>	TileMapPos = { 0,0 };		//맵내 타일셋 내 위치 인덱스
	pos<int>	TileSetPos = { 0,0 };		//맵내 타일 위치 인덱스
};

//기본적으로 사용하는 타일 블록 정보와, 타일셋 내 위치 저장
struct FTileBlockInfo
{
	int			Type = 0;					//어떤 타일인지
	pos<int>	TileMapPos = { 0,0 };		//맵내 타일 위치 인덱스
	int			State = 0;					//블록의 상태 메타 정보(방향)
};

//기본적으로 사용하는 블록 정보와, 맵 내 위치 저장
struct FBlockInfo
{
	int			Type = 0;			//어떤 타일셋인지
	pos<int>	MapPos = { 0,0 };			//맵내 타일셋 내 위치 인덱스
	pos<int>	BlockSize = { 0,0 };		//맵내 타일셋 내 위치 인덱스
	int			State = 0;			//블록의 상태 메타 정보(기믹)
};

//오브젝트의 초기 상태 및 맵 내 위치 저장
struct FObjectInfo
{
	int			Type = 0;			//어떤 오브젝트인지
	pos<int>	MapPos = { 0,0 };	//맵내 타일셋 내 위치 인덱스
	int			State = 0;			//오브젝트의 상태 메타 정보
};


//트리거 정보
struct FTriggerInfo
{
	int			Type = 0;			//어떤 트리거인지
	pos<int>	TileMapPos = {0,0};		//맵내 타일맵 내 위치 인덱스
};

struct FStage
{
	void Init(pos<int> _MapSize);
	void LoadStage(const wchar_t* _fileName);
	void SaveStage(const wchar_t* _fileName, const list<CObject*> const* _pObjList);

	FTileBlockInfo CreateTileBlockInfo(CObject* _pObject);

	FObjectInfo CreateObjectInfo(CObject* _pObject);

	FTriggerInfo CreateTriggerInfo(CObject* _pObject);

//BG info
	int BGTileCount;
	vector<FTileInfo>		vecBGTiles;

	int DecalCount;
	vector<FDecalInfo>		vecDecals;

//Map info
	int TileCount;
	vector<FTileInfo>		vecTiles;

	int TileBlockCount;
	vector<FTileBlockInfo>	vecTileBlocks;

	int BlockCount;
	vector<FBlockInfo>		vecBlocks;
	
	int ObjectCount;
	vector<FObjectInfo>		vecObjects;

//system Info
	int TriggerCount;
	vector<FTriggerInfo>	vecTriggers;
	
	pos<int>	RespawnPos;		//맵에서의 리스폰 위치
	pos<int>	MapSize;		//타일 개수
	//int			m_iSCe;
};

