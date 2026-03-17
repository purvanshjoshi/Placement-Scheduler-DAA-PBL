# Algorithms Implementation

This document describes the algorithms implemented in the Placement Management System.

## Overview

The project now uses efficient algorithms for data management:

### Student Management
1. **Merge Sort** - For sorting students by roll number
2. **Binary Search** - For searching students by exact roll number

### Company Management
3. **Priority Queue (Max Heap)** - For sorting companies by stipend (highest first)

## 1. Merge Sort Algorithm

### Purpose
Merge Sort is used to display students in sorted order by their roll numbers. This ensures that whenever students are listed, they appear in ascending order of their roll numbers.

### Implementation Details

**Location:** `backend/cpp_source/student_manager.cpp`

**Time Complexity:** O(n log n)
**Space Complexity:** O(n)

### How It Works

1. **Divide:** The algorithm recursively divides the student array into two halves
2. **Conquer:** Recursively sorts both halves
3. **Merge:** Merges the two sorted halves back together

### Code Structure

```cpp
// Main merge sort function
void mergeSort(vector<Student>& students, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(students, left, mid);      // Sort left half
        mergeSort(students, mid + 1, right); // Sort right half
        merge(students, left, mid, right);   // Merge sorted halves
    }
}

// Merge function
void merge(vector<Student>& students, int left, int mid, int right) {
    // Creates temporary arrays
    // Compares elements and merges them in sorted order
    // Sorts based on rollNumber field (string comparison)
}
```

### Usage

The merge sort is automatically applied in two operations:

1. **List Operation:** When viewing all students
   ```
   student_manager.exe students.json list
   ```

2. **Search Operation:** Before performing binary search
   ```
   student_manager.exe students.json search <rollNumber>
   ```

### Example

**Before Sorting:**
- Student 1: Roll Number "2418722"
- Student 2: Roll Number "2418721"

**After Merge Sort:**
- Student 1: Roll Number "2418721"
- Student 2: Roll Number "2418722"

## 2. Binary Search Algorithm

### Purpose
Binary Search is used to efficiently find a student by their exact roll number. This provides O(log n) search time instead of O(n) linear search.

### Implementation Details

**Location:** `backend/cpp_source/student_manager.cpp`

**Time Complexity:** O(log n)
**Space Complexity:** O(1)

**Prerequisites:** The student array must be sorted (done automatically using merge sort)

### How It Works

1. **Initialize:** Set left pointer to 0, right pointer to array size - 1
2. **Iterate:** While left <= right:
   - Calculate middle index
   - If middle element equals target, return the index
   - If target is greater, search right half (left = mid + 1)
   - If target is smaller, search left half (right = mid - 1)
3. **Not Found:** Return -1 if element is not found

### Code Structure

```cpp
int binarySearch(const vector<Student>& students, const string& rollNumber) {
    int left = 0;
    int right = students.size() - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (students[mid].rollNumber == rollNumber)
            return mid;  // Found
        
        if (students[mid].rollNumber < rollNumber)
            left = mid + 1;  // Search right half
        else
            right = mid - 1;  // Search left half
    }
    
    return -1;  // Not found
}
```

### Usage

Binary search is used in the search operation:

```
student_manager.exe students.json search <rollNumber>
```

### Search Behavior

The search operation uses a **hybrid approach**:

1. **First:** Attempts exact match using binary search (O(log n))
2. **Fallback:** If no exact match, falls back to partial match using linear search (O(n))

This allows both exact searches (efficient) and partial searches (flexible).

### Example

**Searching for Roll Number "2418722":**

Array (sorted): ["2418721", "2418722", "2418723", "2418724", "2418725"]

**Steps:**
1. left=0, right=4, mid=2 → students[2]="2418723" → too high
2. left=0, right=1, mid=0 → students[0]="2418721" → too low
3. left=1, right=1, mid=1 → students[1]="2418722" → **FOUND!**

## 3. Priority Queue (Max Heap) Algorithm

### Purpose
Priority Queue using Max Heap is used to display companies sorted by stipend in descending order (highest stipend first). This ensures students see the most lucrative opportunities at the top.

### Implementation Details

**Location:** `backend/cpp_source/company_manager.cpp`

**Time Complexity:** 
- Insert: O(log n)
- Extract Max: O(log n)
- Build Heap: O(n log n)

**Space Complexity:** O(n)

### How It Works

A **Max Heap** is a complete binary tree where each parent node has a value greater than or equal to its children. We use this to implement a priority queue.

1. **Insert (Push):** Add company to heap and heapify up
2. **Extract Max (Pop):** Remove root (highest stipend), replace with last element, heapify down
3. **Heapify Up:** Bubble element up if it's larger than parent
4. **Heapify Down:** Push element down if it's smaller than children

### Code Structure

```cpp
class MaxHeap {
private:
    vector<Company> heap;
    
    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    
    void heapifyUp(int index) {
        // Move element up if larger than parent
        while (index > 0 && heap[index].stipend > heap[parent(index)].stipend) {
            swap(index, parent(index));
            index = parent(index);
        }
    }
    
    void heapifyDown(int index) {
        // Move element down if smaller than children
        // Choose largest child to swap with
    }
    
public:
    void push(const Company& company);  // O(log n)
    Company pop();                       // O(log n)
};

// Sort companies using priority queue
vector<Company> sortCompaniesByStipend(const vector<Company>& companies) {
    MaxHeap pq;
    
    // Insert all companies - O(n log n)
    for (const auto& company : companies) {
        pq.push(company);
    }
    
    // Extract in descending order - O(n log n)
    vector<Company> sorted;
    while (!pq.empty()) {
        sorted.push_back(pq.pop());
    }
    
    return sorted;
}
```

### Usage

The priority queue is automatically applied when listing companies:

