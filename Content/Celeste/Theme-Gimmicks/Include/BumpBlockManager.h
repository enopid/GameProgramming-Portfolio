#pragma once

class CBumpBlock;

class CBumpBlockManager
{
	CBumpBlockManager(const CBumpBlockManager& rhs)		= delete;
	CBumpBlockManager& operator=(CBumpBlockManager& rObj) = delete;

public:
	void	Initialize();
	void	Update(double _dDeltaTime);
	void	LateUpdate(double _dDeltaTime);

	void	SetStage(int _stage);
	void	Reset();

public:
	static CBumpBlockManager& Instance() {
		static CBumpBlockManager instance;
		return instance;
	}
private:
	void	Release();

	CBumpBlockManager() {};
	~CBumpBlockManager() { Release(); };

	vector<vector<RECT>>			m_vecRects;
	vector<vector<int>>				m_vecTypes;

	int								m_iStage;
};

