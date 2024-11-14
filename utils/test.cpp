#include "logg.h"
#include "time.h"
#include <cstring>

int main() {
    Timer timer;
    timer.start();
    OUTPUT_TO_FILE;
    AINFO("This is an info message");
    AWARN("This is a warning message");
    timer.stop();
    std::cout << "Time elapsed: " << timer.elapsed_str() <<std::endl;
    return 0;
}

