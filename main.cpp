#include <iostream>
#include "system.h"

using namespace std;

int main()
{
	char choice;
	System s("records.txt", "primary_keys.txt", "secondary_keys.txt", "sec_avail_list.txt");

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
