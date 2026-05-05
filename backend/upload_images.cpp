#include <iostream>
#include <cstdlib>
#include "laserpants/dotenv/dotenv.h"

int main() {
    dotenv::init();

    const char* bucket = std::getenv("BUCKET_NAME");
    if (bucket == nullptr) {
        std::cerr << "Error: BUCKET_NAME environment variable not set." << std::endl;
        return 1;
    }
}
