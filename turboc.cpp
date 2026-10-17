// Employee Management System - C++ Micro Project
// Features:
// Add, View, Search, Update, Delete, Sort, Salary Report
// Delete All Employees
// Project Group Info (option 10)
// Data is saved in "employees.txt"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <string>
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const string DATA_FILE = "employees.txt";

// ---------------------------------------------------------------
// UI Helpers
// ---------------------------------------------------------------
namespace UI {

const string RESET   = "\033[0m";
const string BOLD    = "\033[1m";
const string DIM     = "\033[2m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN    = "\033[36m";

void setup()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | 0x0004);
#endif
}

string rep(const string &s, int n)
{
    string r;

    for (int i = 0; i < n; i++)
        r += s;

    return r;
}

size_t dispLen(const string &s)
{
    size_t n = 0;

    for (unsigned char c : s)
    {
        if ((c & 0xC0) != 0x80)
            n++;
    }

    return n;
}

string padR(const string &s, size_t w)
{
    size_t l = dispLen(s);

    return l >= w ? s : s + string(w - l, ' ');
}

string padL(const string &s, size_t w)
{
    size_t l = dispLen(s);

    return l >= w ? s : string(w - l, ' ') + s;
}

string cut(const string &s, size_t w)
{
    if (dispLen(s) <= w)
        return s;

    string r;
    size_t n = 0;

    for (size_t i = 0; i < s.size();)
    {
        unsigned char c = s[i];
        size_t len = 1;

        if (c >= 0xF0)
            len = 4;
        else if (c >= 0xE0)
            len = 3;
        else if (c >= 0xC0)
            len = 2;

        if (n >= w - 3)
            break;

        r += s.substr(i, len);
        i += len;
        n++;
    }

    return r + "...";
}

string money(double v)
{
    ostringstream o;

    o << "₹" << fixed << setprecision(2) << v;

    return o.str();
}

void clearScreen()
{
    cout << "\033[2J\033[H";
}

void pause()
{
    cout << DIM
         << "\n  ⏎ Press Enter to continue..."
         << RESET;

    string s;
    getline(cin, s);
}

void success(const string &m)
{
    cout << GREEN << BOLD
         << "  ✔ " << m
         << RESET << "\n";
}

void error(const string &m)
{
    cout << RED << BOLD
         << "  ✖ " << m
         << RESET << "\n";
}

void warning(const string &m)
{
    cout << YELLOW
         << "  ⚠ " << m
         << RESET << "\n";
}

void info(const string &m)
{
    cout << CYAN
         << "  ℹ " << m
         << RESET << "\n";
}

void section(const string &title)
{
    cout << "\n"
         << MAGENTA << BOLD
         << "  " << title
         << RESET << "\n";

    cout << MAGENTA
         << "  " << rep("─", 44)
         << RESET << "\n";
}

void banner()
{
    cout << CYAN << BOLD;

    cout << "  ╔" << rep("═", 46) << "╗\n";

    cout << "  ║"
         << string(7, ' ')
         << "★  EMPLOYEE MANAGEMENT SYSTEM  ★"
         << string(7, ' ')
         << "║\n";

    cout << "  ╚" << rep("═", 46) << "╝\n";

    cout << RESET;
}

} // namespace UI


// ---------------------------------------------------------------
// Employee Class
// ---------------------------------------------------------------
class Employee
{
private:

    int id;

    string name;
    string department;
    string designation;

    double salary;

public:

    Employee()
        : id(0), salary(0.0)
    {
    }

    Employee(
        int i,
        const string &n,
        const string &d,
        const string &des,
        double s)
        : id(i),
          name(n),
          department(d),
          designation(des),
          salary(s)
    {
    }

    int getId() const
    {
        return id;
    }

    string getName() const
    {
        return name;
    }

    string getDepartment() const
    {
        return department;
    }

    string getDesignation() const
    {
        return designation;
    }

    double getSalary() const
    {
        return salary;
    }

    void setName(const string &n)
    {
        name = n;
    }

    void setDepartment(const string &d)
    {
        department = d;
    }

