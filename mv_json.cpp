#include "mv_json.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

CMvJson g_oMvJson;
std::string CMvJson::strLog = "";

CMvJson& CMvJson::operator[](const std::string& strMapKey)
{
	if (this->m_mapJson.find(strMapKey) != this->m_mapJson.end())
	{
		return this->m_mapJson[strMapKey];
	}
	else
	{
		return g_oMvJson;
	}
}

CMvJson& CMvJson::operator[](unsigned int nVecIndex)
{
	if (nVecIndex < this->m_vecJson.size())
	{
		return this->m_vecJson[nVecIndex];
	}
	else
	{
		return g_oMvJson;
	}
}

std::string CMvJson::GetString(void)
{
	return m_strData;
}

int CMvJson::GetType(void)
{
	return m_nObjType;
}

int CMvJson::EraseJson(const std::string& strKey)
{
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		return 1;
	}
	if (0 == m_mapJson.erase(strKey))
	{
		return 2;
	}
	return 0;
}

int CMvJson::PopBackJson(void)
{
	if (OBJ_TYPE_VECTOR != m_nObjType)
	{
		return 1;
	}
	if (m_vecJson.size() == 0)
	{
		return 2;
	}
	m_vecJson.pop_back();
	return 0;
}

std::string CMvJson::GetStringPro(void)
{
	std::string strJson;
	if (m_nObjType == OBJ_TYPE_MAP)
	{
		strJson += GetStringMap();
	}
	else if (m_nObjType == OBJ_TYPE_VECTOR)
	{
		strJson += GetStringVector();
	}
	else if (m_nObjType == OBJ_TYPE_INT || m_nObjType == OBJ_TYPE_FLOAT ||
		m_nObjType == OBJ_TYPE_BOOL || m_nObjType == OBJ_TYPE_NULL)
	{
		strJson = m_strData;
	}
	else if (m_nObjType == OBJ_TYPE_STRING)
	{
		strJson = "\"" + m_strData + "\"";
	}
	if (strJson.empty())
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
	}
	return strJson;
}

std::string CMvJson::GetStringMap(void)
{
	std::string str;
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return "";
	}
	str = "{";
	std::map< std::string, CMvJson >::iterator ite = m_mapJson.begin();
	for (; ite != m_mapJson.end(); ++ite)
	{
		str = str + "\"" + ite->first + "\"" + ":" + ite->second.GetStringPro() + ",";
	}
	if (str.size() > 1)
	{
		str = str.substr(0, str.size() - 1);
	}
	str += "}";
	return std::move(str);
}

char CMvJson::GetEffectiveChar()
{
	while (IsInvisibleChar(m_strSrc[m_nBp]))
	{
		++m_nBp;
	}
	if (m_strSrc[m_nBp] > 0)
	{
		TRACK_LOG("[%s][%d][%c]\n", __FUNCTION__, __LINE__, m_strSrc[m_nBp]);
	}
	return m_strSrc[m_nBp++];
}

int CMvJson::GrammarS(CMvJson& oMvJson)
{
	if (CheckNextChar('{'))
	{
		if (0 != GrammarM(oMvJson))
		{
			return 1;
		}
		if (CheckNextChar(0))
		{
			return 0;
		}
	}
	else if (CheckNextChar('['))
	{
		if (0 != GrammarN(oMvJson))
		{
			return 1;
		}
		if (CheckNextChar(0))
		{
			return 0;
		}
	}
	else
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	return 0;
}

int CMvJson::GrammarA(CMvJson& oMvJson)
{
	if (CheckNextChar('}'))
	{
		return 0;
	}
	else if (!CheckNextChar('"'))
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	GetEffectiveChar();
	std::string strKey;
	GetNextString(strKey);
	if (strKey.size() == 0)
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	if (':' != GetEffectiveChar())
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	if (0 != GrammarV(oMvJson.m_mapJson[strKey]))
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	if (CheckNextChar(','))
	{
		GetEffectiveChar();
		if (CheckNextChar('}'))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
		if (0 != GrammarA(oMvJson))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
	}
	TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
	return 0;
}
int CMvJson::GrammarB(CMvJson& oMvJson)
{
	if (CheckNextChar(']'))
	{
		return 0;
	}
	if (!CheckNextChar('{'))
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	CMvJson oMj;
	if (0 != GrammarM(oMj))
	{
		return 1;
	}
	oMvJson.m_vecJson.push_back(oMj);
	if (CheckNextChar(','))
	{
		GetEffectiveChar();
		if (CheckNextChar('}'))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
		if (GrammarB(oMvJson))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
	}
	return 0;
}

