#if !defined __data_cache_h__
#define __data_cache_h__
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
    m_pclLoadHandler = 0x0;
  }
  virtual ~CDataCache()
  {
    Free();
  }
  
  int Append(CDataItem &p_refclDataItem)
  { 
    int iRetCode = -1;
    /*检查能否增加*/
    typename list<CIndexEvent<CDataItem>* > ::iterator it;
    bool bCanAdd = true;
    for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); it++)
    { 
      CIndexEvent<CDataItem>* pIndexEvent = *it;
      bCanAdd = pIndexEvent->CanAdd();  
      if(!bCanAdd)
      { 
        break;
      }
    }
    if(!bCanAdd)
    { 
      iRetCode = -1;
    } 
    else
    {
      m_clItems.push_back(p_refclDataItem);
      typename list<CIndexEvent<CDataItem>* > ::iterator it;
      for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); it++)
      {
        CIndexEvent<CDataItem>* pIndexEvent = *it;
        pIndexEvent->Add(p_refclDataItem, m_iItemNum);
      }
      iRetCode = m_iItemNum;  
      m_iItemNum++;
    }
    return iRetCode;
  }

  int Count(){ return m_iItemNum; }

  void Update(int p_iIndex, CDataItem & p_refclDataItem)
  { 
    if(p_iIndex <0 || p_iIndex >= m_iItemNum)
    { 
      throw out_of_range("索引下标越界");
    } 
    bool bCanUpdate = true;
    typename list<CIndexEvent<CDataItem>* > ::iterator it;
    for(it = m_lstIndexEvent.begin(); it != m_lstIndexEvent.end(); it++)
    { 
      CIndexEvent<CDataItem>* pIndexEvent = *it;
      bCanUpdate = pIndexEvent->CanUpdate(m_clItems[p_iIndex], p_refclDataItem);
      if(!bCanUpdate)
      { 
        throw logic_error("不到修改索引字段");
      } 
    } 
    m_clItems[p_iIndex] = p_refclDataItem;  
  } 
  
  const CDataItem & operator[](int p_iIndex) const
  { 
    if (p_iIndex <0 || p_iIndex >= m_iItemNum)
    {
      throw out_of_range("索引下标越界");
    }
    return m_clItems[p_iIndex];
  }
  void Clear()
  {
    Free();
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
  void SetLoadHandler(CDataCacheLoadHandler * p_pclLoadHandler)
  {
    if(p_pclLoadHandler)
    { 
      m_pclLoadHandler = p_pclLoadHandler;
    } 
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
  list<CIndexEvent<CDataItem>* > m_lstIndexEvent;
  CDataCacheLoadHandler * m_pclLoadHandler;
};

/*
Unique Index模板类定义
*/
template<typename CDataIndex, typename CDataItem, typename CQueryCondtion = CMyDummy>
class CDataCacheUniqueIndex
{ 
private:
  /*Inner class*/
  class CUniqueIndexEvent: public CIndexEvent<CDataItem>
  { 
  public:
    CUniqueIndexEvent()
    { 
       
    }
    virtual bool CanAdd()
    { 
      return true;
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
  };
public:  
  /*class self*/
  CDataCacheUniqueIndex(CDataCache<CDataItem, CQueryCondtion> &p_refclDataCache)
    : m_refclDataCache(p_refclDataCache)
  { 
    m_clUniqueIndexEvent.SetParent(this);
    m_refclDataCache.RegisterNotify(&m_clUniqueIndexEvent);
  }
  ~CDataCacheUniqueIndex()
  { 
    m_refclDataCache.UnRegisterNotify(&m_clUniqueIndexEvent);
  } 

  void CreateIndex(void)
  {
    int iIndex;
    int iCount = m_refclDataCache.Count();
    m_clUniqueIndexes.clear();
    string strIndexStr;
    
    for (iIndex = 0; iIndex < iCount; iIndex++)
    { 
      strIndexStr = SearchKey(m_refclDataCache[iIndex]);
      map<string, int>::iterator it;
      it = m_clUniqueIndexes.find(strIndexStr);
      if (it != m_clUniqueIndexes.end())
      {
        char szErr[255+1];
        if(m_szIndexName.length() == 0)
        { 
          snprintf(szErr, sizeof(szErr)-1, "%s违反唯一性索引, 索引串:[%s]", __FUNCTION__, strIndexStr.c_str());
        }
        else
        { 
          snprintf(szErr, sizeof(szErr)-1, "%s违反唯一性索引，索引串:[%s], 索引名称:[%s]", __FUNCTION__, strIndexStr.c_str(), m_szIndexName.c_str());
        }
        //throw invalid_argument(szErr);
        g_clRuntimeMsg.SetMsg(RUNTIME_TRACE(), 777777, szErr);
        throw CException();
      }
      m_clUniqueIndexes[strIndexStr] = iIndex; 
    } 
  } 
  
  int GetDataItemPos(CDataIndex &p_refclDataIndex)
  { 
    int iRetCode = -1;
    string strIndexStr = SearchKey(p_refclDataIndex);
    map<string, int>::iterator it;
    it = m_clUniqueIndexes.find(strIndexStr);
    if(it == m_clUniqueIndexes.end())
    {  
      iRetCode = -1;
      goto __end;
    } 
    iRetCode = it->second;
__end:
    return iRetCode;
  }

  int GetDataItem(CDataItem &p_refclDataItem, CDataIndex &p_refclDataIndex)
  { 
    int iRetCode = RTMSG_OK;
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

  int GetDataItemEx(int & p_refiIndex, CDataItem &p_refclDataItem, CDataIndex &p_refclDataIndex)
  { 
    int iRetCode = RTMSG_OK;
    string strIndexStr = SearchKey(p_refclDataIndex);
    map<string, int>::iterator it;
    p_refiIndex = -1;
    it = m_clUniqueIndexes.find(strIndexStr);
    if (it == m_clUniqueIndexes.end())
    {  
      iRetCode = -1;
      goto __end;
    } 
    p_refclDataItem = m_refclDataCache[it->second];
    p_refiIndex = it->second;
__end:
    return iRetCode;
  }
  
  const CDataItem & operator[](const CDataIndex &p_refclDataIndex)
  {	
    string strIndexStr = SearchKey(p_refclDataIndex);
    map<string, int>::iterator it;
    
    it = m_clUniqueIndexes.find(strIndexStr);
    if(it == m_clUniqueIndexes.end())
    {  
      throw out_of_range("未找到索引结点"); 
    } 
    return m_refclDataCache[it->second];
  }
  void SetIndexName(const char * p_szIndexName)
  {  
    m_szIndexName = p_szIndexName;
  } 
private:
  string SearchKey(const CDataIndex & p_refclDataIndex);
  string SearchKey(const CDataItem & p_refclDataItem);
  void UpdateIndex(CDataItem & p_refclDataItem, int iDataItemPos)
  { 
    string strIndexStr = SearchKey(p_refclDataItem);
    m_clUniqueIndexes[strIndexStr] = iDataItemPos; 
  }

private:
  CDataCache<CDataItem, CQueryCondtion> & m_refclDataCache;
  map<string, int> m_clUniqueIndexes;
  CUniqueIndexEvent m_clUniqueIndexEvent;
  string m_szIndexName;
};
#endif
