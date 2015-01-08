/*
 * CoreMod by CyrIng
 * 
 * Copyright (C) 2012-2015 CYRIL INGENIERIE
 * 
 * Licenses: GPL2
 * 
 * coremod.h
 */

#define _MAJOR   "1"
#define _MINOR   "1.5"
#define _NIGHTLY "0"
#define AutoDate "CoreMod "_MAJOR"."_MINOR"-"_NIGHTLY"    (C) CYRIL INGENIERIE    "__DATE__
static  char    version[] = AutoDate;

// Intel® 64 and IA-32 Architectures Software Developer's Manual - Vol. 3C
// Table 34-5 (Nehalem) and 34-10 (Sandy Bridge)

#define MSR_PLATFORM_INFO	0xce
#define IA32_MPERF		0xe7
#define IA32_APERF		0xe8
#define IA32_PERF_STATUS	0x198
#define IA32_PERF_CTL		0x199
#define IA32_CLOCK_MODULATION	0x19a
#define IA32_THERM_STATUS	0x19c
#define IA32_MISC_ENABLE	0x1a0
#define MSR_TEMPERATURE_TARGET	0x1a2
#define MSR_TURBO_RATIO_LIMIT	0x1ad

#define PCI_CONFIG_ADDRESS(bus, dev, fn, reg) \
	(0x80000000 | (bus << 16) | (dev << 11) | (fn << 8) | (reg & ~3))


struct COREINFO
{
	struct
	{
		unsigned  long long
				ReservedBits1	:  8-0,
				MaxNonTurboRatio: 16-8,
				ReservedBits2	: 28-16,
				Ratio_Limited	: 29-28,
				TDC_TDP_Limited	: 30-29,
				ReservedBits3	: 40-30,
				MinimumRatio	: 48-40,
				ReservedBits4	: 64-48;
	} Platform;
	unsigned long long
				mperf[2],
				aperf[2];
	struct
	{
		unsigned long long
				Ratio		: 16-0,
				ReservedBits1	: 64-16;
	} Perf;
	struct
	{
		unsigned long long
				Target		: 16-0,
				ReservedBits1	: 32-16,
				NOT_IDA		: 33-32,
				ReservedBits2	: 64-33;
	} Ctl;
	struct
	{
		unsigned long long
				ReservedBits1	:  1-0,
				DutyCycle	:  4-1,
				OnDemand_Enable	:  5-4,
				ReservedBits2	: 64-5;
	} ClockMod;
	struct
	{
		unsigned long long
				Status		:  1-0,
				StatusLog	:  2-1,
				PROCHOT		:  3-2,
				PROCHOTLog	:  4-3,
				CriticalTemp	:  5-4,
				CriticalTempLog	:  6-5,
				Threshold1	:  7-6,
				Threshold1Log	:  8-7,
				Threshold2	:  9-8,
				Threshold2Log	: 10-9,
				PowerLimit	: 11-10,
				PowerLimitLog	: 12-11,
				ReservedBits1	: 16-12,
				DTS		: 23-16,
				ReservedBits2	: 27-23,
				Resolution	: 31-27,
				ReadingValid	: 32-31,
				ReservedBits3	: 64-32;
	} Therm;
	struct
	{
		unsigned long long
				FastStrings	:  1-0,
				ReservedBits1	:  3-1,
				TCC		:  4-3,
				ReservedBits2	:  7-4,
				PerfMonitor	:  8-7,
				ReservedBits3	: 11-8,
				NOT_BTS		: 12-11,
				NOT_PEBS	: 13-12,
				ReservedBits4	: 16-13,
				EIST		: 17-16,
				ReservedBits5	: 18-17,
				FSM		: 19-18,
				ReservedBits6	: 22-19,
				CPUID_Maxval	: 23-22,
				NOT_xTPR	: 24-23,
				ReservedBits7	: 34-24,
				NOT_XD_Bit	: 35-34,
				ReservedBits8	: 38-35,
				NOT_TurboMode	: 39-38,
				ReservedBits9	: 64-39;
	} Misc;
	struct
	{
		unsigned long long
				ReservedBits1	: 16-0,
				Target		: 24-16,
				ReservedBits2	: 64-24;
	} Temp;
	struct
	{
		unsigned long long
				MaxRatio_1C	:  8-0,
				MaxRatio_2C	: 16-8,
				MaxRatio_3C	: 24-16,
				MaxRatio_4C	: 32-24,
				ReservedBits1	: 64-32;
	} Turbo;
};


