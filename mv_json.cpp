#include "mv_json.h"
#include <cstring>
#include <iostream>

std::string CMvJson::strLog = "";

const CMvJson CMvJson::OBJ_NULL = CMvJson::CreateEspecialMvJson("null", CMvJson::OBJ_TYPE_NULL);
const CMvJson CMvJson::OBJ_TRUE = CMvJson::CreateEspecialMvJson("true", CMvJson::OBJ_TYPE_BOOL);
const CMvJson CMvJson::OBJ_FALSE = CMvJson::CreateEspecialMvJson("false", CMvJson::OBJ_TYPE_BOOL);
CMvJson CMvJson::OBJ_INVALID = CMvJson(); // 不加const是让它表面上可以被改变（编译通过），但是这个对象不会被修改

CMvJson CMvJson::CreateEspecialMvJson(const std::string& strData, int nType)
{
    CMvJson oMvJson;
    if ((OBJ_TYPE_BOOL == nType && ("true" == strData or "false" == strData)) ||
        (OBJ_TYPE_NULL == nType && "null" == strData))
    {
        oMvJson.m_strData = strData;
        oMvJson.m_nObjType = nType;
    }
    return oMvJson;
}

CMvJson& CMvJson::operator[](const std::string& strMapKey)
{
    if (OBJ_TYPE_MAP == m_nObjType)
    {
        return m_mapJson[strMapKey];
    }
    else
    {
        return CMvJson::OBJ_INVALID; // 程序运行到这里没有意义。在这里返回一个唯一的空对象，这样程序员在使用之前可以不校验该对象的合法性。
    }
}

CMvJson& CMvJson::operator[](size_t nVecIndex)
{
    if (OBJ_TYPE_VECTOR == this->m_nObjType && nVecIndex < this->m_vecJson.size())
    {
        return this->m_vecJson[nVecIndex];
    }
    else
    {
        return CMvJson::OBJ_INVALID; // 程序运行到这里没有意义。在这里返回一个唯一的空对象，这样程序员在使用之前可以不校验该对象的合法性。
    }
}

std::string CMvJson::GetString(void)  // 获取字符串，如果不是字符串类型对象，则返回空
{
    if (OBJ_TYPE_STRING == this->m_nObjType)
    {
        return m_strData;
    }
    return "";
}

int CMvJson::GetType(void) // 获取对象类型
{
    return m_nObjType;
}

