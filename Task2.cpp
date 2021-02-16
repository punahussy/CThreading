#include <iostream>
#include <cmath>
#include <pthread.h>

using namespace std;
float result = 0;
int unaccounted_members_count;

//Структура, содержащая номер потока и начальный член в его вычислениях
struct thread_data
{
    int id;
    int members_per_thread;
    int start_member;
};

//Считает n-ый член последовательности (вар. 1)
float calc_member(int n) {
    float num = pow(-1.0, n - 1);
    float den = pow(n, n);
    return n;
}

//Вычисляет ряд членов в потоке
void *calc_members(void *data) {
    thread_data *t_data = (thread_data *) data;
    cout << "\n [DEBUG]: Выполняется поток " + to_string(t_data->id);
    for (int i = t_data->start_member; i < t_data->members_per_thread + t_data->start_member; i++) {
        result += calc_member(i);
    }
    return 0;
}

//Создает потоки, распределяет вычисления
void create_run_threads(int threads_count, int member_per_thread) {
    pthread_t threads[threads_count + 1];
    thread_data t_data[threads_count];

    for (int i = 0; i < threads_count; i++) {
        t_data[i].id = i;
        t_data[i].start_member = i * member_per_thread + 1;

        //Перераспределяем неучтенные члены на первый поток
        if (unaccounted_members_count > 0) {
            t_data[i].members_per_thread = member_per_thread + unaccounted_members_count;
            unaccounted_members_count = 0;
        }
        else {
            t_data[i].members_per_thread = member_per_thread;
        }

        cout << "\n [DEBUG]: Создется поток " + to_string(i);
        cout << "\n [DEBUG]: Начальный член для потока " + to_string(t_data[i].start_member);
        cout << "\n [DEBUG]: Членов на поток " + to_string(t_data[i].members_per_thread);

        pthread_create(&threads[i], NULL, calc_members, &t_data[i]);
        cout << "\n [DEBUG]: Создан поток " + to_string(i);
    }

    for (int j = 0; j < threads_count; j++) {
        pthread_join(threads[j], NULL);
    }
}

int main() {
    cout << "\n Введите количество членов последовательности: ";
    int seq_members_count;
    cin >> seq_members_count;
    cout << "\n Введите количество потоков:";
    int threads_count;
    cin >> threads_count;

    if (seq_members_count < threads_count) {
        cout << "\n Количество количество потоков не должно превышать членов последовательности \n";
        exit(-1);
    } 

    int members_per_thread = (int)(seq_members_count / threads_count);
    //Если число членов не делится на число потоков нацело, появляются неучтенные члены
    if (seq_members_count % threads_count != 0) {
        unaccounted_members_count = seq_members_count % threads_count;
    }
    cout << "\n [DEBUG]: Количество членов на поток " + to_string(members_per_thread);
    cout << "\n [DEBUG]: Количество неучтенных членов " + to_string(unaccounted_members_count);
    create_run_threads(threads_count, members_per_thread);

    cout << "\n [RESULT]: " + to_string(result) + '\n';
    return 1;
}
