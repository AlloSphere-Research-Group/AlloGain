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

#ifndef __JUCE_HEADER_CB87035FA43504BB__
#define __JUCE_HEADER_CB87035FA43504BB__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "MainComponentHelper.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainContentComponent  : public Component,
                              public MessageListener,
                              public ButtonListener
{
public:
    //==============================================================================
    MainContentComponent ();
    ~MainContentComponent();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);
    bool keyPressed (const KeyPress& key);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    ehwlist * devices;
    Array<ehw *> deviceList;
    int32 deviceCount;
    int32 masterClockCount;

    float minimumGain;
    float maximumGain;
    float lowGainUpperLevel;
    float midGainUpperLevel;
    float currentGain;
    float sampleRate;
    
    bool muteState;
    bool deviceCountChange;
    
    void changeGain(float gain);
    void disableComponents();

    ModifierKeys modifier;
    OwnedArray<LookAndFeel> lookAndFeels;
    ScopedPointer<DocumentWindow> infoWindow;
    ScopedPointer<DocumentWindow> helpWindow;
    void handleMessage (const Message &message);
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<TextButton> tb_Plus025;
    ScopedPointer<TextButton> tb_Plus100;
    ScopedPointer<TextButton> tb_Minus025;
    ScopedPointer<TextButton> tb_Minus100;
    ScopedPointer<Label> lb_Gain;
    ScopedPointer<ToggleButton> tb_Enable;
    ScopedPointer<TextButton> tb_Plus500;
    ScopedPointer<TextButton> tb_Minus500;
    ScopedPointer<TextButton> tb_Mute;
    ScopedPointer<TextButton> tb_Minus2000;
    ScopedPointer<Label> lb_Info;
    ScopedPointer<TextButton> tb_Info;
    ScopedPointer<TextButton> tb_Help;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_CB87035FA43504BB__
