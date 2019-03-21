/*#if !defined __data_cache_h__
#define __data_cache_h__
*/ 

//���ķ���,������ϵ 
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <list>
using namespace std;
/*
�����¼��ӿ��� 
*/

struct student{
	string name;
	int age;
	student(){
		this->name = "";
		this->age = 0;
	}
	student(string name,int age){
		this->name = name;
		this->age = age;
	}
};

struct studentpk{
	string name;
	studentpk(string name){
		this->name = name;
	}
};

template <typename CDataItem>
class CIndexEvent 
{ 
	public:
		virtual bool CanAdd(CDataItem &p_refclDataItem) = 0;                                               /*�Ƿ���������*/
		virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*���������¼�*/
  		virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*�Ƿ��������*/
  		virtual int Update(CDataItem &oldValue, CDataItem &newValue) = 0;    /*�Ƿ��������*/
		virtual int Clear() = 0;                                                 /*���������¼�*/
};

/**
 * ���ݻ�����
 */ 
template<typename CDataItem>
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
	    
  		int Load(const vector<CDataItem>& p_vecData)
		{
			m_clItems.assign(p_vecData.begin(), p_vecData.end());
			m_iItemNum = m_clItems.size();
		}
		
		const CDataItem & operator[](int p_iIndex) const
		{ 
			if (p_iIndex <0 || p_iIndex >= m_iItemNum)
			{
				//"�����±�Խ��"
			}
    		return m_clItems[p_iIndex];
		}
		
		int Append(CDataItem &p_refclDataItem)
		{
			int iRetCode = -1;
			typename list<CIndexEvent<CDataItem>* > ::iterator it;
			bool bCanAdd = true;
			for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); ++it)
			{
				if((*it)->CanAdd(p_refclDataItem) == -1)
				{
					bCanAdd = false;
					break;
				}
			}
			if(bCanAdd)
			{
				m_clItems.push_back(p_refclDataItem);
				
				for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); ++it)
				{
					(*it)->Add(p_refclDataItem, m_iItemNum);
				}
				++m_iItemNum;
			}	
		}
		
		int Update(int p_iIndex, CDataItem & p_refclDataItem)
		{
			CDataItem oldValue = m_clItems[p_iIndex];
			
			int iRetCode = -1;
			typename list<CIndexEvent<CDataItem>* > ::iterator it;
			bool bCanAdd = true;
			for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); ++it)
			{
				if((*it)->CanUpdate(oldValue, p_refclDataItem) == -1)
				{
					bCanAdd = false;
					break;
				}
			}
			if(bCanAdd)
			{
				for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); ++it)
				{
					(*it)->Update(oldValue,p_refclDataItem); 
				}
				m_clItems[p_iIndex] = p_refclDataItem;
			}
			return 0;
		}
		
		void RegisterNotify(CIndexEvent<CDataItem> * p_pclIndexEvent)
  		{ 
      		m_lstIndexEvent.push_back(p_pclIndexEvent);
  		}
  		  
		void UnRegisterNotify(CIndexEvent<CDataItem> * p_pclIndexEvent)
  		{
      		m_lstIndexEvent.remove(p_pclIndexEvent);
    	} 
protected:
	void Free()
	{ 
		m_clItems.clear();
		m_iItemNum = 0;
		m_lstIndexEvent.clear(); 
	}
private:
	vector<CDataItem> m_clItems;
	int m_iItemNum;
	list<CIndexEvent<CDataItem>* > m_lstIndexEvent;
};

/*
Ψһ����ģ���� 
*/
template<typename CDataItem,typename CDataIndex>
class CDataCacheUniqueIndex
{ 
	private:
		/*Ψһ�����¼�*/ 
		class CUniqueIndexEvent: public CIndexEvent<CDataItem>
		{ 
			public:
				virtual bool CanAdd(CDataItem &p_refclDataItem)
				{
					return m_pclIndex->GetDataItemPos(p_refclDataItem) == -1 ? true : false;
				} 
				
				virtual int Add(CDataItem &newValue, int iPos)
				{ 
					return m_pclIndex->UpdateIndex(newValue, iPos);
				}
    
				virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue)
				{ 
					return m_pclIndex->GetDataItemPos(newValue) == -1 || m_pclIndex->GetDataItemPos(oldValue) ==  m_pclIndex->GetDataItemPos(newValue) ? true : false;
				} 
    
    			virtual int Update(CDataItem &oldValue, CDataItem &newValue)
    			{
    				int iPos = m_pclIndex->GetDataItemPos(oldValue);
    				
    				return m_pclIndex->UpdateIndex(newValue, iPos);
				}
    			
