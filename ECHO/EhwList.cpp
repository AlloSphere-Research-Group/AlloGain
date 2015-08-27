/********************************************************************************
 *
 * ehwlist.cpp
 *
 * A class that builds a list of Echo hardware objects.
 *
 * Code modified by JTILBIAN (20150820) - Changes Marked
 *
 ********************************************************************************/

//#include "../base.h"
#include "ehw.h"
#include "ehwlist.h"
#include "CaDevList.h"
#include "CfHelpers.h"


#ifdef AF_BUILD

#define NUM_VALID_DEV_NAMES		0

#endif


#ifdef ONYX_BUILD

#define NUM_VALID_DEV_NAMES		2

static CFStringRef validnames[ NUM_VALID_DEV_NAMES ] = 
{
	CFSTR("Onyx 400F"),
	CFSTR("Onyx 1200F")
};
#endif

#define ECHO_VENDOR_ID		0x001486


//******************************************************************************
// ehwlist methods
//******************************************************************************

ehwlist::ehwlist(int num_vendor_ids, uint32 *vendor_ids,CriticalSection *lock) :
	_cs(lock),
	localLock(false)
{
	DBG_PRINTF(("ehwlist::ehwlist"));
	
	if (NULL == lock)
	{
		_cs = new CriticalSection;
		localLock = true;
	}
	
	_hwlist = NULL;
	_numdevices = 0;
	_dict = NULL;
	_IterMatching = 0;
	_IterTerminated = 0;
	
    if (num_vendor_ids != 0)
    {
        for (int i = 0; i < num_vendor_ids; i++)
        {
            _vendor_ids.add(vendor_ids[i]);
        }
    }
    else
    {
        _vendor_ids.add((uint32)ECHO_VENDOR_ID);
    }
    
	findboxes();

#if MAC_DEVICE_MENU	
	MenuBarModel::setMacMainMenu(this);
#endif
	
} // constructor



ehwlist::~ehwlist()
{
	DBG_PRINTF(("ehwlist::~ehwlist"));
	
#if MAC_DEVICE_MENU
	MenuBarModel::setMacMainMenu(NULL);
#endif
	
	Cleanup(NULL);
	
	if (_dict)
		CFRelease(_dict);
	
	if (_IterMatching)
		IOObjectRelease(_IterMatching);	
		
	if (_IterTerminated)
		IOObjectRelease(_IterTerminated);
	
	if (localLock)
		delete _cs;
					
} // destructor



//******************************************************************************
//
// Cleanup
//
//******************************************************************************

void ehwlist::Cleanup(class ehw *pDeviceToKeep)
{
	ScopedLock lock(*_cs);
	
	//
	// If the specified device is non-NULL, remove it from the list
	//
	if (NULL != pDeviceToKeep)
	{
		ehw *prev,*next;
		
		prev = pDeviceToKeep->_prev;
		next = pDeviceToKeep->_next;
		
		if (prev)
		{
			prev->_next = next;
			pDeviceToKeep->_prev = NULL;
		}
		
		if (next)
		{
			next->_prev = prev;
			pDeviceToKeep->_next = NULL;
		}
	}

	//
	// Dump the linked list except for the device passed in
	// pDeviceToKeep
	//
	ehw *pTemp;

	pTemp = _hwlist;
	while (NULL != pTemp)
	{
		ehw::ptr pDead;
		
		pDead = pTemp;
		pDead->decReferenceCount();
		pTemp = pTemp->_next;
	}
	
	_hwlist = pDeviceToKeep;
}





//******************************************************************************
//
// validate a device name
//
//******************************************************************************

bool ehwlist::validate(CFStringRef name)
{
#if 0 != NUM_VALID_DEV_NAMES
	int i;

	for (i = 0; i < NUM_VALID_DEV_NAMES; i++)
	{
		if (CFStringHasPrefix(name,validnames[i]))
			return true;
	}

	return false;
#else
	return true;
#endif
}

//******************************************************************************
//
// add a device to the list
//
//******************************************************************************

