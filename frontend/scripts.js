/**
 * Placement Scheduler - Frontend JavaScript
 * Handles all UI interactions and API calls
 */

// API Base URL - use relative path since served from same origin
const API_BASE = '/api';

// Current user state - load from localStorage if available
let currentUser = JSON.parse(localStorage.getItem('currentUser')) || null;
let currentStudentId = localStorage.getItem('currentStudentId') || null;
let currentRole = localStorage.getItem('currentRole') || null;

// Save session to localStorage
function saveSession() {
    if (currentUser) {
        localStorage.setItem('currentUser', JSON.stringify(currentUser));
    }
    if (currentStudentId) {
        localStorage.setItem('currentStudentId', currentStudentId);
    }
    if (currentRole) {
        localStorage.setItem('currentRole', currentRole);
    }
}

// Clear session from localStorage
function clearSession() {
    localStorage.removeItem('currentUser');
    localStorage.removeItem('currentStudentId');
    localStorage.removeItem('currentRole');
    currentUser = null;
    currentStudentId = null;
    currentRole = null;
}

// ============= Utility Functions =============
function showLoading() {
    document.getElementById('loadingOverlay').style.display = 'flex';
}

function hideLoading() {
    document.getElementById('loadingOverlay').style.display = 'none';
}

function showToast(message, type = 'success') {
    const toast = document.getElementById('notificationToast');
    const toastMessage = document.getElementById('toastMessage');
    const icon = toast.querySelector('i');
    
    toastMessage.textContent = message;
    toast.className = `toast show ${type}`;
    icon.className = type === 'success' ? 'fas fa-check-circle' : 'fas fa-exclamation-circle';
    
    setTimeout(() => {
        toast.classList.remove('show');
    }, 3000);
}

async function apiCall(endpoint, method = 'GET', data = null) {
    const options = {
        method,
        headers: {
            'Content-Type': 'application/json'
        }
    };
    
    if (data && method !== 'GET') {
        options.body = JSON.stringify(data);
        console.log(`API Call: ${method} ${endpoint}`, data);
    }
    
    try {
        const response = await fetch(`${API_BASE}${endpoint}`, options);
        const result = await response.json();
        console.log(`API Response: ${endpoint}`, result);
        return result;
    } catch (error) {
        console.error('API Error:', error);
        return { error: 'Network error. Please check if the server is running.' };
    }
}

// ============= Authentication =============
function showLoginForm(type) {
    hideAllForms();
    if (type === 'admin') {
        document.getElementById('adminLoginForm').style.display = 'flex';
    } else {
        document.getElementById('studentLoginForm').style.display = 'flex';
    }
}

function showSignupForm() {
    hideAllForms();
    document.getElementById('studentSignupForm').style.display = 'flex';
}

function hideAllForms() {
    document.getElementById('adminLoginForm').style.display = 'none';
    document.getElementById('studentLoginForm').style.display = 'none';
    document.getElementById('studentSignupForm').style.display = 'none';
}

