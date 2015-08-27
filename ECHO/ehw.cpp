/********************************************************************************
 *
 * ehw.cpp
 *
 * Code modified by JTILBIAN (20150820) - Changes Marked
 *
 ********************************************************************************/

#include "base.h"
#include "ehw.h"
#include "hwcaps.h"
#include "CaDevList.h"
#include "EndianSwap.h"
#include <IOKit/IOKitLib.h>
#include <IOKit/avc/IOFireWireAVCConsts.h>
#include <IOKit/firewire/IOFireWireLib.h>
#include <IOKit/firewire/IOFireWireFamilyCommon.h>
#include <CoreAudio/AudioHardware.h>
//#include "firewire_osx.h"


#define AVC_EFC_BASE_BYTES		(EFC_BASE_BYTES + AVC_COMMAND_OFFSET)
#define AVC_EFC_BYTES(s)		(EFC_BYTES(s) + AVC_COMMAND_OFFSET)
#define AVC_EFR_BASE_BYTES		(EFR_BASE_BYTES + AVC_COMMAND_OFFSET)
#define AVC_EFR_BYTES(s)		(EFR_BYTES(s) + AVC_COMMAND_OFFSET)


//******************************************************************************
// conversion routines
//******************************************************************************

int32 ehw::DbToLin(float gaindb)
{
	const float DbScale = 1.0f / 20.0f;		
	const float GainScale = (float) (1 << 24);
	int32 lin;
	
	lin = roundFloatToInt(GainScale * pow(10.0f,gaindb * DbScale));

	return lin;
}

float ehw::LinToDb(uint32 gainlin)
{
	const float GainScaleInv = 1.0f / ((float) (1 << 24));
	float db;

	if (0 == gainlin)
		return -144.0f;
	
	db = 20.0f * log10( ((float) gainlin) * GainScaleInv );
	return db;
}

uint32 ehw::PanFloatToEfc(float pan)
{
	return roundFloatToInt(pan * EFC_PAN_HARD_RIGHT);
}

float ehw::EfcToPanFloat(uint32 pan)
{
	static const float scale = 1.0f / EFC_PAN_HARD_RIGHT;

    return pan*scale;
}


//******************************************************************************
// ctor/dtor
//******************************************************************************

ehw::ehw(io_object_t dev,CFNumberRef guid,CFStringRef cfname,CriticalSection *lock) :
	_cs(lock),
	localLock(false)
{
	if (NULL == lock)
	{
		_cs = new CriticalSection;
		localLock = true;
	}
	
	//
	// init stuff
	//
	_ok = false;
	_guid = guid;
	_avcsvc = dev;
	_fwplugin = NULL;
	_fwavc = NULL;
	_caps = NULL;
	_CoreAudioId = 0xffffffff;
	
	_next = NULL;
	_prev = NULL;
	
	_seqnum = 0;
	
	_cmd = (efc *) (_outbuff + AVC_COMMAND_OFFSET);
	_rsp = (efr *) (_inbuff + AVC_COMMAND_OFFSET);

	memset(_outbuff,0,sizeof(_outbuff));
	memset(_inbuff,0,sizeof(_inbuff));
	
	_outbuff[ kAVCCommandResponse ] = kAVCControlCommand;
	_outbuff[ kAVCAddress ] = kAVCUnitAddress;
	_outbuff[ kAVCOpcode ] = kAVCVendorDependentOpcode;

#ifdef JUCE_LITTLE_ENDIAN
	memset(_outbuff_le,0,sizeof(_outbuff));
	memset(_inbuff_le,0,sizeof(_inbuff));

	_outbuff_le[ kAVCCommandResponse ] = kAVCControlCommand;
	_outbuff_le[ kAVCAddress ] = kAVCUnitAddress;
	_outbuff_le[ kAVCOpcode ] = kAVCVendorDependentOpcode;
#endif

	CFStringGetCString(cfname,_devname,sizeof(_devname),kCFStringEncodingASCII);

} // constructor


ehw::~ehw()
{
    DestroyPlugin();
	if (_avcsvc) 
    {
        IOObjectRelease(_avcsvc);   
        _avcsvc = NULL;
    }
	delete _caps;
	
	CFRelease(_guid);
	
	if (localLock)
		delete _cs;

} // destructor


//******************************************************************************
// init
//******************************************************************************

OSStatus ehw::CreatePlugin(FileLogger *log)
{
	const ScopedLock lock(*_cs);
	
	//
	// create the plugin
	//
	SInt32 score = 0;
	OSErr status;
	
	status = IOCreatePlugInInterfaceForService(	_avcsvc,
                                               kIOFireWireAVCLibUnitTypeID,
                                               kIOCFPlugInInterfaceID,
                                               &_fwplugin,
                                               &score);
    if (log)
    {
        log->logMessage(String::formatted("IOCreatePlugInInterfaceForService 0x%x",status));
    }
	DBG_PRINTF(("plugin create - returned %d",status));
	if (noErr != status)
		return status;
    
	//
	// get the COM object from the plugin
	//
	(*_fwplugin)->QueryInterface(	_fwplugin, 
                                 CFUUIDGetUUIDBytes(kIOFireWireAVCLibUnitInterfaceID_v2),
                                 (void**)&_fwavc);
    if (log)
    {
        log->logMessage(String::formatted("QueryInterface %p",_fwavc));
    }
	if (NULL == _fwavc)
		return -1;
    
	DBG_PRINTF(("*_fwavc %p",*_fwavc));
	
    return noErr;
}