```
company_manager.exe companies.json list
```

### Example

**Input Companies (unsorted):**
- Infosys: ₹120,000
- Amazon: ₹1,000,000
- Google: ₹200,000
- Facebook: ₹300,000

**After Priority Queue (sorted by stipend):**
1. Amazon: ₹1,000,000 (highest)
2. Facebook: ₹300,000
3. Google: ₹200,000
4. Infosys: ₹120,000 (lowest)

### Heap Structure Visualization

**Max Heap Structure:**
```
       Amazon (1000000)
       /              \
  Facebook (300000)  Google (200000)
     /
Infosys (120000)
```

**Properties:**
- Root always contains maximum stipend
- Parent ≥ Children (max heap property)
- Stored as array: [Amazon, Facebook, Google, Infosys]
- Parent of index i: (i-1)/2
- Left child of index i: 2i+1
- Right child of index i: 2i+2

### Algorithm Steps

**Building the Heap:**
1. Insert Infosys (120000) → Root
2. Insert Amazon (1000000) → Heapify up, becomes root
3. Insert Google (200000) → Insert as right child of Amazon
4. Insert Facebook (300000) → Insert, heapify up

**Extracting in Order:**
1. Pop Amazon (1000000) - highest
2. Heapify down, Facebook becomes root
3. Pop Facebook (300000)
4. Pop Google (200000)
5. Pop Infosys (120000) - lowest

## Integration with REST API

### Endpoints Using These Algorithms

**Student Endpoints:**
1. **GET /api/students** - Lists all students (uses merge sort)
2. **GET /api/students/search?q=<rollNumber>** - Searches students (uses binary search + merge sort)

**Company Endpoints:**
3. **GET /api/companies** - Lists all companies (uses priority queue/max heap)

### Python Backend Integration

The Flask backend (`backend/app.py`) calls the C++ executables which automatically apply these algorithms:

```python
# Get all students - automatically sorted by roll number
result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'list'])

# Search students - uses binary search
result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'search', query])

# Get all companies - automatically sorted by stipend (highest first)
result = run_cpp_executable('company_manager', [COMPANIES_FILE, 'list'])
```

## Performance Comparison

### Before Implementation (Linear Operations)

- **Listing students:** Unsorted, O(1) display
- **Searching students:** Linear search, O(n)
- **Listing companies:** Unsorted, O(1) display

### After Implementation (Optimized)

- **Listing students:** Merge sort + display, O(n log n)
- **Searching students:** Binary search (exact match), O(log n)
- **Listing companies:** Priority queue sort + display, O(n log n)

### Performance with Different Data Sizes

| Number of Students | Linear Search | Binary Search | Merge Sort |
|-------------------|---------------|---------------|------------|
| 10                | 10 ops        | 4 ops         | 33 ops     |
| 100               | 100 ops       | 7 ops         | 664 ops    |
| 1,000             | 1,000 ops     | 10 ops        | 9,965 ops  |
| 10,000            | 10,000 ops    | 14 ops        | 133,616 ops|

**Note:** While merge sort has overhead, it's done once and enables efficient binary search. For repeated searches, this is highly beneficial.

## Algorithm Advantages

### Merge Sort Benefits
- **Stable sorting:** Maintains relative order of equal elements
- **Consistent performance:** Always O(n log n), no worst-case degradation
- **Predictable:** Works well with any data distribution

### Binary Search Benefits
- **Fast search:** Logarithmic time complexity
- **Scalable:** Performance degrades slowly as data grows
- **Efficient:** Particularly beneficial with large datasets

### Priority Queue Benefits
- **Optimal ordering:** Always shows highest priority (stipend) first
- **Efficient insertion:** O(log n) to add new company
- **Efficient extraction:** O(log n) to get highest stipend company
- **Natural fit:** Perfect for priority-based scenarios

## Testing the Algorithms

### Test Merge Sort
```bash
# View students - they should appear sorted by roll number
backend\cpp_executables\student_manager.exe backend\data\students.json list
```

### Test Binary Search
```bash
# Search for exact roll number
backend\cpp_executables\student_manager.exe backend\data\students.json search 2418722
```

### Test Partial Match (Fallback)
```bash
# Search with partial roll number
backend\cpp_executables\student_manager.exe backend\data\students.json search 2418
```

### Test Priority Queue
```bash
# View companies - they should appear sorted by stipend (highest first)
backend\cpp_executables\company_manager.exe backend\data\companies.json list
```

## Future Enhancements

Potential algorithm improvements:
1. **Caching:** Cache sorted array to avoid re-sorting on every operation
2. **Indexing:** Maintain a hash index for O(1) exact match searches
3. **Sorting options:** Allow sorting by name, CGPA, or section
4. **Advanced search:** Implement multi-field search with indexing

## Compilation

To recompile after making changes:

```bash
# Compile student manager
g++ -o backend\cpp_executables\student_manager.exe backend\cpp_source\student_manager.cpp -std=c++11

# Compile company manager
g++ -o backend\cpp_executables\company_manager.exe backend\cpp_source\company_manager.cpp -std=c++11

# Or use the batch file to compile all
cd backend\cpp_source
compile.bat
```

## Summary

The implementation of advanced algorithms provides:

**Student Management:**
- ✅ Sorted student display by roll number (Merge Sort)
- ✅ Fast exact roll number search - O(log n) (Binary Search)
- ✅ Flexible partial search with fallback (Linear Search)

**Company Management:**
- ✅ Sorted company display by stipend, highest first (Priority Queue/Max Heap)
- ✅ Efficient O(n log n) sorting for optimal user experience

**Overall:**
- ✅ Scalable solution for growing databases
- ✅ Industry-standard algorithms with proven efficiency
- ✅ Better user experience with organized, prioritized data
