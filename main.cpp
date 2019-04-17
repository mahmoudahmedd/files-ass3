#include <iostream>
#include "system.h"


using namespace std;

int main()
{
	System s("records.txt", "primary_keys.txt", "secondary_keys.txt");
	Course c;
	cin>>c;
	cout<<c<<endl;
	s.addNewCourse(c);
    return 0;
}
