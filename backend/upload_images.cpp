#include <iostream>
#include <cstdlib>
#include "laserpants/dotenv/dotenv.h"

int main() {
    dotenv::init();

    std::cout << std::getenv("BUCKET_NAME") << std::endl;
}