int CMvJson::GrammarV(CMvJson& oMvJson)
{
	if (CheckNextIsNumber())
	{
		std::string strNumber;
		if (0 != GetNumberString(strNumber))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
		if (CheckNextChar('.'))
		{
			if (CheckNextIsNumber())
			{
				std::string strFloat;
				if (0 != GetNumberString(strFloat))
				{
					TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
					return 1;
				}
				oMvJson.m_nObjType = OBJ_TYPE_FLOAT;
				oMvJson.m_strData = strNumber + "." + strFloat;
			}
		}
		else {
			oMvJson.m_nObjType = OBJ_TYPE_INT;
			oMvJson.m_strData = strNumber;
		}
	}
	else if (CheckNextChar('"'))
	{
		GetEffectiveChar();
		std::string strValue;
		if (0 != GetNextString(strValue))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
		oMvJson.m_nObjType = OBJ_TYPE_STRING;
		oMvJson.m_strData = strValue;
	}
	else if (CheckNextChar('{'))
	{
		if (0 != GrammarM(oMvJson))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
	}
	else if (CheckNextChar('['))
	{
		if (0 != GrammarN(oMvJson))
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
	}
	else if (CheckNextIsLowercase())
	{
		std::string str;
		if (TryGetBoolString(str))
		{
			oMvJson.m_nObjType = OBJ_TYPE_BOOL;
			oMvJson.m_strData = str;
		}
		else if (TryGetNullString(str))
		{
			oMvJson.m_nObjType = OBJ_TYPE_NULL;
			oMvJson.m_strData = str;
		}
		else
		{
			TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
			return 1;
		}
	}
	else
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	return 0;
}

bool CMvJson::TryGetBoolString(std::string& str)
{
	std::string strTrue = "true";
	std::string strFlase = "false";
	if (m_strSrc.substr(m_nBp, strTrue.size()) == strTrue)
	{
		m_nBp += strTrue.size();
		if (!CheckNextIsLowercase())
		{
			str = "true";
			return true;
		}
	}
	else if (m_strSrc.substr(m_nBp, strFlase.size()) == strFlase)
	{
		m_nBp += strFlase.size();
		if (!CheckNextIsLowercase())
		{
			str = "false";
			return true;
		}
	}
	return false;
}

bool CMvJson::CheckNextChar(char ch)
{
	while (IsInvisibleChar(m_strSrc[m_nBp]))
	{
		++m_nBp;
	}
	return m_strSrc[m_nBp] == ch;
}

bool CMvJson::IsInvisibleChar(char ch)
{
	return ch == '\t' || ch == '\n';
}

bool CMvJson::TryGetNullString(std::string& str)
{
	std::string strNull = "null";
	if (m_strSrc.substr(m_nBp, strNull.size()) == strNull)
	{
		m_nBp += strNull.size();
		if (!CheckNextIsLowercase())
		{
			str = "null";
			return true;
		}
	}
	return false;
}

std::string CMvJson::GetStringVector(void)
{
	std::string str = "[";
	for (unsigned int i = 0; i < m_vecJson.size(); ++i)
	{
		str += m_vecJson[i].GetStringMap() + ",";
	}
	if (str.size() > 1)
	{
		str = str.substr(0, str.size() - 1);
	}
	str += "]";
	return std::move(str);
}

int CMvJson::GetNumberString(std::string& str)
{
	char buf[2] = { 0 };
	int i = 0;
	for (; i < 2 && CheckNextIsNumber(); ++i)
	{
		buf[i] = GetEffectiveChar();
	}
	if (0 == i)
	{
		return 1;
	}
	str = buf;
	return 0;
}

bool CMvJson::CheckNextIsNumber(void)
{
	return m_strSrc[m_nBp] >= '0' && m_strSrc[m_nBp] <= '9';
}

