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

#include "guid.h"

#include <uuid/uuid.h>

#include <iomanip>
#include <sstream>
#include <type_traits>

namespace guid
{
std::string getGuidStr8_4_4_4_12()
{
    // formatting style of the GUID (default 8-4-4-4-12)
    constexpr std::array guidNumdigitsGroups{8, 4, 4, 4, 12};

    // generate the 32 random GUID bytes
    uuid_t guidCstyle;
    uuid_generate(guidCstyle);

    // convert he C-style array guidCstyle to a C++ std::array
    constexpr int N = sizeof(guidCstyle) / sizeof(guidCstyle[0]);
    std::array<std::remove_reference<decltype(guidCstyle[0])>::type, N> guid;
    std::move(std::begin(guidCstyle), std::end(guidCstyle), guid.begin());

    auto itGuid = guid.cbegin();

    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto numdigits : guidNumdigitsGroups) {
        for (auto n = 0; n < numdigits; n += 2) {
            ss << std::setw(2) << static_cast<unsigned>(*itGuid++);
        }
        if (itGuid != guid.cend()) {
            ss << "-";
        }
    }

    return ss.str();
}
}  // namespace guid
