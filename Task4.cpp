#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <random>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <pthread.h>
#include <queue>
#include <list>
#include <mutex>
#include <fstream>

using namespace std;

#define DATABASE_SIZE 64
#define WRITE_OPERATIONS 4
#define READ_OPERATIONS 4
#define WRITERS_READERS_CNT 18

string thread_symbols = "ABCDEFGHIJKLMNOPRSTUVWXYZ";
pthread_mutex_t database_locker;

ofstream db_log;

struct Operation {
    void *function;
    void *args;
};

//Инкапсулированная в виде базы данных очередь символов
class Database {
    private:
        queue<char> data;
        queue<Operation> readQueue;
        int dataWritten = 0;
        bool busy = false;
    public:
        //Запись данных в базу
        void Write(char toWrite) {
            pthread_mutex_lock(&database_locker); 
            {
                if (data.size() < DATABASE_SIZE) {
                    data.push(toWrite);
                    dataWritten++;
                }
            }
            pthread_mutex_unlock(&database_locker);
        }
        //Чтение из базы
        char Read() {
            if (busy) {
                readQueue.push(Operation());
            }
            while (busy) {
                usleep(25);
            }
            busy = true;
            if (data.size() > 0) {
                char read_data = data.front();
                data.pop();
                dataWritten--;
                busy = false;
                return read_data;
            }
            busy = false;
            return '\t';
        }

        void ConnectionOpen() {
            usleep(250);
        }
        
        void ConnectionClose() {
            usleep(250);
        }

        int GetWrittenDataAmount() {
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
        db_log << msg + "\n";
        sleep(1);
    }
    return 0;
}

//Метод чтения для потока
void *Reader(void* args) {
    int *writer_Id = (int *) args;
    for (int i = 0; i < READ_OPERATIONS; i++) {
        char data = db.Read();
        string msg = "Считан символ ";
        msg.append(1, data);
        msg += " в потоке " + to_string(*writer_Id) +  "\n";
        cout << msg;
        db_log << msg;
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
        pthread_create(&readers[i], NULL, Reader, &i);
    }
    for (int j = 0; j < WRITERS_READERS_CNT; j++) {
        pthread_join(readers[j], NULL);
    }    
    return 0;
}

//Параллельно создает читателей и писателей
void InitThreads() {
    pthread_t writers_creator;
    pthread_t readers_creator;
    pthread_mutex_init(&database_locker, NULL);
    pthread_create(&writers_creator, NULL, init_writers, NULL);
    pthread_create(&readers_creator, NULL, init_readers, NULL);
    pthread_join(writers_creator, NULL);
    pthread_join(readers_creator, NULL);
}



int main() {
    db_log.open("log.txt");
    InitThreads();
    cout << "Записано информации: " + to_string(db.GetWrittenDataAmount()) + " символов \n";
    db_log.close();
    return 1;
}

