#include <iostream>
#include <map>
#include <cstring>
#include <unistd.h>

using namespace std;

#define THREAD_COUNT 4
#define NUM_OF_DIGITS 10

char _string[THREAD_COUNT * NUM_OF_DIGITS];
map<int, char> thread_letters;

//Добавляет символ в строку исходя из номера потока
void *write_chars(void *arg) {
    char *thread_char = (char *) arg;
    for (int i = 0; i < 10; i++ )  {
        strcat(_string, thread_char);
    }
    return 0;
}

void create_run_threads() {
    pthread_t threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, write_chars, &thread_letters[i]);
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