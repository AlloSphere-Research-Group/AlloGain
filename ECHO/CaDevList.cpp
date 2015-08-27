#include "base.h"
#include "ehw.h"
#include "ehwlist.h"
#include "CaDevList.h"
#include "CfHelpers.h"
#include <IOKit/IOKitLib.h>

CCaDevList CoreAudioDeviceList;

CCaDevList::CCaDevList()
{
	_numids = 0;
	_caids = NULL;
	_array_size = 0;
	
	AudioHardwareAddPropertyListener(kAudioHardwarePropertyDevices,AudioDevListListener,this);
		
	BuildList();
}


void CCaDevList::BuildList()
{	
	ScopedLock lock(_cs);
	
	//
	// Get the list of all CoreAudio device IDs
	//
	OSStatus status;
	UInt32 bytes;
	Boolean writeable;
	
	status = AudioHardwareGetPropertyInfo(kAudioHardwarePropertyDevices,&bytes,&writeable);
	if (0 != status)
		return;
		
	_numids = bytes / sizeof(AudioDeviceID);
	if (_numids > _array_size)
	{
		if (_caids)
			delete[] _caids;
			
		_array_size = _numids;
		_caids = new AudioDeviceID[ _numids ];
	}
	
	status = AudioHardwareGetProperty( kAudioHardwarePropertyDevices, &bytes, _caids);
	if (0 != status)
		return;
		
	//
	// Get the unique ID for each device
	//
	int i;
	
	for (i = 0; i < _numids; i++)
	{
		//
		// get the unique ID
		//
		CfString deviceUID;
		UInt32 propertyDataSize = sizeof(deviceUID);
		
		status = AudioDeviceGetProperty(_caids[i], 0, 0,
										kAudioDevicePropertyDeviceUID, 
										 &propertyDataSize, &deviceUID._ref);
		if (0 != status)
		{
			_numids = 0;
			return;
		}
			
		//
		// convert the unique ID
		//
		String str( String::fromCFString( deviceUID._ref ) );
		_uids.set(i,str);
	}
	
	//Dump();
}


CCaDevList::~CCaDevList()
{
	if (_caids)
		delete[] _caids;
		
	AudioHardwareRemovePropertyListener(kAudioHardwarePropertyDevices,AudioDevListListener);
}


void CCaDevList::Dump()
{
	ScopedLock lock(_cs);
	
	Logger::outputDebugString(String::formatted("Device list count %d",_numids));

	int i;
	
	for (i = 0; i < _numids; i++)
	{
		Logger::outputDebugString(_uids[i]);
	}
}


AudioDeviceID CCaDevList::MatchUid(String &uid)
{
	int i;
	
	for (i = 0; i < _numids; i++)
	{
		if (0 == uid.compareIgnoreCase(_uids[i]))
			return _caids[i];
	}

	return 0xffffffff;
}


AudioDeviceID CCaDevList::GetCaId(CFNumberRef cfguid)
{
	ScopedLock lock(_cs);

	//
	// convert the GUID to a String
	//
	uint64 guid;
	String uid;
	
	CFNumberGetValue( cfguid, kCFNumberSInt64Type, &guid);
	uid = String::formatted("AppleFWAudioEngineGUID:%llu",guid);
	
	//
	// search for a matching unique CA ID
	//
	return MatchUid(uid);
}



OSStatus CCaDevList::AudioDevListListener(AudioHardwarePropertyID inPropertyID,void *context)
{
	DBG_PRINTF(("AudioDevListListener %d",inPropertyID));
	
	CoreAudioDeviceList.BuildList();
	
	return noErr;
}
