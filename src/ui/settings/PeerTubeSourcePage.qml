// SPDX-FileCopyrightText: 2023 Joshua Goins <josh@redstrate.com>
// SPDX-License-Identifier: GPL-3.0-or-later

import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.plasmatube

BaseSourcePage {
    id: page

    FormCard.FormHeader {
        title: i18n("Account")
    }

    AccountCard {
        source: page.source
    }
}