struct FEATURES
{
	struct
	{
		struct
		{
		unsigned
			Stepping	:  4-0,
			Model		:  8-4,
			Family		: 12-8,
			ProcType	: 14-12,
			Unused1		: 16-14,
			ExtModel	: 20-16,
			ExtFamily	: 28-20,
			Unused2		: 32-28;
		} EAX;
		struct
		{
		unsigned
			Brand_ID	:  8-0,
			CLFSH_Size	: 16-8,
			MaxThread	: 24-16,
			APIC_ID		: 32-24;
		} EBX;
		struct
		{
		unsigned
			SSE3	:  1-0,
			PCLMULDQ:  2-1,
			DTES64	:  3-2,
			MONITOR	:  4-3,
			DS_CPL	:  5-4,
			VMX	:  6-5,
			SMX	:  7-6,
			EIST	:  8-7,
			TM2	:  9-8,
			SSSE3	: 10-9,
			CNXT_ID	: 11-10,
			Unused1	: 12-11,
			FMA	: 13-12,
			CX16	: 14-13,
			xTPR	: 15-14,
			PDCM	: 16-15,
			Unused2	: 17-16,
			PCID	: 18-17,
			DCA	: 19-18,
			SSE41	: 20-19,
			SSE42	: 21-20,
			x2APIC	: 22-21,
			MOVBE	: 23-22,
			POPCNT	: 24-23,
			TSCDEAD	: 25-24,
			AES	: 26-25,
			XSAVE	: 27-26,
			OSXSAVE	: 28-27,
			AVX	: 29-28,
			F16C	: 30-29,
			RDRAND	: 31-30,
			Unused3	: 32-31;
		} ECX;
		struct
		{
		unsigned
			FPU	:  1-0,
			VME	:  2-1,
			DE	:  3-2,
			PSE	:  4-3,
			TSC	:  5-4,
			MSR	:  6-5,
			PAE	:  7-6,
			MCE	:  8-7,
			CX8	:  9-8,
			APIC	: 10-9,
			Unused1	: 11-10,
			SEP	: 12-11,
			MTRR	: 13-12,
			PGE	: 14-13,
			MCA	: 15-14,
			CMOV	: 16-15,
			PAT	: 17-16,
			PSE36	: 18-17,
			PSN	: 19-18,
			CLFSH	: 20-19,
			Unused2	: 21-20,
			DS	: 22-21,
			ACPI	: 23-22,
			MMX	: 24-23,
			FXSR	: 25-24,
			SSE	: 26-25,
			SSE2	: 27-26,
			SS	: 28-27,
			HTT	: 29-28,
			TM1	: 30-29,
			Unused3	: 31-30,
			PBE	: 32-31;
		} EDX;
	} Std;
	unsigned	ThreadCount;
	unsigned	LargestExtFunc;
	struct
	{
		struct
		{
			unsigned
			LAHFSAHF:  1-0,
			Unused1	: 32-1;
		} ECX;
		struct
		{
			unsigned
			Unused1	: 11-0,
			SYSCALL	: 12-11,
			Unused2	: 20-12,
			XD_Bit	: 21-20,
			Unused3	: 26-21,
			PG_1GB	: 27-26,
			RDTSCP	: 28-27,
			Unused4	: 29-28,
			IA64	: 30-29,
			Unused5	: 32-30;
		} EDX;
	} Ext;
	char		BrandString[48+1];
};



struct IMCINFO
{
	unsigned ChannelCount;
	struct CHANNEL
	{
		struct
		{
			unsigned
			tCL,
			tRCD,
			tRP,
			tRAS,
			tRRD,
			tRFC,
			tWR,
			tRTPr,
			tWTPr,
			tFAW,
			B2B;
		} Timing;
	}	*Channel;
};


// System Management BIOS (SMBIOS) Reference Specification
// Version: 2.7.1 § 7.5

struct HEADER
{
		unsigned int
				Type	:  8,
				Lenght	:  8,
				Handle	: 16;
};

#define STRING_DELIMITER 0

struct STRING
{
		int		ID;
		char		*Buffer;
		struct STRING	*Link;
};

struct STRUCTINFO
{
	struct HEADER		Header;
	unsigned int		Dimension;
	unsigned long long	*Attrib;
	struct STRING		*String;
};

struct PACKED
{
		const	int	Type;
		const	int	Instance;
		FILE		*File;
		size_t		Length;
		const	int	*Tape;
};

#define _B_		0
#define _W_		1
#define _D_		2
#define _Q_		3
#define _S_		,
#define _EOT_		-1

