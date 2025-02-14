/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "outputsettingsdialog.h"
#include "GUI/mainwindow.h"
#include "GUI/global.h"

OutputSettingsDialog::OutputSettingsDialog(const OutputSettings &settings,
                                           QWidget *parent) :
    QDialog(parent), mInitialSettings(settings) {
    setWindowTitle("Output Settings");

    mSupportedFormats = {
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_PNG << AV_CODEC_ID_TIFF << AV_CODEC_ID_MJPEG << AV_CODEC_ID_LJPEG,
                     QList<AVCodecID>(),
                     "*.jpg"),
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_PRORES << AV_CODEC_ID_PNG << AV_CODEC_ID_QTRLE << AV_CODEC_ID_H264,
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
                     "*.mov"),
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC,
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
                     "*.mp4"),
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC,
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
        "*.mkv"),
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_H264 << AV_CODEC_ID_MPEG4 << AV_CODEC_ID_MPEG2VIDEO << AV_CODEC_ID_HEVC << AV_CODEC_ID_RAWVIDEO << AV_CODEC_ID_LJPEG,
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
        "*.avi"),
        FormatCodecs(QList<AVCodecID>() << AV_CODEC_ID_VP8 << AV_CODEC_ID_VP9,
                     QList<AVCodecID>() << AV_CODEC_ID_VORBIS << AV_CODEC_ID_OPUS,
        "*.webm"),
        FormatCodecs(QList<AVCodecID>(),
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
        "*.mp3"),
        FormatCodecs(QList<AVCodecID>(),
                     QList<AVCodecID>() << AV_CODEC_ID_FLAC,
        "*.flac"),
        FormatCodecs(QList<AVCodecID>(),
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK << AV_CODEC_ID_PCM_U8 << AV_CODEC_ID_PCM_S16LE << AV_CODEC_ID_PCM_S24LE << AV_CODEC_ID_PCM_S32LE << AV_CODEC_ID_PCM_S64LE << AV_CODEC_ID_PCM_F32LE << AV_CODEC_ID_PCM_F64LE,
        "*.wav"),
        FormatCodecs(QList<AVCodecID>(),
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
        "*.ogg"),
        FormatCodecs(QList<AVCodecID>(),
                     QList<AVCodecID>() << AV_CODEC_ID_MP3 << AV_CODEC_ID_AAC << AV_CODEC_ID_AC3 << AV_CODEC_ID_FLAC << AV_CODEC_ID_VORBIS << AV_CODEC_ID_WAVPACK,
        "*.aiff")
    };

    setStyleSheet(MainWindow::sGetInstance()->styleSheet());

    mMainLayout = new QVBoxLayout(this);
    setLayout(mMainLayout);

    mOutputFormatsLayout = new QHBoxLayout();
    mOutputFormatsLabel = new QLabel("Format:", this);
    mOutputFormatsComboBox = new QComboBox(this);
    mOutputFormatsLayout->addWidget(mOutputFormatsLabel);
    mOutputFormatsLayout->addWidget(mOutputFormatsComboBox);

    mVideoGroupBox = new QGroupBox("Video", this);
    mVideoGroupBox->setCheckable(true);
    mVideoGroupBox->setChecked(true);
    mVideoSettingsLayout = new TwoColumnLayout();
    mVideoCodecsLabel = new QLabel("Codec:", this);
    mPixelFormatsLabel = new QLabel("Pixel format:", this);
    mBitrateLabel = new QLabel("Bitrate:", this);

    mVideoCodecsComboBox = new QComboBox(this);
    mPixelFormatsComboBox = new QComboBox(this);
    mBitrateSpinBox = new QDoubleSpinBox(this);
    mBitrateSpinBox->setRange(0.1, 100.);
    mBitrateSpinBox->setSuffix(" Mbps");

    mVideoSettingsLayout->addPair(mVideoCodecsLabel,
                                  mVideoCodecsComboBox);
    mVideoSettingsLayout->addPair(mPixelFormatsLabel,
                                  mPixelFormatsComboBox);
    mVideoSettingsLayout->addPair(mBitrateLabel,
                                  mBitrateSpinBox);

    mAudioGroupBox = new QGroupBox("Audio", this);
    mAudioGroupBox->setCheckable(true);
    mAudioSettingsLayout = new TwoColumnLayout();
    mAudioCodecsLabel = new QLabel("Codec:", this);
    mSampleRateLabel = new QLabel("Sample rate:", this);
    mSampleFormatsLabel = new QLabel("Sample format:", this);
    mAudioBitrateLabel = new QLabel("Bitrate:", this);
    mAudioChannelLayoutLabel = new QLabel("Channels:", this);

    mAudioCodecsComboBox = new QComboBox(this);
    mSampleRateComboBox = new QComboBox(this);
    mSampleFormatsComboBox = new QComboBox(this);
    mAudioBitrateComboBox = new QComboBox(this);
    mAudioChannelLayoutsComboBox = new QComboBox(this);

    mAudioSettingsLayout->addPair(mAudioCodecsLabel,
                                  mAudioCodecsComboBox);
    mAudioSettingsLayout->addPair(mSampleRateLabel,
                                  mSampleRateComboBox);
    mAudioSettingsLayout->addPair(mSampleFormatsLabel,
                                  mSampleFormatsComboBox);
    mAudioSettingsLayout->addPair(mAudioBitrateLabel,
                                  mAudioBitrateComboBox);
    mAudioSettingsLayout->addPair(mAudioChannelLayoutLabel,
                                  mAudioChannelLayoutsComboBox);

    mShowLayout = new QHBoxLayout();
    mShowLabel = new QLabel("Show all formats and codecs", this);
    mShowAllFormatsAndCodecsCheckBox = new QCheckBox(this);
    connect(mShowAllFormatsAndCodecsCheckBox, &QCheckBox::toggled,
            this, &OutputSettingsDialog::setShowAllFormatsAndCodecs);
    mShowLayout->addWidget(mShowAllFormatsAndCodecsCheckBox);
    mShowLayout->addWidget(mShowLabel);

    mShowLayout->setAlignment(Qt::AlignRight);

    mButtonsLayout = new QHBoxLayout();
    mOkButton = new QPushButton("Ok", this);
    mCancelButton = new QPushButton("Cancel", this);
    mResetButton = new QPushButton("Reset", this);
    connect(mOkButton, &QPushButton::released,
            this, &OutputSettingsDialog::accept);
    connect(mCancelButton, &QPushButton::released,
            this, &OutputSettingsDialog::reject);
    connect(mResetButton, &QPushButton::released,
            this, &OutputSettingsDialog::restoreInitialSettings);
    mButtonsLayout->addWidget(mResetButton, Qt::AlignLeft);
    mButtonsLayout->addWidget(mCancelButton, Qt::AlignLeft);
    mButtonsLayout->addWidget(mOkButton, Qt::AlignRight);

    mMainLayout->addLayout(mOutputFormatsLayout);
    eSizesUI::widget.addSpacing(mMainLayout);
    mVideoGroupBox->setLayout(mVideoSettingsLayout);
    mMainLayout->addWidget(mVideoGroupBox);
    mAudioGroupBox->setLayout(mAudioSettingsLayout);
    mMainLayout->addWidget(mAudioGroupBox);
    eSizesUI::widget.addSpacing(mMainLayout);
    mMainLayout->addLayout(mShowLayout);
    eSizesUI::widget.addSpacing(mMainLayout);
    mMainLayout->addLayout(mButtonsLayout);

    connect(mVideoCodecsComboBox, &QComboBox::currentTextChanged,
            this, &OutputSettingsDialog::updateAvailablePixelFormats);

    connect(mAudioCodecsComboBox, &QComboBox::currentTextChanged, [this]() {
        updateAvailableSampleFormats();
        updateAvailableSampleRates();
        updateAvailableAudioBitrates();
        updateAvailableAudioChannelLayouts();
    });

    updateAvailableOutputFormats();
    restoreInitialSettings();
}

