#include "littleboxsocket.h"
#include "littleboxdbutil.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

LittleBoxSocket::LittleBoxSocket()
{
    database = new LittleBoxDBUtil();

    connect(this, SIGNAL(readyRead()), this, SLOT(parseRequest()), Qt::DirectConnection);
//    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()), Qt::DirectConnection);
}


void LittleBoxSocket::parseRequest()
{
    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "serving...";

    //this->waitForReadyRead(3000);

    //QByteArray request = this->readAll();

    QStringList lines;

    QString line;

    int length = 0;

    while (this->canReadLine())
    {
        line = this->readLine();

        qDebug() << line;

        lines.append(line);

        if(line.contains("content-length", Qt::CaseInsensitive))
        {
            length = line.split(':')[1].toInt();
        }

        if(line == "\r\n")
        {
            qDebug() << "break";

            break;
        }
    }

    QString body;

    while(true)
    {
        qDebug() << ".";

        if(this->bytesAvailable() == length)
        {
            qDebug() << "break";

            break;
        }
    }

    body = this->readAll();

    qDebug() << body;

    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "disconnected...";

    this->close();

    //============================================================================

//    QStringList components = QString(request).split(QRegExp("\r\n\r\n"));

//    QString headers = components[0];

//    qDebug() << headers;

//    QString body = components[1];

//    qDebug() << body;

    //============================================================================

//    QStringList lines = headers.split(QRegExp("\r\n"));

    //============================================================================

//    QStringList tokens = lines[0].split(' ');

//    if("POST" == tokens[0])
//    {
//        sendResponse(tokens[1].remove(0, 1), body);
//    }
//    else
//    {
//        sendResponse("display_error_msg", request);
//    }
}


bool LittleBoxSocket::authenticate(int uid, QString password)
{
    if(-1 == uid || password.isEmpty() || password.isNull())
    {
        return false;
    }
    else
    {

        QString sql = QString("SELECT uid FROM usrs WHERE uid = %1 AND password = '%2'").arg(uid).arg(password);

        return (1 == database->execute(sql).size()) ? true : false;
    }
}

