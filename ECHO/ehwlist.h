/********************************************************************************
 *
 * ehwlist.h
 *
 * A class that builds a list of Echo FireWire hardware objects for OS X
 *
 * Code modified by JTILBIAN (20150820) - Changes Marked
 *
 ********************************************************************************/

#ifndef _ehwlist_h_
#define _ehwlist_h_

#include <IOKit/IOKitLib.h>
#include <CoreAudio/AudioHardware.h>

enum 
{
	EHW_DEVICE_ARRIVAL = 0xecc00000,
	EHW_DEVICE_REMOVAL,
	ARRANGE_WINDOWS = 'arng'
};

class ehw;
#if MAC_DEVICE_MENU
class ehwlist : public MenuBarModel
#else
class ehwlist
#endif
{
protected:
	
	Array<uint32>				_vendor_ids;
	class ehw					*_hwlist;
	int32						_numdevices;
	
	Array<MessageListener *> _listeners;
	
	void findboxes();
	bool validate(CFStringRef name);
	
	static void ServiceMatched(void *context,io_iterator_t iterator);
	OSStatus add( io_object_t iodev,CFStringRef name);
	void PostArrivalMessage(ehw *dev);

	static void ServiceTerminated(void *context,io_iterator_t iterator);
	void PostRemovalMessage(ehw *dev);
	
	CriticalSection *_cs;
	bool localLock;
	IONotificationPortRef _np;
	CFMutableDictionaryRef _dict;
	io_iterator_t _IterMatching;
	io_iterator_t _IterTerminated;
	
public:
	ehwlist(int num_vendor_ids, uint32 *vendor_ids,CriticalSection *lock = NULL);
	virtual ~ehwlist();
	
	int32 GetNumDevices()
	{
		return _numdevices;
	}

	class ehw *GetNthDevice(int32 index);
	
	virtual void Cleanup(class ehw *pDeviceToKeep);
	void RegisterMessageListener(MessageListener *listener);
	void UnregisterMessageListener(MessageListener *listener);
	
#if MAC_DEVICE_MENU
	virtual const StringArray getMenuBarNames();
	virtual const PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName);
	virtual void menuItemSelected (int menuItemID, int topLevelMenuIndex);
#endif
};

// JTILBIAN: Constructor arguments changed
// JTILBIAN: Class memebers changes
class DeviceChangeMessage : public Message
{
public:
    DeviceChangeMessage(int action,void* device):
    action(action),
    device(device)
    {
    }
    int action;
    void * device;
    
    JUCE_LEAK_DETECTOR (DeviceChangeMessage)
};

#endif // _ehwlist_h_
