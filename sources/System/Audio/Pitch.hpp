#pragma once

/****************************************************************************
*
* NAME: smbPitchShift.cpp
* VERSION: 1.2
* HOME URL: http://blogs.zynaptiq.com/bernsee
* KNOWN BUGS: none
*
* SYNOPSIS: Routine for doing pitch shifting while maintaining
* duration using the Short Time Fourier Transform.
*
* DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
* (one octave down) and 2. (one octave up). A value of exactly 1 does not change
* the pitch. numSampsToProcess tells the routine how many samples in indata[0...
* numSampsToProcess-1] should be pitch shifted and moved to outdata[0 ...
* numSampsToProcess-1]. The two buffers can be identical (ie. it can process the
* data in-place). fftFrameSize defines the FFT frame size used for the
* processing. Typical values are 1024, 2048 and 4096. It may be any value <=
* MAX_FRAME_LENGTH but it MUST be a power of 2. osamp is the STFT
* oversampling factor which also determines the overlap between adjacent STFT
* frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
* recommended for best quality. sampleRate takes the sample rate for the signal
* in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in
* indata[] should be in the range [-1.0, 1.0), which is also the output range
* for the data, make sure you scale the data accordingly (for 16bit signed integers
* you would have to divide (and multiply) by 32768).
*
* COPYRIGHT 1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
*
* 						The Wide Open License (WOL)
*
* Permission to use, copy, modify, distribute and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice and this license appear in all source copies.
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
* ANY KIND. See http://www.dspguru.com/wol.htm for more information.
*
*****************************************************************************/

#include <vector>
#include <cstring>

#include "Math/Math.hpp"

namespace Game
{
  namespace Audio
  {
    template<std::size_t FrameSize = 2048, std::size_t Step = 4>
    class Pitch
    {
    private:
      std::array<float, FrameSize>          _InFIFO;
      std::array<float, FrameSize>          _OutFIFO;
      std::array<float, FrameSize * 2>      _FFTworksp;
      std::array<float, FrameSize / 2 + 1>  _LastPhase;
      std::array<float, FrameSize / 2 + 1>  _SumPhase;
      std::array<float, FrameSize * 2>      _OutputAccum;
      std::array<float, FrameSize>          _AnaFreq;
      std::array<float, FrameSize>          _AnaMagn;
      std::array<float, FrameSize>          _SynFreq;
      std::array<float, FrameSize>          _SynMagn;

      /*
      ** FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
      ** Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
      ** time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
      ** and returns the cosine and sine parts in an interleaved manner, ie.
      ** fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
      ** must be a power of 2. It expects a complex input signal (see footnote 2),
      ** ie. when working with 'common' audio signals our input signal has to be
      ** passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
      ** of the frequencies of interest is in fftBuffer[0...fftFrameSize].
      */
      template <int Sign>
      void fft()
      {
        float tr, ti;

        for (unsigned int i = 2; i < 2 * FrameSize - 2; i += 2) {
          unsigned int j = 0;
          for (unsigned int bitm = 2; bitm < 2 * FrameSize; bitm <<= 1) {
            if (i & bitm)
              j++;
            j <<= 1;
          }
          if (i < j) {
            float* p1 = _FFTworksp.data() + i;
            float* p2 = _FFTworksp.data() + j;
            float temp = *p1;

            *(p1++) = *p2;
            *(p2++) = temp;
            temp = *p1;
            *p1 = *p2;
            *p2 = temp;
          }
        }
        for (unsigned int k = 0, le = 2; k < (unsigned int)((float)std::log(FrameSize) / std::log(2.f) + 0.5f); k++) {
          le <<= 1;

          unsigned int le2 = le >> 1;
          float ur = 1.0;
          float ui = 0.0;
          float arg = Math::Pi / (le2 >> 1);
          float wr = std::cos(arg);
          float wi = Sign * std::sin(arg);

          for (unsigned int j = 0; j < le2; j += 2) {
            float* p1r = _FFTworksp.data() + j;
            float* p1i = p1r + 1;
            float* p2r = p1r + le2;
            float* p2i = p2r + 1;

            for (unsigned int i = j; i < 2 * FrameSize; i += le) {
              tr = *p2r * ur - *p2i * ui;
              ti = *p2r * ui + *p2i * ur;
              *p2r = *p1r - tr;
              *p2i = *p1i - ti;
              *p1r += tr;
              *p1i += ti;
              p1r += le;
              p1i += le;
              p2r += le;
              p2i += le;
            }
            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
          }
        }
      }

    public:
      Pitch() = default;
      ~Pitch() = default;

      void shift(std::vector<float>& samples, std::size_t sampleRate, float pitchShift)
      {
        shift(samples, sampleRate, std::vector<std::pair<std::size_t, float>>({ { 0, pitchShift } }));
      }

