#include "Student.h"
#include <cassert>
#include <iostream>

int main() {
    assert(normalizeRollNumber("TNT - 2310") == "TNT - 2310");
    assert(normalizeRollNumber("tnt-2310") == "TNT - 2310");
    assert(determineBatch(1600) == 9);
    assert(determineBatch(1799) == 9);
    assert(determineBatch(1800) == 10);
    assert(determineBatch(1999) == 10);
    assert(determineBatch(2000) == 11);
    assert(determineBatch(2199) == 11);
    assert(determineBatch(2200) == 12);
    assert(determineBatch(2399) == 12);
    assert(determineBatch(2400) == 13);
    assert(determineBatch(2600) == 13);
    assert(determineBatch(1599) == 0);
    assert(determineBatch(2601) == 0);
    assert(determineYear(13) == 1);
    assert(determineYear(9) == 5);
    std::cout << "All roll-number tests passed.\n";
}

