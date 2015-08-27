//
// Core definitions for Echo Fireworks Commands
//

#ifndef _core_efc_h_
#define _core_efc_h_

//-----------------------------------------------------------------------------
//
// Firewire node addresses for sending EFC and returning EFR
//
#define efc_node_addr_hi			0xecc0
#define efc_node_addr_lo			0x00000000

#define efr_node_default_addr_hi	0xecc0
#define efr_node_default_addr_lo	0x80000000


//-----------------------------------------------------------------------------
//
// command categories
//

// number of command categories
#define num_efc_cats			11

// categories
#define	cat_hw_info				0
#define cat_flash				1
#define cat_transport			2
#define cat_hw_ctrl				3
#define cat_phys_output_mixer	4		// settings for physical outputs (master outs)
#define cat_phys_input_mixer	5		// settings for physical inputs
#define cat_playback_mixer		6		// settings for playback data coming from 1394
#define cat_record_mixer		7		// settings for recorded data going to 1394
#define cat_monitor_mixer		8
#define cat_io_config			9
#define cat_robot_guitar		10



//-----------------------------------------------------------------------------
//
// command numbers
//

// maximum number of commands in a category
#define max_efc_cmds				10


// cat_hw_info
#define cmd_hwinfo_get_caps			0
#define cmd_get_polled_stuff		1
#define cmd_set_efr_address			2
#define cmd_read_session_block		3
#define cmd_get_debug_info			4
#define cmd_efc_test_dsp			6
#define cmd_efc_test_arm			7
//#define cmd_efc_get_cont_clock_changes	8


// cat_flash
#define	cmd_flash_erase				0
#define cmd_flash_read				1
#define cmd_flash_write				2
#define cmd_flash_get_status		3
#define cmd_flash_get_session_base	4
#define cmd_flash_lock				5


// cat_transport
#define cmd_set_xmit_mode			0
#define cmd_hw_ctrl_config_fw_hdmi	4
#define cmd_midi_output				5


// cat_hw_ctrl
#define	cmd_hw_ctrl_set_clock		0
#define cmd_hw_ctrl_get_clock		1 
#define cmd_hw_ctrl_bsx_handshake	2
#define cmd_hw_ctrl_change_flags	3
#define cmd_hw_ctrl_get_flags		4
#define cmd_hw_ctrl_identify		5
#define cmd_hw_ctrl_reconnect_phy	6
#define cmd_hw_ctrl_set_test_reg	7
#define cmd_hw_ctrl_set_clock_freq	8
#define cmd_hw_ctrl_bus_reset		9


// cat_mixer
#define cmd_set_gain				0
#define cmd_get_gain				1
#define cmd_set_mute				2
#define cmd_get_mute				3
#define cmd_set_solo				4
#define cmd_get_solo				5
#define cmd_set_pan					6
#define cmd_get_pan					7
#define cmd_set_nominal				8
#define cmd_get_nominal				9


// cat_io_config
#define cmd_set_mirror				0
#define cmd_get_mirror				1
#define cmd_set_digital_mode		2
#define cmd_get_digital_mode		3
#define cmd_set_phantom				4
#define cmd_get_phantom				5
#define cmd_set_isoc_audio_map		6
#define cmd_get_isoc_audio_map		7


// cat_robot_guitar
#define cmd_robo_comm_write					0
#define cmd_robo_comm_read					1
#define cmd_robot_guitar_deprecated			2
#define cmd_robot_guitar_write_test_reg		3
#define cmd_robot_guitar_read_status_reg	4
#define cmd_robot_guitar_deprecated2		5
#define cmd_robot_guitar_set_hex_mode_ctrl	6
#define cmd_robot_guitar_set_charge_state	7
#define cmd_robot_guitar_get_charge_state	8



