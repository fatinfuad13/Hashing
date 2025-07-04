#pragma once
#include <iostream>
#include <fstream>

class DualStream
{
public:
    std::ostream& console;
    std::ofstream& file;

    DualStream(std::ostream& c, std::ofstream& f) : console(c), file(f) {}

    template <typename T>
    DualStream& operator<<(const T& data) /// ensures that data is written both in console and file
    {
        console << data;
        file << data;
        return *this;
    }

    DualStream& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        console << manip;
        file << manip;
        return *this;
    }

    // console colors
    void colorRed()
    {
        console << "\033[1;31m";
    }
    void colorWhite()
    {
        console << "\033[1;37m";
    }
    void resetColor()
    {
        console << "\033[0m";
    }
};

