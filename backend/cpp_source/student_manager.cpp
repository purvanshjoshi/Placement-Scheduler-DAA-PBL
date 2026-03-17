/**
 * Student Manager - Handles student-related operations
 * Operations: register, login, submit_form, list, search, get, remove
 * Data stored in JSON format
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

// Forward declaration of Student struct
struct Student {
    string id;
    string name;
    string rollNumber;
    string section;
    string cgpa;
    string contact;
    string password;
    string appliedCompanies;
    string completedWorksheets;
};

// ============= MERGE SORT IMPLEMENTATION =============
// Merge function for merge sort
void merge(vector<Student>& students, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    
    // Create temporary vectors
    vector<Student> leftArr(n1);
    vector<Student> rightArr(n2);
    
    // Copy data to temporary vectors
    for (int i = 0; i < n1; i++)
        leftArr[i] = students[left + i];
    for (int j = 0; j < n2; j++)
        rightArr[j] = students[mid + 1 + j];
    
    // Merge the temporary vectors back
    int i = 0, j = 0, k = left;
    
    while (i < n1 && j < n2) {
        // Compare roll numbers (as strings)
        if (leftArr[i].rollNumber <= rightArr[j].rollNumber) {
            students[k] = leftArr[i];
            i++;
        } else {
            students[k] = rightArr[j];
            j++;
        }
        k++;
    }
    
    // Copy remaining elements of leftArr
    while (i < n1) {
        students[k] = leftArr[i];
        i++;
        k++;
    }
    
    // Copy remaining elements of rightArr
    while (j < n2) {
        students[k] = rightArr[j];
        j++;
        k++;
    }
}

// Merge sort function - sorts students by roll number
void mergeSort(vector<Student>& students, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        
        // Sort first and second halves
        mergeSort(students, left, mid);
        mergeSort(students, mid + 1, right);
        
        // Merge the sorted halves
        merge(students, left, mid, right);
    }
}

// ============= BINARY SEARCH IMPLEMENTATION =============
// Binary search function - searches for student by exact roll number
// Returns index if found, -1 if not found
int binarySearch(const vector<Student>& students, const string& rollNumber) {
    int left = 0;
    int right = students.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        // Check if rollNumber is present at mid
        if (students[mid].rollNumber == rollNumber)
            return mid;
        
        // If rollNumber is greater, ignore left half
        if (students[mid].rollNumber < rollNumber)
            left = mid + 1;
        else // If rollNumber is smaller, ignore right half
            right = mid - 1;
    }
    
    // Element not found
    return -1;
}

string generateId() {
    time_t now = time(0);
    return to_string(now) + to_string(rand() % 1000);
}

string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return "[]";
    
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    file << content;
    file.close();
}

// Extract field value from JSON object string
string extractField(const string& obj, const string& field) {
    string searchKey = "\"" + field + "\":";
    size_t pos = obj.find(searchKey);
    if (pos == string::npos) return "";
    
    size_t start = obj.find("\"", pos + searchKey.length()) + 1;
    size_t finish = obj.find("\"", start);
    if (start == string::npos || finish == string::npos) return "";
    return obj.substr(start, finish - start);
}

vector<Student> parseStudents(const string& json) {
    vector<Student> students;
    if (json.length() < 3) return students;
    
    size_t pos = 0;
    while ((pos = json.find("{", pos)) != string::npos) {
        size_t end = json.find("}", pos);
        if (end == string::npos) break;
        
        string obj = json.substr(pos, end - pos + 1);
        Student s;
        
        s.id = extractField(obj, "id");
        s.name = extractField(obj, "name");
        s.rollNumber = extractField(obj, "rollNumber");
        s.section = extractField(obj, "section");
        s.cgpa = extractField(obj, "cgpa");
        s.contact = extractField(obj, "contact");
        s.password = extractField(obj, "password");
        s.appliedCompanies = extractField(obj, "appliedCompanies");
        s.completedWorksheets = extractField(obj, "completedWorksheets");
        
        if (!s.id.empty()) students.push_back(s);
        pos = end + 1;
    }
    
    return students;
}

string studentsToJson(const vector<Student>& students) {
    stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < students.size(); i++) {
        ss << "  {\n";
        ss << "    \"id\": \"" << students[i].id << "\",\n";
        ss << "    \"name\": \"" << students[i].name << "\",\n";
        ss << "    \"rollNumber\": \"" << students[i].rollNumber << "\",\n";
        ss << "    \"section\": \"" << students[i].section << "\",\n";
        ss << "    \"cgpa\": \"" << students[i].cgpa << "\",\n";
        ss << "    \"contact\": \"" << students[i].contact << "\",\n";
        ss << "    \"password\": \"" << students[i].password << "\",\n";
        ss << "    \"appliedCompanies\": \"" << students[i].appliedCompanies << "\",\n";
        ss << "    \"completedWorksheets\": \"" << students[i].completedWorksheets << "\"\n";
        ss << "  }";
        if (i < students.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "]";
    return ss.str();
}

string studentToJson(const Student& s) {
    stringstream ss;
    ss << "{\n";
    ss << "  \"id\": \"" << s.id << "\",\n";
    ss << "  \"name\": \"" << s.name << "\",\n";
    ss << "  \"rollNumber\": \"" << s.rollNumber << "\",\n";
    ss << "  \"section\": \"" << s.section << "\",\n";
    ss << "  \"cgpa\": \"" << s.cgpa << "\",\n";
    ss << "  \"contact\": \"" << s.contact << "\",\n";
    ss << "  \"appliedCompanies\": \"" << s.appliedCompanies << "\",\n";
    ss << "  \"completedWorksheets\": \"" << s.completedWorksheets << "\"\n";
    ss << "}";
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: student_manager <data_file> <operation> [args...]" << endl;
        return 1;
    }
    
    string dataFile = argv[1];
    string operation = argv[2];
    
    string jsonData = readFile(dataFile);
    vector<Student> students = parseStudents(jsonData);
    
    if (operation == "list") {
        // Sort students using merge sort before displaying
        if (!students.empty()) {
            mergeSort(students, 0, students.size() - 1);
        }
        
        stringstream ss;
        ss << "[\n";
        for (size_t i = 0; i < students.size(); i++) {
            ss << "  " << studentToJson(students[i]);
            if (i < students.size() - 1) ss << ",";
            ss << "\n";
        }
        ss << "]";
        cout << ss.str();
    }
    else if (operation == "register" && argc >= 8) {
        string rollNumber = argv[4];
        
        // Check if roll number already exists
        for (const auto& s : students) {
            if (s.rollNumber == rollNumber) {
                cout << "{\"success\": false, \"message\": \"Roll number already registered\"}";
                return 0;
            }
        }
        
        Student newStudent;
        newStudent.id = generateId();
        newStudent.name = argv[3];
        newStudent.rollNumber = rollNumber;
        newStudent.section = argv[5];
        newStudent.password = argv[6];
        newStudent.contact = argv[7];
        newStudent.cgpa = "";
        newStudent.appliedCompanies = "";
        newStudent.completedWorksheets = "";
        
        students.push_back(newStudent);
        writeFile(dataFile, studentsToJson(students));
        
        cout << "{\"success\": true, \"id\": \"" << newStudent.id << "\", \"message\": \"Registration successful\"}";
    }
    else if (operation == "login" && argc >= 5) {
        string rollNumber = argv[3];
        string password = argv[4];
        
        for (const auto& s : students) {
            if (s.rollNumber == rollNumber && s.password == password) {
                cout << "{\"success\": true, \"student\": " << studentToJson(s) << "}";
                return 0;
            }
        }
        
        cout << "{\"success\": false, \"message\": \"Invalid credentials\"}";
    }
    else if (operation == "update" && argc >= 8) {
        string id = argv[3];
        
        for (auto& s : students) {
            if (s.id == id) {
                s.name = argv[4];
                s.cgpa = argv[5];
                s.contact = argv[6];
                s.section = argv[7];
                writeFile(dataFile, studentsToJson(students));
                cout << "{\"success\": true, \"message\": \"Profile updated\"}";
                return 0;
            }
        }
        
        cout << "{\"success\": false, \"message\": \"Student not found\"}";
    }
    else if (operation == "update_cgpa" && argc >= 5) {
        string id = argv[3];
        string cgpa = argv[4];
        
        for (auto& s : students) {
            if (s.id == id) {
                s.cgpa = cgpa;
                writeFile(dataFile, studentsToJson(students));
                cout << "{\"success\": true, \"message\": \"CGPA updated successfully\"}";
                return 0;
            }
        }
        
        cout << "{\"success\": false, \"message\": \"Student not found\"}";
    }
    else if (operation == "apply" && argc >= 5) {
        string studentId = argv[3];
        string companyId = argv[4];
        
        for (auto& s : students) {
            if (s.id == studentId) {
                if (s.appliedCompanies.empty()) {
                    s.appliedCompanies = companyId;
                } else if (s.appliedCompanies.find(companyId) == string::npos) {
                    s.appliedCompanies += "," + companyId;
                } else {
                    cout << "{\"success\": false, \"message\": \"Already applied to this company\"}";
                    return 0;
                }
                writeFile(dataFile, studentsToJson(students));
                cout << "{\"success\": true, \"message\": \"Applied successfully\"}";
                return 0;
            }
        }
        
        cout << "{\"success\": false, \"message\": \"Student not found\"}";
    }
    else if (operation == "complete_worksheet" && argc >= 5) {
        string studentId = argv[3];
        string worksheetId = argv[4];
        
        for (auto& s : students) {
            if (s.id == studentId) {
                if (s.completedWorksheets.empty()) {
                    s.completedWorksheets = worksheetId;
                } else if (s.completedWorksheets.find(worksheetId) == string::npos) {
                    s.completedWorksheets += "," + worksheetId;
                }
                writeFile(dataFile, studentsToJson(students));
                cout << "{\"success\": true, \"message\": \"Worksheet marked as complete\"}";
                return 0;
            }
        }
        
        cout << "{\"success\": false, \"message\": \"Student not found\"}";
    }
    else if (operation == "search" && argc >= 4) {
        string query = argv[3];
        
        // Sort students first for binary search
        if (!students.empty()) {
            mergeSort(students, 0, students.size() - 1);
        }
        
        // Try exact match using binary search first
        int index = binarySearch(students, query);
        
        stringstream ss;
        ss << "[\n";
        bool first = true;
        
        if (index != -1) {
            // Exact match found using binary search
            ss << "  " << studentToJson(students[index]);
        } else {
            // Fallback to partial match (linear search for substring)
            for (const auto& s : students) {
                if (s.rollNumber.find(query) != string::npos) {
                    if (!first) ss << ",\n";
                    ss << "  " << studentToJson(s);
                    first = false;
                }
            }
        }
        ss << "\n]";
        cout << ss.str();
    }
    else if (operation == "get" && argc >= 4) {
        string id = argv[3];
        for (const auto& s : students) {
            if (s.id == id) {
                cout << studentToJson(s);
                return 0;
            }
        }
        cout << "{\"error\": \"Student not found\"}";
    }
    else if (operation == "remove" && argc >= 4) {
        // NEW OPERATION: Remove student by ID
        string idToRemove = argv[3];
        bool found = false;
        
        // Don't allow removal of admin accounts
        for (const auto& s : students) {
            if (s.id == idToRemove) {
                if (s.rollNumber == "ADMIN001" || s.section == "ADMIN") {
                    cout << "{\"success\": false, \"message\": \"Cannot remove admin account\"}";
                    return 0;
                }
                break;
            }
        }
        
        for (auto it = students.begin(); it != students.end(); ++it) {
            if (it->id == idToRemove) {
                students.erase(it);
                found = true;
                break;
            }
        }
        
        if (found) {
            writeFile(dataFile, studentsToJson(students));
            cout << "{\"success\": true, \"message\": \"Student removed successfully\"}";
        } else {
            cout << "{\"success\": false, \"message\": \"Student not found\"}";
        }
    }
    else {
        cerr << "Invalid operation or missing arguments" << endl;
        return 1;
    }
    
    return 0;
}