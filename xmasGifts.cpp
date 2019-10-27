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

int main(int argc, char **argv)
{
    const string filename = (argc >= 2 ? argv[1] : "cfg.txt");
    auto p = parseFile(filename);

    vector<Person> g = shuffle1(p);

    for (const auto &pList : g)
    {
        cout << pList.first << " -> ";
    }
    cout << g.cbegin()->first << endl;

	return EXIT_SUCCESS;
}