//-----------------------------------------------------------------------------
//
// return values
//
#define efc_ok				0
#define efc_bad				1
#define efc_bad_cmd_id		2			// either the category or the command was bad
#define efc_comm_err		3
#define	efc_bad_quad_count	4
#define efc_unsupported		5
#define efc_1394_timeout	6
#define efc_dsp_timeout		7
#define efc_bad_rate		8
#define efc_bad_clock		9
#define efc_bad_channel		10
#define efc_bad_pan			11
#define efc_flash_busy		12
#define efc_bad_mirror      13
#define efc_bad_led			14
#define efc_bad_parameter	15
#define efc_incomplete		0x80000000
#define efc_unspecified		0xffffffff



//-----------------------------------------------------------------------------
//
// efc header struct
//
typedef struct tag_efc_header
{
	uint32	version;	// the version of the command protocol - set this to efc_version					
	uint32  seqnum;		// sequence number 
	uint32	category;	// what type of command is this
	uint32	cmd;		// the actual command - depends on the category
	uint32	rval;		// the return status of the command
} efc_header;

#define efc_version	1

//
// The extended efr_hwinfo_caps fields are supported for EFC version 1 or greater
//
#define min_efc_version_ext_hwcaps_supported	1



//-----------------------------------------------------------------------------
//
// Hardware info struct
//
#define hwinfo_get_caps_name_size		32

#define efrcaps_supports_dynamic_address			0x01	// Supports setting the EFR destination address
#define efrcaps_supports_mirroring					0x02	// 400F & 1200F control room/monitor out
#define efrcaps_digital_mode_switch_spdif_coax		0x04	// this box can be switched to S/PDIF coax mode
#define efrcaps_digital_mode_switch_aesebu_xlr		0x08	// this box can be switched to AES/EBU XLR mode
#define efrcaps_has_dsp								0x10	// this box has a DSP
#define efrcaps_has_fpga							0x20	// this box has an FPGA
#define efrcaps_has_phantom							0x40	// this box has software controllable phantom power
#define efrcaps_supports_output_bussing				0x80	// supports playback routing
#define efrcaps_supports_input_gain					0x100	// has adjustable input gains
#define efrcaps_digital_mode_switch_spdif_optical	0x200	// this box can be switched to S/PDIF optical mode
#define efrcaps_digital_mode_switch_adat			0x400	// this box can be switched to ADAT optical mode
#define efrcaps_supports_nominal_input_gain			0x800	// this box supports +4/-10 switch for analog inputs
#define efrcaps_supports_nominal_output_gain		0x1000	// this box supports +4/-10 switch for analog outputs
#define efrcaps_supports_softclip					0x2000	// this box supports switchable soft clip analog inputs
#define efrcaps_supports_robot_guitar				0x4000  // this box supports a robot guitar hex input
#define efrcaps_supports_guitar_charging			0x8000  // this box can charge the Dark Fire battery


#define efrcaps_digital_mode_switch		(efrcaps_digital_mode_switch_spdif_coax | \
										efrcaps_digital_mode_switch_aesebu_xlr | \
										efrcaps_digital_mode_switch_spdif_optical | \
										efrcaps_digital_mode_switch_adat)

#define efc_clock_internal				0
#define efc_clock_syt					1
#define efc_clock_word					2
#define efc_clock_spdif					3
#define efc_clock_adat_a				4
#define efc_clock_adat_b				5
#define efc_clock_continuous			6
#define num_efc_clocks					7

#define efc_clock_adat					efc_clock_adat_a


#define efc_midi_in_1					8
#define efc_midi_out_1					9				
#define efc_midi_in_2					10
#define efc_midi_out_2					11				

#define efc_guitar_charging				29
#define efc_guitar_stereo_connect		30
#define efc_guitar_hex_signal			31

#define efc_clock_not_specified			0xffffffff

#define efc_clock_flag_internal			(1 << efc_clock_internal)
#define efc_clock_flag_1394				(1 << efc_clock_syt)
#define efc_clock_flag_word				(1 << efc_clock_word)
#define efc_clock_flag_spdif			(1 << efc_clock_spdif)
#define efc_clock_flag_adat_a			(1 << efc_clock_adat_a)
#define efc_clock_flag_adat_b			(1 << efc_clock_adat_b)
#define efc_clock_flag_continuous		(1 << efc_clock_continuous)

