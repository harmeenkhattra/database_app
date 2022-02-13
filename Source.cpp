
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstring>
#include <occi.h>
#include <iomanip>
using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;
struct Employee {
    int  employeeNumber;
    char lastName[50];
    char firstName[50];
    char extension[10];
    char email[100];
    char phone[50];
    char officecode[10];
    char reportsTo[100];
    char jobTitle[50];
    char city[50];
};
Employee empl = { 0 , '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0','\0' };

/* OCCI Variables */
Environment* env = nullptr;
Connection* conn = nullptr;
Statement* stmt = nullptr;
ResultSet* rs = nullptr;

/* Used Variables */
string user = "********";
string pass = "********";
string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";
int findEmployee(Connection* conn, int employeeNumber, struct Employee* emp)
{
    int r = 0;

    stmt = conn->createStatement("SELECT  E.employeeNumber, E.lastName, E.firstName, E.email, O.phone, E.extension,  M.firstname || ' ' ||  M.lastname AS reportsTo, E.jobTitle, O.city FROM employees E LEFT OUTER JOIN employees M on E.reportsTo = M.employeenumber INNER JOIN offices O on E.officeCode = O.officeCode WHERE E.employeeNumber=:1");
    // sql query for selecting the required columns using inner and outer join and comparring the value of int read with first column
    stmt->setInt(1, employeeNumber);
    rs = stmt->executeQuery();  //execute the query
    if (!rs->next())    //if the result of query is empty
    {
       
        cout << "Employee with this number does not exist. However, you can add one." << endl;
    }

   else
   {
        if (!rs->isNull(1))    //if the first column is not null
        {


            {
                //storing the values read in local variables
                int emp_no = rs->getInt(1);
                string lname = rs->getString(2);
                string fname = rs->getString(3);
                string mail = rs->getString(4);
                string phone = rs->getString(5);
                string ex = rs->getString(6);
                string reports = rs->getString(7);
                string job = rs->getString(8);
                string city = rs->getString(9);

                //storing values read in employee struct
                emp->employeeNumber = emp_no;
                strcpy(emp->lastName, lname.c_str());
                strcpy(emp->firstName, fname.c_str());
                strcpy(emp->extension, ex.c_str());
                strcpy(emp->email, mail.c_str());

                strcpy(emp->phone, phone.c_str());
                strcpy(emp->city, city.c_str());
                if (reports.c_str() == nullptr)
                {
                    emp->reportsTo[0] = '\0';
                }
                else
                {
                    strcpy(emp->reportsTo, reports.c_str());
                }
                strcpy(emp->jobTitle, job.c_str());
            }
            r = 1;

        }
    }
    return r;
}
void displayEmployee(Connection* conn, struct Employee empl)
{
    int employeeNumber;
    int r;
    bool valid = false;
    do {
        cout << "Enter Employee Number:";
        cin >> employeeNumber;
        if (cin.fail())
        {
            cout << "Invalid entry(enter an integer)" << endl;    //validating only int is entered for employee number
            cin.clear();
            cin.ignore(1000, '\n');
        }
        else
        {
            valid = true;
        }
    } while (!valid);
    r = findEmployee(conn, employeeNumber, &empl);
    if (r == 1)
    {
        //displaying the data read with help of employee struct
        cout << "Employee number = " << empl.employeeNumber << endl;
        cout << "lastname = " << empl.lastName << endl;
        cout << "firstName = " << empl.firstName << endl;
        cout << "email = " << empl.email << endl;
        cout << "phone = " << empl.phone << endl;
        cout << "extension = " << empl.extension << endl;
        cout << "reportsTo = " << empl.reportsTo << endl;
        cout << "jobTitle = " << empl.jobTitle << endl;
        cout << "city = " << empl.city << endl << endl;
    }
}
void displayAllEmployees(Connection* conn)
{

    //sql query for selecting the required columns using inner and outer join
    stmt = conn->createStatement("SELECT E.employeeNumber, E.firstName || ' ' || E.lastName  AS name, E.email, O.phone, E.extension,  M.firstname || ' ' ||  M.lastname AS reportsTo FROM employees E LEFT OUTER JOIN employees M on E.reportsTo = M.employeenumber INNER JOIN offices O on E.officeCode = O.officeCode ORDER BY E.employeeNumber");
    rs = stmt->executeQuery();

    cout << "The company employees are:" << endl;
    cout << setw(7) << left << "E" << setw(25) << "Employee Name" << setw(35) << "Email" << setw(20) << "Phone" << setw(10) << "Ext" << setw(7) << "Manager" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
    if (rs->next())
    {
        do
        {
            //storing the values read in local variables
            int emp_no = rs->getInt(1);
            string name = rs->getString(2);
            string mail = rs->getString(3);
            string phone = rs->getString(4);
            string ex = rs->getString(5);
            

            string reports = rs->getString(6);

          

             //displaying the values the read
            cout << setw(7) << left << emp_no << setw(25) << name << setw(35) << mail << setw(20) << phone << setw(10) << ex;
            
            if (reports.c_str() == nullptr)
            {
                cout << endl;
            }
            else
            {
                cout << setw(7) << reports << endl;
            }

        } while (rs->next());
    }
    else
    {
        cout << "There is no employees’ information to be displayed." << endl;
    }
}
void insertEmployee(Connection* conn, struct Employee* empl)
{
 //inserting the new employee if the employee with same number does not exist
    stmt = conn->createStatement();
    stmt->setSQL("INSERT INTO employees VALUES(:1, :2, :3, :4, :5, :6, :7, :8) ");
    stmt->setInt(1, empl->employeeNumber);
    stmt->setString(2, empl->lastName);
    stmt->setString(3, empl->firstName);
    stmt->setString(4, empl->extension);
    stmt->setString(5, empl->email);
    stmt->setString(6, "1");
    stmt->setInt(7, 1002);
    stmt->setString(8, empl->jobTitle);


    stmt->executeUpdate();
    cout << "The new employee is added successfully." << endl;
}
void updateEmployee(Connection* conn, int employeeNumber)
{
    string ext = "";
    //prompting for new extension
    cout << "New Extension: ";
    cin >> ext;
   //updating the employee using update sql query
    stmt = conn->createStatement();
    stmt->setSQL("UPDATE employees SET extension = :1 WHERE employeenumber = :2");
    stmt->setString(1, ext);
    stmt->setInt(2, employeeNumber);
   
    stmt->executeUpdate();
    cout << "The employee is updated successfully." << endl;
}
void deleteEmployee(Connection* conn, int employeeNumber)
{
   // deleting the employee usig delete sql query
    stmt = conn->createStatement("DELETE FROM employees WHERE employeenumber = :1");
    stmt->setInt(1, employeeNumber);
   
    stmt->executeUpdate();
    cout << "The  employee is deleted successfully." << endl;
}
int menu(void)
{
    int selection;
    do {
        cout << " *********************HR Menu *********************" << endl;
        cout << "1) Find Employee" << endl;
        cout << "2) Employee Report" << endl;
        cout << "3) Add Employee" << endl;
        cout << "4) Update Employee" << endl;
        cout << "5) Remove Employee" << endl;
        cout << "0) Exit" << endl;
        cout << "Enter an option (0-5): ";
        cin >> selection;
        switch (selection)
        {
        case 1:
        {
            displayEmployee(conn, empl);
            break;
        }
        case 2:
        {
            displayAllEmployees(conn);
            break;
        }
        case 3:
        {
            int valid ,employeeNumber;;
            cout << "******* ADDING NEW EMPLOYEE *******" << endl;
            // Prompting the user to enter new employee number and other details
            cout << "Employee Number: ";
            cin >> empl.employeeNumber;
            employeeNumber = empl.employeeNumber;
            cout << "Last Name: ";
            cin >> empl.lastName;
            cout << "Fist Name: ";
            cin >> empl.firstName;
            cout << "Email: ";
            cin >> empl.email;
            cout << "extension: ";
            cin >> empl.extension;
            cout << "Job Title: ";
            cin >> empl.jobTitle;
            cout << "City: ";
            cin >> empl.city;
          
            // Checking if the employee with same employee number already exist in table
            valid = findEmployee(conn, employeeNumber, &empl);

            if (valid == 1) 
            {
                // If findEmployee returns 1, the employee already exist. So, display error message.
                cout << "An employee with the same employee number exists." << endl << endl;
            }
            else 
            {
                //Calling insert function to add the new employee.
                insertEmployee(conn, &empl);
            }

           
            break;
        }
        case 4:
        {
            int employeeNumber, valid;
            cout << "******* UPDATING EXISTING EMPLOYEE *******" << endl;
            
        // Prompting the user to enter employee number for the employee you want to update
        cout << "Employee Number: ";
        cin >> employeeNumber;

        // Checking if the employee with same employee number exist
        valid = findEmployee(conn, employeeNumber, &empl);

        if (valid == 0) 
        {
            // If the findEmployee function returns 0, it means employee does not exist and we cannot update an employee which does not exist. So, display error message.
            cout << "Employee " << employeeNumber << " does not exist." << endl << endl;
        }
        else 
        {
            // If employee exist, call update function for updating extension
            updateEmployee(conn, employeeNumber);
        }
            
            break;
        }
        case 5:
        {
            int employeeNumber, valid;
            cout << "******* DELETING EXISTING EMPLOYEE *******" << endl;
           
            // Prompting the user to enter employee number for the employee you want to delete
            cout << "Employee Number: ";
            cin >> employeeNumber;

            // Checking if the employee with same employee number exists
            valid = findEmployee(conn, employeeNumber, &empl);

            if (valid == 0) {
                // If the findEmployee function returns 0, it means employee does not exist and we cannot delete an employee which does not exist. So, display error message.
                cout << "Employee " << employeeNumber << " does not exist." << endl << endl;
            }
            else {
                // If employee exist, call delete function for deleting employee
                deleteEmployee(conn, employeeNumber);
            }

            break;
        }
        case 0:
        {
            break;
        }
        }
    } while (selection != 0 || selection >= 6 || selection <= -1);
    return selection;
}

int main(void)
{

    try {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(user, pass, constr);
        cout << "Connection is Successful!" << endl;
        menu();   //calling menu function

        //terminating the connection
        env->terminateConnection(conn);
        Environment::terminateEnvironment(env);
    }
    catch (SQLException& sqlExcp) {
        cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
    }
    return 0;
}
