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
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
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
		string courseInstructorName;
		int offset;
	};

	fstream recordsFile;
	fstream primaryKeysFile;
	fstream secondaryKeysFile;
	fstream secondaryAvilListFile;
	string recordsFileName;
	string primaryKeysFileName;
	string secondaryKeysFileName;
	string secondaryAvilList;
	vector<PrimaryIndexRecord> primaryIndexArr;
	vector<SecondaryIndexRecord> secondaryIndexArr;
	bool statusFlag;
	bool state;
	short RRN;
	
	int primaryIndexBinarySearch(char *key)
	{
		int low = 0, high = primaryIndexArr.size() - 1;

		while (low <= high)
		{
			int middle = (low + high) / 2;

			if (strcmp(primaryIndexArr[middle].PK, key) == 0)
				return middle;
			else if (atoi(primaryIndexArr[middle].PK) < atoi(key))
				low = middle + 1;
			else
				high = middle - 1;
		}
		return -1;
	}

	int secondaryIndexBinarySearch(string _courseInstructorName)
	{
		int low = 0, high = secondaryIndexArr.size() - 1;

		while (low <= high)
		{
			int middle = (low + high) / 2;

			if (secondaryIndexArr[middle].courseInstructorName == _courseInstructorName)
				return middle;
			else if (secondaryIndexArr[middle].courseInstructorName < _courseInstructorName)
				low = middle + 1;
			else
				high = middle - 1;
		}
		return -1;
	}

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
	
	}

	void sortPrimaryIndices()
	{
		sort(this->primaryIndexArr.begin(), this->primaryIndexArr.end(), 
			[](PrimaryIndexRecord _a, PrimaryIndexRecord _b) 
			{
				return strcmp(_a.PK,_b.PK) != 1 ;
			}
		);
	}

	void sortSecondaryIndices()
	{
		sort(this->secondaryIndexArr.begin(), this->secondaryIndexArr.end(), 
			[](SecondaryIndexRecord _a, SecondaryIndexRecord _b) 
			{
				return _a.courseInstructorName < _b.courseInstructorName;
			}
		);
	}

	void savePrimaryIndices()
	{
		this->primaryKeysFile.clear();
		this->primaryKeysFile.seekp(0, ios::beg);

		for(unsigned int i = 0;i < this->primaryIndexArr.size();i++)
		{
			cout<<this->primaryIndexArr[i].PK<< " "<< this->primaryIndexArr[i].offset<<endl;
			this->primaryKeysFile.write((char*) & this->primaryIndexArr[i], sizeof(this->primaryIndexArr[i]));
		}
	}

	void saveSecondaryIndices()
	{
		this->secondaryKeysFile.clear();
		this->secondaryKeysFile.seekp(0, ios::beg);

		for(unsigned int i = 0;i < this->secondaryIndexArr.size();i++)
		{
			cout<<this->secondaryIndexArr[i].courseInstructorName<<" "<< this->secondaryIndexArr[i].offset<<endl;
			this->secondaryKeysFile.write((char*) & this->secondaryIndexArr[i], sizeof(this->secondaryIndexArr[i]));
		}
	}

	void loadPrimaryIndices()
	{
		if(this->statusFlag && getFileSize() != 3)
		{
			this->recordsFile.clear();
			this->recordsFile.seekg(3, ios::beg); //skip header

			char temp;
			int len;

			while(this->recordsFile.read((char*) &len, sizeof(len)))
			{
				this->recordsFile.read(&temp, 1);
				if(temp == '*')
				{
					this->recordsFile.seekg(len - 1, ios::cur);
				}
				else
				{
					stringstream stream;
					PrimaryIndexRecord temp;
					Course s;

					this->recordsFile.seekg(-1,ios::cur);
					int offset = this->recordsFile.tellg();
					
					char *buffer = new char[len];
					
					this->recordsFile.read(buffer, len);

					stream.write(buffer, len);
					stream.getline(s.courseID, 6, '|');
					getline(stream,s.courseName,'|');
					getline(stream,s.courseInstructorName,'|');
					stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

					strcpy(temp.PK, s.courseID);
					temp.offset = offset - 4;
					this->primaryIndexArr.push_back(temp);

					delete [] buffer;
				}
			}
		}
		else
		{
			PrimaryIndexRecord temp;
			this->primaryKeysFile.clear();
			this->primaryKeysFile.seekg(0, ios::beg);

			while(this->primaryKeysFile.read((char*) &temp, sizeof(temp)))
				this->primaryIndexArr.push_back(temp);
		}

		sortPrimaryIndices();
	}

	void loadSecondaryIndices()
	{
		if(this->statusFlag && getFileSize() != 3)
		{
			this->recordsFile.clear();
			this->recordsFile.seekg(3, ios::beg); //skip header

			char temp;
			int len;

			while(this->recordsFile.read((char*) &len, sizeof(len)))
			{
				this->recordsFile.read(&temp, 1);
				if(temp == '*')
				{
					this->recordsFile.seekg(len - 1, ios::cur);
				}
				else
				{
					stringstream stream;
					SecondaryIndexRecord temp;
					Course s;

					this->recordsFile.seekg(-1,ios::cur);
					
					
					char *buffer = new char[len];
					
					this->recordsFile.read(buffer, len);

					stream.write(buffer, len);
					stream.getline(s.courseID, 6, '|');
					getline(stream,s.courseName,'|');
					getline(stream,s.courseInstructorName,'|');
					stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

					temp.courseInstructorName = s.courseInstructorName;
					//temp.offset = s.courseID;


					this->secondaryIndexArr.push_back(temp);

					delete [] buffer;
				}
			}
		}
		else
		{
			SecondaryIndexRecord temp;
			this->secondaryKeysFile.clear();
			this->secondaryKeysFile.seekg(0, ios::beg);

			while(this->secondaryKeysFile.read((char*) &temp, sizeof(temp)))
				this->secondaryIndexArr.push_back(temp);
		}

		sortSecondaryIndices();
	}

	void setRRN(short _x)
	{
		this->recordsFile.seekp(1, ios::beg);
		this->recordsFile.write((char *)& _x, sizeof(_x));
	}

	void setStatusFlag(bool _x)
	{
		this->recordsFile.clear();
		this->recordsFile.seekp(0, ios::beg);
		this->recordsFile.write((char *)& _x, sizeof(_x));
	}

	short getRRN()
	{
		short s;

		this->recordsFile.clear();
		this->recordsFile.seekg(1, ios::beg);
		this->recordsFile.read((char *)& s, sizeof(s));

		return s;
	}

	bool getStatusFlag()
	{
		bool b;

		this->recordsFile.clear();
		this->recordsFile.seekg(0, ios::beg);
		this->recordsFile.read((char *)& b, sizeof(b));

		return b;
	}

	int getFileSize()
	{
		this->recordsFile.clear();
		this->recordsFile.seekg(0, ios::end);
		int size = this->recordsFile.tellg();
		return size;
	}

	void trunc(fstream &_file, string _fileName)
	{
		_file.close();
		_file.open(_fileName.c_str(), ios::in | ios::out | ios::trunc);
	}

