// This file is part of xmasGifts.
//
// xmasGifts is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// xmasGifts is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with xmasGifts.  If not, see <http://www.gnu.org/licenses/>.
//

#pragma once

#include <iostream>
#include <ostream>

// class for configuring console output
class OutputCfg
{
    friend class OutputData;
    friend class OutputDbg;

public:
    static void increaseVerbosity() { ++verbosityLevel; }
    static void decreaseVerbosity()
    {
        if (verbosityLevel > 0) {
            --verbosityLevel;
        }
    }

private:
    static unsigned int verbosityLevel;
};

// class for outputting data to console (results of operations)
class OutputData
{
public:
    template <typename T>
    OutputData& operator<<(const T& x)
    {
        std::cout << x;
        return *this;
    }

    OutputData& operator<<(std::ostream& (*os)(std::ostream&))
    {
        std::cout << os;
        return *this;
    }
};

extern OutputData out;

// class for outputting debug data on console
class OutputDbg
{
public:
    template <typename T>
    OutputDbg& operator<<(const T& x)
    {
        if (OutputCfg::verbosityLevel >= 1) {
            std::cerr << x;
        }

        return *this;
    }

    OutputDbg& operator<<(std::ostream& (*os)(std::ostream&))
    {
        if (OutputCfg::verbosityLevel >= 1) {
            std::cerr << os;
        }
        return *this;
    }
};

extern OutputDbg dbg;
