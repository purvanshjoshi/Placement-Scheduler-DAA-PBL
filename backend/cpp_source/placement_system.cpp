#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include "database.h"
#include "utils.h"

using namespace std;

class PlacementSystem {
private:
    Database db;
    Student* currentUser;
    
    void displayMainMenu() {
        cout << "\n=== Placement Scheduler System ===" << endl;
        cout << "1. Admin Login" << endl;
        cout << "2. Student Login" << endl;
        cout << "3. Student Signup" << endl;
        cout << "4. Exit" << endl;
        cout << "Choice: ";
    }
    
    void displayAdminMenu() {
        cout << "\n--- Admin Menu ---" << endl;
        cout << "Logged in as: " << currentUser->name << " (Admin)" << endl;
        cout << "1. Manage Companies" << endl;
        cout << "2. Upload Worksheets" << endl;
        cout << "3. View Students" << endl;
        cout << "4. Manage Mock Interviews" << endl;
        cout << "5. Logout" << endl;
        cout << "Choice: ";
    }
    
    void displayStudentMenu() {
        cout << "\n--- Student Menu ---" << endl;
        cout << "Logged in as: " << currentUser->name << " (Student)" << endl;
        cout << "1. View Upcoming Companies" << endl;
        cout << "2. Practice Worksheets" << endl;
        cout << "3. Book Mock Interview" << endl;
        cout << "4. My Applications" << endl;
        cout << "5. My Profile" << endl;
        cout << "6. Logout" << endl;
        cout << "Choice: ";
    }
    
    void adminLogin() {
        string username, password;
        cout << "\n--- Admin Login ---" << endl;
        cout << "Username: ";
        cin >> username;
        cout << "Password: ";
        cin >> password;
        
        if (username == "Ayshi Jain" && password == "12345") {
            // Create admin user
            currentUser = new Student();
            currentUser->name = "Ayshi Jain";
            currentUser->universityRoll = "ADMIN001";
            currentUser->section = "ADMIN";
            currentUser->password = "12345";
            currentUser->role = "admin";
            currentUser->cgpa = 0.0;
            currentUser->contact = "0000000000";
            
            cout << "Admin login successful!" << endl;
            adminDashboard();
        } else {
            cout << "Invalid admin credentials!" << endl;
        }
    }
    
    void studentLogin() {
        string roll, password;
        cout << "\n--- Student Login ---" << endl;
        cout << "University Roll Number: ";
        cin >> roll;
        cout << "Password: ";
        cin >> password;
        
        Student* student = db.findStudent(roll);
        if (student && student->password == password && student->role == "student") {
            currentUser = student;
            cout << "Login successful!" << endl;
            studentDashboard();
        } else {
            cout << "Invalid credentials or user not found!" << endl;
            if (student) delete student;
        }
    }
    
    void studentSignup() {
        Student student;
        cout << "\n--- Student Signup ---" << endl;
        
        cout << "Name: ";
        cin.ignore();
        getline(cin, student.name);
        
        cout << "University Roll Number: ";
        getline(cin, student.universityRoll);
        
        cout << "Section: ";
        getline(cin, student.section);
        
        // Default password
        student.password = "12345";
        
        cout << "CGPA: ";
        cin >> student.cgpa;
        
        cout << "Contact Number: ";
        cin >> student.contact;
        
        student.role = "student";
        
        if (db.addStudent(student)) {
            cout << "Registration successful! Default password: 12345" << endl;
            cout << "Please login with your credentials." << endl;
        } else {
            cout << "Registration failed!" << endl;
        }
    }
    