void ehw::DestroyPlugin()
{
    const ScopedLock lock(*_cs);
    
	if (_fwavc) 
    {
        (*_fwavc)->Release(_fwavc);
        _fwavc = NULL;
    }
	if (_fwplugin) 
    {
        IODestroyPlugInInterface( _fwplugin );   
        _fwplugin = NULL;
    }
}

#if 0
int ehw::ResetInterface(FileLogger *log)
{
    const ScopedLock lock(*_cs);

#if 0
    DestroyPlugin();
    OSStatus status = CreatePlugin(log);
    return noErr != status;
#else
    */
    if (_fwplugin)
    {
        IOFireWireLibDeviceRef  fwDeviceInterface = 0;
        
        (*_fwplugin)->QueryInterface( _fwplugin, CFUUIDGetUUIDBytes(kIOFireWireDeviceInterfaceID ), (void **) fwDeviceInterface );
        if (log)
        {
            log->logMessage(String::formatted("Create kIOFireWireDeviceInterfaceID %p",fwDeviceInterface));
        }
        
        if (fwDeviceInterface)
        {
            IOReturn result = (*fwDeviceInterface)->BusReset(fwDeviceInterface);
            
            if (log)
            {
                log->logMessage(String::formatted("Bus reset result %d/0x%x",result,result));
            }

            (*fwDeviceInterface)->Release(fwDeviceInterface);
        }
    }
    
    return 0;
#endif
}
#endif

OSStatus ehw::init()
{		
	const ScopedLock lock(*_cs);
    
    OSStatus status = CreatePlugin();

    if (noErr == status)
    {
        //
        // read the caps from the box
        //
        int rval;

        rval = readcaps();
        if (0 != rval)
            status = -19; // readErr

        DBG(String(_devname));
    }
    
	return status;
	
} // init


int ehw::readcaps()
{
	OSStatus status;
	UInt32 bytesreturned;
	UInt8 *avccmd,*avcrsp;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_info;
	_cmd->h.cmd = cmd_hwinfo_get_caps;
	_cmd->h.version = efc_version;
	_cmd->h.seqnum = _seqnum;
	
	_seqnum += 2;
	
	#ifdef JUCE_LITTLE_ENDIAN

	swapcopy32(	(uint32 *) (_outbuff_le + AVC_COMMAND_OFFSET),
				(uint32 *) (_outbuff + AVC_COMMAND_OFFSET), 
				EFC_BASE_BYTES);
				
	avccmd = _outbuff_le;
	avcrsp = _inbuff_le;

	#else

	avccmd = _outbuff;
	avcrsp = _inbuff;			

	#endif
	
	DBG_PRINTF(("readcaps %p",_fwavc));
	
	bytesreturned = AVC_EFR_BYTES(efr_hwinfo_caps);
	status = (*_fwavc)->AVCCommand(	_fwavc,
									avccmd,
									AVC_EFC_BASE_BYTES,
									avcrsp,
									&bytesreturned);
	if ((0 == status) && (efc_ok == _rsp->h.rval))
	{
		//
		// Byte swapping for Intel Macs
		//
		#ifdef JUCE_LITTLE_ENDIAN
		
		efr_hwinfo_caps *caps;
		
		caps = &(_rsp->u.r.hwinfo_caps);
		
		swapcopy32( (uint32 *) (_inbuff + AVC_COMMAND_OFFSET),
					(uint32 *) (_inbuff_le + AVC_COMMAND_OFFSET),
					EFR_BYTES(efr_hwinfo_caps));

		swapblock32( (uint32 *) &(caps->vendorname), sizeof(caps->vendorname));
		swapblock32( (uint32 *) &(caps->boxname), sizeof(caps->boxname));
		swapblock32( (uint32 *) &(caps->out_groups), sizeof(caps->out_groups));
		swapblock32( (uint32 *) &(caps->in_groups), sizeof(caps->in_groups));
		
		#endif
	
		//
		// Create the hwcaps object
		//
		_caps = new hwcaps;
		memcpy( &(_caps->_caps),
				&(_rsp->u.r.hwinfo_caps),
				sizeof(efr_hwinfo_caps));
		_caps->init();
		
		DBG_PRINTF(("device name %s  num outputs %d",_caps->BoxTypeName(),_caps->numbusout()));
		
		_efrpolledstuffbytes = EFR_POLLED_STUFF_BYTES(	_caps->_caps.num_phys_audio_out,
														_caps->_caps.num_phys_audio_in) 
								+ AVC_COMMAND_OFFSET ;	
			
		return 0;
	}
	else
	{
		DBG_PRINTF(("AVCCommand failed for readcaps - status 0x%x  EFC status %d",status,_rsp->h.rval));
	}
	
	return 1;
}



