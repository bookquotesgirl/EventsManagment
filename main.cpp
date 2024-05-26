#include <iostream>
#include <mysql.h>
#include <sstream>
#include <string>

using namespace std;

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

// Creating connections and database function
void connect_db();
void close_db();
void create_db();

// Autuntication functions
void register_user();
bool authenticate_user(string username, string password);

// Users Privilage
void view_events(string username);
void search_event(string &username);
void book_event(string username, int event_id);

// Dashbords
void admin_dashboard();
void user_dashboard(string username);

// Admin Privilages
void post_event();
void view_events_with_registrations();
void view_event_registrants();

void create_admin_user();

// Some helper functions
string string_to_int(int x);
int string_to_int(const string &str);

// Main function
int main()
{
    connect_db();
    create_db();
    create_admin_user();

    int choice;
    do
    {
        cout << "1. Register" << endl;
        cout << "2. Login" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            register_user();
            break;
        case 2:
        {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            if (username == "admin" && password == "admin")
            {
                admin_dashboard();
            }
            else if (authenticate_user(username, password))
            {
                user_dashboard(username);
            }
            else
            {
                cout << "No such user!" << endl;
            }
            break;
        }
        case 3:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 3);

    close_db();
    return 0;
}

int string_to_int(const string &str)
{
    stringstream ss(str);
    int num;
    ss >> num;
    return num;
}

string string_to_int(int x)
{
    stringstream ss;
    ss << x;
    return ss.str();
}

// Function to connect to the database
void connect_db()
{
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        cout << "mysql_init() failed" << endl;
        exit(1);
    }

    // Connect to the MySQL database
    if (mysql_real_connect(conn, "localhost", "username", "password", "event_organizer_final", 0, NULL, 0) == NULL) // Replace with ur own username and password 
    {
        cout << "mysql_real_connect() failed\n"
             << mysql_error(conn) << endl;
        mysql_close(conn);
        exit(1);
    }
}

// Function to close the database connection
void close_db()
{
    if (res != NULL)
    {
        mysql_free_result(res); // Free the result set
    }
    mysql_close(conn); // Close the MySQL connection
}

// Function to register a new user
void register_user()
{
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    string query = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "')"; // Inserting to database
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Registration failed: " << mysql_error(conn) << endl;
    }
    // Automatically Logs in after the user registed
    else
    {
        cout << "Registration successful! Logged in successfully" << endl;
        user_dashboard(username);
    }
}

// Function to authenticate a user
bool authenticate_user(string username, string password)
{
    string query = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "'"; // Finding the user with the same name and password
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Authentication failed: " << mysql_error(conn) << endl;
        return false;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "Error check the server: " << mysql_error(conn) << endl;
        return false;
    }

    row = mysql_fetch_row(res); // Fetch the row
    if (row == NULL)
    {
        cout << "Invalid username or password" << endl;
        return false;
    }
    else
    {
        return true;
    }
}

// Function to post a new event (admin only)
void post_event()
{
    string name, date;
    double price;
    int max_capacity;

    cout << "Enter event name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter event date (YYYY-MM-DD): ";
    getline(cin, date);

    cout << "Enter event price: ";
    cin >> price;

    cout << "Enter event max capacity: ";
    cin >> max_capacity;

    string query = "INSERT INTO events (name, date, price, max_capacity) VALUES ('" + name + "', '" + date + "', " + string_to_int(price) + ", " + string_to_int(max_capacity) + ")"; // INserting the Event to events dataabase
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Failed to post event: " << mysql_error(conn) << endl;
    }
    else
    {
        cout << "Event posted successfully!" << endl;
    }
}

// Function to view all events
void view_events(string username)
{
    string query = "SELECT * FROM events"; // Printing all the rows in events table
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Failed to retrieve events: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "Failed to retrieve events: " << mysql_error(conn) << endl;
        return;
    }

    cout << "Id" << "\t" << "Name" << "\t" << "Date" << "\t\t" << "Price" << "\t" << "Max" << "\t" << "current_capacity" << endl;
    int num_fields = mysql_num_fields(res);
    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            cout << row[i] << "\t";
        }
        cout << endl;
    }

    int choice;
    cout << "Enter the event ID to book or 0 to go back: ";
    cin >> choice;

    if (choice != 0)
    {
        book_event(username, choice);
    }
}

// Function to search for an event by name
void search_event(string &username)
{
    string name;
    cout << "Enter event name to search: ";
    cin.ignore();
    getline(cin, name);

    string query = "SELECT * FROM events WHERE name LIKE '%" + name + "%'"; // Filtering out the name the user inserted
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Failed to search events: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "Failed Because: " << mysql_error(conn) << endl;
        return;
    }

    cout << "Id" << "\t" << "Name" << "\t" << "Date" << "\t\t" << "Price" << "\t" << "Max" << "\t" << "current_capacity" << endl;
    int num_fields = mysql_num_fields(res);
    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            cout << row[i] << "\t";
        }
        cout << endl;
    }

    int choice;
    cout << "1. Book Event"<< endl;
    cout << "2. Go Back" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1)
    {
        int event_id;
        cout << "Enter event ID to book: ";
        cin >> event_id;
        book_event(username, event_id);
    }
    else
    {
        user_dashboard(username);
    }
}