    void adminDashboard() {
        int choice;
        while (currentUser && currentUser->role == "admin") {
            displayAdminMenu();
            cin >> choice;
            
            switch (choice) {
                case 1:
                    manageCompanies();
                    break;
                case 2:
                    manageWorksheets();
                    break;
                case 3:
                    viewStudents();
                    break;
                case 4:
                    manageMockInterviews();
                    break;
                case 5:
                    delete currentUser;
                    currentUser = nullptr;
                    cout << "Logged out successfully!" << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    void studentDashboard() {
        int choice;
        while (currentUser && currentUser->role == "student") {
            displayStudentMenu();
            cin >> choice;
            
            switch (choice) {
                case 1:
                    viewCompanies();
                    break;
                case 2:
                    practiceWorksheets();
                    break;
                case 3:
                    bookMockInterview();
                    break;
                case 4:
                    viewMyApplications();
                    break;
                case 5:
                    viewMyProfile();
                    break;
                case 6:
                    delete currentUser;
                    currentUser = nullptr;
                    cout << "Logged out successfully!" << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    // Admin Functions
    void manageCompanies() {
        int choice;
        do {
            cout << "\n--- Manage Companies ---" << endl;
            cout << "1. Add Company" << endl;
            cout << "2. View All Companies" << endl;
            cout << "3. Remove Company" << endl;
            cout << "4. Back to Admin Menu" << endl;
            cout << "Choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    addCompany();
                    break;
                case 2:
                    viewAllCompanies();
                    break;
                case 3:
                    removeCompany();
                    break;
            }
        } while (choice != 4);
    }
    
    void addCompany() {
        Company company;
        cout << "\n--- Add Company ---" << endl;
        
        cout << "Company Name: ";
        cin.ignore();
        getline(cin, company.name);
        
        cout << "Stipend (₹): ";
        cin >> company.stipend;
        
        cin.ignore();
        cout << "Prerequisites: ";
        getline(cin, company.prerequisites);
        
        cout << "Deadline (YYYY-MM-DD): ";
        getline(cin, company.deadline);
        
        if (db.addCompany(company)) {
            cout << "Company added successfully!" << endl;
        } else {
            cout << "Failed to add company!" << endl;
        }
    }
    
    void viewAllCompanies() {
        vector<Company> companies = db.getAllCompanies();
        cout << "\n--- All Companies ---" << endl;
        
        if (companies.empty()) {
            cout << "No companies found." << endl;
            return;
        }
        
        for (size_t i = 0; i < companies.size(); i++) {
            cout << i + 1 << ". " << companies[i].name << endl;
            cout << "   Stipend: ₹" << companies[i].stipend << endl;
            cout << "   Prerequisites: " << companies[i].prerequisites << endl;
            cout << "   Deadline: " << companies[i].deadline << endl;
            cout << endl;
        }
    }
    
    void removeCompany() {
        string companyName;
        cout << "\n--- Remove Company ---" << endl;
        cout << "Enter company name to remove: ";
        cin.ignore();
        getline(cin, companyName);
        
        if (db.removeCompany(companyName)) {
            cout << "Company removed successfully!" << endl;
        } else {
            cout << "Company not found!" << endl;
        }
    }
    
    void manageWorksheets() {
        int choice;
        do {
            cout << "\n--- Manage Worksheets ---" << endl;
            cout << "1. Upload Worksheet" << endl;
            cout << "2. View All Worksheets" << endl;
            cout << "3. Remove Worksheet" << endl;
            cout << "4. Back to Admin Menu" << endl;
            cout << "Choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    uploadWorksheet();
                    break;
                case 2:
                    viewAllWorksheets();
                    break;
                case 3:
                    removeWorksheet();
                    break;
            }
        } while (choice != 4);
    }
    
    void uploadWorksheet() {
        Worksheet worksheet;
        cout << "\n--- Upload Worksheet ---" << endl;
        
        cout << "Title: ";
        cin.ignore();
        getline(cin, worksheet.title);
        
        cout << "Company Name: ";
        getline(cin, worksheet.company);
        
        cout << "Description: ";
        getline(cin, worksheet.description);
        
        cout << "File Name: ";
        getline(cin, worksheet.fileName);
        
        worksheet.uploadDate = Utils::getCurrentDate();
        
        if (db.addWorksheet(worksheet)) {
            cout << "Worksheet uploaded successfully!" << endl;
        } else {
            cout << "Failed to upload worksheet!" << endl;
        }
    }
    
    void viewAllWorksheets() {
        vector<Worksheet> worksheets = db.getAllWorksheets();
        cout << "\n--- All Worksheets ---" << endl;
        
        if (worksheets.empty()) {
            cout << "No worksheets found." << endl;
            return;
        }
        
        for (size_t i = 0; i < worksheets.size(); i++) {
            cout << i + 1 << ". " << worksheets[i].title << endl;
            cout << "   Company: " << worksheets[i].company << endl;
            cout << "   Description: " << worksheets[i].description << endl;
            cout << "   Upload Date: " << worksheets[i].uploadDate << endl;
            cout << "   File: " << worksheets[i].fileName << endl;
            cout << endl;
        }
    }
    
    void removeWorksheet() {
        string title;
        cout << "\n--- Remove Worksheet ---" << endl;
        cout << "Enter worksheet title to remove: ";
        cin.ignore();
        getline(cin, title);
        
        if (db.removeWorksheet(title)) {
            cout << "Worksheet removed successfully!" << endl;
        } else {
            cout << "Worksheet not found!" << endl;
        }
    }
    
    void viewStudents() {
        vector<Student> students = db.getAllStudents();
        cout << "\n--- All Students ---" << endl;
        
        if (students.empty()) {
            cout << "No students found." << endl;
            return;
        }
        
        for (const auto& student : students) {
            if (student.role == "student") {
                cout << "Name: " << student.name << endl;
                cout << "Roll: " << student.universityRoll << endl;
                cout << "Section: " << student.section << endl;
                cout << "CGPA: " << student.cgpa << endl;
                cout << "Contact: " << student.contact << endl;
                cout << "------------------------------" << endl;
            }
        }
        
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }
    
    void manageMockInterviews() {
        int choice;
        do {
            cout << "\n--- Manage Mock Interviews ---" << endl;
            cout << "1. Schedule Interview" << endl;
            cout << "2. View All Interviews" << endl;
            cout << "3. Remove Interview Slot" << endl;
            cout << "4. Back to Admin Menu" << endl;
            cout << "Choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    scheduleInterview();
                    break;
                case 2:
                    viewAllInterviews();
                    break;
                case 3:
                    removeInterview();
                    break;
            }
        } while (choice != 4);
    }
    
    void scheduleInterview() {
        MockInterview interview;
        cout << "\n--- Schedule Mock Interview ---" << endl;
        
        cout << "Teacher Name: ";
        cin.ignore();
        getline(cin, interview.teacherName);
        
        cout << "Date (YYYY-MM-DD): ";
        getline(cin, interview.date);
        
        cout << "Time: ";
        getline(cin, interview.time);
        
        cout << "Venue: ";
        getline(cin, interview.venue);
        
        interview.isCompleted = false;
        
        if (db.addMockInterview(interview)) {
            cout << "Mock interview scheduled successfully!" << endl;
        } else {
            cout << "Failed to schedule interview (slot may be full)!" << endl;
        }
    }
    
    void viewAllInterviews() {
        vector<MockInterview> interviews = db.getAllInterviews();
        cout << "\n--- All Mock Interviews ---" << endl;
        
        if (interviews.empty()) {
            cout << "No interviews scheduled." << endl;
            return;
        }
        
        for (size_t i = 0; i < interviews.size(); i++) {
            cout << i + 1 << ". Teacher: " << interviews[i].teacherName << endl;
            cout << "   Date: " << interviews[i].date << " Time: " << interviews[i].time << endl;
            cout << "   Venue: " << interviews[i].venue << endl;
            cout << "   Slots Available: " << (3 - interviews[i].bookedStudents.size()) << "/3" << endl;
            cout << "   Status: " << (interviews[i].isCompleted ? "Completed" : "Upcoming") << endl;
            cout << endl;
        }
    }
    
    void removeInterview() {
        string teacherName, time, date;
        cout << "\n--- Remove Interview ---" << endl;
        
        cout << "Teacher Name: ";
        cin.ignore();
        getline(cin, teacherName);
        
        cout << "Date (YYYY-MM-DD): ";
        getline(cin, date);
        
        cout << "Time: ";
        getline(cin, time);
        
        if (db.removeInterview(teacherName, time, date)) {
            cout << "Interview removed successfully!" << endl;
        } else {
            cout << "Interview not found!" << endl;
        }
    }
    
    // Student Functions
    void viewCompanies() {
        vector<Company> companies = db.getAllCompanies();
        cout << "\n--- Upcoming Companies ---" << endl;
        
        if (companies.empty()) {
            cout << "No companies found." << endl;
            return;
        }
        
        for (size_t i = 0; i < companies.size(); i++) {
            cout << i + 1 << ". " << companies[i].name << endl;
            cout << "   Stipend: ₹" << companies[i].stipend << endl;
            cout << "   Prerequisites: " << companies[i].prerequisites << endl;
            cout << "   Deadline: " << companies[i].deadline << endl;
            
            // Check if student has already applied
            bool hasApplied = false;
            for (const auto& appliedCompany : currentUser->appliedCompanies) {
                if (appliedCompany == companies[i].name) {
                    hasApplied = true;
                    break;
                }
            }
            
            if (hasApplied) {
                cout << "   Status: Applied ✓" << endl;
            } else {
                cout << "   Status: Not Applied" << endl;
            }
            cout << endl;
        }
        
        cout << "\nEnter company number to apply (0 to go back): ";
        int choice;
        cin >> choice;
        
        if (choice > 0 && choice <= companies.size()) {
            applyToCompany(companies[choice - 1].name);
        }
    }
    
    void applyToCompany(const string& companyName) {
        // Check if already applied
        for (const auto& appliedCompany : currentUser->appliedCompanies) {
            if (appliedCompany == companyName) {
                cout << "You have already applied to this company." << endl;
                return;
            }
        }
        
        cout << "\n--- Application Form for " << companyName << " ---" << endl;
        cout << "Why do you want to work at " << companyName << "? ";
        cin.ignore();
        string answer1;
        getline(cin, answer1);
        
        cout << "What relevant skills do you have? ";
        string answer2;
        getline(cin, answer2);
        
        cout << "What is your expected stipend? ";
        string answer3;
        getline(cin, answer3);
        
        // Submit application
        CompanyForm form;
        form.studentRoll = currentUser->universityRoll;
        form.companyName = companyName;
        form.submissionDate = Utils::getCurrentDateTime();
        form.answers = "Q1: " + answer1 + "\nQ2: " + answer2 + "\nQ3: " + answer3;
        
        if (db.submitCompanyForm(form)) {
            cout << "\nApplication submitted successfully!" << endl;
            // Update current user's applied companies
            currentUser->appliedCompanies.push_back(companyName);
        } else {
            cout << "Failed to submit application!" << endl;
        }
    }
    
    void practiceWorksheets() {
        int choice;
        do {
            cout << "\n--- Practice Worksheets ---" << endl;
            cout << "1. View All Worksheets" << endl;
            cout << "2. Mark Worksheet as Done" << endl;
            cout << "3. View My Progress" << endl;
            cout << "4. Back to Student Menu" << endl;
            cout << "Choice: ";
            cin >> choice;
            
            switch (choice) {
                case 1:
                    viewAllWorksheetsStudent();
                    break;
                case 2:
                    markWorksheetDone();
                    break;
                case 3:
                    viewMyProgress();
                    break;
            }
        } while (choice != 4);
    }
    
    void viewAllWorksheetsStudent() {
        vector<Worksheet> worksheets = db.getAllWorksheets();
        cout << "\n--- All Worksheets ---" << endl;
        
        if (worksheets.empty()) {
            cout << "No worksheets available." << endl;
            return;
        }
        
        for (size_t i = 0; i < worksheets.size(); i++) {
            cout << i + 1 << ". " << worksheets[i].title << endl;
            cout << "   Company: " << worksheets[i].company << endl;
            cout << "   Description: " << worksheets[i].description << endl;
            
            // Check if completed
            bool isCompleted = false;
            for (const auto& studentRoll : worksheets[i].completedBy) {
                if (studentRoll == currentUser->universityRoll) {
                    isCompleted = true;
                    break;
                }
            }
            
            if (isCompleted) {
                cout << "   Status: Completed ✓" << endl;
            } else {
                cout << "   Status: Not Completed" << endl;
            }
            cout << endl;
        }
    }
    
    void markWorksheetDone() {
        vector<Worksheet> worksheets = db.getAllWorksheets();
        
        if (worksheets.empty()) {
            cout << "No worksheets available." << endl;
            return;
        }
        
        cout << "\n--- Mark Worksheet as Done ---" << endl;
        for (size_t i = 0; i < worksheets.size(); i++) {
            // Check if already completed
            bool isCompleted = false;
            for (const auto& studentRoll : worksheets[i].completedBy) {
                if (studentRoll == currentUser->universityRoll) {
                    isCompleted = true;
                    break;
                }
            }
            
            if (!isCompleted) {
                cout << i + 1 << ". " << worksheets[i].title << " (" << worksheets[i].company << ")" << endl;
            }
        }
        
        cout << "\nEnter worksheet number to mark as done (0 to cancel): ";
        int choice;
        cin >> choice;
        
        if (choice > 0 && choice <= worksheets.size()) {
            Worksheet worksheet = worksheets[choice - 1];
            
            // Check if already completed
            bool alreadyCompleted = false;
            for (const auto& studentRoll : worksheet.completedBy) {
                if (studentRoll == currentUser->universityRoll) {
                    alreadyCompleted = true;
                    break;
                }
            }
            
            if (!alreadyCompleted) {
                worksheet.completedBy.push_back(currentUser->universityRoll);
                if (db.updateWorksheet(worksheet)) {
                    cout << "Worksheet marked as done!" << endl;
                } else {
                    cout << "Failed to update worksheet!" << endl;
                }
            } else {
                cout << "You have already completed this worksheet!" << endl;
            }
        }
    }
    
    void viewMyProgress() {
        vector<Worksheet> worksheets = db.getAllWorksheets();
        int total = worksheets.size();
        int completed = 0;
        
        for (const auto& worksheet : worksheets) {
            for (const auto& studentRoll : worksheet.completedBy) {
                if (studentRoll == currentUser->universityRoll) {
                    completed++;
                    break;
                }
            }
        }
        
        cout << "\n--- My Progress ---" << endl;
        cout << "Total Worksheets: " << total << endl;
        cout << "Completed: " << completed << endl;
        cout << "Pending: " << (total - completed) << endl;
        
        if (total > 0) {
            double percentage = (completed * 100.0) / total;
            cout << "Completion: " << percentage << "%" << endl;
        }
    }
    
    void bookMockInterview() {
        vector<MockInterview> interviews = db.getAllInterviews();
        
        if (interviews.empty()) {
            cout << "\nNo interviews available." << endl;
            return;
        }
        
        cout << "\n--- Available Mock Interviews ---" << endl;
        vector<MockInterview> availableInterviews;
        
        for (const auto& interview : interviews) {
            if (!interview.isCompleted && interview.bookedStudents.size() < 3) {
                availableInterviews.push_back(interview);
            }
        }
        
        if (availableInterviews.empty()) {
            cout << "No available interview slots at the moment." << endl;
            return;
        }
        
        for (size_t i = 0; i < availableInterviews.size(); i++) {
            cout << i + 1 << ". Teacher: " << availableInterviews[i].teacherName << endl;
            cout << "   Date: " << availableInterviews[i].date << endl;
            cout << "   Time: " << availableInterviews[i].time << endl;
            cout << "   Venue: " << availableInterviews[i].venue << endl;
            cout << "   Slots Available: " << (3 - availableInterviews[i].bookedStudents.size()) << "/3" << endl;
            cout << endl;
        }
        
        cout << "\nEnter interview number to book (0 to go back): ";
        int choice;
        cin >> choice;
        
        if (choice > 0 && choice <= availableInterviews.size()) {
            const MockInterview& interview = availableInterviews[choice - 1];
            
            if (db.bookInterviewSlot(interview.teacherName, interview.time,
                                  interview.date, currentUser->universityRoll)) {
                cout << "Interview booked successfully!" << endl;
            } else {
                cout << "Failed to book interview (slot may be full or already booked)!" << endl;
            }
        }
    }
    
    void viewMyApplications() {
        vector<CompanyForm> forms = db.getFormsByStudent(currentUser->universityRoll);
        cout << "\n--- My Applications ---" << endl;
        
        if (forms.empty()) {
            cout << "You have not applied to any companies yet." << endl;
            return;
        }
        
        for (const auto& form : forms) {
            cout << "Company: " << form.companyName << endl;
            cout << "Submitted on: " << form.submissionDate << endl;
            cout << "------------------------------" << endl;
        }
    }
    
    void viewMyProfile() {
        cout << "\n--- My Profile ---" << endl;
        cout << "Name: " << currentUser->name << endl;
        cout << "Roll Number: " << currentUser->universityRoll << endl;
        cout << "Section: " << currentUser->section << endl;
        cout << "CGPA: " << currentUser->cgpa << endl;
        cout << "Contact: " << currentUser->contact << endl;
        cout << "Applications Submitted: " << currentUser->appliedCompanies.size() << endl;
        
        if (!currentUser->appliedCompanies.empty()) {
            cout << "\nApplied Companies:" << endl;
            for (const auto& company : currentUser->appliedCompanies) {
                cout << "- " << company << endl;
            }
        }
    }
    
public:
    PlacementSystem() : currentUser(nullptr) {}
    
    void run() {
        int choice;
        
        while (true) {
            displayMainMenu();
            cin >> choice;
            
            switch (choice) {
                case 1:
                    adminLogin();
                    break;
                case 2:
                    studentLogin();
                    break;
                case 3:
                    studentSignup();
                    break;
                case 4:
                    cout << "Thank you for using Placement Scheduler!" << endl;
                    return;
                default:
                    cout << "Invalid choice!" << endl;
                    // Clear input buffer
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }
};

int main() {
    PlacementSystem system;
    system.run();
    return 0;
}