#if !defined __data_cache_h__
#define __data_cache_h__

#include <vector>
/*
Data Index Change Event, not support delete for avoid Halloween question
*/
template <typename CDataItem>
class CIndexEvent 
{ 
public:
  virtual bool CanAdd() = 0;                                               /*�Ƿ���������*/
  virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*�����¼�*/
  virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*�Ƿ��������*/
  virtual int Clear() = 0;                                                 /*�����¼�*/
};





template<typename CDataItem>
class CDataCache
{ 
public:
	CDataCache(): m_iItemNum(0)
  	{
	}
	
	virtual ~CDataCache()
  	{
		Free();
	}
	
	int Count()
	{
		return m_iItemNum;
	}
	
  	void Clear()
  	{
    	Free();
  	}
	    
  	int Load(void);
  	
	int Load(const char* p_pszSql);
protected:
	void Free()
	{ 
		m_clItems.clear();
		m_iItemNum = 0;
	}
protected:
	vector<CDataItem> m_clItems;
	int m_iItemNum;
};

#endif

