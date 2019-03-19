/*#if !defined __data_cache_h__
#define __data_cache_h__
*/ 
#include <vector>
#include <string>
#include <map>
#include <iostream>
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
		virtual bool CanAdd() = 0;                                               /*�Ƿ���������*/
		virtual int Add(CDataItem &newValue, int iPos) = 0;                      /*���������¼�*/
  		virtual bool CanUpdate(CDataItem &oldValue, CDataItem &newValue) = 0;    /*�Ƿ��������*/
		virtual int Clear() = 0;                                                 /*���������¼�*/
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
    		iRetCode = it->second;
    		return iRetCode;
  		}
  		
  		
  		//����������ȡ���� 
  		int GetDataItem(CDataItem &p_refclDataItem,const CDataIndex &p_refclDataIndex)
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
	
	private:
		CDataCache<CDataItem> & m_refclDataCache;  /*���ݻ������*/ 
		map<string, int> m_clUniqueIndexes; /*����ֵ�͸�������index*/ 
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
	
	data2.push_back(student1);
	data2.push_back(student2);
	data2.push_back(student3);
	data2.push_back(student4);
	data2.push_back(student5);
	
	data.Load(data2);
	
	CDataCacheUniqueIndex<student,studentpk> studentNameUniqueIndex(data);
	
	studentpk studentpk1("С2");
	cout<<studentNameUniqueIndex.GetDataItemPos(studentpk1)<<endl;
	student student6;
	studentNameUniqueIndex.GetDataItem(student6,studentpk1);
	cout<<student6.name<< " " << student6.age<<endl; 
	cout<<data[1].name<<endl;
	return 0;
}
