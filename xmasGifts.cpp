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
#include <fstream>
#include <algorithm>

#include "parser.h"
#include "person.h"
#include "shuffle.h"
#include "output.h"

using namespace std;

// prints a little help text to the command line
static void printHelp();

// parse the command line and return the file to be parsed
static std::tuple<string, bool> parseCmdLine(int argc, char **argv);

// prints the final resulting list of donors/giftees
static void printFoundList(const vector<Person> &giftList);

// writes the found gift list into the two output files
static void genFiles(vector<Person> &giftList, const string &inFilename);

// generates the output filenames for the envelopes and cards
static pair<string, string> getOutFilenames(const string &inFilename);

// writes the file with the cards
static void writeCards(const map<unsigned int, string> &personIds, const string &filename);

// writes the file with the cards
static void writeEnvelopes(const map<unsigned int, string> &personIds,
                           const vector<Person> &giftList, const string &filename);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printHelp();
    }
    else
    {
        const std::tuple<string, bool> cfg = parseCmdLine(argc, argv);

        auto p = parseFile(get<0>(cfg));

        bool listConstructionSuccess = ( get<1>(cfg) ? findValidListRand(p) : findValidListRecursive(p) );
        if (listConstructionSuccess)
        {
            printFoundList(p);
            genFiles(p, get<0>(cfg));
        }
    }

	return EXIT_SUCCESS;
}

static void printHelp()
{
    cout << "Usage: xmasGifts [-v] [-r] [<configuration file>]" << endl << endl <<
            "    -v increases verbosity level" << endl <<
            "    -r use purely random search for gift list (by default: systematic, recursive search)" << endl <<
            "    <configuration file>: file containing the participants and their" << endl <<
            "                          past giftees/blocked giftees" << endl << endl <<
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

static std::tuple<string, bool> parseCmdLine(int argc, char **argv)
{
    string filename;
    bool randomAlgo = false;

    for (int n=1; n<argc; ++n)
    {
        if (string("-v")==argv[n])
        {
            OutputCfg::increaseVerbosity();
        }
        else if (string("-r")==argv[n])
        {
            randomAlgo = true;
        }
        else
        {
            filename = argv[n];
        }
    }

    return make_tuple(filename, randomAlgo);
}

static void printFoundList(const vector<Person> &giftList)
{
    for (const auto &pList : giftList)
    {
        dbg << pList.name << " -> ";
    }
    dbg << giftList.cbegin()->name << endl;
}

static void genFiles(vector<Person> &giftList, const string &inFilename)
{
    // now we'll have to produce envelopes and cards. We write two files
    // where we have a mapping number <-> person. Two people might read
    // the two files such that no one knows the actual found donor/giftee
    // assignments
    auto nums = randomizePersonNumbers(giftList);

    auto fn = getOutFilenames(inFilename);

    writeCards(nums, fn.first);
    writeEnvelopes(nums, giftList, fn.second);

    cout << "Info for cards written into " << fn.first << endl;
    cout << "Info for envelopes written into " << fn.second << endl;
}

static pair<string, string> getOutFilenames(const string &inFilename)
{
    bool dotFound = false;
    auto itIn=inFilename.rbegin();
    for (; itIn!=inFilename.rend(); ++itIn)
    {
        if (*itIn=='.')
        {
            dotFound = true;
            break;
        }
    }

    string outFilenameBase;
    if (dotFound)
    {
        outFilenameBase = string(inFilename, 0, inFilename.rend() - itIn - 1);
    }
    else
    {
        outFilenameBase = inFilename;
    }

    return make_pair(outFilenameBase+"_cards.txt", outFilenameBase+"_envelopes.txt");
}

static void writeCards(const map<unsigned int, string> &personIds, const string &filename)
{
    ofstream outputFile(filename);

    for (auto &pn : personIds)
    {
        outputFile << pn.first << " - " << pn.second << endl;
    }
}

static void writeEnvelopes(const map<unsigned int, string> &personIds,
                           const vector<Person> &giftList, const string &filename)
{
    ofstream outputFile(filename);

    // giftee iterator points one ahead
    auto itGiftee = ++(giftList.begin());

    for (auto itDonor=giftList.begin(); itDonor!=giftList.end(); ++itDonor)
    {
        // wrap around the giftee (which always points one ahead)
        if (itGiftee==giftList.end())
        {
            itGiftee = giftList.begin();
        }

        const string &donor = itDonor->name;
        const string &giftee = itGiftee->name;

        // extract the ID's for the two people (donor and giftee)
        auto donorId = find_if(personIds.begin(), personIds.end(),
                [&donor](pair<unsigned int, string> const &v) { return v.second==donor; });
        auto gifteeId = find_if(personIds.begin(), personIds.end(),
                [&giftee](pair<unsigned int, string> const &v) { return v.second==giftee; });

        if (donorId==personIds.end() || gifteeId==personIds.end())
        {
            cerr << "Could not find numbers for " << donor << " and " << giftee << endl;
        }
        else
        {
            outputFile << "Card " << gifteeId->first << " into envelope " << donorId->first << endl;
        }

        ++itGiftee;
    }
}
