#ifndef _MV_JSON_H_
#define _MV_JSON_H_
#include <string>
#include <iostream>
#include <map>
#include <vector>

class CMvJson
{
public:
	CMvJson(void)
	{
		Reset();
	}
	CMvJson(const std::string& strSrc)
	{
		Reset();
		m_strSrc = strSrc;
		GrammarS(*this);
	}
	~CMvJson(void) {}
	void Reset(void)
	{
		m_strData = "";
		m_nObjType = OBJ_TYPE_INVALID;
		m_nBp = 0;
		m_strSrc = "";
	}
	CMvJson& operator[](const std::string& strMapKey);
	CMvJson& operator[](unsigned int nVecIndex);
	std::string GetStringPro(void);
	std::string GetString(void);
	int GetType(void);

	int AddJson(const std::string& strKey, const std::string& strValue);
	int AddJson(const std::string& strKey, int nValue);
	int AddJson(const std::string& strKey, bool bValue);
	int AddJson(const std::string& strKey, void* p);
	int AddJson(const std::string& strKey, const CMvJson& oMvJson);
	int PushBackJson(const std::string& strKey, const CMvJson& oMvJson);
	int EraseJson(const std::string& strKey);
	int PopBackJson(void);

protected:
	int GrammarS(CMvJson& oMvJson);
	int GrammarM(CMvJson& oMvJson);
	int GrammarN(CMvJson& oMvJson);
	int GrammarA(CMvJson& oMvJson);
	int GrammarB(CMvJson& oMvJson);
	int GrammarV(CMvJson& oMvJson);

	std::string GetStringMap(void);
	std::string GetStringVector(void);

protected:
	char GetEffectiveChar(void);
	int GetNextString(std::string& str);
	bool CheckNextChar(char ch);
	bool IsInvisibleChar(char ch);
	bool CheckNextIsNumber(void);
	bool CheckNextIsLowercase(void);
	int GetNumberString(std::string& str);
	bool TryGetBoolString(std::string& str);
	bool TryGetNullString(std::string& str);

protected:
	std::map<std::string, CMvJson> m_mapJson;
	std::vector<CMvJson> m_vecJson;
	std::string m_strData;
	int m_nObjType;

protected:
	std::string m_strSrc;
	int m_nBp;

public:
	static std::string strLog;

public:
	enum {
		OBJ_TYPE_INVALID = 0,
		OBJ_TYPE_INT = 1,
		OBJ_TYPE_FLOAT = OBJ_TYPE_INT * 2,
		OBJ_TYPE_BOOL = OBJ_TYPE_FLOAT * 2,
		OBJ_TYPE_STRING = OBJ_TYPE_BOOL * 2,
		OBJ_TYPE_MAP = OBJ_TYPE_STRING * 2,
		OBJ_TYPE_VECTOR = OBJ_TYPE_MAP * 2,
		OBJ_TYPE_NULL = OBJ_TYPE_VECTOR * 2,
	};
};

#if false
#define TRACK_LOG_RESET CMvJson::strLog = "";

#define TRACK_LOG(...) do {\
	char buf[1024] = {0};\
	sprintf_s(buf, __VA_ARGS__);\
	CMvJson::strLog = buf + CMvJson::strLog;\
	}while(0);\

#define TRACK_LOG_SHOW std::cout << CMvJson::strLog << endl;
#else
#define TRACK_LOG_RESET 
#define TRACK_LOG(...) 
#define TRACK_LOG_SHOW
#endif // !RELEASE

#endif // !_MV_JSON_H_
