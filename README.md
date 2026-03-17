# Placement Scheduler Application

A full-stack placement management system with C++ backend logic and Flask/HTML/CSS/JS frontend.

## Features

### Algorithms Implemented
- **Merge Sort**: Students are automatically sorted by roll number when viewing the list (O(n log n))
- **Binary Search**: Efficient searching by exact roll number (O(log n))
- **Priority Queue (Max Heap)**: Companies are automatically sorted by stipend, highest first (O(n log n))
- See [ALGORITHMS.md](ALGORITHMS.md) for detailed documentation

### Admin Dashboard
- **Manage Companies**: Add, view, and remove upcoming companies with details (stipend, prerequisites, visit date)
- **Upload Worksheets**: Create practice materials for students with topics and difficulty levels
- **View Students**: Search and filter students by roll number, view their applications
- **Manage Mock Interviews**: Schedule interview slots (3 students per teacher per slot)

### Student Interface
- **Sign-up/Sign-in**: Register with name, roll number, and section
- **Upcoming Companies**: View all companies and apply with one click
- **Practice Worksheets**: Access worksheets and mark them as completed
- **Book Mock Interviews**: View available slots and book/cancel interview sessions
- **Profile**: View personal stats and application history

## Project Structure

```
DAA-PBL/
├── backend/
│   ├── cpp_executables/     # Compiled C++ executables
│   ├── cpp_source/          # C++ source files
│   │   ├── company_manager.cpp
│   │   ├── student_manager.cpp
│   │   ├── worksheet_manager.cpp
│   │   ├── interview_manager.cpp
│   │   └── compile.bat
│   ├── data/                # JSON data storage
│   └── app.py               # Flask server
├── frontend/
│   ├── index.html
│   ├── styles.css
│   └── scripts.js
└── README.md
```

## Prerequisites

- **Python 3.7+** with pip
- **g++ (MinGW)** for compiling C++ files on Windows
- Modern web browser

## Setup Instructions

### 1. Clone the Repository
```bash
git clone <repository-url>
cd DAA-PBL
```

### 2. Install Python Dependencies
```bash
pip install flask flask-cors
```

### 3. Compile C++ Source Files
```bash
cd backend/cpp_source
compile.bat
```
This will create executables in `backend/cpp_executables/`.

### 4. Run the Application
```bash
cd backend
python app.py
```

### 5. Access the Application
Open your browser and navigate to:
```
http://localhost:5000
```

## Default Credentials

### Admin Login
- **Username**: `admin`
- **Password**: `admin123`

### Student Registration
- Register with your name, roll number, and section
- Default password: `12345`

## API Endpoints

### Authentication
- `POST /api/auth/admin/login` - Admin login
- `POST /api/auth/student/login` - Student login
- `POST /api/auth/student/register` - Student registration
- `POST /api/auth/logout` - Logout
- `GET /api/auth/check` - Check authentication status

### Companies
- `GET /api/companies` - List all companies
- `POST /api/companies` - Add company (admin)
- `DELETE /api/companies/<id>` - Remove company (admin)

### Students
- `GET /api/students` - List all students (admin)
- `GET /api/students/search?q=<query>` - Search students
- `PUT /api/students/<id>` - Update student profile
- `POST /api/students/<id>/apply/<company_id>` - Apply to company

### Worksheets
- `GET /api/worksheets` - List all worksheets
- `POST /api/worksheets` - Add worksheet (admin)
- `DELETE /api/worksheets/<id>` - Remove worksheet (admin)
- `POST /api/worksheets/<id>/complete` - Mark as complete

### Mock Interviews
- `GET /api/interviews` - List all interview slots
- `GET /api/interviews/upcoming` - List available slots
- `POST /api/interviews` - Add slot (admin)
- `DELETE /api/interviews/<id>` - Remove slot (admin)
- `POST /api/interviews/<id>/book` - Book a slot
- `POST /api/interviews/<id>/cancel` - Cancel booking

## Technology Stack

- **Backend**: Python Flask + C++ executables
- **Frontend**: HTML5, CSS3, JavaScript (ES6+)
- **Data Storage**: JSON files
- **Icons**: Font Awesome

## C++ Modules

1. **company_manager.cpp**: Handles company CRUD operations
   - Implements **Priority Queue (Max Heap)** for sorting companies by stipend (highest first)
2. **student_manager.cpp**: Manages student registration, login, and applications
   - Implements **Merge Sort** for sorting students by roll number
   - Implements **Binary Search** for efficient roll number search
3. **worksheet_manager.cpp**: Manages practice worksheets
4. **interview_manager.cpp**: Handles mock interview scheduling (max 3 students per slot)

## Algorithm Features

### Merge Sort (Student Display)
- **Purpose**: Sorts students by roll number when viewing the list
- **Time Complexity**: O(n log n)
- **Usage**: Automatically applied when listing students
- **Location**: `backend/cpp_source/student_manager.cpp`

### Binary Search (Student Search)
- **Purpose**: Efficiently searches for students by exact roll number
- **Time Complexity**: O(log n)
- **Fallback**: Falls back to partial match (substring) if no exact match
- **Usage**: Applied during search operations
- **Location**: `backend/cpp_source/student_manager.cpp`

### Priority Queue / Max Heap (Company Display)
- **Purpose**: Sorts companies by stipend in descending order (highest stipend first)
- **Time Complexity**: O(n log n)
- **Usage**: Automatically applied when listing companies
- **Location**: `backend/cpp_source/company_manager.cpp`
- **Benefit**: Students see the most lucrative opportunities at the top

For detailed algorithm documentation, see [ALGORITHMS.md](ALGORITHMS.md)

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is developed for DAA-PBL coursework.

---
**Note**: Make sure to compile the C++ files before running the Flask server. The application will show degraded status if executables are missing.
