# SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
# SPDX-License-Identifier: CC0-1.0

ecm_add_test(linkparsertest.cpp
        TEST_NAME linkparsertest
        LINK_LIBRARIES Qt::Test plasmatube_static
        NAME_PREFIX "plasmatube-"
)
