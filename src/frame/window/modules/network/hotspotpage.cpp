/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "hotspotpage.h"
#include "connectionhotspoteditpage.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

#include <DFloatingButton>

#include <networkmodel.h>
#include <wirelessdevice.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

using namespace dde::network;

namespace DCC_NAMESPACE {

using namespace dcc::widgets;

namespace network {

const QString defaultHotspotName()
{
    return getlogin();
}

HotspotDeviceWidget::HotspotDeviceWidget(WirelessDevice *wdev, bool showcreatebtn, QWidget *parent)
    : QWidget(parent)
    , m_wdev(wdev)
    , m_hotspotSwitch(new SwitchWidget)
    , m_createBtn(new QPushButton)
    , m_refreshActiveTimer(new QTimer)
    , m_lvprofiles(new DListView)
    , m_modelprofiles(new QStandardItemModel)
{
    Q_ASSERT(m_wdev->supportHotspot());

    m_lvprofiles->setModel(m_modelprofiles);

    m_hotspotSwitch->setTitle(tr("Hotspot"));
    m_createBtn->setText(tr("Add Settings"));
    m_createBtn->setVisible(showcreatebtn);

    m_refreshActiveTimer->setInterval(300);
    m_refreshActiveTimer->setSingleShot(true);

    SettingsGroup *sgrp = new SettingsGroup;
    sgrp->appendItem(m_hotspotSwitch);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(10);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    centralLayout->addWidget(sgrp);
    centralLayout->addWidget(m_lvprofiles);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();

    setLayout(centralLayout);

    connect(m_lvprofiles, &QListView::clicked, this, &HotspotDeviceWidget::onConnWidgetSelected);
    connect(m_createBtn, &QPushButton::clicked, this, [ = ] {
        openEditPage();
    });

    connect(m_refreshActiveTimer, &QTimer::timeout, this, &HotspotDeviceWidget::refreshActiveConnection);

    connect(m_wdev, &WirelessDevice::removed, this, &HotspotDeviceWidget::onDeviceRemoved);
    connect(m_wdev, &WirelessDevice::hotspotEnabledChanged, this, &HotspotDeviceWidget::onHotsportEnabledChanged);
    connect(m_wdev, &WirelessDevice::hostspotConnectionsChanged, this, &HotspotDeviceWidget::refreshHotspotConnectionList);

    connect(m_hotspotSwitch, &SwitchWidget::checkedChanged, this, &HotspotDeviceWidget::onSwitchToggled);
}

void HotspotDeviceWidget::setModel(NetworkModel *model)
{
    m_model = model;

    QTimer::singleShot(0, this, &HotspotDeviceWidget::onHotsportEnabledChanged);
    QTimer::singleShot(0, this, &HotspotDeviceWidget::refreshHotspotConnectionList);
}

void HotspotDeviceWidget::setPage(HotspotPage *p)
{
    m_page = p;
}

void HotspotDeviceWidget::closeHotspot()
{
    const QString uuid = m_wdev->activeHotspotUuid();
    Q_ASSERT(!uuid.isEmpty());

    Q_EMIT m_page->requestDisconnectConnection(uuid);
    Q_EMIT m_page->requestDeviceRemanage(m_wdev->path());
}

void HotspotDeviceWidget::openHotspot()
{
    const QList<QJsonObject> &connsObj = m_wdev->hotspotConnections();

    if (connsObj.isEmpty()) {
        m_hotspotSwitch->setChecked(false);
        openEditPage(QString());
    } else {
        // use the first connection of the hotspot connection list
        m_page->requestActivateConnection(m_wdev->path(), connsObj.first().value("Uuid").toString());
    }
}

void HotspotDeviceWidget::openEditPage(const QString &uuid)
{
    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    Q_EMIT m_page->requestNextPage(m_editPage);
}

void HotspotDeviceWidget::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_editPage.isNull()) {
        m_editPage->onDeviceRemoved();
    }

    // destroy self page
    this->deleteLater();
}

void HotspotDeviceWidget::onSwitchToggled(const bool checked)
{
    if (checked) {
        openHotspot();
    } else {
        closeHotspot();
    }
}

void HotspotDeviceWidget::onConnWidgetSelected(const QModelIndex &idx)
{
    const QString uuid = idx.data(UuidRole).toString();
    if (uuid.isEmpty()) {
        return;
    }

    m_page->requestActivateConnection(m_wdev->path(), uuid);
}

