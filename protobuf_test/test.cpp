#include <iostream>
#include "Student.pb.h"

using namespace std;

int main()
{
    T::Student st;
    st.set_id(1);
    st.set_name("jack");
    st.set_score(98.9);

    T::Student st1;
    st1.set_id(2);
    st1.set_name("marry");
    st1.set_score(88.9);

    T::Stus stus;
    T::Student *s = stus.add_stus();
    *s = st;
    T::Student *s1 = stus.add_stus();
    *s1 = st1;

    string content;
    stus.SerializeToString(&content);

    cout << "SerializeToString: " << content << endl;

    T::Stus stus1;

    stus1.ParseFromString(content);
    // stus1.stus(0);
    cout << "ParseFromString: "
    << stus1.stus(0).id() <<  ", "<< stus1.stus(0).name() << ", "<< stus1.stus(0).score() << "\n"
    << stus1.stus(1).id() <<  ", "<< stus1.stus(1).name() << ", "<< stus1.stus(1).score()
    << endl;

    return 0;

}