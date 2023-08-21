#include <QCoreApplication>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QJsonObject>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Read CSV file
    QFile file("C:\\Users\\14003275\\Postman\\files\\california-housing-test.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file";
        return -1;
    }
    QByteArray fileData = file.readAll();
    file.close();

    // Create a network manager
    QNetworkAccessManager manager;

    // Create a multi-part HTTP request
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // Add the CSV file as a part
    QHttpPart csvPart;
    csvPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"california-housing-test.csv\""));
    csvPart.setBody(fileData);
    csvPart.setRawHeader("Content-Type", "text/csv"); // Set the content type here
    multiPart->append(csvPart);


    // Create a network request
    QNetworkRequest request(QUrl("http://192.168.205.131:5000/upload"));

    // Send the request
    QNetworkReply *reply = manager.post(request, multiPart);
    multiPart->setParent(reply); // Ownership transferred to the reply

    // Handle the reply
    QObject::connect(reply, &QNetworkReply::finished, [&]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            qDebug() << "Response:" << response;
        } else {
            qDebug() << "Error:" << reply->errorString();
            QByteArray errorData = reply->readAll();
            QJsonDocument errJsonDoc = QJsonDocument::fromJson(errorData);
            QJsonObject errJsonObj = errJsonDoc.object();
            qDebug() << "Error:" << errJsonObj.value("error").toString();
        }

        reply->deleteLater();
        a.quit();
    });

    return a.exec();
}