    void setDesignation(const string &d)
    {
        designation = d;
    }

    void setSalary(double s)
    {
        salary = s;
    }

    // Convert employee object into file format
    string toLine() const
    {
        ostringstream out;

        out << id << '|'
            << name << '|'
            << department << '|'
            << designation << '|'
            << fixed << setprecision(2)
            << salary;

        return out.str();
    }

    // Read employee from file
    static bool fromLine(
        const string &line,
        Employee &e)
    {
        stringstream ss(line);

        string idStr;
        string n;
        string d;
        string des;
        string salStr;

        if (!getline(ss, idStr, '|') ||
            !getline(ss, n, '|') ||
            !getline(ss, d, '|') ||
            !getline(ss, des, '|') ||
            !getline(ss, salStr, '|'))
        {
            return false;
        }

        try
        {
            e = Employee(
                stoi(idStr),
                n,
                d,
                des,
                stod(salStr));
        }
        catch (...)
        {
            return false;
        }

        return true;
    }
};


// ---------------------------------------------------------------
// Project Group Info  (Menu option 10)
// ---------------------------------------------------------------
// >>> EDIT ONLY THIS SECTION to fill in your group's data <<<

struct GroupMember
{
    string rollNo;
    string name;
    string enrollmentNo;
    string className;
};

// One line per student. Class is "SYCM" for everyone.
// Add or remove lines to match your group size.
const vector<GroupMember> GROUP_MEMBERS = {
    { "CM61", "TATE DESHMUKH SUMIT KISAN",   "25212440287", "SYCM" },
    { "CM63", "WAGH OM DIPAK",               "25212440289", "SYCM" },
    { "CM72", "LONDHE SHRIKANT CHANDRAKANT", "26212440294", "SYCM" },
    { "CM73", "DONGARE VIKRANT VIJAY",       "26212440295", "SYCM" }
};

// Separate project information
const string PROJECT_TITLE      = "Employee Management System";
const string PROJECT_TYPE       = "Micro Project (Console Application)";
const string PROJECT_SUBJECT    = "Object Oriented Programming using C++";
const string PROJECT_LANGUAGE   = "C++ (OOP, File Handling, STL)";
const string PROJECT_INSTITUTE  = "Karmayogi Institute of Technology, Pandharpur";
const string PROJECT_PROGRAMME  = "Diploma in Computer Technology (MSBTE)";
const string PROJECT_SEMESTER   = "Semester III";
const string PROJECT_YEAR       = "2026-27";
const string PROJECT_GUIDE      = "MR. BHOSALE S.D";   // guide teacher name
const string PROJECT_AIM        =
    "To manage employee records (add, search, update, delete, report) "
    "with permanent storage in a text file.";
const string PROJECT_CONCEPTS   =
    "Classes & objects, encapsulation, vector, lambda, sort, file I/O";
const string PROJECT_FEATURES   =
    "Add, View, Search, Update, Delete, Sort, Salary Report";

