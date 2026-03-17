"""
Placement Scheduler - Flask Backend Server

This server provides REST API endpoints and calls C++ executables for
data processing.
"""

from flask import Flask, request, jsonify, session, send_from_directory
from flask_cors import CORS
from werkzeug.utils import secure_filename
import subprocess
import os
import json
import time

app = Flask(__name__)
app.secret_key = 'placement_scheduler_secret_key_2024'
app.config['SESSION_COOKIE_SAMESITE'] = 'Lax'
app.config['SESSION_COOKIE_SECURE'] = False
CORS(app, supports_credentials=True)

# Paths configuration
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
CPP_EXECUTABLES = os.path.join(BASE_DIR, 'cpp_executables')
DATA_DIR = os.path.join(BASE_DIR, 'data')

# Data file paths
COMPANIES_FILE = os.path.join(DATA_DIR, 'companies.json')
STUDENTS_FILE = os.path.join(DATA_DIR, 'students.json')
WORKSHEETS_FILE = os.path.join(DATA_DIR, 'worksheets.json')
INTERVIEWS_FILE = os.path.join(DATA_DIR, 'interviews.json')
ADMINS_FILE = os.path.join(DATA_DIR, 'admins.json')

# Uploads directory for worksheets
UPLOADS_DIR = os.path.join(BASE_DIR, 'uploads')

# Ensure directories exist
os.makedirs(DATA_DIR, exist_ok=True)
os.makedirs(UPLOADS_DIR, exist_ok=True)

# Allowed file extensions for uploads
ALLOWED_EXTENSIONS = {'pdf', 'doc', 'docx', 'txt', 'xlsx', 'xls', 'ppt', 'pptx'}

def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1].lower() in ALLOWED_EXTENSIONS

# Initialize data files if they don't exist
for file_path in [COMPANIES_FILE, STUDENTS_FILE, WORKSHEETS_FILE, INTERVIEWS_FILE]:
    if not os.path.exists(file_path):
        with open(file_path, 'w') as f:
            f.write('[]')

# Initialize admins file with default admin
if not os.path.exists(ADMINS_FILE):
    with open(ADMINS_FILE, 'w') as f:
        json.dump([{"username": "admin", "password": "admin123"}], f)

def run_cpp_executable(executable_name, args):
    """Run a C++ executable with given arguments and return the output."""
    exe_path = os.path.join(CPP_EXECUTABLES, f'{executable_name}.exe')
    
    if not os.path.exists(exe_path):
        return {"error": f"Executable {executable_name} not found. Please compile the C++ files first."}
    
    try:
        cmd = [exe_path] + args
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
        
        if result.returncode != 0:
            return {"error": result.stderr or "Execution failed"}
        
        try:
            return json.loads(result.stdout)
        except json.JSONDecodeError:
            return {"raw_output": result.stdout}
            
    except subprocess.TimeoutExpired:
        return {"error": "Execution timed out"}
    except Exception as e:
        return {"error": str(e)}

# ============= Authentication Routes =============
@app.route('/api/auth/admin/login', methods=['POST'])
def admin_login():
    """Admin login endpoint."""
    try:
        data = request.json
        if not data:
            return jsonify({"success": False, "message": "No data received"}), 400
        
        username = data.get('username', '').strip()
        password = data.get('password', '').strip()
        
        print(f"Admin login attempt: username='{username}'")
        
        with open(ADMINS_FILE, 'r', encoding='utf-8') as f:
            admins = json.load(f)
        
        print(f"Loaded admins: {admins}")
        
        for admin in admins:
            if admin['username'] == username and admin['password'] == password:
                session['admin'] = username
                session['role'] = 'admin'
                return jsonify({"success": True, "message": "Login successful", "role": "admin"})
        
        return jsonify({"success": False, "message": "Invalid credentials"}), 401
    except Exception as e:
        print(f"Admin login error: {e}")
        return jsonify({"success": False, "message": str(e)}), 500

