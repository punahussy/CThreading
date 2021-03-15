#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include <sstream>

using namespace std;

//база данных
queue<string> Common_data;
//очередь читателей
queue<int> Readers_queue;
//параметры базы данных и читателей-писателей
int Data_size = 0;
int Reader_threads_count = 0;
int Reader_part_size = 0;
int Writer_threads_count = 0;
int Writer_part_size = 0;
//мьютексы
pthread_mutex_t Common_mutex;
pthread_mutex_t Read_mutex;
//структура, хранящая данные потока
struct thread_data {
    string unique_char = "";
    string received_data = "";
    int thread_id;
};

void* OpenConnection(pthread_mutex_t *mutex) {
    pthread_mutex_lock(&(*mutex));
    return 0;
}

void* CloseConnection(pthread_mutex_t *mutex) {
    pthread_mutex_unlock(&(*mutex));
    usleep(1000);
    return 0;
}

void* Write(void *arg) {
    auto *thread_arg = static_cast<thread_data*>(arg);
    int i = 0;
    while(i < Writer_part_size) {
        OpenConnection(&Common_mutex);
        if(Common_data.size() < Data_size) {
            cout << "Write " << thread_arg->unique_char << endl;
            usleep(1000);
            Common_data.push(thread_arg->unique_char);
            i++;
        }
        else if (Reader_threads_count <= 0)
            i++;

        CloseConnection(&Common_mutex);
    }
    Writer_threads_count--;
    return 0;
}

void* Read(void *arg) {
    thread_data *thread_arg = (thread_data *) arg;
    int i = 0;
    while(i < Reader_part_size) {
        pthread_mutex_lock(&(Read_mutex));
        Readers_queue.push(thread_arg->thread_id);
        pthread_mutex_unlock(&(Read_mutex));
        while (Readers_queue.front() != thread_arg->thread_id) {}
        OpenConnection(&Common_mutex);
        if (Common_data.size() > 0) {
            string current_symbol = Common_data.front();
            thread_arg->received_data += Common_data.front();
            usleep(1000);
            Common_data.pop();
            cout << "Read " << current_symbol << endl;
            i++;
        } else if (Writer_threads_count <= 0) i++;
        CloseConnection(&Common_mutex);
        pthread_mutex_lock(&(Read_mutex));
        Readers_queue.pop();
        pthread_mutex_unlock(&(Read_mutex));
    }
    Reader_threads_count--;
    return 0;
}

int main(int argc, char** argv) {
    int readers_amount = 0;
    int writers_amount = 0;
    cout << "Enter db size: ";
    cin >> Data_size;
    cout << "Enter num of readers: ";
    cin >> readers_amount;
    cout << "Enter part of data to read: ";
    cin >> Reader_part_size;
    cout << "Enter num of writers: ";
    cin >> writers_amount;
    cout << "Enter part of data to write: ";
    cin >> Writer_part_size;

    int threads_number = writers_amount + readers_amount;
    Reader_threads_count = readers_amount;
    Writer_threads_count = writers_amount;

    pthread_mutex_init(&(Common_mutex), NULL);
    pthread_mutex_init(&(Read_mutex), NULL);

    pthread_t threads[threads_number];
    thread_data thread_arg[threads_number];

    for (int i = 0; i < writers_amount; i++) {
        stringstream unic_symbol;
        unic_symbol << "thread_" << i;
        thread_arg[i].unique_char = unic_symbol.str();
        thread_arg[i].thread_id = i;
        pthread_create(&threads[i], nullptr, Write, &thread_arg[i]);
    }

    for (int i = writers_amount; i < threads_number; i++) {
        thread_arg[i].thread_id = i;
        pthread_create(&threads[i], nullptr, Read, &thread_arg[i]);
    }

    for (int i = 0; i < threads_number; i++)
        pthread_join(threads[i], 0);

    stringstream dataResult;
    for (int i = 0; i < threads_number; i++)
        dataResult << thread_arg[i].received_data;
    cout << "info read: " << dataResult.str() + "\n";

    stringstream result_string;
    Data_size = Common_data.size();
    for (int m = 0; m < Data_size; m++) {
        result_string << Common_data.front();
        Common_data.pop();
    }

    cout << "result: " << result_string.str();

    pthread_mutex_destroy(&(Common_mutex));
    pthread_mutex_destroy(&(Read_mutex));
    return 0;
}