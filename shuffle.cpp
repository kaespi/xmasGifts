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

#include "shuffle.h"
#include "output.h"

#include <iostream>
#include <random>

using namespace std;

// checks if the list is ok, i.e. all donors have a valid giftee
static bool checkList(const vector<Person> &giftList);

// debug prints the list
static void debugList(const vector<Person> &list);

std::vector<Person> shuffle1(std::vector<Person> &giftList)
{
    // code below copied from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    debugList(giftList);

    while (!checkList(giftList))
    {
        // swap two random elements in the list
        uniform_int_distribution<unsigned int> idist(0, giftList.size()-1);
        unsigned int ix1 = idist(gen);
        unsigned int ix2 = idist(gen);
        swap(giftList[ix1], giftList[ix2]);

        debugList(giftList);
    }

    dbg << endl;

    return giftList;
}

static bool checkList(const vector<Person> &giftList)
{
    bool listOk = true;

    if (giftList.size() > 1)
    {
        // giftee iterator points one ahead
        auto itGiftee = ++(giftList.cbegin());

        for (auto itDonor=giftList.cbegin(); listOk && itDonor!=giftList.cend(); ++itDonor)
        {
            // wrap around the giftee (which always points one ahead)
            if (itGiftee==giftList.cend())
            {
                itGiftee = giftList.cbegin();
            }

            listOk &= (itDonor->second.find(itGiftee->first) == itDonor->second.end());

            ++itGiftee;
        }
    }

    return listOk;
}

static void debugList(const vector<Person> &list)
{
    for (const auto &p : list)
    {
        dbg << p.first << " -> ";
    }
    dbg << list.cbegin()->first << endl;
}
