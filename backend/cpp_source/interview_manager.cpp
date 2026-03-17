/**
 * Interview Manager - Handles mock interview scheduling
 * Operations: add_slot, remove_slot, list_slots, book, cancel, list_bookings
 * Each slot limited to 3 students per teacher
 * Data stored in JSON format
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;

const int MAX_STUDENTS_PER_SLOT = 3;

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

// Interview Slot structure
struct InterviewSlot {
    string id;
    string teacherName;
    string date;
    string time;
    string venue;
    string bookedStudents;  // Comma-separated student IDs (max 3)
};

string extractField(const string& obj, const string& field) {
    string searchKey = "\"" + field + "\":";
    size_t pos = obj.find(searchKey);
    if (pos == string::npos) return "";
    
    size_t start = obj.find("\"", pos + searchKey.length()) + 1;
    size_t finish = obj.find("\"", start);
    if (start == string::npos || finish == string::npos) return "";
    return obj.substr(start, finish - start);
}

int countBookings(const string& bookedStudents) {
    if (bookedStudents.empty()) return 0;
    int count = 1;
    for (char c : bookedStudents) {
        if (c == ',') count++;
    }
    return count;
}

vector<InterviewSlot> parseSlots(const string& json) {
    vector<InterviewSlot> slots;
    if (json.length() < 3) return slots;
    
    size_t pos = 0;
    while ((pos = json.find("{", pos)) != string::npos) {
        size_t end = json.find("}", pos);
        if (end == string::npos) break;
        
        string obj = json.substr(pos, end - pos + 1);
        InterviewSlot s;
        
        s.id = extractField(obj, "id");
        s.teacherName = extractField(obj, "teacherName");
        s.date = extractField(obj, "date");
        s.time = extractField(obj, "time");
        s.venue = extractField(obj, "venue");
        s.bookedStudents = extractField(obj, "bookedStudents");
        
        if (!s.id.empty()) slots.push_back(s);
        pos = end + 1;
    }
    return slots;
}

string slotsToJson(const vector<InterviewSlot>& slots) {
    stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < slots.size(); i++) {
        int booked = countBookings(slots[i].bookedStudents);
        ss << "  {\n";
        ss << "    \"id\": \"" << slots[i].id << "\",\n";
        ss << "    \"teacherName\": \"" << slots[i].teacherName << "\",\n";
        ss << "    \"date\": \"" << slots[i].date << "\",\n";
        ss << "    \"time\": \"" << slots[i].time << "\",\n";
        ss << "    \"venue\": \"" << slots[i].venue << "\",\n";
        ss << "    \"bookedStudents\": \"" << slots[i].bookedStudents << "\",\n";
        ss << "    \"availableSlots\": " << (MAX_STUDENTS_PER_SLOT - booked) << ",\n";
        ss << "    \"totalSlots\": " << MAX_STUDENTS_PER_SLOT << "\n";
        ss << "  }";
        if (i < slots.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "]";
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: interview_manager <data_file> <operation> [args...]" << endl;
        return 1;
    }
    
    string dataFile = argv[1];
    string operation = argv[2];
    
    string jsonData = readFile(dataFile);
    vector<InterviewSlot> slots = parseSlots(jsonData);
    
    if (operation == "list") {
        // List all interview slots
        cout << slotsToJson(slots);
    }
    else if (operation == "add_slot" && argc >= 7) {
        // Add new slot: teacherName date time venue
        InterviewSlot s;
        s.id = generateId();
        s.teacherName = argv[3];
        s.date = argv[4];
        s.time = argv[5];
        s.venue = argv[6];
        s.bookedStudents = "";
        
        slots.push_back(s);
        writeFile(dataFile, slotsToJson(slots));
        
        cout << "{\"success\": true, \"id\": \"" << s.id << "\", \"message\": \"Interview slot added successfully\"}";
    }
    else if (operation == "remove_slot" && argc >= 4) {
        string idToRemove = argv[3];
        bool found = false;
        
        for (auto it = slots.begin(); it != slots.end(); ++it) {
            if (it->id == idToRemove) {
                slots.erase(it);
                found = true;
                break;
            }
        }
        
        if (found) {
            writeFile(dataFile, slotsToJson(slots));
            cout << "{\"success\": true, \"message\": \"Interview slot removed successfully\"}";
        } else {
            cout << "{\"success\": false, \"message\": \"Slot not found\"}";
        }
    }
    else if (operation == "book" && argc >= 5) {
        // Book a slot: slotId studentId
        string slotId = argv[3];
        string studentId = argv[4];
        
        for (auto& s : slots) {
            if (s.id == slotId) {
                int currentBookings = countBookings(s.bookedStudents);
                
                // Check if slot is full
                if (currentBookings >= MAX_STUDENTS_PER_SLOT) {
                    cout << "{\"success\": false, \"message\": \"This slot is fully booked\"}";
                    return 0;
                }
                
                // Check if student already booked this slot
                if (s.bookedStudents.find(studentId) != string::npos) {
                    cout << "{\"success\": false, \"message\": \"You have already booked this slot\"}";
                    return 0;
                }
                
                // Add student to slot
                if (s.bookedStudents.empty()) {
                    s.bookedStudents = studentId;
                } else {
                    s.bookedStudents += "," + studentId;
                }
                
                writeFile(dataFile, slotsToJson(slots));
                cout << "{\"success\": true, \"message\": \"Slot booked successfully\"}";
                return 0;
            }
        }
        cout << "{\"success\": false, \"message\": \"Slot not found\"}";
    }
    else if (operation == "cancel" && argc >= 5) {
        // Cancel booking: slotId studentId
        string slotId = argv[3];
        string studentId = argv[4];
        
        for (auto& s : slots) {
            if (s.id == slotId) {
                if (s.bookedStudents.find(studentId) == string::npos) {
                    cout << "{\"success\": false, \"message\": \"You haven't booked this slot\"}";
                    return 0;
                }
                
                // Remove student from booked list
                string newBookedStudents = "";
                stringstream ss(s.bookedStudents);
                string item;
                bool first = true;
                while (getline(ss, item, ',')) {
                    if (item != studentId) {
                        if (!first) newBookedStudents += ",";
                        newBookedStudents += item;
                        first = false;
                    }
                }
                s.bookedStudents = newBookedStudents;
                
                writeFile(dataFile, slotsToJson(slots));
                cout << "{\"success\": true, \"message\": \"Booking cancelled successfully\"}";
                return 0;
            }
        }
        cout << "{\"success\": false, \"message\": \"Slot not found\"}";
    }
    else if (operation == "get_student_bookings" && argc >= 4) {
        // Get all slots booked by a student
        string studentId = argv[3];
        stringstream ss;
        ss << "[\n";
        bool first = true;
        
        for (const auto& s : slots) {
            if (s.bookedStudents.find(studentId) != string::npos) {
                if (!first) ss << ",\n";
                int booked = countBookings(s.bookedStudents);
                ss << "  {\n";
                ss << "    \"id\": \"" << s.id << "\",\n";
                ss << "    \"teacherName\": \"" << s.teacherName << "\",\n";
                ss << "    \"date\": \"" << s.date << "\",\n";
                ss << "    \"time\": \"" << s.time << "\",\n";
                ss << "    \"venue\": \"" << s.venue << "\",\n";
                ss << "    \"availableSlots\": " << (MAX_STUDENTS_PER_SLOT - booked) << "\n";
                ss << "  }";
                first = false;
            }
        }
        ss << "\n]";
        cout << ss.str();
    }
    else if (operation == "upcoming") {
        // Get upcoming interviews (slots with available space)
        stringstream ss;
        ss << "[\n";
        bool first = true;
        
        for (const auto& s : slots) {
            int booked = countBookings(s.bookedStudents);
            if (booked < MAX_STUDENTS_PER_SLOT) {
                if (!first) ss << ",\n";
                ss << "  {\n";
                ss << "    \"id\": \"" << s.id << "\",\n";
                ss << "    \"teacherName\": \"" << s.teacherName << "\",\n";
                ss << "    \"date\": \"" << s.date << "\",\n";
                ss << "    \"time\": \"" << s.time << "\",\n";
                ss << "    \"venue\": \"" << s.venue << "\",\n";
                ss << "    \"availableSlots\": " << (MAX_STUDENTS_PER_SLOT - booked) << "\n";
                ss << "  }";
                first = false;
            }
        }
        ss << "\n]";
        cout << ss.str();
    }
    else {
        cerr << "Invalid operation or missing arguments" << endl;
        return 1;
    }
    
    return 0;
}