int ehw::getarmversion(uint32 &armversion)
{
	const ScopedLock lock(*_cs);
	int rval;
    
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_info;
	_cmd->h.cmd = cmd_hwinfo_get_caps;
	
	rval = SendEfc(AVC_EFC_BASE_BYTES,
				   AVC_EFR_BYTES(efr_hwinfo_caps));
	if (0 == rval)
	{
		armversion = _rsp->u.r.hwinfo_caps.armversion;
		_caps->_caps.armversion = armversion;
		_caps->_caps.dspversion = _rsp->u.r.hwinfo_caps.dspversion;
		_caps->_caps.fpga_version = _rsp->u.r.hwinfo_caps.fpga_version;
	}
	
	return rval;
}


int ehw::efctest(uint32 value,uint32 &result)
{
	const ScopedLock lock(*_cs);
	int rval;
    
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_info;
	_cmd->h.cmd = cmd_efc_test_arm;
    _cmd->u.c.test_data.value = value;
	
	rval = SendEfc(AVC_EFC_BYTES(efc_test_data),
				   AVC_EFR_BYTES(efr_test_data));
	if (0 == rval)
	{
		result = _rsp->u.r.test_data.value;
	}
	
	return rval;
}


//******************************************************************************
// This device just got removed from the machine
//******************************************************************************
/*
void ehw::removed()
{
	DBG_PRINTF((T("ehw::removed")));
}
*/


//******************************************************************************
//
// Get vendor ID, box ID, and box revision
//
//******************************************************************************

uint32 ehw::GetVendorId()
{
	return _caps->_caps.node_id_hi >> 8;
}

uint32 ehw::GetBoxType()
{
	return _caps->_caps.hwtype;
}

uint32 ehw::GetBoxRev()
{
	return _caps->_caps.hwversion;
}

uint64 ehw::GetSerialNumber()
{
    uint64 serial = _caps->_caps.node_id_hi & 0xffffff;
    serial <<= 32;
    serial |= _caps->_caps.node_id_lo;
    return serial;
}


//******************************************************************************
//
// Added by JTILBIAN (20150820)
//
//******************************************************************************

int32 ehw::getoutputchannelcount()
{
    return _caps->numbusout();
}


//******************************************************************************
//
// Input bus controls
//
//******************************************************************************

//==============================================================================
// get input nominal shift
//==============================================================================

int ehw::getinshift(int input,int &shift)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_nominal);
	_cmd->h.category = cat_phys_input_mixer;
	_cmd->h.cmd = cmd_get_nominal;
	_cmd->u.c.nominal.chan = input;

	rval = SendEfc(	AVC_EFC_BYTES(efc_nominal),
					AVC_EFR_BYTES(efr_nominal)
					);

	shift = _rsp->u.r.nominal.shift;

	return rval;
	
} // getinshift


//==============================================================================
// set input nominal shift
//==============================================================================

int ehw::setinshift(int input,int shift)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_nominal);
	_cmd->h.category = cat_phys_input_mixer;
	_cmd->h.cmd = cmd_set_nominal;
	_cmd->u.c.nominal.chan = input;
	_cmd->u.c.nominal.shift = shift;

	rval = SendEfc(	AVC_EFC_BYTES(efc_nominal),
					AVC_EFR_BASE_BYTES
					);

	return rval;

} // setinshift


//==============================================================================
// get input meter
//==============================================================================

int ehw::GetInputMeter(int input)
{
	return getpolledstuff()->meters[ input + getpolledstuff()->num_output_meters];
}



//******************************************************************************
//
// Monitor controls
//
//******************************************************************************

//==============================================================================
// get monitor mute
//==============================================================================

int ehw::getmonmute(uint32 input,uint32 output,uint32 &mute)
{
 	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_monitor_mute);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_get_mute;
	_cmd->u.c.monitor_mute.input = input;
	_cmd->u.c.monitor_mute.output = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_mute),
					AVC_EFR_BYTES(efr_monitor_mute)
					);

	mute = _rsp->u.r.monitor_mute.mute;

	return rval;
	
} // getmonmute


//==============================================================================
// set monitor mute
//==============================================================================

int ehw::setmonmute(uint32 input,uint32 output,uint32 mute)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_mute);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_set_mute;
	_cmd->u.c.monitor_mute.input = input;
	_cmd->u.c.monitor_mute.output = output;
	_cmd->u.c.monitor_mute.mute = mute;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_mute),
					AVC_EFR_BYTES(efr_monitor_mute));

	return rval;

} // setmonmute


//==============================================================================
// get monitor gain
//==============================================================================

int ehw::getmongain(uint32 input,uint32 output,float &gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_gain);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_get_gain;
	_cmd->u.c.monitor_gain.input = input;
	_cmd->u.c.monitor_gain.output = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_gain),
					AVC_EFR_BYTES(efr_monitor_gain));

	gain = LinToDb(_rsp->u.r.monitor_gain.gain);

	return rval;

} // getmongain


