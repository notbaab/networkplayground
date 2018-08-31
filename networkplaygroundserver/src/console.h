#include <vector>
#include <string>
#include <functional>

using ConsoleCallback = void (*) (std::vector<std::string>);
void interactive_console();
void add_command(std::vector<std::string> hints, std::string full_command, ConsoleCallback);
