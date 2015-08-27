#include "base.h"
#include "ehw.h"
#include "hwcaps.h"

void hwcaps::init()
{
	uint32 chan,grp;
	bool ok;

	//
	// input channel map
	//	
	chan = 0;
	ok = true;
	memset(_inmap,0xff,sizeof(_inmap));
	for (grp = 0; (grp < _caps.num_in_groups) && ok; grp++)
	{
		int offset;
		
		for (offset = 0; offset < _caps.in_groups[ grp ].count; offset++)
		{
			_inmap[ chan ].type = _caps.in_groups[ grp ].type;
			_inmap[ chan ].group = grp;
			_inmap[ chan ].groupoffset = offset;
			chan++;
			
			if (chan >= max_chan)
			{
				ok = false;
				break;
			}
		}
	}

	//
	// output channel/group map
	//
	chan = 0;
	ok = true;
	memset(_outmap,0xff,sizeof(_outmap));
	for (grp = 0; (grp < _caps.num_out_groups) && ok; grp++)
	{
		int offset;
		
		for (offset = 0; offset < _caps.out_groups[ grp ].count; offset++)
		{
			_outmap[ chan ].type = _caps.out_groups[ grp ].type;
			_outmap[ chan ].group = grp;
			_outmap[ chan ].groupoffset = offset;
			chan++;
			
			if (chan >= max_chan)
			{
				ok = false;
				break;
			}
		}
	}

}



int32 hwcaps::numbusin()
{
	return _caps.num_1394_rec_chan;
}


int32 hwcaps::numbusout()
{
	return _caps.num_mix_play_chan;
}


int32 hwcaps::numplaychan(int samplerate)
{
	if (samplerate > 100000)
		return _caps.num_1394_play_chan_4x;

	if (samplerate > 50000)
		return _caps.num_1394_play_chan_2x;

	return _caps.num_1394_play_chan;
}


int32 hwcaps::numrecchan(int samplerate)
{
	if (samplerate > 100000)
		return _caps.num_1394_rec_chan_4x;

	if (samplerate > 50000)
		return _caps.num_1394_rec_chan_2x;

	return _caps.num_1394_rec_chan;
}


const char *hwcaps::BoxTypeName()
{
	return _caps.boxname;
}


int hwcaps::NumMixInGroups()
{
	return _caps.num_in_groups;
}


int hwcaps::MixInGroupSize(int group)
{
	jassert( ((uint32)group) < _caps.num_in_groups);
	
	return _caps.in_groups[ group ].count;
}


const char *hwcaps::MixInGroupName(int group)
{
	jassert( ((uint32)group) < _caps.num_in_groups);
	
	switch (_caps.in_groups[group].type)
	{
		case analog :
			return "Analog";
			
		case spdif :
			return "S/PDIF";
			
		case adat :
			return "ADAT";

		case digital :
			return "Digital";

		case guitar :
			return "Guitar";

		case piezo_guitar :
			return "Piezo";

		case guitar_string :
			return "String";
			
		default :
			return "Input";
	}
}


const char *hwcaps::MixInGroupShortName(int group)
{
	jassert( ((uint32)group) < _caps.num_in_groups);
	
	switch (_caps.in_groups[group].type)
	{
		case analog :
			return "A";
			
		case spdif :
			return "S";
			
		case adat :
			return "ADAT";

		case digital :
			return "D";
			
		case i2s :
			return "";

		case guitar :
			return "G";

		case piezo_guitar :
			return "P";

		case guitar_string :
			return "S";
		
		default :
			return "In";
	}
}



int hwcaps::MixInGroup(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_rec_chan);
	
	return _inmap[chan].group;
}

int hwcaps::MixInGroupOffset(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_rec_chan);
	
	return _inmap[chan].groupoffset;
}

int hwcaps::MixInType(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_rec_chan);
	
	return _inmap[chan].type;
}


int hwcaps::NumMixOutGroups()
{
	return _caps.num_out_groups;
}


int hwcaps::MixOutGroupSize(int group)
{
	jassert( ((uint32)group) < _caps.num_out_groups);
	
	return _caps.out_groups[ group ].count;
}


const char *hwcaps::MixOutGroupName(int group)
{
	jassert( ((uint32)group) < _caps.num_out_groups);
	
	switch (_caps.out_groups[group].type)
	{
		case analog :
			return "Analog";
			
		case spdif :
			return "S/PDIF";
			
		case adat :
			return "ADAT";

		case digital :
			return "Digital";
			
		case headphones :
			return "Headphones";
		
		default :
			return "Output";
	}
}


const char *hwcaps::MixOutGroupShortName(int group)
{
	jassert( ((uint32)group) < _caps.num_out_groups);
	
	switch (_caps.out_groups[group].type)
	{
		case analog :
			return "A";
			
		case spdif :
			return "S";
			
		case adat :
			return "ADAT";

		case digital :
			return "D";
			
		case headphones :
			return "H";
		
		case i2s :
			return "";
		
		default :
			return "Out";
	}
}


int hwcaps::MixOutGroup(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_play_chan);
	
	return _outmap[chan].group;
}

int hwcaps::MixOutGroupOffset(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_play_chan);
	
	return _outmap[chan].groupoffset;
}

int hwcaps::MixOutType(int chan)
{
	jassert( ((uint32)chan) < _caps.num_mix_play_chan);
	
	return _outmap[chan].type;
}


