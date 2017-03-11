/********************************************************************************
 *
 * ehw.h
 *
 * A single Echo FireWire hardware device on Mac OS X
 *
 * Code modified by JTILBIAN (20150820) - Changes Marked
 *
 ********************************************************************************/

#ifndef _Ehw_h_
#define _Ehw_h_


#include <IOKit/IOTypes.h>
#include <IOKit/IOCFPlugin.h>
#include <IOKit/firewire/IOFireWireLib.h>
#include <IOKit/avc/IOFireWireAVCLib.h>
#include <CoreAudio/CoreAudio.h>
#include "base.h"
#include "ehwlist.h"
#include "CoreEFC.h"

#ifndef DBG_PRINTF
#define DBG_PRINTF(x) DBG(String::formatted x )
#endif

//
// ehw for Mac FireWire
//
class ehwlist;
class hwcaps;
class ehw : public ReferenceCountedObject
{
protected:
	friend class ehwlist;
	
	enum
	{
		max_async_packet_bytes = 2048
	};
	
	const static int AVC_COMMAND_OFFSET	= 8;
	
	int32							_ok;
	CFNumberRef						_guid;
	IOCFPlugInInterface				**_fwplugin;
	IOFireWireAVCLibUnitInterface	**_fwavc;
	AudioDeviceID					_CoreAudioId;
	
	hwcaps *_caps;
	
	char _devname[ 64 ];
	io_object_t _avcsvc;
	
	class ehw *_prev;
	class ehw *_next;
	
	UInt8 _outbuff[ sizeof(efc) +  AVC_COMMAND_OFFSET ];
	UInt8 _inbuff[ sizeof(efr) +  AVC_COMMAND_OFFSET ];
	UInt8 _polledbuff[ max_async_packet_bytes ];
	
	#ifdef JUCE_LITTLE_ENDIAN
	UInt8 _outbuff_le[ sizeof(efc) + AVC_COMMAND_OFFSET  ];
	UInt8 _inbuff_le[ max_async_packet_bytes ];
	#endif
	
	efc *_cmd;
	efr *_rsp;
	
	uint32 _seqnum;
	
	uint32				_efrpolledstuffbytes;
	
	CriticalSection *_cs;
	bool localLock;
	
	int SendEfc(size_t outbytes,size_t inbytes);
	int readcaps();
    
    OSStatus CreatePlugin(FileLogger *log = NULL);
    void DestroyPlugin();
	
public:
	typedef ReferenceCountedObjectPtr<ehw> ptr;

	ehw(io_object_t dev,CFNumberRef guid,CFStringRef cfname,CriticalSection *device_lock);
	~ehw();
	
	OSStatus init();
	
	CFNumberRef GetGuid()
	{
		return _guid;
	}
	
	hwcaps *getcaps()
	{
		return _caps;
	}
	
	String GetIdString()
	{
		return _devname;
	}
	
	const char *GetUniqueName()
	{
		return (const char *) _devname;
	}
	
	uint32 GetVendorId();
	uint32 GetBoxType();	
	uint32 GetBoxRev();
    uint64 GetSerialNumber();


	inline efr_polled_stuff* getpolledstuff()
	{
		return (efr_polled_stuff *) (_polledbuff + AVC_COMMAND_OFFSET);
	}

	int updatepolledstuff();
	
	void OpenDriver() {}
    void CloseDriver() {}
	
	int getarmversion(uint32 &armversion);
    String getFirmwareVersionString();
    
    //int ResetInterface(FileLogger *log = NULL);
    
    int efctest(uint32 value,uint32 &result);
	
    //------------------------------------------------------------------------------
    // Added by JTILBIAN (20150820)
    //------------------------------------------------------------------------------
    
    int32 getoutputchannelcount();

	//------------------------------------------------------------------------------
	// Hardware controls
	//------------------------------------------------------------------------------
	
	enum
	{
		spdif_pro_format = 2	// Matches the 1394 driver
	};

	int changeboxflags(uint32 setmask,uint32 clearmask);
	int getboxflags(uint32 &flags);

	int getmirror(int &output);
	int setmirror(int output);

	int getdigitalmode(int &mode);
	int setdigitalmode(int mode);
	
	int getphantom(int &phantom);
	int setphantom(int phantom);

	int getisocmap(efr_isoc_map *map);
	int setisocmap(efc_isoc_map *map);
	
	int identify();
	
	void reconnectphy();
	void busreset();


	//------------------------------------------------------------------------------
	// Input controls
	//------------------------------------------------------------------------------

