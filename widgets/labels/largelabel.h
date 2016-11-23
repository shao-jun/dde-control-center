#ifndef LARGELABEL_H
#define LARGELABEL_H

#include <QLabel>

class LargeLabel : public QLabel
{
    Q_OBJECT

public:
    explicit LargeLabel(QWidget *parent = nullptr);
    explicit LargeLabel(const QString &text, QWidget *parent = nullptr);
};

#endif // LARGELABEL_H
