# SPDX-FileCopyrightText: 2024 Joshua Goins <josh@redstrate.com>
# SPDX-License-Identifier: BSD-2-Clause

ecm_add_test(linkparsertest.cpp
        TEST_NAME linkparsertest
        LINK_LIBRARIES Qt::Test plasmatube_static
        NAME_PREFIX "plasmatube-"
)