OutputSettings OutputSettingsDialog::getSettings() {
    OutputSettings settings = mInitialSettings;

    const AVOutputFormat *currentOutputFormat = nullptr;
    if(mVideoCodecsComboBox->count() > 0) {
        int formatId = mOutputFormatsComboBox->currentIndex();
        currentOutputFormat = mOutputFormatsList.at(formatId);
    }
    settings.fOutputFormat = currentOutputFormat;

    settings.fVideoEnabled = mVideoGroupBox->isChecked();
    const AVCodec *currentVideoCodec = nullptr;
    if(mVideoCodecsComboBox->count() > 0) {
        int codecId = mVideoCodecsComboBox->currentIndex();
        currentVideoCodec = mVideoCodecsList.at(codecId);
    }
    settings.fVideoCodec = currentVideoCodec;

    AVPixelFormat currentPixelFormat = AV_PIX_FMT_NONE;
    if(mPixelFormatsList.count() > 0) {
        int formatId = mPixelFormatsComboBox->currentIndex();
        currentPixelFormat = mPixelFormatsList.at(formatId);
    }
    settings.fVideoPixelFormat = currentPixelFormat;
    settings.fVideoBitrate = qRound(mBitrateSpinBox->value()*1000000);

    settings.fAudioEnabled = mAudioGroupBox->isChecked();
    const AVCodec *currentAudioCodec = nullptr;
    if(mAudioCodecsComboBox->count() > 0) {
        const int codecId = mAudioCodecsComboBox->currentIndex();
        currentAudioCodec = mAudioCodecsList.at(codecId);
    }
    settings.fAudioCodec = currentAudioCodec;

    AVSampleFormat currentSampleFormat = AV_SAMPLE_FMT_NONE;
    if(mSampleFormatsComboBox->count() > 0) {
        const int formatId = mSampleFormatsComboBox->currentIndex();
        currentSampleFormat = mSampleFormatsList.at(formatId);
    }
    settings.fAudioSampleFormat = currentSampleFormat;

    int currentSampleRate = 0;
    if(mSampleRateComboBox->count() > 0) {
        currentSampleRate = mSampleRateComboBox->currentData().toInt();
    }
    settings.fAudioSampleRate = currentSampleRate;

    int currentAudioBitrate = 0;
    if(mAudioBitrateComboBox->count() > 0) {
        currentAudioBitrate = mAudioBitrateComboBox->currentData().toInt();
    }
    settings.fAudioBitrate = currentAudioBitrate;

    uint64_t currentChannelsLayout = 0;
    if(mAudioChannelLayoutsComboBox->count() > 0) {
        const int layoutId = mAudioChannelLayoutsComboBox->currentIndex();
        currentChannelsLayout = mAudioChannelLayoutsList.at(layoutId);
    }
    settings.fAudioChannelsLayout = currentChannelsLayout;

    return settings;
}