void HotspotDeviceWidget::onConnEditRequested(const QString &uuid)
{
    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, m_page, &HotspotPage::requestNextPage);

    Q_EMIT m_page->requestNextPage(m_editPage);
}

void HotspotDeviceWidget::onHotsportEnabledChanged()
{
    m_hotspotSwitch->setChecked(m_wdev->hotspotEnabled());

    m_refreshActiveTimer->start();
}

void HotspotDeviceWidget::refreshHotspotConnectionList()
{
    if (!m_wdev || !m_wdev->enabled()) {
        return;
    }

    m_modelprofiles->clear();

    for (auto connObj : m_wdev->hotspotConnections()) {
        const QString &ssid = connObj.value("Ssid").toString();
        const QString &uuid = connObj.value("Uuid").toString();

        DStandardItem *it = new DStandardItem;
        it->setText(m_model->connectionNameByPath(connObj.value("Path").toString()));
        it->setData(uuid, UuidRole);
        it->setCheckable(true);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignRight, QSize(24, 24), QSize(), true);
        editaction->setIcon(QIcon::fromTheme("arrow-right"));
        it->setActionList(Qt::Edge::RightEdge, {editaction});
        connect(editaction, &QAction::triggered, std::bind(&HotspotDeviceWidget::onConnEditRequested, this, uuid));

        m_modelprofiles->appendRow(it);
    }

    m_refreshActiveTimer->start();
}

void HotspotDeviceWidget::refreshActiveConnection()
{
    QString activeHotspotUuid;

    if (m_wdev->hotspotEnabled()) {
        activeHotspotUuid = m_wdev->activeHotspotUuid();
        if (activeHotspotUuid.isEmpty()) {
            qDebug() << "Warning: uuid of active hotspot connection is empty";
            return;
        }
    }

    for (int i = 0; i < m_modelprofiles->rowCount(); ++i) {
        QStandardItem *it = m_modelprofiles->item(i);
        it->setCheckState(it->data(UuidRole).toString() == activeHotspotUuid ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    }
}

HotspotPage::HotspotPage(QWidget *parent)
    : ContentWidget(parent)
    , m_contents(new QWidget)
    , m_newprofile(new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement))
{
    setContent(m_contents);

    QVBoxLayout *layout(new QVBoxLayout);
    m_contents->setLayout(layout);

    connect(m_newprofile, &QAbstractButton::clicked, [this] {
        if (this->m_listdevw.empty()) {
            return;
        }
        this->m_listdevw.front()->openEditPage();
    });
}

void HotspotPage::setModel(dde::network::NetworkModel *model)
{
    m_model = model;
    deviceListChanged(model->devices());
    connect(m_model, &NetworkModel::deviceListChanged, this, &HotspotPage::deviceListChanged);
    connect(m_model, &NetworkModel::deviceEnableChanged, [this] {
        this->deviceListChanged(this->m_model->devices());
    });
}

void HotspotPage::deviceListChanged(const QList<dde::network::NetworkDevice *> devices)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_contents->layout());
    layout->removeWidget(m_newprofile);

    qDeleteAll(m_listdevw);
    m_listdevw.clear();

    int ap_devices = 0;

    for (auto d : devices) {
        if (d->type() != NetworkDevice::DeviceType::Wireless) {
            continue;
        }
        if (static_cast<WirelessDevice *>(d)->supportHotspot() && d->enabled()) {
            ++ap_devices;
        }
    }

    if (ap_devices == 0) {
        Q_EMIT back();
    }

    for (auto d : devices) {
        WirelessDevice *wd(static_cast<WirelessDevice *>(d));
        if (d->type() != NetworkDevice::DeviceType::Wireless) {
            continue;
        }
        if (wd->supportHotspot() && wd->enabled()) {
            HotspotDeviceWidget *w = new HotspotDeviceWidget(wd, ap_devices > 1, this);
            w->setPage(this);
            w->setModel(m_model);
            layout->addWidget(w);
            m_listdevw.append(w);
        }
    }

    if (ap_devices == 1) {
        layout->addWidget(m_newprofile, 0, Qt::AlignmentFlag::AlignHCenter);
    }
}
}
}