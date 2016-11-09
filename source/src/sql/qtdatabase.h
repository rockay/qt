#ifndef QTDATABASE_H
#define QTDATABASE_H

#include <QObject>

class QTDataBase: public QObject
{
    Q_OBJECT
public:
    static QTDataBase* getInstance()
    {
        if(m_instance==NULL)
            m_instance = new QTDataBase;
        return m_instance;
    }
public slots:
    void initDB(const QString &user_id);
    void createContact();
    void createConversations();

private:
    static QTDataBase* m_instance;
};

#endif // QTDATABASE_H
