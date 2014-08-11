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

            if("fillin" == method)
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
                        sql = QString("INSERT INTO usrs (nickname, realname, gender, school, college, grade, title) VALUES ('%1', '%2', %3, '%4', '%5', %6, '%7')")
                                .arg(nickname)
                                .arg(realname)
                                .arg(gender)
                                .arg(school)
                                .arg(college)
                                .arg(grade)
                                .arg(title);

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

    }

    qDebug() << __TIME__ << "socket" << this->socketDescriptor() << "disconnected...";

    this->close();

//    this->disconnectFromHost();
//    this->waitForDisconnected();
}
