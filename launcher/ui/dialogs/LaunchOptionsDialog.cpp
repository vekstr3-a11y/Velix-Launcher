// SPDX-License-Identifier: GPL-3.0-only
/*
 *  Velix Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Velix Launcher Contributors
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "LaunchOptionsDialog.h"

#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QLocale>
#include <QPushButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

#include "Application.h"
#include "settings/SettingsObject.h"

namespace {

QString getLangCode()
{
    QString lang;
    if (APPLICATION && APPLICATION->settings()) {
        lang = APPLICATION->settings()->get("Language").toString().trimmed().toLower();
    }
    if (lang.isEmpty()) {
        lang = QLocale::system().name().toLower();
    }
    if (lang.contains('_')) {
        lang = lang.section('_', 0, 0);
    }
    return lang;
}

QLabel* makeLabel(const QString& text, const QString& objectName, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    return label;
}

QPushButton* makeModeButton(const QString& objectName, const QString& title, const QString& description, QWidget* parent)
{
    auto* button = new QPushButton(parent);
    button->setObjectName(objectName);
    button->setMinimumHeight(76);
    button->setCursor(Qt::PointingHandCursor);

    auto* layout = new QVBoxLayout(button);
    layout->setContentsMargins(14, 10, 14, 10);
    layout->setSpacing(4);

    auto* titleLabel = new QLabel(title, button);
    titleLabel->setObjectName("modeTitle");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; background: transparent;");

    auto* descLabel = new QLabel(description, button);
    descLabel->setObjectName("modeDesc");
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("font-size: 12px; opacity: 0.8; background: transparent;");

    layout->addWidget(titleLabel);
    layout->addWidget(descLabel);

    return button;
}

QWidget* makePage(QWidget* parent)
{
    auto* page = new QWidget(parent);
    page->setObjectName("launchPage");
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 22, 24, 22);
    layout->setSpacing(12);
    return page;
}

}  // namespace

QString LaunchOptionsDialog::trLoc(const QString& key)
{
    const QString lang = getLangCode();

    if (key == "dialog_title") {
        if (lang == "uk") return QString::fromUtf8("Запуск Minecraft — Velix");
        if (lang == "ru") return QString::fromUtf8("Запуск Minecraft — Velix");
        if (lang == "de") return QString::fromUtf8("Minecraft starten — Velix");
        if (lang == "pl") return QString::fromUtf8("Uruchom Minecraft — Velix");
        return QStringLiteral("Launch Minecraft — Velix");
    }
    if (key == "eyebrow_velix") {
        return QStringLiteral("VELIX LAUNCHER");
    }
    if (key == "choose_title") {
        if (lang == "uk") return QString::fromUtf8("Оберіть спосіб запуску");
        if (lang == "ru") return QString::fromUtf8("Выберите способ запуска");
        if (lang == "de") return QString::fromUtf8("Startmethode wählen");
        if (lang == "pl") return QString::fromUtf8("Wybierz metodę uruchomienia");
        return QStringLiteral("Choose Launch Method");
    }
    if (key == "choose_sub") {
        if (lang == "uk") return QString::fromUtf8("Ви запускаєте вибраний профіль. Змінюється лише спосіб підключення.");
        if (lang == "ru") return QString::fromUtf8("Вы запускаете выбранный профиль. Меняется только способ подключения.");
        if (lang == "de") return QString::fromUtf8("Sie starten das ausgewählte Profil. Nur die Verbindung ändert sich.");
        if (lang == "pl") return QString::fromUtf8("Uruchamiasz wybrany profil. Zmienia się tylko metoda połączenia.");
        return QStringLiteral("You are launching the selected profile. Only the connection mode changes.");
    }
    if (key == "mode_minestrator_title") {
        if (lang == "uk") return QString::fromUtf8("ЗАПУСК СВОГО СЕРВЕРА");
        if (lang == "ru") return QString::fromUtf8("ЗАПУСК СВОЕГО СЕРВЕРА");
        if (lang == "de") return QString::fromUtf8("EIGENEN SERVER STARTEN");
        if (lang == "pl") return QString::fromUtf8("URUCHOM WŁASNY SERWER");
        return QStringLiteral("START OWN SERVER");
    }
    if (key == "mode_minestrator_desc") {
        if (lang == "uk") return QString::fromUtf8("Запустити сервер MineStrator та Minecraft одночасно — сервер буде готовий до підключення.");
        if (lang == "ru") return QString::fromUtf8("Запустить сервер MineStrator и Minecraft одновременно — сервер будет готов к подключению.");
        if (lang == "de") return QString::fromUtf8("MineStrator-Server und Minecraft gleichzeitig starten.");
        if (lang == "pl") return QString::fromUtf8("Uruchom serwer MineStrator i Minecraft jednocześnie.");
        return QStringLiteral("Start MineStrator server and Minecraft simultaneously — ready when loaded.");
    }
    if (key == "mode_minecraft_title") {
        if (lang == "uk") return QString::fromUtf8("ЗАПУСТИТИ MINECRAFT");
        if (lang == "ru") return QString::fromUtf8("ЗАПУСТИТЬ MINECRAFT");
        if (lang == "de") return QString::fromUtf8("MINECRAFT STARTEN");
        if (lang == "pl") return QString::fromUtf8("URUCHOM MINECRAFT");
        return QStringLiteral("LAUNCH MINECRAFT");
    }
    if (key == "mode_minecraft_desc") {
        if (lang == "uk") return QString::fromUtf8("Звичайний запуск без автоматичного підключення до сервера.");
        if (lang == "ru") return QString::fromUtf8("Обычный запуск без автоматического подключения к серверу.");
        if (lang == "de") return QString::fromUtf8("Standardstart ohne automatisches Verbinden.");
        if (lang == "pl") return QString::fromUtf8("Zwykłe uruchomienie bez automatycznego łączenia.");
        return QStringLiteral("Standard launch without automatic server connection.");
    }
    if (key == "mode_server_title") {
        if (lang == "uk") return QString::fromUtf8("ЗАЙТИ НА СЕРВЕР");
        if (lang == "ru") return QString::fromUtf8("ВОЙТИ НА СЕРВЕР");
        if (lang == "de") return QString::fromUtf8("SERVER BEITRETEN");
        if (lang == "pl") return QString::fromUtf8("DOŁĄCZ DO SERWERA");
        return QStringLiteral("JOIN SERVER");
    }
    if (key == "mode_server_desc") {
        if (lang == "uk") return QString::fromUtf8("Вкажіть будь-який домен або IP — Minecraft підключиться до нього після запуску.");
        if (lang == "ru") return QString::fromUtf8("Укажите любой домен или IP — Minecraft подключится к нему после запуска.");
        if (lang == "de") return QString::fromUtf8("Domain oder IP eingeben — Minecraft verbindet sich direkt.");
        if (lang == "pl") return QString::fromUtf8("Podaj domenę lub IP — Minecraft połączy się bezpośrednio.");
        return QStringLiteral("Specify any domain or IP — Minecraft will connect automatically.");
    }
    if (key == "minestrator_notice") {
        if (lang == "uk") return QString::fromUtf8("Лаунчер надішле команду запуску вашого сервера MineStrator та одразу запустить Minecraft у паралельному режимі. Поки гра завантажується — сервер запускається у фоні.");
        if (lang == "ru") return QString::fromUtf8("Лаунчер отправит команду запуска вашего сервера MineStrator и сразу запустит Minecraft параллельно. Пока игра загружается — сервер запускается в фоне.");
        if (lang == "de") return QString::fromUtf8("Der Launcher startet Ihren MineStrator-Server und öffnet Minecraft sofort parallel im Hintergrund.");
        if (lang == "pl") return QString::fromUtf8("Launcher wyśle polecenie uruchomienia serwera MineStrator i od razu odpali Minecraft.");
        return QStringLiteral("The launcher triggers your MineStrator server power action and starts Minecraft simultaneously. The server boots in the background while Minecraft loads.");
    }
    if (key == "server_address_label") {
        if (lang == "uk") return QString::fromUtf8("IP / адреса сервера");
        if (lang == "ru") return QString::fromUtf8("IP / адрес сервера");
        return QStringLiteral("Server IP / Address");
    }
    if (key == "minestrator_api_key_label") {
        if (lang == "uk") return QString::fromUtf8("MineStrator API-ключ (panel.minestrator.com):");
        if (lang == "ru") return QString::fromUtf8("MineStrator API-ключ (panel.minestrator.com):");
        return QStringLiteral("MineStrator API Key (panel.minestrator.com):");
    }
    if (key == "minestrator_server_id_label") {
        if (lang == "uk") return QString::fromUtf8("MineStrator Server ID:");
        if (lang == "ru") return QString::fromUtf8("MineStrator Server ID:");
        return QStringLiteral("MineStrator Server ID:");
    }
    if (key == "back") {
        if (lang == "uk") return QString::fromUtf8("Назад");
        if (lang == "ru") return QString::fromUtf8("Назад");
        if (lang == "de") return QString::fromUtf8("Zurück");
        if (lang == "pl") return QString::fromUtf8("Wstecz");
        return QStringLiteral("Back");
    }
    if (key == "err_address") {
        if (lang == "uk") return QString::fromUtf8("Введіть коректну адресу сервера без пробілів.");
        if (lang == "ru") return QString::fromUtf8("Введите корректный адрес сервера без пробелов.");
        return QStringLiteral("Please enter a valid server address without spaces.");
    }
    if (key == "err_credentials") {
        if (lang == "uk") return QString::fromUtf8("Для автозапуску сервера введіть ваш MineStrator API-ключ та Server ID.");
        if (lang == "ru") return QString::fromUtf8("Для автозапуска сервера введите ваш MineStrator API-ключ и Server ID.");
        return QStringLiteral("Please enter your MineStrator API Key and Server ID to trigger server start.");
    }
    if (key == "show") {
        if (lang == "uk") return QString::fromUtf8("Показати");
        if (lang == "ru") return QString::fromUtf8("Показать");
        return QStringLiteral("Show");
    }
    if (key == "hide") {
        if (lang == "uk") return QString::fromUtf8("Приховати");
        if (lang == "ru") return QString::fromUtf8("Скрыть");
        return QStringLiteral("Hide");
    }

    return key;
}

LaunchOptionsDialog::LaunchOptionsDialog(const QString& minestratorAddress,
                                         const QString& serverAddress,
                                         QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(trLoc("dialog_title"));
    setModal(true);
    setMinimumWidth(540);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // Адаптивний стиль — використовує кольори активної теми (світлої/темної/Catppuccin тощо)
    setStyleSheet(R"(
        QWidget#launchPage {
            background: transparent;
        }
        QLabel#eyebrow {
            color: #4caf50;
            font-size: 11px;
            font-weight: bold;
            letter-spacing: 1px;
        }
        QLabel#title {
            font-size: 20px;
            font-weight: bold;
        }
        QLabel#subtitle {
            font-size: 13px;
        }
        QLabel#notice {
            border: 1px solid rgba(76, 175, 80, 0.45);
            border-radius: 6px;
            padding: 10px;
            background: rgba(76, 175, 80, 0.12);
            font-size: 13px;
        }
        QLabel#status {
            border: 1px solid rgba(76, 175, 80, 0.45);
            border-radius: 6px;
            padding: 8px;
            background: rgba(76, 175, 80, 0.15);
            font-size: 13px;
        }
        QLabel#statusError {
            border: 1px solid rgba(244, 67, 54, 0.5);
            border-radius: 6px;
            padding: 8px;
            background: rgba(244, 67, 54, 0.15);
            color: #f44336;
            font-size: 13px;
        }
        QLabel#error {
            color: #f44336;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton#modeButton {
            border: 1px solid palette(mid);
            border-radius: 8px;
            text-align: left;
        }
        QPushButton#modeButton:hover {
            border: 1px solid #4caf50;
            background: rgba(76, 175, 80, 0.15);
        }
        QPushButton#primaryAction {
            font-weight: bold;
            padding: 9px 18px;
            min-height: 28px;
        }
        QPushButton#secondaryAction {
            padding: 9px 18px;
            min-height: 28px;
        }
        QLineEdit {
            border-radius: 5px;
            padding: 7px 10px;
            font-size: 14px;
        }
    )");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(4, 4, 4, 4);
    m_pages = new QStackedWidget(this);
    outerLayout->addWidget(m_pages);

    // ── СТОРІНКА 0: Вибір режиму ──────────────────────────────────────────────
    auto* chooser = makePage(this);
    auto* chooserLayout = qobject_cast<QVBoxLayout*>(chooser->layout());
    chooserLayout->addWidget(makeLabel(trLoc("eyebrow_velix"), "eyebrow", chooser));
    chooserLayout->addWidget(makeLabel(trLoc("choose_title"), "title", chooser));
    chooserLayout->addWidget(makeLabel(trLoc("choose_sub"), "subtitle", chooser));

    auto* minestratorMode = makeModeButton("modeButton",
        trLoc("mode_minestrator_title"),
        trLoc("mode_minestrator_desc"),
        chooser);
    auto* minecraftMode = makeModeButton("modeButton",
        trLoc("mode_minecraft_title"),
        trLoc("mode_minecraft_desc"),
        chooser);
    auto* serverMode = makeModeButton("modeButton",
        trLoc("mode_server_title"),
        trLoc("mode_server_desc"),
        chooser);

    chooserLayout->addSpacing(6);
    chooserLayout->addWidget(minestratorMode);
    chooserLayout->addWidget(minecraftMode);
    chooserLayout->addWidget(serverMode);
    chooserLayout->addStretch();
    m_pages->addWidget(chooser);  // index 0

    // ── СТОРІНКА 1: MineStrator (запуск свого сервера) ───────────────────────
    auto* minestratorPage = makePage(this);
    auto* minestratorLayout = qobject_cast<QVBoxLayout*>(minestratorPage->layout());
    minestratorLayout->addWidget(makeLabel(QStringLiteral("MINESTRATOR API"), "eyebrow", minestratorPage));
    minestratorLayout->addWidget(makeLabel(trLoc("mode_minestrator_title"), "title", minestratorPage));
    minestratorLayout->addWidget(makeLabel(trLoc("minestrator_notice"), "notice", minestratorPage));

    // Поле адреси сервера
    minestratorLayout->addWidget(makeLabel(trLoc("server_address_label"), "subtitle", minestratorPage));
    m_minestratorAddress = new QLineEdit(minestratorAddress, minestratorPage);
    m_minestratorAddress->setPlaceholderText(QStringLiteral("example.minestrator.com"));
    minestratorLayout->addWidget(m_minestratorAddress);

    // Секція налаштувань API (Server ID та API Key)
    auto* apiGroup = new QGroupBox(QStringLiteral("MineStrator Credentials"), minestratorPage);
    auto* apiGroupLayout = new QVBoxLayout(apiGroup);
    apiGroupLayout->setContentsMargins(12, 12, 12, 12);
    apiGroupLayout->setSpacing(8);

    // Server ID
    apiGroupLayout->addWidget(makeLabel(trLoc("minestrator_server_id_label"), "subtitle", apiGroup));
    QString savedServerId;
    QString savedApiKey;
    if (APPLICATION && APPLICATION->settings()) {
        savedServerId = APPLICATION->settings()->get("MinestratorServerId").toString();
        savedApiKey   = APPLICATION->settings()->get("MinestratorApiKey").toString();
    }
    m_minestratorServerId = new QLineEdit(savedServerId, apiGroup);
    m_minestratorServerId->setPlaceholderText(QStringLiteral("12345"));
    apiGroupLayout->addWidget(m_minestratorServerId);

    // API Key
    apiGroupLayout->addWidget(makeLabel(trLoc("minestrator_api_key_label"), "subtitle", apiGroup));
    auto* keyRow = new QHBoxLayout();
    m_minestratorApiKey = new QLineEdit(savedApiKey, apiGroup);
    m_minestratorApiKey->setEchoMode(QLineEdit::Password);
    m_minestratorApiKey->setPlaceholderText(QStringLiteral("••••••••••••••••••••••••••••••••"));
    auto* toggleKeyBtn = new QPushButton(trLoc("show"), apiGroup);
    toggleKeyBtn->setFixedWidth(90);
    connect(toggleKeyBtn, &QPushButton::clicked, this, [this, toggleKeyBtn]() {
        if (m_minestratorApiKey->echoMode() == QLineEdit::Password) {
            m_minestratorApiKey->setEchoMode(QLineEdit::Normal);
            toggleKeyBtn->setText(trLoc("hide"));
        } else {
            m_minestratorApiKey->setEchoMode(QLineEdit::Password);
            toggleKeyBtn->setText(trLoc("show"));
        }
    });
    keyRow->addWidget(m_minestratorApiKey);
    keyRow->addWidget(toggleKeyBtn);
    apiGroupLayout->addLayout(keyRow);

    minestratorLayout->addWidget(apiGroup);

    // Лейбл статусу сервера (якщо потрібно)
    m_minestratorStatusLabel = makeLabel(QString(), "status", minestratorPage);
    m_minestratorStatusLabel->setVisible(false);
    minestratorLayout->addWidget(m_minestratorStatusLabel);

    auto* minestratorError = makeLabel(QString(), "error", minestratorPage);
    minestratorLayout->addWidget(minestratorError);
    minestratorLayout->addStretch();

    auto* minestratorButtons = new QHBoxLayout();
    auto* minestratorBack = new QPushButton(trLoc("back"), minestratorPage);
    minestratorBack->setObjectName("secondaryAction");
    auto* minestratorLaunch = new QPushButton(trLoc("mode_minestrator_title"), minestratorPage);
    minestratorLaunch->setObjectName("primaryAction");
    minestratorButtons->addWidget(minestratorBack);
    minestratorButtons->addWidget(minestratorLaunch);
    minestratorLayout->addLayout(minestratorButtons);
    m_pages->addWidget(minestratorPage);  // index 1

    // ── СТОРІНКА 2: Тільки Minecraft ─────────────────────────────────────────
    auto* minecraftPage = makePage(this);
    auto* minecraftLayout = qobject_cast<QVBoxLayout*>(minecraftPage->layout());
    minecraftLayout->addWidget(makeLabel(QStringLiteral("MINECRAFT"), "eyebrow", minecraftPage));
    minecraftLayout->addWidget(makeLabel(trLoc("mode_minecraft_title"), "title", minecraftPage));
    minecraftLayout->addWidget(makeLabel(trLoc("mode_minecraft_desc"), "subtitle", minecraftPage));
    minecraftLayout->addStretch();

    auto* minecraftButtons = new QHBoxLayout();
    auto* minecraftBack = new QPushButton(trLoc("back"), minecraftPage);
    minecraftBack->setObjectName("secondaryAction");
    auto* minecraftLaunch = new QPushButton(trLoc("mode_minecraft_title"), minecraftPage);
    minecraftLaunch->setObjectName("primaryAction");
    minecraftButtons->addWidget(minecraftBack);
    minecraftButtons->addWidget(minecraftLaunch);
    minecraftLayout->addLayout(minecraftButtons);
    m_pages->addWidget(minecraftPage);  // index 2

    // ── СТОРІНКА 3: Прямий вхід на сервер ────────────────────────────────────
    auto* serverPage = makePage(this);
    auto* serverLayout = qobject_cast<QVBoxLayout*>(serverPage->layout());
    serverLayout->addWidget(makeLabel(QStringLiteral("DIRECT JOIN"), "eyebrow", serverPage));
    serverLayout->addWidget(makeLabel(trLoc("mode_server_title"), "title", serverPage));
    serverLayout->addWidget(makeLabel(trLoc("mode_server_desc"), "subtitle", serverPage));

    serverLayout->addWidget(makeLabel(trLoc("server_address_label"), "subtitle", serverPage));
    m_serverAddress = new QLineEdit(serverAddress, serverPage);
    m_serverAddress->setPlaceholderText(QStringLiteral("play.example.org:25565"));
    serverLayout->addWidget(m_serverAddress);

    auto* serverError = makeLabel(QString(), "error", serverPage);
    serverLayout->addWidget(serverError);
    serverLayout->addStretch();

    auto* serverButtons = new QHBoxLayout();
    auto* serverBack = new QPushButton(trLoc("back"), serverPage);
    serverBack->setObjectName("secondaryAction");
    auto* serverLaunch = new QPushButton(trLoc("mode_server_title"), serverPage);
    serverLaunch->setObjectName("primaryAction");
    serverButtons->addWidget(serverBack);
    serverButtons->addWidget(serverLaunch);
    serverLayout->addLayout(serverButtons);
    m_pages->addWidget(serverPage);  // index 3

    // ── Зв'язки навігації ────────────────────────────────────────────────────
    connect(minestratorMode, &QPushButton::clicked, this, [this] { showPage(1); });
    connect(minecraftMode,   &QPushButton::clicked, this, [this] { showPage(2); });
    connect(serverMode,      &QPushButton::clicked, this, [this] { showPage(3); });

    connect(minestratorBack, &QPushButton::clicked, this, [this] { showPage(0); });
    connect(minecraftBack,   &QPushButton::clicked, this, [this] { showPage(0); });
    connect(serverBack,      &QPushButton::clicked, this, [this] { showPage(0); });

    // Кнопка «ЗАПУСК СВОГО СЕРВЕРА»:
    connect(minestratorLaunch, &QPushButton::clicked, this, [this, minestratorError] {
        minestratorError->clear();
        const QString addr = m_minestratorAddress->text().trimmed();
        const QString apiKey = m_minestratorApiKey->text().trimmed();
        const QString srvId = m_minestratorServerId->text().trimmed();

        if (addr.isEmpty() || addr.contains(QRegularExpression("\\s"))) {
            minestratorError->setText(trLoc("err_address"));
            return;
        }

        if (apiKey.isEmpty() || srvId.isEmpty()) {
            minestratorError->setText(trLoc("err_credentials"));
            return;
        }

        // Зберігаємо налаштування MineStrator для наступних запусків
        if (APPLICATION && APPLICATION->settings()) {
            APPLICATION->settings()->set("MinestratorServerAddress", addr);
            APPLICATION->settings()->set("MinestratorApiKey", apiKey);
            APPLICATION->settings()->set("MinestratorServerId", srvId);
        }

        m_choice = Choice::Minestrator;
        accept();
    });

    // Кнопка «ЗАПУСТИТИ MINECRAFT»: тільки запуск, без сервера
    connect(minecraftLaunch, &QPushButton::clicked, this, [this] {
        m_choice = Choice::Minecraft;
        accept();
    });

    // Кнопка «ЗАЙТИ НА СЕРВЕР»: Minecraft + автопідключення (без MineStrator API)
    connect(serverLaunch, &QPushButton::clicked, this, [this, serverError] {
        serverError->clear();
        acceptAddress(Choice::Server, m_serverAddress);
        if (m_choice == Choice::None) {
            serverError->setText(trLoc("err_address"));
        } else if (APPLICATION && APPLICATION->settings()) {
            APPLICATION->settings()->set("DirectJoinServerAddress", m_serverAddress->text().trimmed());
        }
    });
}

QString LaunchOptionsDialog::serverAddress() const
{
    if (m_choice == Choice::Minestrator)
        return m_minestratorAddress->text().trimmed();
    if (m_choice == Choice::Server)
        return m_serverAddress->text().trimmed();
    return {};
}

void LaunchOptionsDialog::showPage(int index)
{
    m_pages->setCurrentIndex(index);
    if (index == 1)
        m_minestratorAddress->setFocus();
    else if (index == 3)
        m_serverAddress->setFocus();
}

void LaunchOptionsDialog::acceptAddress(Choice choice, QLineEdit* addressField)
{
    const QString address = addressField->text().trimmed();
    if (address.isEmpty() || address.contains(QRegularExpression("\\s"))) {
        m_choice = Choice::None;
        return;
    }

    m_choice = choice;
    accept();
}

void LaunchOptionsDialog::updateStatusLabel(const QString& text, bool isError)
{
    if (!m_minestratorStatusLabel)
        return;

    if (text.isEmpty()) {
        m_minestratorStatusLabel->setVisible(false);
        return;
    }

    m_minestratorStatusLabel->setObjectName(isError ? "statusError" : "status");
    m_minestratorStatusLabel->style()->unpolish(m_minestratorStatusLabel);
    m_minestratorStatusLabel->style()->polish(m_minestratorStatusLabel);
    m_minestratorStatusLabel->setText(text);
    m_minestratorStatusLabel->setVisible(true);
}
