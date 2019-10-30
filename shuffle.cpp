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
#include <algorithm>

using namespace std;

// randomizes the entries in the giftList
void shuffleList(std::vector<Person> &giftList, mt19937 &randGen);

// checks if the list is ok, i.e. all donors have a valid giftee
static bool checkList(const vector<Person> &giftList);

// debug prints the list
static void debugList(const vector<Person> &list);

// recursivley tries to swap elements in the people list to find a valid sequence
static bool addGiftees(vector<Person> &giftList, std::vector<Person>::iterator itDonor);

void shuffleList(std::vector<Person> &giftList, mt19937 &randGen)
{
    // swap two random elements in the list
    uniform_int_distribution<unsigned int> idist(0, giftList.size()-1);
    unsigned int ix1 = idist(randGen);
    unsigned int ix2 = idist(randGen);
    swap(giftList[ix1], giftList[ix2]);
}

bool findValidListRand(std::vector<Person> &giftList)
{
    // this is the most stupid way to find a valid list. Whenever we
    // detect that the current list is not ok, swap two randomly chosen
    // entries in the list. In this way we're somewhat guaranteed that
    // after inifinite runtime we find the solution. However, for
    // configurations which have many constraints, this might take
    // aaaaaages. Therefore it's considered to be the most stupid
    // implementation.

    // code below copied from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    debugList(giftList);

    while (!checkList(giftList))
    {
        shuffleList(giftList, gen);
        debugList(giftList);
    }

    dbg << endl;

    // we're not reaching this point if the list is not valid
    return true;
}

bool findValidListRecursive(std::vector<Person> &giftList)
{
    // This implementation is more smart than shuffle1(). In here we're trying
    // to recursively construct a valid list. So in the end we're scanning
    // through all combinations. This can be time-consuming, but it's guaranteed
    // to find the solution or fail.

    bool success = false;

    // randomize the entries in the list first, to allow some randomization...
    // code below copied from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    for (decltype(giftList.size()) i=0; i<giftList.size(); ++i)
    {
        shuffleList(giftList, gen);
    }

    if (addGiftees(giftList, giftList.begin()))
    {
        success = true;
        debugList(giftList);
        dbg << endl;
    }
    else
    {
        cout << "No circular donor/giftee assignment possible" << endl;
    }

    return success;
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

static bool addGiftees(vector<Person> &giftList, std::vector<Person>::iterator itDonor)
{
    bool success = false;

    // giftee is pointing one ahead of donor
    auto itGiftee = itDonor + 1;
    if (itGiftee==giftList.end())
    {
        // we're at the end of the list and therefore need to do the wrap-around, i.e.
        // the first person in the list has to be a valid giftee for the last person
        // in the list
        itGiftee = giftList.begin();
        if (itDonor->second.find(itGiftee->first) == itDonor->second.end())
        {
            // last element is valid, we've got a valid sequence!
            success = true;
        }
    }
    else
    {
        // step through the remaining part of the list and (recursively) test
        // giftees. We do so by swapping elements. However, we need to be careful
        // to maintain a list of giftees to be tested. Therefore we need to undo
        // the swapping if it wasn't successful
        for (auto itNewGiftee=itGiftee; itNewGiftee!=giftList.end(); ++itNewGiftee)
        {
            if (itDonor->second.find(itNewGiftee->first) == itDonor->second.end())
            {
                // itGiftee might point to the same element as itNewGiftee does.
                // We don't need to worry about that, the library swap()-function
                // takes care of it
                swap(*itGiftee, *itNewGiftee);

                if (addGiftees(giftList, itGiftee))
                {
                    success = true;
                    break;
                }
                else
                {
                    // undo the swapping (recover the original state of
                    // the list)
                    swap(*itGiftee, *itNewGiftee);
                }
            }
        }
    }

    return success;
}

static void debugList(const vector<Person> &list)
{
    for (const auto &p : list)
    {
        dbg << p.first << " -> ";
    }
    dbg << list.cbegin()->first << endl;
}

map<unsigned int, string> randomizePersonNumbers(vector<Person> &people)
{
    map<unsigned int, string> persNum;

    // code below copied from https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
    random_device rd;  // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_int_distribution<unsigned int> idist(0, people.size()-1);

    vector<bool> ids(people.size(), false);

    for (const auto &p : people)
    {
        // find a not yet used id for this person
        unsigned int id = idist(gen);
        while (ids[id])
        {
            if (++id == ids.size())
            {
                id = 0;
            }
        }
        ids[id] = true;
        persNum.emplace(make_pair(id, p.first));
    }

    return persNum;
}