//==============================================================================
// set monitor gain
//==============================================================================

int ehw::setmongain(uint32 input,uint32 output,float gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_gain);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_set_gain;
	_cmd->u.c.monitor_gain.input = input;
	_cmd->u.c.monitor_gain.output = output;
	_cmd->u.c.monitor_gain.gain = DbToLin(gain);

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_gain),
					AVC_EFR_BYTES(efr_monitor_gain));

	return rval;
}



//==============================================================================
// get monitor solo
//==============================================================================

int ehw::getmonsolo(uint32 input,uint32 output,uint32 &solo)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_solo);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_get_solo;
	_cmd->u.c.monitor_solo.input = input;
	_cmd->u.c.monitor_solo.output = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_solo),
					AVC_EFR_BYTES(efr_monitor_solo));

	solo = _rsp->u.r.monitor_solo.solo;

	return rval;
	
} // getmonsolo


//==============================================================================
// set monitor solo
//==============================================================================

int ehw::setmonsolo(uint32 input,uint32 output,uint32 solo)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_solo);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_set_solo;
	_cmd->u.c.monitor_solo.input = input;
	_cmd->u.c.monitor_solo.output = output;
	_cmd->u.c.monitor_solo.solo = solo;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_solo),
					AVC_EFR_BYTES(efr_monitor_solo));
	return rval;
	
} // setmonsolo


//==============================================================================
// get monitor pan
//==============================================================================

int ehw::getmonpan(uint32 input,uint32 output,float &pan)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_pan);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_get_pan;
	_cmd->u.c.monitor_pan.input = input;
	_cmd->u.c.monitor_pan.output = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_pan),
					AVC_EFR_BYTES(efr_monitor_pan));

	pan = EfcToPanFloat(_rsp->u.r.monitor_pan.pan);

	return rval; 

} // getmonpan


//==============================================================================
// set monitor pan
//==============================================================================

int ehw::setmonpan(uint32 input,uint32 output,float pan)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_monitor_pan);
	_cmd->h.category = cat_monitor_mixer;
	_cmd->h.cmd = cmd_set_pan;
	_cmd->u.c.monitor_pan.input = input;
	_cmd->u.c.monitor_pan.output = output;
	_cmd->u.c.monitor_pan.pan = PanFloatToEfc(pan);

	rval = SendEfc(	AVC_EFC_BYTES(efc_monitor_pan),
					AVC_EFR_BYTES(efr_monitor_pan));

	return rval;
	
} // setmonpan


//==============================================================================
// set playback gain
//==============================================================================

int ehw::setplaygain(uint32 virtout,uint32 output,float gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_gain);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_set_gain;
	_cmd->u.c.gain.chan = output;
	_cmd->u.c.gain.gain = DbToLin(gain);

	rval = SendEfc(	AVC_EFC_BYTES(efc_gain),
					AVC_EFR_BYTES(efr_gain));

	return rval;
}


//==============================================================================
// set playback mute
//==============================================================================

int ehw::setplaymute(uint32 virtout,uint32 output,uint32 mute)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_mute);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_set_mute;
	_cmd->u.c.mute.chan = output;
	_cmd->u.c.mute.mute = mute;

	rval = SendEfc(	AVC_EFC_BYTES(efc_mute),
					AVC_EFR_BYTES(efr_mute));

	return rval;
}


//==============================================================================
// get playback gain
//==============================================================================

int ehw::getplaygain(uint32 virtout,uint32 output,float &gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_gain);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_get_gain;
	_cmd->u.c.gain.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_gain),
					AVC_EFR_BYTES(efr_gain)	);

	gain = LinToDb(_rsp->u.r.gain.gain);

	return rval; 
}


//==============================================================================
// get playback mute
//==============================================================================

int ehw::getplaymute(uint32 virtout,uint32 output,uint32 &mute)
{
 	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_mute);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_get_mute;
	_cmd->u.c.mute.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_mute),
					AVC_EFR_BYTES(efr_mute));

	mute = _rsp->u.r.mute.mute;

	return rval;
}


//==============================================================================
// get playback solo
//==============================================================================

int ehw::getplaysolo(uint32 output,uint32 &solo)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_solo);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_get_solo;
	_cmd->u.c.solo.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_solo),
					AVC_EFR_BYTES(efr_solo));
	
	solo = _cmd->u.r.monitor_solo.solo;

	return rval;

} // getplaysolo


//==============================================================================
// set playback solo
//==============================================================================

int ehw::setplaysolo(uint32 output,uint32 solo)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_solo);
	_cmd->h.category = cat_playback_mixer;
	_cmd->h.cmd = cmd_set_solo;
	_cmd->u.c.solo.chan = output;
	_cmd->u.c.solo.solo = solo;

	rval = SendEfc(	AVC_EFC_BYTES(efc_solo),
					AVC_EFR_BYTES(efr_solo));

	return rval;
} // setplaysolo


