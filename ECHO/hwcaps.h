#ifndef _hwcaps_h_
#define _hwcaps_h_

class hwcaps
{
protected:
	friend class ehw;

	typedef struct
	{
		byte	type;
		byte	group;
		byte	groupoffset;
	} chanmap;

	echo1394_caps	_caps;	
	
	enum
	{
		max_chan = 64
	};
	
	chanmap	_inmap[ max_chan ];
	chanmap _outmap[ max_chan ];
	
public:
	//
	// Connector types
	//
	enum
	{
		analog = 0,
		spdif,
		adat,
		digital,
		analog_control_room,
		headphones,
		i2s,
		guitar,
		piezo_guitar,
		guitar_string,

		virt = 0x10000,
		dummy
	};

	//
	// Clock types
	// 
	enum
	{
		internal_clock = 0,
		syt_clock,
		word_clock,
		spdif_clock,
		adat_clock,
		adat_b_clock,
		variable_clock,

		num_clock_sources,

		not_specified = 0xff
	};
	
	void init();
	void dump(Logger *log);

	int32 numbusin();
	int32 numbusout();
	int32 numplaychan(int samplerate = 48000);
	int32 numrecchan(int samplerate = 48000);
	
	int32 NumMidiOut();
	int32 NumMidiIn();

	const char *BoxTypeName();
	
	int NumMixInGroups();
	int MixInGroupSize(int group);
	const char *MixInGroupName(int group);
	const char *MixInGroupShortName(int group);
	int MixInGroup(int chan);
	int MixInGroupOffset(int chan);
	int MixInType(int chan);
	
	bool HasPhysicalInputs() { return true; }
	bool HasInputNominal(int inbus);
	
	int NumMixOutGroups();
	int MixOutGroupSize(int group);
	const char *MixOutGroupName(int group);
	const char *MixOutGroupShortName(int group);
	int MixOutGroup(int chan);
	int MixOutGroupOffset(int chan);
	int MixOutType(int chan);
	bool HasOutputNominal();

	int MixPlayType(int chan);
	int32 NumVirtualOutputs() { return 0; }
	
	int MinSampleRate();
	int MaxSampleRate();
	
	bool HasAnalogMirror();

	bool HasDigitalModeSwitch();
	
	bool HasSpdifOut();
	bool HasSpdifIn();

	bool ExternalSync();
	bool ClockSupported(int clock);
	void GetClockName(int clock,String &name);

	bool HasIsocAudioMapping();
	bool HasOutputBussing();

	bool HasDsp();
	uint32 DspVersion();
	
	uint32 ArmVersion();

	bool HasFpga();
	uint32 FpgaVersion();

	bool HasPhantomPower();
	
	bool HasSoftClip();
	
	bool SupportsRobotGuitar();
	bool SupportsGuitarCharging();
};


#endif