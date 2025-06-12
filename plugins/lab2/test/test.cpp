#include <iostream>

extern "C" void test_func() {
    for (int i = 0; i < 2; ++i) {
        std::cout << "Hello from test func: " << i << "\n";
    }
}

int main() {
    for (int i = 0; i < 2; ++i) {
        std::cout << "Hello from for loop: " << i << "\n";
        for (int j = 0; j < 2; ++j) {
            std::cout << "Hello from nested for loop: " << j << "\n";
        }
    }

    int i = 2;
    while (i > 0) {
        std::cout << "Hello from while loop: " << i << "\n";
        --i;
    }

    test_func();

    return 0;
}

/*

plugin output:

[INFO] LOOP START
Hello from for loop: 0
[INFO] LOOP START
Hello from nested for loop: 0
Hello from nested for loop: 1
[INFO] LOOP END
Hello from for loop: 1
[INFO] LOOP START
Hello from nested for loop: 0
Hello from nested for loop: 1
[INFO] LOOP END
[INFO] LOOP END
[INFO] LOOP START
Hello from while loop: 2
Hello from while loop: 1
[INFO] LOOP END
[INFO] LOOP START
Hello from test func: 0
Hello from test func: 1
[INFO] LOOP END

*/