// Function to book an event by its ID
void book_event(string username, int event_id)
{
    string query = "SELECT * FROM events WHERE id = " + string_to_int(event_id); // selecting the specifc event to check its max and curr capacity
    if (mysql_query(conn, query.c_str()))
    { 
        cout << "Failed to retrieve event beacause: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "Failed to the the data beacause: " << mysql_error(conn) << endl;
        return;
    }

    row = mysql_fetch_row(res);
    int max_capacity = string_to_int(row[4]);
    int current_capacity = string_to_int(row[5]);

    if (current_capacity >= max_capacity)
    {
        cout << "Event is full!" << endl;
        return;
    }

    query = "INSERT INTO registrations (user_id, event_id) VALUES ((SELECT id FROM users WHERE username = '" + username + "'), " + string_to_int(event_id) + ")"; // Inserting to registration table
    if (mysql_query(conn, query.c_str()))
    { 
        cout << "Cannot Book twice with the same name becuase: " << mysql_error(conn) << endl;
    }
    else
    {
        query = "UPDATE events SET current_capacity = current_capacity + 1 WHERE id = " + string_to_int(event_id); // Updates the number of people when ever a new user books the event
        if (mysql_query(conn, query.c_str()))
        { 
            cout << "Failed to update event capacity beacause: " << mysql_error(conn) << endl;
        }
        else
        {
            cout << "Event booked successfully!" << endl;
        }
    }
}

// Function to display the admin dashboard
void admin_dashboard()
{
    int choice;
    do
    {
        cout << "1. Post Event" << endl;
        cout << "2. View Events with Registrations" << endl;
        cout << "3. View Event Registrants" << endl;
        cout << "4. Logout" << endl;

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            post_event();
            break;
        case 2:
            view_events_with_registrations();
            break;
        case 3:
            view_event_registrants();
            break;
        case 4:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 4);
}

// Function to display the user dashboard
void user_dashboard(string username)
{
    int choice;
    do
    {
        cout << "1. View Available Events" << endl;
        cout << "2. Search Event by Name" << endl;
        cout << "3. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            view_events(username);
            break;
        case 2:
            search_event(username);
            break;
        case 3:
            break;
        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 3);
}

// Function to view events with the number of registrations (admin only)
void view_events_with_registrations()
{
    string query = "SELECT e.id, e.name, e.date, e.price, e.max_capacity, e.current_capacity FROM events e"; // Selecting from the events table
    if (mysql_query(conn, query.c_str()))
    { // Execute the query
        cout << "Failed to retrieve events with registrations: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "Failed beacause: " << mysql_error(conn) << endl;
        return;
    }

    int num_fields = mysql_num_fields(res);
    cout << "Id" << "\t" << "Name" << "\t" << "Date" << "\t\t" << "Price" << "\t" << "Max" << "\t" << "current_capacity" << endl;
    while ((row = mysql_fetch_row(res)))
    {
        for (int i = 0; i < num_fields; i++)
        {
            cout << row[i] << "\t";
        }
        cout << endl;
    }
}

// Function to view the registrants of a specific event (admin only)
void view_event_registrants()
{
    int event_id;
    cout << "Enter event ID to view registrants: ";
    cin >> event_id;

    string query = "SELECT u.username FROM registrations r INNER JOIN users u ON r.user_id = u.id WHERE r.event_id = " + string_to_int(event_id); // Show the users who have booked and event with that given id
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Failed to retrieve event registrants: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn); // Store the result set
    if (res == NULL)
    {
        cout << "mysql_store_result() failed: " << mysql_error(conn) << endl;
        return;
    }

    while ((row = mysql_fetch_row(res)))
    {
        cout << row[0] << endl;
    }
}

// Function to ensure the admin user is created
void create_admin_user()
{
    string query = "INSERT IGNORE INTO users (username, password, is_admin) VALUES ('admin', 'admin', 1)"; // It doesnt matter if it exists or not it creates the admin every time
    if (mysql_query(conn, query.c_str()))
    {
        cout << "Failed to create admin user: " << mysql_error(conn) << endl;
    }
}

// Creating the database
/* 

SQL CODE:

CREATE DATABASE event_organizer_final;

USE event_organizer_final;

CREATE TABLE IF NOT EXISTS users (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(50) NOT NULL,
    password VARCHAR(50) NOT NULL,
    is_admin BOOLEAN DEFAULT 0
);

CREATE TABLE IF NOT EXISTS events (
    id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    date DATE NOT NULL,
    price FLOAT NOT NULL,
    max_capacity INT NOT NULL,
    current_capacity INT DEFAULT 0
);

CREATE TABLE IF NOT EXISTS registrations (
    user_id INT,
    event_id INT,
    PRIMARY KEY (user_id, event_id),
    FOREIGN KEY (user_id) REFERENCES users(id),
    FOREIGN KEY (event_id) REFERENCES events(id)
);
*/

// This fuction excutes the above SQL code
void create_db()
{
    string query3 = "CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, username VARCHAR(50) NOT NULL, password VARCHAR(50) NOT NULL, is_admin BOOLEAN DEFAULT 0 )";
    
    if (mysql_query(conn, query3.c_str()))
    { 
        cout << "Failed to retrieve events with registrations: " << mysql_error(conn) << endl;
        return;
    }

    string query4 = "CREATE TABLE IF NOT EXISTS events (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(100) NOT NULL, date DATE NOT NULL, price FLOAT NOT NULL, max_capacity INT NOT NULL, current_capacity INT DEFAULT 0 )";

    if (mysql_query(conn, query4.c_str()))
    { 
        cout << "Failed to retrieve events with registrations: " << mysql_error(conn) << endl;
        return;
    }

    string query5 = "CREATE TABLE IF NOT EXISTS registrations (user_id INT, event_id INT, PRIMARY KEY (user_id, event_id), FOREIGN KEY (user_id) REFERENCES users(id), FOREIGN KEY (event_id) REFERENCES events(id) )"; 
    
    if (mysql_query(conn, query5.c_str()))
    { 
        cout << "Failed to retrieve events with registrations: " << mysql_error(conn) << endl;
        return;
    }
}