#define efc_clock_flag_adat				efc_clock_flag_adat_a


#define efc_midi_in_1_flag				(1 << efc_midi_in_1)
#define efc_midi_out_1_flag				(1 << efc_midi_out_1)
#define efc_midi_in_2_flag				(1 << efc_midi_in_2)
#define efc_midi_out_2_flag				(1 << efc_midi_out_2)

#define efc_guitar_charging_flag		(1 << efc_guitar_charging)
#define efc_guitar_stereo_connect_flag	(1 << efc_guitar_stereo_connect)
#define efc_guitar_hex_signal_flag		(1 << efc_guitar_hex_signal)

#define chan_type_analog				0
#define chan_type_spdif					1
#define chan_type_adat					2
#define chan_type_spdif_or_adat			3
#define chan_type_analog_mirror			4
#define chan_type_headphones			5
#define chan_type_i2s					6
#define chan_type_guitar				7
#define chan_type_piezo_guitar			8
#define chan_type_guitar_string			9

typedef struct 
{
	byte	type;
	byte	count;
} caps_phys_group;

#define hwinfo_caps_max_groups			8

// vendor ID values
#define ECHO_VENDOR_ID		0x001486
#define LOUD_VENDOR_ID		0x000FF2
#define GIBSON_VENDOR_ID	0x00075B


// values for hwtype 
#define AUDIOFIRE2			0x000af2
#define AUDIOFIRE4			0x000af4
#define AUDIOFIRE8			0x000af8
#define AUDIOFIRE8P			0x000af9
#define AUDIOFIRE12			0x00af12
#define AUDIOFIRE12HD		0x0af12d
#define AUDIOFIRE12_APPLE	0x0af12a
#define FWHDMI				0x00afd1
#define ONYX400F			0x00400f 
#define ONYX1200F			0x01200f
#define FIREWORKS8			0x0000f8
#define GIBSON_RIP          0x00afb2
#define GIBSON_AUDIOPUNK    0x00afb9

typedef struct tag_efr_hwinfo_caps
{
	uint32			flags;			// you always need flags!

	uint32			node_id_hi;		// upper 24 bits == 1394 vendor serial number
									// low 8 bits == serial number high 8 bits
	uint32			node_id_lo;		// serial number unique to this box - low 32 bits

	uint32			hwtype;			// ID unique to this type of product
	uint32			hwversion;		// rev ID of this type of product

	char			vendorname[ hwinfo_get_caps_name_size ];
	char			boxname[ hwinfo_get_caps_name_size ];	

	uint32			inputclocks;	// clocks supported by this box - use clock flags above

	uint32			num_1394_play_chan;	// Number of 1394 playback channels (iPCR0)
	uint32			num_1394_rec_chan;	// Number of 1394 record channels (oPCR0)

	uint32			num_phys_audio_out;	// total number of mono output channels (analog and digital)
	uint32			num_phys_audio_in;	// total number of mono input channels (analog and digital)
	
	uint32			num_out_groups;
	caps_phys_group	out_groups[ hwinfo_caps_max_groups ];

	uint32			num_in_groups;
	caps_phys_group	in_groups[ hwinfo_caps_max_groups ];
	
	uint32			num_midi_out;
	uint32			num_midi_in;
	
	uint32			max_sample_rate;
	uint32			min_sample_rate;
	
	uint32			dspversion;		// version # for DSP code
	uint32			armversion;		// version # for ARM code

	uint32			num_mix_play_chan;
	uint32			num_mix_rec_chan;

	//
	// New extended information for 1200F & Fireworks 3
	// 
	// This is only returned if the command specifices efc_version >= 1
	//
	uint32			fpga_version;	// version # for FPGA

	uint32			num_1394_play_chan_2x;	// 1394 playback channels at 88.2 and 96 kHz
	uint32			num_1394_rec_chan_2x;	// 1394 record channels at 88.2 and 96 kHz

	uint32			num_1394_play_chan_4x;	// 1394 playback channels at 176.4 and 192 kHz
	uint32			num_1394_rec_chan_4x;	// 1394 record channels at 176.4 and 192 kHz

	uint32			reserved[16];

} efr_hwinfo_caps;

