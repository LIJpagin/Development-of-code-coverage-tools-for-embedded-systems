#include <iostream>
#include <map>
#include <set>
#include <vector>

void foo() {COV_LOG_FUNC(1695);
}

void foo() {COV_LOG_FUNC(1695);
}

void foo() {COV_LOG_FUNC(1695);
}

void foo() {COV_LOG_FUNC(1695);
}

void foo() {COV_LOG_FUNC(1695);
}

void foo() {COV_LOG_FUNC(1695);
    int i = 1;
}

template <typename T>
void foo1() {
}

template <typename T>
void foo2() {
}

template <typename T>
void foo3() {
}

template <typename T>
void foo4() {
}

template <typename T>
void foo5() {
}

template <typename T>
void foo6() {
    int i = 1;
}

class h {
    void foo1();

    void foo() {COV_LOG_FUNC(1695);
    }

    void foo() {COV_LOG_FUNC(1695);
    }

    void foo() {COV_LOG_FUNC(1695);
    }

    void foo() {COV_LOG_FUNC(1695);
    }
};

void h::foo1() {COV_LOG_FUNC(1695);
}

int main() {COV_LOG_FUNC(1695);
    foo1();
    foo2();
    foo3();
    foo4();
    foo5();
    foo6();

    int i = 0;
    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
        i = 1;
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
        i = 1;
        i = 1;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
        i = 1;
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
        i = 2;
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
        i = 1;
        i = 1;
    } else {COV_LOG_ELSE(1695);
        i = 2;
        i = 2;
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
        i = 1;
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
        i = 2;
        i = 2;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    } else if (i == 3) {COV_LOG_IF(1695);
        i = 3;
    } else if (i == 4) {COV_LOG_IF(1695);
        i = 4;
    } else if (i == 5) {COV_LOG_IF(1695);
        i = 5;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    } else if (i == 3) {COV_LOG_IF(1695);
        i = 3;
    } else if (i == 4) {COV_LOG_IF(1695);
        i = 4;
    } else if (i == 5) {COV_LOG_IF(1695);
        i = 5;
    }

    if (i == 1) {COV_LOG_IF(1695);
        i = 1;
    } else if (i == 2) {COV_LOG_IF(1695);
        i = 2;
    } else if (i == 3) {COV_LOG_IF(1695);
        i = 3;
    } else if (i == 4) {COV_LOG_IF(1695);
        i = 4;
    } else if (i == 5) {COV_LOG_IF(1695);
        i = 5;
    }

    while (true)
        ;COV_LOG_WHILE(1695);

    while (true) {COV_LOG_WHILE(1695);
        i++;
    }

    while (true) {COV_LOG_WHILE(1695);
        i++;
    }

    while (true) {COV_LOG_WHILE(1695);
        i++;
    }

    while (true) {COV_LOG_WHILE(1695);
        i++;
    }

    while (true) {COV_LOG_WHILE(1695);
        i++;
    }

    while (true) {COV_LOG_WHILE(1695);
        i++;
        i++;
        i++;
    }

    for (;;)
        ;COV_LOG_FOR(1695);

    for (;;) {COV_LOG_FOR(1695);
        i++;
    }

    for (;;) {COV_LOG_FOR(1695);
        i++;
    }

    for (;;) {COV_LOG_FOR(1695);
        i++;
    }

    for (;;) {COV_LOG_FOR(1695);
        i++;
    }

    for (;;) {COV_LOG_FOR(1695);
        i++;
    }

    for (;;) {COV_LOG_FOR(1695);
        i++;
        i++;
        i++;
    }

    for (int q = 0; q < 10; ++q) {COV_LOG_FOR(1695);
        i++;
    }

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
    } while (true);

    do {COV_LOG_DO(1695);
        i++;
        i++;
        i++;
    } while (true);

    bool c = i == 1 ? 1 : 2;

    if (i == 1 ? 1 : 2) {COV_LOG_IF(1695);
        i++;
    }

    switch (i) {
        case 1:
            COV_LOG_CASE(1695);i++;
        case 2:
            COV_LOG_CASE(1695);i++;
        case 3:
            COV_LOG_CASE(1695);i++;
        case 4:
            COV_LOG_CASE(1695);i++;
    }

    switch (i) {
        case 1:
            COV_LOG_CASE(1695);i++;
            break;
        case 2:
            COV_LOG_CASE(1695);i++;
            break;
        case 3:
            COV_LOG_CASE(1695);i++;
            break;
        case 4:
            COV_LOG_CASE(1695);i++;
            break;
    }

    switch (i) {
        case 1:
            COV_LOG_CASE(1695);i++;
            break;
        case 2:
            COV_LOG_CASE(1695);i++;
            break;
        case 3:
            COV_LOG_CASE(1695);i++;
            break;
        case 4:
            COV_LOG_CASE(1695);i++;
            break;
    }

    switch (i) {
        case 1: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
        } break;
        case 2: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
        } break;
        case 3: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
        } break;
        case 4: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
        } break;
    }

    switch (i) {
        case 1: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
            break;
        }
        case 2: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
            break;
        }
        case 3: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
            break;
        }
        case 4: COV_LOG_CASE(1695);{
            i++;
            i++;
            i++;
            break;
        }
    }

    []() {COV_LOG_LAMBDA(1695);
    };

    []() {COV_LOG_LAMBDA(1695);
    };

    []() {COV_LOG_LAMBDA(1695);
    };

    int w = []() {COV_LOG_LAMBDA(1695);
    };

    int e = []() {COV_LOG_LAMBDA(1695);
    };

    int r = []() {COV_LOG_LAMBDA(1695);
    };

    int t = []() {COV_LOG_LAMBDA(1695);

    };
}