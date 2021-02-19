#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <string>
#include <algorithm>
#include <unistd.h>

using namespace std;

#define DATABASE_SIZE 128
#define WRITE_OPERATIONS 10
#define READ_OPERATIONS 10

class Database {
    private:
        string data[DATABASE_SIZE];  
        int dataRead = 0;
        int dataWritten = 0;      
    public:
        void WriteAt(int pos, string toWrite) {
            data[pos] = toWrite;
            dataWritten += toWrite.length();
        }
        string ReadAt(int pos) {
            string read_data = data[pos];
            dataRead += read_data.length();
            return read_data;
        }
        int GetDataRead() {
            return dataRead;
        }
        int GetDataWritten() {
            return dataWritten;
        }
};

Database db;

int random_in_range(int begin, int end) {
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> uni(begin, end);

    return uni(generator);
}

//Создает рандомную строку
string prepare_data() {
    string newData("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    random_device rd;
    mt19937 generator(rd());

    shuffle(newData.begin(), newData.end(), generator);

    int begin_pos = random_in_range(0, newData.length() - 1);
    return newData.substr(begin_pos, newData.length());
}

void *Writer() {
    for (int i = 0; i < WRITE_OPERATIONS; i++) {
        string prepared_data = prepare_data();
        int pos = random_in_range(0, prepared_data.length() - 1);
        db.WriteAt(pos, prepared_data);
        cout << 
            to_string(i) + ". На позиции " + to_string(pos) + " записано " + to_string(prepared_data.length()) + " символов \n";
        sleep(1);
    }
    return 0;
}

void *Reader() {
    for (int i = 0; i < READ_OPERATIONS; i++) {
        int pos = random_in_range(0, DATABASE_SIZE - 1);
        string data = db.ReadAt(pos);
        cout << 
            to_string(i) + ") На позиции " + to_string(pos) + " прочитано " + to_string(data.length()) + " символов \n";
        
        sleep(1);
    }
    return 0;
}


int main() {
    Writer();
    Reader();
    cout << "Записано: " + to_string(db.GetDataWritten()) + " символов \n";
    cout << "Прочитано: " + to_string(db.GetDataRead()) + " символов \n";
    return 1;
}