typedef efr_hwinfo_caps echo1394_caps;



//-----------------------------------------------------------------------------
//
// Polled stuff - meters and available clocks
//
// To get this, send a regular EFC struct with the appropriate command
// and category.  Use this to parse the response.
//
// This is in its own struct so the main efc struct doens't get too big
//
// Output meters are in the first num_output_meters entries of the 
// "meters" array, followed by the input meters.
//
#define max_num_efr_meters 100

typedef struct tag_efr_polled_stuff
{
	uint32		quads;			// size of this struct in quads, including this field

	efc_header	h;
	
	uint32		boxstatus;		// uses efc_clock_flag_ and efc_midi_flag - shows which clocks may be selected
								//										    and midi status	
	uint32		spdifdetect;	// for boxes with multiple S/PDIF inputs
	uint32		adatdetect;		// for boxes with multiple ADAT inputs
	uint32		reserved3;
	uint32		reserved4;
	
	uint32		num_output_meters;
	uint32		num_input_meters;
	uint32		reserved5;
	uint32		reserved6;
	
	int32		meters[ max_num_efr_meters ];	// linear, left justified values from 0 to 0x7fffff00

} efr_polled_stuff;

//
// Use this macro to specify the size of the struct; specify the number of input and
// output meters
//
#define EFR_POLLED_STUFF_BYTES(outputs,inputs)	(	sizeof(efr_polled_stuff) -				\
													sizeof(int32)*max_num_efr_meters +		\
													sizeof(int32)*(outputs + inputs) )
													
#define EFR_POLLED_STUFF_QUADS(outputs,inputs)	(EFR_POLLED_STUFF_BYTES(outputs,inputs)/sizeof(uint32))


//-----------------------------------------------------------------------------
//
// Dynamic EFR address
//
typedef struct tag_efc_dynamic_efr_address
{
	uint32		hi;		// only lower 16 bits should be set
	uint32		lo;		// all 32 bits are valid
} efc_dynamic_efr_address;


//-----------------------------------------------------------------------------
//
// read a block of the session
//	
#define max_efc_session_block_quads 64

typedef struct tag_efc_session_block
{
	uint32		offsetquads;	// starting position in quads
	uint32		quadcount;		// number of quads to read
} efc_session_block;


typedef struct tag_efr_session_block
{
	uint32		offsetquads;	// starting position in quads
	uint32		quadcount;		// number of quads actually read
	
	uint32		data[ max_efc_session_block_quads ];
} efr_session_block;


//-----------------------------------------------------------------------------
//
// debug info
//

typedef struct tag_efr_debug_info
{
	uint32		iso1_flushes;
	uint32		iso1_underruns;

	uint32		fw3_ctrl;
	uint32		fw3_ctrl_written;

	uint32		average_fifo_level;
	int32		packet_index_delta;

	uint32		iso1_sync_count;
	
	uint32		reserved[ 9 ];
} efc_debug_info,efr_debug_info;


//-----------------------------------------------------------------------------
//
// EFC test
//

typedef struct tag_efc_test
{
	uint32		value;
} efc_test_data,efr_test_data;


//-----------------------------------------------------------------------------
//
// flash structs
//	
#define max_efc_flash_bytes	256
#define max_efc_flash_quads (max_efc_flash_bytes / sizeof(uint32))

typedef struct tag_efc_flash_read
{
	uint32			addr;		// relative to start of flash
	uint32			quadcount;	// # of quadlets
} efc_flash_read;


typedef struct tag_efr_flash_read
{
	uint32			addr;		// relative to start of flash
	uint32			quadcount;	// # of quadlets
	
	uint32			data[ max_efc_flash_quads ];
} efr_flash_read;


typedef struct tag_efc_flash_write
{
	uint32			addr;		// relative to start of flash
	uint32			quadcount;	// # of quadlets
	
	uint32			data[ max_efc_flash_quads ];
} efc_flash_write;