	int getinshift(int input,int &shift);
	int setinshift(int input,int shift);


	//------------------------------------------------------------------------------
	// Monitor controls
	//------------------------------------------------------------------------------
	
	int getmonmute(uint32 input,uint32 output,uint32 &mute);
	int setmonmute(uint32 input,uint32 output,uint32 mute);

	int getmongain(uint32 input,uint32 output,float &gain);
	int setmongain(uint32 input,uint32 output,float gain);

	int getmonsolo(uint32 input,uint32 output,uint32 &solo);
	int setmonsolo(uint32 input,uint32 output,uint32 solo);
	
	int setmonpan(uint32 input,uint32 output,float pan);
	int getmonpan(uint32 input,uint32 output,float &pan);

	
	//------------------------------------------------------------------------------
	// Playback mixer controls
	//------------------------------------------------------------------------------

	int setplaygain(uint32 virtout,uint32 output,float gain);
	int setplaymute(uint32 virtout,uint32 output,uint32 mute);
	int getplaygain(uint32 virtout,uint32 output,float &gain);
	int getplaymute(uint32 virtout,uint32 output,uint32 &mute);
	int setplaysolo(uint32 output,uint32 solo);
	int getplaysolo(uint32 output,uint32 &solo);
	
	
	//------------------------------------------------------------------------------
	// Master output controls
	//------------------------------------------------------------------------------

	int setmastergain(uint32 output,float gain);
	int setmastermute(uint32 output,uint32 mute);
	int getmastergain(uint32 output,float &gain);
	int getmastermute(uint32 output,uint32 &mute);
	int getoutshift(int output,int &shift);
	int setoutshift(int output,int shift);
	int GetMasterOutMeter(int output);
	int GetInputMeter(int input);
	

	//------------------------------------------------------------------------------
	// Transport controls
	//------------------------------------------------------------------------------
	
	enum
	{
		clock_not_specified = 0xffffffff
	};

	int getclock(int32 &samplerate,int32 &clock);
	int setclock(int32 samplerate,int32 clock,bool reconnect = false);
	
	int32 GetCoreAudioSampleRate();	// Mac only
	int SetCoreAudioSampleRate(int32 samplerate);

	//------------------------------------------------------------------------------
	// Flash memory
	//------------------------------------------------------------------------------

	int readflash(uint32 offset,uint32 quadcount,uint32 *dest);
	int eraseflash(uint32 offset);
	int writeflash(uint32 offset,uint32 quadcount,uint32 *data);
	int getflashstatus(bool &ready);
	int getsessionbase(uint32 &offset);
	int setflashlock(bool locked);
	

	//------------------------------------------------------------------------------
	// Read a session block
	//------------------------------------------------------------------------------

	int readsession(uint32 offsetquads,int &quads,uint32 *dest);	
	
	
	//------------------------------------------------------------------------------
	// MIDI and clock status
	//------------------------------------------------------------------------------

	uint32 GetMidiStatus();
	bool MidiInActive(int input);
	bool MidiOutActive(int output);

	uint32 GetClockEnableMask();
	uint32 ClockDetected(int clock);

	
	//------------------------------------------------------------------------------
	//  robot guitar
	//------------------------------------------------------------------------------
	
	int SetRIPChargeState
	(
	 bool manual_charge_on,
	 bool automatic_charging,
	 uint32 inactivity_time_seconds
	 );
	int GetRIPChargeState
	(
	 bool &manual_charge_on,
	 bool &automatic_charging,
	 uint32 &inactivity_time_seconds
	 );
	
	int RoboCommWrite(	byte *data,						// data to send to guitar
						uint32 write_byte_count,		// number of bytes to write to guitar
						bool &success);		// true if it's safe to write more data
	
	int RoboCommRead(	byte *data,			// caller's buffer for data received from the guitar
						uint32 buffer_size,	// caller's buffer size in bytes
						uint32 &byte_count);// number of bytes received from guitar
	
	bool HexSignalDetected();
	bool GuitarStereoCableDetected();
	bool GuitarCharging();
	uint32 GuitarStatusFlags();
	
	int SetHexModeControl(uint32 mode);
	
	
	//------------------------------------------------------------------------------
	//  conversion routines
	//------------------------------------------------------------------------------

	static int32 DbToLin(float gaindb);
	static float LinToDb(uint32 gainlin);
	static uint32 PanFloatToEfc(float pan);
	static float EfcToPanFloat(uint32 pan);	
};

#define COMMAND_TIMEOUT_MSEC				2000

#endif // _Ehw_h_
