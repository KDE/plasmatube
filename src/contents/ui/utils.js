/*
 * Copyright 2019  Linus Jahn <lnj@kaidan.im>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

.pragma library

function formatCount(count, decimals) {
   if (count === 0)
       return '0';
   var k = 1000,
       dm = decimals <= 0 ? 0 : decimals || 0,
       sizes = ['', 'K', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y'],
       i = Math.floor(Math.log(count) / Math.log(k));
   return parseFloat((count / Math.pow(k, i)).toFixed(dm)) + sizes[i];
}

function formatTime(time) {
    if (Qt.formatTime(time, "hh") === "00")
        return Qt.formatTime(time, "mm:ss")
    else if (Qt.formatTime(time, "hh") < 10)
        return Qt.formatTime(time, "h:mm:ss")
    else
        return Qt.formatTime(time, "hh:mm:ss")
}

function trendingToString(category) {
    switch (category) {
    case "music":
        return qsTr("Trending Music")
    case "gaming":
        return qsTr("Trending Gaming")
    case "news":
        return qsTr("Trending News")
    case "movies":
        return qsTr("Trending Movies")
    default:
        return qsTr("Trending")
    }
}
