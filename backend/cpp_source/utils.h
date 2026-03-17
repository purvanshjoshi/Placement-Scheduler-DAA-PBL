#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace Utils {
    std::string getCurrentDate() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        
        std::stringstream ss;
        ss << (now->tm_year + 1900) << '-'
           << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << '-'
           << std::setw(2) << std::setfill('0') << now->tm_mday;
        return ss.str();
    }

    std::string getCurrentDateTime() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        
        std::stringstream ss;
        ss << getCurrentDate() << " "
           << std::setw(2) << std::setfill('0') << now->tm_hour << ":"
           << std::setw(2) << std::setfill('0') << now->tm_min << ":"
           << std::setw(2) << std::setfill('0') << now->tm_sec;
        return ss.str();
    }

    bool validateEmail(const std::string& email) {
        return email.find('@') != std::string::npos;
    }

    bool validatePhone(const std::string& phone) {
        return phone.length() == 10 &&
               std::all_of(phone.begin(), phone.end(), ::isdigit);
    }
};

#endif