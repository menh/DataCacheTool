#include <map>
#include <iostream>
#include <vector>
using namespace std;

/*
条件哑元结构
*/
class CMyDummy
{ 
};
template<typename CDataItem,typename CLoadCondtion = CMyDummy>
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
	
		const CDataItem & operator[](int p_iIndex) const
    	{ 
    		if (p_iIndex <0 || p_iIndex >= m_iItemNum)
    		{
      			
    		}
    		return m_clItems[p_iIndex];
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



/*
Unique Index模板类定义
*/
template<typename CDataIndex, typename CDataItem, typename CQueryCondtion = CMyDummy>
class CDataCacheUniqueIndex
{ 
	public:  
  /*class self*/
  		CDataCacheUniqueIndex( CDataCache<CDataItem , CQueryCondtion> &p_refclDataCache): m_refclDataCache(p_refclDataCache)
  		{
  			
  		}
  		~CDataCacheUniqueIndex()
  		{ 
  		}
  		
		void CreateIndex(void)
		{
    		int iIndex;
			int iCount = m_refclDataCache.Count();
    		m_mapUniqueIndexes.clear();
    		string strIndexStr;
    		for (iIndex = 0; iIndex < iCount; iIndex++)
    		{ 
				strIndexStr = SearchKey(m_refclDataCache[iIndex]);
      			map<string, int>::iterator it;
				it = m_mapUniqueIndexes.find(strIndexStr);
      			if (it != m_mapUniqueIndexes.end())
      			{
        			
      			}
      			m_mapUniqueIndexes[strIndexStr] = iIndex; 
    		}
  		}
  		  
		int GetDataItem(CDataItem &p_refclDataItem, CDataIndex &p_refclDataIndex)
  		{ 
    		int iRetCode = 0;
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;
    		it = m_mapUniqueIndexes.find(strIndexStr);
    		if (it == m_mapUniqueIndexes.end())
    		{  

    		}
    		p_refclDataItem = m_refclDataCache[it->second];
    		return iRetCode;
  		}
  		
  		const CDataItem & operator[](const CDataIndex &p_refclDataIndex)
  		{	
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;
    
    		it = m_mapUniqueIndexes.find(strIndexStr);
    		if(it == m_mapUniqueIndexes.end())
    		{  
    		} 
    		return m_refclDataCache[it->second];
  }

private:
	string SearchKey(const CDataIndex & p_refclDataIndex);
	/*Inner class*/
	class CUniqueIndexEvent: public CIndexEvent<CDataItem>
	{ 
		public:
    		CUniqueIndexEvent()
    		{
    			
    		}
  		private:
    		CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * m_pclIndex;
  	};
	private:
  		CDataCache<CDataItem, CQueryCondtion> & m_refclDataCache;
  		map<string, int> m_mapUniqueIndexes;
};
int main()
{
	return 0;
}