void showGroupInfo()
{
    UI::section("👥  Project Group Information");

    // Column widths
    const int W_SR = 4, W_ROLL = 8, W_NAME = 30, W_ENR = 14, W_CLS = 8;

    auto line = [&](const string &l, const string &m, const string &r)
    {
        cout << UI::CYAN << "  " << l
             << UI::rep("─", W_SR + 2)   << m
             << UI::rep("─", W_ROLL + 2) << m
             << UI::rep("─", W_NAME + 2) << m
             << UI::rep("─", W_ENR + 2)  << m
             << UI::rep("─", W_CLS + 2)  << r
             << UI::RESET << "\n";
    };

    const string bar = UI::CYAN + "│" + UI::RESET;

    auto row = [&](const string &a, const string &b, const string &c,
                   const string &d, const string &e, const string &style)
    {
        cout << "  " << bar
             << style << " " << UI::padR(a, W_SR)   << " " << UI::RESET << bar
             << style << " " << UI::padR(b, W_ROLL) << " " << UI::RESET << bar
             << style << " " << UI::padR(UI::cut(c, W_NAME), W_NAME) << " " << UI::RESET << bar
             << style << " " << UI::padR(d, W_ENR)  << " " << UI::RESET << bar
             << style << " " << UI::padR(e, W_CLS)  << " " << UI::RESET << bar
             << "\n";
    };

    line("┌", "┬", "┐");
    row("Sr", "Roll No", "Name", "Enrollment No", "Class", UI::BOLD);
    line("├", "┼", "┤");

    int sr = 1;
    for (const auto &m : GROUP_MEMBERS)
        row(to_string(sr++), m.rollNo, m.name, m.enrollmentNo, m.className, UI::GREEN);

    line("└", "┴", "┘");

    UI::info("Total group members: " + to_string(GROUP_MEMBERS.size()));

    // ---- Separate project information box ----
    UI::section("📁  Project Information");

    auto info = [](const string &label, const string &value)
    {
        cout << "  " << UI::YELLOW << UI::padR(label, 16) << UI::RESET
             << ": " << value << "\n";
    };

    info("Project Title", PROJECT_TITLE);
    info("Project Type", PROJECT_TYPE);
    info("Subject", PROJECT_SUBJECT);
    info("Language", PROJECT_LANGUAGE);
    info("Institute", PROJECT_INSTITUTE);
    info("Programme", PROJECT_PROGRAMME);
    info("Semester", PROJECT_SEMESTER);
    info("Academic Year", PROJECT_YEAR);
    info("Guide Teacher", PROJECT_GUIDE);
    info("Aim", PROJECT_AIM);
    info("Concepts Used", PROJECT_CONCEPTS);
    info("Features", PROJECT_FEATURES);
    info("Data File", DATA_FILE);
}


// ---------------------------------------------------------------
// Input Helpers
// ---------------------------------------------------------------

string readLine(const string &label)
{
    string s;

    while (true)
    {
        cout << UI::YELLOW
             << "  ➤ "
             << UI::RESET
             << label;

        getline(cin, s);

        // Prevent | from breaking file format
        replace(
            s.begin(),
            s.end(),
            '|',
            '-');

        if (!s.empty())
            return s;

        UI::error(
            "Input cannot be empty.");
    }
}


int readInt(const string &label)
{
    while (true)
    {
        cout << UI::YELLOW
             << "  ➤ "
             << UI::RESET
             << label;

        int x;

        if (cin >> x)
        {
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n');

            return x;
        }

        if (cin.eof())
            exit(0);

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n');

        UI::error(
            "Please enter a valid whole number.");
    }
}


double readSalary(const string &label)
{
    while (true)
    {
        cout << UI::YELLOW
             << "  ➤ "
             << UI::RESET
             << label;

        double x;

        if (cin >> x && x >= 0)
        {
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n');

            return x;
        }

        if (cin.eof())
            exit(0);

        cin.clear();

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n');

        UI::error(
            "Please enter a valid non-negative amount.");
    }
}


string toLower(string s)
{
    transform(
        s.begin(),
        s.end(),
        s.begin(),
        ::tolower);

    return s;
}


// ---------------------------------------------------------------
// Employee Manager
// ---------------------------------------------------------------
class EmployeeManager
{
private:

    vector<Employee> employees;

    static const int
        W_ID = 6,
        W_NAME = 20,
        W_DEPT = 16,
        W_DES = 18,
        W_SAL = 14;


    int findIndexById(int id) const
    {
        for (size_t i = 0;
             i < employees.size();
             i++)
        {
            if (employees[i].getId() == id)
                return (int)i;
        }

        return -1;
    }


    // -----------------------------------------------------------
    // Table Drawing
    // -----------------------------------------------------------

    static void tableLine(
        const string &l,
        const string &m,
        const string &r)
    {
        cout << UI::CYAN
             << "  "
             << l
             << UI::rep("─", W_ID + 2)
             << m
             << UI::rep("─", W_NAME + 2)
             << m
             << UI::rep("─", W_DEPT + 2)
             << m
             << UI::rep("─", W_DES + 2)
             << m
             << UI::rep("─", W_SAL + 2)
             << r
             << UI::RESET
             << "\n";
    }


