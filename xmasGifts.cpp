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

struct Config {
    std::string filename{};
    bool randomAlgo{false};
    bool sendEmails{false};
};

// prints a little help text to the command line
static void printHelp();

// parse the command line and return the file to be parsed
static Config parseCmdLine(int argc, char **argv);

// prints the final resulting list of donors/giftees
static void printFoundList(const std::vector<Person> &giftList);

// writes the found gift list into the two output files
static void genFiles(std::vector<Person> &giftList, const std::string &inFilename);

// generates the output filenames for the envelopes and cards
static std::pair<std::string, std::string> getOutFilenames(const std::string &inFilename);

// writes the file with the cards
static void writeCards(const std::map<unsigned int, std::string> &personIds, const std::string &filename);

// writes the file with the cards
static void writeEnvelopes(const std::map<unsigned int, std::string> &personIds,
                           const std::vector<Person> &giftList, const std::string &filename);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printHelp();
    }
    else
    {
        auto const cfg = parseCmdLine(argc, argv);

        auto p = parseFile(cfg.filename, cfg.sendEmails);

        bool listConstructionSuccess = ( cfg.randomAlgo ? findValidListRand(p) : findValidListRecursive(p) );
        if (listConstructionSuccess)
        {
            printFoundList(p);
            genFiles(p, cfg.filename);
        }
    }

	return EXIT_SUCCESS;
}

static void printHelp()
{
    std::cout << "Usage: xmasGifts [-v] [-r] [-e] [<configuration file>]" << std::endl << std::endl <<
                 "    -v increases verbosity level" << std::endl <<
                 "    -r use purely random search for gift list (by default: systematic, recursive search)" << std::endl <<
                 "    -e parse email addresses (2nd column in the input file)" << std::endl <<
                 "    <configuration file>: file containing the participants and their" << std::endl <<
                 "                          past giftees/blocked giftees" << std::endl << std::endl <<
                 "The configuration file should list on each line first the participant's name" << std::endl <<
                 "followed by a list of names which he shouldn't get assigned. E.g." << std::endl <<
                 " Alice Bob" << std::endl <<
                 " Bob   Peter,Tom" << std::endl <<
                 " Tom   Alice" << std::endl <<
                 " Peter Bob" << std::endl << std::endl <<
                 "The software then tries to find a circular list including all participants" << std::endl <<
                 "having assigned another participant as giftee, such as for the above e.g." << std::endl << std::endl <<
                 " Tom -> Bob -> Alice -> Peter -> Tom" << std::endl << std::endl;
}

static Config parseCmdLine(int argc, char **argv)
{
    Config cfg;

    for (int n=1; n<argc; ++n)
    {
        if (std::string("-v")==argv[n])
        {
            OutputCfg::increaseVerbosity();
        }
        else if (std::string("-r")==argv[n])
        {
            cfg.randomAlgo = true;
        }
        else if (std::string("-e")==argv[n])
        {
            cfg.sendEmails = true;
        }
        else
        {
            cfg.filename = argv[n];
        }
    }

    return cfg;
}

static void printFoundList(const std::vector<Person> &giftList)
{
    for (const auto &pList : giftList)
    {
        dbg << pList.name << " -> ";
    }
    dbg << giftList.cbegin()->name << std::endl;
}

static void genFiles(std::vector<Person> &giftList, const std::string &inFilename)
{
    // now we'll have to produce envelopes and cards. We write two files
    // where we have a mapping number <-> person. Two people might read
    // the two files such that no one knows the actual found donor/giftee
    // assignments
    auto nums = randomizePersonNumbers(giftList);

    auto fn = getOutFilenames(inFilename);

    writeCards(nums, fn.first);
    writeEnvelopes(nums, giftList, fn.second);

    std::cout << "Info for cards written into " << fn.first << std::endl;
    std::cout << "Info for envelopes written into " << fn.second << std::endl;
}


static std::pair<std::string, std::string> getOutFilenames(const std::string &inFilename)
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

    std::string outFilenameBase;
    if (dotFound)
    {
        outFilenameBase = std::string(inFilename, 0, inFilename.rend() - itIn - 1);
    }
    else
    {
        outFilenameBase = inFilename;
    }

    return make_pair(outFilenameBase+"_cards.txt", outFilenameBase+"_envelopes.txt");
}

static void writeCards(const std::map<unsigned int, std::string> &personIds, const std::string &filename)
{
    std::ofstream outputFile(filename);

    for (auto &pn : personIds)
    {
        outputFile << pn.first << " - " << pn.second << std::endl;
    }
}

static void writeEnvelopes(const std::map<unsigned int, std::string> &personIds,
                           const std::vector<Person> &giftList, const std::string &filename)
{
    std::ofstream outputFile(filename);

    // giftee iterator points one ahead
    auto itGiftee = ++(giftList.begin());

    for (auto itDonor=giftList.begin(); itDonor!=giftList.end(); ++itDonor)
    {
        // wrap around the giftee (which always points one ahead)
        if (itGiftee==giftList.end())
        {
            itGiftee = giftList.begin();
        }

        const std::string &donor = itDonor->name;
        const std::string &giftee = itGiftee->name;

        // extract the ID's for the two people (donor and giftee)
        auto donorId = find_if(personIds.begin(), personIds.end(),
                [&donor](std::pair<unsigned int, std::string> const &v) { return v.second==donor; });
        auto gifteeId = find_if(personIds.begin(), personIds.end(),
                [&giftee](std::pair<unsigned int, std::string> const &v) { return v.second==giftee; });

        if (donorId==personIds.end() || gifteeId==personIds.end())
        {
            std::cerr << "Could not find numbers for " << donor << " and " << giftee << std::endl;
        }
        else
        {
            outputFile << "Card " << gifteeId->first << " into envelope " << donorId->first << std::endl;
        }

        ++itGiftee;
    }
}
