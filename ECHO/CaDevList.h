#ifndef _CCaDevList_h_
#define _CCaDevList_h_

class CCaDevList
{
public :
	CCaDevList();
	~CCaDevList();
	
	void BuildList();
	void Dump();
	
    AudioDeviceID GetCaId(CFNumberRef cfguid);

protected :
	static OSStatus AudioDevListListener(AudioHardwarePropertyID inPropertyID,void *context);
	AudioDeviceID MatchUid(String &uid);

	CriticalSection	_cs;
	int				_numids;
	size_t			_array_size;
	AudioDeviceID	*_caids;
	StringArray		_uids;
};

extern CCaDevList CoreAudioDeviceList;

#endif