typedef struct tag_efc_flash_erase
{
	uint32			addr;		// relative to start of flash
								// should always be on block boundary
} efc_flash_erase,efr_flash_session_base;

typedef struct tag_efc_flash_lock
{
	uint32			lock;		// non-zero for lock, zero for unlock
} efc_flash_lock;



//-----------------------------------------------------------------------------
//
// transport structs
//	
typedef struct tag_efc_set_xmit_mode
{
	uint32			mode;		// 0 for Windows driver mode
								// non-zero for 61883-6
} efc_set_xmit_mode;


typedef struct tag_efc_transport_state
{
	uint32			rec_enabled;	// 0 to disable record 
									//
									// non-zero enable record - audio data
									// will be transferred from DSP -> ARM
	
	uint32			reset_playback;	// set this to 1 to reset the DSP's internal
									// playback FIFO
} efc_transport_state;


typedef struct tag_efc_transport_buffering
{
	uint32			factor;
} efc_transport_buffering;

typedef struct tag_efr_transport_buffering
{
	uint32			factor;
	uint32			max;
} efr_transport_buffering;


//-----------------------------------------------------------------------------
//
// Mixer
//
// Gain values are 32 bit unsigned integers in 8.24 format.  
//
// Examples:
//			0 dB	0x01000000
//			-6 dB	0x00800000
//			-12 dB	0x00400000
//			+6 dB	0x02000000
//
// Channels are numbered starting with 0.
//
// Pan ranges are defined below
//

#define EFC_PAN_HARD_LEFT	0
#define EFC_PAN_HARD_RIGHT	255
#define EFC_PAN_CENTER		((EFC_PAN_HARD_RIGHT + 1) / 2)



// Set input & output gain, get input & output gain
typedef struct tag_efc_gain
{
	int32	chan;
	uint32	gain;
} efc_gain, efr_gain;


// Set input & output mute, get input & output mute
typedef struct tag_efc_mute
{
	int32	chan;
	uint32	mute;		// 0 if not muted, 1 if muted
} efc_mute, efr_mute;


// Set input & output solo, get input & output solo
typedef struct tag_efc_solo
{
	int32	chan;
	uint32	solo;		// 0 for solo off, 1 for solo on
} efc_solo, efr_solo;


// Set input & output pan, get input & output pan
// Currently not used, but included for reasons of symmetry
typedef struct tag_efc_pan
{
	int32	chan;
	uint32	pan;		// 0 to 255, 0 is hard left, 255 is hard right
} efc_pan, efr_pan;


// Get & set monitor gain
typedef struct tag_efc_monitor_gain
{
	int32	input;
	int32	output;
	uint32	gain;
} efc_monitor_gain, efr_monitor_gain;


// Get & set monitor mute
typedef struct tag_efc_monitor_mute
{
	int32	input;
	int32	output;
	uint32	mute;		// 0 if not muted, 1 if muted
} efc_monitor_mute, efr_monitor_mute;


// Get & set monitor solo
typedef struct tag_efc_monitor_solo
{
	int32	input;
	int32	output;
	uint32	solo;		// 0 for solo off, 1 for solo on
} efc_monitor_solo, efr_monitor_solo;


// Get & set monitor pan
typedef struct tag_efc_monitor_pan
{
	int32	input;
	int32	output;
	uint32	pan;		// 0 to 255, 0 is hard left, 255 is hard right
} efc_monitor_pan, efr_monitor_pan;

// Set input & output nominal levels, get input & output nominal levels
typedef struct tag_efc_nominal
{
	int32	chan;
	uint32	shift;		
} efc_nominal, efr_nominal;



//-----------------------------------------------------------------------------
//
// channel mirroring for 400F & 1200F
//

// Get & set mirror
typedef struct tag_efc_mirror
{
	int32	output;		// which physical output channel pair to mirror - 0 indexing
} efc_mirror, efr_mirror;


//-----------------------------------------------------------------------------
//
// digital mode switch
//

// Get & set digital mode
#define digital_mode_spdif_coax		0
#define digital_mode_aesebu_xlr		1
#define digital_mode_spdif_optical	2
#define digital_mode_adat			3


