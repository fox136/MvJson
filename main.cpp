
#include <iostream>
#include <map>
#include <cstdlib>
#include <fstream>

#include "mv_json.h"
using namespace std;

int main()
{
    std::string strJson;
    std::string strDataLine;
    ifstream fin("test.json");
    while (fin >> strDataLine)
    {
        strJson += strDataLine;
    }
    std::string strJsonOut;


    CMvJson oMvJson = CMvJson(strJson);
    if (oMvJson.GetType() == CMvJson::OBJ_TYPE_INVALID)
    {
        TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
        goto out;
    }
    if (0 && 0 != oMvJson.AddJson("key", strJson))
    {
        TRACK_LOG("[%s][%d]\n", __FUNCTION__, __LINE__);
        goto out;
    }

    strJsonOut = oMvJson.GetStringPro();
out:
    TRACK_LOG_SHOW;
    strJsonOut = oMvJson.GetStringPro();
    std::cout << "strJson:" << strJson << std::endl;
    std::cout << "strJsonOut:" << strJsonOut << std::endl;
    std::cout << "Hello World!\n";

    return 0;
    cout << oMvJson["bbb"].GetStringPro() << endl;
    cout << oMvJson["key"]["a1"].GetStringPro() << endl;
    cout << oMvJson["key"]["a2"].GetStringPro() << endl;
    cout << oMvJson["key"]["a3"].GetStringPro() << endl;
    cout << oMvJson["key"]["a4"].GetStringPro() << endl;
    cout << oMvJson["key"]["a6"].GetStringPro() << endl;
    return 0;
}