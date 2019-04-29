#include <iostream>
#include "system.h"


using namespace std;

int main()
{
	char choice;
	System s("records.txt", "primary_keys.txt", "secondary_keys.txt", "sec_inverted_list.txt");

	/*Course c1("21","mahmcccccccoud qq","sdfd",9999);
	Course c2("4d74","daaaccc xc","xcvs",32654);

	s.addNewCourse(c1);
	s.addNewCourse(c2);
	
	s.printAll();
	*/

	while(s.isOpen())
	{
		s.printMenu();
		cin >> choice;
		system("CLS");
		s.doIt(choice);
	}
	s.printBye();

    return 0;
}
