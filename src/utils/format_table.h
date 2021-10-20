#pragma once

#include "utils.h"
#include <sstream>
#include <iomanip>
#include <deque>


class FormatTable {
public:
    FormatTable(const std::string& name);

    enum class Align { Left, Right };

    struct ColFmt {
        std::string colname;
        int w;
        Align align;
    };

    FormatTable& addCol(const std::string& colname, int w, Align align = Align::Right) {
        cols_.emplace_back(ColFmt{ colname, w, align });
        return *this;
    }

    template <typename T>
    FormatTable& operator << (const T & v) {
        std::ostringstream o;
        o << v;
        data_.push_back(o.str());
        return *this;
    }

    void setFmt(std::ostream& oss, const ColFmt& col) const {
        if (col.w)
            oss << std::setw(col.w);
        if (col.align == FormatTable::Align::Left)
            oss << std::left;
        else
            oss << std::right;
    }

    friend std::ostream& operator << (std::ostream& o, const FormatTable& t) {
        {
            std::ostringstream oss;
            for (auto& col : t.cols_) {
                t.setFmt(oss, col);
                oss << col.colname;
                if (&col != &t.cols_.back())
                    oss << " | ";
            }
            o << oss.str() << std::endl;
            o << std::string(t.max_width_, '-') << std::endl;
        }

        std::ostringstream oss;
        auto itr = t.data_.begin();
        while (itr != t.data_.end()) {
            for (auto& col : t.cols_) {
                t.setFmt(oss, col);
                oss << *itr;
                if (&col != &t.cols_.back())
                    oss << " | ";

                if (++itr == t.data_.end())
                    break;
            }
            oss << std::endl;
        }
        o << oss.str();
        return o;
    }

private:
    const std::string name_;
    std::deque<ColFmt> cols_;
    std::deque<std::string> data_;
    int max_width_ = 79;
};