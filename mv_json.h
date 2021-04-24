#ifndef _MV_JSON_H_
#define _MV_JSON_H_
#include <string>
#include <iostream>
#include <map>
#include <vector>

/*
json文法描述：
    S->hAi
    S->jNk
    A->agVfA
    A->ε
    N->SfN
    N->ε
    V->a
    V->b
    V->c
    V->d
    V->e
    V->S
    ε->空集
    a->字符串
    b->数字
    c->null
    d->true
    e->false
    f->,
    g->:
    h->{
    i->}
    j->[
    k->]

注意：A和N最后面不可以是f

以上是json文法规则，分别对应代码中的：GrammarS, GrammarA, GrammarN, GrammarV四个函数
*/

class CMvJson
{
public:
    CMvJson(void)
    {
        Reset(); // 初始化当前对象
    }
    CMvJson(const std::string& strSrc) // 利用字符串构造对象
    {
        Reset(); // 初始化当前对象
        m_strSrc = strSrc; // 用于构造对象的字符串
        GrammarS(*this); // 使用S文法生成对象
        if (!CheckNextChar(0)) // 文法S走完之后，如果末尾还有字符串，则不符合json语法
        {
            Reset(); // 如果创建对象失败，将对象还原到最初状态
        }
    }
    ~CMvJson(void) {}
    void Reset(void)
    {
        m_strData = ""; // 当前对象的数据
        m_nObjType = OBJ_TYPE_INVALID; // 当前对象类型
        m_nBp = 0; // 用于构造对象的字符串的偏移量
        m_strSrc = ""; // 用于构造对象的字符串
        m_nBelongType = OBJ_TYPE_INVALID; // 当前对象是map元素还是数组元素
        m_mapJson.clear(); // 清空map数据
        m_vecJson.clear(); // 清空数组数据
    }

    CMvJson& operator[](const std::string& strMapKey); // 操作符重载
    CMvJson& operator[](size_t nVecIndex); // 操作符重载
	std::string operator()(void); // 操作符重载(获取字符串)
    CMvJson& operator=(const std::string& strValue); // 操作符重载
    CMvJson& operator=(double dValue); // 操作符重载
    CMvJson& operator=(const CMvJson& dValue); // 操作符重载
    int erase(const std::string& strKey); // 删除map对象的一个元素
    int push_back(const CMvJson& oMvJson); // 向数组添加一个元素
    int pop_back(void); // 删除数组对象最后一个元素

    std::string GetString(void); // 获取字符串类型对象的字符串
    std::string GetStringPro(void); // 获取任意类型对象字符串
    int GetType(void); // 获取对象类型
    int size(void); // 获取元素个数
    std::map<std::string, CMvJson>::iterator begin_map(void);
    std::map<std::string, CMvJson>::iterator end_map(void);
    std::vector<CMvJson>::iterator begin_vector(void);
    std::vector<CMvJson>::iterator end_vector(void);

protected: // 这个protected修辞的区域与文法规则有关
    int GrammarS(CMvJson& oMvJson); // S文法
    int GrammarA(CMvJson& oMvJson); // A文法
    int GrammarN(CMvJson& oMvJson); // N文法
    int GrammarV(CMvJson& oMvJson); // V文法

    std::string GetStringMap(void); // 获取map类型对象的字符串
    std::string GetStringVector(void); // 获取数组类型对象的字符串

protected: // 这个protected修辞的区域是一些辅助函数
    char GetEffectiveChar(void); // 获取下一个有效字符
    int GetNextString(std::string& str); // 获取一个字符串
    bool CheckNextChar(char ch); // 校验下一个字符是否符合预期
    bool IsInvisibleChar(char ch); // 校验当前字符是否有效
    bool CheckNextIsNumber(void); // 校验下一个字符是否为数字
    bool CheckNextIsLowercase(void); // 校验当前字符是否为小写字母
    int GetNumberString(std::string& str); // 获取一串数字
    bool TryGetBoolString(std::string& str); // 尝试获取true或者false
    bool TryGetNullString(std::string& str); // 尝试获取null

protected:
    std::map<std::string, CMvJson> m_mapJson; // 当前对象map，如果当前对象是map类型，这个变量有内容，否则没内容。
    std::vector<CMvJson> m_vecJson; // 当前对象数组，如果当前对象是数组类型，这个变量有内容，否则没内容。
    std::string m_strData; // 当前对象的字符串
    int m_nObjType; // 当前对象类型
    int m_nBelongType; // 当前对象是一个map元素还是数组元素（之所以有这个变量，是因为json数组元素不能是字符串或者数字等）

protected:
    std::string m_strSrc; // 用于构造对象的字符串，仅当用字符串构造对象时使用
    int m_nBp; // 字符串偏移量，仅当用字符串构造对象时使用

public:
    static std::string strLog;     // 保存log字符串

public:
    const static CMvJson OBJ_NULL; // null 对象
    const static CMvJson OBJ_TRUE; // true 对象
    const static CMvJson OBJ_FALSE; // false 对象
    static CMvJson OBJ_INVALID; //这是一个无效的对象,虽然没有加const，但是这个对象不会被复制或者修改
    static CMvJson CreateEspecialMvJson(const std::string& strData, int nType); // 获取一个特殊的对象（null, true, false）
    enum { // 对象枚举类型
        OBJ_TYPE_INVALID = 0, // 无效类型
        OBJ_TYPE_DOUBLE = 1,  // 数字类型
        OBJ_TYPE_BOOL = OBJ_TYPE_DOUBLE * 2, // true 或者 false
        OBJ_TYPE_STRING = OBJ_TYPE_BOOL * 2, // 字符串类型
        OBJ_TYPE_MAP = OBJ_TYPE_STRING * 2, // map类型
        OBJ_TYPE_VECTOR = OBJ_TYPE_MAP * 2, // 数组类型
        OBJ_TYPE_NULL = OBJ_TYPE_VECTOR * 2, // null类型
    };
};

#if 0
#define TRACK_LOG_RESET std::cout << "TRACK_LOG_RESET. FUNCTION:" << __FUNCTION__ << "LINE:" << __LINE__ << endl; CMvJson::strLog = "";

// 这里用do while(0)包起来，避免局部变量污染
#define TRACK_LOG(...) do { \
    char rootBuff[1024] = {0}; \
    sprintf(rootBuff, "TRACK_LOG. [FUNCTION:%s][LINE:%d]", __FUNCTION__, __LINE__);\
    char buf[1024 * 10] = {0};\
    sprintf(buf, __VA_ARGS__);\
    CMvJson::strLog = std::string(rootBuff) + buf + "\n" + CMvJson::strLog;\
    } while(0);\

#define TRACK_LOG_SHOW std::cout << CMvJson::strLog; std::cout << "TRACK_LOG_SHOW. [FUNCTION:" << __FUNCTION__  << "][LINE:" << __LINE__ << "]" << endl;;
#else
#define TRACK_LOG_RESET 
#define TRACK_LOG(...) 
#define TRACK_LOG_SHOW
#endif // !RELEASE

#endif // !_MV_JSON_H_
