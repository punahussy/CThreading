#include <iostream>
#include <map>
#include <cstring>
#include <unistd.h>

using namespace std;

#define THREAD_COUNT 3
#define NUM_OF_DIGITS 10

char _string[THREAD_COUNT * NUM_OF_DIGITS];
map<int, char> thread_letters;
bool string_available = true;

//Добавляет один указанный символ в строку
void add_char(char *_char) {
    strcat(_string, _char);
}

//Добавляет символ в строку исходя из номера потока
void *write_chars(void *arg) {
    char *thread_char = (char *) arg;
    /* Смотрим на флаг и ждем пока переменная станет доступна*/
    while (!string_available) {
        sleep(1);
    }
    string_available = false;
    for (int i = 0; i < 10; i++ )  {
        add_char(thread_char);
        cout << _string;
        cout << '\n';
        sleep(1);
    }
    //После завершения операций снимаем флаг
    string_available = true;
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
    thread_letters[0] = 'A';
    thread_letters[1] = 'B';
    thread_letters[2] = 'C';
    thread_letters[3] = 'D';

    create_run_threads();
    cout << _string << "\n";
}