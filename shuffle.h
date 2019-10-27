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

#include <vector>
#include <map>

#include "person.h"

// shuffle the list of people and find a valid circular sequence of donor->giftee
std::vector<Person> shuffle1(std::vector<Person> &people);

// rotates the beginning of the vector randomly
std::map<unsigned int, std::string> randomizePersonNumbers(std::vector<Person> &people);
