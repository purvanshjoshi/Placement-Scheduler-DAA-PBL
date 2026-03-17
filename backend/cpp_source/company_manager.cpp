/**
 * Company Manager - Handles company-related operations
 * Operations: add, remove, list, get
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

// Forward declaration
struct Company {
    string id;
    string name;
    string stipend;
    string prerequisites;
    string visitDate;
    string status;
};

// ============= PRIORITY QUEUE (MAX HEAP) IMPLEMENTATION =============
// Helper function to convert stipend string to double for comparison
double getStipendValue(const string& stipend) {
    try {
        return stod(stipend);
    } catch (...) {
        return 0.0;
    }
}

// Max Heap implementation for Priority Queue
class MaxHeap {
private:
    vector<Company> heap;
    
    // Get parent index
    int parent(int i) { return (i - 1) / 2; }
    
    // Get left child index
    int leftChild(int i) { return 2 * i + 1; }
    
    // Get right child index
    int rightChild(int i) { return 2 * i + 2; }
    
    // Swap two companies
    void swap(int i, int j) {
        Company temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;
    }
    
    // Heapify up - move element up to maintain max heap property
    void heapifyUp(int index) {
        while (index > 0) {
            int parentIdx = parent(index);
            // Compare stipends (higher stipend has higher priority)
            if (getStipendValue(heap[index].stipend) > getStipendValue(heap[parentIdx].stipend)) {
                swap(index, parentIdx);
                index = parentIdx;
            } else {
                break;
            }
        }
    }
    
    // Heapify down - move element down to maintain max heap property
    void heapifyDown(int index) {
        int size = heap.size();
        
        while (true) {
            int largest = index;
            int left = leftChild(index);
            int right = rightChild(index);
            
            // Find largest among node and its children
            if (left < size && getStipendValue(heap[left].stipend) > getStipendValue(heap[largest].stipend)) {
                largest = left;
            }
            
            if (right < size && getStipendValue(heap[right].stipend) > getStipendValue(heap[largest].stipend)) {
                largest = right;
            }
            
            // If largest is not the current node, swap and continue
            if (largest != index) {
                swap(index, largest);
                index = largest;
            } else {
                break;
            }
        }
    }
    
public:
    // Insert a company into the priority queue
    void push(const Company& company) {
        heap.push_back(company);
        heapifyUp(heap.size() - 1);
    }
    
    // Remove and return the company with highest stipend
    Company pop() {
        if (heap.empty()) {
            return Company();
        }
        
        Company maxCompany = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        
        if (!heap.empty()) {
            heapifyDown(0);
        }
        
        return maxCompany;
    }
    
    // Check if priority queue is empty
    bool empty() const {
        return heap.empty();
    }
    
    // Get size of priority queue
    int size() const {
        return heap.size();
    }
};

// Function to sort companies by stipend using priority queue (max heap)
vector<Company> sortCompaniesByStipend(const vector<Company>& companies) {
    MaxHeap pq;
    
    // Insert all companies into priority queue
    for (const auto& company : companies) {
        pq.push(company);
    }
    
    // Extract companies in descending order of stipend
    vector<Company> sortedCompanies;
    while (!pq.empty()) {
        sortedCompanies.push_back(pq.pop());
    }
    
    return sortedCompanies;
}

// Simple JSON handling without external libraries
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r\"");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r\"");
    return str.substr(first, last - first + 1);
}

// Generate unique ID based on timestamp
string generateId() {
    time_t now = time(0);
    return to_string(now) + to_string(rand() % 1000);
}

// Read entire file content
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return "[]";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Write content to file
void writeFile(const string& filename, const string& content) {
    ofstream file(filename);
    file << content;
    file.close();
}

// Parse companies from JSON string (simple parser)
vector<Company> parseCompanies(const string& json) {
    vector<Company> companies;
    if (json.length() < 3) return companies;
    
    size_t pos = 0;
    while ((pos = json.find("{", pos)) != string::npos) {
        size_t end = json.find("}", pos);
        if (end == string::npos) break;
        
        string obj = json.substr(pos, end - pos + 1);
        Company c;
        
        // Extract fields
        size_t idPos = obj.find("\"id\":");
        if (idPos != string::npos) {
            size_t start = obj.find("\"", idPos + 5) + 1;
            size_t finish = obj.find("\"", start);
            c.id = obj.substr(start, finish - start);
        }
        
        size_t namePos = obj.find("\"name\":");
        if (namePos != string::npos) {
            size_t start = obj.find("\"", namePos + 7) + 1;
            size_t finish = obj.find("\"", start);
            c.name = obj.substr(start, finish - start);
        }
        
        size_t stipendPos = obj.find("\"stipend\":");
        if (stipendPos != string::npos) {
            size_t start = obj.find("\"", stipendPos + 10) + 1;
            size_t finish = obj.find("\"", start);
            c.stipend = obj.substr(start, finish - start);
        }
        
        size_t prereqPos = obj.find("\"prerequisites\":");
        if (prereqPos != string::npos) {
            size_t start = obj.find("\"", prereqPos + 16) + 1;
            size_t finish = obj.find("\"", start);
            c.prerequisites = obj.substr(start, finish - start);
        }
        
        size_t datePos = obj.find("\"visitDate\":");
        if (datePos != string::npos) {
            size_t start = obj.find("\"", datePos + 12) + 1;
            size_t finish = obj.find("\"", start);
            c.visitDate = obj.substr(start, finish - start);
        }
        
        size_t statusPos = obj.find("\"status\":");
        if (statusPos != string::npos) {
            size_t start = obj.find("\"", statusPos + 9) + 1;
            size_t finish = obj.find("\"", start);
            c.status = obj.substr(start, finish - start);
        }
        
        if (!c.id.empty()) companies.push_back(c);
        pos = end + 1;
    }
    return companies;
}

// Convert companies to JSON string
string companiesToJson(const vector<Company>& companies) {
    stringstream ss;
    ss << "[\n";
    for (size_t i = 0; i < companies.size(); i++) {
        ss << "  {\n";
        ss << "    \"id\": \"" << companies[i].id << "\",\n";
        ss << "    \"name\": \"" << companies[i].name << "\",\n";
        ss << "    \"stipend\": \"" << companies[i].stipend << "\",\n";
        ss << "    \"prerequisites\": \"" << companies[i].prerequisites << "\",\n";
        ss << "    \"visitDate\": \"" << companies[i].visitDate << "\",\n";
        ss << "    \"status\": \"" << companies[i].status << "\"\n";
        ss << "  }";
        if (i < companies.size() - 1) ss << ",";
        ss << "\n";
    }
    ss << "]";
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: company_manager <data_file> <operation> [args...]" << endl;
        return 1;
    }
    
    string dataFile = argv[1];
    string operation = argv[2];
    
    // Read existing data
    string jsonData = readFile(dataFile);
    vector<Company> companies = parseCompanies(jsonData);
    
    if (operation == "list") {
        // Sort companies by stipend (highest first) using priority queue
        vector<Company> sortedCompanies = sortCompaniesByStipend(companies);
        cout << companiesToJson(sortedCompanies);
    }
    else if (operation == "add" && argc >= 8) {
        // Add new company: name stipend prerequisites visitDate status
        Company newCompany;
        newCompany.id = generateId();
        newCompany.name = argv[3];
        newCompany.stipend = argv[4];
        newCompany.prerequisites = argv[5];
        newCompany.visitDate = argv[6];
        newCompany.status = argv[7];
        
        companies.push_back(newCompany);
        writeFile(dataFile, companiesToJson(companies));
        
        cout << "{\"success\": true, \"id\": \"" << newCompany.id << "\", \"message\": \"Company added successfully\"}";
    }
    else if (operation == "remove" && argc >= 4) {
        // Remove company by ID
        string idToRemove = argv[3];
        bool found = false;
        
        for (auto it = companies.begin(); it != companies.end(); ++it) {
            if (it->id == idToRemove) {
                companies.erase(it);
                found = true;
                break;
            }
        }
        
        if (found) {
            writeFile(dataFile, companiesToJson(companies));
            cout << "{\"success\": true, \"message\": \"Company removed successfully\"}";
        } else {
            cout << "{\"success\": false, \"message\": \"Company not found\"}";
        }
    }
    else if (operation == "get" && argc >= 4) {
        // Get single company by ID
        string id = argv[3];
        for (const auto& c : companies) {
            if (c.id == id) {
                cout << "{\n";
                cout << "  \"id\": \"" << c.id << "\",\n";
                cout << "  \"name\": \"" << c.name << "\",\n";
                cout << "  \"stipend\": \"" << c.stipend << "\",\n";
                cout << "  \"prerequisites\": \"" << c.prerequisites << "\",\n";
                cout << "  \"visitDate\": \"" << c.visitDate << "\",\n";
                cout << "  \"status\": \"" << c.status << "\"\n";
                cout << "}";
                return 0;
            }
        }
        cout << "{\"error\": \"Company not found\"}";
    }
    else {
        cerr << "Invalid operation or missing arguments" << endl;
        return 1;
    }
    
    return 0;
}
