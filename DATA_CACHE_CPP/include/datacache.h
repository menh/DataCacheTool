#if !defined __data_cache_h__
#define __data_cache_h__


/*
�����¼��ӿ��� 
*/
template <typename CDataItem>
class CIndexEvent 
{ 
	public:
		virtual bool CanAdd() = 0;                                               /*�Ƿ���������*/
		virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*���������¼�*/
  		virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*�Ƿ��������*/
		virtual int Clear() = 0;                                                 /*���������¼�*/
};


/*
Ψһ����ģ���� 
*/
template<typename CDataIndex, typename CDataItem, typename CQueryCondtion = CMyDummy>
class CDataCacheUniqueIndex
{ 
	private:
		/*Ψһ�����¼�*/ 
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
				strIndexStr = SearchKey(m_refclDataCache[iIndex]);//��ȡ����ֵ 
				
				map<string, int>::iterator it;
				it = m_clUniqueIndexes.find(strIndexStr);
	      		if (it != m_clUniqueIndexes.end())
				{
        			//Υ��Ψһ����
      			}
      			m_clUniqueIndexes[strIndexStr] = iIndex; 
    		} 
		}
	private:
		string SearchKey(const CDataItem & p_refclDataItem); 
	
	private:
		CDataCache<CDataItem, CQueryCondtion> & m_refclDataCache;  /*���ݻ������*/ 
		map<string, int> m_clUniqueIndexes; /*����ֵ�͸�������index*/ 
};

/**
 * ���ݻ����� 
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