//******************************************************************************
//
// Master outputs
//
//******************************************************************************

//==============================================================================
// set master output gain
//==============================================================================

int ehw::setmastergain(uint32 output,float gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_gain);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_set_gain;
	_cmd->u.c.gain.chan = output;
	_cmd->u.c.gain.gain = DbToLin(gain);

	rval = SendEfc(	AVC_EFC_BYTES(efc_gain),
					AVC_EFR_BYTES(efr_gain));

	return rval;
}
 

//==============================================================================
// set master output mute
//==============================================================================

int ehw::setmastermute(uint32 output,uint32 mute)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_mute);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_set_mute;
	_cmd->u.c.mute.chan = output;
	_cmd->u.c.mute.mute = mute;
	
	rval = SendEfc(	AVC_EFC_BYTES(efc_mute),
					AVC_EFR_BYTES(efr_mute));
	//DBG_PRINTF(("ehw::setmastermute %d",rval));
	return rval;
}


int ehw::getmastergain(uint32 output,float &gain)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_gain);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_get_gain;
	_cmd->u.c.gain.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_gain),
					AVC_EFR_BYTES(efr_gain));
	
	gain = LinToDb(_rsp->u.r.gain.gain);
	
	return rval;
}


int ehw::getmastermute(uint32 output,uint32 &mute)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_mute);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_get_mute;
	_cmd->u.c.mute.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_mute),
					AVC_EFR_BYTES(efr_mute));

	mute = _rsp->u.r.mute.mute;

	return rval;
}

//==============================================================================
// get output nominal shift
//==============================================================================

int ehw::getoutshift(int output,int &shift)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_nominal);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_get_nominal;
	_cmd->u.c.nominal.chan = output;

	rval = SendEfc(	AVC_EFC_BYTES(efc_nominal),
					AVC_EFR_BYTES(efr_nominal)
					);

	shift = _rsp->u.r.nominal.shift;

	return rval;
} // getoutshift


//==============================================================================
// set output nominal shift
//==============================================================================

int ehw::setoutshift(int output,int shift)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_nominal);
	_cmd->h.category = cat_phys_output_mixer;
	_cmd->h.cmd = cmd_set_nominal;
	_cmd->u.c.nominal.chan = output;
	_cmd->u.c.nominal.shift = shift;

	rval = SendEfc(	AVC_EFC_BYTES(efc_nominal),
					AVC_EFR_BASE_BYTES
					);
	
	return rval;

} // setoutshift

//==============================================================================
// get master out meter
//==============================================================================

int ehw::GetMasterOutMeter(int output)
{
	return getpolledstuff()->meters[output];
}



//******************************************************************************
//
// Box flags
//
//******************************************************************************

int ehw::changeboxflags(uint32 setmask,uint32 clearmask)
{
 	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_change_flags);
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_change_flags;
	_cmd->u.c.ctrl_flags.setmask = setmask;
	_cmd->u.c.ctrl_flags.clearmask = clearmask;

	rval = SendEfc(	AVC_EFC_BYTES(efc_change_flags),
					AVC_EFR_BASE_BYTES);

	return rval;
}


int ehw::getboxflags(uint32 &flags)
{
 	int rval;
	const ScopedLock lock(*_cs);
	
	flags = 0xffffffff;
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_get_flags;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_get_flags));

	if (0 == rval)
		flags = _rsp->u.r.ctrl_flags.flags;

	return rval; 
}


//******************************************************************************
// EFC over AVC wrapper
//******************************************************************************

int ehw::SendEfc(size_t outbytes,size_t inbytes)
{
	OSStatus status;
	UInt32 bytesreturned;
	byte *avccmd,*avcrsp;
	//int64 start,stop;
	
	//
	// EFC sequence number
	//
	_cmd->h.seqnum = _seqnum;
	_seqnum += 2;
	
	//
	// Byte swapping for Intel Macs
	//
	#ifdef JUCE_LITTLE_ENDIAN
	
	swapcopy32(	(uint32 *) (_outbuff_le + AVC_COMMAND_OFFSET),
				(uint32 *) (_outbuff + AVC_COMMAND_OFFSET), 
				outbytes - AVC_COMMAND_OFFSET);
				
	avccmd = _outbuff_le;
	avcrsp = _inbuff_le;
	
	#else
	
	avccmd = _outbuff;
	avcrsp = _inbuff;			
	
	#endif
	
	//
	// Send the AV/C command
	//
	bytesreturned = inbytes;
	
	//start = Time::getHighResolutionTicks();
	status = (*_fwavc)->AVCCommand(	_fwavc,
									avccmd,
									outbytes,
									avcrsp,
									&bytesreturned);
	//stop = Time::getHighResolutionTicks();									
	
	//
	// Byte swapping for Intel Macs
	//
	#ifdef JUCE_LITTLE_ENDIAN

	swapcopy32(	(uint32 *) (_inbuff + AVC_COMMAND_OFFSET),
				(uint32 *) (_inbuff_le + AVC_COMMAND_OFFSET), 
				bytesreturned);

	#endif																
	
	//
	// Process the results
	//
	if (0 != status)
	{
		Logger::outputDebugString(String::formatted("ehw::SendEfc - AVCCommand - bad status %d (0x%x)",status,status));
        
		return 1;
	}
				
	if ((efc_ok != _rsp->h.rval) && (efc_flash_busy != _rsp->h.rval))
	{
		Logger::outputDebugString(String::formatted("AVCCommand - bad EFC rval %d",_rsp->h.rval));
		return 1;
	}
	
	//if ((stop - start) > maxticks)
	//	maxticks = stop - start;
	
	return 0;
}