@app.route('/api/auth/student/register', methods=['POST'])
def student_register():
    """Student registration endpoint."""
    try:
        data = request.json
        if not data:
            return jsonify({"success": False, "message": "No data received"}), 400
        
        name = data.get('name', '').strip()
        roll_number = data.get('rollNumber', '').strip()
        section = data.get('section', '').strip()
        password = data.get('password', '12345').strip()
        contact = data.get('contact', '').strip()
        
        print(f"Registration attempt: name='{name}', roll='{roll_number}', section='{section}', contact='{contact}'")
        
        if not all([name, roll_number, section]):
            return jsonify({"success": False, "message": "Name, roll number, and section are required"}), 400
        
        result = run_cpp_executable('student_manager', [
            STUDENTS_FILE, 'register', name, roll_number, section, password, contact
        ])
        
        print(f"Registration result: {result}")
        
        if 'error' in result:
            return jsonify({"success": False, "message": result['error']}), 500
        
        return jsonify(result)
    except Exception as e:
        print(f"Registration error: {e}")
        return jsonify({"success": False, "message": str(e)}), 500

@app.route('/api/auth/student/login', methods=['POST'])
def student_login():
    """Student login endpoint."""
    try:
        data = request.json
        roll_number = data.get('rollNumber', '')
        password = data.get('password', '')
        
        if not roll_number or not password:
            return jsonify({"success": False, "message": "Roll number and password are required"}), 400
        
        result = run_cpp_executable('student_manager', [
            STUDENTS_FILE, 'login', roll_number, password
        ])
        
        print(f"Student login result: {result}")
        
        if result.get('success'):
            session['student_id'] = result['student']['id']
            session['roll_number'] = roll_number
            session['role'] = 'student'
            result['role'] = 'student'
        
        return jsonify(result)
    except Exception as e:
        print(f"Student login error: {e}")
        return jsonify({"success": False, "message": str(e)}), 500

@app.route('/api/auth/logout', methods=['POST'])
def logout():
    """Logout endpoint for both admin and student."""
    session.clear()
    return jsonify({"success": True, "message": "Logged out successfully"})

@app.route('/api/auth/check', methods=['GET'])
def check_auth():
    """Check authentication status."""
    if 'role' in session:
        return jsonify({
            "authenticated": True,
            "role": session['role'],
            "student_id": session.get('student_id'),
            "admin": session.get('admin')
        })
    return jsonify({"authenticated": False})

# ============= Company Routes =============
@app.route('/api/companies', methods=['GET'])
def get_companies():
    """Get all companies."""
    result = run_cpp_executable('company_manager', [COMPANIES_FILE, 'list'])
    return jsonify(result)

@app.route('/api/companies', methods=['POST'])
def add_company():
    """Add a new company (admin only)."""
    data = request.json
    name = data.get('name', '')
    stipend = data.get('stipend', '')
    prerequisites = data.get('prerequisites', '')
    visit_date = data.get('visitDate', '')
    status = data.get('status', 'upcoming')
    
    if not all([name, stipend, prerequisites, visit_date]):
        return jsonify({"success": False, "message": "All fields are required"}), 400
    
    result = run_cpp_executable('company_manager', [
        COMPANIES_FILE, 'add', name, stipend, prerequisites, visit_date, status
    ])
    
    return jsonify(result)

@app.route('/api/companies/<company_id>', methods=['DELETE'])
def delete_company(company_id):
    """Delete a company (admin only)."""
    result = run_cpp_executable('company_manager', [COMPANIES_FILE, 'remove', company_id])
    return jsonify(result)

@app.route('/api/companies/<company_id>', methods=['GET'])
def get_company(company_id):
    """Get a single company."""
    result = run_cpp_executable('company_manager', [COMPANIES_FILE, 'get', company_id])
    return jsonify(result)

# ============= Student Routes =============
@app.route('/api/students', methods=['GET'])
def get_students():
    """Get all students (admin only)."""
    result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'list'])
    return jsonify(result)

@app.route('/api/students/search', methods=['GET'])
def search_students():
    """Search students by roll number."""
    query = request.args.get('q', '')
    if not query:
        return jsonify([])
    
    result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'search', query])
    return jsonify(result)

@app.route('/api/students/<student_id>', methods=['GET'])
def get_student(student_id):
    """Get a single student."""
    result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'get', student_id])
    return jsonify(result)

