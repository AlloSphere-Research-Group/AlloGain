/*
  ==============================================================================

    MainComponentHelper.h
    Created: 20 Aug 2015
    Author:  Joseph Tilbian

  ==============================================================================
*/

#include "base.h"
#include "ehw.h"
#include "ehwlist.h"
#include "Info.h"
#include "Help.h"
#include "Config.h"

#ifndef MAINCOMPONENTHELPER_H_INCLUDED
#define MAINCOMPONENTHELPER_H_INCLUDED

// Look And Feel for Mute Button
struct MuteLookAndFeel    : public LookAndFeel_V3
{
    Font getTextButtonFont (TextButton&, int buttonHeight)
    {
        return Font (jmin (30.0f, buttonHeight * 0.8f));
    }
};

// Information Window
class InfoWindow: public DocumentWindow
{
public:
    InfoWindow (const String& name, Colour backgroundColour, int buttonsNeeded, Array<ehw *> deviceList)
    : DocumentWindow (name, backgroundColour, buttonsNeeded)
    {
        setUsingNativeTitleBar (true);
        setContentOwned(new Info(deviceList), true);
        centreWithSize (getWidth(), getHeight());
        setCentreRelative(0.8f, 0.5f);
        setVisible (false);
        setResizable(false,false);
    }
    
    ~InfoWindow()
    {
    }
    
    void closeButtonPressed()
    {
        this->setVisible(false);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoWindow)
};

// Help Window
class HelpWindow: public DocumentWindow
{
public:
    HelpWindow (const String& name, Colour backgroundColour, int buttonsNeeded)
    : DocumentWindow (name, backgroundColour, buttonsNeeded)
    {
        setUsingNativeTitleBar (true);
        setContentOwned(new Help(), true);
        centreWithSize (getWidth(), getHeight());
        setCentreRelative(0.5f, 0.8f);
        setVisible (false);
        setResizable(false,false);
    }
    
    ~HelpWindow()
    {
    }
    
    void closeButtonPressed()
    {
        this->setVisible(false);
    }
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HelpWindow)
};



#endif  // MAINCOMPONENTHELPER_H_INCLUDED
