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
#include "output.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// parse a list of (delimited) names
static void parseBlockedGiftees(Person &p, istringstream &is);

// debug prints the parsed configuration
static void debugPrintCfg(const vector<Person> &people);

bool operator==(const Person &x, const Person &y)
{
    return x.name==y.name;
}

vector<Person> parseFile(const string &fIn)
{
    vector<Person> people;

    ifstream inputFile(fIn);

    string line;
    while (getline(inputFile, line))
    {
        istringstream entry(line);
        string name;

        entry >> name;
        if (!name.empty())
        {
            Person p{name, {}};
            // check if this person doesn't exist yet in the list
            if (find(people.cbegin(), people.cend(), p)==people.cend())
            {
                parseBlockedGiftees(p, entry);
                people.emplace_back(p);
            }
            else
            {
                cerr << name << " appears multiple times (using just the first entry)" << endl;
            }
        }
    }

    dbg << people.size() << " people parsed" << endl;

    debugPrintCfg(people);

    return people;
}

static void parseBlockedGiftees(Person &p, istringstream &is)
{
    string s;
    while (is >> s)
    {
        string tmp;
        for (auto c = s.cbegin(); c!=s.cend(); c++)
        {
            if (*c==',' || *c==';')
            {
                if (tmp.size() > 0)
                {
                    p.blockedNames.insert(tmp);
                    tmp = "";
                }
            }
            else
            {
                tmp += *c;
            }
        }

        if (tmp.size() > 0)
        {
            p.blockedNames.insert(tmp);
        }
    }
}

static void debugPrintCfg(const vector<Person> &people)
{
    for (const auto &p : people)
    {
        dbg << p.name << ":";

        for (const auto &b : p.blockedNames)
        {
            dbg << " " << b;
        }

        dbg << endl;
    }

    dbg << endl;
}
