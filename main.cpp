#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "mysql_driver.h"
#include "mysql_connection.h"
#include "cppconn/statement.h"

using namespace std;

sql::mysql::MySQL_Driver *driver;
sql::Connection *con;
sql::Statement *stmt;


void action(string json){
    if(json=="") return;

    int comma,secondSemiColon;
    string withoutBraces,first,last,id,quantity,value;

    withoutBraces=json.substr(1,json.length()-2);
    comma=withoutBraces.find_first_of(',');
    first=withoutBraces.substr(0,comma);
    last=withoutBraces.substr(comma+1);
    id=first.substr(first.find_first_of(':')+1);
    secondSemiColon=last.find_first_of(':');
    quantity=last.substr(0,secondSemiColon);
    value=last.substr(secondSemiColon+1);


    // {id:papap,temper:31}

    try{
        stmt = con->createStatement();

        // Perform an INSERT statement
        std::string insertQuery = "INSERT INTO data (device_name, quantity, value) VALUES ('"+id+"', '"+quantity+"', '"+value+"')";
        stmt->execute(insertQuery);
    }
    catch(sql::SQLException &e) {
        cout << "MySQL Exception: " << e.what() << endl;
    }
    cout<<json<<endl;
    //cout<<withoutBraces<<endl;
    //cout<<first<<endl;
    //cout<<last<<endl;
}

int main() {
    // Create a MySQL driver instance
    driver = sql::mysql::get_mysql_driver_instance();

    // Establish a connection to the MySQL server
    con = driver->connect("tcp://127.0.0.1:3306", "root", "gnomegnome");

    // Select the database to use
    con->setSchema("calliope");


    // Open the serial port
    const char* port = "/dev/ttyACM0";  // Replace with your Arduino's port
    int serialPort = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serialPort == -1) {
        cerr << "Failed to open serial port." << endl;
        return 1;
    }

    // Configure the serial port
    struct termios tty;
    if (tcgetattr(serialPort, &tty) != 0) {
        cerr << "Error in tcgetattr." << endl;
        close(serialPort);
        return 1;
    }
    cfsetispeed(&tty, B9600);  // Replace with your Arduino's baud rate
    cfsetospeed(&tty, B9600);  // Replace with your Arduino's baud rate
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | IGNBRK);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;  // Read timeout in deciseconds

    if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
        cerr << "Error in tcsetattr." << endl;
        close(serialPort);
        return 1;
    }

    // Read from the serial port
    char buffer[256];
    ssize_t bytesRead;
    string json="";
    while (true) {
        bytesRead = read(serialPort, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            string data(buffer, bytesRead);
            for(int i=0;i<data.size();i++){
                if(data[i]==10){ // enter
                    action(json);
                    json="";
                }
                else if(data[i]!=32){
                    json+=data[i];
                }
            }
        }
    }

    // Close the serial port
    close(serialPort);

    return 0;
}