//******************************************************************************
// Polled stuff
//******************************************************************************

int ehw::updatepolledstuff()
{
	OSStatus status;
	UInt32 bytesreturned;
	byte *avccmd,*avcrsp;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_info;
	_cmd->h.cmd = cmd_get_polled_stuff;
	_cmd->h.seqnum = _seqnum;
	_seqnum += 2;

	//
	// Byte swapping for Intel Macs
	//
	#ifdef JUCE_LITTLE_ENDIAN
	
	swapcopy32(	(uint32 *) (_outbuff_le + AVC_COMMAND_OFFSET),
				(uint32 *) (_outbuff + AVC_COMMAND_OFFSET), 
				EFC_BASE_BYTES);
				
	avccmd = _outbuff_le;
	avcrsp = _inbuff_le;

	#else
	
	avccmd = _outbuff;
	avcrsp = _polledbuff;
	
	#endif
	
	//
	// Send the AV/C command
	//
	bytesreturned = _efrpolledstuffbytes;
	status = (*_fwavc)->AVCCommand(	_fwavc,
									avccmd,
									AVC_EFC_BASE_BYTES,
									avcrsp,
									&bytesreturned);
	//
	// Byte swapping for Intel Macs
	//
	#ifdef JUCE_LITTLE_ENDIAN

	swapcopy32(	(uint32 *) (_polledbuff + AVC_COMMAND_OFFSET),
				(uint32 *) (_inbuff_le + AVC_COMMAND_OFFSET), 
				bytesreturned);

	#endif																
    
    //DBG_PRINTF(("update polled stuff result %d",status));
	
	//
	// Process the results
	//
	if (0 != status)
	{
		//Logger::outputDebugPrintf(T("AVCCommand for polled stuff - bad status %d (0x%x)"),status,status);
		return 1;
	}
				
	if ((efc_ok != _rsp->h.rval) && (efc_flash_busy != _rsp->h.rval))
	{
		Logger::outputDebugString(String::formatted("AVCCommand for polled stuff - bad EFC rval %d",_rsp->h.rval));
		return 1;
	}
	
	return 0;
}


//******************************************************************************
//
// get/set clock and samplerate
//
//******************************************************************************

int ehw::getclock(int32 &samplerate,int32 &clock)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_get_clock;
	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_get_clock)
					);
	
	clock = _rsp->u.r.get_clock.clock;
	samplerate = _rsp->u.r.get_clock.samplerate;

	return rval;
}


int ehw::setclock(int32 samplerate,int32 clock,bool reconnect)
{
	int rval = 0;
	const ScopedLock lock(*_cs);
	
	//
	// Set the clock source via EFC
	//
	_cmd->quads = EFC_QUADS(efc_set_clock);
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_set_clock;
	_cmd->u.c.set_clock.clock = clock;
	_cmd->u.c.set_clock.samplerate = samplerate;
	_cmd->u.c.set_clock.index = 0;
	
	rval = SendEfc(	AVC_EFC_BYTES(efc_set_clock),
					AVC_EFR_BASE_BYTES);
	if (0 != rval)
	{
		return rval;		
	}
					
	//
	// Optionally send the "phy reconnect command" so the device will vanish and reappear 
	// with a new descriptor.
	//
	// Ignore the return value since it's going off the bus and the
	// command will fail.
	//
	if (reconnect)
	{
		_cmd->quads = EFC_BASE_QUADS;
		_cmd->h.category = cat_hw_ctrl;
		_cmd->h.cmd = cmd_hw_ctrl_reconnect_phy;
		SendEfc( AVC_EFC_BASE_BYTES, AVC_EFR_BASE_BYTES);
	}
				
	return rval;
}


int32 ehw::GetCoreAudioSampleRate()
{
	Float64 sampleRate;
	UInt32 propertyDataSize = sizeof(sampleRate);
	OSErr status;

	status = AudioDeviceGetProperty(_CoreAudioId, 0, 0,
									 kAudioDevicePropertyNominalSampleRate,
									 &propertyDataSize, &sampleRate);
	if (0 != status)
	{
		//Logger::outputDebugPrintf(T("getsamplerate failed - id %d - rval %d\n"),_CoreAudioId,status);

		CoreAudioDeviceList.BuildList();
		_CoreAudioId = CoreAudioDeviceList.GetCaId(_guid);

		return 48000;
	}
		
	return (int32) sampleRate;
}


