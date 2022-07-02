#include <iostream>
#include "message.pb.h"

using namespace std;

int main()
{
	P p;
	p.set_id(12);
	p.set_name("jack");

	string content;
	p.SerializeToString(&content);

	cout << content << endl;

	P p1;
	p1.ParseFromString(content);
	cout << p1.id() << "," << p1.name() << endl;


	return 0;
}
