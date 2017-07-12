
#include "receiver.h"
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDataStream>
#include <iostream>

std::string receiver::get_random_file(void)
{
    static std::string path;
    if (path.empty())
    {
        path = "/dev/random";
        QFileInfo checkFile(QString::fromStdString(path));
        if (!checkFile.exists())
        {
            //static temp file persists until process end
            static QTemporaryFile temp_file;
            temp_file.open();
            path = temp_file.fileName().toStdString();
            {
                QDataStream stream(&temp_file);
                for (size_t i = 0; i < 1024*8; i++) stream << qint8(rand());
            }
            std::cout << "Created random file " << path << std::endl;
        }
    }
    return path;
}

std::string receiver::get_null_file(void)
{
    static std::string path;
    if (path.empty())
    {
        path = "/dev/null";
        QFileInfo checkFile(QString::fromStdString(path));
        if (!checkFile.exists())
        {
            //static temp file persists until process end
            static QTemporaryFile temp_file;
            temp_file.open();
            path = temp_file.fileName().toStdString();
            {
                QDataStream stream(&temp_file);
                for (size_t i = 0; i < 1024*8; i++) stream << qint8(0);
            }
            std::cout << "Created null file " << path << std::endl;
        }
    }
    return path;
}
