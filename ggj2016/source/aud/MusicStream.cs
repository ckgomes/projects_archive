using System;
using System.Threading;
using System.Collections.Concurrent;

using NVorbis;

using OpenTK.Audio;
using OpenTK.Audio.OpenAL;

namespace jam.source
{

    class MusicStream : IDisposable
    {

        AudioContext context;
        ConcurrentDictionary<int, MusicThreadState> musicStates;


        public MusicStream()
        {
            context = new AudioContext();
            musicStates = new ConcurrentDictionary<int, MusicThreadState>();
        }

        public int Play(string path, bool loop, float gain = 1.0f)
        {

            MusicThreadParameters parameters = new MusicThreadParameters();
            parameters.Path = path;
            parameters.Loop = loop;
            parameters.Gain = gain;

            Thread musicPlayer = new Thread(musicThread);
            musicPlayer.Start(parameters);

            return musicPlayer.ManagedThreadId;
        }

        public void Stop(int musicThread)
        {
            MusicThreadState state;
            if (musicStates.TryGetValue(musicThread, out state))
            {
                state.Play = false;
                state.MusicThread.Join();
            }
        }

        public void SetVolume(int level)
        {
            AL.Listener(ALListenerf.Gain, level / 100f);
        }

        private void musicThread(object param)
        {

            MusicThreadParameters parameters = (MusicThreadParameters)param;

            MusicThreadState musicThreadState = new MusicThreadState();
            musicThreadState.Play = true;
            musicThreadState.MusicThread = Thread.CurrentThread;

            musicStates[Thread.CurrentThread.ManagedThreadId] = musicThreadState;

            int[] buffers = AL.GenBuffers(2);
            int source = AL.GenSource();

            AL.Source(source, ALSourcef.Gain, parameters.Gain);

            do
            {

                VorbisReader vorbis;
                lock (sync)
                {
                    vorbis = new VorbisReader(parameters.Path);
                }

                ALFormat format = (vorbis.Channels == 1) ? ALFormat.Mono16 : ALFormat.Stereo16;
                int bufferStride = vorbis.Channels * sizeof(ushort);
                int bufferLength = vorbis.Channels * vorbis.SampleRate * bufferCapacityInSeconds;
                float[] floatBuffer = new float[bufferLength];
                ushort[] pcmBuffer = new ushort[bufferLength];

                int samplesRead = vorbis.ReadSamples(floatBuffer, 0, bufferLength);
                toPCM(pcmBuffer, floatBuffer);
                AL.BufferData(buffers[0], format, pcmBuffer, samplesRead * sizeof(ushort), vorbis.SampleRate);

                if (samplesRead > 0)
                {

                    samplesRead = vorbis.ReadSamples(floatBuffer, 0, bufferLength);
                    toPCM(pcmBuffer, floatBuffer);
                    AL.BufferData(buffers[1], format, pcmBuffer, samplesRead * sizeof(ushort), vorbis.SampleRate);

                    AL.SourceQueueBuffers(source, buffers.Length, buffers);
                    AL.SourcePlay(source);

                    while ((samplesRead > 0) && musicThreadState.Play)
                    {

                        if (AL.GetSourceState(source) != ALSourceState.Playing)
                            AL.SourcePlay(source);

                        int processedBuffers;
                        AL.GetSource(source, ALGetSourcei.BuffersProcessed, out processedBuffers);

                        if (processedBuffers > 0)
                        {

                            int buffer = AL.SourceUnqueueBuffer(source);

                            samplesRead = vorbis.ReadSamples(floatBuffer, 0, bufferLength);
                            toPCM(pcmBuffer, floatBuffer);

                            AL.BufferData(buffer, format, pcmBuffer, samplesRead * sizeof(ushort), vorbis.SampleRate);
                            AL.SourceQueueBuffer(source, buffer);

                        }
                        else Thread.Sleep(100);

                    }

                    while ((AL.GetSourceState(source) == ALSourceState.Playing) && musicThreadState.Play)
                        Thread.Sleep(100);

                    AL.Source(source, ALSourcei.Buffer, 0);


                }

            } while (parameters.Loop && musicThreadState.Play);

            AL.DeleteSource(source);
            AL.DeleteBuffers(buffers);

            musicStates.TryRemove(Thread.CurrentThread.ManagedThreadId, out musicThreadState);

        }

        private void toPCM(ushort[] output, float[] input)
        {
            for (int i = 0; i < output.Length; ++i)
            {
                output[i] = (ushort)(input[i] * 32768f);
            }
        }

        private bool shouldStop(int thread)
        {
            MusicThreadState state;
            if (musicStates.TryGetValue(thread, out state))
            {
                if ((state.MusicThread.ManagedThreadId == thread) && !state.Play)
                    return true;
                else return false;
            }
            return true;
        }

        public void Dispose()
        {
            foreach (var state in musicStates) state.Value.Play = false;
        }

        const int bufferCapacityInSeconds = 2;

        private static object sync = new object();

    }

    internal class MusicThreadParameters
    {
        public string Path { get; set; }
        public bool Loop { get; set; }
        public float Gain { get; set; }
    }

    internal class MusicThreadState
    {
        public Thread MusicThread { get; set; }
        public bool Play { get; set; }
    }

}
