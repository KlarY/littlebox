#include "littleboxsocket.h"
#include "littleboxdbutil.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

LittleBoxSocket::LittleBoxSocket()
{
    dbWorker = new LittleBoxDBUtil();

    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
//    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}

void LittleBoxSocket::parseRequest()
{
    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "serving...";

    this->waitForReadyRead(3000);

    QByteArray request = this->readAll();

    //============================================================================

    QStringList components = QString(request).split(QRegExp("\r\n\r\n"));

    QString headers = components[0];

    QString body = components[1];

    //============================================================================

    QStringList lines = headers.split(QRegExp("\r\n"));

    //============================================================================

    QStringList tokens = lines[0].split(' ');

    if("POST" == tokens[0])
    {
        sendResponse(tokens[1].remove(0, 1), body);
    }
    else
    {
        sendResponse("display_error_msg", request);
    }
}

void LittleBoxSocket::sendResponse(QString method, QString parameter)
{
    QTextStream response(this);

    response.setAutoDetectUnicode(true);

    if("display_error_msg" == method)
    {
        response << "HTTP/1.1 400 Bad Request\r\n"
                 << "content-type: text/html; charset=\"utf-8\"\r\n"
                 //<< "content-length:" << "\r\n"
                 << "\r\n"
                 << "Bad Request";
    }
    else
    {
        QJsonParseError parseStatus;

        QJsonDocument doc = QJsonDocument::fromJson(parameter.toUtf8(), &parseStatus);

        if(parseStatus.error == QJsonParseError::NoError)
        {
            if("signup" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject argvs= doc.object();

                    QString email = argvs["email"].toString();

                    QString password = argvs["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE email = '%1'").arg(email);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(0 == query.size())
                    {
                        sql = QString("INSERT INTO usrs (email, password) VALUES ('%1', '%2')").arg(email).arg(password);

                        dbWorker->execute(sql);

                        sql = QString("SELECT uid FROM usrs WHERE email = '%1'").arg(email);

                        query = dbWorker->execute(sql);

                        query.next();

                        int uid = query.value(0).toInt();

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        query.next();

                        int uid = query.value(0).toInt();

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":%1}").arg(uid);
                    }
                }
            }

            if("signin" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject argvs= doc.object();

                    QString email = argvs["email"].toString();

                    QString password = argvs["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE email = '%1' AND password = '%2'").arg(email).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
//                        sql = QString("INSERT INTO usrs (email, password) VALUES ('%1', '%2')").arg(email).arg(password);

//                        dbWorker->execute(sql);

//                        sql = QString("SELECT uid FROM usrs WHERE email = '%1'").arg(email);

//                        query = dbWorker->execute(sql);

                        query.next();

                        int uid = query.value(0).toInt();

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
//                        query.next();

//                        QString uid = query.value(0).toString();

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("update_personal_info" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString nickname = items["nickname"].toString();

                    QString password = items["password"].toString();

                    QString realname = items["realname"].toString();

                    int gender = items["gender"].toInt();

                    QString school = items["school"].toString();

                    QString college = items["college"].toString();

                    int grade = items["grade"].toInt();

                    QString title = items["title"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE usrs SET nickname = '%1', \
                                                       realname = '%2', \
                                                       gender = %3, 	\
                                                       school = '%4', 	\
                                                       college = '%5', 	\
                                                       grade = %6, 		\
                                                       title = '%7' 	\
                                                       WHERE uid = %8")
                                                                        .arg(nickname)
                                                                        .arg(realname)
                                                                        .arg(gender)
                                                                        .arg(school)
                                                                        .arg(college)
                                                                        .arg(grade)
                                                                        .arg(title)
                                                                        .arg(uid);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("index" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int offset = items["offset"].toInt();

                    double longitude = items["longitude"].toDouble();

                    double latitude = items["latitude"].toDouble();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SElECT mid FROM msgs WHERE pid IN (SELECT pid FROM pois WHERE ((%1 - longitude) * (%2 - longitude) + (%3 - latitude) * (%4 - latitude)) < 10000)").arg(longitude).arg(longitude).arg(latitude).arg(latitude);

                        query = dbWorker->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            QSqlQuery tmp = dbWorker->execute(sql);

                            tmp.next();

                            QString usrname = tmp.value(0).toString();

                            sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            tmp = dbWorker->execute(sql);

                            tmp.next();

                            QString poiname = tmp.value(0).toString();

                            sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = dbWorker->execute(sql);

                            tmp.next();

                            int mid = tmp.value(0).toInt();

                            QString content = tmp.value(1).toString();

                            QString timestamp = tmp.value(2).toString();

                            sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = dbWorker->execute(sql);

                            tmp.next();

                            int count_of_like = tmp.value(0).toInt();

                            sql = QString("SELECT COUNT(rid) FROM reviews_of_msg WHERE mid = %1").arg(query.value(0).toInt());

                            tmp = dbWorker->execute(sql);

                            tmp.next();

                            int count_of_review = tmp.value(0).toInt();

                            QJsonObject json;

                            json.insert("mid", mid);

                            json.insert("usrname", usrname);

                            json.insert("poiname", poiname);

                            json.insert("content", content);

                            json.insert("timestamp", timestamp);

                            json.insert("count_of_like", count_of_like);

                            json.insert("count_of_review", count_of_review);

                            msgs.append(json);
                        }

                        QJsonDocument doc = QJsonDocument(msgs);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << doc.toJson();
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("msg_details" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int mid = items["mid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(mid);

                        query = dbWorker->execute(sql);

                        query.next();

                        QString usrname = query.value(0).toString();

                        sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(mid);

                        query = dbWorker->execute(sql);

                        query.next();

                        QString poiname = query.value(0).toString();

                        sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1").arg(mid);

                        query = dbWorker->execute(sql);

                        query.next();

                        int mid = query.value(0).toInt();

                        QString content = query.value(1).toString();

                        QString timestamp = query.value(2).toString();

                        sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(mid);

                        query = dbWorker->execute(sql);

                        query.next();

                        int count_of_like = query.value(0).toInt();

                        sql = QString("SELECT content, timestamp FROM reviews_of_msg WHERE mid = %1").arg(mid);

                        query = dbWorker->execute(sql);

                        QJsonArray reviews;

                        while(query.next())
                        {
                            QJsonObject tmp;

                            tmp.insert("content", query.value(0).toString());

                            tmp.insert("timestamp", query.value(1).toString());

                            reviews.append(tmp);
                        }

                        QJsonObject json;

                        json.insert("mid", mid);

                        json.insert("usrname", usrname);

                        json.insert("poiname", poiname);

                        json.insert("content", content);

                        json.insert("timestamp", timestamp);

                        json.insert("count_of_like", count_of_like);

                        json.insert("reviews", reviews);

                        QJsonDocument doc = QJsonDocument(json);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << doc.toJson();
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("comment_on_msg" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int mid = items["mid"].toInt();

                    QString content = items["content"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO reviews_of_msg (uid, mid, content) VALUES (%1, %2, '%3')").arg(uid).arg(mid).arg(content);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("comment_on_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int pid = items["pid"].toInt();

                    QString content = items["content"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO reviews_of_poi (uid, pid, content) VALUES (%1, %2, '%3')").arg(uid).arg(pid).arg(content);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("comments" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        QString sql = QString("SELECT content, timestamp");
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("delete_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int pid = items["pid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE pois SET alive = 0 WHERE pid = %1").arg(pid);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("delete_msg" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int mid = items["mid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE msgs SET alive = 0 WHERE mid = %1").arg(mid);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("cancel_like_msg" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int mid = items["mid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE likes_of_msg SET alive = 0 WHERE mid = %1 AND uid = %2").arg(mid).arg(uid);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }

            if("cancel_like_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int pid = items["pid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = dbWorker->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE likes_of_poi SET alive = 0 WHERE pid = %1 AND uid = %2").arg(pid).arg(uid);

                        dbWorker->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"success\",\"uid\":%1}").arg(uid);
                    }
                    else
                    {
                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":-1}");
                    }
                }
            }
        }
        else
        {
            qDebug() << __TIME__ << "parse error" << parseStatus.error << parseStatus.errorString();

            qDebug() << parameter;
        }

        if("search_msgs" == method)
        {
            if(doc.isObject())
            {
                QJsonObject items = doc.object();

                int uid = items["uid"].toInt();

                QString password = items["password"].toString();

                QString keyword = items["keyword"].toString();

                QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                QSqlQuery query = dbWorker->execute(sql);

                if(1 == query.size())
                {
                    sql = QString("SELECT mid FROM msgs WHERE content LIKE '%%1%'").arg(keyword);

                    query = dbWorker->execute(sql);

                    QJsonArray msgs;

                    while(query.next())
                    {
                        sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                        QSqlQuery tmp = dbWorker->execute(sql);

                        tmp.next();

                        QString usrname = tmp.value(0).toString();

                        sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                        tmp = dbWorker->execute(sql);

                        tmp.next();

                        QString poiname = tmp.value(0).toString();

                        sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1 ").arg(query.value(0).toInt());

                        tmp = dbWorker->execute(sql);

                        tmp.next();

                        int mid = tmp.value(0).toInt();

                        QString content = tmp.value(0).toString();

                        QString timestamp = tmp.value(1).toString();

                        sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(query.value(0).toInt());

                        tmp = dbWorker->execute(sql);

                        tmp.next();

                        int count_of_like = tmp.value(0).toInt();

                        sql = QString("SELECT COUNT(rid) FROM reviews_of_msg WHERE mid = %1").arg(query.value(0).toInt());

                        tmp = dbWorker->execute(sql);

                        tmp.next();

                        int count_of_review = tmp.value(0).toInt();

                        QJsonObject json;

                        json.insert("mid", mid);

                        json.insert("usrname", usrname);

                        json.insert("poiname", poiname);

                        json.insert("content", content);

                        json.insert("timestamp", timestamp);

                        json.insert("count_of_like", count_of_like);

                        json.insert("count_of_review", count_of_review);

                        msgs.append(json);
                    }

                    QJsonDocument doc = QJsonDocument(msgs);

                    response << "HTTP/1.1 200 OK\r\n"
                             << "content-type: application/json; charset=\"utf-8\"\r\n"
                             //<< "content-length:" << "\r\n"
                             << "\r\n"
                             << doc.toJson();
                }
                else
                {
                    response << "HTTP/1.1 200 OK\r\n"
                             << "content-type: application/json; charset=\"utf-8\"\r\n"
                             //<< "content-length:" << "\r\n"
                             << "\r\n"
                             << QString("{\"status\":\"failed\",\"uid\":-1}");
                }
            }
        }
    }

    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "disconnected...";

    this->close();

//    this->disconnectFromHost();
//    this->waitForDisconnected();
}
