/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "generalsettingwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/contentwidget.h"
#include "widgets/settingsgroup.h"
#include "../../../modules/mouse/widget/palmdetectsetting.h"
#include "widgets/dccslider.h"
#include "../../../modules/mouse/widget/doutestwidget.h"
#include "../../../modules/mouse/mousemodel.h"
#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

GeneralSettingWidget::GeneralSettingWidget(QWidget *parent) : QWidget(parent)
{
    m_baseSettingsGrp = new SettingsGroup;

    m_leftHand = new SwitchWidget(tr("Left Hand"));
    m_disInTyping = new SwitchWidget(tr("Disable the touchpad while typing"));
    m_scrollSpeedSlider = new TitledSliderItem(tr("Scroll Speed"));
    m_doubleSlider = new TitledSliderItem(tr("Double-click Speed"));
    m_doubleTest = new DouTestWidget;

    m_baseSettingsGrp->appendItem(m_leftHand);
    m_baseSettingsGrp->appendItem(m_disInTyping);
    m_baseSettingsGrp->appendItem(m_scrollSpeedSlider);
    m_baseSettingsGrp->appendItem(m_doubleSlider);
    m_baseSettingsGrp->appendItem(m_doubleTest);

    m_contentLayout = new QVBoxLayout();
    m_contentLayout->addWidget(m_baseSettingsGrp);
    setLayout(m_contentLayout);
    connect(m_leftHand, &SwitchWidget::checkedChanged, this, &GeneralSettingWidget::requestSetLeftHand);
    connect(m_disInTyping, &SwitchWidget::checkedChanged, this, &GeneralSettingWidget::requestSetDisTyping);
    connect(m_scrollSpeedSlider->slider(), &DCCSlider::valueChanged, this, &GeneralSettingWidget::requestScrollSpeed);
    connect(m_doubleSlider->slider(), &DCCSlider::valueChanged, this, &GeneralSettingWidget::requestSetDouClick);
}

GeneralSettingWidget::~GeneralSettingWidget()
{
    qDebug() << "destory GeneralSettingWidget";
    if (m_baseSettingsGrp) {
        delete m_baseSettingsGrp;
        m_baseSettingsGrp = nullptr;
    }
}

void GeneralSettingWidget::setModel(dcc::mouse::MouseModel *const model)
{
    m_mouseModel = model;

    connect(model, &MouseModel::leftHandStateChanged, m_leftHand, &SwitchWidget::setChecked);
    connect(model, &MouseModel::disIfTypingStateChanged, m_disInTyping, &SwitchWidget::setChecked);
    m_leftHand->setChecked(model->leftHandState());
    m_disInTyping->setChecked(model->disIfTyping());
}