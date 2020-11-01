#include <iostream>

#include "InteractivePrompt.h"


int main(){
    FileManager fm;
    InteractivePrompt prompt(fm);
    prompt.welcome(std::cout);
    prompt.promptLoop();
}