/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_WIDGETS_TITLEDSLIDERITEM_H
#define DCC_WIDGETS_TITLEDSLIDERITEM_H

#include "settingsitem.h"

class NormalLabel;
class QSlider;
class QLabel;

namespace dcc {
namespace widgets {

class TitledSliderItem : public SettingsItem
{
    Q_OBJECT
public:
    TitledSliderItem(QString title, QWidget *parent = 0);

    Q_PROPERTY(QString leftIcon READ leftIcon WRITE setLeftIcon NOTIFY leftIconChanged)
    Q_PROPERTY(QString rightIcon READ rightIcon WRITE setRightIcon NOTIFY rightIconChanged)

    QSlider *slider() const;

    QString leftIcon() const;
    void setLeftIcon(const QString &leftIcon);

    QString rightIcon() const;
    void setRightIcon(const QString &rightIcon);

    QString valueLiteral() const;
    void setValueLiteral(const QString &valueLiteral);

signals:
    void leftIconChanged();
    void rightIconChanged();

private:
    NormalLabel *m_titleLabel;
    NormalLabel *m_valueLabel;
    QSlider *m_slider;
    QLabel *m_leftIconLabel;
    QLabel *m_rightIconLabel;

    QString m_leftIcon;
    QString m_rightIcon;
    QString m_valueLiteral;
};

} // namespace widgets
} // namespace dcc

#endif // DCC_WIDGETS_TITLEDSLIDERITEM_H