				virtual int Clear()
    			{
					return -1;    
    			}
  
  				void SetParent(CDataCacheUniqueIndex<CDataItem,CDataIndex> * p_pclIndex)
    			{ 
					m_pclIndex = p_pclIndex;
    			}
  			private:
				CDataCacheUniqueIndex<CDataItem,CDataIndex> * m_pclIndex;
		};
	public:  
		CDataCacheUniqueIndex(CDataCache<CDataItem> &p_refclDataCache): m_refclDataCache(p_refclDataCache)
		{
			this->CreateIndex();
			m_clUniqueIndexEvent.SetParent(this);
			m_refclDataCache.RegisterNotify(&m_clUniqueIndexEvent);
		}
  
		~CDataCacheUniqueIndex()
		{ 

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
		
		//����������ѯ��������λ�� 
		int GetDataItemPos(CDataIndex &p_refclDataIndex)
  		{ 
    		int iRetCode = -1;
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;
    		it = m_clUniqueIndexes.find(strIndexStr);
    		if(it == m_clUniqueIndexes.end())
    		{
    		
    		}
    		else
    		{
    			iRetCode = it->second;
			}
    		
    		return iRetCode;
  		}
  		
  				//����������ѯ��������λ�� 
		int GetDataItemPos(CDataItem &p_refclDataItem)
  		{ 
    		int iRetCode = -1;
    		string strIndexStr = SearchKey(p_refclDataItem);
    		map<string, int>::iterator it;
    		it = m_clUniqueIndexes.find(strIndexStr);
    		if(it == m_clUniqueIndexes.end())
    		{
			
    		}
    		else
    		{
    			iRetCode = it->second;
			}
    		return iRetCode;
  		}
  		
  		const CDataItem& operator[] (const CDataIndex& p_refclDataIndex)
  		{
  			string strIndexStr = SearchKey(p_refclDataIndex);
  			map<string,int>:: iterator it = m_clUniqueIndexes.find(strIndexStr);
  			int iPos = it->second;
  			return m_refclDataCache[iPos];
		}
  		
  		//����������ȡ���� 
  		int GetDataItem(CDataItem &p_refclDataItem, const CDataIndex &p_refclDataIndex)
  		{ 
    		int iRetCode;
    		string strIndexStr = SearchKey(p_refclDataIndex);
    		map<string, int>::iterator it;

    		it = m_clUniqueIndexes.find(strIndexStr);
    		if (it == m_clUniqueIndexes.end())
    		{  
      			iRetCode = -1;
      			goto __end;
    		} 
    		p_refclDataItem = m_refclDataCache[it->second];
			__end:
    		return iRetCode;
  		}
	private:
		
		string SearchKey(const CDataItem & p_refclDataItem){
  			return p_refclDataItem.name;
		}
		string SearchKey(const CDataIndex & p_refclDataIndex){
  			return p_refclDataIndex.name;
		}
		
		int UpdateIndex(CDataItem & p_refclDataItem, int iDataItemPos)
  		{
    		string strIndexStr = SearchKey(p_refclDataItem);
    		m_clUniqueIndexes[strIndexStr] = iDataItemPos;
			return 0; 
  		}
	private:
		CDataCache<CDataItem> & m_refclDataCache;  /*���ݻ������*/ 
		map<string, int> m_clUniqueIndexes; /*����ֵ�͸�������index*/
		CUniqueIndexEvent m_clUniqueIndexEvent;
};

/* 
#endif
*/ 
int main()
{
	CDataCache<student> data;
	vector<student>data2;
	
	student student1("С1",1);
	student student2("С2",2);
	student student3("С3",3);
	student student4("С4",4);
	student student5("С5",5);
	student student6("С6",6);
	student student7("С7",7);
	student student8("С8",8);
	
	data2.push_back(student1);
	data2.push_back(student2);
	data2.push_back(student3);
	data2.push_back(student4);
//	data2.push_back(student5);
	
	data.Load(data2);
	
	CDataCacheUniqueIndex<student,studentpk> studentNameUniqueIndex(data);
	
	data.Append(student5);
	data.Append(student6);
	studentpk studentpk1("С8");
	data.Update(2,student8);
	cout<<studentNameUniqueIndex.GetDataItemPos(studentpk1)<<endl;
	
	/*student student6;
	studentNameUniqueIndex.GetDataItem(student6,studentpk1);
	cout<<student6.name<< " " << student6.age<<endl; 
	cout<<data[1].name<<endl;*/
	return 0;
}
