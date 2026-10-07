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

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

/**
 * MineStratorAPI - асинхронний клієнт для офіційного MineStrator API
 *
 * Офіційний API MineStrator (https://mine.sttr.io):
 *   - Авторизація: Bearer токен у заголовку Authorization
 *   - Запуск сервера:  PUT /server/{id}/poweraction  Body: {"poweraction":"start"}
 *   - Статус сервера:  GET /server/{id}/live
 *                       Відповідь JSON: data.state = "online" | "offline" | ...
 *
 * API-ключ та ID сервера НІКОЛИ не зберігаються у вихідному коді.
 * Вони зчитуються з налаштувань застосунку під час виконання.
 *
 * Джерело: офіційна документація MineStrator API (panel.minestrator.com / mine.sttr.io)
 */
class MineStratorAPI : public QObject {
    Q_OBJECT

   public:
    enum class ServerState {
        Unknown,    ///< Статус ще не відомий
        Online,     ///< Сервер онлайн
        Offline,    ///< Сервер офлайн
        Starting,   ///< Сервер запускається
        Error,      ///< Помилка отримання статусу
    };
    Q_ENUM(ServerState)

    /**
     * @param apiKey   API-ключ (зчитується з налаштувань, не з коду)
     * @param serverId ID сервера MineStrator (зчитується з налаштувань)
     * @param parent   Qt parent object
     */
    explicit MineStratorAPI(const QString& apiKey, const QString& serverId, QObject* parent = nullptr);
    ~MineStratorAPI() override;

    /**
     * Асинхронно відправляє команду запуску сервера до MineStrator API.
     * Офіційний endpoint: PUT /server/{id}/poweraction
     * Body: {"poweraction": "start"}
     *
     * Емітує startServerFinished(success, errorMessage) після завершення.
     * Захист від подвійного натискання: повторний виклик ігнорується, якщо запит вже йде.
     * ВАЖЛИВО: не блокує UI-потік.
     */
    void startServer();

    /**
     * Асинхронно перевіряє статус сервера.
     * Офіційний endpoint: GET /server/{id}/live
     *
     * Емітує serverStatusChecked(state, errorMessage) після завершення.
     */
    void checkServerStatus();

    /**
     * Запускає автоматичний циклічний моніторинг статусу кожні intervalMs мс.
     * Зупинити через stopStatusPolling().
     */
    void startStatusPolling(int intervalMs = 5000);
    void stopStatusPolling();

    /** Повертає true якщо є і API-ключ, і ID сервера */
    bool hasCredentials() const;

    static QString stateToString(ServerState state);

   signals:
    /** Емітується після спроби запуску сервера */
    void startServerFinished(bool success, const QString& errorMessage);

    /** Емітується після перевірки статусу сервера */
    void serverStatusChecked(MineStratorAPI::ServerState state, const QString& errorMessage);

   private:
    QString m_apiKey;
    QString m_serverId;
    QNetworkAccessManager* m_network;
    QTimer* m_pollTimer;
    bool m_startInProgress = false;
    bool m_statusInProgress = false;

    // Офіційний базовий URL MineStrator API (підтверджено через panel.minestrator.com)
    static constexpr const char* API_BASE = "https://mine.sttr.io";

    /** Створює QNetworkRequest з правильними заголовками для MineStrator API */
    QNetworkRequest makeRequest(const QString& path) const;
};
