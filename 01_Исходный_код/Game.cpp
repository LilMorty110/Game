#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdlib>

using namespace std;

void setup_console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void clear_screen() {
    system("cls");
}

int read_key() {
    cout.flush();
    return _getch();
}

void wait_key() {
    cout << "\nНажмите любую клавишу...";
    read_key();
}

class Interface {
public:
    void Vibor() {
        cout << "\n1. Перейти в другую локацию\n";
        cout << "2. Показать текущую локацию\n";
        cout << "3. Показать переходы\n";
        cout << "0. Выйти из игры\n";
        cout << "Ваш выбор: ";
    }
};

class Golovolomka {
public:
    Golovolomka(int my_ID) {
        ID = my_ID;
    }

    bool Puzzle() {
        char answer;
        char correct = '0';

        if (ID == 1) {
            cout << "\nНа двери генераторной установлен кодовый замок.\n";
            cout << "Вычислите: 8 + 4 * 3 = ?\n";
            cout << "1. 36\n2. 20\n3. 24\n";
            correct = '2';
        }
        else if (ID == 2) {
            cout << "\nТерминал архива показывает зашифрованное слово: EPPS.\n";
            cout << "Каждая буква заменена следующей буквой латинского алфавита.\n";
            cout << "Какое слово было до шифрования?\n";
            cout << "1. DOOR\n2. ROOM\n3. CODE\n";
            correct = '1';
        }
        else if (ID == 3) {
            cout << "\nПеред выходом три рычага: A, B и C.\n";
            cout << "A не первый. B сразу после A. C раньше A.\n";
            cout << "В каком порядке нужно нажать все три рычага?\n";
            cout << "1. ABC\n2. BCA\n3. CAB\n";
            correct = '3';
        }

        cout << "0. Отмена\nОтвет: ";
        answer = (char)read_key();
        if (answer == '0') {
            cout << "\nВы отошли от двери.\n";
            return false;
        }
        if (answer == correct) {
            return true;
        }
        cout << "\nНеверно. Дверь осталась закрытой.\n";
        return false;
    }

private:
    int ID;
};

class Location {
public:
    Location(int my_ID, string my_name, string filename, bool my_open, Golovolomka* my_puzzle = nullptr) {
        ID = my_ID;
        name = my_name;
        open = my_open;
        puzzle = my_puzzle;

        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            shape.push_back(line);
        }
    }

    int get_ID() { return ID; }
    string get_name() { return name; }
    bool get_open() { return open; }
    Golovolomka* get_puzzle() { return puzzle; }
    vector<int> get_connections() { return connections; }

    bool map_loaded() {
        return !shape.empty();
    }

    void add_connection(int number) {
        connections.push_back(number);
    }

    void add_incoming(int number) {
        incoming.push_back(number);
    }

    void open_location() {
        open = true;
        puzzle = nullptr;
    }

    void get_text() {
        cout << "\n=== " << name << " ===\n";
        for (int i = 0; i < (int)shape.size(); i++) {
            cout << shape[i] << "\n";
        }
        cout << "# - стена, . - пол, @ - игрок, D - дверь\n";
    }

private:
    int ID;
    string name;
    vector<string> shape;
    vector<int> connections;
    vector<int> incoming;
    bool open;
    Golovolomka* puzzle;
};

class Character {
public:
    int get_location() {
        return location;
    }

    bool Interact(Location& next) {
        if (next.get_open()) {
            return true;
        }
        cout << "\nЛокация закрыта. Нужно решить головоломку.\n";
        if (next.get_puzzle() != nullptr && next.get_puzzle()->Puzzle()) {
            next.open_location();
            cout << "\nВерно! Локация открыта.\n";
            return true;
        }
        return false;
    }

    bool Move_to(Location& next, vector<Location>& locs) {
        vector<int> ways = locs[location - 1].get_connections();
        for (int i = 0; i < (int)ways.size(); i++) {
            if (ways[i] == next.get_ID() && next.get_open()) {
                location = next.get_ID();
                return true;
            }
        }
        return false;
    }

private:
    int location = 1;
};

void show_connections(Location& current, vector<Location>& locs) {
    vector<int> ways = current.get_connections();
    cout << "\nДоступные переходы:\n";
    for (int i = 0; i < (int)ways.size(); i++) {
        int index = ways[i] - 1;
        cout << ways[i] << ". " << locs[index].get_name();
        if (locs[index].get_open()) {
            cout << " (открыта)\n";
        }
        else {
            cout << " (закрыта)\n";
        }
    }
}

int main() {
    setup_console();
    clear_screen();

    Golovolomka gol1(1), gol2(2), gol3(3);
    Location loc1(1, "Диспетчерская", "loc1.txt", true);
    Location loc2(2, "Генераторная", "loc2.txt", false, &gol1);
    Location loc3(3, "Архив станции", "loc3.txt", false, &gol2);
    Location loc4(4, "Выход на поверхность", "loc4.txt", false, &gol3);
    vector<Location> locs{loc1, loc2, loc3, loc4};

    locs[0].add_connection(2);
    locs[1].add_connection(1);
    locs[1].add_connection(3);
    locs[2].add_connection(2);
    locs[2].add_connection(4);

    for (int i = 0; i < (int)locs.size(); i++) {
        if (!locs[i].map_loaded()) {
            cout << "Не найдена карта loc" << i + 1 << ".txt\n";
            wait_key();
            return 1;
        }
        vector<int> ways = locs[i].get_connections();
        for (int j = 0; j < (int)ways.size(); j++) {
            locs[ways[j] - 1].add_incoming(locs[i].get_ID());
        }
    }

    Interface menu;
    Character player;
    char choice;

    cout << "АВАРИЙНЫЙ ВЫХОД\n";
    cout << "Группа ИСП-3,4\n";
    cout << "Выполнили: Емелев Никита Александрович, Ананина Кира Алексеевна\n";
    cout << "\nВы техник подземной станции. После аварии двери заблокированы.\n";
    cout << "Откройте проходы и выберитесь на поверхность.\n";
    wait_key();

    while (true) {
        clear_screen();
        int current = player.get_location() - 1;
        locs[current].get_text();
        menu.Vibor();
        choice = (char)read_key();
        if (choice == '0') {
            break;
        }
        clear_screen();

        if (choice == '1') {
            show_connections(locs[current], locs);
            cout << "Номер локации (0 - назад): ";
            char number = (char)read_key();
            if (number == '0') {
                continue;
            }

            int next = -1;
            vector<int> ways = locs[current].get_connections();
            for (int i = 0; i < (int)ways.size(); i++) {
                if (number - '0' == ways[i]) {
                    next = ways[i] - 1;
                }
            }

            if (next == -1) {
                cout << "\nИз этой локации такого перехода нет.\n";
                wait_key();
                continue;
            }
            if (!locs[next].get_open()) {
                clear_screen();
                bool opened = player.Interact(locs[next]);
                wait_key();
                if (!opened) {
                    continue;
                }
            }
            player.Move_to(locs[next], locs);
            if (player.get_location() == 4) {
                clear_screen();
                locs[next].get_text();
                cout << "\nВы открыли аварийный люк и поднялись на поверхность.\n";
                cout << "ПОБЕДА! Вы выбрались с подземной станции.\n";
                wait_key();
                break;
            }
        }
        else if (choice == '2') {
            locs[current].get_text();
            wait_key();
        }
        else if (choice == '3') {
            show_connections(locs[current], locs);
            wait_key();
        }
        else {
            cout << "Такого пункта нет.\n";
            wait_key();
        }
    }

    return 0;
}
