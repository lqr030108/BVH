#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include "bvh_parser.h"

using namespace std;

// a naive bvh parser

void motion(joint *root, string str)
{
    joint *tempchild = new joint;
    tempchild = root;
    stack<joint *> s;
    s.push(tempchild);
    int i = 0;
    //对整棵树进行前序遍历,每次读取弹出栈顶，再把它的孩子从右向左依次压栈，如果没孩子或孩子是end，则不用压栈
    //根据channels的个数，读取相应个数的数据到一个vector<double>里，再把这个vector压入motion
    while (!s.empty())
    {
        tempchild = s.top();
        s.pop();
        //孩子压栈
        if (!(*tempchild).children.empty())
        {
            int k = (*tempchild).children.size();
            k = k - 1;
            for (; k >= 0; k--)
            {

                s.push((*tempchild).children[k]);
            }
        }
        //给tempchild的motion赋值
        int j = (*tempchild).channels.size();
        vector<double> a;
        if ((*tempchild).joint_type != "End")
        {
            while (j)
            {
                string tempstr = "";
                for (; str[i] != ' '; i++)
                {
                    tempstr = tempstr + str[i];
                }
                a.push_back(stod(tempstr));
                i++;
                j--;
            }
        }
        (*tempchild).motion.push_back(a);
    }
}

void offset(joint *p, string str)
{
    string temp = "";
    int i = 0;
    for (i = 0; str[i] == ' ' || str[i] == '\t'; i++) // i先指向O
        ;
    for (i = i + 7; str[i] != ' '; i++)
    {
        temp = temp + str[i];
    }
    (*p).offset_x = stod(temp);
    temp = "";
    for (i = i + 1; str[i] != ' '; i++)
    {
        temp = temp + str[i];
    }
    (*p).offset_y = stod(temp);
    temp = "";
    for (i = i + 1; i < str.size() && str[i] != '\n' && str[i] != '\r'; i++)
    {
        temp = temp + str[i];
    }
    (*p).offset_z = stod(temp);
}

void channels(joint *p, string str)
{
    string temp = "";
    int i = 0;
    int num;
    for (i = 0; str[i] == ' ' || str[i] == '\t'; i++) // i先指向C
        ;
    i = i + 9;
    while (str[i] == ' ')
        i++; // i指向第一个数字
    num = str[i] - '0';
    i = i + 2;
    while (str[i] == ' ')
        i++; // i指向第一个channel的第一个字母

    for (int j = num; j > 0; j--)
    {
        while (str[i] == ' ')
            i++;
        while (str[i] != ' ' && i < str.size() && str[i] != '\n' && str[i] != '\r')
        {
            temp = temp + str[i];
            i++;
        }
        (*p).channels.push_back(temp);
        temp = "";
        i++;
    }
}

int main(int argc, char **argv)
{
    joint root;
    META meta_data;
    ifstream file(argv[1]);

    // do something

    if (!file.is_open())
    {
        cout << "打开文件失败" << endl;
        system("pause");
        return 0;
    }
    string str;                   //临时变量
    joint *tempchild = new joint; //临时变量
    joint *top = new joint;       //指向root
    stack<joint *> s;             //使用栈的方式进行非递归存储

    //先对ROOT进行特殊处理,让根结点进栈
    getline(file, str); //得HIERARCHY
    getline(file, str);
    while (str.substr(0, 4) != "ROOT")
        getline(file, str); //得ROOT hip
    (*tempchild).joint_type = "ROOT";
    for (int i = 5; i < str.size() && str[i] != '\n' && str[i] != '\r'; i++)
    {
        if (str[i] == ' ')
            continue;
        (*tempchild).name = (*tempchild).name + str[i];
    }
    getline(file, str); //得{
    getline(file, str); //得  OFFSET 76.5323 59.3615 165.373
    offset(tempchild, str);
    getline(file, str); //得  CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation
    channels(tempchild, str);
    top = tempchild;
    s.push(tempchild);

    //再对JOINT统一处理
    while (!s.empty())
    {
        getline(file, str); //得到JOINT或End或}
        // point先指向第一个不是空格的字符
        int point;
        for (point = 0; str[point] == ' ' || str[point] == '\t'; point++)
            ;
        //遇到JOINT，把tempchild弹出，把新的joint保存为tempchild的孩子,再把tempchild压回去，最后把新的joint压栈
        if (str[point] == 'J')
        {
            tempchild = s.top();
            s.pop();
            joint *child = new joint;
            (*child).joint_type = "JOINT";                                                   //保存type
            for (int i = point + 6; i < str.size() && str[i] != '\n' && str[i] != '\r'; i++) //保存name
            {
                if (str[i] == ' ')
                    continue;
                (*child).name = (*child).name + str[i];
            }
            getline(file, str); //得到{
            getline(file, str); //得到OFFSET
            offset(child, str);
            getline(file, str); //得到CHANNELS
            channels(child, str);
            (*tempchild).children.push_back(child); //设为栈顶tempchild的孩子
            s.push(tempchild);
            s.push(child);
        }
        //遇到End,要有特殊的处理
        if (str[point] == 'E')
        {
            tempchild = s.top();
            s.pop();
            joint *child = new joint;
            (*child).joint_type = "End"; //保存type
            string temp = (*tempchild).name;
            (*child).name = temp + "_End";          //保存name
            getline(file, str);                     //得到{
            getline(file, str);                     //得到OFFSET
            offset(child, str);                     // channels为空
            (*tempchild).children.push_back(child); //设为栈顶tempchild的孩子
            s.push(tempchild);
            s.push(child);
        }
        //遇到}，要删除栈顶
        if (str[point] == '}')
        {
            s.pop();
        }
    }
    getline(file, str); //得MOTION
    getline(file, str); //得Frames
    string tempstr = "";
    for (int i = 8; i < str.size() && str[i] != '\n' && str[i] != '\r'; i++)
    {
        if (str[i] == ' ')
            continue;
        tempstr = tempstr + str[i];
    }
    meta_data.frame = stoi(tempstr);
    getline(file, str); //得Frame Time
    tempstr = "";
    for (int i = 12; i < str.size() && str[i] != '\n' && str[i] != '\r'; i++)
    {
        if (str[i] == ' ')
            continue;
        tempstr = tempstr + str[i];
    }
    meta_data.frame_time = stod(tempstr);

    while (!file.eof())
    {
        getline(file, str);
        motion(top, str);
    }

    root = *top;
    jsonify(root, meta_data);
    file.close();
    return 0;
}