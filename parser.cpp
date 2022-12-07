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

#include "parser.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "output.h"

namespace
{
// parse a list of (delimited) names
void parseBlockedGiftees(Person &p, std::istringstream &is);

// debug prints the parsed configuration
void debugPrintCfg(const std::vector<Person> &people);

void parseBlockedGiftees(Person &p, std::istringstream &is)
{
    std::string s;
    while (is >> s) {
        std::string tmp;
        for (auto c = s.cbegin(); c != s.cend(); c++) {
            if (*c == ',' || *c == ';') {
                if (tmp.size() > 0) {
                    p.blockedNames.insert(tmp);
                    tmp = "";
                }
            } else {
                tmp += *c;
            }
        }

        if (tmp.size() > 0) {
            p.blockedNames.insert(tmp);
        }
    }
}

void debugPrintCfg(const std::vector<Person> &people)
{
    for (const auto &p : people) {
        dbg << p.name << ":";

        for (const auto &b : p.blockedNames) {
            dbg << " " << b;
        }

        dbg << std::endl;
    }

    dbg << std::endl;
}
}  // namespace

bool operator==(const Person &x, const Person &y) { return x.name == y.name; }

std::vector<Person> parseFile(const std::string &fIn, const bool sendEmails)
{
    std::vector<Person> people;

    std::ifstream inputFile(fIn);

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream entry(line);
        std::string name;

        entry >> name;
        if (!name.empty()) {
            Person p{name, {}, {}};

            if (sendEmails) {
                std::string email;
                entry >> email;
                p.email = email;
            }

            // check if this person doesn't exist yet in the list
            if (find(people.cbegin(), people.cend(), p) == people.cend()) {
                parseBlockedGiftees(p, entry);
                people.emplace_back(p);
            } else {
                std::cerr
                    << name
                    << " appears multiple times (using just the first entry)"
                    << std::endl;
            }
        }
    }

    dbg << people.size() << " people parsed" << std::endl;

    debugPrintCfg(people);

    return people;
}
