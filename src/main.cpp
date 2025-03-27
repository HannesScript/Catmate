#include "api/api.cpp"

// ----- Main ----- //
int main(int argc, char *argv[])
{
    CatmateAPI api = CatmateAPI();

    std::string command;
    while (std::getline(std::cin, command))
        api.processCmd(command);   // Whenever a command is entered, process the command
    
    return 0;
}
