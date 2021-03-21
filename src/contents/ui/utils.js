// SPDX-FileCopyrightText: 2019 Linus Jahn <lnj@kaidan.im>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
