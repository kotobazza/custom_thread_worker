cmake_minimum_required(VERSION 3.30)  # FetchContent_MakeAvailable is available since 3.14

project(my_app LANGUAGES CXX)

# Create application
add_executable(my_app)
target_sources(my_app PRIVATE src/main1.cpp)

# # Download oneTBB during configuration phase
# include(FetchContent)
# FetchContent_Declare(
#   onetbb
#   GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
# )

# # Customize oneTBB configuration
# option(TBB_TEST OFF)

# # Make oneTBB source tree available in this project (like add_subdirectory)
# FetchContent_MakeAvailable(onetbb)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
  GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)


find_package(OpenSSL REQUIRED)

# Connect oneTBB core target to our application (oneTBB will be built as dependency)
target_link_libraries(my_app 
    PRIVATE OpenSSL::SSL  
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component 
)

target_compile_options(my_app PRIVATE -Wno-deprecated-declarations)

