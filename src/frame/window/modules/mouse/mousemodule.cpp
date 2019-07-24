/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "mousemodule.h"
#include "mousewidget.h"
#include "../../../modules/mouse/mousemodel.h"
#include "../../../modules/mouse/mouseworker.h"
#include "widgets/contentwidget.h"
//#include "generalsettingwidget.h"
//#include "mousesettingwidget.h"
//#include "touchpadsettingwidget.h"
//#include "palmdetectsettingwidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::mouse;

MouseModule::MouseModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),
      m_model(nullptr),
      m_worker(nullptr),
      m_mouseWidget(nullptr),
      m_generalSettingWidget(nullptr),
      m_mouseSettingWidget(nullptr),
      m_touchpadSettingWidget(nullptr),
      m_palmdetectSettingWidget(nullptr)
{
}

void MouseModule::initialize()
{
    m_model  = new dcc::mouse::MouseModel;
    m_worker = new dcc::mouse::MouseWorker(m_model);
    m_model->moveToThread(qApp->thread());
    m_worker->moveToThread(qApp->thread());
}

void MouseModule::reset()
{
    m_worker->onDefaultReset();
}

QWidget *MouseModule::moduleWidget()
{
    if (!m_mouseWidget) {
        m_mouseWidget = new MouseWidget;
//        connect(m_mouseWidget, &MouseWidget::showGeneralSetting, this, &MouseModule::showGeneralSetting);
//        connect(m_mouseWidget, &MouseWidget::showMouseSetting, this, &MouseModule::showMouseSetting);
//        connect(m_mouseWidget, &MouseWidget::showTouchpadSetting, this, &MouseModule::showTouchpadSetting);
        // connect(m_mouseWidget, &MouseWidget::showPalmdetectSetting, this, &MouseModule::showPalmdetectSetting);
    }
    return m_mouseWidget;
}

void MouseModule::showGeneralSetting()
{
//    m_generalSettingWidget = new GeneralSettingWidget;
//    m_generalSettingWidget->setModel(m_model);
//    m_frameProxy->pushWidget(this, m_generalSettingWidget);
}

void MouseModule::showMouseSetting()
{
//    m_mouseSettingWidget = new MouseSettingWidget;
//    m_mouseSettingWidget->setModel(m_model);
//    m_frameProxy->pushWidget(this, m_mouseSettingWidget);
}

void MouseModule::showTouchpadSetting()
{
//    m_touchpadSettingWidget = new TouchPadSettingWidget;
//    m_touchpadSettingWidget->setModel(m_model);
//    m_frameProxy->pushWidget(this, m_touchpadSettingWidget);
}

void MouseModule::showPalmdetectSetting()
{
//    if (!m_palmdetectSettingWidget) {
//        m_palmdetectSettingWidget = new PalmDetectSettingWidget;
//        m_palmdetectSettingWidget->setModel(m_model);
//        m_frameProxy->pushWidget(this, m_palmdetectSettingWidget);
//    }
}

const QString MouseModule::name() const
{
    return QStringLiteral("mouse");
}

MouseModule::~MouseModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void MouseModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}