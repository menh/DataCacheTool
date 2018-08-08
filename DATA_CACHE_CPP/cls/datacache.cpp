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
  virtual bool CanAdd() = 0;                                               /*是否允许增加*/
  virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*增加事件*/
  virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*是否允许更新*/
  virtual int Clear() = 0;                                                 /*数据事件*/
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

