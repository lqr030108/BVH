#include <iostream>
#include <fstream>
#include "bvh_parser.h"

using namespace std;

void joint_out(joint child, int flag, ofstream &outfile)
{
    //{
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "{\n";
    flag++;
    // type
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "\"type\": \"" << child.joint_type << "\",\n";
    // name
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "\"name\": \"" << child.name << "\",\n";
    // offset
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "\"offset\": [" << child.offset_x << ", " << child.offset_y << ", " << child.offset_z << "],\n";
    // channels
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    if (child.joint_type == "ROOT")
        outfile << "\"channels\": [\"" << child.channels[0] << "\", \"" << child.channels[1] << "\", \"" << child.channels[2] << "\", \"" << child.channels[3] << "\", \"" << child.channels[4] << "\", \"" << child.channels[5] << "\"],\n";
    else if (child.joint_type == "JOINT")
        outfile << "\"channels\": [\"" << child.channels[0] << "\", \"" << child.channels[1] << "\", \"" << child.channels[2] << "\"],\n";
    else if (child.joint_type == "End")
        outfile << "\"channels\": [],\n";
    // motion
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "\"motion\": [\n";
    flag++;
    int len = child.motion.size() - 1;
    int j = 0;
    if (child.joint_type == "ROOT")
    {
        while (j < len)
        {
            for (int i = 0; i < flag; i++)
            {
                outfile << "\t";
            }
            outfile << "[" << (child.motion[j])[0] << ", " << (child.motion[j])[1] << ", " << (child.motion[j])[2] << ", " << (child.motion[j])[3] << ", " << (child.motion[j])[4] << ", " << (child.motion[j])[5] << "],\n";
            j++;
        }
        for (int i = 0; i < flag; i++)
        {
            outfile << "\t";
        }
        outfile << "[" << (child.motion[j])[0] << ", " << (child.motion[j])[1] << ", " << (child.motion[j])[2] << ", " << (child.motion[j])[3] << ", " << (child.motion[j])[4] << ", " << (child.motion[j])[5] << "]\n";
    }
    else if (child.joint_type == "JOINT")
    {
        while (j < len)
        {
            for (int i = 0; i < flag; i++)
            {
                outfile << "\t";
            }
            outfile << "[" << (child.motion[j])[0] << ", " << (child.motion[j])[1] << ", " << (child.motion[j])[2] << "],\n";
            j++;
        }
        for (int i = 0; i < flag; i++)
        {
            outfile << "\t";
        }
        outfile << "[" << (child.motion[j])[0] << ", " << (child.motion[j])[1] << ", " << (child.motion[j])[2] << "]\n";
    }
    else if (child.joint_type == "End")
    {
        while (j < len)
        {
            for (int i = 0; i < flag; i++)
            {
                outfile << "\t";
            }
            outfile << "[],\n";
            j++;
        }
        for (int i = 0; i < flag; i++)
        {
            outfile << "\t";
        }
        outfile << "[]\n";
    }
    flag--;
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "],\n";
    // children
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "\"children\": [\n";
    //递归孩子
    int children_num = child.children.size();
    if (children_num == 0)
        outfile << "\n";
    for (int i = 0; i < children_num; i++)
    {
        joint_out(*(child.children[i]), flag + 1, outfile);
        if (i != children_num - 1)
            outfile << ",\n";
        else
            outfile << "\n";
    }
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "]\n";
    //}
    flag--;
    for (int i = 0; i < flag; i++)
    {
        outfile << "\t";
    }
    outfile << "}\n";
}

void jsonify(joint root, META meta_data)
{
    ofstream outfile;
    outfile.open("output.json");

    // output the root and meta_data
    if (!outfile.is_open())
    {
        cout << "打开文件失败" << endl;
        system("pause");
        return;
    }

    int flag = 2; //记录缩进个数
    //框架的开头部分
    outfile << "{\n";
    outfile << "\t\"frame\": " << meta_data.frame << ",\n";
    outfile << "\t\"frame_time\": " << meta_data.frame_time << ",\n";
    outfile << "\t\"joint\":\n";
    //递归输出所有结点
    joint_out(root, flag, outfile);
    //框架的结尾部分
    outfile << "}\n";

    outfile.close();
}