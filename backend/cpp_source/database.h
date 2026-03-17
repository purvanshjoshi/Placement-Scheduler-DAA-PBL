#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <sstream>

struct Student {
    std::string name;
    std::string universityRoll;
    std::string section;
    std::string password;
    std::string role; // "admin" or "student"
    double cgpa;
    std::string contact;
    std::vector<std::string> appliedCompanies;
};

struct Company {
    std::string name;
    double stipend;
    std::string prerequisites;
    std::string deadline;
    std::vector<std::string> appliedStudents;
};

struct Worksheet {
    std::string title;
    std::string company;
    std::string description;
    std::string uploadDate;
    std::string fileName;
    std::vector<std::string> completedBy;
};

struct MockInterview {
    std::string teacherName;
    std::string time;
    std::string venue;
    std::vector<std::string> bookedStudents;
    std::string date;
    bool isCompleted;
};

struct CompanyForm {
    std::string studentRoll;
    std::string companyName;
    std::string submissionDate;
    std::string answers;
};

class Database {
private:
    std::vector<Student> students;
    std::vector<Company> companies;
    std::vector<Worksheet> worksheets;
    std::vector<MockInterview> interviews;
    std::vector<CompanyForm> forms;

public:
    Database() {
        // Initialize with default admin
        Student admin;
        admin.name = "Ayshi Jain";
        admin.universityRoll = "ADMIN001";
        admin.section = "ADMIN";
        admin.password = "12345";
        admin.role = "admin";
        admin.cgpa = 0.0;
        admin.contact = "0000000000";
        students.push_back(admin);

        // Add some sample companies
        Company c1 = {"Google", 50000, "CGPA > 8.0, DSA Knowledge", "2024-12-31", {}};
        Company c2 = {"Microsoft", 45000, "CGPA > 7.5, OOP Concepts", "2024-12-25", {}};
        Company c3 = {"Amazon", 55000, "CGPA > 8.5, System Design", "2024-12-20", {}};
        companies.push_back(c1);
        companies.push_back(c2);
        companies.push_back(c3);

        // Add some sample worksheets
        Worksheet w1 = {"DSA Practice Set - Google", "Google", 
                       "Basic data structures problems for Google interview", 
                       "2024-01-15", "dsa_practice_google.pdf", {}};
        Worksheet w2 = {"OOP Concepts - Microsoft", "Microsoft", 
                       "Object oriented programming exercises", 
                       "2024-01-10", "oop_exercises_microsoft.pdf", {}};
        worksheets.push_back(w1);
        worksheets.push_back(w2);

        // Add some sample interviews
        MockInterview i1 = {"Dr. Smith", "10:00", "Room 101", {}, "2024-01-20", false};
        MockInterview i2 = {"Prof. Johnson", "14:00", "Room 202", {}, "2024-01-21", false};
        interviews.push_back(i1);
        interviews.push_back(i2);
    }

    // Student operations
    bool addStudent(const Student& student) {
        students.push_back(student);
        return true;
    }

    std::vector<Student> getAllStudents() {
        return students;
    }

    Student* findStudent(const std::string& roll) {
        for (auto& student : students) {
            if (student.universityRoll == roll) {
                return new Student(student);
            }
        }
        return nullptr;
    }

    bool updateStudent(const Student& updatedStudent) {
        for (auto& student : students) {
            if (student.universityRoll == updatedStudent.universityRoll) {
                student = updatedStudent;
                return true;
            }
        }
        return false;
    }

    bool removeStudent(const std::string& roll) {
        auto it = std::remove_if(students.begin(), students.end(),
            [&roll](const Student& s) { return s.universityRoll == roll; });
        
        if (it != students.end()) {
            students.erase(it, students.end());
            return true;
        }
        return false;
    }

    // Company operations
    bool addCompany(const Company& company) {
        companies.push_back(company);
        return true;
    }

    std::vector<Company> getAllCompanies() {
        return companies;
    }

    bool removeCompany(const std::string& companyName) {
        auto it = std::remove_if(companies.begin(), companies.end(),
            [&companyName](const Company& c) { return c.name == companyName; });
        
        if (it != companies.end()) {
            companies.erase(it, companies.end());
            return true;
        }
        return false;
    }

    bool updateCompany(const Company& updatedCompany) {
        for (auto& company : companies) {
            if (company.name == updatedCompany.name) {
                company = updatedCompany;
                return true;
            }
        }
        return false;
    }

