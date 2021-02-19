#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <queue>

using namespace std;

#define DATABASE_SIZE 64
#define WRITE_OPERATIONS 5
#define READ_OPERATIONS 15
#define WRITERS_READERS_CNT 3

string thread_symbols = "ABCDEFGHIJKLMNOPRSTUVWXYZ";

//Инкапсулированная в виде базы данных очередь символов
class Database {
    private:
        queue<char> data;  
        int dataWritten = 0;   
    public:
        //Запись данных в базу
        void Write(char toWrite) {
            if (data.size() < DATABASE_SIZE) {
                data.push(toWrite);
                dataWritten++;
            }
        }
        char Read() {
            if (data.size() > 0) {
                char read_data = data.front();
                data.pop();
                dataWritten--;
                return read_data;
            }
            else return '\n';
        }
        int GetDataWritten() {
            return dataWritten;
        }
};

Database db;

//Метод записи для потока
void *Writer(void *args) {
    int *writer_Id = (int *) args;
    for (int i = 0; i < WRITE_OPERATIONS; i++) {
        char data = thread_symbols.at(*writer_Id);
        db.Write(data);
        string msg = "Записан символ ";
        msg.append(1, data);
        msg.append(" в потоке ");
        msg.append(to_string(*writer_Id));
        cout << msg +"\n";
        sleep(1);
    }
    return 0;
}

//Метод чтения для потока
void *Reader(void* args) {
    for (int i = 0; i < READ_OPERATIONS; i++) {
        char data = db.Read();
        cout << 
            to_string(i) + ") Считано символ" + to_string(data) + "  \n";
        sleep(1);
    }
    return 0;
}

//Создание писателей
void *init_writers(void *args) {
    pthread_t writers[WRITERS_READERS_CNT];
    for (int i = 0; i < WRITERS_READERS_CNT; i++) {
        pthread_create(&writers[i], NULL, Writer, &i);
    }
    for (int j = 0; j < WRITERS_READERS_CNT; j++) {
        pthread_join(writers[j], NULL);
    }    
    return 0;
}

//Создание читателей
void *init_readers(void *args) {
    pthread_t readers[WRITERS_READERS_CNT];
    for (int i = 0; i < WRITERS_READERS_CNT; i++) {
        pthread_create(&readers[i], NULL, Reader, NULL);
    }
    for (int j = 0; j < WRITERS_READERS_CNT; j++) {
        pthread_join(readers[j], NULL);
    }    
    return 0;
}

//Параллельно создает читателей и писателей
void InitThreads() {
    pthread_t writers_creator;
    //pthread_t readers_creator;
    pthread_create(&writers_creator, NULL, init_writers, NULL);
    //pthread_create(&readers_creator, NULL, init_readers, NULL);
    pthread_join(writers_creator, NULL);
    //pthread_join(readers_creator, NULL);
}



int main() {
    InitThreads();
    cout << "Записано информации: " + to_string(db.GetDataWritten()) + " символов \n";
    return 1;
}

