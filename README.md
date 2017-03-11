# AlloGain
AlloGain is a standalone application designed to control the gain level of the AlloSphere's audio system. The application is compatible with the ECHO AudioFire 12 interfaces. Gain level adjustments are performed digitally on the DSP at 24-bit resolution.

### Features
* Displays the set gain level
* Enables single button mute
* Sets system to mute and -96.00 dB gain when application starts or quits
* Allows gain increase by +0.25, +1.00 and +5.00 dB
* Allows gain decrease by -0.25, -1.00 -5.00 and -20.00 dB
* Displays number of connected devices, system bit depth and sampling rate
* Lists connected interfaces
* Checks bus master clock
* Detects addition and removal of interfaces

### Keyboard Shortcuts
| Key Combination |  Function |
|:---|:---|
| control + shift + = | Increases the gain by 1.00 dB  |
| control + shift + - | Decreases the gain by 1.00 dB  |

### Compatible OS
* Apple - OS X

### Dependency
* [JUCE for MAC](http://www.juce.com/)

### Build Instruction
* Download JUCE and unzip.
* Start Projucer and select "Open Existing Project"
* Open AlloGain.jucer
* Go to the "Config" tab
* Configure the path of the JUCE modules (Should point to where JUCE was unzipped)
* Configure the Debug and Release builds to the correct SDK
* Press "Save Project and Open in Xcode..."
* Build the project in Xcode