typedef struct tag_efc_digital_mode
{
	int32	mode;
} efc_digital_mode, efr_digital_mode;




//-----------------------------------------------------------------------------
//
// Isoc channel mapping 
//
//   -----------------------------------------------------------------------
//
// For the 1200F:
//
// The "playmap" and "recmap" arrays map a stereo pair from the isoc audio
// packet to a stereo pair of physical audio channels.
//
// The array index is the isoc packet offset divided by two.
//
// The array contents at that index is the physical audio input or output.
//
// If the array holds -1, then that isoc audio slot isn't connected to 
// a physical channel.
//
//   -----------------------------------------------------------------------
//
// For the AudioFire2 & 4:
//
// Only the playmap array is used; it maps a physical output to an offset 
// into the isoc packet.
//
// The array index is the physical audio output divided by two.
//
//  The array contents at that index is the isoc packet offset.
//

#define EFC_MAX_ISOC_MAP_ENTRIES	32

typedef struct tag_efc_isoc_map
{
	uint32	samplerate;
	uint32	flags;
	
	uint32	num_playmap_entries;
	uint32	num_phys_out;
	int32	playmap[ EFC_MAX_ISOC_MAP_ENTRIES ];

	uint32	num_recmap_entries;
	uint32	num_phys_in;
	int32	recmap[ EFC_MAX_ISOC_MAP_ENTRIES ];
} efc_isoc_map,efr_isoc_map;


//-----------------------------------------------------------------------------
//
// hardware control structs
//
// Set the high bit in the "index" member to force a codec reset
// even if the clock is the same
//
#define EFC_FORCE_CLOCK_CHANGE	0x80000000

#define EFC_SERIAL_DATA_FORMAT_LEFT_JUSTIFIED	0
#define EFC_SERIAL_DATA_FORMAT_I2S				1

typedef struct tag_efc_set_clock
{
	uint32			clock;
	uint32			samplerate;
	uint32			index;
} efc_set_clock, efr_get_clock;


typedef struct tag_efc_config_fw_hdmi
{
	uint32			playback_drop_ratio;		// 1, 2, or 4.  One of the ratios must be one
	uint32			record_stretch_ratio;		// 1, 2, or 4

	uint32			serial_bits_per_sample;		// 16 or 24
	uint32			serial_data_format;			// EFC_SERIAL_DATA_FORMAT_??? from above
} efc_config_fw_hdmi;


//-----------------------------------------------------------------------------
//
// BSX handshake - for Fireworks 2, pass the box LED bits
// to the DSP.
//
// For the 1200F this returns the various front panel
// controls
//
// LEDs are active low - clear the appropriate
// bit to turn on the LED, set it to turn it off
// 
// This command is private between the DSP & the ARM; if it's
// sent over 1394, the ARM will reject the command.
//
#define efc_led_midi_in_shift 	30
#define efc_led_midi_out_shift	29

#define efc_led_1394		0x80000000
#define efc_led_midi_in		(1 << efc_led_midi_in_shift)
#define efc_led_midi_out	(1 << efc_led_midi_out_shift)

typedef struct tag_efc_bsx_handshake
{
	uint32			mask;		// or-mask of the efc_led_??? values
} efc_bsx_handshake;

typedef struct tag_efr_bsx_handshake 
{
	uint32			samplerate; // rate selected on front panel
	uint32			clock;		// clock selected on front panel
	uint32			panelflags;	// miscellaneous other front panel stuff
} efr_bsx_handshake;


//
// To set a flag, just put that flag in the "setmask" field.  The ARM will do
// this:
//
// ARM flags |= setmask
//
// To clear a flag, just put that same value in the "clearmask" field - the
// ARM will do this:
//
// ARM flags &= ~clearmask
//
// that is, the ARM will do a logical bitwise invert of the clearmask
// and then and that with the existing flags. 
//
#define efc_ctrl_mixer_enabled			0x0001	// set this flag to enable DSP mixing
#define efc_ctrl_spdif_pro				0x0002	// clear this flag for consumer mode
#define efc_ctrl_spdif_non_audio		0x0004	// set this flag for "non-audio" status bit
												// clear it to disable monitors

