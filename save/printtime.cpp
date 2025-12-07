#include <iostream>
#include <expected>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>

class Awesome {
  private:
    int cool = 10;
  public:
    int& Cool() {
      return this->cool;
    }
    void printCool() {
      std::cout << cool << "\n";
    }
};


int main
(
    int argc,
    char **argv
)
{
    
    Awesome great;
    great.printCool();
    great.Cool() = 20;
    great.printCool();

    std::chrono::seconds duration(1);

    while (true) {


        char key_press = _getch();

        std::cout << "You pressed: " << key_press << std::endl;

        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t t_c = std::chrono::system_clock::to_time_t(now);
        std::tm* ptm = std::localtime(&t_c);
        std::cout << "\033[2J";
        std::cout << std::put_time(ptm, "%H:%M:%S") << std::endl;
        std::this_thread::sleep_for(duration);

    }


    std::vector<std::vector<int>> vec(3, std::vector<int>(4, 0)); // Generate grid of 0s

    return 0;
}