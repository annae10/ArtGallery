#ifndef IMAGELOADER_H
#define IMAGELOADER_H
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
#include <QThreadPool>
#include <QRunnable>
#include <QObject>
#include <QFont>
#include "imageloadertask.h"

class ImageLoader : public QObject {
    Q_OBJECT
public:
    explicit ImageLoader(QVBoxLayout *layout) : layout(layout) {}

    void loadImage(const QString &url, const QString &text) {
        ImageLoaderTask *task = new ImageLoaderTask(url, text, layout);
        connect(task, &ImageLoaderTask::imageLoaded,
                this, &ImageLoader::onImageLoaded);
        QThreadPool::globalInstance()->start(task);
    }

private slots:
    void onImageLoaded(const QByteArray &imageData, const QString &text) {
        QPixmap pixmap;
        pixmap.loadFromData(imageData);

        // Создаем новый элемент списка
        QHBoxLayout *itemLayout = new QHBoxLayout();
        QLabel *imageLabel = new QLabel();

        QSize imgSize(150,150);
        imageLabel->setPixmap(pixmap.scaled(imgSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        imageLabel->setFixedWidth(150);
        imageLabel->setFixedHeight(150);


        QLabel *textLabel = new QLabel("Loaded Image");
        textLabel->setText(text);
        textLabel->setFixedWidth(350);
        textLabel->setAlignment(Qt::AlignLeft);
        textLabel->setWordWrap(true);
        textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QFont font = textLabel->font();
        font.setPointSize(14);
        textLabel->setFont(font);

        itemLayout->addWidget(imageLabel);
        itemLayout->addWidget(textLabel);

        layout->addLayout(itemLayout);
    }

private:
    QVBoxLayout *layout;
};

#endif // IMAGELOADER_H
