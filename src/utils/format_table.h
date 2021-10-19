#pragma once

#include <iomanip>
#include <deque>


class FormatTable {
public:
    FormatTable(const std::string& name) : name_(name) {
    }

    enum class Align { Left, Right };

    struct ColFmt {
        std::string colname;
        int w;
        Align align;
    };

    FormatTable& addCol(const std::string& colname, int w, Align align = Align::Right) {
        cols.emplace_back(ColFmt{ colname, w, align });
        return *this;
    }


    //o << std::left << std::setw(15) << "ccy" << std::right << std::setw(15) << "eq" << std::setw(15) << "avail_eq"
    //    << std::setw(15) << "cash" << std::setw(15) << "upl" << std::endl;
    //o << std::setw(80) << std::setfill('-') << "" << std::setfill(' ') << std::endl;

    template <typename T>
    FormatTable& operator << (const T & v) {

        return *this;
    }

    friend std::ostream& operator << (std::ostream& o, const FormatTable& t) {

        return o;
    }

private:
    const std::string name_;
    std::deque<ColFmt> cols;
};