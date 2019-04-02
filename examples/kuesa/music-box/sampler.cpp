/*
    sampler.cpp

    This file is part of Kuesa.

    Copyright (C) 2018-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Author: Jean-Michaël Celerier <jean-michael.celerier@kdab.com>

    Licensees holding valid proprietary KDAB Kuesa licenses may use this file in
    accordance with the Kuesa Enterprise License Agreement provided with the Software in the
    LICENSE.KUESA.ENTERPRISE file.

    Contact info@kdab.com if any conditions of this licensing are not clear to you.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "sampler.h"
#if __has_include(<portaudio.h>)
#define KUESA_HAS_AUDIO 1
#endif

#if KUESA_HAS_AUDIO
#include <portaudio.h>

#include <QFile>
#include <QDebug>
#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#define DR_WAV_NO_CONVERSION_API
#include "3rdparty/dr_wav.h"
#include "3rdparty/readerwriterqueue/readerwriterqueue.h"
struct Note {
    Note()
    {
        played.reserve(32);
    }

    struct PlayedNote {
        int position{};
        float volume{ 1. };
    };

    drwav *wave{};
    std::vector<PlayedNote> played{};
};

struct NoteRequest {
    int note{};
    float volume{};
};

struct SamplerPrivate final {
    Q_DISABLE_COPY(SamplerPrivate)
    static constexpr int sampleCount = 37;
    static constexpr unsigned int sampleRate = 44100;
    static constexpr unsigned int bufferSize = 512;
    static constexpr unsigned int channels = 2;

    SamplerPrivate()
    {
        // Load all our samples
        for (int i = 1; i <= sampleCount; i++) {
            QString path = QStringLiteral("samples/%1.wav").arg(i);
            init_file(i - 1, path);
        }

        // Start audio playback
        Pa_Initialize();
        auto err = Pa_OpenDefaultStream(&stream, 2, channels, paInt16, sampleRate, bufferSize, audioCallback, this);
        if (err == paNoError) {
            err = Pa_StartStream(stream);
        }
        if (err != paNoError)
            qDebug() << "PortAudio error: " << Pa_GetErrorText(err);
    }

    ~SamplerPrivate()
    {
        if (stream)
            Pa_StopStream(stream);

        Pa_Terminate();

        for (const auto &note : notes) {
            drwav_close(note.wave);
        }
    }

    void init_file(int i, const QString &name)
    {
        files[i].setFileName(name);
        bool ok = files[i].open(QIODevice::ReadOnly);
        Q_ASSERT(ok);

        auto mem = files[i].map(0, files[i].size());
        if (mem) {
            auto &w = notes[i];
            w.wave = drwav_open_memory(mem, files[i].size());
            Q_ASSERT(w.wave->fmt.bitsPerSample == 16);
            Q_ASSERT(w.wave->fmt.channels == 1 || w.wave->fmt.channels == 2);
            Q_ASSERT(w.wave->fmt.sampleRate == 44100);
            Q_ASSERT(w.wave->fmt.formatTag == 1); // PCM
        }
    };

    static PaError audioCallback(
            const void *, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo *,
            PaStreamCallbackFlags,
            void *userData)
    {
        auto &self = *reinterpret_cast<SamplerPrivate *>(userData);
        const auto out = (int16_t *)output;

        int max_dequeue = sampleCount;
        // Dequeue all the notes requested by the UI
        NoteRequest nr;
        while (self.notesRequests.try_dequeue(nr) && --max_dequeue > 0) {
            if (nr.note >= 0 && nr.note < self.notes.size())
                self.notes[nr.note].played.push_back({ nr.note, nr.volume });
        }

        if (frameCount == 0)
            return paBufferTooSmall;

        // Zero-out our buffer
        std::fill_n(out, channels * bufferSize, 0);

        // Playback all running notes
        for (auto &n : self.notes) {
            switch (n.wave->fmt.channels) {
            case 1: {
                // Mono samples are duplicated on both channels
                for (auto it = n.played.begin(); it != n.played.end();) {
                    auto &p = *it;
                    // Load sample data
                    auto data = reinterpret_cast<int16_t *>(alloca(sizeof(int16_t) * frameCount));

                    drwav_seek_to_pcm_frame(n.wave, p.position);
                    const drwav_uint64 max = drwav_read_pcm_frames(n.wave, frameCount, data);

                    // Blit it to the sound card output
                    for (drwav_uint64 i = 0; i < max; i++) {
                        out[2 * i] += data[i] * p.volume;
                        out[2 * i + 1] += data[i] * p.volume;
                    }

                    if (max < frameCount) {
                        // If we have read up to the end of the file...
                        it = n.played.erase(it);
                    } else {
                        // Continue reading
                        p.position += frameCount;
                        ++it;
                    }
                }
                break;
            }
            case 2: {
                for (auto it = n.played.begin(); it != n.played.end();) {
                    auto &p = *it;
                    // Load sample data
                    auto data = reinterpret_cast<int16_t *>(alloca(sizeof(int16_t) * frameCount * 2));

                    drwav_seek_to_pcm_frame(n.wave, p.position);
                    const drwav_uint64 max = drwav_read_pcm_frames(n.wave, frameCount, data);

                    // Blit it to the sound card output
                    for (drwav_uint64 i = 0; i < 2 * max; i++) {
                        out[i] += data[i] * p.volume;
                    }

                    if (max < frameCount) {
                        // If we have read up to the end of the file...
                        it = n.played.erase(it);
                    } else {
                        // Continue reading
                        p.position += frameCount;
                        ++it;
                    }
                }
                break;
            }
            default:
                break;
            }
        }

        return paNoError;
    }
    moodycamel::ReaderWriterQueue<NoteRequest> notesRequests{ 512 };

private:
    std::array<QFile, sampleCount> files;
    std::array<Note, sampleCount> notes;
    PaStream *stream{};
};
#else
struct SamplerPrivate final {
};
#endif

Sampler::Sampler(QObject *parent)
    : QObject(parent)
#if KUESA_HAS_AUDIO
    , d_ptr(new SamplerPrivate)
#endif
{
}

Sampler::~Sampler()
{
}

void Sampler::note(int pitch, float volume)
{
#if KUESA_HAS_AUDIO
    Q_D(Sampler);
    d->notesRequests.enqueue(NoteRequest{ pitch, 0.1f * volume });
#endif
}