void OutputSettingsDialog::updateAvailablePixelFormats() {
    const QString currentFormatName = mPixelFormatsComboBox->currentText();
    const AVCodec *currentCodec = nullptr;
    if(mVideoCodecsComboBox->count() > 0) {
        currentCodec = mVideoCodecsList.at(mVideoCodecsComboBox->currentIndex());
    }
    mPixelFormatsComboBox->clear();
    mPixelFormatsList.clear();
    if(currentCodec) {
        const AVPixelFormat *format = currentCodec->pix_fmts;
        if(!format) return;
        while(*format != -1) {
            mPixelFormatsList << *format;
            const QString formatName(av_get_pix_fmt_name(*format));
            mPixelFormatsComboBox->addItem(formatName);
            if(formatName == currentFormatName)
                mPixelFormatsComboBox->setCurrentText(formatName);
            format++;
        }
    }
}

void OutputSettingsDialog::addVideoCodec(const AVCodec* const codec,
                                         const AVOutputFormat *outputFormat,
                                         const QString &currentCodecName) {
    if(!codec) return;
    if(codec->type != AVMEDIA_TYPE_VIDEO) return;
    if(codec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) return;
    if(codec->pix_fmts == nullptr) return;
    if(avformat_query_codec(outputFormat, codec->id, COMPLIENCE) == 0) return;
    mVideoCodecsList << codec;
    const QString codecName(codec->long_name);
    mVideoCodecsComboBox->addItem(codecName);
    if(codecName == currentCodecName) {
        mVideoCodecsComboBox->setCurrentText(codecName);
    }
}