    static void tableHeader()
    {
        const string bar =
            UI::CYAN + "│" + UI::RESET;

        tableLine(
            "┌",
            "┬",
            "┐");

        cout << "  "
             << bar
             << UI::BOLD
             << " "
             << UI::padR(
                    "ID",
                    W_ID)
             << " "
             << UI::RESET
             << bar

             << UI::BOLD
             << " "
             << UI::padR(
                    "👤 Name",
                    W_NAME - 1)
             << " "
             << UI::RESET
             << bar

             << UI::BOLD
             << " "
             << UI::padR(
                    "Department",
                    W_DEPT)
             << " "
             << UI::RESET
             << bar

             << UI::BOLD
             << " "
             << UI::padR(
                    "Designation",
                    W_DES)
             << " "
             << UI::RESET
             << bar

             << UI::BOLD
             << " "
             << UI::padL(
                    "Salary",
                    W_SAL)
             << " "
             << UI::RESET
             << bar
             << "\n";

        tableLine(
            "├",
            "┼",
            "┤");
    }


    static void tableRow(
        const Employee &e)
    {
        const string bar =
            UI::CYAN + "│" + UI::RESET;

        cout << "  "
             << bar
             << " "
             << UI::GREEN
             << UI::padR(
                    to_string(e.getId()),
                    W_ID)
             << UI::RESET
             << " "
             << bar

             << " "
             << UI::padR(
                    UI::cut(
                        e.getName(),
                        W_NAME),
                    W_NAME)
             << " "
             << bar

             << " "
             << UI::padR(
                    UI::cut(
                        e.getDepartment(),
                        W_DEPT),
                    W_DEPT)
             << " "
             << bar

             << " "
             << UI::padR(
                    UI::cut(
                        e.getDesignation(),
                        W_DES),
                    W_DES)
             << " "
             << bar

             << " "
             << UI::YELLOW
             << UI::padL(
                    UI::money(
                        e.getSalary()),
                    W_SAL)
             << UI::RESET
             << " "
             << bar
             << "\n";
    }


    static void tableFooter()
    {
        tableLine(
            "└",
            "┴",
            "┘");
    }


public:

    EmployeeManager()
    {
        load();
    }


    // -----------------------------------------------------------
    // Load Data
    // -----------------------------------------------------------
    void load()
    {
        ifstream in(DATA_FILE);

        if (!in)
            return;

        string line;

        while (getline(in, line))
        {
            Employee e;

            if (Employee::fromLine(line, e))
                employees.push_back(e);
        }
    }


    // -----------------------------------------------------------
    // Save Data
    // -----------------------------------------------------------
    void save() const
    {
        ofstream out(DATA_FILE);

        if (!out)
        {
            UI::error(
                "Could not save data to "
                + DATA_FILE);

            return;
        }

        for (const auto &e : employees)
        {
            out << e.toLine()
                << "\n";
        }
    }


    // -----------------------------------------------------------
    // Add Employee
    // -----------------------------------------------------------
    void addEmployee()
    {
        UI::section(
            "➕  Add New Employee");

        int id;

        while (true)
        {
            id = readInt(
                "Employee ID     : ");

            if (id <= 0)
            {
                UI::error(
                    "ID must be positive.");
            }
            else if (
                findIndexById(id) != -1)
            {
                UI::error(
                    "ID already exists.");
            }
            else
            {
                break;
            }
        }

        string name =
            readLine(
                "Name            : ");

        string dept =
            readLine(
                "Department      : ");

        string desig =
            readLine(
                "Designation     : ");

        double sal =
            readSalary(
                "Salary (₹)      : ");

        employees.push_back(
            Employee(
                id,
                name,
                dept,
                desig,
                sal));

        save();

        cout << "\n";

        UI::success(
            "Employee added successfully!");
    }


    // -----------------------------------------------------------
    // Display All Employees
    // -----------------------------------------------------------
    void displayAll() const
    {
        UI::section(
            "📋  All Employees");

        if (employees.empty())
        {
            UI::warning(
                "No employee records found.");

            return;
        }

        tableHeader();

        for (const auto &e : employees)
            tableRow(e);

        tableFooter();

        UI::info(
            "Total employees: "
            + to_string(
                employees.size()));
    }


