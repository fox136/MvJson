#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <fstream>
#include <fcntl.h>
#include <iostream>
#include <cassert>
#include <string>

#include "mv_json.h"
using namespace std;

string textFileRead(string file)
{
    ifstream infile;
    infile.open(file.data());   // 将文件流对象与文件连接起来 
    assert(infile.is_open());   // 若失败,则输出错误消息,并终止程序运行 

    string str;
    string s;
    while (getline(infile, s))
    {
        str = str + s + "\n";
    }
    infile.close();             // 关闭文件输入流 
    while (str[0] != '{' && str[0] != '[') // 开头三个字节是utf-8 bom头部标记
    {
        str = str.substr(1, str.size());
    }
    return str;
}

void test1(void)
{
    std::string strJson = textFileRead("test.json");
    cout << "原始字符串:" << strJson << endl;
    CMvJson oMvJson = CMvJson(strJson);
    cout << "Json还原的:" << oMvJson.GetStringPro() << endl;

    cout << endl << "获取元素的值：" << endl;
    cout << "a1: " << oMvJson["a1"].GetStringPro() << endl;
    cout << "a2: " << oMvJson["a2"].GetStringPro() << endl;
    cout << "a3: " << oMvJson["a3"].GetStringPro() << endl;
    cout << "a4: " << oMvJson["a4"].GetString() << endl;
    cout << "a5: " << oMvJson["a5"].GetStringPro() << endl;
    cout << "a6: " << oMvJson["bb"]["bb"]["a6"].GetStringPro() << endl;
    cout << "a7: " << oMvJson["bb"]["cc"][0]["cc"][1][0][0]["a7"].GetString() << endl;
    cout << "a8: " << oMvJson["a8"].GetString() << endl;
    cout << "a9: " << oMvJson["a9"].GetStringPro() << endl;

    cout << endl << "遍历map元素的值" << endl;
    map<string, CMvJson>::iterator iteMap = oMvJson.begin_map();
    for (; iteMap != oMvJson.end_map(); ++iteMap)
    {
        cout << iteMap->first << ": " << iteMap->second.GetStringPro() << endl;
    }

    cout << endl << "遍历数组元素的值：方法一" << endl;
    CMvJson& vecMj = oMvJson["bb"]["cc"][0]["cc"][0];
    vector<CMvJson>::iterator iteVec = vecMj.begin_vector();
    for (; iteVec != vecMj.end_vector(); ++iteVec)
    {
        cout << iteVec->GetStringPro() << endl;
    }

    cout << endl << "遍历数组元素的值：方法二" << endl;
    for (int i = 0; i < vecMj.size(); ++i)
    {
        cout << vecMj[i].GetStringPro() << endl;
    }

    cout << endl << "map修改一个元素的值" << endl;
    cout << "map修改前：a9: " << oMvJson["a9"].GetStringPro() << endl;
    oMvJson["a9"] = "你很棒哦！";
    cout << "map修改后：a9: " << oMvJson["a9"].GetStringPro() << endl;

    cout << endl << "map增加一个元素" << endl;
    oMvJson["a10"] = "你很棒哦！123";
    cout << "a10: " << oMvJson["a10"].GetStringPro() << endl;

    cout << endl << "map删除一个元素" << endl;
    oMvJson.erase("a10");
    cout << "map删除后：a10: " << oMvJson["a10"].GetStringPro() << endl;

    cout << endl << "数组修改一个元素值" << endl;
    if (vecMj.size() > 0 && vecMj.GetType() == CMvJson::OBJ_TYPE_VECTOR)
    {
        cout << "数组修改前：vecMj[0]: " << vecMj[0].GetStringPro() << endl;
        vecMj[0] = CMvJson("[]");
        cout << "数组修改后：vecMj[0]: " << vecMj[0].GetStringPro() << endl;
    }

    if (vecMj.GetType() == CMvJson::OBJ_TYPE_VECTOR)
    {
        cout << endl << "数组增加一个元素" << endl;
        vecMj.push_back(vecMj);
        cout << "vecMj[vecMj.size()-1]: " << vecMj[vecMj.size() - 1].GetStringPro() << endl;
    }

    cout << endl << "创建一个全新的对象" << endl;
    CMvJson myJson = CMvJson("{}");
    myJson["学号"] = "00001";
    myJson["姓名"] = "玛丽";
    myJson["学校"] = "东兰县高级中学";
    myJson["是否三好学生"] = CMvJson::OBJ_TRUE;
    myJson["旷课记录"] = CMvJson::OBJ_NULL;
    myJson["科目"] = CMvJson("[]");
    CMvJson& mjKm = myJson["科目"];
    CMvJson chinese = CMvJson("{}");
    chinese["科目"] = "语文";
    chinese["分数"] = 100;
    mjKm.push_back(chinese);
    string strMath = "{\"科目\":\"数学\",\"分数\":110}";
    mjKm.push_back(CMvJson(strMath));

    map<string, CMvJson>::iterator iteMyJson = myJson.begin_map();
    for (; iteMyJson != myJson.end_map(); ++iteMyJson)
    {
        cout << iteMyJson->first << ": " << iteMyJson->second.GetStringPro() << endl;
    }
}

int main()
{
    TRACK_LOG_RESET; // 开始监控log
    test1();
    TRACK_LOG_SHOW; // 打印监控到的log
    return 0;
}
