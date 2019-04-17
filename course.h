/**
 *  @file    course.h
 *  @author  Mahmoud Ahmed Tawfik (20160227)
 *  @date    14/04/2019
 *  @version 1.0
 *
 *  @Problem Assignment(3)
 *
 */

#pragma once

#ifndef COURSE_H_INCLUDED
#define COURSE_H_INCLUDED

#include <iostream>
#include <string>

using namespace std;

class Course
{
public:
	char courseID[6];
	string courseName;
	string courseInstructorName;
	short courseWeeks;

	Course(){}

	Course(char *_courseID, string _courseName, string _courseInstructorName, short _courseWeeks)
	{
		strcpy_s(courseID, _courseID);
		courseName = _courseName;
		courseInstructorName = _courseInstructorName;
		courseWeeks = _courseWeeks;
	}

	friend istream& operator >>(istream &_in, Course &_course)
	{
		cout << "Enter COURSE ID: ";		       _in.getline(_course.courseID, 6, '\n');
		cout << "Enter COURSE NAME: ";		       getline(cin, _course.courseName);
		cout << "Enter COURSE INSTRUCTOR NAME: ";  getline(cin, _course.courseInstructorName);
		cout << "Enter COURSE WEEKS: "; 		   _in >> _course.courseWeeks;

		return _in;
	}

	friend ostream& operator <<(ostream &_out, Course &_course)
	{
		cout << "COURSE ID: "		       << _course.courseID             << endl;
		cout << "COURSE NAME: "		       << _course.courseName           << endl;
		cout << "COURSE INSTRUCTOR NAME: " << _course.courseInstructorName << endl;;
		cout << "COURSE WEEKS: "		   << _course.courseWeeks          << endl;
		
		return _out;
	}
};

#endif // COURSE_H_INCLUDED