int hwcaps::MixPlayType(int chan)
{
	return hwcaps::MixOutType(chan);
}


int hwcaps::MinSampleRate()
{
	return _caps.min_sample_rate;
}


int hwcaps::MaxSampleRate()
{
#if JUCE_MAC
	if (AUDIOFIRE12 == _caps.hwtype)
		return 192000;
#endif
	
	return _caps.max_sample_rate;
}


bool hwcaps::HasAnalogMirror()
{
	return (0 != (_caps.flags & efrcaps_supports_mirroring));
}


bool hwcaps::HasDigitalModeSwitch()
{
	return 0 != (_caps.flags & efrcaps_digital_mode_switch);
}


bool hwcaps::HasSpdifOut()
{	
	uint32 grp,type;
	
	for (grp = 0; grp < _caps.num_out_groups; grp++)
	{
		type = _caps.out_groups[ grp ].type;
		if ((spdif == type) || (digital == type))
			return true;
	}
	
	return false;
}


bool hwcaps::HasSpdifIn()
{	
	uint32 grp,type;
	
	for (grp = 0; grp < _caps.num_in_groups; grp++)
	{
		type = _caps.in_groups[ grp ].type;
		if ((spdif == type) || (digital == type))
			return true;
	}
	
	return false;
}


bool hwcaps::ExternalSync()
{
	uint32 mask = ~(efc_clock_flag_internal | efc_clock_flag_1394 | efc_clock_flag_continuous);

	return 0 != (mask & _caps.inputclocks);
}


bool hwcaps::ClockSupported(int clock)
{
	uint32 clockbit;
	
	clockbit = 1 << clock;
	if (0 == (clockbit & _caps.inputclocks))
		return false;
		
	return true;
}

void hwcaps::GetClockName(int clock,String &name)
{
	static const char *clocknames[ num_clock_sources ] =
	{
		"Internal",
		"FireWire",
		"Word",
		"S/PDIF",
		"ADAT",
		"ADAT B",
		"Variable"
	};

	name = clocknames[clock];
}


bool hwcaps::HasDsp()
{
	return 0 != (_caps.flags & efrcaps_has_dsp);
}


uint32 hwcaps::DspVersion()
{
	return _caps.dspversion;
}

uint32 hwcaps::ArmVersion()
{
	return _caps.armversion;
}

bool hwcaps::HasFpga()
{
	return 0 != (_caps.flags & efrcaps_has_fpga);
}

uint32 hwcaps::FpgaVersion()
{
	return _caps.fpga_version;
}


bool hwcaps::HasPhantomPower()
{
	return 0 != (_caps.flags & efrcaps_has_phantom);
}


bool hwcaps::HasSoftClip()
{
	return 0 != (_caps.flags & efrcaps_supports_softclip);
}


bool hwcaps::HasIsocAudioMapping()
{
	return	(_caps.num_1394_play_chan != _caps.num_1394_play_chan_2x) ||
			(_caps.num_1394_play_chan != _caps.num_1394_play_chan_4x) ||
			(_caps.num_1394_rec_chan != _caps.num_1394_rec_chan_2x) ||
			(_caps.num_1394_rec_chan != _caps.num_1394_rec_chan_4x);
}


bool hwcaps::HasOutputBussing()
{
	return (_caps.flags & efrcaps_supports_output_bussing) != 0;
}


void PrintFirmwareVersion(String &str,uint32 ver)
{
	uint32 major,minor,rev,build;

	major = ver >> 24;
	minor = (ver >> 16) & 0xff;
	rev = (ver >> 8) & 0xff;
	build = (ver >> 4) & 0xf;

	str = String::formatted("%d.%d",major,minor);
	
	if (0 != rev)
	{
		str += ".";
		str += String( (int) rev);
	}

	if (0 != build)
	{
		str += ".";
		str += String( (int) build);
	}

	if (0xe == (ver & 0xf))
	{
		str += "E";
	}
}


int32 hwcaps::NumMidiIn()
{
	return _caps.num_midi_in;
}

int32 hwcaps::NumMidiOut()
{
	return _caps.num_midi_out;
}

bool hwcaps::HasOutputNominal()
{
#ifdef ECHO1394
	switch (_caps.hwtype)
	{
		case AUDIOFIRE2 :
		case AUDIOFIRE4 :
		case AUDIOFIRE8 :
		case AUDIOFIRE12 :
		case AUDIOFIRE8P :
			return true;
	}
#endif

	return (_caps.flags & efrcaps_supports_nominal_output_gain) != 0;
}

bool hwcaps::HasInputNominal(int inbus)
{
#ifdef ECHO1394
	switch (_caps.hwtype)
	{
		case AUDIOFIRE2 :
		case AUDIOFIRE4 :
		case AUDIOFIRE8 :
		case AUDIOFIRE12 :
		case AUDIOFIRE8P :
			return true;
	}
#endif

	return (_caps.flags & efrcaps_supports_nominal_input_gain) != 0;
}

bool hwcaps::SupportsRobotGuitar()
{
	return (_caps.flags & efrcaps_supports_robot_guitar) != 0;
}


bool hwcaps::SupportsGuitarCharging()
{
	return (_caps.flags & efrcaps_supports_guitar_charging) != 0;
}
