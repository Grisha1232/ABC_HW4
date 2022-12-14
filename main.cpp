#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

string ANSI_RESET = "\033[0m";
string ANSI_RED = "\033[31m";
string ANSI_GREEN = "\033[32m";
string ANSI_YELLOW = "\033[33m";
string ANSI_PURPLE = "\033[35m";
string ANSI_CYAN = "\033[36m";

// Мьютекс для защиты операции чтения
pthread_mutex_t mutex_flowers;
pthread_mutex_t mutex_gardener;
pthread_mutex_t mutex_cout;

// Кол-во дней которое моделируем
int days;
// Переменные, отвечающие за поведение приложения
bool is_console_input = true;
bool is_file_input = true;
bool is_random_input = true;

int seed;
// Переменная, отвечающая за завершение работы садовников
bool work_is_done = true;

// Переменная для ввода данных
string str;
string into_the_out_file = "";

ifstream input;
ofstream output;


// Структура клумбы
std::vector<int> flowers = std::vector<int>(40);
// Массив для ввода данных
std::vector<int> indexes;
// Массив с индексами цветов, которые увядают
std::vector<int> indexes_for_gardeners;

// Структур садовника
struct Gardener {
    // id садовника (1 или 2)
    int id;

    Gardener(int i) {
        id = i;
    }
};

// Садовник поливает цветок
void *waterFlower(void *param) {
    auto *gar = (Gardener *) param;
    while (days) {// Пока дни наблюдений не кончились продолжаем работать
        sleep(2); // Садовники не флэш чтобы моментально реагировать на появление увядающего цветка
        if (!indexes_for_gardeners.empty() && indexes_for_gardeners.size() >= 2) {
            pthread_mutex_lock(&mutex_gardener);// Блокируем другому садовнику читать одну и туже память
            int i = *indexes_for_gardeners.begin();
            pthread_mutex_lock(&mutex_cout);
            if (!is_file_input) {
                cout << "Gardener " << gar->id << " water the flower number " << i + 1 << "\n";
            } else {
                into_the_out_file += "Gardener " + to_string(gar->id) + " water the flower number " + to_string(i + 1) + "\n";
            }
            pthread_mutex_unlock(&mutex_cout);
            flowers.at(i) = 0;                                         // поливаем цветочек
            indexes_for_gardeners.erase(indexes_for_gardeners.begin());// Удаляем индекс политого цветочка, чтобы его не полил другой садовник
            sleep(1);                                                  // отдыхаем после проделанного
            pthread_mutex_unlock(&mutex_gardener);                     // Даем возможность поработать другому садовнику
            continue;
        }
        pthread_mutex_lock(&mutex_gardener);// Блокируем другому садовнику читать одну и туже память
        if (!indexes_for_gardeners.empty()) {
            int i = *indexes_for_gardeners.begin();
            pthread_mutex_lock(&mutex_cout);
            if (!is_file_input) {
                cout << "Gardener " << gar->id << " water the flower number " << i + 1 << "\n";
            } else {
                into_the_out_file += "Gardener " + to_string(gar->id) + " water the flower number " + to_string(i + 1) + "\n";
            }
            pthread_mutex_unlock(&mutex_cout);
            flowers.at(i) = 0;                                         // поливаем цветочек
            indexes_for_gardeners.erase(indexes_for_gardeners.begin());// Удаляем индекс политого цветочка, чтобы его не полил другой садовник
            sleep(1);
        }
        pthread_mutex_unlock(&mutex_gardener);// Даем возможность поработать другому садовнику
    }
    return nullptr;
}

