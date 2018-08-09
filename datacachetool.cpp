#include <map>
#include <iostream>
#include <vector>
#include <list>
using namespace std;

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

/*
������Ԫ�ṹ
*/
class CMyDummy
{ 
};

//���ݻ����� 
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
	    //�������� 
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
	protected:
		vector<CDataItem> m_clItems;
		list<CIndexEvent<CDataItem>* > m_lstIndexEvent;//������֧�� 
		int m_iItemNum;
};






/*
Unique Indexģ���ඨ��
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
  		
  		//�������� 
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
        			//Υ��Ψһ������
      			}
      			else
				{
					m_mapUniqueIndexes[strIndexStr] = iIndex; 
				} 
      			
    		}
  		}
  		
		//��ȡ����ָ��λ��  
  		int GetDataItemPos(CDataIndex &p_refclDataIndex)
  		{ 
    		int iItemPos = -1;
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;
    		it = m_mapUniqueIndexes.find(strIndexStr);
    		if(it == m_mapUniqueIndexes.end())
    		{  
      			iItemPos = -1;//������ 
    		}
    		else
    		{
    			iItemPos = it->second;	
			}
    		return iItemPos;
  		}
  		
  		//��ȡ����ָ����Ŀ 
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
    			//������
    		}
    		return m_refclDataCache[it->second];
  		}
		void UpdateIndex(CDataItem & p_refclDataItem, int iDataItemPos)
  		{ 
    		string strIndexStr = SearchKey(p_refclDataItem);
    		m_mapUniqueIndexes[strIndexStr] = iDataItemPos; 
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
    			void SetParent(CDataCacheUniqueIndex<CDataIndex, CDataItem, CQueryCondtion> * p_pclIndex)
    			{ 
     		 		m_pclIndex = p_pclIndex;
    			}
    			virtual int Add(CDataItem &newValue, int iPos)
    			{ 
     	 			m_pclIndex->UpdateIndex(newValue, iPos);
      				return 0;
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
int main()
{
	return 0;
}


