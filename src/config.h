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

#pragma once

#include <string>

#include "output.h"

namespace config
{
class Config
{
public:
    Config() = default;

    template <typename T>
    void setConfigValue(std::string_view cfgOption, T&& cfgValue)
    {
        if constexpr (std::is_same_v<bool, T>) {
            if (cfgOption == "useEmails") {
                m_useEmails = cfgValue;
            } else if (cfgOption == "useRandomAlgo") {
                m_useRandomAlgo = cfgValue;
            } else {
                // unknown entry, just don't do anything
            }
        } else if constexpr (std::is_same_v<std::string, T>) {
            if (cfgOption == "inputFilename") {
                m_inputFilename = cfgValue;
            } else if (cfgOption == "emailSender") {
                m_emailSender = cfgValue;
            } else if (cfgOption == "smtpServer") {
                m_smtpServer = cfgValue;
            } else if (cfgOption == "emailUsername") {
                m_emailUsername = cfgValue;
            } else if (cfgOption == "emailPwd") {
                m_emailPwd = cfgValue;
            } else {
                // unknown entry, just don't do anything
            }
        } else {
            dbg << "Unknown configuration option " << cfgOption << std::endl;
        }
    }

    std::string const& getInputFilename() const;
    std::string const& getEmailSender() const;
    std::string const& getSmtpServer() const;
    std::string const& getEmailUsername() const;
    std::string const& getEmailPwd() const;
    bool useEmails() const;
    bool useRandomAlgo() const;

private:
    std::string m_inputFilename{};
    std::string m_emailSender{};
    std::string m_smtpServer{};
    std::string m_emailUsername{};
    std::string m_emailPwd{};
    bool m_useEmails{false};
    bool m_useRandomAlgo{false};
};
}  // namespace config
