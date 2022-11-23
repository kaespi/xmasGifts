# This file is part of xmasGifts.
#
# xmasGifts is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# xmasGifts is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xmasGifts.  If not, see <http:#www.gnu.org/licenses/>.
#

CXXFLAGS = -O2 -Wall -std=c++11
OBJS = xmasGifts.o parser.o shuffle.o output.o
TARGET = xmasGifts

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS)

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
