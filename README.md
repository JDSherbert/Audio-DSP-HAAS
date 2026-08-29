![image](https://github.com/JDSherbert/Audio-Delay/assets/43964243/6a7b530e-8740-423b-a20b-defd88ea625b)
 
# Audio DSP: Haas Effect
<!-- Header Start -->
<img align="right" alt="Stars Badge" src="https://img.shields.io/github/stars/jdsherbert/Audio-DSP-Haas?label=%E2%AD%90"/>
<img align="right" alt="Forks Badge" src="https://img.shields.io/github/forks/jdsherbert/Audio-DSP-Haas?label=%F0%9F%8D%B4"/>
<img align="right" alt="Watchers Badge" src="https://img.shields.io/github/watchers/jdsherbert/Audio-DSP-Haas?label=%F0%9F%91%81%EF%B8%8F"/>
<img align="right" alt="Issues Badge" src="https://img.shields.io/github/issues/jdsherbert/Audio-DSP-Haas?label=%E2%9A%A0%EF%B8%8F"/>
<a href="https://learn.microsoft.com/en-us/cpp/cpp-language">
  <img height="40" width="40" src="https://cdn.simpleicons.org/cplusplus">
</a>
<!-- Header End -->
 
-----------------------------------------------------------------------
 
<a href="">
  <img align="left" alt="Audio Processing" src="https://img.shields.io/badge/Audio%20Processing-black?style=for-the-badge&logo=audacity&logoColor=white&color=black&labelColor=black">
</a>
 
<a href="https://choosealicense.com/licenses/mit/">
  <img align="right" alt="License" src="https://img.shields.io/badge/License%20:%20MIT-black?style=for-the-badge&logo=mit&logoColor=white&color=black&labelColor=black">
</a>
 
<br></br>
 
-----------------------------------------------------------------------
 
## Overview
 
A C++ implementation of the Haas effect (also known as the precedence effect), intended as a learning resource for understanding how inter-channel delay creates perceived stereo width from a mono source.
 
The Haas effect is a psychoacoustic phenomenon: when the same sound reaches both ears with a delay of 1–40ms, the brain fuses them into a single perceived sound and localises it toward the ear that heard it first. This is exploited in audio production to widen a mono signal into a convincing stereo image, without creating a perceptible echo.
 
> For a deeper dive into delay in general, see the [Audio Delay repository](https://github.com/JDSherbert/Audio-Delay).
 
![image](https://github.com/JDSherbert/Audio-Haas-Effect/assets/43964243/afbaf0b1-29b9-411b-8ce3-6400b7bdde89)
 
-----------------------------------------------------------------------
 
## Files
 
| File | Description |
|---|---|
| `Haas.h / .cpp` | Haas effect implementation: stereo delay with configurable delayed channel |
| `main.cpp` | Example: applies the effect to a 440Hz mono sine wave, demonstrates both channel configurations and two delay times |
 
-----------------------------------------------------------------------
 
## How It Works
 
Both channels receive the same mono input signal. One channel (the *leading* channel) passes through unmodified. The other (the *delayed* channel) is held in a circular buffer and output after the configured delay time:
 
```
monoInput ──►── Left  (leading) ──────────────────────────────────► leftOut
            │
            └──► Right (delayed) ──► [ circular buffer, N samples ] ──► rightOut
```
 
The brain perceives this timing difference as spatial information and localises the sound toward the leading channel. Swapping which channel is delayed flips the perceived position.
 
The delay is implemented as a fixed-size circular buffer — identical in structure to the reverb implementation — so no memory is allocated or freed during audio processing.
 
-----------------------------------------------------------------------
 
## Delay Time Reference
 
| Delay | Perceptual Effect |
|---|---|
| < 1ms | Phase cancellation / comb filtering — not a Haas effect |
| 1 – 10ms | Subtle width, tight image |
| 10 – 30ms | Clear stereo spread, natural sounding |
| 30 – 40ms | Wide image, approaching the limit of fusion |
| > 40ms | Perceived as a distinct echo rather than spatial width |
 
-----------------------------------------------------------------------
 
## Parameters
 
| Parameter | Type | Range | Description |
|---|---|---|---|
| `delayInMilliseconds` | `int` | 1 – 40ms | Delay applied to the delayed channel. |
| `delayedChannel` | `DelayedChannel` | `Left` or `Right` | Which channel is delayed. The other channel leads. |
| `sampleRate` | `float` | > 0.0 | Audio sample rate in Hz. Used to convert ms to samples. |
 
**`DelayedChannel` enum:**
 
| Value | Leading Channel | Perceived Image |
|---|---|---|
| `DelayedChannel::Right` | Left | Sound perceived from the left |
| `DelayedChannel::Left` | Right | Sound perceived from the right |
 
-----------------------------------------------------------------------
 
## Usage
 
### Basic
 
```cpp
// 20ms delay, right channel delayed, at 44100Hz
Sherbert::Haas haas(20, Sherbert::Haas::DelayedChannel::Right, 44100.0f);
 
float leftOut  = 0.0f;
float rightOut = 0.0f;
 
// Pass the same mono signal to both inputs
haas.ProcessSample(monoIn, monoIn, leftOut, rightOut);
```
 
### Flipping the Perceived Position
 
```cpp
// Flip which channel is delayed at runtime — no reset required
haas.setDelayedChannel(Sherbert::Haas::DelayedChannel::Left);
```
 
### Changing Delay Time
 
```cpp
// setDelayInMilliseconds calls reset() internally to avoid buffer artefacts
haas.setDelayInMilliseconds(5);
```
 
### Resetting State
 
```cpp
haas.reset();
```
 
-----------------------------------------------------------------------
 
## API Reference
 
| Method | Description |
|---|---|
| `Haas(delayInMs, delayedChannel, sampleRate)` | Construct with initial parameters. |
| `ProcessSample(leftIn, rightIn, leftOut, rightOut)` | Process one stereo sample pair. Leading channel passes through; delayed channel is buffered. |
| `reset()` | Clears the delay buffer and resets the write index. |
| `setDelayInMilliseconds(value)` | Update delay time. Calls `reset()` internally. |
| `setDelayedChannel(value)` | Swap which channel is delayed. Does not call `reset()`. |
| `getDelayInMilliseconds()` | Returns current delay time in ms. |
| `getDelayedChannel()` | Returns current `DelayedChannel` value. |
| `getSampleRate()` | Returns the sample rate set at construction. |
 
-----------------------------------------------------------------------
 
## Limitations & Next Steps
 
**No wet/dry mix**: this implementation outputs the delayed channel fully wet. In a production context you would typically blend the delayed channel with the dry signal to control the intensity of the effect.
 
**Mono input assumed**: the `ProcessSample` API accepts separate `leftIn` and `rightIn` values, but the Haas effect only makes sense when both inputs carry the same mono signal. Passing a true stereo source will produce unpredictable results.
 
**No delay time modulation**: some production Haas implementations slowly drift the delay time to avoid phase cancellation artefacts when the effect is mixed to mono. This would require an LFO similar to the phaser implementation.
 
If you want to explore further, the natural next steps from here are:
- A wet/dry mix parameter
- Slow LFO modulation of the delay time to reduce mono compatibility issues
- Mid/Side processing — applying the Haas effect only to the Side signal to preserve mono compatibility
 
-----------------------------------------------------------------------
