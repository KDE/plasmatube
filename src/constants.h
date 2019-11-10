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

#define DEFAULT_INSTANCE "https://invidio.us"

#define SETTINGS_INSTANCE "auth/instance"
#define SETTINGS_USERNAME "auth/username"
#define SETTINGS_COOKIE "auth/cookie"

#define INVIDIOUS_API_SEARCH "/api/v1/search"
#define INVIDIOUS_API_TRENDING "/api/v1/trending"
#define INVIDIOUS_API_TOP "/api/v1/top"
#define INVIDIOUS_API_VIDEOS_ "/api/v1/videos/"

#define INVIDIOUS_API_LOGIN "/login"
#define INVIDIOUS_API_SUBSCRIPTIONS "/api/v1/auth/subscriptions"
#define INVIDIOUS_API_SUBSCRIPTIONS_ "/api/v1/auth/subscriptions/"
#define INVIDIOUS_API_FEED "/api/v1/auth/feed"

#define YTMUSIC_URL "https://music.youtube.com"
#define YTMUSIC_API_SEARCH "/youtubei/v1/search"

#define CREATE_GETTER_AND_SETTER(className, type, memberName, getterName, setterName) \
type className::getterName() const \
{ \
    return memberName; \
} \
\
void className::setterName(const type &getterName) \
{ \
    memberName = getterName; \
}
