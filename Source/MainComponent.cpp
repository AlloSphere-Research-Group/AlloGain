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
//[/Headers]

#include "MainComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MainContentComponent::MainContentComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    addAndMakeVisible (tb_Plus025 = new TextButton ("+0.25 dB"));
    tb_Plus025->addListener (this);

    addAndMakeVisible (tb_Plus100 = new TextButton ("+1.00 dB"));
    tb_Plus100->addListener (this);

    addAndMakeVisible (tb_Minus025 = new TextButton ("-0.25 dB"));
    tb_Minus025->addListener (this);

    addAndMakeVisible (tb_Minus100 = new TextButton ("-1.00 dB"));
    tb_Minus100->addListener (this);

    addAndMakeVisible (lb_Gain = new Label ("gain",
                                            TRANS("-96.00 dB")));
    lb_Gain->setFont (Font ("PT Sans", 50.00f, Font::bold));
    lb_Gain->setJustificationType (Justification::centred);
    lb_Gain->setEditable (false, false, false);
    lb_Gain->setColour (Label::backgroundColourId, Colours::green);
    lb_Gain->setColour (TextEditor::textColourId, Colours::black);
    lb_Gain->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (tb_Enable = new ToggleButton ("Enable"));
    tb_Enable->setButtonText (String::empty);
    tb_Enable->addListener (this);

    addAndMakeVisible (tb_Plus500 = new TextButton ("+5.00 dB"));
    tb_Plus500->setTooltip (TRANS("Select toggle button to enable."));
    tb_Plus500->addListener (this);

    addAndMakeVisible (tb_Minus500 = new TextButton ("-5 dB"));
    tb_Minus500->setButtonText (TRANS("-5.00 dB"));
    tb_Minus500->addListener (this);

    addAndMakeVisible (tb_Mute = new TextButton ("Mute"));
    tb_Mute->setButtonText (TRANS("MUTE"));
    tb_Mute->addListener (this);
    tb_Mute->setColour (TextButton::buttonColourId, Colours::red);
    tb_Mute->setColour (TextButton::buttonOnColourId, Colours::red);

    addAndMakeVisible (tb_Minus2000 = new TextButton ("-20 dB"));
    tb_Minus2000->setButtonText (TRANS("-20.00 dB"));
    tb_Minus2000->addListener (this);

    addAndMakeVisible (lb_Info = new Label ("info",
                                            TRANS("Infromation:")));
    lb_Info->setFont (Font (15.20f, Font::plain));
    lb_Info->setJustificationType (Justification::centredLeft);
    lb_Info->setEditable (false, false, false);
    lb_Info->setColour (Label::textColourId, Colour (0xffa0a0a0));
    lb_Info->setColour (Label::outlineColourId, Colour (0x00808080));
    lb_Info->setColour (TextEditor::textColourId, Colours::black);
    lb_Info->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (tb_Info = new TextButton ("Interfaces"));
    tb_Info->setButtonText (TRANS("i"));
    tb_Info->addListener (this);

    addAndMakeVisible (tb_Help = new TextButton ("Help"));
    tb_Help->setButtonText (TRANS("h"));
    tb_Help->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 220);


    //[Constructor] You can add your own custom stuff here..

    lookAndFeels.add (new MuteLookAndFeel());
    lookAndFeels.add (new LookAndFeel_V3());

    for (int i = 0; i < this->getNumChildComponents(); ++i)
    {
        if (Component * c = this->getChildComponent(i))
        {
            if (c->getName() == "Mute")
            {
                c->setLookAndFeel (lookAndFeels[0]);
            }
            else
            {
                c->setLookAndFeel (lookAndFeels[1]);
            }
        }
    }

    minimumGain = MINIMUM_GAIN;
    maximumGain = MAXIMUM_GAIN;

    midGainUpperLevel = MID_GAIN_RANGE_UPPER_LIMIT;
    lowGainUpperLevel = LOW_GAIN_RANGE_UPPER_LIMIT;

    currentGain = minimumGain;

    sampleRate = 0.0;
    masterClockCount = 0;

    devices = new ehwlist(0,NULL);
    devices->RegisterMessageListener(this);
    deviceCount = devices->GetNumDevices();

#ifdef NDEBUG
    if (deviceCount == 0)
    {
        getLookAndFeel().setUsingNativeAlertWindows (true);
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Audio Interface Not Found!",
                                          "Add an audio interface to the system and restart the application.",
                                          "OK");
        disableComponents();
    }
