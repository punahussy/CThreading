#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <stack>

using namespace std;

#define THREADS_CNT 5

//Структура, объединяющая номер потока и сообщение
struct thread_info {
    int id;
    string message;
};

thread_info thread_data[THREADS_CNT];


//Функция, меняющая данные по адресу
void *change_data(void *data) {
    thread_info *new_data = (thread_info *) data;
    //Меняем сообщение на номер потока
    new_data->message = to_string(new_data->id) + " Ready! \n"; 
    return 0;
}

//Вывод данных в консоль
void print_data() {
    for (int k = 0; k < THREADS_CNT; k++) {
        cout << thread_data[k].message;
    }
}

int main() {
    pthread_t threads[THREADS_CNT];
    //Создание потоков
    for (int i = 0; i < THREADS_CNT; i++) {
        thread_data[i].id = i;
        thread_data[i].message = to_string(i) + " Loading... \n"; 
        pthread_create(&threads[i], NULL, change_data, &thread_data[i]);
    }

    //Вывод до завершения всех потоков
    print_data();
    cout << "================ \n";

    //Ожидание завершения выполнения потоков
    for (int j = 0; j < THREADS_CNT; j++) {
        pthread_join(threads[j], NULL);
    }

    //Вывод после завершения всех потоков
    print_data();
}