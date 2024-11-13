#include "ThreadPool.h"
#include "HashFunctions.h"

#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for string, basic_string
#include <vector> // for vector


#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp" // for Element, separator, operator|, vbox, border

#include <ftxui/component/component_options.hpp>

#include <ftxui/dom/table.hpp>
#include <ftxui/screen/terminal.hpp>
#include <cmath>



class Application{
public:
    Application(std::function<void()>exitor);
    ftxui::Component render();


private:
    void beginHashOperations();
    void generateHashVariants();


    bool showHashFunctonSelectBlock=true;

    std::string threadsAmountString{};
    std::string searchForString{};
    std::vector<std::string> sample{};





    ftxui::Component mainContainer;
    size_t threadsAmount{1};

    ThreadPool mainPool{threadsAmount};
    std::mutex results_mutex;
    std::mutex generates_mutex;
    
    std::string alphabet{"qazxswedcvfrtgbnhyujmkiolp"};
    int alphabetMaxVariantsAmout=26;
    int variantLength=5;
    int maxVariantsAmount = std::pow(alphabetMaxVariantsAmout, variantLength);

    std::vector<std::string> hashVariants{};

    std::chrono::time_point<std::chrono::system_clock> searchProcessBegin;
    std::chrono::time_point<std::chrono::system_clock> searchProcessEnd;

    int hashFunctionSelected=0;

    int foundIndex=-1;
    bool found;
    int processedValues=0;
    int generatedValues=0;




    bool isStarted;
    bool valueNotFound;
    bool showResult;
    std::string searchVal;

    std::function<std::string(const std::string&)> selectedHashFunction;

    

    std::function<void()> exitClosure;

};