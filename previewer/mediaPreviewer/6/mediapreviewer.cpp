/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mediapreviewer.h"
#include "playercontrols.h"
#include "../videowidget.h"

#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QtWidgets>

MediaPreviewer::MediaPreviewer(QWidget *parent)
    : QWidget(parent)
{
//! [create-objs]
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    // m_playlist = new QMediaPlaylist(this);
    m_player->setAudioOutput(m_audioOutput);
//! [create-objs]
    connect(m_player, &QMediaPlayer::durationChanged, this, &MediaPreviewer::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &MediaPreviewer::positionChanged);
    connect(m_player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this,
            [this] () {metaDataIdxChanged(m_metaDataFieldCbo->currentIndex());});
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MediaPreviewer::statusChanged);
    connect(m_player, &QMediaPlayer::hasVideoChanged, this, &MediaPreviewer::videoAvailableChanged);

//! [2]
    m_videoWidget = new VideoWidget(this);
    m_videoWidget->resize(1280, 720);
    m_player->setVideoOutput(m_videoWidget);

//! [2]
    // player layout
    QBoxLayout *layout = new QVBoxLayout(this);

    // display
    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(m_videoWidget, 2);

    layout->addLayout(displayLayout);

    // duration slider and label
    QHBoxLayout *hLayout = new QHBoxLayout;

    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0, m_player->duration());
    connect(m_slider, &QSlider::sliderMoved, this, &MediaPreviewer::seek);
    hLayout->addWidget(m_slider);

    m_labelDuration = new QLabel();
    m_labelDuration->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    hLayout->addWidget(m_labelDuration);
    layout->addLayout(hLayout);

    // controls
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);

    PlayerControls *controls = new PlayerControls();
    controls->setState(m_player->playbackState());
    controls->setVolume(m_audioOutput->volume());
    controls->setMuted(controls->isMuted());

    connect(controls, &PlayerControls::play, m_player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::previous, this, &MediaPreviewer::previousClicked);
    connect(controls, &PlayerControls::changeVolume, m_audioOutput, &QAudioOutput::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_audioOutput, &QAudioOutput::setMuted);
    connect(controls, &PlayerControls::changeRate, m_player, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop, m_videoWidget, QOverload<>::of(&QVideoWidget::update));

    connect(m_player, &QMediaPlayer::playbackStateChanged, controls, &PlayerControls::setState);
    connect(m_audioOutput, &QAudioOutput::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_audioOutput, &QAudioOutput::mutedChanged, controls, &PlayerControls::setMuted);

    controlLayout->addWidget(controls);
    controlLayout->addStretch(1);

    m_fullScreenButton = new QPushButton(tr("FullScreen"), this);
    m_fullScreenButton->setCheckable(true);
    controlLayout->addWidget(m_fullScreenButton);

    layout->addLayout(controlLayout);
    layout->setStretch(0, 10);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    // metadata
    layout->addWidget(new QLabel(tr("Metadata for file:")));
    layout->addWidget(m_metaDataFieldCbo = new QComboBox);
    layout->addWidget(m_metaDataLabel = new QLabel);

    for (int i = 0; i < QMediaMetaData::NumMetaData; ++i)
        m_metaDataFieldCbo->addItem(QMediaMetaData::metaDataKeyToString(
             static_cast<QMediaMetaData::Key>(i)));
    connect(m_metaDataFieldCbo, &QComboBox::currentIndexChanged,
            this, &MediaPreviewer::metaDataIdxChanged);

#endif

    setLayout(layout);

    if (!isPlayerAvailable()) {
        QMessageBox::warning(this, tr("Service not available"),
                             tr("The QMediaPlayer object does not have a valid service.\n"\
                                "Please check the media service plugins are installed."));

        controls->setEnabled(false);
        m_fullScreenButton->setEnabled(false);
    }
}

bool MediaPreviewer::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void MediaPreviewer::addToPlaylist(const QUrl &url) {
    m_player->setSource(url);
    m_player->play();
}

void MediaPreviewer::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(duration);
}

void MediaPreviewer::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress);

    updateDurationInfo(progress / 1000);
}

void MediaPreviewer::metaDataIdxChanged(int idx)
{
    auto key = static_cast<QMediaMetaData::Key>(idx);
    if (key == QMediaMetaData::CoverArtImage || key == QMediaMetaData::ThumbnailImage) {
        m_metaDataLabel->setPixmap(QPixmap::fromImage(
             m_player->metaData().value(key).value<QImage>()
        ));
    } else {
        m_metaDataLabel->setText(m_player->metaData().stringValue(
            static_cast<QMediaMetaData::Key>(idx)));
    }
/*
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    for (int i = 0; i < QMediaMetaData::NumMetaData; i++) {
        if (QLineEdit* field = qobject_cast<QLineEdit*>(m_metaDataFields[i]))
            field->clear();
        else if (QLabel* label = qobject_cast<QLabel*>(m_metaDataFields[i]))
            label->clear();
        m_metaDataFields[i]->setDisabled(true);
        m_metaDataLabels[i]->setDisabled(true);
    }

    for (auto &key : metaData.keys()) {
        int i = int(key);
        if (key == QMediaMetaData::CoverArtImage) {
            QVariant v = metaData.value(key);
            if (QLabel *cover = qobject_cast<QLabel*>(m_metaDataFields[key])) {
                QImage coverImage = v.value<QImage>();
                cover->setPixmap(QPixmap::fromImage(coverImage));
            }
        } else if (key == QMediaMetaData::ThumbnailImage) {
            QVariant v = metaData.value(key);
            if (QLabel *thumbnail = qobject_cast<QLabel*>(m_metaDataFields[key])) {
                QImage thumbnailImage = v.value<QImage>();
                thumbnail->setPixmap(QPixmap::fromImage(thumbnailImage));
            }
        } else if (QLineEdit *field = qobject_cast<QLineEdit*>(m_metaDataFields[key])) {
            QString stringValue = metaData.stringValue(key);
            field->setText(stringValue);
        }
        m_metaDataFields[i]->setDisabled(false);
        m_metaDataLabels[i]->setDisabled(false);
    }
#endif
*/
}

void MediaPreviewer::previousClicked() {
    // Seek to the beginning.
    m_player->setPosition(0);
}

void MediaPreviewer::seek(int mseconds) {
    m_player->setPosition(mseconds);
}

void MediaPreviewer::statusChanged(QMediaPlayer::MediaStatus status) {
    handleCursor(status);
    if (status == QMediaPlayer::EndOfMedia) {
        QApplication::alert(this);
    }
}

void MediaPreviewer::handleCursor(QMediaPlayer::MediaStatus status) {
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void MediaPreviewer::videoAvailableChanged(bool available) {
    if (!available) {
        disconnect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        disconnect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);
        m_videoWidget->setFullScreen(false);
        m_videoWidget->hide();
    } else {
        connect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        connect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);

        if (m_fullScreenButton->isChecked())
            m_videoWidget->setFullScreen(true);
        m_videoWidget->show();
    }
}

void MediaPreviewer::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}