void OutputSettingsDialog::addAudioCodec(const AVCodecID &codecId,
                                         const AVOutputFormat *outputFormat,
                                         const QString &currentCodecName) {
    const AVCodec* currentCodec = avcodec_find_encoder(codecId);
    if(!currentCodec) return;
    if(currentCodec->type != AVMEDIA_TYPE_AUDIO) return;
    if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) return;
    if(currentCodec->sample_fmts == nullptr) return;
    if(avformat_query_codec(outputFormat, codecId, COMPLIENCE) == 0) return;
    mAudioCodecsList << currentCodec;
    const QString codecName(currentCodec->long_name);
    mAudioCodecsComboBox->addItem(codecName);
    if(codecName == currentCodecName) {
        mAudioCodecsComboBox->setCurrentText(codecName);
    }
}

void OutputSettingsDialog::updateAvailableCodecs() {
    updateAvailableAudioCodecs();
    updateAvailableVideoCodecs();
}

void OutputSettingsDialog::updateAvailableVideoCodecs() {
    const QString currentCodecName = mVideoCodecsComboBox->currentText();
    mVideoCodecsComboBox->clear();
    mVideoCodecsList.clear();
    if(mOutputFormatsComboBox->count() == 0) return;
    const int outputFormatId = mOutputFormatsComboBox->currentIndex();
    const AVOutputFormat * const outputFormat =
            mOutputFormatsList.at(outputFormatId);
    if(!outputFormat) return;

    if(mShowAllFormatsAndCodecs) {
        const AVCodec *iCodec = nullptr;
        void *i = nullptr;
        while((iCodec = av_codec_iterate(&i))) {
          if(av_codec_is_encoder(iCodec)) {
              if(!iCodec) break;
              if(iCodec->type != AVMEDIA_TYPE_VIDEO) continue;
              if(iCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) {
                  continue;
              }
              if(iCodec->pix_fmts == nullptr) continue;
              addVideoCodec(iCodec, outputFormat, currentCodecName);
          }
        }
    } else {
        const FormatCodecs currFormatT =
                mSupportedFormats.at(outputFormatId);
        for(const AVCodecID &codecId : currFormatT.mVidCodecs) {
            const AVCodec* iCodec = avcodec_find_encoder(codecId);
            if(!iCodec) break;
            if(iCodec->type != AVMEDIA_TYPE_VIDEO) continue;
            if(iCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) {
                continue;
            }
            if(iCodec->pix_fmts == nullptr) continue;
            addVideoCodec(iCodec, outputFormat, currentCodecName);
        }
    }
    const bool noCodecs = mVideoCodecsComboBox->count() == 0;
    mVideoGroupBox->setChecked(!noCodecs);
    mVideoGroupBox->setDisabled(noCodecs);
}