    // Worksheet operations
    bool addWorksheet(const Worksheet& worksheet) {
        worksheets.push_back(worksheet);
        return true;
    }

    std::vector<Worksheet> getAllWorksheets() {
        return worksheets;
    }

    bool removeWorksheet(const std::string& title) {
        auto it = std::remove_if(worksheets.begin(), worksheets.end(),
            [&title](const Worksheet& w) { return w.title == title; });
        
        if (it != worksheets.end()) {
            worksheets.erase(it, worksheets.end());
            return true;
        }
        return false;
    }

    bool updateWorksheet(const Worksheet& updatedWorksheet) {
        for (auto& worksheet : worksheets) {
            if (worksheet.title == updatedWorksheet.title) {
                worksheet = updatedWorksheet;
                return true;
            }
        }
        return false;
    }

    // Mock Interview operations
    bool addMockInterview(const MockInterview& interview) {
        // Check if teacher already has 3 bookings at this time
        for (const auto& existing : interviews) {
            if (existing.teacherName == interview.teacherName &&
                existing.time == interview.time &&
                existing.date == interview.date) {
                if (existing.bookedStudents.size() >= 3) {
                    return false; // Slot full
                }
            }
        }
        interviews.push_back(interview);
        return true;
    }

    std::vector<MockInterview> getAllInterviews() {
        return interviews;
    }

    bool removeInterview(const std::string& teacherName, const std::string& time, 
                        const std::string& date) {
        auto it = std::remove_if(interviews.begin(), interviews.end(),
            [&teacherName, &time, &date](const MockInterview& i) {
                return i.teacherName == teacherName && i.time == time && i.date == date;
            });
        
        if (it != interviews.end()) {
            interviews.erase(it, interviews.end());
            return true;
        }
        return false;
    }

    bool updateInterview(const MockInterview& updatedInterview) {
        for (auto& interview : interviews) {
            if (interview.teacherName == updatedInterview.teacherName &&
                interview.time == updatedInterview.time &&
                interview.date == updatedInterview.date) {
                interview = updatedInterview;
                return true;
            }
        }
        return false;
    }

    bool bookInterviewSlot(const std::string& teacherName, const std::string& time,
                          const std::string& date, const std::string& studentRoll) {
        for (auto& interview : interviews) {
            if (interview.teacherName == teacherName &&
                interview.time == time &&
                interview.date == date) {
                
                if (interview.bookedStudents.size() < 3) {
                    // Check if student already booked
                    auto it = std::find(interview.bookedStudents.begin(),
                                       interview.bookedStudents.end(), studentRoll);
                    if (it != interview.bookedStudents.end()) {
                        return false; // Already booked
                    }
                    interview.bookedStudents.push_back(studentRoll);
                    return true;
                }
            }
        }
        return false;
    }

    bool cancelInterviewSlot(const std::string& teacherName, const std::string& time,
                            const std::string& date, const std::string& studentRoll) {
        for (auto& interview : interviews) {
            if (interview.teacherName == teacherName &&
                interview.time == time &&
                interview.date == date) {
                
                auto it = std::find(interview.bookedStudents.begin(),
                                   interview.bookedStudents.end(), studentRoll);
                if (it != interview.bookedStudents.end()) {
                    interview.bookedStudents.erase(it);
                    return true;
                }
            }
        }
        return false;
    }

    // Company Form operations
    bool submitCompanyForm(const CompanyForm& form) {
        forms.push_back(form);
        
        // Update student's applied companies
        for (auto& student : students) {
            if (student.universityRoll == form.studentRoll) {
                student.appliedCompanies.push_back(form.companyName);
                break;
            }
        }
        
        // Update company's applied students
        for (auto& company : companies) {
            if (company.name == form.companyName) {
                company.appliedStudents.push_back(form.studentRoll);
                break;
            }
        }
        
        return true;
    }

    std::vector<CompanyForm> getAllForms() {
        return forms;
    }

    std::vector<CompanyForm> getFormsByStudent(const std::string& studentRoll) {
        std::vector<CompanyForm> studentForms;
        for (const auto& form : forms) {
            if (form.studentRoll == studentRoll) {
                studentForms.push_back(form);
            }
        }
        return studentForms;
    }

    std::vector<CompanyForm> getFormsByCompany(const std::string& companyName) {
        std::vector<CompanyForm> companyForms;
        for (const auto& form : forms) {
            if (form.companyName == companyName) {
                companyForms.push_back(form);
            }
        }
        return companyForms;
    }
};

#endif