#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/statement.h"

int main() {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;

    try {
        // Create a MySQL driver instance
        driver = sql::mysql::get_mysql_driver_instance();

        // Establish a connection to the MySQL server
        con = driver->connect("tcp://127.0.0.1:3306", "root", "gnomegnome");

        // Select the database to use
        con->setSchema("calliope");

        // Create a statement object
        stmt = con->createStatement();

        // Perform an INSERT statement
        std::string insertQuery = "INSERT INTO data (device_name, quantity, value) VALUES ('pappapa', 'temp', '32')";
        stmt->execute(insertQuery);

        // Clean up resources
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        std::cout << "MySQL Exception: " << e.what() << std::endl;
    }

    return 0;
}5