void OutputSettingsDialog::updateAvailableAudioCodecs() {
    const QString currentCodecName =
            mAudioCodecsComboBox->currentText();
    mAudioCodecsComboBox->clear();
    mAudioCodecsList.clear();
    if(mOutputFormatsComboBox->count() == 0) return;
    const int outputFormatId = mOutputFormatsComboBox->currentIndex();
    const  AVOutputFormat * const outputFormat =
            mOutputFormatsList.at(outputFormatId);
    if(!outputFormat) return;
    if(mShowAllFormatsAndCodecs) {
        const AVCodec *currentCodec = nullptr;
        void *i = nullptr;
        while((currentCodec = av_codec_iterate(&i))) {
            if(av_codec_is_encoder(currentCodec)) {
                if(!currentCodec) break;
                if(currentCodec->type != AVMEDIA_TYPE_AUDIO) continue;
                if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) {
                    continue;
                }
                if(currentCodec->sample_fmts == nullptr) continue;
                addAudioCodec(currentCodec->id,
                              outputFormat,
                              currentCodecName);
            }
        }
    } else {
        const FormatCodecs currFormatT =
                mSupportedFormats.at(outputFormatId);
        for(const AVCodecID &codecId : currFormatT.mAudioCodecs) {
            const AVCodec* currentCodec = avcodec_find_encoder(codecId);
            if(!currentCodec) break;
            if(currentCodec->type != AVMEDIA_TYPE_AUDIO) continue;
            if(currentCodec->capabilities & AV_CODEC_CAP_EXPERIMENTAL) {
                continue;
            }
            if(currentCodec->sample_fmts == nullptr) continue;
            addAudioCodec(currentCodec->id,
                          outputFormat,
                          currentCodecName);
        }
    }
    bool noCodecs = mAudioCodecsComboBox->count() == 0;
    mAudioGroupBox->setChecked(!noCodecs);
    mAudioGroupBox->setDisabled(noCodecs);
}


void OutputSettingsDialog::updateAvailableOutputFormats() {
    disconnect(mOutputFormatsComboBox, &QComboBox::currentTextChanged,
               this, &OutputSettingsDialog::updateAvailableCodecs);

    QString currentFormatName = mOutputFormatsComboBox->currentText();

    mOutputFormatsComboBox->clear();
    mOutputFormatsList.clear();

    if(mShowAllFormatsAndCodecs) {
        const AVOutputFormat *currentFormat = nullptr;
        void *i = nullptr;
        while((currentFormat = av_muxer_iterate(&i))) {
            if(!currentFormat) break;
            //if(!currentFormat->codec_tag || !currentFormat->codec_tag[0]) continue;
//            int supportedCodecs = 0;
//            for(const AVCodecID &codecId : VIDEO_CODECS) {
//                if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//                supportedCodecs++;
//                break;
//            }
//            for(const AVCodecID &codecId : AUDIO_CODECS) {
//                if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//                supportedCodecs++;
//                break;
//            }
//            if(supportedCodecs == 0) continue;
            //if(currentFormat->query_codec == nullptr) continue;
            mOutputFormatsList << currentFormat;
            QString formatName = QString(currentFormat->long_name);
            mOutputFormatsComboBox->addItem(formatName);
            if(formatName == currentFormatName) {
                mOutputFormatsComboBox->setCurrentText(formatName);
            }
        }
    } else {
        for(const FormatCodecs &formatT : mSupportedFormats) {
            const auto currentFormat = formatT.mFormat;
            if(!currentFormat) break;
            //if(!currentFormat->codec_tag || !currentFormat->codec_tag[0]) continue;
//            int supportedCodecs = 0;
//            for(const AVCodecID &codecId : currentFormat->video_codec) {
//                if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//                supportedCodecs++;
//                break;
//            }
//            for(const AVCodecID &codecId : currentFormat->audio_codec) {
//                if(avformat_query_codec(currentFormat, codecId, COMPLIENCE) == 0) continue;
//                supportedCodecs++;
//                break;
//            }
//            if(supportedCodecs == 0) continue;
            //if(currentFormat->query_codec == nullptr) continue;
            mOutputFormatsList << currentFormat;
            QString formatName = QString(currentFormat->long_name);
            mOutputFormatsComboBox->addItem(formatName);
            if(formatName == currentFormatName) {
                mOutputFormatsComboBox->setCurrentText(formatName);
            }
        }
    }
    connect(mOutputFormatsComboBox, &QComboBox::currentTextChanged,
            this, &OutputSettingsDialog::updateAvailableCodecs);

    updateAvailableCodecs();
}