// Указываем какие цветочки начнут вянуть
void *specifyWhichFlowersGonnaFade(void *param) {
    sleep(2);
    while (days) {                      // Пока дни наблюдений не закончились продолжаем наблюдать
        if (work_is_done) {             // Если предыдущая работа выполнена -> вводим другие данные и наблюдаем
            cout << "days left: " << days << "\n";
            if (is_console_input) {     // Консольный ввод
                int n;
                cout << "How many flowers gonna fade today? (1 < n <= 40):";
                cin >> n;
                indexes = vector<int>(n);
                cout << "Enter " << n << " index of flower that will fade at night divided by space:";
                for (int i = 0; i < n; ++i) {
                    cin >> indexes.at(i);
                }
                cout << "\n";
            } else if (is_file_input) {// Файловый ввод
                int n;
                input >> n;
                indexes = vector<int>(n);
                for (int i = 0; i < n; ++i) {
                    input >> indexes.at(i);
                }
            } else {// Рандомный ввод
                srand(seed++);
                int n = rand() % 39 + 1;
                indexes = vector<int>(n);
                for (int i = 0; i < n; ++i) {
                    int a = rand() % 40;
                    while (std::find(indexes.begin(), indexes.end(), a) != indexes.end()) {
                        a = rand() % 40;
                    }
                    indexes.at(i) = a;
                }
                cout << "Number of flowers that will fade today: " << n << "\n";
            }
            work_is_done = false;
            for (int i: indexes) {
                sleep(1);                               // Цветочки вянут не все моментально
                pthread_mutex_lock(&mutex_flowers); // Блокируем изменение переменной
                flowers.at(i) = 1;
                pthread_mutex_lock(&mutex_cout);
                if (!is_file_input) {
                    string line = "flower number " + to_string(i + 1) + " start to fade\n";
                    cout << line;
                } else {
                    into_the_out_file += "flower number " + to_string(i + 1) + " start to fade\n";
                }
                pthread_mutex_unlock(&mutex_cout);
                indexes_for_gardeners.push_back(i);  // Добавляем в переменную индекс цветка
                pthread_mutex_unlock(&mutex_flowers);// даем доступ переменной
            }
        }
        if (indexes_for_gardeners.empty()) {// если не осталось увядающих цветков значит день закончен
            days--;
            work_is_done = true;
        }
    }
    return nullptr;
}


int main(int argc, char *argv[]) {
    auto gardener_1 = Gardener(1);
    auto gardener_2 = Gardener(2);

    // Задаем потоки
    pthread_t thread_flowerbed; // Для изменения состояние цветков
    pthread_t thread_gardener_1;// Для садовника номер 1
    pthread_t thread_gardener_2;// Для садовника номер 2

    // Инициализация mutex
    pthread_mutex_init(&mutex_flowers, nullptr);
    pthread_mutex_init(&mutex_gardener, nullptr);
    pthread_mutex_init(&mutex_cout, nullptr);

    // Определяем какой ввод нам нужен
    if (argc == 1) {// Консольный ввод => ./program.exe
        is_console_input = true;
        is_file_input = false;
        is_random_input = false;
        int variant;
        cout << "Which input do u prefer?\n"
                "1. Console input\n"
                "2. Random input\n"
                "3. File input output\n";
        cin >> variant;
        if (variant == 1) {
            is_console_input = true;
            is_file_input = false;
            is_random_input = false;
        } else if (variant == 2) {
            is_console_input = false;
            is_file_input = false;
            is_random_input = true;
            cout << "Enter the seed number:";
            cin >> seed;
        } else if (variant == 3) {
            is_console_input = false;
            is_file_input = true;
            is_random_input = false;
            cout << "Enter the input file name(Check if it exist in folder with program): ";
            cin >> str;
            input = ifstream(str);
            if (!input.is_open()) {
                cout << "Incorrect name of file or it doesnt even exist";
                return 1;
            }
            cout << "Enter the output file name(Check if it exist in folder with program): ";
            cin >> str;
            output = ofstream(str);
            if (!output.is_open()) {
                cout << "Incorrect name of file or it doesnt even exist";
                return 1;
            }
        }
    } else if (argc == 2) {// Рандомизированный ввод => ./program.exe seed (где seed - это константа для рандомизации)
        is_console_input = false;
        is_file_input = false;
        is_random_input = true;
        strtol(argv[1], nullptr, seed);
    } else if (argc == 3) {// Файловый ввод => ./program.exe input.txt output.txt
        is_console_input = false;
        is_file_input = true;
        is_random_input = false;
        input = ifstream(argv[1]);
        output = ofstream(argv[2]);
        if (!input.is_open() || !output.is_open()) {
            cout << "Incorrect name of file / files or it doesnt even exist";
            return 1;
        }
    }


    if (!is_file_input) {
        cout << "How many days do u wanna see?: ";
        cin >> days;
    } else {
        input >> days;
    }

    // Инициализация потоков клумбы и садовников 1 и 2
    pthread_create(&thread_flowerbed, nullptr, specifyWhichFlowersGonnaFade, &indexes);// Создаем поток для клумбы
    pthread_create(&thread_gardener_1, nullptr, waterFlower, &gardener_1);             // Создаем поток для садовника номер 1
    pthread_create(&thread_gardener_2, nullptr, waterFlower, &gardener_2);             // Создаем поток для садовника номер 2


    work_is_done = true;// Работа завершена.
    // Ждем пока садовники закончат поливать оставшиеся цветочки
    pthread_join(thread_gardener_1, nullptr);
    pthread_join(thread_gardener_2, nullptr);
    if (is_file_input) {
        input.close();
        output << into_the_out_file;
        output.close();
    }
}