    // -----------------------------------------------------------
    // Search By ID
    // -----------------------------------------------------------
    void searchById() const
    {
        UI::section(
            "🔍  Search by ID");

        int id =
            readInt(
                "Enter Employee ID : ");

        int idx =
            findIndexById(id);

        cout << "\n";

        if (idx == -1)
        {
            UI::error(
                "Employee not found.");

            return;
        }

        tableHeader();

        tableRow(
            employees[idx]);

        tableFooter();
    }


    // -----------------------------------------------------------
    // Search By Name
    // -----------------------------------------------------------
    void searchByName() const
    {
        UI::section(
            "🔎  Search by Name");

        string key =
            toLower(
                readLine(
                    "Enter name (or part) : "));

        cout << "\n";

        vector<const Employee *> hits;

        for (const auto &e : employees)
        {
            if (
                toLower(
                    e.getName())
                .find(key)
                != string::npos)
            {
                hits.push_back(&e);
            }
        }

        if (hits.empty())
        {
            UI::error(
                "No matching employee found.");

            return;
        }

        tableHeader();

        for (auto p : hits)
            tableRow(*p);

        tableFooter();

        UI::info(
            to_string(hits.size())
            + " match(es) found.");
    }


    // -----------------------------------------------------------
    // Update Employee
    // -----------------------------------------------------------
    void updateEmployee()
    {
        UI::section(
            "✏️   Update Employee");

        int id =
            readInt(
                "Enter Employee ID to update : ");

        int idx =
            findIndexById(id);

        if (idx == -1)
        {
            cout << "\n";

            UI::error(
                "Employee not found.");

            return;
        }

        Employee &e =
            employees[idx];

        cout << "\n"
             << "  "
             << UI::BOLD
             << "Current record:"
             << UI::RESET
             << "\n";

        tableHeader();

        tableRow(e);

        tableFooter();

        cout << "\n"
             << "  What do you want to update?\n"

             << "   "
             << UI::CYAN
             << "1"
             << UI::RESET
             << " ▸ Name\n"

             << "   "
             << UI::CYAN
             << "2"
             << UI::RESET
             << " ▸ Department\n"

             << "   "
             << UI::CYAN
             << "3"
             << UI::RESET
             << " ▸ Designation\n"

             << "   "
             << UI::CYAN
             << "4"
             << UI::RESET
             << " ▸ Salary\n";

        int ch =
            readInt(
                "Choice : ");

        switch (ch)
        {
            case 1:

                e.setName(
                    readLine(
                        "New name        : "));

                break;

            case 2:

                e.setDepartment(
                    readLine(
                        "New department  : "));

                break;

            case 3:

                e.setDesignation(
                    readLine(
                        "New designation : "));

                break;

            case 4:

                e.setSalary(
                    readSalary(
                        "New salary (₹)  : "));

                break;

            default:

                UI::error(
                    "Invalid choice.");

                return;
        }

        save();

        cout << "\n";

        UI::success(
            "Record updated successfully!");
    }


    // -----------------------------------------------------------
    // Delete Single Employee
    // -----------------------------------------------------------
    void deleteEmployee()
    {
        UI::section(
            "🗑️   Delete Employee");

        int id =
            readInt(
                "Enter Employee ID to delete : ");

        int idx =
            findIndexById(id);

        if (idx == -1)
        {
            cout << "\n";

            UI::error(
                "Employee not found.");

            return;
        }

        cout << "\n";

        UI::warning(
            "Delete "
            + employees[idx].getName()
            + "? This cannot be undone.");

        cout << UI::YELLOW
             << "  ➤ "
             << UI::RESET
             << "Confirm (YES/NO) : ";

        string confirm;

        getline(
            cin,
            confirm);

        confirm =
            toLower(confirm);

        cout << "\n";

        if (confirm == "yes")
        {
            employees.erase(
                employees.begin()
                + idx);

            save();

            UI::success(
                "Employee deleted.");
        }
        else if (confirm == "no")
        {
            UI::info(
                "Deletion cancelled.");
        }
        else
        {
            UI::error(
                "Please enter YES or NO.");
        }
    }


