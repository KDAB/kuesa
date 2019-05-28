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

#if KUESA_HAS_AUDIO
#include <portaudio.h>

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <array>
#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO
#define DR_WAV_NO_CONVERSION_API
#include "3rdparty/dr_wav.h"
#include "3rdparty/freeverb.h"
#include "3rdparty/readerwriterqueue/readerwriterqueue.h"

static constexpr int maxSampleCount = 127;
static constexpr unsigned int sampleRate = 44100;
static constexpr unsigned int channels = 2;
static_assert(channels == 2, "The code needs some adjustment if you want use more than two channels");

// Represents a wave file associated to multiple playheads
class Note
{
public:
    Note()
    {
        played.reserve(32);
    }

    Note(const Note &) = delete;
    Note &operator=(const Note &) = delete;

    Note(Note &&other)
        : wave{ other.wave }
        , played{ std::move(other.played) }
    {
        other.wave = nullptr;
        other.played.clear();
    }

    Note &operator=(Note &&other)
    {
        wave = other.wave;
        played = std::move(other.played);
        other.wave = nullptr;
        other.played.clear();
        return *this;
    }

    ~Note()
    {
        if (wave)
            drwav_close(wave);
    }

    struct PlayedNote {
        int position; // The position in the sample.
        float volume;
    };

    drwav *wave{};
    std::vector<PlayedNote> played{};
};

// The UI sends this to the audio engine
struct NoteRequest {
    int note;
    float volume;
};

// A set of memory-mapped samples
struct SampleSet {
    std::array<QFile, maxSampleCount> files;
    std::array<Note, maxSampleCount> notes;

    void initFile(int i, const QString &name)
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
    }
};
class SamplerPrivate
{
    Q_DISABLE_COPY(SamplerPrivate)

public:
    SamplerPrivate(Sampler &s)
        : m_sampler{ s }
    {
        m_reverb.init(sampleRate);
        m_reverb.setDryWet(0.3);
        m_reverb.setRoomSize(0.7);

        // Start audio playback
        Pa_Initialize();
        auto err = Pa_OpenDefaultStream(&stream, 0, channels, paInt16, sampleRate, paFramesPerBufferUnspecified, audioCallback, this);
        if (err == paNoError) {
            err = Pa_StartStream(stream);
        }

        if (err != paNoError) {
            qDebug() << "PortAudio error: " << Pa_GetErrorText(err);
            if (stream) {
                Pa_CloseStream(stream);
                stream = nullptr;
            }
        }
    }

    ~SamplerPrivate()
    {
        if (stream)
            Pa_StopStream(stream);

        Pa_Terminate();
        garbageCollectSamples();
        {
            SampleSet *top{};
            while (m_nextSamples.try_dequeue(top)) {
                delete top;
            }
        }
        delete m_current;
    }

    void reloadNotes(const QStringList &notes)
    {
        auto samples = new SampleSet;
        for (int i = 0; i < notes.size(); i++) {
            samples->initFile(i, notes[i]);
        }

        m_nextSamples.enqueue(samples);

        QTimer::singleShot(1000, [this] { garbageCollectSamples(); });
    }

    moodycamel::ReaderWriterQueue<NoteRequest> notesRequests{ 512 };

private:
    void
    garbageCollectSamples()
    {
        SampleSet *sample{};
        while (m_oldSamples.try_dequeue(sample)) {
            delete sample;
        }
    }

    static PaError audioCallback(
            const void *, void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo *,
            PaStreamCallbackFlags,
            void *userData)
    {
        auto &self = *reinterpret_cast<SamplerPrivate *>(userData);
        const auto out = (int16_t *)output;

        // Zero-out our buffer
        std::fill_n(out, channels * frameCount, 0);

        // If we have to use new samples we perform a swap.
        {
            SampleSet *top{};
            while (self.m_nextSamples.try_dequeue(top)) {
                self.m_oldSamples.enqueue(self.m_current);
                self.m_current = top;
            }
        }

        // No set of samples - just dequeue the requests
        if (!self.m_current) {
            NoteRequest nr;
            while (self.notesRequests.try_dequeue(nr))
                ;
            return 0;
        }

        auto &notes = self.m_current->notes;

        int max_dequeue = maxSampleCount;
        // Dequeue all the notes requested by the UI
        NoteRequest nr;
        while (self.notesRequests.try_dequeue(nr) && --max_dequeue > 0) {
            if (nr.note >= 0 && nr.note < notes.size())
                notes[nr.note].played.push_back(Note::PlayedNote{ nr.note, nr.volume });
        }

        if (frameCount == 0)
            return paBufferTooSmall;

        // Playback all running notes
        for (auto &n : notes) {
            if (!n.wave)
                continue;

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
                        out[channels * i] += data[i] * p.volume;
                        out[channels * i + 1] += data[i] * p.volume;
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

        const auto int_to_float = [](int16_t v) -> float {
            return v / float(INT16_MAX);
        };
        const auto float_to_int = [](float f) -> int16_t {
            int32_t v = (f * float(INT16_MAX));
            return (v < INT16_MIN)
                    ? INT16_MIN
                    : ((v > INT16_MAX)
                               ? INT16_MAX
                               : v);
        };

        // Reverb pass
        if (self.m_enableReverb) {
            float *reverb_in[channels];
            for (std::size_t i = 0; i < channels; i++) {
                reverb_in[i] = (float *)alloca(sizeof(float) * frameCount);
            }
            for (std::size_t i = 0; i < frameCount; i++) {
                for (std::size_t c = 0; c < channels; c++) {
                    reverb_in[c][i] = int_to_float(out[channels * i + c]);
                }
            }

            self.m_reverb.compute(frameCount, reverb_in, reverb_in);

            for (std::size_t i = 0; i < frameCount; i++) {
                for (std::size_t c = 0; c < channels; c++) {
                    out[channels * i + c] = float_to_int(reverb_in[c][i]);
                }
            }
        }

        // Compute RMS and peak values for the first channel
        int64_t rms = 0;
        int16_t peak = 0;
        for (std::size_t i = 0; i < frameCount; i++) {
            const int16_t val = out[channels * i];
            peak = std::max(val, peak);
            rms += val * val;
        }

        self.m_sampler.audioSignal(std::sqrt((float(rms) / float(INT16_MAX)) / frameCount), int_to_float(peak));

        return paNoError;
    }

    Sampler &m_sampler;
    SampleSet *m_current{};

    moodycamel::ReaderWriterQueue<SampleSet *> m_nextSamples, m_oldSamples;

    PaStream *stream{};

    mydsp m_reverb;
    bool m_enableReverb{ true };
};
#else
// Dummy class needed for Sampler::d_ptr
struct SamplerPrivate final {
};
#endif

Sampler::Sampler(QObject *parent)
    : QObject(parent)
#if KUESA_HAS_AUDIO
    , d_ptr(new SamplerPrivate{ *this })
#else
    , d_ptr(nullptr)
#endif

{
}

Sampler::~Sampler()
{
}

void Sampler::note(int sample, float volume)
{
#if KUESA_HAS_AUDIO
    Q_D(Sampler);
    if (sample >= 0 && sample < m_notes.size()) {
        d->notesRequests.enqueue(NoteRequest{ sample, 0.1f * volume });
    }
#endif
}

QStringList Sampler::notes() const
{
    return m_notes;
}

void Sampler::setNotes(QStringList notes)
{
    if (m_notes == notes)
        return;

    m_notes = std::move(notes);

#if KUESA_HAS_AUDIO
    Q_D(Sampler);
    d->reloadNotes(m_notes);
#endif

    emit notesChanged(m_notes);
}
