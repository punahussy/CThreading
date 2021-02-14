#include <iostream>
#include <pthread.h>
#include <cstdlib>
#include <stack>

using namespace std;

#define THREADS_CNT 5

//Функция, меняющая данные по адресу
void *change_data(void *data) {
    string *new_data = (string *) data;
    *new_data = "Ready \n"; 
    return 0;
}

int main() {
    pthread_t threads[THREADS_CNT];
    string thread_data[THREADS_CNT];

    //Создание потоков
    for (int i = 0; i < THREADS_CNT; i++) {
        thread_data[i] = "Loading..."; 
        pthread_create(&threads[i], NULL, change_data, &thread_data[i]);
    }

    //Ожидание завершения выполнения потоков
    for (int j = 0; j < THREADS_CNT; j++) {
        pthread_join(threads[j], NULL);
    }

    //Вывод в консоль
    for (int k = 0; k < THREADS_CNT; k++) {
        cout << thread_data[k];
    }

}