#endif

    for(int deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
    {
        deviceList.add(devices->GetNthDevice(deviceNumber));

        int32 outputChannelCount = deviceList[deviceNumber]->getoutputchannelcount();

        for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
        {
            deviceList[deviceNumber]->setmastergain(channelNumber, minimumGain);
        }

        for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
        {
            deviceList[deviceNumber]->setmastermute(channelNumber, MuteState::MUTE);
        }

        int32 clockSource;
        int32 clockRate;
        deviceList[deviceNumber]->getclock(clockRate,clockSource);


        if ( clockSource == 0)
        {
            sampleRate = clockRate;
            masterClockCount++;
        }
    }

#ifdef NDEBUG
    if (masterClockCount == 0)
    {
        getLookAndFeel().setUsingNativeAlertWindows (true);
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "No Clock Source!",
                                          "A clock source was not found. Set a clock master device and restart the application.",
                                          "OK");
        disableComponents();
    }

    if (masterClockCount > 1)
    {

        getLookAndFeel().setUsingNativeAlertWindows (true);
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Too Many Clock Source!",
                                          "There are multiple clock master devices on the system. Only one is allowed.",
                                          "OK");
        disableComponents();
        sampleRate = 0;
    }
#endif


    String info;
    info << "Device Count: " << String(deviceCount);
    info << "     ";
    info << "Bit Depth: 24";
    info << "     ";
    info << "Samling Rate: " << String(sampleRate) << " Hz";
    lb_Info->setText(info, dontSendNotification);

    muteState = TRUE;
    tb_Mute->setButtonText("UNMUTE");
    tb_Mute->setColour(TextButton::buttonColourId, Colours::greenyellow);
    tb_Mute->setColour (TextButton::buttonOnColourId, Colours::green);

    tb_Plus500->setEnabled(FALSE);

    infoWindow = new InfoWindow ("Audio Interfaces", Colours::black, DocumentWindow::closeButton, deviceList);
    helpWindow = new HelpWindow ("Help", Colours::black, DocumentWindow::closeButton);
    //[/Constructor]
}

MainContentComponent::~MainContentComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    infoWindow = nullptr;
    //helpWindow = nullptr;
    //[/Destructor_pre]

    tb_Plus025 = nullptr;
    tb_Plus100 = nullptr;
    tb_Minus025 = nullptr;
    tb_Minus100 = nullptr;
    lb_Gain = nullptr;
    tb_Enable = nullptr;
    tb_Plus500 = nullptr;
    tb_Minus500 = nullptr;
    tb_Mute = nullptr;
    tb_Minus2000 = nullptr;
    lb_Info = nullptr;
    tb_Info = nullptr;
    tb_Help = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    for(int32 deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
    {
        
        int32 outputChannelCount = deviceList[deviceNumber]->getoutputchannelcount();
        
        for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
        {
            deviceList[deviceNumber]->setmastergain(channelNumber, minimumGain);
        }

        for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
        {
            deviceList[deviceNumber]->setmastermute(channelNumber, MuteState::MUTE);
        }
    }

    deviceList.clear();
    delete devices;
    //[/Destructor]
}

