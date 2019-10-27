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

#include <cstdlib>
#include <iostream>

#include "parser.h"
#include "person.h"
#include "shuffle.h"

using namespace std;

// prints a little help text to the command line
static void printHelp();

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printHelp();
    }
    else
    {
        const string filename = argv[1];
        auto p = parseFile(filename);

        vector<Person> g = shuffle1(p);

        for (const auto &pList : g)
        {
            cout << pList.first << " -> ";
        }
        cout << g.cbegin()->first << endl;
    }

	return EXIT_SUCCESS;
}

static void printHelp()
{
    cout << "Usage: xmasGifts [<configuration file>]" << endl << endl <<
            "    configuration file: file containing the participants and their" << endl <<
            "                        past giftees/blocked giftees" << endl << endl <<
            "The configuration file should list on each line first the participant's name" << endl <<
            "followed by a list of names which he shouldn't get assigned. E.g." << endl <<
            " Alice Bob" << endl <<
            " Bob   Peter,Tom" << endl <<
            " Tom   Alice" << endl <<
            " Peter Bob" << endl << endl <<
            "The software then tries to find a circular list including all participants" << endl <<
            "having assigned another participant as giftee, such as for the above e.g." << endl << endl <<
            " Tom -> Bob -> Alice -> Peter -> Tom" << endl << endl;
}
