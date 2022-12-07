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

#include "config.h"

namespace config
{
std::string const &Config::getInputFilename() const { return m_inputFilename; }

std::string const &Config::getEmailSender() const { return m_emailSender; }

std::string const &Config::getSmtpServer() const { return m_smtpServer; }

std::string const &Config::getEmailUsername() const { return m_emailUsername; }

std::string const &Config::getEmailPwd() const { return m_emailPwd; }

bool Config::useEmails() const { return m_useEmails; }

bool Config::useRandomAlgo() const { return m_useRandomAlgo; }
}  // namespace config