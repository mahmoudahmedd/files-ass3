/**
 *  @file    system.h
 *  @author  Mahmoud Ahmed Tawfik (20160227)
 *  @date    14/04/2019
 *  @version 1.0
 *
 *  @Problem Assignment(3)
 */

#pragma once

#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "course.h"

using namespace std;

class System
{
private:
	struct PrimaryIndexRecord
	{
		char PK[6];
		int offset;
	};

	struct SecondaryIndexRecord
	{
		string SK;
		PrimaryIndexRecord offset;
	};

	fstream recordsFile;
	fstream primaryKeysFile;
	fstream secondaryKeysFile;
	string recordsFileName;
	string primaryKeysFileName;
	string secondaryKeysFileName;
	vector<PrimaryIndexRecord> primaryIndexArr;
	vector<SecondaryIndexRecord> secondaryIndexArr;
	bool statusFlag;
	bool state;
	short RRN;
	
	void writeCourse(Course _c)
	{
		int sizeOfRecord = 
			strlen(_c.courseID) + 
			_c.courseName.size() + 
			_c.courseInstructorName.size() + 
			sizeof(_c.courseWeeks) + 
			4;

		this->recordsFile.write((char *)& sizeOfRecord, sizeof(sizeOfRecord));

		this->recordsFile.write((char *)& _c.courseID, strlen(_c.courseID));
		this->recordsFile.write((char *)& "|", 1);

		this->recordsFile.write(_c.courseName.c_str(), _c.courseName.size());
		this->recordsFile.write((char *)& "|", 1);

		this->recordsFile.write(_c.courseInstructorName.c_str(), _c.courseInstructorName.size());
		this->recordsFile.write((char *)& "|", 1);

		this->recordsFile.write((char *)& _c.courseWeeks, sizeof(_c.courseWeeks));
		this->recordsFile.write((char *)& "|", 1);
	}

	void setRRN(short _x)
	{
		this->recordsFile.seekp(1, ios::beg);
		this->recordsFile.write((char *)& _x, sizeof(_x));
	}

	void setStatusFlag(bool _x)
	{
		this->recordsFile.seekp(0, ios::beg);
		this->recordsFile.write((char *)& _x, sizeof(_x));
	}

	short getRRN()
	{
		short s;
		this->recordsFile.seekg(1, ios::beg);
		this->recordsFile.read((char *)& s, sizeof(s));

		return s;
	}

	bool getStatusFlag()
	{
		bool b;
		this->recordsFile.seekg(0, ios::beg);
		this->recordsFile.read((char *)& b, sizeof(b));

		return b;
	}

	int getFileSize()
	{
		this->recordsFile.seekg(0, ios::end);
		int size = this->recordsFile.tellg();
		return size;
	}

	void trunc()
	{
		this->recordsFile.close();
		this->recordsFile.open(this->recordsFileName.c_str(), ios::in | ios::out | ios::trunc);
	}

public:
	System(string _recordsFileName, string _primaryKeysFileName, string _secondaryKeysFileName)
	{
		this->recordsFile.open(_recordsFileName.c_str(), ios::in | ios::out);
		this->primaryKeysFile.open(_primaryKeysFileName.c_str(), ios::in | ios::out);
		this->secondaryKeysFile.open(_secondaryKeysFileName.c_str(), ios::in | ios::out);

		this->recordsFileName = _recordsFileName;
		this->primaryKeysFileName = _primaryKeysFileName;
		this->secondaryKeysFileName = _secondaryKeysFileName;

		if(!this->recordsFile)
		{
			cerr << "System::System(string): ERROR - "<< this->recordsFileName <<" Not Found."  << endl;
			exit(EXIT_FAILURE);
		}

		if(!this->primaryKeysFile)
		{
			cerr << "System::System(string): ERROR - "<< this->primaryKeysFileName <<" Not Found."  << endl;
			exit(EXIT_FAILURE);
		}

		if(!this->secondaryKeysFile)
		{
			cerr << "System::System(string): ERROR - "<< this->secondaryKeysFileName <<" Not Found."  << endl;
			exit(EXIT_FAILURE);
		}
		
		if(getFileSize() == 0)
		{
			this->setStatusFlag(true);
			this->setRRN(-1);
			this->statusFlag = true;
			this->RRN = -1;
		}
		else
		{
			this->statusFlag = this->getStatusFlag();
			this->RRN = this->getRRN();

			if(this->statusFlag)
			{
				// Do something.
			}
		}

		this->state = true;
	}