int ehw::SetCoreAudioSampleRate(int32 samplerate)
{
	int rval = 0;
	OSStatus status;
	
	//
	// Set the sample rate via CoreAudio
	//
	Float64 rate = samplerate;
	UInt32 propertyDataSize = sizeof(Float64);
	AudioDeviceID id;

	CoreAudioDeviceList.BuildList();
	id = CoreAudioDeviceList.GetCaId(_guid);
	status = AudioDeviceSetProperty(id, NULL, 0, 0,
								   kAudioDevicePropertyNominalSampleRate,
								   propertyDataSize, &rate);
	switch (status)
	{
		case kAudioHardwareNoError :
			rval = 0;
			break;
			
		default :
			DBG_PRINTF(("set clock - AudioDeviceSetProperty returned %d\n",status));
			rval = 1;
			break;
	}

	return rval;
}



//******************************************************************************
//
// Onyx400F control room mirror
//
//******************************************************************************

int ehw::getmirror(int &output)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_get_mirror;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_mirror));
					
	output = _rsp->u.r.mirror.output;

	return rval;
}


int ehw::setmirror(int output)
{
 	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_mirror);
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_set_mirror;
	_cmd->u.c.mirror.output = output;

	rval = SendEfc(	AVC_EFR_BYTES(efc_mirror),
					AVC_EFR_BASE_BYTES);

	return rval;
}


//******************************************************************************
//
// Flash memory
//
//******************************************************************************

int ehw::readflash(uint32 offset,uint32 quadcount,uint32 *dest)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_flash_read);
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_read;
	_cmd->u.c.flash_read.addr = offset;
	_cmd->u.c.flash_read.quadcount = quadcount;
	
	rval = SendEfc(	AVC_EFC_BYTES(efc_flash_read),
					sizeof(_inbuff)	// do this to allow flashing with 0.73 firmware
					);

	if (0 ==  rval)
		memcpy( dest, _rsp->u.r.flash_read.data, quadcount * sizeof(uint32));

	return rval;
}



int ehw::eraseflash(uint32 offset)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_flash_erase);
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_erase;
	_cmd->u.c.flash_erase.addr = offset;
	
	rval = SendEfc(	AVC_EFC_BYTES(efc_flash_erase),
					sizeof(_inbuff)	// do this to allow flashing with 0.73 firmware
					);

	return rval;
}


int ehw::writeflash(uint32 offset,uint32 quadcount,uint32 *data)
{
	int rval;
	const ScopedLock lock(*_cs);
    size_t outbytes;

	_cmd->quads = EFC_QUADS(efc_flash_write);
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_write;
	_cmd->u.c.flash_write.addr = offset;
	_cmd->u.c.flash_write.quadcount = quadcount;
	
	memcpy(_cmd->u.c.flash_write.data,data,quadcount * sizeof(uint32));
    
    DBG(String::formatted("ehw::writeflash %d to 0x%x source:%p",quadcount,offset,data));

    outbytes = AVC_EFC_BYTES(efc_flash_write) - max_efc_flash_quads*sizeof(uint32) + quadcount * sizeof(uint32);
	rval = SendEfc( outbytes,
					sizeof(_inbuff)	// do this to allow flashing with 0.73 firmware
					);
					
	return rval;
}



int ehw::getflashstatus(bool &ready)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_flash_read);
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_get_status;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					sizeof(_inbuff)
					);
	if ((0 == rval) && (efc_ok == _rsp->h.rval))
	{
		ready = true;
	}
	else if (efc_flash_busy == _rsp->h.rval)
	{
		rval = 0;
		ready = false;
	}

	if (0 != rval)
	{
		DBG_PRINTF(("getflashstatus failed"));
	}

	return rval;
}


int ehw::getsessionbase(uint32 &offset)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_get_session_base;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_flash_session_base)
					);

	offset = _rsp->u.r.flash_session_base.addr;

	return rval;
}


int ehw::setflashlock(bool locked)
{
	int rval;

	//
	// Only needed Fireworks3 designs, which don't have
	// a DSP.
	//
	if (_caps->HasDsp())
		return 0;
	
	_cmd->quads = EFC_QUADS(efc_flash_lock);
	_cmd->h.category = cat_flash;
	_cmd->h.cmd = cmd_flash_lock;
	_cmd->u.c.flash_lock.lock = locked;

	rval = SendEfc(	AVC_EFC_BYTES(efc_flash_lock),
					sizeof(_inbuff)
					);

	return rval;
}



//******************************************************************************
//
// Read part of a session
//
//******************************************************************************

int ehw::readsession(uint32 offsetquads,int &quads,uint32 *dest)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_session_block);
	_cmd->h.category = cat_hw_info;
	_cmd->h.cmd = cmd_read_session_block;
	_cmd->u.c.session_block.offsetquads = offsetquads;
	_cmd->u.c.session_block.quadcount = quads;

	rval = SendEfc( AVC_EFC_BYTES(efc_session_block),
					AVC_EFR_BYTES(efr_session_block)
					);
	if (0 == rval)
	{
		quads = _rsp->u.r.session_block.quadcount;

		memcpy( dest, _rsp->u.r.session_block.data, quads*sizeof(uint32));
	}

	return rval;
}