    // -----------------------------------------------------------
    // Sort Employees
    // -----------------------------------------------------------
    void sortEmployees()
    {
        UI::section(
            "🔃  Sort Employees");

        if (employees.empty())
        {
            UI::warning(
                "No employee records to sort.");

            return;
        }

        cout << "  Sort by:\n"

             << "   "
             << UI::CYAN
             << "1"
             << UI::RESET
             << " ▸ ID\n"

             << "   "
             << UI::CYAN
             << "2"
             << UI::RESET
             << " ▸ Name (A-Z)\n"

             << "   "
             << UI::CYAN
             << "3"
             << UI::RESET
             << " ▸ Salary (high → low)\n";

        int ch =
            readInt(
                "Choice : ");

        switch (ch)
        {
            case 1:

                sort(
                    employees.begin(),
                    employees.end(),
                    [](const Employee &a,
                       const Employee &b)
                    {
                        return a.getId()
                             < b.getId();
                    });

                break;

            case 2:

                sort(
                    employees.begin(),
                    employees.end(),
                    [](const Employee &a,
                       const Employee &b)
                    {
                        return toLower(
                                   a.getName())
                               <
                               toLower(
                                   b.getName());
                    });

                break;

            case 3:

                sort(
                    employees.begin(),
                    employees.end(),
                    [](const Employee &a,
                       const Employee &b)
                    {
                        return a.getSalary()
                             >
                             b.getSalary();
                    });

                break;

            default:

                UI::error(
                    "Invalid choice.");

                return;
        }

        save();

        UI::success(
            "Sorted successfully!");

        displayAll();
    }


    // -----------------------------------------------------------
    // Salary Report
    // -----------------------------------------------------------
    void salaryReport() const
    {
        UI::section(
            "📊  Salary Report");

        if (employees.empty())
        {
            UI::warning(
                "No employee records found.");

            return;
        }

        double total = 0;
        double maxSal = 0;

        const Employee *hi =
            &employees[0];

        const Employee *lo =
            &employees[0];

        for (const auto &e : employees)
        {
            total +=
                e.getSalary();

            maxSal =
                max(
                    maxSal,
                    e.getSalary());

            if (
                e.getSalary()
                >
                hi->getSalary())
            {
                hi = &e;
            }

            if (
                e.getSalary()
                <
                lo->getSalary())
            {
                lo = &e;
            }
        }

        cout << "  "
             << UI::BOLD
             << "Salary comparison"
             << UI::RESET
             << "\n\n";

        const int BAR_MAX = 28;

        for (const auto &e : employees)
        {
            int len =
                maxSal > 0
                ?
                (int)round(
                    e.getSalary()
                    /
                    maxSal
                    *
                    BAR_MAX)
                :
                0;

            if (
                len < 1 &&
                e.getSalary() > 0)
            {
                len = 1;
            }

            cout << "  "
                 << UI::padR(
                        UI::cut(
                            e.getName(),
                            16),
                        16)
                 << " "

                 << UI::GREEN
                 << UI::rep(
                        "█",
                        len)
                 << UI::RESET

                 << UI::DIM
                 << UI::rep(
                        "░",
                        BAR_MAX - len)
                 << UI::RESET

                 << " "

                 << UI::YELLOW
                 << UI::money(
                        e.getSalary())
                 << UI::RESET
                 << "\n";
        }

        cout << "\n"
             << UI::CYAN
             << "  ┌"
             << UI::rep("─", 44)
             << "┐"
             << UI::RESET
             << "\n";

        auto row =
            [](const string &label,
               const string &value)
        {
            cout << UI::CYAN
                 << "  │"
                 << UI::RESET
                 << " "
                 << UI::padR(
                        label,
                        15)
                 << UI::padL(
                        value,
                        26)
                 << " "
                 << UI::CYAN
                 << "│"
                 << UI::RESET
                 << "\n";
        };

        row(
            "👥 Employees",
            to_string(
                employees.size()));

        row(
            "💰 Total payroll",
            UI::money(total));

        row(
            "📈 Average salary",
            UI::money(
                total /
                employees.size()));

        row(
            "🏆 Highest paid",
            UI::cut(
                hi->getName(),
                12)
            + " "
            + UI::money(
                hi->getSalary()));

        row(
            "📉 Lowest paid",
            UI::cut(
                lo->getName(),
                12)
            + " "
            + UI::money(
                lo->getSalary()));

        cout << UI::CYAN
             << "  └"
             << UI::rep("─", 44)
             << "┘"
             << UI::RESET
             << "\n";
    }


