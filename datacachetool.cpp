#include <map>
#include <iostream>
#include <vector>
#include <list>
using namespace std;
struct ST_DATA{
	string key;
	int num;
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
条件哑元结构
*/
class CMyDummy
{ 
};

//数据缓存类 
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
	    //数据总量 
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
	    
  		int Load(CDataItem data){
  		  m_clItems.push_back(data);
		  m_iItemNum=m_clItems.size();	
		}
		
  		int Append(CDataItem &p_refclDataItem)
  		{ 
    		int iRetCode = -1;
     	 	m_clItems.push_back(p_refclDataItem);
      		typename list<CIndexEvent<CDataItem>* > ::iterator it;
      		for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); it++)
      		{
        		CIndexEvent<CDataItem>* pIndexEvent = *it;
        		pIndexEvent->Add(p_refclDataItem, m_iItemNum);
      		}
      		iRetCode = m_iItemNum;  
      		m_iItemNum++;
    		return iRetCode;
  		}
		void Free()
		{ 
			m_clItems.clear();
			m_iItemNum = 0;
		}
		
		void RegisterNotify(CIndexEvent<CDataItem> * p_pclIndexEvent)
  		{ 
   			if(p_pclIndexEvent != 0)
    		{
     			m_lstIndexEvent.push_back(p_pclIndexEvent);
    		}
  		}
  	
	  	void UnRegisterNotify(CIndexEvent<CDataItem> * p_pclIndexEvent)
  		{
    		if(p_pclIndexEvent != 0)
    		{ 
     			m_lstIndexEvent.remove(p_pclIndexEvent);
    		} 
  		}
  		
  		void Update(int p_iIndex, CDataItem & p_refclDataItem)
  		{ 
    		bool bCanUpdate = true;
    		typename list<CIndexEvent<CDataItem>* > ::iterator it;
    		for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); it++)
    		{ 
      			CIndexEvent<CDataItem>* pIndexEvent = *it;
      			bCanUpdate = pIndexEvent->CanUpdate(m_clItems[p_iIndex], p_refclDataItem);
    		} 
    		m_clItems[p_iIndex] = p_refclDataItem;  
  		}
  		
  		const CDataItem & operator[](int p_iIndex)
  		{ 
    		return m_clItems[p_iIndex];
  		}
  
	protected:
		vector<CDataItem> m_clItems;
		list<CIndexEvent<CDataItem>* > m_lstIndexEvent;//多索引支持 
		int m_iItemNum;
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
  			m_clUniqueIndexEvent.SetParent(this);
  			m_refclDataCache.RegisterNotify(&m_clUniqueIndexEvent);
  		}
  		~CDataCacheUniqueIndex()
  		{
  			m_refclDataCache.UnRegisterNotify(&m_clUniqueIndexEvent);
  		}
  		
  		//创建索引 
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
        			//违反唯一性索引
      			}
      			else
				{
					m_mapUniqueIndexes[strIndexStr] = iIndex; 
				} 
      			
    		}
  		}
  		
		//获取索引指向位置  
  		int GetDataItemPos(CDataIndex &p_refclDataIndex)
  		{ 
    		int iItemPos = -1;
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;
    		it = m_mapUniqueIndexes.find(strIndexStr);
    		if(it == m_mapUniqueIndexes.end())
    		{  
      			iItemPos = -1;//不存在 
    		}
    		else
    		{
    			iItemPos = it->second;	
			}
    		return iItemPos;
  		}
  		
  		//获取索引指向条目 
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
    			//不存在
    		}
    		return m_refclDataCache[it->second];
  		}
		void UpdateIndex(CDataItem & p_refclDataItem, int iDataItemPos)
  		{ 
    		string strIndexStr = SearchKey(p_refclDataItem);
    		m_mapUniqueIndexes[strIndexStr] = iDataItemPos; 
  		}
  		  
		void SetIndexName(const char * p_szIndexName)
  		{  
    		m_szIndexName = p_szIndexName;
  		}
  		string SearchKey(const CDataIndex & p_refclDataIndex);
  		string SearchKey(const CDataItem & p_refclDataItem);
	private:
		/*Inner class*/
		class CUniqueIndexEvent: public CIndexEvent<CDataItem>
		{ 
			public:
    			CUniqueIndexEvent()
    			{
    			
    			}
    			void SetParent(CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * p_pclIndex)
    			{ 
     		 		m_pclIndex = p_pclIndex;
    			}
    			virtual int Add(CDataItem &newValue, int iPos)
    			{ 
     	 			m_pclIndex->UpdateIndex(newValue, iPos);
      				return 0;
    			}
    			virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue)
                { 
      				string szOldKey = m_pclIndex->SearchKey(oldValue);
      				string szNewKey = m_pclIndex->SearchKey(newValue);
      				return (szOldKey == szNewKey);
    			} 
    			virtual bool CanAdd()
				{
					return true;
				}
  				virtual int Clear()
				{
					return -1;
				} 
  			private:
    			CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * m_pclIndex;
  		};
	private:
  		CDataCache<CDataItem, CQueryCondtion> & m_refclDataCache;
  		map<string, int> m_mapUniqueIndexes;
  		CUniqueIndexEvent m_clUniqueIndexEvent;
  		string m_szIndexName;
};

template<>
inline string CDataCacheUniqueIndex<string,ST_DATA>::SearchKey(const string & p_refclDataIndex)
{
  return p_refclDataIndex;
}

template<>
inline string CDataCacheUniqueIndex<string,ST_DATA>::SearchKey(const ST_DATA & p_refclDataItem)
{ 
  return p_refclDataItem.key;
}

int main()
{
	CDataCache<ST_DATA>clDataCache;
	CDataCacheUniqueIndex<string,ST_DATA> clDataCacheUniqueIndex(clDataCache);
	ST_DATA data;
	data.num=1;
	data.key="data";
	clDataCache.Append(data);
	clDataCache.Append(data);
	cout<<clDataCacheUniqueIndex["data"].num<<endl;
	cout<<clDataCache[0].num<<endl;
	return 0;
}