@app.route('/api/students/<student_id>', methods=['PUT'])
def update_student(student_id):
    """Update student profile."""
    data = request.json
    name = data.get('name', '')
    cgpa = data.get('cgpa', '')
    contact = data.get('contact', '')
    section = data.get('section', '')
    
    result = run_cpp_executable('student_manager', [
        STUDENTS_FILE, 'update', student_id, name, cgpa, contact, section
    ])
    
    return jsonify(result)

# NEW ENDPOINT: Delete student
@app.route('/api/students/<student_id>', methods=['DELETE'])
def delete_student(student_id):
    """Delete a student (admin only)."""
    result = run_cpp_executable('student_manager', [STUDENTS_FILE, 'remove', student_id])
    return jsonify(result)

@app.route('/api/students/<student_id>/cgpa', methods=['PUT'])
def update_student_cgpa(student_id):
    """Update student CGPA."""
    data = request.json
    cgpa = data.get('cgpa', '').strip()
    
    if not cgpa:
        return jsonify({"success": False, "message": "CGPA is required"}), 400
    
    try:
        cgpa_float = float(cgpa)
        if cgpa_float < 0 or cgpa_float > 10:
            return jsonify({"success": False, "message": "CGPA must be between 0 and 10"}), 400
    except ValueError:
        return jsonify({"success": False, "message": "Invalid CGPA format"}), 400
    
    result = run_cpp_executable('student_manager', [
        STUDENTS_FILE, 'update_cgpa', student_id, cgpa
    ])
    
    return jsonify(result)

@app.route('/api/students/<student_id>/apply/<company_id>', methods=['POST'])
def apply_to_company(student_id, company_id):
    """Student applies to a company."""
    result = run_cpp_executable('student_manager', [
        STUDENTS_FILE, 'apply', student_id, company_id
    ])
    return jsonify(result)

# ============= Worksheet Routes =============
@app.route('/api/worksheets', methods=['GET'])
def get_worksheets():
    """Get all worksheets."""
    result = run_cpp_executable('worksheet_manager', [WORKSHEETS_FILE, 'list'])
    return jsonify(result)

@app.route('/api/worksheets', methods=['POST'])
def add_worksheet():
    """Add a new worksheet (admin only)."""
    if request.content_type and 'multipart/form-data' in request.content_type:
        title = request.form.get('title', '')
        description = request.form.get('description', '')
        topic = request.form.get('topic', '')
        difficulty = request.form.get('difficulty', 'medium')
        
        if not all([title, description, topic]):
            return jsonify({"success": False, "message": "Title, description and topic are required"}), 400
        
        file_link = ''
        if 'file' in request.files:
            file = request.files['file']
            if file and file.filename and allowed_file(file.filename):
                filename = secure_filename(file.filename)
                unique_filename = f"{int(time.time())}_{filename}"
                file_path = os.path.join(UPLOADS_DIR, unique_filename)
                file.save(file_path)
                file_link = f"/api/uploads/{unique_filename}"
        
        result = run_cpp_executable('worksheet_manager', [
            WORKSHEETS_FILE, 'add', title, description, topic, difficulty, file_link
        ])
        
        return jsonify(result)
    else:
        data = request.json
        title = data.get('title', '')
        description = data.get('description', '')
        topic = data.get('topic', '')
        difficulty = data.get('difficulty', 'medium')
        link = data.get('link', '')
        
        if not all([title, description, topic]):
            return jsonify({"success": False, "message": "Title, description and topic are required"}), 400
        
        result = run_cpp_executable('worksheet_manager', [
            WORKSHEETS_FILE, 'add', title, description, topic, difficulty, link
        ])
        
        return jsonify(result)

@app.route('/api/uploads/<filename>')
def serve_upload(filename):
    """Serve uploaded files."""
    return send_from_directory(UPLOADS_DIR, filename)

@app.route('/api/worksheets/<worksheet_id>', methods=['DELETE'])
def delete_worksheet(worksheet_id):
    """Delete a worksheet (admin only)."""
    result = run_cpp_executable('worksheet_manager', [WORKSHEETS_FILE, 'remove', worksheet_id])
    return jsonify(result)

@app.route('/api/worksheets/<worksheet_id>/complete', methods=['POST'])
def complete_worksheet(worksheet_id):
    """Mark worksheet as completed by student."""
    data = request.json
    student_id = data.get('studentId', '')
    
    if not student_id:
        return jsonify({"success": False, "message": "Student ID required"}), 400
    
    result = run_cpp_executable('student_manager', [
        STUDENTS_FILE, 'complete_worksheet', student_id, worksheet_id
    ])
    
    return jsonify(result)