//==============================================================================
void MainContentComponent::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff0f0f0f));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MainContentComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    tb_Plus025->setBounds (225, 33, 79, 24);
    tb_Plus100->setBounds (320, 32, 71, 24);
    tb_Minus025->setBounds (225, 73, 79, 24);
    tb_Minus100->setBounds (320, 72, 71, 24);
    lb_Gain->setBounds (24, 24, 168, 144);
    tb_Enable->setBounds (456, 32, 23, 24);
    tb_Plus500->setBounds (480, 32, 88, 24);
    tb_Minus500->setBounds (408, 72, 72, 24);
    tb_Mute->setBounds (225, 113, 343, 55);
    tb_Minus2000->setBounds (496, 72, 72, 24);
    lb_Info->setBounds (16, 184, 480, 24);
    tb_Info->setBounds (512, 184, 24, 24);
    tb_Help->setBounds (544, 184, 24, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MainContentComponent::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == tb_Plus025)
    {
        //[UserButtonCode_tb_Plus025] -- add your button handler code here..
        changeGain(+0.25);
        //[/UserButtonCode_tb_Plus025]
    }
    else if (buttonThatWasClicked == tb_Plus100)
    {
        //[UserButtonCode_tb_Plus100] -- add your button handler code here..
        changeGain(+1.00);
        //[/UserButtonCode_tb_Plus100]
    }
    else if (buttonThatWasClicked == tb_Minus025)
    {
        //[UserButtonCode_tb_Minus025] -- add your button handler code here..
        changeGain(-0.25);
        //[/UserButtonCode_tb_Minus025]
    }
    else if (buttonThatWasClicked == tb_Minus100)
    {
        //[UserButtonCode_tb_Minus100] -- add your button handler code here..
        changeGain(-1.00);
        //[/UserButtonCode_tb_Minus100]
    }
    else if (buttonThatWasClicked == tb_Enable)
    {
        //[UserButtonCode_tb_Enable] -- add your button handler code here..
        if (tb_Enable->getToggleState())
        {
            tb_Plus500->setEnabled(TRUE);
        }
        else
        {
            tb_Plus500->setEnabled(FALSE);
        }
        //[/UserButtonCode_tb_Enable]
    }
    else if (buttonThatWasClicked == tb_Plus500)
    {
        //[UserButtonCode_tb_Plus500] -- add your button handler code here..
        if(tb_Enable->getToggleState())
        {
            changeGain(+5.00);
            tb_Plus500->setEnabled(FALSE);
            tb_Enable->setToggleState(FALSE, dontSendNotification);
        }
        //[/UserButtonCode_tb_Plus500]
    }
    else if (buttonThatWasClicked == tb_Minus500)
    {
        //[UserButtonCode_tb_Minus500] -- add your button handler code here..
        changeGain(-5.00);
        //[/UserButtonCode_tb_Minus500]
    }
    else if (buttonThatWasClicked == tb_Mute)
    {
        //[UserButtonCode_tb_Mute] -- add your button handler code here..
        if (muteState)
        {
            for(int deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
            {
                int32 outputChannelCount = deviceList[deviceNumber]->getoutputchannelcount();

                for(int channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
                {
                    deviceList[deviceNumber]->setmastermute(channelNumber, MuteState::UNMUTE);
                }
            }

            muteState = FALSE;
            tb_Mute->setButtonText("MUTE");
            tb_Mute->setColour(TextButton::buttonColourId, Colours::red);
            tb_Mute->setColour (TextButton::buttonOnColourId, Colours::red);

        }
        else
        {
            for(int32 deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
            {
                int32 outputChannelCount = deviceList[deviceNumber]->getoutputchannelcount();

                for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
                {
                    deviceList[deviceNumber]->setmastermute(channelNumber, MuteState::MUTE);
                }
            }

            muteState = TRUE;
            tb_Mute->setButtonText("UNMUTE");
            tb_Mute->setColour(TextButton::buttonColourId, Colours::greenyellow);
            tb_Mute->setColour (TextButton::buttonOnColourId, Colours::green);
        }

        //[/UserButtonCode_tb_Mute]
    }
    else if (buttonThatWasClicked == tb_Minus2000)
    {
        //[UserButtonCode_tb_Minus2000] -- add your button handler code here..
        changeGain(-20.00);
        //[/UserButtonCode_tb_Minus2000]
    }
    else if (buttonThatWasClicked == tb_Info)
    {
        //[UserButtonCode_tb_Info] -- add your button handler code here..
        infoWindow->setVisible(!infoWindow->isVisible());
        //[/UserButtonCode_tb_Info]
    }
    else if (buttonThatWasClicked == tb_Help)
    {
        //[UserButtonCode_tb_Help] -- add your button handler code here..
        helpWindow->setVisible(!helpWindow->isVisible());
        //[/UserButtonCode_tb_Help]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

bool MainContentComponent::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
    if (key.getKeyCode() == 95 && key.getModifiers().isCtrlDown())
    {
        changeGain(-1.00);
    }
    if (key.getKeyCode() == 43 && key.getModifiers().isCtrlDown())
    {
        changeGain(+1.00);
    }
    return false;  // Return true if your handler uses this key event, or false to allow it to be passed-on.
    //[/UserCode_keyPressed]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void MainContentComponent::changeGain(float gain)
{
    currentGain += gain;
    if (currentGain < minimumGain) currentGain = minimumGain;
    if (currentGain > maximumGain) currentGain = maximumGain;
    lb_Gain->setText(String::formatted("%2.2f",currentGain) + " dB", dontSendNotification);
    if (currentGain < lowGainUpperLevel)
    {
        lb_Gain->setColour(Label::ColourIds::backgroundColourId, Colours::green);
    }
    else if (currentGain >= lowGainUpperLevel && currentGain < midGainUpperLevel)
    {
        lb_Gain->setColour(Label::ColourIds::backgroundColourId, Colours::yellow);
    }
    else
    {
        lb_Gain->setColour(Label::ColourIds::backgroundColourId, Colours::red);
    }
    for(int32 deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
    {
        int32 outputChannelCount = deviceList[deviceNumber]->getoutputchannelcount();

        for(int32 channelNumber = 0; channelNumber < outputChannelCount; channelNumber++)
        {
            deviceList[deviceNumber]->setmastergain(channelNumber, currentGain);
        }
    }
}

void MainContentComponent::disableComponents()
{
    ;
    for (int i = 0; i < this->getNumChildComponents(); i++)
    {
        if (Component * component = this->getChildComponent(i))
        {
            component->setEnabled(false);
        }
    }
}

void MainContentComponent::handleMessage (const Message &message)
{
    const DeviceChangeMessage * dcm = dynamic_cast<const DeviceChangeMessage *> (&message);
    
    if (dcm->action == EHW_DEVICE_ARRIVAL)
    {
        DBG("Device Added!");
        
        getLookAndFeel().setUsingNativeAlertWindows (true);
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Audio Interface Added!",
                                          "An interface was just added to the system. If audio was playing while this event occured, please stop the source first. Then proceed to closing and restarting this application.",
                                          "OK");
        
        deviceList.clear();
        deviceCount = devices->GetNumDevices();
        
        for (int32 deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
        {
            deviceList.add(devices->GetNthDevice(deviceNumber));
        }
        
        DBG(String::formatted("Total: %d",deviceCount));
        
        disableComponents();
    }
    
    if (dcm->action == EHW_DEVICE_REMOVAL)
    {
        DBG("Device Removed!");
        
        getLookAndFeel().setUsingNativeAlertWindows (true);
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "Audio Interface Removed!",
                                          "An interface was just removed from the system. If audio was playing while this event occured, please stop the source first. Then proceed to closing and restarting this application.",
                                          "OK");
        
        deviceList.clear();
        deviceCount = devices->GetNumDevices();
        
        for (int32 deviceNumber = 0; deviceNumber < deviceCount; deviceNumber++)
        {
            deviceList.add(devices->GetNthDevice(deviceNumber));
        }
        
        DBG(String::formatted("Total: %d",deviceCount));
        
        disableComponents();
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainContentComponent" componentName=""
                 parentClasses="public Component, public MessageListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="1" initialWidth="600" initialHeight="220">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff0f0f0f"/>
  <TEXTBUTTON name="+0.25 dB" id="fb6350fda5208324" memberName="tb_Plus025"
              virtualName="" explicitFocusOrder="0" pos="225 33 79 24" buttonText="+0.25 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="+1.00 dB" id="710c96e8d114efe1" memberName="tb_Plus100"
              virtualName="" explicitFocusOrder="0" pos="320 32 71 24" buttonText="+1.00 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="-0.25 dB" id="acb3525b0142df96" memberName="tb_Minus025"
              virtualName="" explicitFocusOrder="0" pos="225 73 79 24" buttonText="-0.25 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="-1.00 dB" id="7072e256f04476e1" memberName="tb_Minus100"
              virtualName="" explicitFocusOrder="0" pos="320 72 71 24" buttonText="-1.00 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="gain" id="4825762310f4b09f" memberName="lb_Gain" virtualName=""
         explicitFocusOrder="0" pos="24 24 168 144" bkgCol="ff008000"
         edTextCol="ff000000" edBkgCol="0" labelText="-96.00 dB" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="PT Sans"
         fontsize="50" bold="1" italic="0" justification="36"/>
  <TOGGLEBUTTON name="Enable" id="f73de0c54ffa3fee" memberName="tb_Enable" virtualName=""
                explicitFocusOrder="0" pos="456 32 23 24" buttonText="" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="0"/>
  <TEXTBUTTON name="+5.00 dB" id="f809e4c0663eb560" memberName="tb_Plus500"
              virtualName="" explicitFocusOrder="0" pos="480 32 88 24" tooltip="Select toggle button to enable."
              buttonText="+5.00 dB" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="-5 dB" id="9905071a95e54af3" memberName="tb_Minus500" virtualName=""
              explicitFocusOrder="0" pos="408 72 72 24" buttonText="-5.00 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Mute" id="ecdd750c6cc35574" memberName="tb_Mute" virtualName=""
              explicitFocusOrder="0" pos="225 113 343 55" bgColOff="ffff0000"
              bgColOn="ffff0000" buttonText="MUTE" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="-20 dB" id="ea9b9b5eee0d8bf5" memberName="tb_Minus2000"
              virtualName="" explicitFocusOrder="0" pos="496 72 72 24" buttonText="-20.00 dB"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="info" id="12525ea3acbb37a" memberName="lb_Info" virtualName=""
         explicitFocusOrder="0" pos="16 184 480 24" textCol="ffa0a0a0"
         outlineCol="808080" edTextCol="ff000000" edBkgCol="0" labelText="Infromation:"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15.199999999999999289" bold="0"
         italic="0" justification="33"/>
  <TEXTBUTTON name="Interfaces" id="1c77213455aedee6" memberName="tb_Info"
              virtualName="" explicitFocusOrder="0" pos="512 184 24 24" buttonText="i"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Help" id="26a1e9d7448ae123" memberName="tb_Help" virtualName=""
              explicitFocusOrder="0" pos="544 184 24 24" buttonText="h" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
