#ifndef IMAGELOADERTASK_H
#define IMAGELOADERTASK_H


#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include <QHBoxLayout>
#include <QThreadPool>
#include <QRunnable>
#include <QObject>

class ImageLoaderTask : public QObject, public QRunnable {
    Q_OBJECT
public:
    ImageLoaderTask(const QString &url, const QString &text, QVBoxLayout *layout)
        : url(url), text(text),layout(layout) {}

    void run() override {
        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

        QObject::connect(reply, &QNetworkReply::finished, [reply, this]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray imageData = reply->readAll();
                emit imageLoaded(imageData, text);
            }
            reply->deleteLater();
        });

        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
    }

signals:
    void imageLoaded(const QByteArray &imageData, const QString &text);

private:
    QString url;
    QString text;
    QVBoxLayout *layout;
};

#endif // IMAGELOADERTASK_H
