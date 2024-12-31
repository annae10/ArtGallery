#ifndef CONTENTLOADER_H
#define CONTENTLOADER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QPixmap>
#include <QLabel>

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
#include <iostream>
#include <QList>

class ContentLoader : public QObject {
    Q_OBJECT
public:
    ContentLoader(QVBoxLayout *layout)
        : layout(layout) {
        manager = new QNetworkAccessManager(this);
        connect(manager, &QNetworkAccessManager::finished,
                this, &ContentLoader::onImageLoaded);
    }

    void loadImage(const QString &url, const QString &title, const QString &text){

        manager->get(QNetworkRequest(QUrl(url)));
        pendingRequests.append({url, title, text});
        std::cout<<"Image loader received text:"<<text.toStdString()<<"\n";
    }

private slots:
    void onImageLoaded(QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());

            auto requestInfo = pendingRequests.takeFirst();
            QString url = requestInfo.url;
            QString title = requestInfo.title;
            QString text = requestInfo.text;


            QHBoxLayout *itemLayout = new QHBoxLayout();

            QVBoxLayout *textLayout = new QVBoxLayout();

            QLabel *imageLabel = new QLabel();
            QSize imgSize(150,150);
            imageLabel->setPixmap(pixmap.scaled(imgSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            imageLabel->setFixedWidth(150);
            imageLabel->setFixedHeight(150);

            QLabel *titleLabel = new QLabel(title);
            titleLabel->setStyleSheet("color: orange;");
            QFont font = titleLabel->font();
            font.setPointSize(14);
            font.setBold(true);
            titleLabel->setFont(font);
            titleLabel->setFixedWidth(350);
            titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            titleLabel->setWordWrap(true);

            QLabel *textLabel = new QLabel(text);
            textLabel->setFixedWidth(350);
            textLabel->setAlignment(Qt::AlignLeft);
            textLabel->setWordWrap(true);

            textLayout->addWidget(titleLabel);
            textLayout->addWidget(textLabel);

            itemLayout->addWidget(imageLabel);
            itemLayout->addLayout(textLayout);

            layout->addLayout(itemLayout);
            std::cout<<"Image loader added text:"<<text.toStdString()<<"\n";
        }
        reply->deleteLater();
    }

private:
    struct RequestInfo {
        QString url;
        QString title;
        QString text;
    };
    QVBoxLayout *layout;
    QNetworkAccessManager *manager;
    QList<RequestInfo> pendingRequests;
};

#endif // CONTENTLOADER_H