int CMvJson::erase(const std::string& strKey) // 删除一个一个map元素
{
    int status = 0;
    if (OBJ_TYPE_MAP != m_nObjType)
    {
        status = 1;
        goto out;
    }
    if (0 == m_mapJson.erase(strKey))
    {
        status = 2;
        goto out;
    }
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

int CMvJson::pop_back(void) // 删除数组中最后一个元素
{
    int status = 0;
    if (OBJ_TYPE_VECTOR != m_nObjType)
    {
        status = 1;
        goto out;
    }
    if (m_vecJson.size() == 0)
    {
        status = 2;
        goto out;
    }
    if (this == &CMvJson::OBJ_INVALID) // 不能给这个无效的静态变量赋值
    {
        status = 3;
        goto out;
    }
    m_vecJson.pop_back();
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

std::string CMvJson::GetStringPro(void) // 获取任意类型对象的字符串
{
    std::string strJson;
    if (m_nObjType == OBJ_TYPE_MAP)
    {
        strJson = GetStringMap();
    }
    else if (m_nObjType == OBJ_TYPE_VECTOR)
    {
        strJson = GetStringVector();
    }
    else if (m_nObjType == OBJ_TYPE_DOUBLE || m_nObjType == OBJ_TYPE_BOOL || m_nObjType == OBJ_TYPE_NULL)
    {
        strJson = m_strData;
    }
    else if (m_nObjType == OBJ_TYPE_STRING)
    {
        strJson = "\"" + m_strData + "\"";
    }
    return strJson;
}

std::string CMvJson::GetStringMap(void) // 获取map对象字符串
{
    std::string str;
    if (OBJ_TYPE_MAP != m_nObjType)
    {
        return "";
    }
    str = "{";
    std::map< std::string, CMvJson >::iterator ite = m_mapJson.begin();
    for (; ite != m_mapJson.end(); ++ite)
    {
        if (OBJ_TYPE_INVALID != ite->second.m_nObjType)
        {
            str = str + "\"" + ite->first + "\"" + ":" + ite->second.GetStringPro() + ",";
        }
    }
    if (str.size() > 1)
    {
        str = str.substr(0, str.size() - 1);
    }
    str += "}";
    return str;
}

char CMvJson::GetEffectiveChar() // 获取下一个有效字符，跳过空白字符
{
    while (IsInvisibleChar(m_strSrc[m_nBp]))
    {
        ++m_nBp;
    }
    return m_strSrc[m_nBp++];
}

int CMvJson::GrammarS(CMvJson& oMvJson) // S语法
{
    int status = 0;
    if (CheckNextChar('{'))
    {
        GetEffectiveChar(); // 跳过一个有效字符
        oMvJson.m_nObjType = OBJ_TYPE_MAP;
        if (0 != GrammarA(oMvJson))
        {
            status = 1;
            goto out;
        }
        if (!CheckNextChar('}'))
        {
            status = 2;
            goto out;
        }
        GetEffectiveChar(); // 跳过一个有效字符
    }
    else if (CheckNextChar('['))
    {
        GetEffectiveChar(); // 跳过一个有效字符
        oMvJson.m_nObjType = OBJ_TYPE_VECTOR;
        if (0 != GrammarN(oMvJson))
        {
            status = 3;
            goto out;
        }
        if (!CheckNextChar(']'))
        {
            status = 4;
            goto out;
        }
        GetEffectiveChar(); // 跳过一个有效字符
    }
    else
    {
        status = 5;
        goto out;
    }
out:
    TRACK_LOG("[status:%d]", status);
    if (0 != status)
    {
        oMvJson.Reset();
    }
    return status;
}

int CMvJson::GrammarA(CMvJson& oMvJson) // A语法
{
    int status = 0;
    std::string strKey;
    if (CheckNextChar('}'))
    {
        status = 0;
        goto out;
    }
    else if (!CheckNextChar('"'))
    {
        status = 1;
        goto out;
    }
    GetEffectiveChar();
    GetNextString(strKey);
    if (strKey.size() == 0)
    {
        status = 2;
        goto out;
    }
    if (':' != GetEffectiveChar())
    {
        status = 3;
        goto out;
    }
    if (0 != GrammarV(oMvJson.m_mapJson[strKey]))
    {
        status = 4;
        goto out;
    }
    if (CheckNextChar(','))
    {
        GetEffectiveChar();
        if (CheckNextChar('}'))
        {
            status = 5;
            goto out;
        }
        if (0 != GrammarA(oMvJson))
        {
            status = 6;
            goto out;
        }
    }
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

int CMvJson::GrammarV(CMvJson& oMvJson) // V语法
{
    int status = 0;
    if (CheckNextIsNumber())
    {
        std::string strNumber;
        if (0 != GetNumberString(strNumber))
        {
            status = 1;
            goto out;
        }
        if (CheckNextChar('.'))
        {
            GetEffectiveChar();
            if (CheckNextIsNumber())
            {
                std::string strFloat;
                if (0 != GetNumberString(strFloat))
                {
                    status = 2;
                    goto out;
                }
                oMvJson.m_strData = strNumber + "." + strFloat;
            }
            else
            {
                status = 3;
                goto out;
            }
        }
        else {
            oMvJson.m_strData = strNumber;
        }
        oMvJson.m_nObjType = OBJ_TYPE_DOUBLE;
    }
    else if (CheckNextChar('"'))
    {
        GetEffectiveChar();
        std::string strValue;
        if (0 != GetNextString(strValue))
        {
            status = 4;
            goto out;
        }
        oMvJson.m_nObjType = OBJ_TYPE_STRING;
        oMvJson.m_strData = strValue;
    }
    else if (CheckNextChar('{') || CheckNextChar('['))
    {
        if (0 != GrammarS(oMvJson))
        {
            status = 5;
            goto out;
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
            status = 6;
            goto out;
        }
    }
    else
    {
        status = 7;
        goto out;
    }
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

bool CMvJson::TryGetBoolString(std::string& str) // 尝试获取bool类型字符串
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

bool CMvJson::CheckNextChar(char ch) // 校验下一个有效字符
{
    while (IsInvisibleChar(m_strSrc[m_nBp]))
    {
        ++m_nBp;
    }
    return m_strSrc[m_nBp] == ch;
}

bool CMvJson::IsInvisibleChar(char ch)
{
    return '\t' == ch || '\n' == ch || ' ' == ch;
}

bool CMvJson::TryGetNullString(std::string& str) // 尝试获取null类型字符串
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

std::string CMvJson::GetStringVector(void) // 获取数组类型对象字符串
{
    std::string str = "[";
    for (unsigned int i = 0; i < m_vecJson.size(); ++i)
    {
        str += m_vecJson[i].GetStringPro() + ",";
    }
    if (str.size() > 1)
    {
        str = str.substr(0, str.size() - 1);
    }
    str += "]";
    return str;
}

int CMvJson::GetNumberString(std::string& str) // 获取一个数字
{
    int status = 0;
    char buf[64] = { 0 };
    int i = 0;
    for (; i < sizeof(buf) && CheckNextIsNumber(); ++i)
    {
        buf[i] = GetEffectiveChar();
    }
    if (0 == i)
    {
        status = 2;
        goto out;
    }
    str = buf;
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

bool CMvJson::CheckNextIsNumber(void) // 判断一个字符是否是数字
{
    while (IsInvisibleChar(m_strSrc[m_nBp]))
    {
        ++m_nBp;
    }
    return m_strSrc[m_nBp] >= '0' && m_strSrc[m_nBp] <= '9';
}

bool CMvJson::CheckNextIsLowercase(void)
{
    return m_strSrc[m_nBp] >= 'a' && m_strSrc[m_nBp] <= 'z';
}

int CMvJson::GetNextString(std::string& str) // 获取一个字符串
{
    CMvJson oMvJson;
    int status = 0;
    for (int i = 0; i < 20000; ++i)
    {
        char ch = m_strSrc[m_nBp++];
        if (0 == ch)
        {
            status = 2;
            goto out;
        }
        if ('\\' == ch)
        {
            char chNext = m_strSrc[m_nBp++];
            if (0 == chNext)
            {
                status = 3;
                goto out;
            }
            str = str + '\\' + chNext;
            continue;
        }
        if ('"' == ch)
        {
            status = 0;
            goto out;
        }
        str += ch;
    }
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

CMvJson& CMvJson::operator=(const std::string& strValue)
{
    int status = 0;
    CMvJson oMvJson;
    oMvJson.m_strData = strValue;
    oMvJson.m_nObjType = OBJ_TYPE_STRING;
    if (OBJ_TYPE_INVALID != m_nObjType && CMvJson::OBJ_TYPE_VECTOR == m_nBelongType)
    {
        status = 1;
        goto out;
    }
    if (this == &CMvJson::OBJ_INVALID) // 不能给这个无效的静态变量赋值
    {
        status = 2;
        goto out;
    }
    *this = oMvJson;
out:
    TRACK_LOG("[status:%d]", status);
    return *this;
}

CMvJson& CMvJson::operator=(double dValue)
{
    int status = 0;
    char buf[1024] = { 0 };
    CMvJson oMvJson;
    snprintf(buf, sizeof(buf), "%lf", dValue);
    for (int i = std::string(buf).size() - 1; i > 0; --i)
    {
        if (buf[i] == '0')
        {
            buf[i] = 0;
        }
        else if (buf[i] == '.')
        {
            buf[i] = 0;
            break;
        }
        else
        {
            break;
        }
    }
    oMvJson.m_strData = buf;
    oMvJson.m_nObjType = OBJ_TYPE_DOUBLE;
    if (OBJ_TYPE_INVALID != m_nObjType && OBJ_TYPE_VECTOR == m_nBelongType)
    {
        status = 1;
        goto out;
    }
    if (this == &CMvJson::OBJ_INVALID) // 不能给这个无效的静态变量赋值
    {
        status = 2;
        goto out;
    }
    *this = oMvJson;
out:
    TRACK_LOG("[status:%d][buf:%s][dValue:%lf]", status, this->m_strData.c_str(), dValue);
    return *this;
}

CMvJson& CMvJson::operator=(const CMvJson& oMvJson)
{
    int status = 0;
    if (OBJ_TYPE_INVALID != m_nObjType && OBJ_TYPE_VECTOR == m_nBelongType &&
        OBJ_TYPE_MAP != oMvJson.m_nObjType && OBJ_TYPE_VECTOR != oMvJson.m_nObjType)
    {
        status = 1;
        goto out;
    }
    if (this == &CMvJson::OBJ_INVALID) // 不能给这个无效的静态变量赋值
    {
        status = 2;
        goto out;
    }
    this->Reset();
    this->m_strData = oMvJson.m_strData;
    this->m_nObjType = oMvJson.m_nObjType;
    // this->m_nBelongType = oMvJson.m_nBelongType;
    this->m_nBp = oMvJson.m_nBp;
    this->m_strSrc = oMvJson.m_strSrc;
    this->m_mapJson = oMvJson.m_mapJson;
    this->m_vecJson = oMvJson.m_vecJson;
out:
    TRACK_LOG("[status:%d]", status);
    return *this;
}

int CMvJson::push_back(const CMvJson& oMvJson)
{
    int status = 0;
    if (OBJ_TYPE_VECTOR != m_nObjType)
    {
        status = 1;
        goto out;
    }
    if (OBJ_TYPE_MAP != oMvJson.m_nObjType && OBJ_TYPE_VECTOR != oMvJson.m_nObjType)
    {
        status = 2;
        goto out;
    }
    if (this == &CMvJson::OBJ_INVALID) // 不能给这个无效的静态变量赋值
    {
        status = 3;
        goto out;
    }
    m_vecJson.push_back(oMvJson);
    m_vecJson[m_vecJson.size() - 1].m_nBelongType = OBJ_TYPE_VECTOR;
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

int CMvJson::GrammarN(CMvJson& oMvJson)
{
    int status = 0;
    if (CheckNextChar(']'))
    {
        status = 0;
        goto out;
    }
    do
    {
        CMvJson oMj;
        if (0 != GrammarS(oMj)) // 调用S语法
        {
            status = 2;
            goto out;
        }
        oMvJson.push_back(oMj);
    } while (CheckNextChar(',') && GetEffectiveChar());
out:
    TRACK_LOG("[status:%d]", status);
    return status;
}

int CMvJson::size(void)
{
    int nRet = 0;
    if (OBJ_TYPE_STRING == m_nObjType)
    {
        nRet = m_strData.size();
    }
    else if (OBJ_TYPE_MAP == m_nObjType)
    {
        nRet = m_mapJson.size();
    }
    else if (OBJ_TYPE_VECTOR == m_nObjType)
    {
        nRet = m_vecJson.size();
    }
    return nRet;
}

std::map<std::string, CMvJson>::iterator CMvJson::begin_map(void)
{
    return m_mapJson.begin();
}

std::map<std::string, CMvJson>::iterator CMvJson::end_map(void)
{
    return m_mapJson.end();
}

std::vector<CMvJson>::iterator CMvJson::begin_vector(void)
{
    return m_vecJson.begin();
}
std::vector<CMvJson>::iterator CMvJson::end_vector(void)
{
    return m_vecJson.end();
}
