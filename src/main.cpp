#include "Application.h"


#include <ftxui/component/component.hpp> 
#include <ftxui/component/component_options.hpp>
#include <ftxui/screen/terminal.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/screen/color.hpp"
#include <ftxui/screen/terminal.hpp>
#include <memory>
#include "ftxui/component/screen_interactive.hpp" 
#include "ftxui/component/component_base.hpp" 
#include "ftxui/component/captured_mouse.hpp"
#include <iostream>


using namespace ftxui;


auto screen = ftxui::ScreenInteractive::Fullscreen();

void exitor()
{
    screen.Exit();
}


int main(){

    using namespace std::string_literals;
    std::cout << "Hello World!" << std::endl;

    Application a(exitor);

    screen.Loop(a.render());



    return 0;
}