#define SMBIOS_BIOSINFO_TYPE		0
#define SMBIOS_BIOSINFO_INSTANCE	0

#define SMBIOS_BIOSINFO_PACKED {_B_ _S_ _B_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _Q_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _EOT_}

struct SMBIOS0
{
		unsigned long long
				Vendor,
				Version,
				Address,
				Release_Date,
				ROM_Size,
				Characteristics,
				Extension_Bytes,
				Major_Release,
				Minor_Release,
				Firmware_Major,
				Firmware_Minor;
};

struct BIOSINFO
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS0		*Attrib;
	struct STRING		*String;
};


#define SMBIOS_BOARDINFO_TYPE		2
#define SMBIOS_BOARDINFO_INSTANCE	0
/*
* Spec. for a multi motherboard
#define SMBIOS_BOARDINFO_PACKED {_B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _EOT_}

* The ASUS Rampage II Gene is a single motherboard
*/
#define SMBIOS_BOARDINFO_PACKED {_B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _EOT_}
#define SMBIOS_BOARDINFO_EXTENS {_W_ _S_ _EOT_}

struct SMBIOS2
{
		unsigned long long
				Manufacturer,
				Product,
				Version,
				Serial,
				AssetTag,
				Feature,
				Location,
				Chassis_Handle,
				Board_Type,
				Number_Object,
				Object_Handles;
			//	Use Attrib[10] to Attrib[265] to read the list of the 255 object handles
};

struct BOARDINFO
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS2		*Attrib;
	struct STRING		*String;
};


#define SMBIOS_PROCINFO_TYPE		4
#define SMBIOS_PROCINFO_INSTANCE	0
/*
* Spec. Version 2.6+
#define SMBIOS_PROCINFO_PACKED {_B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _Q_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _EOT_}

* The SMBIOS version of the ASUS Rampage II Gene is a 2.5
*/
#define SMBIOS_PROCINFO_PACKED {_B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _Q_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _EOT_}
#define SMBIOS_PROCINFO_EXTENS {_W_ _S_ _EOT_}

struct SMBIOS4
{
		unsigned long long
				Socket,
				ProcType,
				Family,
				Manufacturer;
				struct
				{
		unsigned
					EAX	: 32-0,
					EDX	: 64-32;
				} CPUID_0x01;
		unsigned long long
				Version;
				struct
				{
		unsigned
					Tension	: 7-0,
					Mode	: 8-7;
				} Voltage;
		unsigned long long
				Clock,
				MaxSpeed,
				CurrentSpeed,
				Status,
				Upgrade,
				L1_Cache_Handle,
				L2_Cache_Handle,
				L3_Cache_Handle,
				Serial,
				AssetTag,
				PartNumber,
				CoreCount,
				CoreEnabled,
				ThreadCount,
				Characteristics,
				Family2;
};

struct PROCINFO
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS4		*Attrib;
	struct STRING		*String;
};


#define SMBIOS_CACHEINFO_TYPE		7

#define SMBIOS_CACHEINFO_PACKED {_B_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _EOT_}

struct SMBIOS7
{
		unsigned long long
				Socket,
				Configuration,
				Maximum_Size,
				Installed_Size,
				Supported_SRAM,
				Current_SRAM,
				Cache_Speed,
				Error_Correction,
				System_Cache,
				Associativity;
};

struct CACHEINFO
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS7		*Attrib;
	struct STRING		*String;
};


#define SMBIOS_MEMARRAY_TYPE		16
#define SMBIOS_MEMARRAY_INSTANCE	0

#define SMBIOS_MEMARRAY_PACKED {_B_ _S_ _B_ _S_ _B_ _S_ _D_ _S_ _W_ _S_ _W_ _S_ _EOT_}
#define SMBIOS_MEMARRAY_EXTENS {_Q_ _S_ _EOT_}

struct SMBIOS16
{
		unsigned long long
				Location,
				Use,
				Error_Correction,
				Maximum_Capacity,
				Error_Handle,
				Number_Devices,
				Extended_Capacity;
};

struct MEMARRAY
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS16		*Attrib;
	struct STRING		*String;
};


#define SMBIOS_MEMDEV_TYPE		17

#define SMBIOS_MEMDEV_PACKED {_W_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _W_ _S_ _W_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _B_ _S_ _EOT_}
#define SMBIOS_MEMDEV_EXTENS {_B_ _S_ _D_ _S_ _W_ _S_ _EOT_}