#define efc_1200f_ctrl_room_ab_select	0x0100	// 1200F - clear for "A" output selected
												//         set for "B" output selected
#define efc_1200f_volume_knob_bypass	0x0200	// 1200F - state of analog out volume knob bypass
#define efc_1200f_meter_mode_mask		0x0003	// 1200F - current front panel meter mode
#define efc_1200f_meter_mode_shift		12		// two bits starting at this bit pos
#define efc_1200f_panel_flag_mask		0xff00

#define efc_flag_soft_clip_enabled		0x00010000

#define efc_flag_robot_guitar_hex_mode	0x20000000
#define efc_flag_automatic_charging		0x40000000
#define efc_flag_phantom_power			0x80000000

typedef struct tag_efc_change_flags
{
	uint32			setmask;
	uint32			clearmask;
} efc_change_flags;


typedef struct tag_efc_get_flags
{
	uint32			flags;
} efr_get_flags;


//
// Frequency register for continuous clock mode 
//
typedef struct tag_efc_clock_freq
{
	uint32			frequency;		// (samplerate * 512)/1500
} efc_clock_freq;


//-----------------------------------------------------------------------------
//
// Write Robot Guitar test mode register or read the status reg - only for RIP
//

// Robot guitar hex input test bits
#ifndef FW3_STATUS_HEX_SYNC_LEVEL_BIT
#define FW3_STATUS_HEX_SYNC_LEVEL_BIT	10
#define FW3_STATUS_HEX_DATA_LEVEL_BIT	11
#define FW3_STATUS_HEX_SYNC_LEVEL_MASK	(1 << FW3_STATUS_HEX_SYNC_LEVEL_BIT)
#define FW3_STATUS_HEX_DATA_LEVEL_MASK	(1 << FW3_STATUS_HEX_DATA_LEVEL_BIT)
#endif

typedef struct tag_efc_reg_value
{
	uint32			value;
} efc_reg_value,efr_reg_value;


//-----------------------------------------------------------------------------
//
// Robot Guitar communication
//	
#define max_robo_comm_efc_data_bytes 256

typedef struct tag_efc_robo_comm_write
{
	uint32		write_byte_count;			// number of bytes to write to the guitar
	byte		data[ max_robo_comm_efc_data_bytes ];
} efc_robo_comm_write;


typedef struct tag_efr_robo_comm_write
{
	uint32		success;			// 1 if write was OK
									// 0 means last write would have 
									// overflowed the FIFO; try again
} efr_robo_comm_write;


typedef struct tag_efr_robo_comm_read
{
	uint32		byte_count;			// number of bytes returned in this packet
	byte		data[ max_robo_comm_efc_data_bytes ];
} efr_robo_comm_read;


typedef struct tag_efc_charge_state
{
	uint32 manual_charge_on;		// 0 == charge off, 1 == charge now
	uint32 automatic_charging;		// 0 == never, 1 == automatic charging after timeout enabled
	uint32 inactivity_time_seconds;	// charge the guitar this many seconds after you stop playing
} efc_charge_state,efr_charge_state;



//-----------------------------------------------------------------------------
//
// General "set mode" struct
//
// This is used for phantom power and the guitar mode
//
#define EFC_ROBOT_GUITAR_MODE_HEX_CTRL_OFF		0
#define EFC_ROBOT_GUITAR_MODE_LEGACY_PING_ON	1
#define EFC_ROBOT_GUITAR_MODE_HEX_TOGGLE_ON		2

typedef struct tag_efc_set_mode
{
	uint32			mode;
} efc_set_mode,efr_get_mode;


//-----------------------------------------------------------------------------
//
// MIDI
//	
#define max_midi_efc_data_bytes 256

typedef struct tag_efc_midi
{
	uint32		midi_port;			// which MIDI input or output
	uint32		byte_count;			// number of MIDI bytes
	byte		data[ max_midi_efc_data_bytes ];
} efc_midi;