# ============= Interview Routes =============
@app.route('/api/interviews', methods=['GET'])
def get_interviews():
    """Get all interview slots."""
    result = run_cpp_executable('interview_manager', [INTERVIEWS_FILE, 'list'])
    return jsonify(result)

@app.route('/api/interviews/upcoming', methods=['GET'])
def get_upcoming_interviews():
    """Get upcoming interview slots with available space."""
    result = run_cpp_executable('interview_manager', [INTERVIEWS_FILE, 'upcoming'])
    return jsonify(result)

@app.route('/api/interviews', methods=['POST'])
def add_interview_slot():
    """Add a new interview slot (admin only)."""
    data = request.json
    teacher_name = data.get('teacherName', '')
    date = data.get('date', '')
    start_time = data.get('startTime', '')
    end_time = data.get('endTime', '')
    venue = data.get('venue', '')
    
    if not all([teacher_name, date, start_time, end_time, venue]):
        return jsonify({"success": False, "message": "All fields are required"}), 400
    
    time_slot = f"{start_time}-{end_time}"
    result = run_cpp_executable('interview_manager', [
        INTERVIEWS_FILE, 'add_slot', teacher_name, date, time_slot, venue
    ])
    
    return jsonify(result)

@app.route('/api/interviews/<slot_id>', methods=['DELETE'])
def delete_interview_slot(slot_id):
    """Delete an interview slot (admin only)."""
    result = run_cpp_executable('interview_manager', [INTERVIEWS_FILE, 'remove_slot', slot_id])
    return jsonify(result)

@app.route('/api/interviews/<slot_id>/book', methods=['POST'])
def book_interview(slot_id):
    """Book an interview slot."""
    data = request.json
    student_id = data.get('studentId', '')
    
    if not student_id:
        return jsonify({"success": False, "message": "Student ID required"}), 400
    
    result = run_cpp_executable('interview_manager', [
        INTERVIEWS_FILE, 'book', slot_id, student_id
    ])
    
    return jsonify(result)

@app.route('/api/interviews/<slot_id>/cancel', methods=['POST'])
def cancel_interview(slot_id):
    """Cancel an interview booking."""
    data = request.json
    student_id = data.get('studentId', '')
    
    if not student_id:
        return jsonify({"success": False, "message": "Student ID required"}), 400
    
    result = run_cpp_executable('interview_manager', [
        INTERVIEWS_FILE, 'cancel', slot_id, student_id
    ])
    
    return jsonify(result)

@app.route('/api/interviews/student/<student_id>', methods=['GET'])
def get_student_bookings(student_id):
    """Get all interview bookings for a student."""
    result = run_cpp_executable('interview_manager', [
        INTERVIEWS_FILE, 'get_student_bookings', student_id
    ])
    return jsonify(result)

# ============= Static File Serving =============
@app.route('/')
def serve_frontend():
    """Serve the frontend index.html."""
    return send_from_directory('../frontend', 'index.html')

@app.route('/<path:path>')
def serve_static(path):
    """Serve static files from frontend directory."""
    return send_from_directory('../frontend', path)

# ============= Health Check =============
@app.route('/api/health', methods=['GET'])
def health_check():
    """Health check endpoint."""
    executables = ['company_manager', 'student_manager', 'worksheet_manager', 'interview_manager']
    missing = []
    
    for exe in executables:
        exe_path = os.path.join(CPP_EXECUTABLES, f'{exe}.exe')
        if not os.path.exists(exe_path):
            missing.append(exe)
    
    return jsonify({
        "status": "healthy" if not missing else "degraded",
        "missing_executables": missing,
        "message": "All systems operational" if not missing else f"Please compile C++ files: {', '.join(missing)}"
    })

if __name__ == '__main__':
    print("=" * 50)
    print("Placement Scheduler Backend Server")
    print("=" * 50)
    print(f"Data directory: {DATA_DIR}")
    print(f"C++ executables: {CPP_EXECUTABLES}")
    print("\nStarting server on http://localhost:5000")
    print("=" * 50)
    app.run(debug=True, port=5000)