	void addNewCourse(Course _c)
	{
		if(RRN == -1)
		{
			this->recordsFile.clear();
			this->recordsFile.seekp(0, ios::end);
			
			this->writeCourse(_c);
			/*
			IndexRecord temp;
			strcpy(temp.PK, s.ID);
			temp.offset = addess;
			index[next] = temp;
			next++;
			*/
		}
		else
		{
			char delimiter;
			short t = this->RRN;

			this->recordsFile.seekp(this->RRN, ios::beg);
			this->recordsFile.read(&delimiter, 1);
			this->recordsFile.read((char *)& this->RRN, sizeof(this->RRN));
			this->setRRN(this->RRN);

			this->recordsFile.seekp(t, ios::beg);
			this->writeCourse(_c);
		}
	}

	/*
	void updateBook(char *_isbn)
	{
		Book b;
		Book newBook;
		bool flag = false;
		char temp, delimiter;

		file.clear();
		file.seekg(2, ios::beg);

		while(file.read(&temp, 1))
		{
			if(temp == '*')
			{
				file.seekp(-1, ios::cur);
				file.seekg(127, ios::cur);
			}
			else
			{
				file.seekg(-1, ios::cur);
				
				file.read((char *)& b.isbn, sizeof(b.isbn));
				file.read(&delimiter, 1);
				file.read((char *)& b.title, sizeof(b.title));
				file.read(&delimiter, 1);
				file.read((char *)& b.authorName, sizeof(b.authorName));
				file.read(&delimiter, 1);
				file.read((char *)& b.price, sizeof(b.price));
				file.read(&delimiter, 1);
				file.read((char *)& b.year, sizeof(b.year));
				file.read(&delimiter, 1);
				file.read((char *)& b.numOfPages, sizeof(b.numOfPages));
				file.read(&delimiter, 1);

				if(strcmp(b.isbn, _isbn) == 0)
				{
					flag = true;
					cin >> newBook;
					
					file.seekg(-127, ios::cur);
					writeBook(newBook);

					break;
				}

				if(flag)
					cout << newBook << endl;
			}
		}

		if(!flag)
			cout << endl << "NOT FOUND." << endl << endl;
	} 

	void deleteBook(char * s)
	{
		Book b;
		char d;

		file.clear();
		file.seekg(2);
		
		while(file.read((char *)& b.isbn, sizeof(b.isbn)))
		{
			if(strcmp(b.isbn, s) == 0)
			{
				file.seekp(-5, ios::cur);
				short temp = this->RRN;
				this->RRN = file.tellp() ;

				file.write("*", 1);
				file.write((char *)& temp, sizeof(temp));

				setRRN(this->RRN);

				break;
			}

			file.read(&d, 1);
			file.read((char *)& b.title, sizeof(b.title));
			file.read(&d, 1);
			file.read((char *)& b.authorName, sizeof(b.authorName));
			file.read(&d, 1);
			file.read((char *)& b.price, sizeof(b.price));
			file.read(&d, 1);
			file.read((char *)& b.year,sizeof(b.year));
			file.read(&d, 1);
			file.read((char *)& b.numOfPages,sizeof(b.numOfPages));
			file.read(&d, 1);
		}
	}

	void printBook(char *_cArr, bool _isTitle)
	{
		Book b;
		bool flag = false;
		char temp, delimiter;

		file.clear();
		file.seekg(2, ios::beg);

		while(file.read(&temp, 1))
		{
			if(temp == '*')
			{
				file.seekp(-1, ios::cur);
				file.seekg(127, ios::cur);  // 6 + 121
			}
			else
			{
				file.seekg(-1,ios::cur);
				
				file.read((char *)& b.isbn, sizeof(b.isbn));
				file.read(&delimiter, 1);
				file.read((char *)& b.title, sizeof(b.title));

				if(_isTitle)
				{
					if(strcmp(b.title, _cArr) == 0)
						flag = true;
				}
				else
				{
					if(strcmp(b.isbn, _cArr) == 0)
						flag = true;
				}

				file.read(&delimiter, 1);
				file.read((char *)& b.authorName, sizeof(b.authorName));
				file.read(&delimiter, 1);
				file.read((char *)& b.price, sizeof(b.price));
				file.read(&delimiter, 1);
				file.read((char *)& b.year, sizeof(b.year));
				file.read(&delimiter, 1);
				file.read((char *)& b.numOfPages, sizeof(b.numOfPages));
				file.read(&delimiter, 1);

				if(flag)
					cout << b << endl;
			}
		}

		if(!flag)
			cout << endl << "NOT FOUND." << endl << endl;
	} 

	void printAll()
	{
		Book b;
		file.clear();
		file.seekg(2, ios::beg);
	
		char ax;

		while(file.read(&ax, 1))
		{
			char d;
			
			if(ax == '*')
			{
				file.seekp(-1, ios::cur);
				file.seekg(127, ios::cur);  // 6 + 121
			}
			else
			{
				file.seekg(-1,ios::cur);
	
				file.read((char *)& b.isbn, sizeof(b.isbn));
				file.read(&d, 1);
				file.read((char *)& b.title, sizeof(b.title));
				file.read(&d, 1);
				file.read((char *)& b.authorName, sizeof(b.authorName));
				file.read(&d, 1);
				file.read((char *)& b.price, sizeof(b.price));
				file.read(&d, 1);
				file.read((char *)& b.year, sizeof(b.year));
				file.read(&d, 1);
				file.read((char *)& b.numOfPages, sizeof(b.numOfPages));
				file.read(&d, 1);
				cout << b << endl;
			}
		}
	} 

	void compactTheFile()
	{
		vector<Book> books;
		Book b;
		char temp;

		file.clear();
		file.seekg(2, ios::beg);

		while(file.read(&temp, 1))
		{
			char d;
			
			if(temp == '*')
			{
				file.seekp(-1, ios::cur);
				file.seekg(127, ios::cur);  // 6 + 121
			}
			else
			{
				file.seekg(-1,ios::cur);
	
				file.read((char *)& b.isbn, sizeof(b.isbn));
				file.read(&d, 1);
				file.read((char *)& b.title, sizeof(b.title));
				file.read(&d, 1);
				file.read((char *)& b.authorName, sizeof(b.authorName));
				file.read(&d, 1);
				file.read((char *)& b.price, sizeof(b.price));
				file.read(&d, 1);
				file.read((char *)& b.year, sizeof(b.year));
				file.read(&d, 1);
				file.read((char *)& b.numOfPages, sizeof(b.numOfPages));
				file.read(&d, 1);
				
				books.push_back(b);
			}
		}

		trunc();
		RRN = -1;

		file.clear();
		file.seekp(0, ios::beg);
		file.write((char *)& this->RRN, sizeof(this->RRN));
		

		for(unsigned  int i = 0 ; i < books.size() ; i++)
			writeBook(books[i]);
	}

	

	void setRRN(short x = -1)
	{
		file.seekp(0, ios::beg);
		file.write((char *)& x, sizeof(x));

		getRRN();
	}

	int getRRN()
	{
		file.clear();
		file.seekg(0, ios::beg);
		file.read((char *)& this->RRN, sizeof(this->RRN));

		return RRN;
	}
	*/
	bool isOpen()
	{
		return this->state;
	}