struct SMBIOS17
{
		unsigned long long
				MemArray_Handle,
				Error_Handle,
				Total_Width,
				Data_Width,
				Size,
				Form_Factor,
				Set,
				Socket,
				Bank,
				Mem_Type,
				Mem_Detail,
				Speed,
				Manufacturer,
				Serial,
				AssetTag,
				PartNumber,
				Attributes,
				Extended_Size,
				Clock_Speed;
};

struct MEMDEV
{
	struct HEADER		Header;
	unsigned int		Dimension;
	struct SMBIOS17		*Attrib;
	struct STRING		*String;
};


struct SMBIOS_TREE
{
	struct BIOSINFO		*Bios;
	struct BOARDINFO	*Board;
	struct PROCINFO		*Proc;
	struct CACHEINFO	*Cache[3];
	struct MEMARRAY		*MemArray;
	struct MEMDEV		**Memory;
};

struct HAL_TREE
{
	struct FEATURES		*Features;
	struct COREINFO		*Core;
	struct IMCINFO		*Imc;
};


struct MENUNAV
{
	char		Command;
	int		NewLevel;
	int		Function;
};

#define MENU_LABEL_MORE   "  More "
#define MENU_LABEL_ROOT "\n  Root "
#define MENU_LABEL_HELP "\n  Help "
#define MENU_LABEL_SHOW "\n  Show "
#define MENU_LABEL_DUMP "\n  Dump "
#define MENU_LABEL_ATTR "\nAttribs"
#define MENU_LABEL_STRG "\nStrings"
#define MENU_LABEL_CACH "\n Cache "
#define MENU_LABEL_MEMY "\n Memory"
#define MENU_LABEL_CMOD "\n Modify"

#define MENU_PROMPT_MORE "[<]  [RETURN]                                                    "
#define MENU_PROMPT_ROOT "[E]xit    [H]elp       [S]how         [D]ump       [M]odify      "
#define	MENU_PROMPT_HELP "[<]Back                     (C)2012-2015 CyrIng                  "
#define MENU_PROMPT_SHOW "[<]Back   [C]ore       [P]rocessor    [F]eatures   [S]ystem      "
#define MENU_PROMPT_DUMP "[<]Back   [A]ttributes                [S]trings                  "
#define MENU_PROMPT_ATTR "[<]Back   [B]ios       [M]otherboard  [P]rocessor  [C]ache  [R]AM"
#define MENU_PROMPT_STRG "[<]Back   [B]ios       [M]otherboard  [P]rocessor  [C]ache  [R]AM"
#define MENU_PROMPT_CACH "[<]Back   Level L[1]   Level L[2]     Level L[3]                 "
#define MENU_PROMPT_MEMY "[<]Back   [A]rray                                                "
#define MENU_PROMPT_CMOD "[<]Back   [F]requency  [T]imings                                 "

#define LV_MORE  0
#define LV_ROOT  LV_MORE + 1
#define LV_HELP  LV_MORE + 2
#define LV_SHOW  LV_MORE + 3
#define LV_DUMP  LV_MORE + 4
#define LV_ATTR  LV_MORE + 5
#define LV_STRG  LV_MORE + 6
#define LV_ATTH  LV_MORE + 7
#define LV_STRH  LV_MORE + 8
#define LV_ATTY  LV_MORE + 9
#define LV_STRY  LV_MORE + 10
#define LV_CMOD  LV_MORE + 11

#define LEVELS   LV_MORE + 12

#define FN_BACK  -3
#define FN_MORE  -2
#define FN_NOOP  -1
#define FN_EXIT   0
#define FN_HELP  (LV_HELP * 10) + 1
#define FN_SHWC  (LV_SHOW * 10) + 1
#define FN_SHWP  (LV_SHOW * 10) + 2
#define FN_SHWF  (LV_SHOW * 10) + 3
#define FN_SHWS  (LV_SHOW * 10) + 4
#define FN_ATTB  (LV_ATTR * 10) + 1
#define FN_ATTM  (LV_ATTR * 10) + 2
#define FN_ATTP  (LV_ATTR * 10) + 3
#define FN_STRB  (LV_STRG * 10) + 1
#define FN_STRM  (LV_STRG * 10) + 2
#define FN_STRP  (LV_STRG * 10) + 3
#define FN_ATL1  (LV_ATTH * 10) + 1
#define FN_ATL2  (LV_ATTH * 10) + 2
#define FN_ATL3  (LV_ATTH * 10) + 3
#define FN_STL1  (LV_STRH * 10) + 1
#define FN_STL2  (LV_STRH * 10) + 2
#define FN_STL3  (LV_STRH * 10) + 3
#define FN_ATTY  (LV_ATTY * 10) + 1
#define FN_ATTD  (LV_ATTY * 100)
#define FN_STRY  (LV_STRY * 10) + 1
#define FN_STRD  (LV_STRY * 100)
#define FN_CMDF  (LV_CMOD * 10) + 1
#define FN_CMDT  (LV_CMOD * 10) + 2
#define FN_CMOD  (LV_CMOD * 10) + 5