    // -----------------------------------------------------------
    // DELETE ALL EMPLOYEES
    // -----------------------------------------------------------
    void deleteAllEmployees()
    {
        UI::section(
            "🗑️   Delete All Employees");

        if (employees.empty())
        {
            UI::warning(
                "No employees to delete.");

            return;
        }

        UI::warning(
            "This will delete ALL employees. "
            "This cannot be undone.");

        cout << UI::YELLOW
             << "  ➤ "
             << UI::RESET
             << "Confirm (YES/NO) : ";

        string confirm;

        getline(
            cin,
            confirm);

        confirm =
            toLower(confirm);

        if (confirm == "yes")
        {
            employees.clear();

            // Save empty vector
            // This also clears employees.txt
            save();

            UI::success(
                "All employee records deleted.");
        }
        else if (confirm == "no")
        {
            UI::info(
                "Deletion cancelled.");
        }
        else
        {
            UI::error(
                "Please enter YES or NO.");
        }
    }
};


// ---------------------------------------------------------------
// Main Menu
// ---------------------------------------------------------------
void showMenu()
{
    using namespace UI;

    banner();

    cout << "\n";

    cout << "   " << GREEN << "1"  << RESET << "  ➕  Add Employee\n";
    cout << "   " << GREEN << "2"  << RESET << "  📋  Display All Employees\n";
    cout << "   " << GREEN << "3"  << RESET << "  🔍  Search by ID\n";
    cout << "   " << GREEN << "4"  << RESET << "  🔎  Search by Name\n";
    cout << "   " << GREEN << "5"  << RESET << "  ✏️   Update Employee\n";
    cout << "   " << GREEN << "6"  << RESET << "  🗑️   Delete Employee\n";
    cout << "   " << GREEN << "7"  << RESET << "  🔃  Sort Employees\n";
    cout << "   " << GREEN << "8"  << RESET << "  📊  Salary Report\n";
    cout << "   " << GREEN << "9"  << RESET << "  🗑️  Delete ALL Employees\n";
    cout << "   " << GREEN << "10" << RESET << " 👥  Project Group Info\n";
    cout << "   " << RED   << "0"  << RESET << "  🚪  Exit\n";

    cout << CYAN
         << "  "
         << rep("─", 48)
         << RESET
         << "\n";
}


// ---------------------------------------------------------------
// Main Function
// ---------------------------------------------------------------
int main()
{
    UI::setup();

    EmployeeManager manager;

    int choice;

    do
    {
        UI::clearScreen();

        showMenu();

        choice =
            readInt(
                "Enter your choice : ");

        switch (choice)
        {
            case 1:
                manager.addEmployee();
                break;

            case 2:
                manager.displayAll();
                break;

            case 3:
                manager.searchById();
                break;

            case 4:
                manager.searchByName();
                break;

            case 5:
                manager.updateEmployee();
                break;

            case 6:
                manager.deleteEmployee();
                break;

            case 7:
                manager.sortEmployees();
                break;

            case 8:
                manager.salaryReport();
                break;

            case 9:
                manager.deleteAllEmployees();
                break;

            case 10:
                showGroupInfo();
                break;

            case 0:

                cout << "\n";

                UI::success(
                    "Thank you for using the system. Goodbye! 👋");

                cout << "\n";

                break;

            default:

                cout << "\n";

                UI::error(
                    "Invalid choice. Try again.");
        }

        if (choice != 0)
            UI::pause();

    }
    while (choice != 0);

    return 0;
}
