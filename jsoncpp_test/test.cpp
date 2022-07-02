#include <iostream>
#include <string>
#include <fstream>
#include <json/json.h>

using namespace std;

void write_data()
{
	Json::Value v;
	// [12, 19.8, true, "hello", ["a", "b", "c"], { "name":"xiaoming" }, "age":12]
	v.append(Json::Value(12));
	v.append(19.8);
	v.append(true);
	v.append("hello");

	Json::Value arr;
	arr.append("a");
	arr.append("b");
	arr.append("c");

	v.append(arr);

	Json::Value obj;
	obj["name"] = "xioaming";
	obj["age"] = 12;
	v.append(obj);


	string content = v.toStyledString();
	cout << content << endl;

	Json::FastWriter fw; // 无格式
	string s = fw.write(v);
	cout << s << endl;

	Json::StyledWriter sw;
	string s1 = sw.write(v);
	cout << s1 << endl;

	// 写入文件
	ofstream ofs("test.json", ios::out);
	ofs << content;

	ofs.close();
}

void read_data()
{
	ifstream in("test.json", ios::in);
	Json::Reader rd;
	Json::Value root;
	rd.parse(in, root);

	for (int i = 0; i < root.size(); i++)
	{
		Json::Value sub = root[i];

		if (sub.isInt())
		{
			cout << sub.asInt() << " ";
		}
		else if (sub.isBool())
		{
			cout << sub.asBool() << " ";
		}
		else if (sub.isDouble())
		{
			cout << sub.asDouble() << " ";
		}
		else if (sub.isString())
		{
			cout << sub.asString() << " ";
		}
		else if (sub.isArray())
		{
			cout << "[" << " ";
			for (int j = 0; j < sub.size(); j++)
			{
				cout << sub[j].asString() << ", ";
			}
			cout << "] ";
		}
		else if (sub.isObject())
		{
			cout << sub["name"].asString() << ", "
				<< sub["age"].asInt() << " ";
		}
	}
}

int main()
{
	write_data();
	read_data();
	return 0;
}