public:
	System(string _recordsFileName, string _primaryKeysFileName, string _secondaryKeysFileName, string _secondaryAvilList)
	{
		this->recordsFile.open(_recordsFileName.c_str(), ios::in | ios::out);
		this->primaryKeysFile.open(_primaryKeysFileName.c_str(), ios::in | ios::out);
		this->secondaryKeysFile.open(_secondaryKeysFileName.c_str(), ios::in | ios::out);
		this->secondaryAvilListFile.open(_secondaryAvilList.c_str(), ios::in | ios::out);

		this->recordsFileName = _recordsFileName;
		this->primaryKeysFileName = _primaryKeysFileName;
		this->secondaryKeysFileName = _secondaryKeysFileName;
		this->secondaryAvilList = _secondaryAvilList;

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

		if(!this->secondaryAvilListFile)
		{
			cerr << "System::System(string): ERROR - "<< this->secondaryAvilList <<" Not Found."  << endl;
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
			this->loadPrimaryIndices();
			this->loadSecondaryIndices();
			this->statusFlag = true;
		}
		
		this->state = true;

		cout << "Status Flag = " << this->statusFlag << endl;
		cout << "RRN = " << this->RRN << endl;
	}

	void addNewCourse(Course _c)
	{
		this->recordsFile.clear();
		this->recordsFile.seekp(0, ios::end);

		this->secondaryAvilListFile.clear();
		this->secondaryAvilListFile.seekp(0, ios::end);

		int offset = this->secondaryAvilListFile.tellp();
		int addess = this->recordsFile.tellp();

		this->writeCourse(_c);

		PrimaryIndexRecord temp1;
		strcpy(temp1.PK, _c.courseID);
		temp1.offset = addess;
		primaryIndexArr.push_back(temp1);
		sortPrimaryIndices();

		int address = this->secondaryIndexBinarySearch(_c.courseInstructorName);

		if (address == -1)
		{
			SecondaryIndexRecord temp2;
			temp2.courseInstructorName = _c.courseInstructorName;
			temp2.offset = offset;
		
			secondaryIndexArr.push_back(temp2);
		}
		
		
		sortSecondaryIndices();
	}

	void deleteCourseID()
	{
		char PK[6]; 
		cout << "Enter PK to delete: ";  cin.getline(PK,6);

		int address = this->primaryIndexBinarySearch(PK);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		this->recordsFile.clear();
		this->recordsFile.seekp(primaryIndexArr[address].offset + 4, ios::beg);
		this->recordsFile.write((char *) &"*",1);

		primaryIndexArr.erase(primaryIndexArr.begin() + address);
	}

	void printCourseID()
	{
		this->recordsFile.close();
		this->recordsFile.open(this->recordsFileName.c_str(), ios::in | ios::out);

		this->recordsFile.clear();
		this->recordsFile.seekg(3, ios::beg); //skip header

		char PK[6]; 
		
		cout << "Enter Course ID: ";  cin >> PK;

		int address = this->primaryIndexBinarySearch(PK);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		stringstream stream;
		Course s;
		int len;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);
		this->recordsFile.read((char*) &len, sizeof(len));

		char *buffer = new char[len];
					
		this->recordsFile.read(buffer, len);

		stream.write(buffer, len);
		stream.getline(s.courseID, 6, '|');
		getline(stream,s.courseName,'|');
		getline(stream,s.courseInstructorName,'|');
		stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

		cout << s << endl;

		delete [] buffer;
	}

	void updateCourseID()
	{
		this->recordsFile.close();
		this->recordsFile.open(this->recordsFileName.c_str(), ios::in | ios::out);

		this->recordsFile.clear();
		this->recordsFile.seekg(3, ios::beg); //skip header

		char PK[6]; 
		
		cout << "Enter PK to delete: ";  cin >> PK;

		int address = this->primaryIndexBinarySearch(PK);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		stringstream stream;
		Course s;
		int len;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);
		this->recordsFile.read((char*) &len, sizeof(len));

		char *buffer = new char[len];
					
		this->recordsFile.read(buffer, len);

		stream.write(buffer, len);
		stream.getline(s.courseID, 6, '|');
		getline(stream,s.courseName,'|');
		getline(stream,s.courseInstructorName,'|');
		stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

		cin >> s;

		cout << s << endl;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);

		delete [] buffer;
	}

	void deleteCourseName()
	{
		string name; 
		cout << "Enter PK to Name: ";  cin>>name;

		int address = this->secondaryIndexBinarySearch(name);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		this->recordsFile.clear();
		this->recordsFile.seekp(secondaryIndexArr[address].offset + 4, ios::beg);
		this->recordsFile.write((char *) &"*",1);

		secondaryIndexArr.erase(secondaryIndexArr.begin() + address);
	}

	void printCourseName()
	{
		this->recordsFile.close();
		this->recordsFile.open(this->recordsFileName.c_str(), ios::in | ios::out);

		this->recordsFile.clear();
		this->recordsFile.seekg(3, ios::beg); //skip header

		char PK[6]; 
		
		cout << "Enter Course ID: ";  cin >> PK;

		int address = this->primaryIndexBinarySearch(PK);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		stringstream stream;
		Course s;
		int len;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);
		this->recordsFile.read((char*) &len, sizeof(len));

		char *buffer = new char[len];
					
		this->recordsFile.read(buffer, len);

		stream.write(buffer, len);
		stream.getline(s.courseID, 6, '|');
		getline(stream,s.courseName,'|');
		getline(stream,s.courseInstructorName,'|');
		stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

		cout << s << endl;

		delete [] buffer;
	}

	void updateCourseName()
	{
		this->recordsFile.close();
		this->recordsFile.open(this->recordsFileName.c_str(), ios::in | ios::out);

		this->recordsFile.clear();
		this->recordsFile.seekg(3, ios::beg); //skip header

		char PK[6]; 
		
		cout << "Enter PK to delete: ";  cin >> PK;

		int address = this->primaryIndexBinarySearch(PK);

		if (address == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		stringstream stream;
		Course s;
		int len;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);
		this->recordsFile.read((char*) &len, sizeof(len));

		char *buffer = new char[len];
					
		this->recordsFile.read(buffer, len);

		stream.write(buffer, len);
		stream.getline(s.courseID, 6, '|');
		getline(stream,s.courseName,'|');
		getline(stream,s.courseInstructorName,'|');
		stream.read((char *) &s.courseWeeks,sizeof(s.courseWeeks));

		cin >> s;

		cout << s << endl;

		this->recordsFile.seekp(primaryIndexArr[address].offset, ios::beg);

		delete [] buffer;
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

		cout << "RRN = " << this->RRN << endl << endl;

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
				Course c;
				cin>>c;
				this->addNewCourse(c);
				break;
			}
			case '2':
			{
				this->deleteCourseID();
				break;
			}
			case '3':
			{
				this->deleteCourseName();
				break;
			}
			case '4':
			{
				this->printCourseID();
				break;
			}
			case '5':
			{
				this->printCourseName();
				break;
			}
			case '6':
			{
				this->updateCourseID();
				break;
			}
			case '7':
			{
				this->updateCourseName();
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
		trunc(this->primaryKeysFile, this->primaryKeysFileName);
		trunc(this->secondaryKeysFile, this->secondaryKeysFileName);

		this->savePrimaryIndices();
		this->saveSecondaryIndices();
		this->setStatusFlag(false);

		this->recordsFile.close();
		this->primaryKeysFile.close();
		this->secondaryKeysFile.close();
	}
};

#endif // SYSTEM_H_INCLUDED