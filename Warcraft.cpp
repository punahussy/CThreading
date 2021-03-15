#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

pthread_mutex_t unit_mutex;

class Gold_Mine{
private:
    int gold_amount;
    int distance;
public:
    Gold_Mine(int gold_amount = 103, int distance = 10000){
        this->gold_amount = gold_amount;
        this->distance = distance;
    }
    int get_gold_amount(){
        return gold_amount;
    }
    void set_gold_amount(int gold_amount){
        this->gold_amount = gold_amount;
    }
    int get_distance(){
        return distance;
    }
};

Gold_Mine gold_mine;

class Unit {
private:
    int id;
    int dig_amount;
    int speed;
public:
    Unit(int id = 0, int dig_amount = 5, int speed = 10){
        this->id = id;
        this->dig_amount = dig_amount;
        this->speed = speed;
    }
    bool digging(){
        pthread_mutex_lock(&unit_mutex);
        if (gold_mine.get_gold_amount() <= 0){
            pthread_mutex_unlock(&unit_mutex);
            return true;
        }
        int gold_in_mine = gold_mine.get_gold_amount();
        int remaining_gold = (gold_in_mine < this->dig_amount ? 0 : gold_in_mine - this->dig_amount);
        gold_mine.set_gold_amount(remaining_gold);
        cout << "Unit " << this->id << " осталось золота: " << remaining_gold << endl;
        pthread_mutex_unlock(&unit_mutex);
        return false;
    }
    void walk(){
        int walk_time = 1000000 / speed + rand() % 10000;
        usleep(walk_time);
    }
    int get_id(){
        return id;
    }
};

void *unit_thread(void *thread_arg){
    auto unit = static_cast<Unit*>(thread_arg);
    bool is_mine_empty = false;
    while (is_mine_empty != true){
        unit->walk();
        is_mine_empty = unit->digging();
        unit->walk();
    }
    pthread_exit(0);
}

int main() {
    int units_quantity = 0;
    cout << "Enter unit quantity: ";
    cin >> units_quantity;
    pthread_t threads[units_quantity];
    pthread_mutex_init(&unit_mutex,0);
    int result = 0;

    for (int i = 0; i < units_quantity; i++) {
        auto unit = new Unit(i);
        result = pthread_create(&threads[i], NULL, unit_thread, (void*)unit);
    }
    for (int i = 0; i < units_quantity; i++){
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&unit_mutex);
    pthread_exit(NULL);
}