OSStatus ehwlist::add(io_object_t iodev,CFStringRef name)
{
	CFNumberRef cfguid;
	
	cfguid = (CFNumberRef) IORegistryEntryCreateCFProperty(iodev,CFSTR("GUID"),NULL,0);
	if (NULL == cfguid)
		return -1;
	
	//
	// scan the existing list; see if this GUID is already present
	//
	ehw *dev;

	dev = _hwlist;
	while (dev != NULL)
	{
		if (kCFCompareEqualTo == CFNumberCompare(cfguid,dev->GetGuid(),NULL))
		{
			CFRelease(cfguid);
			return noErr;
		}
		
		dev = dev->_next;
	}

/*
	//
	// get the CoreAudio device name
	//
	CFStringRef cfname;
	String name;
	UInt32 size;
	OSStatus rval;
	
	size = sizeof(cfname);	
	rval = AudioDeviceGetProperty(	caid,
									0,
									false,
									kAudioDevicePropertyDeviceNameCFString,
									&size,
									&cfname);
									
	if (noErr != rval)
	{
		DBG_PRINTF((T("Failed on AudioDeviceGetProperty for kAudioDevicePropertyDeviceNameCFString - rval %d"),rval));
		return rval;
	}
	
	name = PlatformUtilities::cfStringToJuceString(cfname);
	CFRelease(cfname);
*/
	
	//
	// make a new ehw with the supplied info
	//
	ehw *box;
	CriticalSection *device_lock;
	
	if (localLock)
		device_lock = NULL;
	else
		device_lock = _cs;
	
	box = new ehw(iodev,cfguid,name,device_lock);
	OSStatus status = box->init();
	if (noErr == status)
	{
		if (_hwlist)
			_hwlist->_prev = box;
		box->_next = _hwlist;
		_hwlist = box;
		_numdevices++;
		
		box->incReferenceCount();
	}
	else
	{
		delete box;
		//CFRelease(cfguid);
        return status;
	}
	
	//
	// Tell JUCE-world that a new device is here
	//
	PostArrivalMessage(box);
	
	return status;
}


//******************************************************************************
//
// Call this function to look at all the devices; index ranges from
// 0 to GetNumDevices()-1
//
//******************************************************************************

ehw *ehwlist::GetNthDevice(int32 index)
{
	ScopedLock lock(*_cs);

	ehw *pTemp = _hwlist;
	int32 count = 0;

	while (pTemp != NULL)
	{
		if (count == index)
			break;

		count++;
		pTemp = pTemp->_next;
	}

	return pTemp;

} // GetNthDevice



//******************************************************************************
//
// register device remove and add listener
//
//******************************************************************************

void ehwlist::RegisterMessageListener(MessageListener *listener)
{
	_listeners.addIfNotAlreadyThere(listener);
}


void ehwlist::UnregisterMessageListener(MessageListener *listener)
{
	_listeners.remove( _listeners.indexOf(listener) );
}


//******************************************************************************
//
// Make IOService iterators to find all the AVC units
//
//******************************************************************************

void ehwlist::findboxes()
{
	CfNumber vendorid;
		
	//
	// Make a dictionary
	//
	_dict = IOServiceMatching("AppleRemoteAudioDevice");		
	if (NULL == _dict)
		return;

	CFRunLoopSourceRef rl = NULL;
	kern_return_t rval;
	
	_np = IONotificationPortCreate(kIOMasterPortDefault);
	if (NULL == _np)
		return;
	
	rl = IONotificationPortGetRunLoopSource(_np);
	if (NULL == rl)
		return;
		
	CFRunLoopAddSource(CFRunLoopGetCurrent(), rl,kCFRunLoopDefaultMode);

	//
	// Device arrival notification
	//
	// IOServiceAddMatchingNotification consumes a reference to _dict,
	// so retain it first
	//
	CFRetain(_dict);
	rval = IOServiceAddMatchingNotification(_np,
											kIOMatchedNotification,
											_dict,
											ServiceMatched,
											this,
											&_IterMatching);
	if (0 != rval)
		return;
											
	//
	// Walk through the device arrival iterator and look for devices
	//
	ServiceMatched(this,_IterMatching);
											
	//
	// Device removal notification
	//
	CFRetain(_dict);
	rval = IOServiceAddMatchingNotification(_np,
											kIOTerminatedNotification,
											_dict,
											ServiceTerminated,
											this,
											&_IterTerminated);	
	if (0 != rval)
		return;
	
	//
	// Clear out the device removal iterator; have to do this on startup
	// or it doesn't work properly
	//
	ServiceTerminated(this,_IterTerminated);
}

//******************************************************************************
//
// Recursive IO reg parent finder
//
//******************************************************************************

io_object_t FindIoRegParent(io_object_t child,CFStringRef cfname)
{
	kern_return_t rval;
	io_registry_entry_t parent,temp;
	io_name_t parent_str;
	CFStringRef parent_cfstr;
	
	rval = IORegistryEntryGetParentEntry(child,kIOServicePlane,&parent);
	if (0 != rval)
	{
		return 0;
	}
	
	rval = IOObjectGetClass(parent,parent_str);
	if (0 != rval)
	{
		IOObjectRelease(parent);
		return 0;
	}
		
	parent_cfstr = CFStringCreateWithCString(NULL,parent_str,kCFStringEncodingASCII);
	if (NULL == parent_cfstr)
	{
		IOObjectRelease(parent);
		return 0;
	}
	
	if (kCFCompareEqualTo == CFStringCompare(parent_cfstr,cfname,0))
	{
		CFRelease(parent_cfstr);
		return parent;
	}
	
	temp = parent;
	parent = FindIoRegParent(temp,cfname);
	IOObjectRelease(temp);

	return parent;
}


//******************************************************************************
//
// Device arrival callback
//
//******************************************************************************

