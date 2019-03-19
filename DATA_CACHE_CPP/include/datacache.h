#if !defined __data_cache_h__
#define __data_cache_h__


/*
索引事件接口类 
*/
template <typename CDataItem>
class CIndexEvent 
{ 
	public:
		virtual bool CanAdd() = 0;                                               /*是否允许增加*/
		virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*增加数据事件*/
  		virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*是否允许更新*/
		virtual int Clear() = 0;                                                 /*数据清理事件*/
};


/*
唯一索引模板类 
*/
template<typename CDataIndex, typename CDataItem, typename CQueryCondtion = CMyDummy>
class CDataCacheUniqueIndex
{ 
	private:
		/*唯一索引事件*/ 
		/*class CUniqueIndexEvent: public CIndexEvent<CDataItem>
		{ 
			public:
				virtual bool CanAdd()
				{ 
					return true;
				} 
				
				virtual int Add(CDataItem &newValue, int iPos)
				{ 
					return 0;
				}
    
				virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue)
				{ 
					return true;
				} 
    
				virtual int Clear()
    			{
					return -1;    
    			}
  
  				void SetParent(CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * p_pclIndex)
    			{ 
					m_pclIndex = p_pclIndex;
    			}
  			private:
				CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * m_pclIndex;
		};*/ 
	public:  
		CDataCacheUniqueIndex(CDataCache<CDataItem, CQueryCondtion> &p_refclDataCache): m_refclDataCache(p_refclDataCache)
		{ 
		}
  
		~CDataCacheUniqueIndex()
		{ 
			m_refclDataCache.UnRegisterNotify(&m_clUniqueIndexEvent);
		} 

		void CreateIndex()
		{
			int iIndex;
			int iCount = m_refclDataCache.Count();
			m_clUniqueIndexes.clear();
			string strIndexStr;
    
			for (iIndex = 0; iIndex < iCount; iIndex++)
			{ 
				strIndexStr = SearchKey(m_refclDataCache[iIndex]);//获取索引值 
				
				map<string, int>::iterator it;
				it = m_clUniqueIndexes.find(strIndexStr);
	      		if (it != m_clUniqueIndexes.end())
				{
        			//违反唯一索引
      			}
      			m_clUniqueIndexes[strIndexStr] = iIndex; 
    		} 
		}
	private:
		string SearchKey(const CDataItem & p_refclDataItem); 
	
	private:
		CDataCache<CDataItem, CQueryCondtion> & m_refclDataCache;  /*数据缓存变量*/ 
		map<string, int> m_clUniqueIndexes; /*索引值和该项所在index*/ 
};

/**
 * 数据缓存类 
 *
 *
 *
 *
 *
 *
 */ 
template<typename CDataItem, typename CLoadCondtion = CMyDummy>
class CDataCache
{ 
	public:
		CDataCache(): m_iItemNum(0)
  		{
  			m_clItems.clear(); 
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