	void printMenu()
	{
        cout << "1) Add New course  \n"
				"2) Delete course (ID)\n"
				"3) Delete course (instructor  name)\n"
				"4) Print course (ID)\n"
				"5) Print course (instructor  name)\n"
				"6) update course (ID)\n"
				"7) update course (instructor  name)\n"
				"8) Exit\n\n\n";

		cout << "RRN = " << RRN << endl << endl;

		cout << "Please Enter Your Choice: ";
	}

	void printBye()
	{
        cout << "Bye..." << endl;
	}

	void printDone()
	{
        cout << "Done!" << endl << endl;
	}

	void printError()
	{
        cout << "ERROR." << endl << endl;
	}

	void doIt(char _choice)
	{
		switch(_choice)
		{
			case '1':
			{
				break;
			}
			case '2':
			{
				break;
			}
			case '3':
			{
				break;
			}
			case '4':
			{
				break;
			}
			case '5':
			{
				break;
			}
			case '6':
			{
				break;
			}
			case '7':
			{
				break;
			}
			case '8':
			{
				this->state = false;
				break;
			}
			default:
			{
				cin.ignore();
				cin.clear();
				this->printError();
				break;
			}
				
		}
	}

	~System()
	{
		this->recordsFile.close();
		this->primaryKeysFile.close();
		this->secondaryKeysFile.close();

		this->setStatusFlag(0);
	}
};

#endif // SYSTEM_H_INCLUDED