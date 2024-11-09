#include <cassert>
#include <iomanip>
#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <future>
#include <functional>
#include <atomic>
#include <string>
#include <sstream>
#include <openssl/sha.h> // Для хэширования, если используешь OpenSSL
#include <openssl/md5.h>

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/flexbox_config.hpp>
#include <ftxui/dom/node.hpp>
#include "ftxui/screen/color.hpp"
#include <ftxui/screen/terminal.hpp>

#include <ftxui/screen/terminal.hpp>
#include <memory> // for allocator, __shared_ptr_access, shared_ptr
#include <string> // for string, basic_string
#include <vector> // for vector

#include "ftxui/component/captured_mouse.hpp" // for ftxui
#include "ftxui/component/component.hpp" // for Radiobox, Renderer, Tab, Toggle, Vertical
#include "ftxui/component/component_base.hpp"     // for ComponentBase
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive
#include "ftxui/dom/elements.hpp" // for Element, separator, operator|, vbox, border

#include <ftxui/component/component_options.hpp>

#include <ftxui/dom/table.hpp>
#include <ftxui/screen/terminal.hpp>
#include <cmath>
class ThreadPool {
public:
    ThreadPool(size_t threads);
    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::result_of<F()>::type>;
    ~ThreadPool();
    bool isEmpty();
    std::atomic<int> getUsedTasks();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }
}

bool ThreadPool::isEmpty(){
    return tasks.empty();
}

std::atomic<int> ThreadPool::getUsedTasks(){
    return std::atomic<int>(tasks.size());
}

template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<typename std::result_of<F()>::type> {
    using return_type = typename std::result_of<F()>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

// Пример использования
std::string computeHash(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    


    std::stringstream ss;
    for(int i =0; i< SHA256_DIGEST_LENGTH; i++){
        ss<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)hash[i];
    }
    
    return ss.str();
}



#include <ftxui/component/component.hpp> // Для использования Button и Text
#include <ftxui/component/component_options.hpp>
#include <iostream>
#include <vector>
#include <cmath> // Для математических вычислений
#include <mutex> // Для защиты доступа к строке результатов
#include <thread> // Для использования std::this_thread::sleep_for
#include <chrono> // Для использования std::chrono::seconds
using namespace ftxui;


std::string computeHashSHA256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
    


    std::stringstream ss;
    for(int i =0; i< SHA256_DIGEST_LENGTH; i++){
        ss<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)hash[i];
    }
    
    return ss.str();
}

std::string computeHashMD5(const std::string& input) {
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);


    std::stringstream ss;
    for(int i =0; i< MD5_DIGEST_LENGTH; i++){
        ss<<std::setfill('0')<<std::setw(2)<<std::hex<<(int)hash[i];
    }
    
    return ss.str();
}



int main(){
    ThreadPool pool(10); 

    std::vector<std::string> data{};
    std::string alphabet = "qazxswedcvfrtgbnhyujmkiolp";
    for(auto a: alphabet){
        for(auto b: alphabet){
            for(auto c: alphabet){
                for(auto d: alphabet){
                    for(auto e: alphabet){
                        data.push_back(std::string{a}+std::string{b}+std::string{c}+std::string{d}+std::string{e});
                    }
                }
            }
        }
    }

    std::vector<std::string> hashFunctionSelect{"SHA256", "MD5"};
    int hashFunctionSelected{0};

    std::string results;
    std::mutex results_mutex;

    std::string searchingValue = "1115dd800feaacefdf481f1f9070374a2a81e27880f187396db67958b207cbad";
    std::string searchVal{};


    bool found = false;
    int found_index=-1;
    std::atomic<int> counted_values=0;
    bool showModal=false;
    bool showRadio=true;

    


    auto button = ftxui::Button("Start search", [&]() {
        results.clear(); // Очищаем предыдущие результаты
        results += "Task started";
        found=false;
        showModal=false;
        showRadio=false;
        searchVal = searchingValue;
        searchingValue="";
        counted_values=0;
        found_index=-1;
        

        
        if(hashFunctionSelected==0){
            pool.enqueue([&]() {
                std::string value{};
                
                for(int i = 0; i< data.size(); i++){
                    if (found) {
                        break; // Если найден, выходим из цикла
                    }
                    value=data[i];
                    pool.enqueue([&, value, i]() {
                        if (found) {
                            return; // Если найден, выходим из цикла
                        }
                        std::string val = computeHashSHA256(value); // Симуляция долгого вычисления
    
                        if (val == searchVal) {
                            
                            
                                std::lock_guard<std::mutex> lock(results_mutex);
                                found_index = i;
                                found=true;
                                showModal=true;
                                counted_values++;
                                return;
                            
                        }
                        std::lock_guard<std::mutex> lock(results_mutex);
                        counted_values++;


                    });
                }
            });
        }
        else{
            pool.enqueue([&]() {
                std::string value{};
                
                for(int i = 0; i< data.size(); i++){
                    if (found) {
                        break; // Если найден, выходим из цикла
                    }
                    value=data[i];
                    pool.enqueue([&, value, i]() {
                        if (found) {
                            return; // Если найден, выходим из цикла
                        }
                        std::string val = computeHashMD5(value); // Симуляция долгого вычисления
    
                        if (val == searchVal) {
                            
                            
                                std::lock_guard<std::mutex> lock(results_mutex);
                                found_index = i;
                                found=true;
                                showModal=true;
                                counted_values++;
                                return;
                            
                        }
                        std::lock_guard<std::mutex> lock(results_mutex);
                        counted_values++;


                    });
                }
            });
        }
        
    });




    auto screen = ftxui::ScreenInteractive::Fullscreen();

    auto container = Container::Vertical({
        Renderer([&]{
            return vbox({
                text("Система поиска значения по хэшу"),
                text("Alphabet: abcdefghijklmnopqrstyvwxyz"),
                
            });
        }),
        Maybe({
            Container::Horizontal({
                Renderer([&]{return text("Select used hash function: ");}),
                Radiobox(&hashFunctionSelect, &hashFunctionSelected)
                
            }),   
        }, &showRadio), 
        
        Container::Horizontal({
            Renderer([&]{
                return text("Enter the hash value into input");
            })|vcenter,
            Input(&searchingValue)|border,
        }),
        
        button,
        Renderer([&]{
            return paragraph(results);
        }),
        Renderer([&]{
            return hbox({
                text("I've counted " + std::to_string(counted_values.load())),
                gauge(static_cast<float>(counted_values.load())/data.size())|border
            }); 
        }),
        Maybe({
            Renderer([&]{
                showRadio=true;
                std::lock_guard<std::mutex> lock(results_mutex);
                if(0==hashFunctionSelected)
                    return text("Found value: " + data[found_index] + ":" + computeHashSHA256(std::string(data[found_index])));
                else
                    return text("Found value: " + data[found_index] + ":" + computeHashMD5(std::string(data[found_index])));
                                
            })
        }, &showModal),
    });

    screen.Loop(container);
}
