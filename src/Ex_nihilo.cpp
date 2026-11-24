#include "pch.hpp"
#include "runtime.hpp"
#include <iostream>



int main() {

    std::unique_ptr<Runtime> rt = std::make_unique<Runtime>();

    rt->Run();

    return 0;
}