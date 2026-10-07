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

#include "MinestratorAPI.h"

#include <QJsonArray>
#include <QLoggingCategory>
#include <QNetworkReply>
#include <QTimer>

Q_LOGGING_CATEGORY(logMinestrator, "velix.minestrator", QtWarningMsg)

MineStratorAPI::MineStratorAPI(const QString& apiKey, const QString& serverId, QObject* parent)
    : QObject(parent)
    , m_apiKey(apiKey)
    , m_serverId(serverId)
    , m_network(new QNetworkAccessManager(this))
    , m_pollTimer(new QTimer(this))
{
    m_network->setTransferTimeout(10000);  // 10 секунд timeout

    connect(m_pollTimer, &QTimer::timeout, this, &MineStratorAPI::checkServerStatus);
}

MineStratorAPI::~MineStratorAPI()
{
    stopStatusPolling();
}

bool MineStratorAPI::hasCredentials() const
{
    return !m_apiKey.isEmpty() && !m_serverId.isEmpty();
}

QNetworkRequest MineStratorAPI::makeRequest(const QString& path) const
{
    QNetworkRequest request;
    request.setUrl(QUrl(QString("%1%2").arg(QString::fromLatin1(API_BASE), path)));

    // Авторизація: Bearer токен (офіційний метод MineStrator API)
    // API-ключ НІКОЛИ не логується
    request.setRawHeader("Authorization", QByteArray("Bearer ") + m_apiKey.toUtf8());
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Accept", "application/json");

    return request;
}

void MineStratorAPI::startServer()
{
    if (!hasCredentials()) {
        qCWarning(logMinestrator) << "startServer: немає облікових даних (API-ключ або ID сервера порожні)";
        emit startServerFinished(false, tr("Введіть API-ключ та ID сервера в налаштуваннях."));
        return;
    }

    // Захист від подвійного натискання
    if (m_startInProgress) {
        qCDebug(logMinestrator) << "startServer: запит вже виконується, ігноруємо";
        return;
    }
    m_startInProgress = true;

    // Офіційний endpoint MineStrator: PUT /server/{id}/poweraction
    // Body: {"poweraction": "start"}
    // Джерело: офіційна документація mine.sttr.io
    const QString path = QString("/server/%1/poweraction").arg(m_serverId);
    QNetworkRequest request = makeRequest(path);

    const QByteArray body = R"({"poweraction":"start"})";

    QNetworkReply* reply = m_network->put(request, body);
    reply->setParent(this);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_startInProgress = false;

        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QNetworkReply::NetworkError error = reply->error();

        if (error != QNetworkReply::NoError) {
            // ВАЖЛИВО: не логуємо API-ключ, лише код помилки
            qCWarning(logMinestrator) << "startServer: мережева помилка:" << error << "HTTP:" << statusCode;
            emit startServerFinished(false,
                tr("Помилка з'єднання з MineStrator API. Перевірте ваш API-ключ та ID сервера."));
            return;
        }

        // HTTP 204 або 200 — успіх для MineStrator poweraction
        if (statusCode == 200 || statusCode == 204) {
            qCDebug(logMinestrator) << "startServer: команда запуску надіслана успішно";
            emit startServerFinished(true, QString());
        } else if (statusCode == 401 || statusCode == 403) {
            qCWarning(logMinestrator) << "startServer: помилка авторизації, HTTP" << statusCode;
            emit startServerFinished(false, tr("Невірний API-ключ або недостатньо прав. Перевірте налаштування."));
        } else if (statusCode == 404) {
            qCWarning(logMinestrator) << "startServer: сервер не знайдений, HTTP" << statusCode;
            emit startServerFinished(false, tr("Сервер із таким ID не знайдений. Перевірте ID сервера в налаштуваннях."));
        } else {
            qCWarning(logMinestrator) << "startServer: несподіваний HTTP статус:" << statusCode;
            emit startServerFinished(false, tr("Помилка MineStrator API (HTTP %1).").arg(statusCode));
        }
    });
}

void MineStratorAPI::checkServerStatus()
{
    if (!hasCredentials()) {
        emit serverStatusChecked(ServerState::Error, tr("Введіть API-ключ та ID сервера в налаштуваннях."));
        return;
    }

    // Захист від паралельних запитів статусу
    if (m_statusInProgress) {
        return;
    }
    m_statusInProgress = true;

    // Офіційний endpoint MineStrator: GET /server/{id}/live
    // Відповідь: JSON з полем data.state ("online", "offline", тощо)
    const QString path = QString("/server/%1/live").arg(m_serverId);
    QNetworkRequest request = makeRequest(path);

    QNetworkReply* reply = m_network->get(request);
    reply->setParent(this);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        m_statusInProgress = false;

        const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QNetworkReply::NetworkError error = reply->error();

        if (error != QNetworkReply::NoError) {
            qCWarning(logMinestrator) << "checkServerStatus: мережева помилка:" << error;
            emit serverStatusChecked(ServerState::Error, tr("Помилка з'єднання з MineStrator API."));
            return;
        }

        if (statusCode == 401 || statusCode == 403) {
            emit serverStatusChecked(ServerState::Error, tr("Невірний API-ключ або недостатньо прав."));
            return;
        }

        if (statusCode == 404) {
            emit serverStatusChecked(ServerState::Error, tr("Сервер із таким ID не знайдений."));
            return;
        }

        const QByteArray responseData = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(responseData);

        if (doc.isNull() || !doc.isObject()) {
            qCWarning(logMinestrator) << "checkServerStatus: невалідна JSON відповідь";
            emit serverStatusChecked(ServerState::Error, tr("Невалідна відповідь від MineStrator API."));
            return;
        }

        // Офіційна структура відповіді MineStrator /server/{id}/live:
        // { "data": { "state": "online" | "offline" | ... } }
        const QJsonObject root = doc.object();
        const QJsonObject data = root.value("data").toObject();
        const QString state = data.value("state").toString().toLower();

        qCDebug(logMinestrator) << "checkServerStatus: стан сервера:" << state;

        ServerState serverState = ServerState::Unknown;
        if (state == "online") {
            serverState = ServerState::Online;
        } else if (state == "offline") {
            serverState = ServerState::Offline;
        } else if (state == "starting" || state == "launching") {
            serverState = ServerState::Starting;
        } else {
            serverState = ServerState::Unknown;
        }

        emit serverStatusChecked(serverState, QString());
    });
}

void MineStratorAPI::startStatusPolling(int intervalMs)
{
    if (m_pollTimer->isActive())
        m_pollTimer->stop();
    m_pollTimer->start(intervalMs);
    // Виконуємо першу перевірку одразу
    checkServerStatus();
}

void MineStratorAPI::stopStatusPolling()
{
    if (m_pollTimer->isActive())
        m_pollTimer->stop();
}

// static
QString MineStratorAPI::stateToString(ServerState state)
{
    switch (state) {
        case ServerState::Online:   return tr("Сервер онлайн");
        case ServerState::Offline:  return tr("Сервер офлайн");
        case ServerState::Starting: return tr("Сервер запускається...");
        case ServerState::Error:    return tr("Помилка перевірки статусу");
        case ServerState::Unknown:  return tr("Статус невідомий");
        default:                    return tr("Статус невідомий");
    }
}