void OutputSettingsDialog::updateAvailableSampleFormats() {
    const QString lastSet = mSampleFormatsComboBox->currentText();
    mSampleFormatsComboBox->clear();
    mSampleFormatsList.clear();
    const AVCodec *currentCodec = nullptr;
    if(mAudioCodecsComboBox->count() > 0) {
        const int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(!currentCodec) return;
    const AVSampleFormat *sampleFormats = currentCodec->sample_fmts;
    if(!sampleFormats) return;
    while(*sampleFormats != -1) {
        mSampleFormatsList << *sampleFormats;
        QString formatName;
        const auto it = OutputSettings::sSampleFormatNames.find(*sampleFormats);
        if(it == OutputSettings::sSampleFormatNames.end()) {
            formatName = "unrecognized";
        } else {
            formatName = it->second;
        }
        mSampleFormatsComboBox->addItem(formatName);
        sampleFormats++;
    }

    if(mSampleFormatsComboBox->findText(lastSet)) {
        mSampleFormatsComboBox->setCurrentText(lastSet);
    }
}

void OutputSettingsDialog::updateAvailableAudioBitrates() {
    const auto lastSet = mAudioBitrateComboBox->currentData();
    mAudioBitrateComboBox->clear();
    const AVCodec *currentCodec = nullptr;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(!currentCodec) return;
    if(currentCodec->capabilities & AV_CODEC_PROP_LOSSLESS) {
        mAudioBitrateComboBox->addItem("Loseless", QVariant(384000));
    } else {
        QList<int> rates = { 24, 32, 48, 64, 128, 160, 192, 320, 384 };
        for(const int rate : rates) {
            mAudioBitrateComboBox->addItem(QString::number(rate) + " kbps",
                                           QVariant(rate*1000));
        }
    }

    bool set = false;
    if(lastSet.isValid()) {
        const int lastSetId = mAudioBitrateComboBox->findData(lastSet.toInt());
        set = lastSetId != -1;
        if(set) mAudioBitrateComboBox->setCurrentIndex(lastSetId);
    }
    if(!set) {
        const int i192 = mAudioBitrateComboBox->findData(QVariant(192000));
        if(i192 != -1) mAudioBitrateComboBox->setCurrentIndex(i192);
    }
}

void OutputSettingsDialog::updateAvailableSampleRates() {
    const auto lastSet = mSampleRateComboBox->currentData();
    const AVCodec *currentCodec = nullptr;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(!currentCodec) return;
    const int *sampleRates = currentCodec->supported_samplerates;
    if(!sampleRates) {
        QList<int> rates = { 96000, 88200, 64000, 48000, 44100,
                             32000, 22050, 11025, 8000 };
        for(const int rate : rates) {
            mSampleRateComboBox->addItem(QString::number(rate) + " Hz",
                                         QVariant(rate));
        }
    } else {
        int rate = *sampleRates;
        while(rate != 0) {
            mSampleRateComboBox->addItem(QString::number(rate) + " Hz",
                                         QVariant(rate));
            rate = *(++sampleRates);
        }
    }
    bool set = false;
    if(lastSet.isValid()) {
        const int lastSetId = mSampleRateComboBox->findData(lastSet.toInt());
        set = lastSetId != -1;
        if(set) mSampleRateComboBox->setCurrentIndex(lastSetId);
    }
    if(!set) {
        const int i441 = mAudioBitrateComboBox->findData(44100);
        if(i441 != -1) mAudioBitrateComboBox->setCurrentIndex(i441);
    }
}

void OutputSettingsDialog::updateAvailableAudioChannelLayouts() {
    const auto lastSet = mAudioChannelLayoutsComboBox->currentText();
    mAudioChannelLayoutsComboBox->clear();
    mAudioChannelLayoutsList.clear();
    const AVCodec *currentCodec = nullptr;
    if(mAudioCodecsComboBox->count() > 0) {
        int codecId = mAudioCodecsComboBox->currentIndex();
        currentCodec = mAudioCodecsList.at(codecId);
    }
    if(!currentCodec) return;
    const uint64_t *layouts = currentCodec->channel_layouts;
    if(!layouts) {
        mAudioChannelLayoutsList << AV_CH_LAYOUT_MONO;
        mAudioChannelLayoutsList << AV_CH_LAYOUT_STEREO;
        mAudioChannelLayoutsComboBox->addItem("Mono");
        mAudioChannelLayoutsComboBox->addItem("Stereo");
    } else {
        uint64_t layout = *layouts;
        while(layout != 0) {
            const QString layoutName = OutputSettings::sGetChannelsLayoutName(layout);
            mAudioChannelLayoutsList << layout;
            mAudioChannelLayoutsComboBox->addItem(layoutName);
            layouts++;
            layout = *layouts;
        }
    }
    if(mAudioChannelLayoutsComboBox->findText(lastSet) != -1) {
        mAudioChannelLayoutsComboBox->setCurrentText(lastSet);
    } else if(mAudioChannelLayoutsComboBox->findText("Stereo")) {
        mAudioChannelLayoutsComboBox->setCurrentText("Stereo");
    }
}

void OutputSettingsDialog::restoreInitialSettings() {
    const auto currentOutputFormat = mInitialSettings.fOutputFormat;
    if(!currentOutputFormat) {
        mOutputFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentOutputFormatName = QString(currentOutputFormat->long_name);
        mOutputFormatsComboBox->setCurrentText(currentOutputFormatName);
    }
    const AVCodec *currentVideoCodec = mInitialSettings.fVideoCodec;
    if(!currentVideoCodec) {
        mVideoCodecsComboBox->setCurrentIndex(0);
    } else {
        QString currentCodecName = QString(currentVideoCodec->long_name);
        mVideoCodecsComboBox->setCurrentText(currentCodecName);
    }
    AVPixelFormat currentFormat = mInitialSettings.fVideoPixelFormat;
    if(currentFormat == AV_PIX_FMT_NONE) {
        mPixelFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentFormatName = QString(av_get_pix_fmt_name(currentFormat));
        mPixelFormatsComboBox->setCurrentText(currentFormatName);
    }
    const int currentBitrate = mInitialSettings.fVideoBitrate;
    if(currentBitrate <= 0) {
        mBitrateSpinBox->setValue(9.);
    } else {
        mBitrateSpinBox->setValue(currentBitrate/1000000.);
    }
    const bool noVideoCodecs = mVideoCodecsComboBox->count() == 0;
    mVideoGroupBox->setChecked(mInitialSettings.fVideoEnabled &&
                               !noVideoCodecs);

    const AVCodec *currentAudioCodec = mInitialSettings.fAudioCodec;
    if(!currentAudioCodec) {
        mAudioCodecsComboBox->setCurrentIndex(0);
    } else {
        QString currentCodecName = QString(currentAudioCodec->long_name);
        mAudioCodecsComboBox->setCurrentText(currentCodecName);
    }

    AVSampleFormat currentSampleFormat = mInitialSettings.fAudioSampleFormat;
    if(currentSampleFormat == AV_SAMPLE_FMT_NONE) {
        mSampleFormatsComboBox->setCurrentIndex(0);
    } else {
        QString currentFormatName = OutputSettings::sSampleFormatNames.at(currentSampleFormat);
        mSampleFormatsComboBox->setCurrentText(currentFormatName);
    }

    const int currentSampleRate = mInitialSettings.fAudioSampleRate;
    const int currSRId = mSampleRateComboBox->findData(QVariant(currentSampleRate));
    if(currSRId == -1) {
        const int i48000Id = mSampleRateComboBox->findData(QVariant(48000));
        if(i48000Id == -1) {
            const int i44100Id = mSampleRateComboBox->findData(QVariant(44100));
            if(i44100Id == -1) {
                const int lastId = mSampleRateComboBox->count() - 1;
                mSampleRateComboBox->setCurrentIndex(lastId);
            } else {
                mSampleRateComboBox->setCurrentIndex(i44100Id);
            }
        } else {
            mSampleRateComboBox->setCurrentIndex(i48000Id);
        }
    } else {
        mSampleRateComboBox->setCurrentIndex(currSRId);
    }

    const int currentAudioBitrate = mInitialSettings.fAudioBitrate;
    const int currABRId = mAudioBitrateComboBox->findData(QVariant(currentAudioBitrate));
    if(currABRId == -1) {
        int i192Id = mAudioBitrateComboBox->findData(QVariant(192000));
        if(i192Id == -1) {
            const int lastId = mAudioBitrateComboBox->count() - 1;
            mAudioBitrateComboBox->setCurrentIndex(lastId);
        } else {
            mAudioBitrateComboBox->setCurrentIndex(i192Id);
        }
    } else {
        mAudioBitrateComboBox->setCurrentIndex(currABRId);
    }

    const uint64_t currentChannelsLayout = mInitialSettings.fAudioChannelsLayout;
    if(currentChannelsLayout == 0) {
        const int st = mAudioChannelLayoutsComboBox->findText("Stereo");
        mAudioChannelLayoutsComboBox->setCurrentIndex(st == -1 ? 0 : st);
    } else {
        const QString channLayStr =
                OutputSettings::sGetChannelsLayoutName(currentChannelsLayout);
        if(channLayStr == "Unrecognized") {
            const int st = mAudioChannelLayoutsComboBox->findText("Stereo");
            mAudioChannelLayoutsComboBox->setCurrentIndex(st == -1 ? 0 : st);
        } else {
            mAudioChannelLayoutsComboBox->setCurrentText(channLayStr);
        }
    }

    const bool noAudioCodecs = mAudioCodecsComboBox->count() == 0;
    mAudioGroupBox->setChecked(mInitialSettings.fAudioEnabled &&
                               !noAudioCodecs);
}


#ifndef AVFORMAT_INTERNAL_H
#define AVFORMAT_INTERNAL_H
typedef struct AVCodecTag {
    enum AVCodecID id;
    unsigned int tag;
} AVCodecTag;
#endif

OutputSettingsDialog::FormatCodecs::FormatCodecs(
        const QList<AVCodecID> &vidCodecs,
        const QList<AVCodecID> &audioCodec, const char *name) {
    mFormat = av_guess_format(nullptr, name, nullptr);
//    bool defVidAdded = false;
//    bool defAudAdded = false;
    for(const AVCodecID &vidCodec : vidCodecs) {
        if(avformat_query_codec(mFormat, vidCodec, COMPLIENCE) == 0) continue;
        mVidCodecs << vidCodec;
//        if(vidCodec == mFormat->video_codec) defVidAdded = true;
    }
    for(const AVCodecID &audCodec : audioCodec) {
        if(avformat_query_codec(mFormat, audCodec, COMPLIENCE) == 0) continue;
        mAudioCodecs << audCodec;
//        if(audCodec == mFormat->audio_codec) defAudAdded = true;
    }
//    if(!defAudAdded) {
//        if(mFormat->audio_codec != AV_CODEC_ID_NONE) {
//            mAudioCodecs << mFormat->audio_codec;
//        }
//    }
//    if(!defVidAdded) {
//        if(mFormat->video_codec != AV_CODEC_ID_NONE) {
//            mVidCodecs << mFormat->video_codec;
//        }
//    }

//    for(int i = 0; mFormat->codec_tag && mFormat->codec_tag[i]; i++) {
//        const AVCodecTag *tags2 = mFormat->codec_tag[i];
//        for(int j = 0; tags2[j].id != AV_CODEC_ID_NONE; j++) {
//            AVCodecID codecId = tags2[j].id;
//            if(avformat_query_codec(mFormat, codecId, COMPLIENCE) == 0) continue;
//            AVCodec *codecT = avcodec_find_encoder(codecId);
//            if(!codecT) continue;
//            if(codecT->type == AVMEDIA_TYPE_VIDEO) {
//                if(!mVidCodecs.contains(codecId)) {
//                    mVidCodecs << codecId;
//                }
//            } else if(codecT->type == AVMEDIA_TYPE_AUDIO) {
//                if(!mAudioCodecs.contains(codecId)) {
//                    mAudioCodecs << codecId;
//                }
//            }
//        }
//    }
}