typedef struct tag_efr_midi_out
{
	uint32		success;			// 1 if write was OK
									// 0 means last write would have 
									// overflowed the FIFO; try again
} efr_midi_out;



//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//
// efc and efr super-struct
//
typedef struct tag_efc_efr
{
	uint32		quads;		// size of this struct in quads, including this field

	efc_header	h;
	
	union
	{
		union
		{
			efc_dynamic_efr_address		efr_address;
			efc_session_block			session_block;
			efc_debug_info				debug_info;
			efc_test_data				test_data;
			
			efc_flash_read				flash_read;
			efc_flash_write				flash_write;
			efc_flash_erase				flash_erase;
			efc_flash_lock				flash_lock;

			efc_set_xmit_mode			xmit_mode;
			efc_transport_state			transport_state;
			efc_transport_buffering		transport_buffering;
			efc_config_fw_hdmi			config_fw_hdmi;

			efc_midi					midi;
		
			efc_gain					gain;
			efc_mute					mute;
			efc_solo					solo;
			efc_pan						pan;
			
			efc_monitor_gain			monitor_gain;
			efc_monitor_mute			monitor_mute;
			efc_monitor_solo			monitor_solo;
			efc_monitor_pan				monitor_pan;

			efc_nominal					nominal;

			efc_mirror					mirror;
			efc_digital_mode			digital_mode;
			efc_isoc_map				isoc_map;
			
			efc_set_clock				set_clock;
			efc_bsx_handshake			handshake;
			efc_change_flags			ctrl_flags;
			efc_clock_freq				clock_freq;

			efc_robo_comm_write			robo_comm_write;
			efc_set_mode				mode;
			efc_reg_value				reg_value;
			efc_charge_state			charge_state;
		} c;
		
		union
		{
			efr_hwinfo_caps				hwinfo_caps;
			efr_session_block			session_block;
			efr_debug_info				debug_info;
			efr_test_data				test_data;
			
			efr_flash_read				flash_read;
			efr_flash_session_base		flash_session_base;

			efr_transport_buffering		transport_buffering;

			efr_midi_out				midi_out;
			
			efr_gain					gain;
			efr_mute					mute;
			efr_solo					solo;
			efr_pan						pan;
			
			efr_monitor_gain			monitor_gain;
			efr_monitor_mute			monitor_mute;
			efr_monitor_solo			monitor_solo;
			efr_monitor_pan				monitor_pan;

			efr_nominal					nominal;
			
			efr_mirror					mirror;
			efr_digital_mode			digital_mode;
			efr_isoc_map				isoc_map;
			
			efr_get_clock				get_clock;
			efr_bsx_handshake			handshake;
			efr_get_flags				ctrl_flags;

			efr_robo_comm_write			robo_comm_write;
			efr_robo_comm_read			robo_comm_read;
			efr_reg_value				reg_value;
			efr_charge_state			charge_state;
			efr_get_mode				mode;
		} r;
		
	} u;
	
} efc,efr;



//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//
// Used for packaging EFC & EFR over AVC
//
#define EFC_AVC_BYTE_OFFSET		5


//''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
//
// structure sizes
//
#define EFC_MAX_QUADS	240
#define EFC_MAX_BYTES	(EFC_MAX_QUADS*sizeof(uint32))

#define EFC_BYTES(s)	(sizeof(uint32) + sizeof(efc_header) + sizeof(s))
#define EFC_QUADS(s)	(EFC_BYTES(s)/sizeof(uint32))

#define EFR_BYTES(s)	(sizeof(uint32) + sizeof(efc_header) + sizeof(s))
#define EFR_QUADS(s)	(EFR_BYTES(s)/sizeof(uint32))

#define EFC_BASE_BYTES	(sizeof(uint32) + sizeof(efc_header))
#define EFC_BASE_QUADS	(EFC_BASE_BYTES/sizeof(uint32))

#define EFR_BASE_BYTES	EFC_BASE_BYTES
#define EFR_BASE_QUADS	(EFR_BASE_BYTES/sizeof(uint32))

#endif // _core_efc_h_
