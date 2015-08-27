/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.2.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "ehw.h"
//[/Headers]

#include "Info.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Info::Info (Array<ehw *> deviceList)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (textEditor = new TextEditor ("new text editor"));
    textEditor->setMultiLine (true);
    textEditor->setReturnKeyStartsNewLine (false);
    textEditor->setReadOnly (true);
    textEditor->setScrollbarsShown (true);
    textEditor->setCaretVisible (false);
    textEditor->setPopupMenuEnabled (false);
    textEditor->setColour (TextEditor::textColourId, Colours::black);
    textEditor->setColour (TextEditor::backgroundColourId, Colours::grey);
    textEditor->setText (String::empty);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (220, 420);


    //[Constructor] You can add your own custom stuff here..

    String text;
    text << "Device Count: ";
    text << String (deviceList.size());
    text << newLine;
    text << newLine;


    for (int device = 0; device < deviceList.size(); device++)
    {

        int32 clockSource;
        int32 clockRate;
        deviceList[device]->getclock(clockRate,clockSource);

        text << "Device Number: ";
        text << String(device + 1);
        text << newLine;
        text << "Device Name: ";
        text << String (deviceList[device]->GetUniqueName());
        text << newLine;
        text << "Clock: ";
        if (clockSource == 0)
        {
            text << "Internal";
            text << newLine;
            text << "Sampling Rate: ";
            text << String(clockRate);

        }
        else if (clockSource == 2)
        {
            text << "Word";
        }
        else
        {
            text << "Unknown";
        }
        text << newLine;
        text << newLine;

        DBG(newLine);
        DBG(deviceList[device]->GetUniqueName());
        DBG(String::formatted("Output Channels: %d",deviceList[device]->getoutputchannelcount()));
        DBG(String::formatted("clockSource: %d, clockRate: %d", clockSource, clockRate));
        DBG(String::formatted("Box Type: %d",deviceList[device]->GetBoxType()));
        DBG(String::formatted("Box Rev: %d",deviceList[device]->GetBoxRev()));
        DBG(String::formatted("Serial Number: %d",deviceList[device]->GetSerialNumber()));
        DBG("Firmware: " + deviceList[device]->getFirmwareVersionString());
    }

    textEditor->setText(text);
    //[/Constructor]
}

Info::~Info()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    textEditor = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Info::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::grey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Info::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    textEditor->setBounds (2, 2, 216, 414);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Info" componentName="" parentClasses="public Component"
                 constructorParams="Array&lt;ehw *&gt; deviceList" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="220" initialHeight="420">
  <BACKGROUND backgroundColour="ff808080"/>
  <TEXTEDITOR name="new text editor" id="f5d407433779f919" memberName="textEditor"
              virtualName="" explicitFocusOrder="0" pos="2 2 216 414" textcol="ff000000"
              bkgcol="ff808080" initialText="" multiline="1" retKeyStartsLine="0"
              readonly="1" scrollbars="1" caret="0" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