      /*
      ** Routine smbPitchShift(). See top of file for explanation
      ** Purpose: doing pitch shifting while maintaining duration using the Short
      ** Time Fourier Transform.
      ** Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
      */
      void shift(std::vector<float>& samples, std::size_t sampleRate, const std::vector<std::pair<std::size_t, float>>& pitchShifts)
      {
        // Reset data arrays
        _InFIFO.fill(0.f);
        _OutFIFO.fill(0.f);
        _FFTworksp.fill(0.f);
        _LastPhase.fill(0.f);
        _SumPhase.fill(0.f);
        _OutputAccum.fill(0.f);
        _AnaFreq.fill(0.f);
        _AnaMagn.fill(0.f);
        _SynFreq.fill(0.f);
        _SynMagn.fill(0.f);

        /* Set up some handy variables */
        float freqPerBin = (float)sampleRate / (float)FrameSize;
        float expected = 2.f * Math::Pi * (float)(FrameSize / Step) / (float)FrameSize;

        auto pitch = pitchShifts.begin();

        // Normalize sample size to FrameSize
        samples.resize((std::size_t)std::ceil((float)samples.size() / (float)FrameSize) * FrameSize, 0.f);

        /* Main processing loop */
        for (std::size_t i = 0, gRover = FrameSize - FrameSize / Step; i < samples.size() + (FrameSize - FrameSize / Step); i++)
        {
          /* As long as we have not yet collected enough data just read in */
          _InFIFO[gRover] = (i < samples.size()) ? samples[i] : 0.f;
          if (i >= FrameSize - FrameSize / Step)
            samples[i - (FrameSize - FrameSize / Step)] = _OutFIFO[gRover - (FrameSize - FrameSize / Step)];
          gRover++;

          /* Now we have enough data for processing */
          if (gRover >= FrameSize) {
            gRover = FrameSize - FrameSize / Step;

            /* Do windowing and re,im interleave */
            for (std::size_t k = 0; k < FrameSize; k++) {
              _FFTworksp[2 * k] = _InFIFO[k] * (-0.5f * std::cos(2.f * Math::Pi * (float)k / (float)FrameSize) + 0.5f);
              _FFTworksp[2 * k + 1] = 0.f;
            }

            /* ***************** ANALYSIS ******************* */
            /* Do transform */
            fft<-1>();

            /* This is the analysis step */
            for (std::size_t k = 0; k <= FrameSize / 2; k++)
            {
              /* De-interlace FFT buffer */
              float real = _FFTworksp[2 * k];
              float imag = _FFTworksp[2 * k + 1];

              /* Compute magnitude and phase */
              float magnitude = 2.f * std::sqrt(real * real + imag * imag);
              float phase = std::atan2(imag, real);

              /* Compute phase difference */
              float tmp = phase - _LastPhase[k];
              _LastPhase[k] = phase;

              /* Subtract expected phase difference */
              tmp -= (float)k * expected;

              /* Map delta phase into +/- Pi interval */
              int qpd = (int)(tmp / Math::Pi);
              if (qpd >= 0)
                qpd += qpd & 1;
              else
                qpd -= qpd & 1;
              tmp -= Math::Pi * (float)qpd;

              /* Get deviation from bin frequency from the +/- Pi interval */
              tmp = Step * tmp / (2.f * Math::Pi);

              /* Compute the k-th partials' true frequency */
              tmp = (float)k * freqPerBin + tmp * freqPerBin;

              /* Store magnitude and true frequency in analysis arrays */
              _AnaMagn[k] = magnitude;
              _AnaFreq[k] = tmp;
            }

            /* ***************** PROCESSING ******************* */
            /* This does the actual pitch shifting */
            _SynMagn.fill(0.f);
            _SynFreq.fill(0.f);

            // Find pitch shift to apply
            while (std::next(pitch) != pitchShifts.end() && std::next(pitch)->first < i)
              pitch++;

            for (unsigned int k = 0; k <= FrameSize / 2; k++) {
              unsigned int index = (int)(k * pitch->second);

              if (index <= FrameSize / 2) {
                _SynMagn[index] += _AnaMagn[k];
                _SynFreq[index] = _AnaFreq[k] * pitch->second;
              }
            }

            /* ***************** SYNTHESIS ******************* */
            /* This is the synthesis step */
            for (std::size_t k = 0; k <= FrameSize / 2; k++)
            {
              /* Get magnitude and true frequency from synthesis arrays */
              float magnitude = _SynMagn[k];
              float tmp = _SynFreq[k];

              /* Subtract bin mid frequency */
              tmp -= (float)k * freqPerBin;

              /* Get bin deviation from freq deviation */
              tmp /= freqPerBin;

              /* Take Step into account */
              tmp = 2.f * Math::Pi * tmp / Step;

              /* Add the overlap phase advance back in */
              tmp += (float)k * expected;

              /* Accumulate delta phase to get bin phase */
              _SumPhase[k] += tmp;
              float phase = _SumPhase[k];

              /* Get real and imag part and re-interleave */
              _FFTworksp[2 * k] = magnitude * std::cos(phase);
              _FFTworksp[2 * k + 1] = magnitude * std::sin(phase);
            }

            /* Zero negative frequencies */
            for (std::size_t k = FrameSize + 2; k < 2 * FrameSize; k++)
              _FFTworksp[k] = 0.;

            /* Do inverse transform */
            fft<1>();

            /* Do windowing and add to output accumulator */
            for (std::size_t k = 0; k < FrameSize; k++)
              _OutputAccum[k] += 2.f * (-0.5f * std::cos(2.f * Math::Pi * (float)k / (float)FrameSize) + 0.5f) * _FFTworksp[2 * k] / (FrameSize / 2 * Step);
            for (std::size_t k = 0; k < FrameSize / Step; k++)
              _OutFIFO[k] = _OutputAccum[k];

            /* Shift accumulator */
            std::memmove(_OutputAccum.data(), _OutputAccum.data() + FrameSize / Step, FrameSize * sizeof(float));

            /* Move input FIFO */
            for (std::size_t k = 0; k < FrameSize - FrameSize / Step; k++)
              _InFIFO[k] = _InFIFO[k + FrameSize / Step];
          }
        }
      }
    };
  }
}