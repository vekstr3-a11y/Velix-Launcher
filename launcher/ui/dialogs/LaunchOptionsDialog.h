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

#pragma once

#include <QDialog>
#include <QString>

class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;

/**
 * LaunchOptionsDialog - діалог вибору режиму запуску Velix Launcher
 *
 * Три режими:
 *   1. ЗАПУСК СВОГО СЕРВЕРА  — запускає MineStrator сервер + Minecraft паралельно
 *   2. ЗАПУСТИТИ MINECRAFT    — тільки Minecraft, без сервера
 *   3. ЗАЙТИ НА СЕРВЕР       — Minecraft + автопідключення до довільного IP
 *
 * Підтримує теми оформлення та мультимовний переклад (uk, en, ru, de, pl тощо).
 */
class LaunchOptionsDialog final : public QDialog {
    Q_OBJECT

   public:
    enum class Choice {
        None,
        Minestrator,  ///< Запуск свого сервера MineStrator + Minecraft паралельно
        Minecraft,    ///< Тільки запуск Minecraft
        Server,       ///< Minecraft + автопідключення до довільного сервера
    };

    explicit LaunchOptionsDialog(const QString& minestratorAddress,
                                 const QString& serverAddress,
                                 QWidget* parent = nullptr);

    Choice choice() const { return m_choice; }

    /** Повертає адресу сервера (для Minestrator або Server режимів) */
    QString serverAddress() const;

   private:
    void showPage(int index);
    void acceptAddress(Choice choice, QLineEdit* addressField);
    void updateStatusLabel(const QString& text, bool isError = false);
    static QString trLoc(const QString& key);

    Choice m_choice = Choice::None;
    QStackedWidget* m_pages = nullptr;
    QLineEdit* m_minestratorAddress = nullptr;   ///< Поле адреси для MineStrator
    QLineEdit* m_minestratorApiKey = nullptr;    ///< Поле API-ключа MineStrator
    QLineEdit* m_minestratorServerId = nullptr;  ///< Поле ID сервера MineStrator
    QLineEdit* m_serverAddress = nullptr;        ///< Поле адреси для прямого підключення
    QLabel* m_minestratorStatusLabel = nullptr;  ///< Лейбл статусу MineStrator сервера
};
