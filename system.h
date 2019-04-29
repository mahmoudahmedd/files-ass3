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
	struct PrimaryKey
	{
		char courseID[6];
		int  offset;
	};

	struct SecondaryKey
	{
		string courseInstructorName;
		int indexList;
	};

	struct SecondaryKeyList
	{
		char courseID[6];
		int  next;
	};

	fstream recordsFile;
	fstream primaryKeysFile;
	fstream secondaryKeysFile;
	fstream secondaryKeysListFile;

	string recordsFileName;
	string primaryKeysFileName;
	string secondaryKeysFileName;
	string secondaryAvilList;

	vector<PrimaryKey>       primaryKeyArr;
	vector<SecondaryKey>     secondaryKeyArr;
	vector<SecondaryKeyList> secondaryKeyListArr;

	bool statusFlag;
	bool state;
	short RRN;
	
	int primaryIndexBinarySearch(char *key)
	{
		int low = 0, high = primaryKeyArr.size() - 1;

		while (low <= high)
		{
			int middle = (low + high) / 2;

			if (strcmp(primaryKeyArr[middle].courseID, key) == 0)
				return middle;
			else if (atoi(primaryKeyArr[middle].courseID) < atoi(key))
				low = middle + 1;
			else
				high = middle - 1;
		}
		return -1;
	}

	int secondaryIndexBinarySearch(string _courseInstructorName)
	{
		int low = 0, high = secondaryKeyArr.size() - 1;

		while (low <= high)
		{
			int middle = (low + high) / 2;

			if (secondaryKeyArr[middle].courseInstructorName == _courseInstructorName)
				return middle;
			else if (secondaryKeyArr[middle].courseInstructorName < _courseInstructorName)
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
		sort(this->primaryKeyArr.begin(), this->primaryKeyArr.end(), 
			[](PrimaryKey _a, PrimaryKey _b) 
			{
				return strcmp(_a.courseID,_b.courseID) == -1 ;
			}
		);
	}

	void sortSecondaryIndices()
	{
		sort(this->secondaryKeyArr.begin(), this->secondaryKeyArr.end(), 
			[](SecondaryKey _a, SecondaryKey _b) 
			{
				return _a.courseInstructorName < _b.courseInstructorName;
			}
		);
	}

	void savePrimaryIndices()
	{
		this->primaryKeysFile.clear();
		this->primaryKeysFile.seekp(0, ios::beg);

		for(unsigned int i = 0;i < this->primaryKeyArr.size();i++)
		{
			this->primaryKeysFile.write((char*) & this->primaryKeyArr[i], sizeof(this->primaryKeyArr[i]));
		}
	}

	void saveSecondaryIndices()
	{
		this->secondaryKeysFile.clear();
		this->secondaryKeysFile.seekp(0, ios::beg);

		for(unsigned int i = 0;i < this->secondaryKeyArr.size();i++)
		{
			char buffer [200];
			strcpy_s(buffer, this->secondaryKeyArr[i].courseInstructorName.c_str());
			strcat_s(buffer, "|");

			string t = to_string((long long) this->secondaryKeyArr[i].indexList);
			strcat_s(buffer, t.c_str());
			strcat_s(buffer, "|");

			int len = strlen(buffer);

			this->secondaryKeysFile.write((char*) &len, sizeof(int));
			this->secondaryKeysFile.write((char*) &buffer, len);
		}
	}

	void saveSecondaryListIndices()
	{
		this->secondaryKeysListFile.clear();
		this->secondaryKeysListFile.seekp(0, ios::beg);

		for(unsigned int i = 0;i < this->secondaryKeyListArr.size();i++)
		{
			this->secondaryKeysListFile.write((char*) & this->secondaryKeyListArr[i], sizeof(this->secondaryKeyListArr[i]));
		}
	}

	void loadPrimaryIndices()
	{
		if(this->statusFlag)
		{
			this->recordsFile.clear();
			this->recordsFile.seekp(3, ios::beg);

			int length;
			while(this->recordsFile.read((char*)&length,sizeof(length)))
			{
				int offset = (int) this->recordsFile.tellg();

				Course obj;
				obj.courseName = "";
				obj.courseInstructorName = "";

				char *buffer;
				buffer = new char[length];
				this->recordsFile.read(buffer,length);

				if (buffer[0] == '*')
					continue;

				istringstream stream(buffer);

				char courseWeeksTemp[30];

				stream.getline(obj.courseID, 6, '|');
				getline(stream, obj.courseName,'|');
				getline(stream, obj.courseInstructorName,'|');
				stream.getline(courseWeeksTemp,30,'|');
				stringstream toShort(courseWeeksTemp); 
				toShort >> obj.courseWeeks;


				PrimaryKey temp;
				strcpy_s(temp.courseID, obj.courseID);
				temp.offset = offset - 4;
				this->primaryKeyArr.push_back(temp);

				delete [] buffer;
			}
		}
		else
		{
			PrimaryKey temp;
			this->primaryKeysFile.clear();
			this->primaryKeysFile.seekg(0, ios::beg);

			while(this->primaryKeysFile.read((char*) &temp, sizeof(temp)))
				this->primaryKeyArr.push_back(temp);
		}

		sortPrimaryIndices();
	}

	void loadSecondaryIndices()
	{
		if(this->statusFlag)
		{
			this->recordsFile.clear();
			this->recordsFile.seekp(3, ios::beg);

			int length;
			while(this->recordsFile.read((char*)&length,sizeof(length)))
			{
				Course obj;
				obj.courseName = "";
				obj.courseInstructorName = "";

				char *buffer;
				buffer = new char[length];
				this->recordsFile.read(buffer,length);

				if (buffer[0] == '*')
					continue;

				istringstream stream(buffer);

				char courseWeeksTemp[30];

				stream.getline(obj.courseID, 6, '|');
				getline(stream, obj.courseName,'|');
				getline(stream, obj.courseInstructorName,'|');
				stream.getline(courseWeeksTemp,30,'|');
				stringstream toShort(courseWeeksTemp); 
				toShort >> obj.courseWeeks;


				SecondaryKey sk;
				SecondaryKeyList skList;

				// Add SecondaryKey to vector
				int i = secondaryIndexBinarySearch(obj.courseInstructorName);

				if (i == -1)
				{
					sk.courseInstructorName = obj.courseInstructorName;
					sk.indexList = secondaryKeyListArr.size();
					secondaryKeyArr.push_back(sk);

					strcpy_s(skList.courseID, obj.courseID);
					skList.next = -1;
					secondaryKeyListArr.push_back(skList);
				}
				else
				{
					int indexList = secondaryKeyArr[i].indexList;

					while(true)
					{
						if(secondaryKeyListArr[indexList].next == -1)
						{
							secondaryKeyListArr[indexList].next = secondaryKeyListArr.size() ;
							break;
						}
						else
						{
							indexList = secondaryKeyListArr[indexList].next;
						}
					}

					strcpy_s(skList.courseID, obj.courseID);
					skList.next = -1;
					secondaryKeyListArr.push_back(skList);
				}
				

				delete [] buffer;
			}
			
		}
		else
		{
			this->secondaryKeysFile.clear();
			this->secondaryKeysFile.seekg(0, ios::beg);

			int length;
			while(this->secondaryKeysFile.read((char*)&length, sizeof(length)))
			{
				SecondaryKey temp;
				temp.courseInstructorName = "";

				char *buffer;
				buffer = new char[length];
				this->secondaryKeysFile.read(buffer, length);
				istringstream stream(buffer);

				char offsetListTemp[30];

				getline(stream, temp.courseInstructorName, '|');
				stream.getline(offsetListTemp,30,'|');

				stringstream toInt(offsetListTemp);
				toInt >> temp.indexList;

				this->secondaryKeyArr.push_back(temp);
			}
		}

		this->sortSecondaryIndices();
	}

	void loadSecondaryListIndices()
	{
		SecondaryKeyList temp;

		this->secondaryKeysListFile.clear();
		this->secondaryKeysListFile.seekg(0, ios::beg);

		while(this->secondaryKeysListFile.read((char*) &temp, sizeof(temp)))
			this->secondaryKeyListArr.push_back(temp);
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
		int size = (int) this->recordsFile.tellg();
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
		this->recordsFile.open(_recordsFileName.c_str(), ios::in | ios::out | ios::binary);
		this->primaryKeysFile.open(_primaryKeysFileName.c_str(), ios::in | ios::out | ios::binary);
		this->secondaryKeysFile.open(_secondaryKeysFileName.c_str(), ios::in | ios::out | ios::binary);
		this->secondaryKeysListFile.open(_secondaryAvilList.c_str(), ios::in | ios::out | ios::binary);

		this->recordsFileName       = _recordsFileName;
		this->primaryKeysFileName   = _primaryKeysFileName;
		this->secondaryKeysFileName = _secondaryKeysFileName;
		this->secondaryAvilList     = _secondaryAvilList;

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

		if(!this->secondaryKeysListFile)
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
			this->RRN        = this->getRRN();

			// keys into vectors
			this->loadPrimaryIndices();
			this->loadSecondaryIndices();
			this->loadSecondaryListIndices();

			this->statusFlag = true;
		}
		
		this->state = true;

		cout << "Status Flag = "  << this->statusFlag << endl << endl << endl;
	}

	void printAll()
	{
		this->recordsFile.clear();
		this->recordsFile.seekp(3, ios::beg);

		int length;
		while(this->recordsFile.read((char*)&length,sizeof(length)))
		{
			Course obj;
			obj.courseName = "";
			obj.courseInstructorName = "";

			char *buffer;
			buffer = new char[length];
			this->recordsFile.read(buffer,length);

			if (buffer[0] == '*')
			{
				cout << "deleted record !!" << endl << endl;
				continue;
			}

			istringstream stream(buffer);

			char courseWeeksTemp[30];

			stream.getline(obj.courseID, 6, '|');
			getline(stream, obj.courseName,'|');
			getline(stream, obj.courseInstructorName,'|');
			stream.getline(courseWeeksTemp,30,'|');
			stringstream toShort(courseWeeksTemp); 
			toShort >> obj.courseWeeks;

			delete [] buffer;

			cout << obj << endl;
		}
	}

	void printKeys()
	{
		cout << "------------------primaryKey-----------------------" << endl;

		for(unsigned int i = 0;i < this->primaryKeyArr.size();i++)
		{
			cout << this->primaryKeyArr[i].courseID << " " << this->primaryKeyArr[i].offset << endl;
		}

		cout << "------------------secondaryKey---------------------" << endl;

		for(unsigned int i = 0;i < this->secondaryKeyArr.size();i++)
		{
			cout << this->secondaryKeyArr[i].courseInstructorName << " " << this->secondaryKeyArr[i].indexList << endl;
		}

		cout << "-----------------secondaryKeyListArr---------------" << endl;

		for(unsigned int i = 0;i < this->secondaryKeyListArr.size();i++)
		{
			cout << this->secondaryKeyListArr[i].courseID << " " << this->secondaryKeyListArr[i].next << endl;
		}
	}

	void addNewCourse(Course _c)
	{
		//this->recordsFile.close();

		this->recordsFile.clear();
		this->recordsFile.seekp(0, ios::end);

		int offset = (int) this->recordsFile.tellg();

		char buffer [200];
		strcpy_s(buffer, _c.courseID);
		strcat_s(buffer, "|");
		strcat_s(buffer, _c.courseName.c_str());
		strcat_s(buffer, "|");
		strcat_s(buffer, _c.courseInstructorName.c_str());
		strcat_s(buffer, "|");

		string t = to_string((long long) _c.courseWeeks);

		strcat_s(buffer, t.c_str());
		strcat_s(buffer, "|");

		int len = strlen(buffer);

		this->recordsFile.write((char*) &len, sizeof(int));
		this->recordsFile.write((char*) &buffer, len);

		// Add PrimaryKey to vector
		PrimaryKey temp;
		strcpy_s(temp.courseID, _c.courseID);
		temp.offset = offset;
		this->primaryKeyArr.push_back(temp);
		this->sortPrimaryIndices();

		// Add SecondaryKey to vector
		int i = secondaryIndexBinarySearch(_c.courseInstructorName);

		SecondaryKey sk;
		SecondaryKeyList skList;

		if (i == -1)
		{
			sk.courseInstructorName = _c.courseInstructorName;
			sk.indexList = secondaryKeyListArr.size();
			secondaryKeyArr.push_back(sk);

			strcpy_s(skList.courseID, _c.courseID);
			skList.next = -1;
			secondaryKeyListArr.push_back(skList);

			this->sortSecondaryIndices();
		}
		else
		{
			int indexList = secondaryKeyArr[i].indexList;

			while(true)
			{
				if(secondaryKeyListArr[indexList].next == -1)
				{
					secondaryKeyListArr[indexList].next = secondaryKeyListArr.size() ;
					break;
				}
				else
				{
					indexList = secondaryKeyListArr[indexList].next;
				}
			}

			strcpy_s(skList.courseID, _c.courseID);
			skList.next = -1;
			secondaryKeyListArr.push_back(skList);
		}
	}

	bool deleteCourseID(char *PK)
	{
		int index = this->primaryIndexBinarySearch(PK);

		if (index == -1)
			return false;

		// delete from file
		this->recordsFile.clear();
		this->recordsFile.seekp(primaryKeyArr[index].offset + 4, ios::beg); // skip len
		this->recordsFile.write((char *) &"*",1);

		// delete from vector
		primaryKeyArr.erase(primaryKeyArr.begin() + index);

		return true;
	}

	void deleteInstructorName()
	{
		string name; 
		cout << "Enter name to Name: ";  cin >> name;

		int index = this->secondaryIndexBinarySearch(name);

		if (index == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		int indexList = secondaryKeyArr[index].indexList;

		while(true)
		{
			if(secondaryKeyListArr[indexList].next == -1)
			{
				deleteCourseID(secondaryKeyListArr[indexList].courseID);

				break;
			}
			else
			{
				deleteCourseID(secondaryKeyListArr[indexList].courseID);
				indexList = secondaryKeyListArr[indexList].next;
			}
		}
		
		// delete from vector
		secondaryKeyArr.erase(secondaryKeyArr.begin() + index);
	}

	void printCourseID(char *PK)
	{
		Course obj;
		obj.courseName = "";
		obj.courseInstructorName = "";

		int index = this->primaryIndexBinarySearch(PK);

		if (index == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		this->recordsFile.clear();
		this->recordsFile.seekg(primaryKeyArr[index].offset, ios::beg);

		int length;
		this->recordsFile.read((char *) &length, sizeof(length));

		char *buffer;
		buffer = new char[length];
		this->recordsFile.read(buffer,length);

		istringstream stream(buffer);

		char courseWeeksTemp[30];

		stream.getline(obj.courseID, 6, '|');
		getline(stream, obj.courseName,'|');
		getline(stream, obj.courseInstructorName,'|');
		stream.getline(courseWeeksTemp,30,'|');
		stringstream toShort(courseWeeksTemp); 
		toShort >> obj.courseWeeks;

		cout << obj << endl;

		delete [] buffer;
	}

	void printCourseName()
	{
		string name; 
		cout << "Enter name to print: ";  cin >> name;

		int index = this->secondaryIndexBinarySearch(name);

		if (index == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		int indexList = secondaryKeyArr[index].indexList;

		while(true)
		{
			if(secondaryKeyListArr[indexList].next == -1)
			{
				printCourseID(secondaryKeyListArr[indexList].courseID);
				break;
			}
			else
			{
				printCourseID(secondaryKeyListArr[indexList].courseID);
				indexList = secondaryKeyListArr[indexList].next;
			}
		}
	}

	bool updateCourseID(char *PK)
	{
		if(!deleteCourseID(PK))
			return false;

		Course c;
		cin >> c;
		addNewCourse(c);
		
		return true;
	}

	void updateCourseName()
	{
		string name; 
		cout << "Enter name to update: ";  cin >> name;

		int index = this->secondaryIndexBinarySearch(name);

		if (index == -1)
		{
			cout << "Course not found.\n";
			return;
		}

		int indexList = secondaryKeyArr[index].indexList;

		while(true)
		{
			if(secondaryKeyListArr[indexList].next == -1)
			{
				updateCourseID(secondaryKeyListArr[indexList].courseID);
				break;
			}
			else
			{
				updateCourseID(secondaryKeyListArr[indexList].courseID);
				indexList = secondaryKeyListArr[indexList].next;
			}
		}
		
		// delete from vector
		secondaryKeyArr.erase(secondaryKeyArr.begin() + index);
	}

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
				"8) Print keys\n"
				"9) Print all\n"
				"0) Exit\n\n\n";

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
			case '0':
			{
				this->state = false;
				break;
			}
			case '1':
			{
				Course c1("1111", "Mahmoud Ahmed", "fci sadsa sas", 5);
				//Course c2("1112", "Hemai", "adf asa sd", 2000);
				cin>>c1;
				this->addNewCourse(c1);
				//this->addNewCourse(c2);
				break;
			}
			case '2':
			{
				char PK[6]; 
				cout << "Enter PK to delete: ";  cin >> PK;

				if (this->deleteCourseID(PK))
				{
					cout << "DONE!\n";	
				}
				else
				{
					cout << "Course not found.\n";	
				}
				
				break;
			}
			case '3':
			{
				this->deleteInstructorName();
				break;
			}
			case '4':
			{
				char PK[6]; 
				cout << "Enter PK to print: ";  cin >> PK;
				this->printCourseID(PK);
				break;
			}
			case '5':
			{
				this->printCourseName();
				break;
			}
			case '6':
			{
				char PK[6]; 
				cout << "Enter PK to update: ";  cin >> PK;

				if(this->updateCourseID(PK))
					cout << "DONE!" << endl;
				else
					cout << "Course not found " << endl;
					
				break;
			}
			case '7':
			{
				this->updateCourseName();
				break;
			}
			case '8':
			{
				this->printKeys();
				break;
			}
			case '9':
			{
				this->printAll();
				break;
			}
			case 't':
			{
				trunc(this->recordsFile, this->recordsFileName);
				trunc(this->primaryKeysFile, this->primaryKeysFileName);
				trunc(this->secondaryKeysFile, this->secondaryKeysFileName);
				trunc(this->secondaryKeysListFile, this->secondaryAvilList);
				exit(0);
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
		trunc(this->secondaryKeysListFile, this->secondaryAvilList);

		this->savePrimaryIndices();
		this->saveSecondaryIndices();
		this->saveSecondaryListIndices();
		this->setStatusFlag(false);

		this->recordsFile.close();
		this->primaryKeysFile.close();
		this->secondaryKeysFile.close();
		this->secondaryKeysListFile.close();
	}
};

#endif // SYSTEM_H_INCLUDED