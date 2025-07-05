#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <algorithm>
#include <vector>
#include <conio.h>

bool toroidal_mode = false;

void countdown()
{
    for (int i = 3; i > 1; i--)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::cout << i;
        for (int y = 0; y < 3; y++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            std::cout << ".";
        }
    }
    std::cout << 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}
void grid_display(std::vector<char>& grid, std::pair<int, int> grid_size)
{
    system("cls");
    for (int i = 0; i < 5; i++)
    {
        std::cout << std::endl;
    }
    std::cout << "\t\t\t+";
    for (int y = 0; y < grid_size.second; y++)
    {
        std::cout << "--";
    }
    std::cout << '+' << std::endl;
    for (int i = 0; i < grid_size.first; i++)
    {
        std::cout << "\t\t\t|";
        for (int y = 0; y < grid_size.second; y++)
        {
            std::cout << grid[i * grid_size.second + y] << ' ';
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "\t\t\t+";
    for (int y = 0; y < grid_size.second; y++)
    {
        std::cout << "--";
    }
    std::cout << '+' << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}
void grid_fill(std::vector<char>& grid, std::pair<int, int> grid_size)
{
    int pos = 0;
    char prev_char = ' ';
    grid[pos] = '.';
    bool on_choosing = true;

    while (on_choosing)
    {
        grid_display(grid, grid_size);
        std::cout << std::endl << std::endl;
        std::cout << "\tLEFT, UP, DOWN, RIGHT, DOWN - Передвижение\n\tE - Выбрать, x2 Убрать\n\tEnter - Завершить выбор";

        if (_kbhit())
        {
            int old_pos = pos;

            switch (_getch())
            {
            case 75:
                if (pos % grid_size.second != 0) { pos--; }
                break;
            case 72:
                if (pos >= grid_size.second) { pos -= grid_size.second; }
                break;
            case 77:
                if ((pos + 1) % grid_size.second != 0) { pos++; }
                break;
            case 80:
                if (pos + grid_size.second < grid_size.first * grid_size.second) { pos += grid_size.second; }
                break;
            case 101:
                if (grid[pos] == '*') { grid[pos] = ' '; }
                else { grid[pos] = '*'; }
                break;
            case 99:
                std::fill(grid.begin(), grid.end(), ' ');
                grid[pos] = '.';
                break;
            case 13:
                if (auto count = count_if(grid.begin(), grid.end(), [](char a) {return a == '*'; }) == 0)
                {
                    std::cout << std::endl << std::endl << "\tПредупреждение: из за нулевого количества клеток игра не запустится.";
                }
                std::cout << "\t\t";
                countdown();
                std::cout << std::endl << std::endl << std::endl;
                on_choosing = false;
                break;
            }

            if (old_pos != pos)
            {
                if (grid[old_pos] == '.')
                {
                    grid[old_pos] = (prev_char == '*') ? '*' : ' ';
                }

                prev_char = (grid[pos] == '*') ? '*' : ' ';
                grid[pos] = '.';
            }
        }
    }
}
std::vector<char> start_init(std::pair<int, int>& grid_size)
{
    system("cls");
    std::cout << "\n\t\t\t\t\tВы в игре \"Game of Life\", это самая достоверная симуляция жизни,\n\t\t\t\t\tКак клеток так и многоклеточныых существ (в том числе и человека) на 2D пространстве." << std::endl << std::endl;
    std::cout << "\tВыберите тип границ:\n\t1 - Классические (мертвые)\n\t2 - Тороидальные (бесконечные)\n\t>> ";

    int border_type = 0;
    while (border_type < 1 || border_type > 2)
    {
        std::cin >> border_type;
        if (border_type == 1 || border_type == 2) break;
        std::cout << "\tНекорректный ввод, попробуйте еще раз: ";
    }
    toroidal_mode = (border_type == 2);

    while (grid_size.first <= 10 || grid_size.second <= 10)
    {
        system("cls");
        std::cout << "\n\tВысота поля: ";
        std::cin >> grid_size.first;
        std::cout << "\n\tШирина поля: ";
        std::cin >> grid_size.second;
        if (grid_size.first <= 10 || grid_size.second <= 10)
        {
            std::cout << "\n\tНекорректное значение, попробуйте еще раз (размер минимум 11x11).\t";
            countdown();
        }
    }
    std::vector<char> grid(grid_size.first * grid_size.second, ' ');
    std::cout << "\n\n\tСейчас вы будете перенаправлены на страницу выбора первоначального положения клеток.\t";
    countdown();
    system("cls");
    grid_fill(grid, grid_size);
    return grid;
}
void logic(std::vector<char>& grid, std::pair<int, int> grid_size)
{
    std::vector<bool> stats;
    for (int i = 0; i < grid_size.first * grid_size.second; i++)
    {
        std::pair<int, int> current_poses = { i / grid_size.second, i % grid_size.second };
        std::pair<int, int> directions[] =
        {
            {current_poses.first - 1, current_poses.second - 1},
            {current_poses.first - 1, current_poses.second},
            {current_poses.first - 1, current_poses.second + 1},
            {current_poses.first, current_poses.second - 1},
            {current_poses.first, current_poses.second + 1},
            {current_poses.first + 1, current_poses.second - 1},
            {current_poses.first + 1, current_poses.second},
            {current_poses.first + 1, current_poses.second + 1}
        };

        int alive_counter = 0;
        if (!toroidal_mode)
        {
            for (int y = 0; y < sizeof(directions) / sizeof(directions[0]); y++)
            {
                if (!(directions[y].first < 0 || directions[y].first >= grid_size.first || directions[y].second < 0 || directions[y].second >= grid_size.second))
                {
                    int index = directions[y].first * grid_size.second + directions[y].second;
                    if (grid[index] == '*') { alive_counter++; }
                }
            }
        }
        else
        {
            for (int y = 0; y < sizeof(directions) / sizeof(directions[0]); y++)
            {
                int row = (directions[y].first + grid_size.first) % grid_size.first;
                int col = (directions[y].second + grid_size.second) % grid_size.second;

                int index = row * grid_size.second + col;
                if (grid[index] == '*') { alive_counter++; }
            }
        }
        if (grid[i] == '*')
        {
            (alive_counter == 2 || alive_counter == 3) ? stats.push_back(true) : stats.push_back(false);
        }
        else
        {
            alive_counter == 3 ? stats.push_back(true) : stats.push_back(false);
        }
    }
    for (int i = 0; i < grid_size.first * grid_size.second; i++)
    {
        (stats[i] == true) ? grid[i] = '*' : grid[i] = ' ';
    }
}
void game(std::vector<char> grid, std::pair<int, int>& grid_size)
{
    auto population = count_if(grid.begin(), grid.end(), [](char a) { return a == '*'; });
    int generation = 1;
    int speed = 70;

    while (population != 0)
    {
        grid_display(grid, grid_size);
        std::cout << std::endl;
        std::cout << "\tZ - Медленно" << "\tPopulation: " << population << "\n\tX - Средне" << "\tGeneration: " << generation << "\n\tC - Быстро\n\n\tP - Пауза/Продолжить\n\tQ - Остановить";
        logic(grid, grid_size); // логика
        population = count_if(grid.begin(), grid.end(), [](char a) { return a == '*'; });
        generation++;
        if (_kbhit())
        {
            int key = _getch();
            speed = (key == 122) ? 250 : (key == 120) ? 70 : (key == 99) ? 1 : speed;

            if (key == 112)
            {
                int pause_stat = 0;
                while (pause_stat != 112)
                {
                    if (_kbhit()) { pause_stat = _getch(); }
                    std::this_thread::sleep_for(std::chrono::milliseconds(150));
                }
            }
            else if (key == 113)
            {
                std::cout << std::endl << std::endl << std::endl;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
        system("cls");
    }
}

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::pair<int, int> grid_size(0, 0);
    game(start_init(grid_size), grid_size);

    return 0;
}