void ehwlist::ServiceMatched(void *context,io_iterator_t iterator)
{
	io_object_t audiodev;
	ehwlist *that = (ehwlist *) context;
	ScopedLock lock(*(that->_cs));
	
	Logger::outputDebugString("ehwlist::ServiceMatched");

	while ((audiodev = IOIteratorNext(iterator)))
	{
		//
		// Get and check the audio device name
		//
		CfString name;
		bool nameok;

		name._ref = (CFStringRef) IORegistryEntryCreateCFProperty(	audiodev,
																	CFSTR("IOAudioDeviceName"),
																	NULL,0);
		if (NULL == name._ref)
		{
			IOObjectRelease(audiodev);
			continue;
		}
		
		nameok = that->validate(name._ref);
		if (false == nameok)
		{
			Logger::outputDebugString("ehwlist::ServiceMatched - name invalid");
			IOObjectRelease(audiodev);
			continue;
		}
			
		//
		// Find the AVC unit parent
		//
		io_object_t avcunit;

		avcunit = FindIoRegParent(audiodev,CFSTR("IOFireWireAVCUnit"));
		if (0 == avcunit)
		{
			Logger::outputDebugString("ehwlist::ServiceMatched - no parent");
			IOObjectRelease(audiodev);
			continue;
		}
		
		//
		// Get the 1394 vendor ID
		//
		CfNumber vendor;
		SInt32 id;
		Boolean ok;
		
		vendor._ref = (CFNumberRef) IORegistryEntryCreateCFProperty(avcunit,CFSTR("Vendor_ID"),NULL,0);
		if (NULL == vendor._ref)
		{
			Logger::outputDebugString("ehwlist::ServiceMatched - cannot read vendor ID");
			IOObjectRelease(audiodev);
			continue;
		}

		ok = CFNumberGetValue(vendor._ref,kCFNumberSInt32Type,&id);
		if (!ok)
		{
			Logger::outputDebugString("ehwlist::ServiceMatched - CFNumberGetValue failed");
			IOObjectRelease(audiodev);
			continue;
		}
		
		//
		// Check the vendor ID
		//
		ok = false;
		for (int i = 0; i < that->_vendor_ids.size(); i++)
		{
			if (id == that->_vendor_ids[i])
			{
				ok = true;
				break;
			}
		}
		if (!ok)
		{
			IOObjectRelease(audiodev);
			continue;
		}
		
		//
		// add this device to the list
		//
		that->add(avcunit,name._ref);
		
		//
		// clean up
		//
		IOObjectRelease(avcunit);
		IOObjectRelease(audiodev);
	}
}


//******************************************************************************
//
// Device removal callback
//
//******************************************************************************

void ehwlist::ServiceTerminated(void *context,io_iterator_t iterator)
{
	io_object_t audiodev;
	ehwlist *that = (ehwlist *) context;
	ScopedLock lock(*(that->_cs));
	
	//Logger::outputDebugString("ehwlist::ServiceTerminated");

	while ((audiodev = IOIteratorNext(iterator)))
	{
		DBG_PRINTF(("ServiceTerminated - audiodev %p",audiodev));
		
		//
		// Read the GUID
		//
		CfNumber cfguid;
	
		cfguid._ref = (CFNumberRef) IORegistryEntryCreateCFProperty(audiodev,CFSTR("GUID"),NULL,0);
		if ((NULL != cfguid._ref) && (that->_listeners.size()))
		{
			ehw *dev;
		
			//
			// Search the ehw list for a match
			//
			dev = that->_hwlist;
			while (dev != NULL)
			{
				ehw *prev,*next;
				
				prev = dev->_prev;
				next = dev->_next;

				if (kCFCompareEqualTo == CFNumberCompare(cfguid._ref,dev->GetGuid(),NULL))
				{
					//
					// Prune the linked list`
					//
					if (prev)
						prev->_next = next;
					if (next)
						next->_prev = prev;
					if (that->_hwlist == dev)
						that->_hwlist = next;
						
					that->_numdevices--;
						
					//
					// Tweak the ref count
					//
					dev->decReferenceCount();
				
					//
					// Tell the app what happened
					//
					that->PostRemovalMessage(dev);
					
					break;
				}
				
				dev = next;
			}
		}
		
		IOObjectRelease(audiodev);
	}
}

//******************************************************************************
//
// Post a message informing JUCE-world that a device has arrived
//
//******************************************************************************

void ehwlist::PostArrivalMessage(ehw *dev)
{
#if 1
	for (int index = 0; index < _listeners.size(); index++)
	{
		// JTILBIAN: Constructor arguments changed
        Message *msg = new DeviceChangeMessage(EHW_DEVICE_ARRIVAL,dev);
		_listeners[index]->postMessage(msg);
	}
#endif
}


//******************************************************************************
//
// Post a message informing JUCE-world that a device has departed
//
//******************************************************************************

void ehwlist::PostRemovalMessage(ehw *dev)
{
#if 1
	for (int index = 0; index < _listeners.size(); index++)
	{
        // JTILBIAN: Constructor arguments changed
        Message *msg = new DeviceChangeMessage(EHW_DEVICE_REMOVAL,dev);
		_listeners[index]->postMessage(msg);
	}
#endif
    
}