bool CMvJson::CheckNextIsLowercase(void)
{
	return m_strSrc[m_nBp] >= 'a' && m_strSrc[m_nBp] <= 'z';
}

int CMvJson::GetNextString(std::string& str)
{
	for (int i = 0; i < 20000; ++i)
	{
		char ch = GetEffectiveChar();
		if (0 == ch)
		{
			return 1;
		}
		if ('\\' == ch && !CheckNextChar(0))
		{
			str = str + '\\' + GetEffectiveChar();
		}
		if ('"' == ch)
		{
			return 0;
		}
		str += ch;
	}
	TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
	return 1;
}

int CMvJson::AddJson(const std::string& strKey, const std::string& strValue)
{
	if (strKey.empty())
	{
		return 1;
	}
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		return 2;
	}
	CMvJson oMvJson;
	oMvJson.m_strData = strValue;
	oMvJson.m_nObjType = OBJ_TYPE_STRING;
	m_mapJson[strKey] = oMvJson;
	return 0;
}

int CMvJson::AddJson(const std::string& strKey, int nValue)
{
	if (strKey.empty())
	{
		return 1;
	}
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		return 2;
	}
	CMvJson oMvJson;
	char buf[32] = { 0 };
	snprintf(buf, sizeof(buf), "%d", nValue);
	oMvJson.m_strData = buf;
	oMvJson.m_nObjType = OBJ_TYPE_INT;
	m_mapJson[strKey] = oMvJson;
	return 0;
}

int CMvJson::AddJson(const std::string& strKey, bool bValue)
{
	if (strKey.empty())
	{
		return 1;
	}
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		return 2;
	}
	CMvJson oMvJson;
	if (bValue)
	{
		oMvJson.m_strData = "true";
	}
	else
	{
		oMvJson.m_strData = "false";
	}
	oMvJson.m_nObjType = OBJ_TYPE_BOOL;
	m_mapJson[strKey] = oMvJson;
	return 0;
}

int CMvJson::AddJson(const std::string& strKey, void *p)
{
	if (strKey.empty())
	{
		return 1;
	}
	if (OBJ_TYPE_MAP != m_nObjType)
	{
		return 2;
	}
	CMvJson oMvJson;
	if (!p)
	{
		oMvJson.m_strData = "null";
	}
	else
	{
		return 3;
	}
	oMvJson.m_nObjType = OBJ_TYPE_NULL;
	m_mapJson[strKey] = oMvJson;
	return 0;
}

int CMvJson::AddJson(const std::string& strKey, const CMvJson& oMvJson)
{
	if (OBJ_TYPE_MAP == this->m_nObjType && OBJ_TYPE_INVALID != oMvJson.m_nObjType)
	{
		if (!strKey.empty())
		{
			this->m_mapJson[strKey] = oMvJson;
		}
	}
	else
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 1;
	}
	return 0;
}

int CMvJson::PushBackJson(const std::string& strKey, const CMvJson& oMvJson)
{
	if (strKey.empty())
	{
		return 1;
	}
	if (OBJ_TYPE_VECTOR != m_nObjType)
	{
		return 2;
	}
	if (OBJ_TYPE_MAP != oMvJson.m_nObjType && OBJ_TYPE_VECTOR != oMvJson.m_nObjType)
	{
		TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
		return 3;
	}
	m_vecJson.push_back(oMvJson);
	return 0;
}

int CMvJson::GrammarM(CMvJson& oMvJson)
{
	if (!CheckNextChar('{'))
	{
		return 1;
	}
	GetEffectiveChar();
	oMvJson.m_nObjType = OBJ_TYPE_MAP;
	if (0 != GrammarA(oMvJson))
	{
		return 2;
	}
	if (!CheckNextChar('}'))
	{
		return 3;
	}
	GetEffectiveChar();
	return 0;
}

int CMvJson::GrammarN(CMvJson& oMvJson)
{
	if (!CheckNextChar('['))
	{
		return 1;
	}
	GetEffectiveChar();
	oMvJson.m_nObjType = OBJ_TYPE_VECTOR;
	if (0 != GrammarB(oMvJson))
	{
		return 2;
	}
	if (!CheckNextChar(']'))
	{
		return 3;
	}
	GetEffectiveChar();
	return 0;
}
