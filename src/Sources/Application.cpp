#include "Application.h"
#include <exception>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <iostream>
#include <string>


using namespace ftxui;

std::string defaultHashFunction(const std::string& input){
    throw "USED DEFAULT FUNCTION";
    return std::string{input};
}



std::function<std::string(const std::string&)> getHashFunction(int selectedValue){
    switch(selectedValue){
        case 0:
            return computeHashSHA256;
        case 1:
            return computeHashMD5;
        default:
            return defaultHashFunction;
    }
}

namespace Definitions{
    std::vector<std::string> HashFunctionNames{"SHA256", "MD5"};
}


void Application::generateHashVariants(){
    sample.clear();
    generatedValues=0;


    int threadValue = std::stoi(threadsAmountString);
    if(threadValue<0) throw "Value of threads must be >0";

    mainPool.resize(threadValue);

    mainPool.enqueue([&](){
        for(auto a: alphabet){
            for(auto b: alphabet){
                for(auto c: alphabet){
                    for(auto d: alphabet){
                        for(auto e: alphabet){
                            
                            std::lock_guard<std::mutex> lock(generates_mutex);
                            sample.push_back(std::string{a}+std::string{b}+std::string{c}+std::string{d}+std::string{e});
                            
                            generatedValues++;
                        }
                    }
                }
            }
        }

    });
}

void Application::beginHashOperations(){
    {
        std::lock_guard<std::mutex> lock(generates_mutex);
        if(!(generatedValues==maxVariantsAmount)){
            generateHashVariants();
            return;
        }
    }
    


    isStarted=true;    
    found=false;
    showResult=false;
    showHashFunctonSelectBlock=false;

    searchVal = searchForString;
    searchForString="";
    processedValues=0;
    foundIndex=-1;
    valueNotFound=false;



    int threadValue = std::stoi(threadsAmountString);
    if(threadValue<0) throw "Value of threads must be >0";

    mainPool.resize(threadValue);

    searchProcessBegin = std::chrono::high_resolution_clock::now();

    selectedHashFunction = getHashFunction(hashFunctionSelected);




    mainPool.enqueue([&]() {
        std::string value{};
        
        for(int i = 0; i< maxVariantsAmount; i++){
            if (found) {
                break; 
            }
            value=sample[i];
            mainPool.enqueue([&, value, i]() {
                if (found) {
                    return; 
                }
                std::string val = selectedHashFunction(value); 

                if (val == searchVal) {
                    std::lock_guard<std::mutex> lock(results_mutex);
                    foundIndex = i;
                    found=true;
                    showResult=true;
                    processedValues++;
                    searchProcessEnd = std::chrono::high_resolution_clock::now();
                    return;
                    
                }
                std::lock_guard<std::mutex> lock(results_mutex);
                processedValues++;


            });
        }
    });

}

Application::Application(std::function<void()> exitor){
    exitClosure=exitor;
    threadsAmountString = std::to_string(threadsAmount);

    mainContainer = Container::Vertical({
        Container::Horizontal({
            Container::Horizontal({Renderer([&]{return filler();})})|flex,
            Button(" X ", [&]{exitClosure();}),
        }),
        Container::Vertical({
            Renderer([&]{
                return text("Search for Hash Value")|hcenter|bold;
            }),
            Container::Vertical({
                Renderer([&]{return text("Data");})|bold|hcenter,
                Renderer([&]{return separator();}),
                Renderer([&]{
                    return hbox({
                        text("Using alphabet: "),
                        text(alphabet)|bold
                    });
                }),
                Maybe({
                    Container::Horizontal({
                        Renderer([&]{return text("Select used hash function: ");})|vcenter,
                        Radiobox(&Definitions::HashFunctionNames, &hashFunctionSelected)|border
                        
                    }),   
                }, &showHashFunctonSelectBlock),
            })|ftxui::border,
            Container::Horizontal({
                Renderer([&]{return text("Enter the number of threads to use ");})|vcenter,
                Input(&threadsAmountString)|border,
            })|border,
            Container::Horizontal({
                Renderer([&]{
                        return text("Enter the hash value into input");
                    })|vcenter,
                Input(&searchForString)|border,
            })|border,
            Container::Vertical({
                Renderer([&]{return text("Variants Generation");})|bold|hcenter,
                Renderer([&]{return separator();}),
                Container::Horizontal({
                    Button("Generate Variants", [&]{generateHashVariants();}),
                    Renderer([&]{
                        
                        return gauge(static_cast<float>(generatedValues)/maxVariantsAmount)|border;
                    }),
                    Renderer([&]{
                        return text(std::to_string(generatedValues)+"/"+std::to_string(maxVariantsAmount));
                    })|vcenter
                }),
            })|border,

            Container::Horizontal({
                Button("Start search", [&]{beginHashOperations();})|flex,
                Maybe({
                    Button("Stop search", [&]{
                        std::lock_guard<std::mutex> lock(results_mutex);
                        found=true;
                    })|flex
                }, &isStarted)
            }),

            Container::Vertical({
                Renderer([&]{
                    return text("Processing data")|bold;
                })|hcenter,
                Renderer([&]{
                    return separator();
                }),
                Maybe({
                    Renderer([&]{
                        std::chrono::duration<float> between = std::chrono::high_resolution_clock::now() - searchProcessBegin;
                        if(found){
                            between = searchProcessEnd - searchProcessBegin;
                        }
                        
                        return text("Time from start: " + std::to_string(between.count()));
                    })
                }, &isStarted),
                Renderer([&]{
                    if(processedValues==maxVariantsAmount) valueNotFound=true;
                    return hbox({
                        
                        gauge(static_cast<float>(processedValues)/maxVariantsAmount)|border,
                        text(std::to_string(processedValues)+"/"+std::to_string(maxVariantsAmount))|vcenter,
                        
                    }); 
                }),
            })|border,
            Maybe({
                Renderer([&]{
                    isStarted=false;
                    
                    std::chrono::duration<float> between = searchProcessEnd - searchProcessBegin;
                    showHashFunctonSelectBlock=true;
                    std::lock_guard<std::mutex> lock(results_mutex);
                    std::string computedHashString{};
                    if(0==hashFunctionSelected)
                        computedHashString = computeHashSHA256(std::string(sample[foundIndex]));
                        
                    else
                        computedHashString = computeHashMD5(std::string(sample[foundIndex]));
                    

                    return vbox({
                        text("Results")|hcenter,
                        separator(),
                        hbox({
                            text("Found value: "),
                            text(sample[foundIndex])|bold|color(Color::BlueLight),
                            text(":"),
                            text(computedHashString)|bold|color(Color::GreenLight),
                        }),
                        text("Time taken: " + std::to_string(between.count())),
                    });
                                    
                })|border
            }, &showResult),
            Maybe({
                Renderer([&]{
                    showResult=false;
                    showHashFunctonSelectBlock=true;
                    isStarted=false;
                    searchProcessEnd = std::chrono::high_resolution_clock::now();
                    return text("Value for a hash wasn't found!");
                })|border
            }, &valueNotFound)

        })|ftxui::flex
    });

}





ftxui::Component Application::render(){

    return mainContainer;
}