void LittleBoxSocket::sendResponse(QString method, QString parameter)
{
    if(method.isEmpty() || method.isNull() || parameter.isEmpty() || parameter.isNull())
    {
        return;
    }

    QTextStream response(this);

    response.setAutoDetectUnicode(true);

    if("display_error_msg" == method)
    {
        response << "HTTP/1.1 400 Bad Request\r\n"
                 << "content-type: text/html; charset=\"utf-8\"\r\n"
                 << "content-length:" << 12 << "\r\n"
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

                    QSqlQuery query = database->execute(sql);

                    if(0 == query.size())
                    {
                        sql = QString("INSERT INTO usrs (email, password) VALUES ('%1', '%2')").arg(email).arg(password);

                        database->execute(sql);

                        sql = QString("SELECT uid FROM usrs WHERE email = '%1'").arg(email);

                        query = database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

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

                    QString description = items["description"].toString();

                    int gender = items["gender"].toInt();

                    int student_number = items["student_number"].toInt();

                    int age = items["age"].toInt();

                    QString school = items["school"].toString();

                    QString college = items["college"].toString();

                    int grade = items["grade"].toInt();

                    QString title = items["title"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE usrs SET nickname = '%1',    \
                                                       realname = '%2',    \
                                                       description = '%3', \
                                                       age = %4,		   \
                                                       gender = %5, 	   \
                                                       school = '%6', 	   \
                                                       college = '%7', 	   \
                                                       grade = %8, 		   \
                                                       student_number = %9 \
                                                       title = '%10' 	   \
                                                       WHERE uid = %11")
                                                                        .arg(nickname)
                                                                        .arg(realname)
                                                                        .arg(description)
                                                                        .arg(age)
                                                                        .arg(gender)
                                                                        .arg(school)
                                                                        .arg(college)
                                                                        .arg(grade)
                                                                        .arg(student_number)
                                                                        .arg(title)
                                                                        .arg(uid);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SElECT mid FROM msgs WHERE pid IN (SELECT pid FROM pois WHERE ((%1 - longitude) * (%2 - longitude) + (%3 - latitude) * (%4 - latitude)) < 10000) LIMIT %5, %6")
                                                                                                                                                                            .arg(longitude)
                                                                                                                                                                            .arg(longitude)
                                                                                                                                                                            .arg(latitude)
                                                                                                                                                                            .arg(latitude)
                                                                                                                                                                            .arg(25 * offset)
                                                                                                                                                                            .arg(25 * (offset + 1));

                        query = database->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            QSqlQuery tmp = database->execute(sql);

                            tmp.next();

                            QString usrname = tmp.value(0).toString();

                            sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            QString poiname = tmp.value(0).toString();

                            sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int mid = tmp.value(0).toInt();

                            QString content = tmp.value(1).toString();

                            QString timestamp = tmp.value(2).toString();

                            sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int count_of_like = tmp.value(0).toInt();

                            sql = QString("SELECT COUNT(rid) FROM reviews_of_msg WHERE mid = %1").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(mid);

                        query = database->execute(sql);

                        query.next();

                        QString usrname = query.value(0).toString();

                        sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(mid);

                        query = database->execute(sql);

                        query.next();

                        QString poiname = query.value(0).toString();

                        sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1").arg(mid);

                        query = database->execute(sql);

                        query.next();

                        int mid = query.value(0).toInt();

                        QString content = query.value(1).toString();

                        QString timestamp = query.value(2).toString();

                        sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(mid);

                        query = database->execute(sql);

                        query.next();

                        int count_of_like = query.value(0).toInt();

                        sql = QString("SELECT content, timestamp FROM reviews_of_msg WHERE mid = %1").arg(mid);

                        query = database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO reviews_of_msg (uid, mid, content) VALUES (%1, %2, '%3')").arg(uid).arg(mid).arg(content);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO reviews_of_poi (uid, pid, content) VALUES (%1, %2, '%3')").arg(uid).arg(pid).arg(content);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        QString sql = QString("SELECT mid, content FROM msgs WHERE uid = %1").arg(uid);

                        query = database->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            int mid = query.value(0).toInt();

                            QString content = query.value(1).toString();

                            QJsonObject msg;

                            msg.insert("mid", mid);
                            msg.insert("content", content);

                            sql = QString("SELECT rid, reviews_of_msg.uid, nickname, content, timestamp FROM reviews_of_msg, usrs WHERE mid = %1 AND reviews_of_msg.uid = usrs.uid").arg(mid);

                            QSqlQuery tmp = database->execute(sql);

                            QJsonArray reviews;

                            while(tmp.next())
                            {
                                QJsonObject review;

                                review.insert("rid", tmp.value(0).toInt());
                                review.insert("uid", tmp.value(1).toInt());
                                review.insert("nickname", tmp.value(2).toString());
                                review.insert("content", tmp.value(3).toString());
                                review.insert("timestamp", tmp.value(4).toString());

                                reviews.append(review);
                            }

                            msg.insert("reviews", reviews);

                            msgs.append(msg);
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

            if("delete_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    int pid = items["pid"].toInt();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE pois SET alive = 0 WHERE pid = %1").arg(pid);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE msgs SET alive = 0 WHERE mid = %1").arg(mid);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE likes_of_msg SET alive = 0 WHERE mid = %1 AND uid = %2").arg(mid).arg(uid);

                        database->execute(sql);

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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE likes_of_poi SET alive = 0 WHERE pid = %1 AND uid = %2").arg(pid).arg(uid);

                        database->execute(sql);

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

            if("search_msgs" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString keyword = items["keyword"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT mid FROM msgs WHERE content LIKE '%%1%'").arg(keyword);

                        query = database->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            QSqlQuery tmp = database->execute(sql);

                            tmp.next();

                            QString usrname = tmp.value(0).toString();

                            sql = QString("SELECT name FROM pois WHERE pid = (SELECT pid FROM msgs WHERE mid = %1)").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            QString poiname = tmp.value(0).toString();

                            sql = QString("SELECT mid, content, timestamp FROM msgs WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int mid = tmp.value(0).toInt();

                            QString content = tmp.value(0).toString();

                            QString timestamp = tmp.value(1).toString();

                            sql = QString("SELECT COUNT(lid) FROM likes_of_msg WHERE mid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int count_of_like = tmp.value(0).toInt();

                            sql = QString("SELECT COUNT(rid) FROM reviews_of_msg WHERE mid = %1").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

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

            if("search_pois" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString keyword = items["keyword"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT pid FROM pois WHERE description LIKE '%%1%'").arg(keyword);

                        query = database->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            sql = QString("SELECT nickname FROM usrs WHERE uid = (SELECT uid FROM pois WHERE pid = %1)").arg(query.value(0).toInt());

                            QSqlQuery tmp = database->execute(sql);

                            tmp.next();

                            QString usrname = tmp.value(0).toString();

                            sql = QString("SELECT pid, name, description, timestamp FROM pois WHERE pid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int pid = tmp.value(0).toInt();

                            QString poiname = tmp.value(1).toString();

                            QString description = tmp.value(2).toString();

                            QString timestamp = tmp.value(3).toString();

                            sql = QString("SELECT COUNT(lid) FROM likes_of_poi WHERE pid = %1 ").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int count_of_like = tmp.value(0).toInt();

                            sql = QString("SELECT COUNT(rid) FROM reviews_of_poi WHERE pid = %1").arg(query.value(0).toInt());

                            tmp = database->execute(sql);

                            tmp.next();

                            int count_of_review = tmp.value(0).toInt();

                            QJsonObject json;

                            json.insert("pid", pid);

                            json.insert("usrname", usrname);

                            json.insert("poiname", poiname);

                            json.insert("description", description);

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

            if("add_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString name = items["name"].toString();

                    QString description = items["description"].toString();

                    double longitude = items["longitude"].toDouble();

                    double latitude = items["latitude"].toDouble();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO pois (uid, name, description, longitude, latitude) VALUES (%1, '%2', '%3', %4, %5)").arg(uid).arg(name).arg(description).arg(longitude).arg(latitude);

                        database->execute(sql);

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

            if("change_password" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString new_password = items["new_password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("UPDATE usrs SET password = %1 WHERE uid = %2").arg(new_password).arg(uid);

                        database->execute(sql);

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

            if("likes" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        QString sql = QString("SELECT mid, content FROM msgs WHERE uid = %1").arg(uid);

                        query = database->execute(sql);

                        QJsonArray msgs;

                        while(query.next())
                        {
                            int mid = query.value(0).toInt();

                            QString content = query.value(1).toString();

                            QJsonObject msg;

                            msg.insert("mid", mid);
                            msg.insert("content", content);

                            sql = QString("SELECT lid, likes_of_msg.uid, nickname FROM likes_of_msg, usrs WHERE mid = %1 AND likes_of_msg.uid = usrs.uid").arg(mid);

                            QSqlQuery tmp = database->execute(sql);

                            QJsonArray likes;

                            while(tmp.next())
                            {
                                QJsonObject like;

                                like.insert("lid", tmp.value(0).toInt());
                                like.insert("uid", tmp.value(1).toInt());
                                like.insert("nickname", tmp.value(2).toString());

                                likes.append(like);
                            }

                            msg.insert("likes", likes);

                            msgs.append(msg);
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

            if("show_pois" == method)
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

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT pid, pois.uid, usrs.nickname, name, pois.description, timestamp, longitude, latitude FROM pois, usrs\
                                       WHERE (((%1 - longitude) * (%2 - longitude) + (%3 - latitude) * (%4 - latitude)) < 10000) AND pois.uid = usrs.uid LIMIT %5, %6")
                                                                                                                                                                 .arg(longitude)
                                                                                                                                                                 .arg(longitude)
                                                                                                                                                                 .arg(latitude)
                                                                                                                                                                 .arg(latitude)
                                                                                                                                                                 .arg(25 * offset)
                                                                                                                                                                 .arg(25 * (offset + 1));

                        query = database->execute(sql);

                        QJsonArray pois;

                        while(query.next())
                        {
                            int pid = query.value(0).toInt();

                            int uid = query.value(1).toInt();

                            QString usrname = query.value(2).toString();

                            QString poiname = query.value(3).toString();

                            QString description = query.value(4).toString();

                            QString timestamp = query.value(5).toString();

                            longitude = query.value(6).toDouble();

                            latitude = query.value(7).toDouble();

                            QJsonObject poi;

                            poi.insert("pid", pid);

                            poi.insert("uid", uid);

                            poi.insert("usrname", usrname);

                            poi.insert("poiname", poiname);

                            poi.insert("description", description);

                            poi.insert("timestamp", timestamp);

                            poi.insert("longitude", longitude);

                            poi.insert("latitude", latitude);

                            pois.append(poi);
                        }

                        QJsonDocument doc = QJsonDocument(pois);

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

            if("show_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    int pid = items["pid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT pois.uid, usrs.nickname, name, pois.description, timestamp, longitude, latitude FROM pois, usrs WHERE pid = %1 AND pois.uid = usrs.uid").arg(pid);

                        query = database->execute(sql);

                        query.next();

                        QJsonObject poi;

                        poi.insert("pid", pid);
                        poi.insert("uid", query.value(0).toInt());
                        poi.insert("usrname", query.value(1).toString());
                        poi.insert("poiname", query.value(2).toString());
                        poi.insert("description", query.value(3).toString());
                        poi.insert("timestamp", query.value(4).toString());
                        poi.insert("longitude", query.value(5).toDouble());
                        poi.insert("latitude", query.value(6).toDouble());

                        QJsonDocument doc = QJsonDocument(poi);

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

            if("like_on_poi" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    int pid = items["pid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO likes_of_poi (pid, uid) VALUES (%1, %2)").arg(pid).arg(uid);

                        query = database->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":%1}").arg(uid);
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

            if("like_on_msg" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    int mid = items["mid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("INSERT INTO likes_of_msg (mid, uid) VALUES (%1, %2)").arg(mid).arg(uid);

                        query = database->execute(sql);

                        response << "HTTP/1.1 200 OK\r\n"
                                 << "content-type: application/json; charset=\"utf-8\"\r\n"
                                 //<< "content-length:" << "\r\n"
                                 << "\r\n"
                                 << QString("{\"status\":\"failed\",\"uid\":%1}").arg(uid);
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

            if("profile" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT * FROM usrs WHERE uid = %1").arg(uid);

                        query = database->execute(sql);

                        query.next();

                        QJsonObject usr;

                        usr.insert("uid", query.value(0).toInt());
                        usr.insert("email", query.value(1).toString());
                        usr.insert("avatar", query.value(2).toString());
                        usr.insert("nickname", query.value(3).toString());
                        usr.insert("realname", query.value(5).toString());
                        usr.insert("gender", query.value(6).toInt());
                        usr.insert("school", query.value(7).toString());
                        usr.insert("college", query.value(8).toString());
                        usr.insert("grade", query.value(9).toString());
                        usr.insert("title", query.value(10).toString());
                        usr.insert("age", query.value(11).toInt());
                        usr.insert("description", query.value(12).toString());
                        usr.insert("student_number", query.value(13).toInt());

                        QJsonDocument doc = QJsonDocument(usr);

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

            if("my_pois" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    if(1 == query.size())
                    {
                        sql = QString("SELECT pid, name, description, timestamp, longitude, latitude FROM pois WHERE uid = %1").arg(uid);

                        query = database->execute(sql);

                        QJsonArray pois;

                        while(query.next())
                        {
                            QJsonObject poi;

                            poi.insert("pid", query.value(0).toInt());
                            poi.insert("name", query.value(1).toString());
                            poi.insert("description", query.value(2).toString());
                            poi.insert("timestamp", query.value(3).toString());
                            poi.insert("longitude", query.value(4).toDouble());
                            poi.insert("latitude", query.value(5).toDouble());

                            pois.append(poi);
                        }

                        QJsonDocument doc = QJsonDocument(pois);

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

            if("my_msgs" == method)
            {
                if(doc.isObject())
                {
                    QJsonObject items = doc.object();

                    int uid = items["uid"].toInt();

                    QString password = items["password"].toString();

                    QString sql = QString("SELECT uid FROM usrs WHERE uid = '%1' AND password = '%2'").arg(uid).arg(password);

                    QSqlQuery query = database->execute(sql);

                    QJsonArray msgs;

                    if(1 == query.size())
                    {
                        sql = QString("SELECT mid, msgs.pid, pois.name, content, msgs.timestamp FROM msgs, pois WHERE msgs.uid = %1 AND msgs.uid = pois.pid").arg(uid);

                        query = database->execute(sql);

                        while(query.next())
                        {
                            QJsonObject msg;

                            msg.insert("mid", query.value(0).toInt());
                            msg.insert("pid", query.value(1).toInt());
                            msg.insert("poiname", query.value(2).toString());
                            msg.insert("content", query.value(3).toString());
                            msg.insert("timestamp", query.value(4).toString());

                            msgs.append(msg);
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

            if("pics" == method)
            {
                qDebug() << parameter;
            }
        }
        else
        {
            qDebug() << __TIME__ << "parse error" << parseStatus.error << parseStatus.errorString();

            qDebug() << parameter;
        }  
    }

    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "disconnected...";

    this->close();

//    this->disconnectFromHost();
//    this->waitForDisconnected();
}

void LittleBoxSocket::dealWithSignUP(QJsonObject parameters)
{
    QString email = parameters["email"].toString();
    QString password = parameters["password"].toString();

    QString sql = QString("SElECT uid FROM usrs WHERE email = %1").arg(email);

    QSqlQuery query = database->execute(sql);

    if(0 == query.size())
    {
        sql = QString("INSERT INTO usrs (email, password) VALUES ('%1', '%2')").arg(email).arg(password);

        query = database->execute(sql);

        query.lastInsertId();
    }
    else
    {

    }
}
