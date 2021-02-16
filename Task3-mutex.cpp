#include <iostream>
#include <map>
#include <cstring>
#include <unistd.h>
#include <mutex>

using namespace std;

#define THREAD_COUNT 3
#define NUM_OF_DIGITS 10

char _string[THREAD_COUNT * NUM_OF_DIGITS];
map<int, char> thread_letters;
mutex string_lock;

//Добавляет символ в строку исходя из номера потока
void *write_chars(void *arg) {
    char *thread_char = (char *) arg;
    /* Блокируем переменную мьютексом чтобы предотварить изменение данных
        другими потоками */
    string_lock.lock();
    for (int i = 0; i < 10; i++ )  {
        strcat(_string, thread_char);
        cout << _string;
        cout << '\n';
        sleep(1);
    }
    //После завершения операций над строкой разблокируем мьютекс
    string_lock.unlock();
    return 0;
}

//Создает и запускает потоки, дожидается их завершения
void create_run_threads() {
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, write_chars, &thread_letters[i]);
    }
    
    for (int j = 0; j < THREAD_COUNT; j++) {
        pthread_join(threads[j], NULL);
    }
}

int main() {
    string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_letters[i] = alphabet[i];
    }

    create_run_threads();
    cout << _string << "\n";
}