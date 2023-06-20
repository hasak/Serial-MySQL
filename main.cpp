#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

using namespace std;


void action(string json){
    if(json=="") return;
    cout<<json<<endl;
}

int main() {
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
