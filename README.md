<p align="center">
  <a href="" rel="noopener">
 <img width=200px height=200px src="docs/logo.png" alt="Project logo"></a>
</p>

<h3 align="center">PowerVS</h3>

---

<p align="center"> PowerVS is a standalone broadcast video server based on ffmpeg
    <br> 
</p>

## üìù Table of Contents
- [About](#about)
- [Getting Started](#getting_started)
- [Running the tests](#tests)
- [Usage](#usage)
- [Deployment](#deployment)
- [Built Using](#built_using)
- [TODO](docs/TODO.md)
- [Authors](#authors)
- [Acknowledgments](#acknowledgement)

## üß About <a name="about"></a>
The year was 2011, the broadcast company where I worked at needed to improve the production workflow, which at the time consisted in ingesting and recording hundreds of betacam tapes every week.

I took on the challenge to implement a playout server based on the then recent Blackmagic cards in order to speed up the process and taking advantage of the fact that everything was handled by digital files after the first ingest from the camera source.

The first engine I thought of implementing was ffmpeg but because we were using D10 IMX MXF files and also my lack of knowledge at the time on how ffmpeg worked, the first version came out using VLC as the decoding engine.

There were some limitations however on seeking files and lack of control, so I started working on a GStreamer solution that worked ok for a while also.

I ended up returning to ffmpeg after some more investigation and analysis of the ffmbc fork that is used for broadcast and created the functionality to also record files.

There were other contestant open-source solutions at the time like CasparCG, but none seemed to work correctly with broadcast formats.

This software has been running since 2012 with little to no bugs or issues. Development of new features stopped around 2018.

I spent much of my off time developing the software and I hope it will prove useful to others even if only for academic purposes :slightly_smiling_face:


## ü Getting Started <a name="getting_started"></a>
These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See [deployment](#deployment) for notes on how to deploy the project on a live system.

### Prerequisites

```
Windows machine, tested on Windows 7 and 10

Blackmagic card, tested with:
	Decklink Duo 2
	Decklink Mini Monitor
	Decklink Mini Recorder
	Decklink Studio 4K
	UltraStudio SDI (USB 3.0)
```

### Installing
If you have a licensed copy of Visual Studio 2010, install it and then jump to the [Qt](#Qt) section

#### Compiler
 1. Install [Visual C++ 2010 Express](https://archive.org/download/vs-2010-express-1/VS2010Express1.iso)
 2. Uninstall any Visual C++ 2010 runtime/redistributable newer or equal to 10.0.30319 (Otherwise, it will cause a failure when installing Windows SDK)
 3. Install [Windows SDK 7.1](https://www.microsoft.com/en-us/download/details.aspx?id=8442) (get the GRMSDKX_EN_DVD.iso and make sure to include headers, libraries, tools, compilers and the debugging tools). Make sure .NET framework 4.x is installed, installation of the WinSDK will not allow you to select compilers otherwise.
	- On Windows 10, there is an issue with the .NET framework, so you need to execute the setup from "Setup\SDKSetup.exe" to install the compilers and you may need to install the debugging tools after setup finishes from "Setup\WinSDKDebuggingTools\dbg_x86.exe"
 4. Install [VS 2010 SP1](https://archive.org/download/vs-2010-sp-1dvd-1/VS2010SP1dvd1.iso) for the missing headers (this requires VS Express)
 5. Install [MSVC 2010 SP1 Compiler Update](https://www.microsoft.com/en-au/download/details.aspx?id=4422) for the compiler fixes.

#### Qt <a name="Qt"></a>
1. Install [Qt 4.8](https://download.qt.io/archive/qt/4.8/4.8.7/qt-opensource-windows-x86-vs2010-4.8.7.exe)
2. Install [Qt Creator](https://download.qt.io/official_releases/qtcreator/4.15/4.15.0/qt-creator-opensource-windows-x86_64-4.15.0.exe)

Open the [PowerVS.pro](https://github.com/alfredosilvestre/powervs/tree/master/PowerVS.pro) file in Qt Creator and build using the standard process.

## üîß Running the tests <a name="tests"></a>
Sadly, no automated tests were created yet since you need a physical card to test all the functionalities, usually you should have a folder with several test video files and run them through the GUI.

## üéà Usage <a name="usage"></a>
This repository has the functionalities for the GUI of PowerVS.

### Available Defines

* DECKLINK - Enables Decklink code
* GUI - Enables Graphic User Interface code
* PORTAUDIO - Enables audio output for internal audio card

## üöÄ Deployment <a name="deployment"></a>
You just need to install [Microsoft Visual C++ 2010 Redistributable Package](https://download.microsoft.com/download/1/6/5/165255E7-1014-4D0A-B094-B6A430A6BFFC/vcredist_x86.exe)
You should also copy the Qt, ffmpeg, portaudio and log4cxx shared libraries to the same location (check the [releases](https://github.com/alfredosilvestre/powervs/releases) for examples).

## ‚õèÔ∏ Built Using <a name="built_using"></a>
- [Qt 4.8](https://doc.qt.io/archives/qt-4.8/index.html) - Application development framework
- [FFmpeg](https://www.ffmpeg.org/) - Video/Audio encoding/decoding framework
- [Blackmagic](https://www.blackmagicdesign.com/) - Decklink SDK
- [PortAudio](https://github.com/PortAudio/portaudio/) - Real-time audio I/O library
- [Log4cxx](https://logging.apache.org/log4cxx/) - Logging framework

## ‚úçÔ∏ Authors <a name="authors"></a>
- [@alfredosilvestre](https://github.com/alfredosilvestre)


## üéâ Acknowledgements <a name="acknowledgement"></a>
- [@kylelobo](https://github.com/kylelobo) - For this doc template :slightly_smiling_face:
- [@aescande](https://github.com/aescande) - For the log4cxx build (forked it [here](https://github.com/alfredosilvestre/log4cxxWin32) also)
- [@bcoudurier](https://github.com/bcoudurier) - For the FFmbc fork that made it possible for me to create the recording functionality
- [@brandaopaulo](https://github.com/brandaopaulo) - For the opportunity to work on this project
