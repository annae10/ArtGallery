#include "mainwindow.h"

#include <QApplication>

#include <openssl/ssl.h>

#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include "ui_mainwindow.h"

#include <QLabel>
#include <QPixmap>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QNetworkAccessManager>
#include "imageloader.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

void init_openssl() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl() {
    EVP_cleanup();
}

struct Token {
    std::string type;
    std::string value;
};

std::vector<Token> tokenize(const std::string& html) {
    std::vector<Token> tokens;
    size_t pos = 0;
    while (pos < html.size()) {
        if (html[pos] == '<') {
            size_t endPos = html.find('>', pos);
            if (endPos != std::string::npos) {
                tokens.push_back({"TAG", html.substr(pos, endPos - pos + 1)});
                pos = endPos + 1;
            } else {
                break;
            }
        } else {
            size_t endPos = html.find('<', pos);
            tokens.push_back({"TEXT", html.substr(pos, endPos - pos)});
            pos = endPos;
        }
    }
    return tokens;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    Ui::MainWindow ui;
    ui.setupUi(&w);

    QWidget *centralWidget = new QWidget(&w);
    w.setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);


    //----------title------------//
    std::string title = "Art gallery";
    QLabel *textLabel = new QLabel(QString::fromStdString(title));
    textLabel->setFixedWidth(350);
    textLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    textLabel->setStyleSheet("color: orange;");
    QFont font = textLabel->font();
    font.setPointSize(18);
    font.setBold(true);
    textLabel->setFont(font);
    textLabel->setWordWrap(true);
    mainLayout->addWidget(textLabel);
    //---------------------------//

    QScrollArea *scrollArea = new QScrollArea(&w);
    scrollArea->setFrameStyle(QFrame::NoFrame);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    ImageLoader loader(layout);


    init_openssl();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        std::cerr << "Unable to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
        return EXIT_FAILURE;
    }

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2),&wsaData)!=0)
    {
        std::cerr << "WSAStartup(MAKEWORD(2,2),&wsaData)!=0" << std::endl;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(443);

    inet_pton(AF_INET, "151.101.3.5", &server_addr.sin_addr); // https://edition.cnn.com/style/arts

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(sock);
        return EXIT_FAILURE;
    }

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL* ssl = SSL_new(ctx);
    if(ssl == NULL){
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        closesocket(sock);
        return EXIT_FAILURE;
    }

    const char* request = "GET /style/arts HTTP/1.0\r\nHost: edition.cnn.com\r\nConnection: close\r\n\r\n";
    SSL_write(ssl, request, strlen(request));

    char buffer[4096];

    std::string response;

    while(true){
        int bytes = SSL_read(ssl, buffer, sizeof(buffer) -1);
        if(bytes <= 0) break;
        buffer[bytes]=0;
        response += buffer;

    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(sock);
    SSL_CTX_free(ctx);


    std::vector<Token> tokens = tokenize(response);


        std::string header_tag {"<div class=\"container__field-links container_grid-4__field-links"};
        std::string tag_article {"<div data-uri=\"cms.cnn.com/_components/card/"};
        std::string tag_image {"<div\n  data-uri=\"cms.cnn.com/_components/image"};

        bool check=false;
        bool find_article = false;
        bool find_image = false;

        int count_articles = 0;

        std::string article_link = "https://edition.cnn.com";
        std::string image_link = "";
        std::string text = "";

        std::string header = "";
    for (const auto& token : tokens) {

        bool empty=true;
        for(auto i:token.value)
        {
            if(i!=' ' && i!='\n')
                empty=false;
        }
        if(empty)continue;

        if (token.value.substr(0, header_tag.length()) == header_tag)
        {
            check=true;
            continue;
        }
        article_link = "https://edition.cnn.com";


        if (check && token.value.substr(0, tag_article.length()) == tag_article)
        {
            find_article = true;


            int count_n=0;
            bool find_article_link=false;

            for(auto i:token.value)
            {
                if(i=='\n')count_n++;

                if(find_article_link && i=='"')
                {
                    find_article_link=false;break;
                }

                if(count_n==4 && i=='"')
                {
                    find_article_link=true;continue;
                }

                if(find_article_link)
                {
                    article_link+=i;
                }
            }
            std::cout<<"Article link: "<<article_link<<"\n";
        }
        if (check && token.value.substr(0, tag_image.length()) == tag_image)
        {
            find_image = true;
            image_link = "";

            int count_n=0;
            bool find_image_link=false;

            for(auto i:token.value)
            {
                if(i=='\n')count_n++;

                if(find_image_link && i=='?')
                {
                    find_image_link=false;break;
                }

                if(count_n==11 && i=='"')
                {
                    find_image_link=true;continue;
                }

                if(find_image_link)
                {
                    image_link+=i;
                }
            }
            std::cout<<"Image link: "<<image_link<<"\n";

        }

        if(check && find_article && find_image && token.type == "TEXT"&& count_articles<32)
        {
            text = token.value;

            if(count_articles%2==0)
                header = text;

            std::cout << "Article: " << text << std::endl;

            QString imageUrl = QString::fromStdString(image_link);
            QString titleStr = QString::fromStdString(header);
            QString textStr = QString::fromStdString(text);

            if(count_articles%2==1)
            loader.loadImage(imageUrl, textStr);

            count_articles++;
        }
    }

    cleanup_openssl();

    response.clear();

    contentWidget->setLayout(layout);
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);
    w.show();

    return a.exec();
}