#define EOL       0

#define MENU_LEVEL_MORE {{'e', LV_ROOT, FN_EXIT}, {'<', LV_MORE, FN_BACK}, {EOL, LV_MORE, FN_MORE}}
#define MENU_LEVEL_ROOT {{'e', LV_ROOT, FN_EXIT}, {'h', LV_HELP, FN_HELP}, {'s', LV_SHOW, FN_NOOP}, {'d', LV_DUMP, FN_NOOP}, {'m', LV_CMOD, FN_NOOP}, {EOL, LV_ROOT, FN_NOOP}}
#define MENU_LEVEL_HELP {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ROOT, FN_NOOP}, {EOL, LV_HELP, FN_HELP}}
#define MENU_LEVEL_SHOW {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ROOT, FN_NOOP}, {'c', LV_SHOW, FN_SHWC}, {'p', LV_SHOW, FN_SHWP}, {'f', LV_SHOW, FN_SHWF}, {'s', LV_SHOW, FN_SHWS}, {EOL, LV_SHOW, FN_NOOP}}
#define MENU_LEVEL_DUMP {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ROOT, FN_NOOP}, {'a', LV_ATTR, FN_NOOP}, {'s', LV_STRG, FN_NOOP}, {EOL, LV_DUMP, FN_NOOP}}
#define MENU_LEVEL_ATTR {{'e', LV_ROOT, FN_EXIT}, {'<', LV_DUMP, FN_NOOP}, {'b', LV_ATTR, FN_ATTB}, {'m', LV_ATTR, FN_ATTM}, {'p', LV_ATTR, FN_ATTP}, {'c', LV_ATTH, FN_NOOP}, {'r', LV_ATTY, FN_NOOP}, {EOL, LV_ATTR, FN_NOOP}}
#define MENU_LEVEL_STRG {{'e', LV_ROOT, FN_EXIT}, {'<', LV_DUMP, FN_NOOP}, {'b', LV_STRG, FN_STRB}, {'m', LV_STRG, FN_STRM}, {'p', LV_STRG, FN_STRP}, {'c', LV_STRH, FN_NOOP}, {'r', LV_STRY, FN_NOOP}, {EOL, LV_STRG, FN_NOOP}}
#define MENU_LEVEL_ATTH {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ATTR, FN_NOOP}, {'1', LV_ATTH, FN_ATL1}, {'2', LV_ATTH, FN_ATL2}, {'3', LV_ATTH, FN_ATL3}, {EOL, LV_ATTH, FN_NOOP}}
#define MENU_LEVEL_STRH {{'e', LV_ROOT, FN_EXIT}, {'<', LV_STRG, FN_NOOP}, {'1', LV_STRH, FN_STL1}, {'2', LV_STRH, FN_STL2}, {'3', LV_STRH, FN_STL3}, {EOL, LV_STRH, FN_NOOP}}
#define MENU_LEVEL_ATTY {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ATTR, FN_NOOP}, {'a', LV_ATTY, FN_ATTY}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}, {EOL, LV_ATTY, FN_NOOP}}
#define MENU_LEVEL_STRY {{'e', LV_ROOT, FN_EXIT}, {'<', LV_STRG, FN_NOOP}, {'a', LV_STRY, FN_STRY}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}, {EOL, LV_STRY, FN_NOOP}}
#define MENU_LEVEL_CMOD {{'e', LV_ROOT, FN_EXIT}, {'<', LV_ROOT, FN_NOOP}, {'f', LV_CMOD, FN_CMDF}, {'t', LV_CMOD, FN_CMDT}, {'m', LV_CMOD, FN_CMOD}, {EOL, LV_CMOD, FN_NOOP}}

#define MENU_LABEL_SIZE  10
#define MENU_PROMPT_SIZE 70
#define CONSOLE_WIDTH    80
#define CONSOLE_HEIGHT   23
#define CONSOLE_PAGES     4

#define cflush()	{ bzero(bConsole, CONSOLE_HEIGHT*CONSOLE_PAGES*CONSOLE_WIDTH); eConsole=0; }
#define cprintf(...)	{ sprintf(bConsole[eConsole++], __VA_ARGS__); }