async function adminLogin() {
    const username = document.getElementById('adminUsername').value.trim();
    const password = document.getElementById('adminPassword').value.trim();
    
    console.log('Admin login attempt:', { username, password });
    
    if (!username || !password) {
        showToast('Please fill in all fields', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall('/auth/admin/login', 'POST', { username, password });
    hideLoading();
    
    console.log('Admin login result:', result);
    
    if (result.success) {
        currentUser = { role: 'admin', username };
        currentRole = 'admin';
        saveSession();
        showToast('Login successful!');
        showApp('admin');
    } else {
        showToast(result.message || result.error || 'Login failed', 'error');
    }
}

async function studentLogin() {
    const rollNumber = document.getElementById('studentRoll').value.trim();
    const password = document.getElementById('studentPassword').value.trim();
    
    if (!rollNumber || !password) {
        showToast('Please fill in all fields', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall('/auth/student/login', 'POST', { rollNumber, password });
    hideLoading();
    
    console.log('Student login result:', result);
    
    if (result.success) {
        currentUser = result.student;
        currentStudentId = result.student.id;
        currentRole = 'student';
        saveSession();
        showToast('Login successful!');
        showApp('student');
    } else {
        showToast(result.message || 'Login failed', 'error');
    }
}

async function studentSignup() {
    const name = document.getElementById('signupName').value.trim();
    const rollNumber = document.getElementById('signupRoll').value.trim();
    const section = document.getElementById('signupSection').value.trim();
    const contact = document.getElementById('signupContact').value.trim();
    const passwordField = document.getElementById('signupPassword');
    const password = passwordField ? passwordField.value.trim() : '12345';
    
    console.log('Signup data:', { name, rollNumber, section, contact, password });
    
    if (!name || !rollNumber || !section) {
        showToast('Please fill in all required fields', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall('/auth/student/register', 'POST', {
        name, rollNumber, section, contact, password: password || '12345'
    });
    hideLoading();
    
    console.log('Signup result:', result);
    
    if (result.success) {
        showToast('Registration successful!');
        hideAllForms();
        showLoginForm('student');
    } else {
        showToast(result.message || result.error || 'Registration failed', 'error');
    }
}

async function logout() {
    await apiCall('/auth/logout', 'POST');
    clearSession();
    document.getElementById('appContainer').style.display = 'none';
    document.getElementById('loginPage').style.display = 'flex';
    showToast('Logged out successfully');
}

function showApp(role) {
    document.getElementById('loginPage').style.display = 'none';
    document.getElementById('appContainer').style.display = 'flex';
    hideAllForms();
    
    // Update navigation based on role
    const adminLinks = ['adminCompanyLink', 'adminWorksheetLink', 'adminStudentLink', 'adminInterviewLink'];
    const studentLinks = ['studentCompanyLink', 'studentWorksheetLink', 'studentInterviewLink', 'studentApplicationLink', 'studentProfileLink'];
    
    if (role === 'admin') {
        adminLinks.forEach(id => document.getElementById(id).style.display = 'block');
        studentLinks.forEach(id => document.getElementById(id).style.display = 'none');
        document.getElementById('dashboardTitle').textContent = 'Admin Dashboard';
        document.getElementById('userGreeting').textContent = `Welcome, Admin!`;
        loadAdminDashboard();
    } else {
        adminLinks.forEach(id => document.getElementById(id).style.display = 'none');
        studentLinks.forEach(id => document.getElementById(id).style.display = 'block');
        document.getElementById('dashboardTitle').textContent = 'Student Dashboard';
        document.getElementById('userGreeting').textContent = `Welcome, ${currentUser.name}!`;
        loadStudentDashboard();
    }
    
    showSection('dashboard');
}

// ============= Navigation =============
function showSection(sectionId) {
    document.querySelectorAll('.section').forEach(s => s.classList.remove('active'));
    document.getElementById(sectionId).classList.add('active');
    
    document.querySelectorAll('.nav-links a').forEach(a => a.classList.remove('active'));
    event?.target?.classList?.add('active');
    
    // Load data for the section
    switch(sectionId) {
        case 'dashboard':
            if (currentRole === 'admin') {
                loadAdminDashboard();
            } else {
                loadStudentDashboard();
            }
            break;
        case 'manageCompanies': loadCompanies(); break;
        case 'uploadWorksheet': loadWorksheets(); break;
        case 'viewStudents': loadStudents(); break;
        case 'manageInterviews': loadInterviews(); break;
        case 'upcomingCompanies': loadCompaniesForStudent(); break;
        case 'practiceWorksheets': loadWorksheetsForStudent(); break;
        case 'bookInterview': loadInterviewsForStudent(); break;
        case 'myApplications': loadMyApplications(); break;
        case 'myProfile': loadProfile(); break;
    }
}

function toggleMenu() {
    document.getElementById('navLinks').classList.toggle('active');
}

// ============= Dashboard =============
async function loadAdminDashboard() {
    const dashboardCards = document.getElementById('dashboardCards');
    
    // Fetch counts
    const [companies, students, worksheets, interviews] = await Promise.all([
        apiCall('/companies'),
        apiCall('/students'),
        apiCall('/worksheets'),
        apiCall('/interviews')
    ]);
    
    const companyCount = Array.isArray(companies) ? companies.length : 0;
    const studentCount = Array.isArray(students) ? students.length : 0;
    const worksheetCount = Array.isArray(worksheets) ? worksheets.length : 0;
    const interviewCount = Array.isArray(interviews) ? interviews.length : 0;
    
    dashboardCards.innerHTML = `
        <div class="dashboard-card" onclick="showSection('manageCompanies')">
            <i class="fas fa-building"></i>
            <h3>${companyCount}</h3>
            <p>Companies</p>
        </div>
        <div class="dashboard-card" onclick="showSection('viewStudents')">
            <i class="fas fa-users"></i>
            <h3>${studentCount}</h3>
            <p>Students</p>
        </div>
        <div class="dashboard-card" onclick="showSection('uploadWorksheet')">
            <i class="fas fa-file-alt"></i>
            <h3>${worksheetCount}</h3>
            <p>Worksheets</p>
        </div>
        <div class="dashboard-card" onclick="showSection('manageInterviews')">
            <i class="fas fa-calendar-alt"></i>
            <h3>${interviewCount}</h3>
            <p>Interview Slots</p>
        </div>
    `;
}

async function loadStudentDashboard() {
    const dashboardCards = document.getElementById('dashboardCards');
    
    const [companies, worksheets, interviews] = await Promise.all([
        apiCall('/companies'),
        apiCall('/worksheets'),
        apiCall('/interviews/upcoming')
    ]);
    
    const companyCount = Array.isArray(companies) ? companies.length : 0;
    const worksheetCount = Array.isArray(worksheets) ? worksheets.length : 0;
    const interviewCount = Array.isArray(interviews) ? interviews.length : 0;
    
    dashboardCards.innerHTML = `
        <div class="dashboard-card" onclick="showSection('upcomingCompanies')">
            <i class="fas fa-building"></i>
            <h3>${companyCount}</h3>
            <p>Upcoming Companies</p>
        </div>
        <div class="dashboard-card" onclick="showSection('practiceWorksheets')">
            <i class="fas fa-file-alt"></i>
            <h3>${worksheetCount}</h3>
            <p>Practice Worksheets</p>
        </div>
        <div class="dashboard-card" onclick="showSection('bookInterview')">
            <i class="fas fa-calendar-check"></i>
            <h3>${interviewCount}</h3>
            <p>Available Interviews</p>
        </div>
        <div class="dashboard-card" onclick="showSection('myProfile')">
            <i class="fas fa-user"></i>
            <h3>Profile</h3>
            <p>View Profile</p>
        </div>
    `;
}

// ============= Students =============
async function loadStudents() {
    showLoading();
    const students = await apiCall('/students');
    hideLoading();
    
    const tbody = document.getElementById('studentsTable');
    tbody.innerHTML = '';
    
    if (Array.isArray(students) && students.length > 0) {
        students.forEach(student => {
            // Skip admin users from deletion
            const isAdmin = student.rollNumber === 'ADMIN001' || student.section === 'ADMIN';
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${student.name}</td>
                <td>${student.rollNumber}</td>
                <td>${student.section}</td>
                <td>${student.cgpa || 'N/A'}</td>
                <td>${student.appliedCompanies ? student.appliedCompanies.split(',').length : 0}</td>
                <td>
                    <button class="btn btn-sm btn-info" onclick="viewStudentDetails('${student.id}')" title="View Details">
                        <i class="fas fa-eye"></i>
                    </button>
                    ${!isAdmin ? `
                        <button class="btn btn-sm btn-danger" onclick="removeStudent('${student.id}', '${student.name}')" title="Remove Student">
                            <i class="fas fa-trash"></i>
                        </button>
                    ` : `
                        <button class="btn btn-sm btn-secondary" disabled title="Admin Account">
                            <i class="fas fa-shield-alt"></i>
                        </button>
                    `}
                </td>
            `;
            tbody.appendChild(row);
        });
    } else {
        tbody.innerHTML = '<tr><td colspan="6" class="no-data">No students found</td></tr>';
    }
}

async function removeStudent(studentId, studentName) {
    if (!confirm(`Are you sure you want to remove student "${studentName}"?\n\nThis action will:\n• Remove all their applications\n• Cancel any interview bookings\n• Delete their account permanently\n\nThis action cannot be undone!`)) {
        return;
    }
    
    showLoading();
    
    try {
        // First, check if the student has any interview bookings and cancel them
        const bookings = await apiCall(`/interviews/student/${studentId}`);
        
        if (Array.isArray(bookings) && bookings.length > 0) {
            for (const booking of bookings) {
                await apiCall(`/interviews/${booking.id}/cancel`, 'POST', {
                    studentId: studentId
                });
            }
        }
        
        // Delete the student
        const result = await apiCall(`/students/${studentId}`, 'DELETE');
        
        hideLoading();
        
        if (result.success) {
            showToast(`Student "${studentName}" removed successfully!`);
            // Reload the students list
            await loadStudents();
        } else {
            showToast(result.message || 'Failed to remove student', 'error');
        }
        
    } catch (error) {
        hideLoading();
        console.error('Error removing student:', error);
        showToast('Failed to remove student. Please try again.', 'error');
    }
}

function searchStudents() {
    const query = document.getElementById('studentSearch').value.toLowerCase();
    const rows = document.querySelectorAll('#studentsTable tr');
    rows.forEach(row => {
        const text = row.textContent.toLowerCase();
        row.style.display = text.includes(query) ? '' : 'none';
    });
}

// ============= Student Details Modal =============
function viewStudentDetails(studentId) {
    // Open a modal with student details
    const modalHtml = `
        <div id="studentDetailsModal" class="modal" style="display: flex;">
            <div class="modal-content">
                <div class="modal-header">
                    <h3>Student Details</h3>
                    <button class="close-btn" onclick="closeStudentDetailsModal()">&times;</button>
                </div>
                <div class="modal-body">
                    <div id="studentDetailsContent">
                        <div class="loading-spinner">
                            <i class="fas fa-spinner fa-spin"></i>
                            <p>Loading student details...</p>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    `;
    
    // Remove existing modal if any
    const existingModal = document.getElementById('studentDetailsModal');
    if (existingModal) {
        existingModal.remove();
    }
    
    // Add modal to page
    document.body.insertAdjacentHTML('beforeend', modalHtml);
    
    // Load student details
    loadStudentDetails(studentId);
}

async function loadStudentDetails(studentId) {
    try {
        const student = await apiCall(`/students/${studentId}`);
        const content = document.getElementById('studentDetailsContent');
        
        if (student.error) {
            content.innerHTML = `<p class="no-data">Student not found</p>`;
            return;
        }
        
        // Get applied companies names
        let appliedCompaniesHtml = '<p class="no-data">No applications</p>';
        if (student.appliedCompanies) {
            const companyIds = student.appliedCompanies.split(',').filter(id => id.trim());
            if (companyIds.length > 0) {
                // Fetch all companies to get names
                const allCompanies = await apiCall('/companies');
                if (Array.isArray(allCompanies)) {
                    const companyNames = companyIds.map(id => {
                        const company = allCompanies.find(c => c.id === id);
                        return company ? company.name : `Company ID: ${id}`;
                    });
                    appliedCompaniesHtml = companyNames.map(name => `<span class="badge badge-info" style="margin: 2px;">${name}</span>`).join(' ');
                }
            }
        }
        
        // Get completed worksheets
        let completedWorksheetsHtml = '<p class="no-data">No worksheets completed</p>';
        if (student.completedWorksheets) {
            const worksheetIds = student.completedWorksheets.split(',').filter(id => id.trim());
            if (worksheetIds.length > 0) {
                completedWorksheetsHtml = `<p>Completed ${worksheetIds.length} worksheet(s)</p>`;
            }
        }
        
        content.innerHTML = `
            <div class="student-details">
                <div class="detail-item">
                    <span class="detail-label">Name:</span>
                    <span class="detail-value">${student.name}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Roll Number:</span>
                    <span class="detail-value">${student.rollNumber}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Section:</span>
                    <span class="detail-value">${student.section}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">CGPA:</span>
                    <span class="detail-value">${student.cgpa || 'N/A'}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Contact:</span>
                    <span class="detail-value">${student.contact || 'N/A'}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Applied Companies:</span>
                    <div class="detail-value" style="margin-top: 5px;">${appliedCompaniesHtml}</div>
                </div>
                <div class="detail-item">
                    <span class="detail-label">Completed Worksheets:</span>
                    <div class="detail-value">${completedWorksheetsHtml}</div>
                </div>
                <div class="modal-actions" style="margin-top: 20px; display: flex; gap: 10px;">
                    ${student.rollNumber !== 'ADMIN001' && student.section !== 'ADMIN' ? `
                        <button class="btn btn-danger" onclick="removeStudent('${student.id}', '${student.name}'); closeStudentDetailsModal();">
                            <i class="fas fa-trash"></i> Remove Student
                        </button>
                    ` : ''}
                    <button class="btn btn-outline" onclick="closeStudentDetailsModal()">
                        <i class="fas fa-times"></i> Close
                    </button>
                </div>
            </div>
        `;
        
    } catch (error) {
        console.error('Error loading student details:', error);
        const content = document.getElementById('studentDetailsContent');
        content.innerHTML = `<p class="no-data">Error loading student details: ${error.message}</p>`;
    }
}

function closeStudentDetailsModal() {
    const modal = document.getElementById('studentDetailsModal');
    if (modal) {
        modal.remove();
    }
}

// ============= Companies =============
async function loadCompanies() {
    console.log('loadCompanies() called');
    showLoading();
    // Fetch both companies and students to count applications
    const [companies, students] = await Promise.all([
        apiCall('/companies'),
        apiCall('/students')
    ]);
    hideLoading();
    console.log('Companies loaded:', companies?.length || 0, 'Students loaded:', students?.length || 0);
    
    // Count applications per company
    const applicationCounts = {};
    if (Array.isArray(students)) {
        students.forEach(student => {
            if (student.appliedCompanies) {
                const appliedIds = student.appliedCompanies.split(',').filter(id => id.trim());
                appliedIds.forEach(companyId => {
                    applicationCounts[companyId] = (applicationCounts[companyId] || 0) + 1;
                });
            }
        });
    }
    
    const tbody = document.getElementById('companiesTable');
    tbody.innerHTML = '';
    
    if (Array.isArray(companies) && companies.length > 0) {
        companies.forEach(company => {
            const appCount = applicationCounts[company.id] || 0;
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${company.name}</td>
                <td>₹${company.stipend}</td>
                <td>${company.visitDate}</td>
                <td><span class="badge badge-info">${appCount}</span></td>
                <td>
                    <button class="btn btn-sm btn-danger" onclick="removeCompany('${company.id}')">
                        <i class="fas fa-trash"></i>
                    </button>
                </td>
            `;
            tbody.appendChild(row);
        });
    } else {
        tbody.innerHTML = '<tr><td colspan="5" class="no-data">No companies found</td></tr>';
    }
}

async function loadCompaniesForStudent() {
    showLoading();
    const companies = await apiCall('/companies');
    hideLoading();
    
    const grid = document.getElementById('companiesGrid');
    grid.innerHTML = '';
    
    // Get list of companies the student has already applied to
    const appliedCompanies = currentUser?.appliedCompanies ? currentUser.appliedCompanies.split(',') : [];
    
    if (Array.isArray(companies) && companies.length > 0) {
        companies.forEach(company => {
            const hasApplied = appliedCompanies.includes(company.id);
            const card = document.createElement('div');
            card.className = 'card';
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="card-title">${company.name}</h3>
                    <span class="badge badge-success">₹${company.stipend}</span>
                </div>
                <div class="card-body">
                    <p><strong>Visit Date:</strong> ${company.visitDate}</p>
                    <p><strong>Prerequisites:</strong> ${company.prerequisites}</p>
                    <p><strong>Status:</strong> ${company.status}</p>
                </div>
                <div class="card-actions">
                    ${hasApplied ?
                        `<button class="btn btn-success" disabled>
                            <i class="fas fa-check"></i> Applied
                        </button>` :
                        `<button class="btn btn-primary" onclick="applyToCompany('${company.id}', '${company.name}')">
                            <i class="fas fa-edit"></i> Apply Now
                        </button>`
                    }
                </div>
            `;
            grid.appendChild(card);
        });
    } else {
        grid.innerHTML = '<p class="no-data">No upcoming companies</p>';
    }
}

function showAddCompanyModal() {
    document.getElementById('addCompanyModal').style.display = 'flex';
}

function hideAddCompanyModal() {
    document.getElementById('addCompanyModal').style.display = 'none';
}

async function addCompany() {
    const name = document.getElementById('modalCompanyName').value.trim();
    const stipend = document.getElementById('modalCompanyStipend').value.trim();
    const prerequisites = document.getElementById('modalCompanyPrerequisites').value.trim();
    const visitDate = document.getElementById('modalCompanyDeadline').value;
    
    if (!name || !stipend || !prerequisites || !visitDate) {
        showToast('Please fill in all fields', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall('/companies', 'POST', {
        name, stipend, prerequisites, visitDate, status: 'upcoming'
    });
    hideLoading();
    
    console.log('Add company result:', result);
    
    if (result.success) {
        // Clear form first
        document.getElementById('modalCompanyName').value = '';
        document.getElementById('modalCompanyStipend').value = '';
        document.getElementById('modalCompanyPrerequisites').value = '';
        document.getElementById('modalCompanyDeadline').value = '';
        // Hide modal
        hideAddCompanyModal();
        // Show success message
        showToast('Company added successfully!');
        // Small delay to ensure server has written the data, then reload
        console.log('Reloading companies list...');
        setTimeout(async () => {
            await loadCompanies();
            console.log('Companies list reloaded');
        }, 100);
    } else {
        showToast(result.message || 'Failed to add company', 'error');
    }
}

async function removeCompany(companyId) {
    if (!confirm('Are you sure you want to remove this company?')) return;
    
    showLoading();
    const result = await apiCall(`/companies/${companyId}`, 'DELETE');
    hideLoading();
    
    if (result.success) {
        showToast('Company removed successfully!');
        loadCompanies();
    } else {
        showToast(result.message || 'Failed to remove company', 'error');
    }
}

async function applyToCompany(companyId, companyName) {
    if (!currentStudentId) {
        showToast('Please log in as a student', 'error');
        return;
    }
    
    if (!confirm(`Apply to ${companyName}?`)) return;
    
    showLoading();
    const result = await apiCall(`/students/${currentStudentId}/apply/${companyId}`, 'POST');
    hideLoading();
    
    if (result.success) {
        showToast('Application submitted successfully!');
        // Update local user data
        if (currentUser.appliedCompanies) {
            currentUser.appliedCompanies += ',' + companyId;
        } else {
            currentUser.appliedCompanies = companyId;
        }
        saveSession();
        // Refresh the companies list to show updated status
        await loadCompaniesForStudent();
    } else {
        showToast(result.message || 'Failed to apply', 'error');
    }
}

function searchCompanies() {
    const query = document.getElementById('companySearch').value.toLowerCase();
    const rows = document.querySelectorAll('#companiesTable tr');
    rows.forEach(row => {
        const text = row.textContent.toLowerCase();
        row.style.display = text.includes(query) ? '' : 'none';
    });
}

// ============= Worksheets =============
async function loadWorksheets() {
    showLoading();
    // Fetch both worksheets and companies
    const [worksheets, companies] = await Promise.all([
        apiCall('/worksheets'),
        apiCall('/companies')
    ]);
    hideLoading();
    
    // Populate company dropdown
    const companySelect = document.getElementById('worksheetCompany');
    companySelect.innerHTML = '<option value="">Select Company</option>';
    if (Array.isArray(companies) && companies.length > 0) {
        companies.forEach(company => {
            const option = document.createElement('option');
            option.value = company.name;
            option.textContent = company.name;
            companySelect.appendChild(option);
        });
    }
    // Add "Others" option at the end
    const othersOption = document.createElement('option');
    othersOption.value = 'Others';
    othersOption.textContent = 'Others';
    companySelect.appendChild(othersOption);
    
    const grid = document.getElementById('worksheetsList');
    grid.innerHTML = '';
    
    if (Array.isArray(worksheets) && worksheets.length > 0) {
        worksheets.forEach(worksheet => {
            const card = document.createElement('div');
            card.className = 'card';
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="card-title">${worksheet.title}</h3>
                    <span class="badge badge-info">${worksheet.difficulty}</span>
                </div>
                <div class="card-body">
                    <p><strong>Topic:</strong> ${worksheet.topic}</p>
                    <p>${worksheet.description}</p>
                </div>
                <div class="card-actions">
                    ${worksheet.link ? `
                        <a href="${worksheet.link}" target="_blank" class="btn btn-sm btn-primary" download>
                            <i class="fas fa-download"></i> Download
                        </a>
                    ` : ''}
                    <button class="btn btn-sm btn-danger" onclick="removeWorksheet('${worksheet.id}')">
                        <i class="fas fa-trash"></i> Remove
                    </button>
                </div>
            `;
            grid.appendChild(card);
        });
    } else {
        grid.innerHTML = '<p class="no-data">No worksheets found</p>';
    }
}

async function loadWorksheetsForStudent() {
    showLoading();
    const worksheets = await apiCall('/worksheets');
    hideLoading();
    
    const grid = document.getElementById('studentWorksheetsGrid');
    grid.innerHTML = '';
    
    const completedIds = currentUser?.completedWorksheets?.split(',') || [];
    let completedCount = 0;
    
    if (Array.isArray(worksheets) && worksheets.length > 0) {
        worksheets.forEach(worksheet => {
            const isCompleted = completedIds.includes(worksheet.id);
            if (isCompleted) completedCount++;
            
            const card = document.createElement('div');
            card.className = 'card';
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="card-title">${worksheet.title}</h3>
                    <span class="badge badge-${isCompleted ? 'success' : 'warning'}">
                        ${isCompleted ? 'Completed' : 'Pending'}
                    </span>
                </div>
                <div class="card-body">
                    <p><strong>Topic:</strong> ${worksheet.topic}</p>
                    <p><strong>Difficulty:</strong> ${worksheet.difficulty}</p>
                    <p>${worksheet.description}</p>
                </div>
                <div class="card-actions">
                    ${worksheet.link ? `
                        <a href="${worksheet.link}" target="_blank" class="btn btn-sm btn-primary" download>
                            <i class="fas fa-download"></i> Download
                        </a>
                    ` : ''}
                    ${!isCompleted ? `
                        <button class="btn btn-success" onclick="markWorksheetDone('${worksheet.id}')">
                            <i class="fas fa-check"></i> Mark as Done
                        </button>
                    ` : '<span class="badge badge-success">✓ Completed</span>'}
                </div>
            `;
            grid.appendChild(card);
        });
        
        // Update progress
        const progress = worksheets.length > 0 ? Math.round((completedCount / worksheets.length) * 100) : 0;
        document.getElementById('completionRate').textContent = `${progress}% Complete`;
        document.getElementById('progressFill').style.width = `${progress}%`;
    } else {
        grid.innerHTML = '<p class="no-data">No worksheets available</p>';
    }
}

async function uploadWorksheet() {
    const title = document.getElementById('worksheetTitle').value.trim();
    const description = document.getElementById('worksheetDescription').value.trim();
    const topic = document.getElementById('worksheetCompany').value || 'General';
    const fileInput = document.getElementById('worksheetFile');
    const file = fileInput.files[0];
    
    if (!title || !description) {
        showToast('Please fill in required fields', 'error');
        return;
    }
    
    showLoading();
    
    // Use FormData for file upload
    const formData = new FormData();
    formData.append('title', title);
    formData.append('description', description);
    formData.append('topic', topic);
    formData.append('difficulty', 'medium');
    if (file) {
        formData.append('file', file);
    }
    
    try {
        const response = await fetch(`${API_BASE}/worksheets`, {
            method: 'POST',
            body: formData
        });
        const result = await response.json();
        hideLoading();
        
        if (result.success) {
            // Clear form
            document.getElementById('worksheetTitle').value = '';
            document.getElementById('worksheetDescription').value = '';
            document.getElementById('worksheetCompany').value = '';
            fileInput.value = '';
            document.getElementById('selectedFileName').textContent = 'No file selected';
            // Show success message
            showToast('Worksheet uploaded successfully!');
            // Reload worksheets list
            await loadWorksheets();
        } else {
            showToast(result.message || 'Failed to upload worksheet', 'error');
        }
    } catch (error) {
        hideLoading();
        console.error('Upload error:', error);
        showToast('Failed to upload worksheet', 'error');
    }
}

// File selection handler
function initFileUpload() {
    const fileInput = document.getElementById('worksheetFile');
    if (fileInput) {
        fileInput.addEventListener('change', function() {
            const fileName = this.files[0] ? this.files[0].name : 'No file selected';
            document.getElementById('selectedFileName').textContent = fileName;
        });
    }
}

async function removeWorksheet(worksheetId) {
    if (!confirm('Are you sure you want to remove this worksheet?')) return;
    
    showLoading();
    const result = await apiCall(`/worksheets/${worksheetId}`, 'DELETE');
    hideLoading();
    
    if (result.success) {
        showToast('Worksheet removed successfully!');
        loadWorksheets();
    } else {
        showToast(result.message || 'Failed to remove worksheet', 'error');
    }
}

async function markWorksheetDone(worksheetId) {
    if (!currentStudentId) {
        showToast('Please log in as a student', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall(`/worksheets/${worksheetId}/complete`, 'POST', {
        studentId: currentStudentId
    });
    hideLoading();
    
    if (result.success) {
        showToast('Worksheet marked as complete!');
        // Update local user data
        if (currentUser.completedWorksheets) {
            currentUser.completedWorksheets += ',' + worksheetId;
        } else {
            currentUser.completedWorksheets = worksheetId;
        }
        loadWorksheetsForStudent();
    } else {
        showToast(result.message || 'Failed to mark worksheet', 'error');
    }
}

function filterWorksheets() {
    const filter = document.getElementById('worksheetFilter').value;
    const cards = document.querySelectorAll('#studentWorksheetsGrid .card');
    
    cards.forEach(card => {
        const badge = card.querySelector('.badge');
        const isCompleted = badge && badge.textContent.includes('Completed');
        
        if (filter === 'all') {
            card.style.display = '';
        } else if (filter === 'completed') {
            card.style.display = isCompleted ? '' : 'none';
        } else {
            card.style.display = !isCompleted ? '' : 'none';
        }
    });
}

// ============= Interviews =============
async function loadInterviews() {
    showLoading();
    const interviews = await apiCall('/interviews');
    hideLoading();
    
    const grid = document.getElementById('interviewsGrid');
    grid.innerHTML = '';
    
    if (Array.isArray(interviews) && interviews.length > 0) {
        interviews.forEach(interview => {
            const bookedCount = interview.totalSlots - interview.availableSlots;
            const card = document.createElement('div');
            card.className = 'card';
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="card-title" style="cursor: pointer; color: var(--primary-color);"
                        onclick="viewBookedStudents('${interview.id}', '${interview.teacherName}', '${interview.bookedStudents || ''}')">
                        ${interview.teacherName}
                        ${bookedCount > 0 ? `<small style="font-size: 0.7em; color: var(--gray-color);"> (${bookedCount} booked - click to view)</small>` : ''}
                    </h3>
                    <span class="badge badge-${interview.availableSlots > 0 ? 'success' : 'danger'}">
                        ${interview.availableSlots}/${interview.totalSlots} slots
                    </span>
                </div>
                <div class="card-body">
                    <p><i class="fas fa-calendar"></i> ${interview.date}</p>
                    <p><i class="fas fa-clock"></i> ${interview.time}</p>
                    <p><i class="fas fa-map-marker-alt"></i> ${interview.venue}</p>
                </div>
                <div class="card-actions">
                    <button class="btn btn-sm btn-info"
                        onclick="viewBookedStudents('${interview.id}', '${interview.teacherName}', '${interview.bookedStudents || ''}')">
                        <i class="fas fa-users"></i> View Booked
                    </button>
                    <button class="btn btn-sm btn-danger" onclick="removeInterview('${interview.id}')">
                        <i class="fas fa-trash"></i> Remove
                    </button>
                </div>
            `;
            grid.appendChild(card);
        });
    } else {
        grid.innerHTML = '<p class="no-data">No interview slots scheduled</p>';
    }
}

async function loadInterviewsForStudent() {
    showLoading();
    const [interviews, myBookings] = await Promise.all([
        apiCall('/interviews/upcoming'),
        currentStudentId ? apiCall(`/interviews/student/${currentStudentId}`) : []
    ]);
    hideLoading();
    
    // Available interviews
    const availableGrid = document.getElementById('availableInterviews');
    availableGrid.innerHTML = '';
    
    if (Array.isArray(interviews) && interviews.length > 0) {
        interviews.forEach(interview => {
            const card = document.createElement('div');
            card.className = 'card';
            card.innerHTML = `
                <div class="card-header">
                    <h3 class="card-title">${interview.teacherName}</h3>
                    <span class="badge badge-success">${interview.availableSlots} slots left</span>
                </div>
                <div class="card-body">
                    <p><i class="fas fa-calendar"></i> ${interview.date}</p>
                    <p><i class="fas fa-clock"></i> ${interview.time}</p>
                    <p><i class="fas fa-map-marker-alt"></i> ${interview.venue}</p>
                </div>
                <div class="card-actions">
                    <button class="btn btn-primary" onclick="bookInterview('${interview.id}')">
                        <i class="fas fa-calendar-check"></i> Book Slot
                    </button>
                </div>
            `;
            availableGrid.appendChild(card);
        });
    } else {
        availableGrid.innerHTML = '<p class="no-data">No available slots</p>';
    }
    
    // My bookings
    const bookingsList = document.getElementById('myBookingsList');
    bookingsList.innerHTML = '';
    
    if (Array.isArray(myBookings) && myBookings.length > 0) {
        myBookings.forEach(booking => {
            const item = document.createElement('div');
            item.className = 'booking-item';
            item.innerHTML = `
                <div>
                    <strong>${booking.teacherName}</strong>
                    <p>${booking.date} at ${booking.time}</p>
                    <small>${booking.venue}</small>
                </div>
                <button class="btn btn-sm btn-danger" onclick="cancelInterview('${booking.id}')">
                    <i class="fas fa-times"></i>
                </button>
            `;
            bookingsList.appendChild(item);
        });
    } else {
        bookingsList.innerHTML = '<p class="no-data">No bookings yet</p>';
    }
}

function showScheduleInterviewModal() {
    document.getElementById('scheduleInterviewModal').style.display = 'flex';
}

function hideScheduleInterviewModal() {
    document.getElementById('scheduleInterviewModal').style.display = 'none';
}

async function scheduleInterview() {
    const teacherName = document.getElementById('modalTeacherName').value.trim();
    const date = document.getElementById('modalInterviewDate').value;
    const startTime = document.getElementById('modalInterviewStartTime').value;
    const endTime = document.getElementById('modalInterviewEndTime').value;
    const venue = document.getElementById('modalInterviewVenue').value.trim();
    
    if (!teacherName || !date || !startTime || !endTime || !venue) {
        showToast('Please fill in all fields', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall('/interviews', 'POST', {
        teacherName, date, startTime, endTime, venue
    });
    hideLoading();
    
    if (result.success) {
        // Clear form first
        document.getElementById('modalTeacherName').value = '';
        document.getElementById('modalInterviewDate').value = '';
        document.getElementById('modalInterviewStartTime').value = '';
        document.getElementById('modalInterviewEndTime').value = '';
        document.getElementById('modalInterviewVenue').value = '';
        // Hide modal
        hideScheduleInterviewModal();
        // Show success
        showToast('Interview slot scheduled successfully!');
        // Reload interviews list
        await loadInterviews();
    } else {
        showToast(result.message || 'Failed to schedule interview', 'error');
    }
}

async function removeInterview(interviewId) {
    if (!confirm('Are you sure you want to remove this interview slot?')) return;
    
    showLoading();
    const result = await apiCall(`/interviews/${interviewId}`, 'DELETE');
    hideLoading();
    
    if (result.success) {
        showToast('Interview slot removed successfully!');
        loadInterviews();
    } else {
        showToast(result.message || 'Failed to remove interview', 'error');
    }
}

async function bookInterview(slotId) {
    if (!currentStudentId) {
        showToast('Please log in as a student', 'error');
        return;
    }
    
    if (!confirm('Book this interview slot?')) return;
    
    showLoading();
    const result = await apiCall(`/interviews/${slotId}/book`, 'POST', {
        studentId: currentStudentId
    });
    hideLoading();
    
    if (result.success) {
        showToast('Interview booked successfully!');
        loadInterviewsForStudent();
    } else {
        showToast(result.message || 'Failed to book interview', 'error');
    }
}

async function cancelInterview(slotId) {
    if (!currentStudentId) return;
    
    if (!confirm('Cancel this booking?')) return;
    
    showLoading();
    const result = await apiCall(`/interviews/${slotId}/cancel`, 'POST', {
        studentId: currentStudentId
    });
    hideLoading();
    
    if (result.success) {
        showToast('Booking cancelled successfully!');
        loadInterviewsForStudent();
    } else {
        showToast(result.message || 'Failed to cancel booking', 'error');
    }
}

// Store current interview being viewed
let currentViewingInterviewId = null;

async function viewBookedStudents(interviewId, teacherName, bookedStudentsStr) {
    currentViewingInterviewId = interviewId;
    
    // Get booked student IDs
    const studentIds = bookedStudentsStr ? bookedStudentsStr.split(',').filter(id => id.trim()) : [];
    
    if (studentIds.length === 0) {
        showToast('No students have booked this slot yet', 'error');
        return;
    }
    
    showLoading();
    
    // Fetch all students to get their details
    const allStudents = await apiCall('/students');
    hideLoading();
    
    // Filter to get booked students
    const bookedStudents = Array.isArray(allStudents)
        ? allStudents.filter(s => studentIds.includes(s.id))
        : [];
    
    // Show modal with booked students
    let modalHtml = `
        <div id="bookedStudentsModal" class="modal" style="display: flex;">
            <div class="modal-content">
                <div class="modal-header">
                    <h3>Students Booked with ${teacherName}</h3>
                    <button class="close-btn" onclick="closeBookedStudentsModal()">&times;</button>
                </div>
                <div class="modal-body">
                    <p><strong>Total Booked:</strong> ${bookedStudents.length} / 3</p>
                    <div class="booked-students-list">
    `;
    
    if (bookedStudents.length > 0) {
        bookedStudents.forEach(student => {
            modalHtml += `
                <div class="booking-item" style="margin-bottom: 10px; padding: 15px; background: var(--light-color); border-radius: var(--radius);">
                    <div style="display: flex; justify-content: space-between; align-items: center;">
                        <div>
                            <strong>${student.name}</strong>
                            <p style="margin: 5px 0;">Roll: ${student.rollNumber} | Section: ${student.section}</p>
                            <small>Contact: ${student.contact || 'N/A'}</small>
                        </div>
                        <button class="btn btn-sm btn-warning" onclick="removeStudentFromSlot('${interviewId}', '${student.id}')">
                            <i class="fas fa-user-minus"></i> Remove
                        </button>
                    </div>
                </div>
            `;
        });
    } else {
        modalHtml += '<p class="no-data">No students found</p>';
    }
    
    modalHtml += `
                    </div>
                </div>
            </div>
        </div>
    `;
    
    // Remove existing modal if any
    const existingModal = document.getElementById('bookedStudentsModal');
    if (existingModal) {
        existingModal.remove();
    }
    
    // Add modal to page
    document.body.insertAdjacentHTML('beforeend', modalHtml);
}

function closeBookedStudentsModal() {
    const modal = document.getElementById('bookedStudentsModal');
    if (modal) {
        modal.remove();
    }
}

async function removeStudentFromSlot(interviewId, studentId) {
    if (!confirm('Remove this student from the interview slot?')) return;
    
    showLoading();
    const result = await apiCall(`/interviews/${interviewId}/cancel`, 'POST', {
        studentId: studentId
    });
    hideLoading();
    
    if (result.success) {
        showToast('Student removed from slot successfully!');
        closeBookedStudentsModal();
        loadInterviews();
    } else {
        showToast(result.message || 'Failed to remove student', 'error');
    }
}

// ============= My Applications =============
async function loadMyApplications() {
    if (!currentStudentId) return;
    
    showLoading();
    const student = await apiCall(`/students/${currentStudentId}`);
    hideLoading();
    
    const applicationsList = document.getElementById('applicationsList');
    applicationsList.innerHTML = '';
    
    if (student.appliedCompanies) {
        const companyIds = student.appliedCompanies.split(',').filter(id => id.trim());
        
        if (companyIds.length > 0) {
            // Fetch all companies to get names
            showLoading();
            const allCompanies = await apiCall('/companies');
            hideLoading();
            
            if (Array.isArray(allCompanies)) {
                companyIds.forEach(companyId => {
                    const company = allCompanies.find(c => c.id === companyId);
                    if (company) {
                        const appItem = document.createElement('div');
                        appItem.className = 'application-item';
                        appItem.innerHTML = `
                            <div class="application-header">
                                <h4>${company.name}</h4>
                                <span class="badge badge-success">Applied</span>
                            </div>
                            <div class="application-body">
                                <p><strong>Stipend:</strong> ₹${company.stipend}</p>
                                <p><strong>Visit Date:</strong> ${company.visitDate}</p>
                                <p><strong>Status:</strong> ${company.status}</p>
                            </div>
                        `;
                        applicationsList.appendChild(appItem);
                    }
                });
            }
        } else {
            applicationsList.innerHTML = '<p class="no-data">You have not applied to any companies yet.</p>';
        }
    } else {
        applicationsList.innerHTML = '<p class="no-data">You have not applied to any companies yet.</p>';
    }
}

// ============= Profile =============
async function loadProfile() {
    if (!currentUser) return;
    
    // Update basic info
    document.getElementById('profileName').textContent = currentUser.name || 'N/A';
    document.getElementById('profileRoll').textContent = `Roll Number: ${currentUser.rollNumber || 'N/A'}`;
    document.getElementById('profileSection').textContent = `Section: ${currentUser.section || 'N/A'}`;
    document.getElementById('profileCgpa').textContent = currentUser.cgpa || 'N/A';
    document.getElementById('profileCgpaDisplay').textContent = currentUser.cgpa || 'N/A';
    document.getElementById('profileContact').textContent = currentUser.contact || 'N/A';
    document.getElementById('profileRole').textContent = currentRole === 'admin' ? 'Admin' : 'Student';
    
    // Calculate counts
    const appliedCount = currentUser.appliedCompanies ? 
        currentUser.appliedCompanies.split(',').filter(x => x).length : 0;
    const completedCount = currentUser.completedWorksheets ? 
        currentUser.completedWorksheets.split(',').filter(x => x).length : 0;
    
    // FETCH ACTUAL BOOKED INTERVIEW COUNT
    let bookedCount = 0;
    if (currentStudentId) {
        try {
            const bookings = await apiCall(`/interviews/student/${currentStudentId}`);
            if (Array.isArray(bookings)) {
                bookedCount = bookings.length;
            }
        } catch (error) {
            console.error('Error fetching interview bookings:', error);
        }
    }
    
    // Update UI
    document.getElementById('appliedCompanies').textContent = appliedCount;
    document.getElementById('completedWorksheets').textContent = completedCount;
    document.getElementById('bookedInterviews').textContent = bookedCount;
}

// NEW FUNCTIONS for CGPA editing
function enableCgpaEdit() {
    document.getElementById('profileCgpaDisplay').style.display = 'none';
    document.getElementById('cgpaEditForm').style.display = 'flex';
    document.getElementById('cgpaInput').value = currentUser.cgpa || '';
}

function cancelCgpaEdit() {
    document.getElementById('profileCgpaDisplay').style.display = 'block';
    document.getElementById('cgpaEditForm').style.display = 'none';
}

async function updateCgpa() {
    const cgpa = document.getElementById('cgpaInput').value.trim();
    
    if (!cgpa) {
        showToast('Please enter a CGPA value', 'error');
        return;
    }
    
    // Validate CGPA
    const cgpaNum = parseFloat(cgpa);
    if (isNaN(cgpaNum) || cgpaNum < 0 || cgpaNum > 10) {
        showToast('CGPA must be a number between 0 and 10', 'error');
        return;
    }
    
    if (!currentStudentId) {
        showToast('Please log in to update profile', 'error');
        return;
    }
    
    showLoading();
    const result = await apiCall(`/students/${currentStudentId}/cgpa`, 'PUT', { cgpa });
    hideLoading();
    
    if (result.success) {
        // Update local user data
        currentUser.cgpa = cgpa;
        saveSession();
        
        // Update UI
        document.getElementById('profileCgpa').textContent = cgpa;
        document.getElementById('profileCgpaDisplay').textContent = cgpa;
        
        // Hide edit form
        cancelCgpaEdit();
        
        showToast('CGPA updated successfully!');
    } else {
        showToast(result.message || 'Failed to update CGPA', 'error');
    }
}

// ============= Modal Helpers =============
function hideApplyCompanyModal() {
    document.getElementById('applyCompanyModal').style.display = 'none';
}

function hideViewApplicationsModal() {
    document.getElementById('viewApplicationsModal').style.display = 'none';
}

// Close modals on outside click
window.onclick = function(event) {
    const modals = document.querySelectorAll('.modal, .auth-modal');
    modals.forEach(modal => {
        if (event.target === modal) {
            modal.style.display = 'none';
        }
    });
};

// ============= Initialize =============
document.addEventListener('DOMContentLoaded', function() {
    // Check for existing session from localStorage
    checkAuth();
    // Initialize file upload handler
    initFileUpload();
});

async function checkAuth() {
    // First check localStorage
    if (currentRole && currentUser) {
        if (currentRole === 'admin') {
            showApp('admin');
            return;
        } else if (currentRole === 'student' && currentStudentId) {
            showApp('student');
            return;
        }
    }
    
    // Fallback to server check
    const result = await apiCall('/auth/check');
    if (result.authenticated) {
        if (result.role === 'admin') {
            currentUser = { role: 'admin', username: result.admin };
            currentRole = 'admin';
            saveSession();
            showApp('admin');
        } else if (result.role === 'student') {
            currentStudentId = result.student_id;
            currentRole = 'student';
            const student = await apiCall(`/students/${result.student_id}`);
            currentUser = student;
            saveSession();
            showApp('student');
        }
    }
}