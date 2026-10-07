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
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QStyle>
#include <QVBoxLayout>

namespace {
QLabel* makeLabel(const QString& text, const QString& objectName, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    return label;
}

QPushButton* makeModeButton(const QString& objectName, const QString& title, const QString& description, QWidget* parent)
{
    auto* button = new QPushButton(title + "\n" + description, parent);
    button->setObjectName(objectName);
    button->setMinimumHeight(78);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QWidget* makePage(QWidget* parent)
{
    auto* page = new QWidget(parent);
    page->setObjectName("launchPage");
    page->setLayout(new QVBoxLayout());
    page->layout()->setContentsMargins(26, 24, 26, 26);
    page->layout()->setSpacing(12);
    return page;
}
}  // namespace

LaunchOptionsDialog::LaunchOptionsDialog(const QString& minestratorAddress,
                                         const QString& serverAddress,
                                         QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Запуск Minecraft"));
    setModal(true);
    setMinimumWidth(540);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    setStyleSheet(R"(
        QDialog { background: #171d20; color: #edf3e9; }
        QWidget#launchPage { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #263238, stop:1 #1b2226); }
        QLabel#eyebrow { color: #8fbe55; font-size: 11px; font-weight: 700; letter-spacing: 1px; }
        QLabel#title { color: #ffffff; font-size: 23px; font-weight: 800; }
        QLabel#subtitle { color: #b8c5bb; font-size: 13px; }
        QLabel#notice { background: #303c28; border: 1px solid #76944c; border-radius: 6px; color: #e5f0d9; padding: 11px; }
        QLabel#status { background: #1e2a20; border: 1px solid #4a6644; border-radius: 6px; color: #9dc46e; padding: 9px; font-size: 13px; }
        QLabel#statusError { background: #2a1e1e; border: 1px solid #804444; border-radius: 6px; color: #ff9c8d; padding: 9px; font-size: 13px; }
        QLabel#error { color: #ff9c8d; font-size: 12px; }
        QPushButton { border: 1px solid #53655a; border-radius: 6px; color: #eff6ed; font-size: 15px; font-weight: 700; padding: 11px 14px; text-align: left; }
        QPushButton:hover { border-color: #b6d476; background: #3b4c38; }
        QPushButton#minestratorMode { background: #344b2f; border-color: #8ebc59; }
        QPushButton#minestratorMode:hover { background: #48663e; }
        QPushButton#minecraftMode { background: #303a3e; border-color: #6d858c; }
        QPushButton#minecraftMode:hover { background: #3f5056; }
        QPushButton#serverMode { background: #3b344a; border-color: #9980c7; }
        QPushButton#serverMode:hover { background: #4d4261; }
        QPushButton#primaryAction { background: #699c3c; border-color: #a7d36b; text-align: center; }
        QPushButton#primaryAction:hover { background: #7bb84a; }
        QPushButton#secondaryAction { background: #293237; text-align: center; }
        QLineEdit { background: #101518; border: 1px solid #6b7b6e; border-radius: 5px; color: #f5faf2; font-size: 15px; padding: 10px; }
        QLineEdit:focus { border: 2px solid #a7d36b; }
    )");

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(1, 1, 1, 1);
    m_pages = new QStackedWidget(this);
    outerLayout->addWidget(m_pages);

    // ── СТОРІНКА 0: Вибір режиму ──────────────────────────────────────────────
    auto* chooser = makePage(this);
    auto* chooserLayout = qobject_cast<QVBoxLayout*>(chooser->layout());
    chooserLayout->addWidget(makeLabel(tr("VELIX LAUNCHER"), "eyebrow", chooser));
    chooserLayout->addWidget(makeLabel(tr("Оберіть спосіб запуску"), "title", chooser));
    chooserLayout->addWidget(makeLabel(tr("Ви завжди запускаєте той самий вибраний Minecraft-профіль. Змінюється лише спосіб підключення."), "subtitle", chooser));

    auto* minestratorMode = makeModeButton("minestratorMode",
        tr("ЗАПУСК СВОГО СЕРВЕРА"),
        tr("Запустити сервер MineStrator та Minecraft одночасно — сервер буде готовий до підключення."),
        chooser);
    auto* minecraftMode = makeModeButton("minecraftMode",
        tr("ЗАПУСТИТИ MINECRAFT"),
        tr("Звичайний запуск без автоматичного підключення до сервера."),
        chooser);
    auto* serverMode = makeModeButton("serverMode",
        tr("ЗАЙТИ НА СЕРВЕР"),
        tr("Вкажіть будь-який домен або IP — Minecraft підключиться до нього після запуску."),
        chooser);

    chooserLayout->addSpacing(4);
    chooserLayout->addWidget(minestratorMode);
    chooserLayout->addWidget(minecraftMode);
    chooserLayout->addWidget(serverMode);
    chooserLayout->addStretch();
    m_pages->addWidget(chooser);  // index 0

    // ── СТОРІНКА 1: MineStrator (запуск свого сервера) ───────────────────────
    auto* minestratorPage = makePage(this);
    auto* minestratorLayout = qobject_cast<QVBoxLayout*>(minestratorPage->layout());
    minestratorLayout->addWidget(makeLabel(tr("MINESTRATOR"), "eyebrow", minestratorPage));
    minestratorLayout->addWidget(makeLabel(tr("Запуск свого сервера"), "title", minestratorPage));
    minestratorLayout->addWidget(makeLabel(tr("Введіть адресу вашого сервера MineStrator. Сервер та Minecraft запустяться одночасно."), "subtitle", minestratorPage));
    minestratorLayout->addWidget(makeLabel(
        tr("Лаунчер надішле команду запуску вашого сервера MineStrator та одразу запустить Minecraft. "
           "Поки Minecraft завантажується — сервер запускається у фоні."),
        "notice", minestratorPage));

    minestratorLayout->addWidget(makeLabel(tr("Введіть IP/адресу свого сервера"), "eyebrow", minestratorPage));
    m_minestratorAddress = new QLineEdit(minestratorAddress, minestratorPage);
    m_minestratorAddress->setPlaceholderText(tr("example.minestrator.com"));
    minestratorLayout->addWidget(m_minestratorAddress);

    // Лейбл статусу сервера (прихований за замовчуванням)
    m_minestratorStatusLabel = makeLabel(QString(), "status", minestratorPage);
    m_minestratorStatusLabel->setVisible(false);
    minestratorLayout->addWidget(m_minestratorStatusLabel);

    auto* minestratorError = makeLabel(QString(), "error", minestratorPage);
    minestratorLayout->addWidget(minestratorError);
    minestratorLayout->addStretch();

    auto* minestratorButtons = new QHBoxLayout();
    auto* minestratorBack = new QPushButton(tr("Назад"), minestratorPage);
    minestratorBack->setObjectName("secondaryAction");
    auto* minestratorLaunch = new QPushButton(tr("ЗАПУСК СВОГО СЕРВЕРА"), minestratorPage);
    minestratorLaunch->setObjectName("primaryAction");
    minestratorButtons->addWidget(minestratorBack);
    minestratorButtons->addWidget(minestratorLaunch);
    minestratorLayout->addLayout(minestratorButtons);
    m_pages->addWidget(minestratorPage);  // index 1

    // ── СТОРІНКА 2: Тільки Minecraft ─────────────────────────────────────────
    auto* minecraftPage = makePage(this);
    auto* minecraftLayout = qobject_cast<QVBoxLayout*>(minecraftPage->layout());
    minecraftLayout->addWidget(makeLabel(tr("MINECRAFT"), "eyebrow", minecraftPage));
    minecraftLayout->addWidget(makeLabel(tr("Звичайний запуск"), "title", minecraftPage));
    minecraftLayout->addWidget(makeLabel(tr("Minecraft запуститься без адреси для автоматичного підключення. Ви зможете вибрати сервер вже в грі."), "subtitle", minecraftPage));
    minecraftLayout->addStretch();

    auto* minecraftButtons = new QHBoxLayout();
    auto* minecraftBack = new QPushButton(tr("Назад"), minecraftPage);
    minecraftBack->setObjectName("secondaryAction");
    auto* minecraftLaunch = new QPushButton(tr("ЗАПУСТИТИ MINECRAFT"), minecraftPage);
    minecraftLaunch->setObjectName("primaryAction");
    minecraftButtons->addWidget(minecraftBack);
    minecraftButtons->addWidget(minecraftLaunch);
    minecraftLayout->addLayout(minecraftButtons);
    m_pages->addWidget(minecraftPage);  // index 2

    // ── СТОРІНКА 3: Прямий вхід на сервер ────────────────────────────────────
    auto* serverPage = makePage(this);
    auto* serverLayout = qobject_cast<QVBoxLayout*>(serverPage->layout());
    serverLayout->addWidget(makeLabel(tr("ШВИДКЕ ПІДКЛЮЧЕННЯ"), "eyebrow", serverPage));
    serverLayout->addWidget(makeLabel(tr("Зайти на сервер"), "title", serverPage));
    serverLayout->addWidget(makeLabel(tr("Вкажіть домен або IP Minecraft-сервера. За потреби додайте порт через двокрапку. MineStrator API не використовується."), "subtitle", serverPage));

    serverLayout->addWidget(makeLabel(tr("Введіть IP/адресу сервера"), "eyebrow", serverPage));
    m_serverAddress = new QLineEdit(serverAddress, serverPage);
    m_serverAddress->setPlaceholderText(tr("play.example.org:25565"));
    serverLayout->addWidget(m_serverAddress);

    auto* serverError = makeLabel(QString(), "error", serverPage);
    serverLayout->addWidget(serverError);
    serverLayout->addStretch();

    auto* serverButtons = new QHBoxLayout();
    auto* serverBack = new QPushButton(tr("Назад"), serverPage);
    serverBack->setObjectName("secondaryAction");
    auto* serverLaunch = new QPushButton(tr("ЗАЙТИ НА СЕРВЕР"), serverPage);
    serverLaunch->setObjectName("primaryAction");
    serverButtons->addWidget(serverBack);
    serverButtons->addWidget(serverLaunch);
    serverLayout->addLayout(serverButtons);
    m_pages->addWidget(serverPage);  // index 3

    // ── Зв'язки сигналів ─────────────────────────────────────────────────────
    connect(minestratorMode, &QPushButton::clicked, this, [this] { showPage(1); });
    connect(minecraftMode,   &QPushButton::clicked, this, [this] { showPage(2); });
    connect(serverMode,      &QPushButton::clicked, this, [this] { showPage(3); });

    connect(minestratorBack, &QPushButton::clicked, this, [this] { showPage(0); });
    connect(minecraftBack,   &QPushButton::clicked, this, [this] { showPage(0); });
    connect(serverBack,      &QPushButton::clicked, this, [this] { showPage(0); });

    // Кнопка «ЗАПУСК СВОГО СЕРВЕРА»:
    // Просто приймаємо діалог — реальний паралельний запуск відбувається в MainWindow
    connect(minestratorLaunch, &QPushButton::clicked, this, [this, minestratorError] {
        minestratorError->clear();
        acceptAddress(Choice::Minestrator, m_minestratorAddress);
        if (m_choice == Choice::None)
            minestratorError->setText(tr("Введіть адресу сервера без пробілів."));
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
        if (m_choice == Choice::None)
            serverError->setText(tr("Введіть адресу без пробілів."));
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
    // Оновлюємо стиль через polish/unpolish
    m_minestratorStatusLabel->style()->unpolish(m_minestratorStatusLabel);
    m_minestratorStatusLabel->style()->polish(m_minestratorStatusLabel);
    m_minestratorStatusLabel->setText(text);
    m_minestratorStatusLabel->setVisible(true);
}
