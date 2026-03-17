/**
 * Worksheet Manager - Handles worksheet/practice material operations
 * Operations: add, remove, list, get
 * Data stored in JSON format
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;

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

// Worksheet structure
struct Worksheet {
    string id;
    string title;
    string description;
    string topic;
    string difficulty;
    string link;
    string createdDate;
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

vector<Worksheet> parseWorksheets(const string& json) {
    vector<Worksheet> worksheets;
    if (json.length() < 3) return worksheets;
    
    size_t pos = 0;
    while ((pos = json.find("{", pos)) != string::npos) {
        size_t end = json.find("}", pos);
        if (end == string::npos) break;
        
        string obj = json.substr(pos, end - pos + 1);
        Worksheet w;
        
        w.id = extractField(obj, "id");
        w.title = extractField(obj, "title");
        w.description = extractField(obj, "description");
        w.topic = extractField(obj, "topic");
        w.difficulty = extractField(obj, "difficulty");
        w.link = extractField(obj, "link");
        w.createdDate = extractField(obj, "createdDate");
        
        if (!w.id.empty()) worksheets.push_back(w);
        pos = end + 1;
    }
    return worksheets;
}

string worksheetsToJson(const vector<Worksheet>& worksheets) {
    stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < worksheets.size(); i++) {
        ss << "  {\n";
        ss << "    \"id\": \"" << worksheets[i].id << "\",\n";
        ss << "    \"title\": \"" << worksheets[i].title << "\",\n";
        ss << "    \"description\": \"" << worksheets[i].description << "\",\n";
        ss << "    \"topic\": \"" << worksheets[i].topic << "\",\n";
        ss << "    \"difficulty\": \"" << worksheets[i].difficulty << "\",\n";
        ss << "    \"link\": \"" << worksheets[i].link << "\",\n";
        ss << "    \"createdDate\": \"" << worksheets[i].createdDate << "\"\n";
        ss << "  }";
        if (i < worksheets.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "]";
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: worksheet_manager <data_file> <operation> [args...]" << endl;
        return 1;
    }
    
    string dataFile = argv[1];
    string operation = argv[2];
    
    string jsonData = readFile(dataFile);
    vector<Worksheet> worksheets = parseWorksheets(jsonData);
    
    if (operation == "list") {
        cout << worksheetsToJson(worksheets);
    }
    else if (operation == "add" && argc >= 8) {
        // Add: title description topic difficulty link
        Worksheet w;
        w.id = generateId();
        w.title = argv[3];
        w.description = argv[4];
        w.topic = argv[5];
        w.difficulty = argv[6];
        w.link = argv[7];
        
        // Get current date
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateBuffer[11];
        sprintf(dateBuffer, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
        w.createdDate = dateBuffer;
        
        worksheets.push_back(w);
        writeFile(dataFile, worksheetsToJson(worksheets));
        
        cout << "{\"success\": true, \"id\": \"" << w.id << "\", \"message\": \"Worksheet added successfully\"}";
    }
    else if (operation == "remove" && argc >= 4) {
        string idToRemove = argv[3];
        bool found = false;
        
        for (auto it = worksheets.begin(); it != worksheets.end(); ++it) {
            if (it->id == idToRemove) {
                worksheets.erase(it);
                found = true;
                break;
            }
        }
        
        if (found) {
            writeFile(dataFile, worksheetsToJson(worksheets));
            cout << "{\"success\": true, \"message\": \"Worksheet removed successfully\"}";
        } else {
            cout << "{\"success\": false, \"message\": \"Worksheet not found\"}";
        }
    }
    else if (operation == "get" && argc >= 4) {
        string id = argv[3];
        for (const auto& w : worksheets) {
            if (w.id == id) {
                cout << "{\n";
                cout << "  \"id\": \"" << w.id << "\",\n";
                cout << "  \"title\": \"" << w.title << "\",\n";
                cout << "  \"description\": \"" << w.description << "\",\n";
                cout << "  \"topic\": \"" << w.topic << "\",\n";
                cout << "  \"difficulty\": \"" << w.difficulty << "\",\n";
                cout << "  \"link\": \"" << w.link << "\",\n";
                cout << "  \"createdDate\": \"" << w.createdDate << "\"\n";
                cout << "}";
                return 0;
            }
        }
        cout << "{\"error\": \"Worksheet not found\"}";
    }
    else {
        cerr << "Invalid operation or missing arguments" << endl;
        return 1;
    }
    
    return 0;
}