//******************************************************************************
//
// Onyx 1200F digital mode
//
//******************************************************************************

int ehw::getdigitalmode(int &mode)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_get_digital_mode;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_digital_mode));
	if (0 == rval)
	{
		mode = _rsp->u.r.digital_mode.mode;
	}

	return rval;
}


int ehw::setdigitalmode(int mode)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_digital_mode);
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_set_digital_mode;
	_cmd->u.c.digital_mode.mode = mode;

	rval = SendEfc( AVC_EFC_BYTES(efc_digital_mode),
					AVC_EFR_BASE_BYTES
					);

	return rval;
}


//******************************************************************************
//
// Phantom power for AF4
//
//******************************************************************************

int ehw::getphantom(int &phantom)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_get_phantom;

	rval = SendEfc(	AVC_EFC_BASE_BYTES,
					AVC_EFR_BYTES(efr_get_mode)
					);
	if (0 == rval)
	{
		phantom = _rsp->u.r.mode.mode;
	}

	return rval;
}


int ehw::setphantom(int phantom)
{
	int rval;
	const ScopedLock lock(*_cs);

	_cmd->quads = EFC_QUADS(efc_set_mode);
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_set_phantom;
	_cmd->u.c.mode.mode = phantom;

	rval = SendEfc(	AVC_EFC_BYTES(efc_set_mode),
					AVC_EFR_BASE_BYTES
					);

	return rval;
}



//******************************************************************************
//
// Isoc audio mapping
//
//******************************************************************************

int ehw::getisocmap(efr_isoc_map *map)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_isoc_map);
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_get_isoc_audio_map;
	
	_cmd->u.c.isoc_map.flags = map->flags;
	_cmd->u.c.isoc_map.samplerate = map->samplerate;

	rval = SendEfc(	AVC_EFC_BYTES(efc_isoc_map),
					AVC_EFR_BYTES(efc_isoc_map)
					);

	if (0 == rval)
	{
		memcpy(map,&(_rsp->u.r.isoc_map),sizeof(efr_isoc_map));
	}

	return rval;
}


int ehw::setisocmap(efc_isoc_map *map)
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_QUADS(efc_isoc_map);
	_cmd->h.category = cat_io_config;
	_cmd->h.cmd = cmd_set_isoc_audio_map;

	memcpy(&(_cmd->u.c.isoc_map),map,sizeof(efc_isoc_map));

	rval = SendEfc(	AVC_EFC_BYTES(efc_isoc_map),
					AVC_EFR_BASE_BYTES
					);

	return rval;
}



//******************************************************************************
//
// Make the box identify itself to the user by blinking an
// appropriate LED
//
//******************************************************************************

int ehw::identify()
{
	int rval;
	const ScopedLock lock(*_cs);
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_identify;

	rval = SendEfc( AVC_EFC_BASE_BYTES,
					AVC_EFC_BASE_BYTES
					);

	return rval;
}


//******************************************************************************
//
// MIDI activity
//
//******************************************************************************

uint32 ehw::GetMidiStatus()
{
	const uint32 mask =	efc_midi_in_1_flag | 
								efc_midi_out_1_flag |
								efc_midi_in_2_flag |
								efc_midi_out_2_flag;		
		
	return getpolledstuff()->boxstatus & mask;
}

bool ehw::MidiInActive(int input)
{
	uint32 bit,shift;

	bit = efc_midi_in_1_flag;
	shift = input * 2;
	return (getpolledstuff()->boxstatus & (bit << shift)) != 0; 
}

bool ehw::MidiOutActive(int output)
{
	uint32 bit,shift;

	bit = efc_midi_out_1_flag;
	shift = output * 2;
	return (getpolledstuff()->boxstatus & (bit << shift)) != 0; 
}


//******************************************************************************
//
// Clocking
//
//******************************************************************************

uint32 ehw::GetClockEnableMask()
{
	return getpolledstuff()->boxstatus;
}

uint32 ehw::ClockDetected(int clock)
{
	return getpolledstuff()->boxstatus & (1 << clock);
}


void ehw::reconnectphy()
{
	DBG("ehw::reconnectphy()");
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_reconnect_phy;
	SendEfc( AVC_EFC_BASE_BYTES, AVC_EFR_BASE_BYTES);
}


void ehw::busreset()
{
	DBG("ehw::busreset");
	
	_cmd->quads = EFC_BASE_QUADS;
	_cmd->h.category = cat_hw_ctrl;
	_cmd->h.cmd = cmd_hw_ctrl_bus_reset;
	SendEfc( AVC_EFC_BASE_BYTES, AVC_EFR_BASE_BYTES);
}

String ehw::getFirmwareVersionString()
{
    return String("DSP:" + String::toHexString((int32)_caps->DspVersion()) + " ARM:" + String::toHexString((int32)_caps->ArmVersion()));
}