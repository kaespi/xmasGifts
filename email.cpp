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

#include "email.h"

#include "guid.h"
#include "output.h"

#include <iostream>
#include <sstream>

#include <quickmail.h>

namespace
{
std::string_view getEmailAddrDomain(std::string_view const emailAddr)
{
    auto posAtChar = emailAddr.find_first_of('@');
    if (posAtChar != std::string_view::npos) {
        return emailAddr.substr(posAtChar+1);
    } else {
        return emailAddr;
    }
}

bool validEmailConfig(config::Config const& cfg)
{
    return not cfg.getEmailPwd().empty() &&
        not cfg.getEmailSender().empty() &&
        not cfg.getEmailUsername().empty() &&
        not cfg.getSmtpServer().empty();
}
} // namespace

namespace email
{
constexpr std::string_view msgSubject{"Ho Ho Ho!"};
constexpr std::string_view msgSalutation{"Hallo"};
constexpr std::string_view msgPart1{"Ich freue mich, dass du mich auch dieses Jahr tatkräftig beim "
                                    "Wichteln unterstützt. Deine Aufgabe bis Weihnachten: ein unvergessliches, grandioses, lustiges und "
                                    "nicht all zu teures Geschenk für"};
constexpr std::string_view msgPart2{"basteln/kaufen/bestellen/organisieren. "
                                    "Viel Spass und Erfolg!\n\nDeine Familie Schweizer Wichtelfee"};

void sendEmails(std::vector<Person> &giftList, config::Config const& cfg)
{
    if (not cfg.useEmails()) {
        // sending emails not commanded
        return;
    } else if (not validEmailConfig(cfg)) {
        std::cerr << "Email configuration incomplete, not sending any email." << std::endl;
        return;
    }

    std::cout << "Sending emails ";

    // giftee iterator points one ahead
    auto itGiftee = ++(giftList.begin());

    for (auto itDonor=giftList.begin(); itDonor!=giftList.end(); ++itDonor)
    {
        // wrap around the giftee (which always points one ahead)
        if (itGiftee==giftList.end())
        {
            itGiftee = giftList.begin();
        }

        quickmail mailobj = quickmail_create(cfg.getEmailSender().c_str(), msgSubject.data());
        quickmail_add_to(mailobj, itDonor->email.value().c_str());
        std::ostringstream ss;
        ss << msgSalutation << " " << itDonor->name << ",\n\n";
        ss << msgPart1 << " " << itGiftee->name << " ";
        ss << msgPart2;
        quickmail_set_body(mailobj, ss.str().c_str());

        // Google needs a GUID in the email header's Message-ID
        // (otherwise you have a good chance it's moved to Junk)
        ss.str("");
        ss.clear();
        ss << "Message-ID: <" << guid::getGuidStr8_4_4_4_12() << "@" << getEmailAddrDomain(cfg.getEmailSender()) << ">";
        quickmail_add_header(mailobj, ss.str().c_str());

        constexpr unsigned smtpport{25};
        const char* emailSendResult = quickmail_send(mailobj, cfg.getSmtpServer().c_str(),
            smtpport, cfg.getEmailUsername().c_str(), cfg.getEmailPwd().c_str());

        if (emailSendResult) {
            std::cerr << "Could not send an email to " << itDonor->name << " (" << itDonor->email.value() << "): " << emailSendResult << std::endl;
        } else {
            std::cout << ".";
        }

        ++itGiftee;
    }

    std::cout << " Done." << std::endl;
}
} // namespace email
