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

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>

#include "config.h"
#include "email.h"
#include "output.h"
#include "parser.h"
#include "person.h"
#include "shuffle.h"

namespace
{
struct Config {
    std::string filename{};
    bool randomAlgo{false};
    bool sendEmails{false};
};

// prints a little help text to the command line
void printHelp();

// parse the command line and return the file to be parsed
void parseCmdLine(int argc, char **argv, config::Config &cfg);

// prints the final resulting list of donors/giftees
void printFoundList(const std::vector<Person> &giftList);

// writes the found gift list into the two output files
void genFiles(std::vector<Person> &giftList, const std::string &inFilename);

// generates the output filenames for the envelopes and cards
std::pair<std::string, std::string> getOutFilenames(
    const std::string &inFilename);

// writes the file with the cards
void writeCards(const std::map<unsigned int, std::string> &personIds,
                const std::string &filename);

// writes the file with the cards
void writeEnvelopes(const std::map<unsigned int, std::string> &personIds,
                    const std::vector<Person> &giftList,
                    const std::string &filename);

void printHelp()
{
#ifdef WITH_EMAIL
    std::cout << R"(
Usage: xmasGifts [-v] [-r] [-e] [-u <username>] [-p <pwd>] [-f <sender>]
                 [-s <smtpserver>] <configuration file>)";
#else   // WITH_EMAIL
    std::cout << R"(
Usage: xmasGifts [-v] [-r] [-e] <configuration file>)";
#endif  // WITH_EMAIL
    std::cout << R"(

    -v increases verbosity level
    -r use purely random search for gift list (by default: systematic, recursive search))";
#ifdef WITH_EMAIL
    std::cout << R"(
    -e parse and send email addresses (2nd column in the input file)
    -u <username> the username for the STMP server
    -p <pwd> the password for the STMP server
    -s <smtpserver> the STMP server address
    -f <sender> the sender email address)";
#else   // WITH_EMAIL
    std::cout << R"(
    -e parse email addresses (2nd column in the input file))";
#endif  // WITH_EMAIL
    std::cout << R"(
    <configuration file>: file containing the participants and their
                          past giftees/blocked giftees

The configuration file should list on each line first the participant's name
followed by a list of names which he shouldn't get assigned. E.g.
 Alice Bob
 Bob   Peter,Tom
 Tom   Alice
 Peter Bob

The software then tries to find a circular list including all participants
having assigned another participant as giftee, such as for the above e.g.

 Tom -> Bob -> Alice -> Peter -> Tom

With the email option set (-e) the second column in the input file is
the person in the first column's email address, e.g.

 Alice alice@aol.com  Bob
 Bob   bob@bell.com   Peter,Tom
 Tom   tom@ti.com     Alice
 Peter peter@pepsi.co Bob
)";

#ifdef WITH_EMAIL
    std::cout << R"(
In addition to the cards and assignments in the output files, each participant
receives an email disclosing who is is giftee. I.e. in case the circular list
 Tom -> Bob -> Alice -> Peter -> Tom
Then an email to tom@ti.com is sent stating that "Hi Tom, ... your giftee is Bob",
and so an email to bob@bell.com, etc.. Currently the text of the email is hardcoded
in the email.cpp file. So in case you need customization: just edit and recompile.

")";
#endif  // WITH_EMAIL
}

void parseCmdLine(int argc, char **argv, config::Config &cfg)
{
    for (int n = 1; n < argc; ++n) {
        if (std::string("-v") == argv[n]) {
            OutputCfg::increaseVerbosity();
        } else if (std::string("-r") == argv[n]) {
            cfg.setConfigValue("useRandomAlgo", true);
        } else if (std::string("-e") == argv[n]) {
            cfg.setConfigValue("useEmails", true);
        } else if (std::string("-u") == argv[n]) {
            ++n;
            cfg.setConfigValue("emailUsername", std::string{argv[n]});
        } else if (std::string("-s") == argv[n]) {
            ++n;
            cfg.setConfigValue("smtpServer", std::string{argv[n]});
        } else if (std::string("-f") == argv[n]) {
            ++n;
            cfg.setConfigValue("emailSender", std::string{argv[n]});
        } else if (std::string("-p") == argv[n]) {
            ++n;
            cfg.setConfigValue("emailPwd", std::string{argv[n]});
        } else {
            cfg.setConfigValue("inputFilename", std::string{argv[n]});
        }
    }
}

void printFoundList(const std::vector<Person> &giftList)
{
    for (const auto &pList : giftList) {
        dbg << pList.name << " -> ";
    }
    dbg << giftList.cbegin()->name << std::endl;
}

void genFiles(std::vector<Person> &giftList, const std::string &inFilename)
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

std::pair<std::string, std::string> getOutFilenames(
    const std::string &inFilename)
{
    bool dotFound = false;
    auto itIn = inFilename.rbegin();
    for (; itIn != inFilename.rend(); ++itIn) {
        if (*itIn == '.') {
            dotFound = true;
            break;
        }
    }

    std::string outFilenameBase;
    if (dotFound) {
        outFilenameBase =
            std::string(inFilename, 0, inFilename.rend() - itIn - 1);
    } else {
        outFilenameBase = inFilename;
    }

    return make_pair(outFilenameBase + "_cards.txt",
                     outFilenameBase + "_envelopes.txt");
}

void writeCards(const std::map<unsigned int, std::string> &personIds,
                const std::string &filename)
{
    std::ofstream outputFile(filename);

    for (auto &pn : personIds) {
        outputFile << pn.first << " - " << pn.second << std::endl;
    }
}

void writeEnvelopes(const std::map<unsigned int, std::string> &personIds,
                    const std::vector<Person> &giftList,
                    const std::string &filename)
{
    std::ofstream outputFile(filename);

    // giftee iterator points one ahead
    auto itGiftee = ++(giftList.begin());

    for (auto itDonor = giftList.begin(); itDonor != giftList.end();
         ++itDonor) {
        // wrap around the giftee (which always points one ahead)
        if (itGiftee == giftList.end()) {
            itGiftee = giftList.begin();
        }

        const std::string &donor = itDonor->name;
        const std::string &giftee = itGiftee->name;

        // extract the ID's for the two people (donor and giftee)
        auto donorId =
            find_if(personIds.begin(), personIds.end(),
                    [&donor](std::pair<unsigned int, std::string> const &v) {
                        return v.second == donor;
                    });
        auto gifteeId =
            find_if(personIds.begin(), personIds.end(),
                    [&giftee](std::pair<unsigned int, std::string> const &v) {
                        return v.second == giftee;
                    });

        if (donorId == personIds.end() || gifteeId == personIds.end()) {
            std::cerr << "Could not find numbers for " << donor << " and "
                      << giftee << std::endl;
        } else {
            outputFile << "Card " << gifteeId->first << " into envelope "
                       << donorId->first << std::endl;
        }

        ++itGiftee;
    }
}
}  // namespace

int main(int argc, char **argv)
{
    if (argc < 2) {
        printHelp();
    } else {
        config::Config cfg;
        parseCmdLine(argc, argv, cfg);

        dbg << "parsed cmdline" << std::endl;

        auto p = parseFile(cfg.getInputFilename(), cfg.useEmails());

        bool listConstructionSuccess =
            (cfg.useRandomAlgo() ? findValidListRand(p)
                                 : findValidListRecursive(p));
        if (listConstructionSuccess) {
            printFoundList(p);
            genFiles(p, cfg.getInputFilename());
#ifdef WITH_EMAIL
            email::sendEmails(p, cfg);
#endif
        }
    }

    return EXIT_SUCCESS;
}
