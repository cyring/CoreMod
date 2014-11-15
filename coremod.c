/*
 * CoreMod by CyrIng
 * 
 * Copyright (C) 2012 CYRIL INGENIERIE
 * 
 * Licenses: GPL2
 * 
 * coremod.c release 2012-0.1.4
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "coremod.h"

char  bConsole[CONSOLE_HEIGHT*CONSOLE_PAGES][CONSOLE_WIDTH]={{0}};
int   eConsole=0, lConsole=0;

const char menuLabel[LEVELS][MENU_LABEL_SIZE]=
{
	MENU_LABEL_MORE,
	MENU_LABEL_ROOT,
	MENU_LABEL_HELP,
	MENU_LABEL_SHOW,
	MENU_LABEL_DUMP,
	MENU_LABEL_ATTR,
	MENU_LABEL_STRG,
	MENU_LABEL_CACH,
	MENU_LABEL_CACH,
	MENU_LABEL_MEMY,
	MENU_LABEL_MEMY,
	MENU_LABEL_CMOD
};

char menuPrompt[LEVELS][MENU_PROMPT_SIZE]=
{
	MENU_PROMPT_MORE,
	MENU_PROMPT_ROOT,
	MENU_PROMPT_HELP,
	MENU_PROMPT_SHOW,
	MENU_PROMPT_DUMP,
	MENU_PROMPT_ATTR,
	MENU_PROMPT_STRG,
	MENU_PROMPT_CACH,
	MENU_PROMPT_CACH,
	MENU_PROMPT_MEMY,
	MENU_PROMPT_MEMY,
	MENU_PROMPT_CMOD
};

struct MENUNAV \
	levelMore[]=MENU_LEVEL_MORE, \
	levelRoot[]=MENU_LEVEL_ROOT, \
	levelHelp[]=MENU_LEVEL_HELP, \
	levelShow[]=MENU_LEVEL_SHOW, \
	levelDump[]=MENU_LEVEL_DUMP, \
	levelAttr[]=MENU_LEVEL_ATTR, \
	levelStrg[]=MENU_LEVEL_STRG, \
	levelAttH[]=MENU_LEVEL_ATTH, \
	levelStrH[]=MENU_LEVEL_STRH, \
	levelAttY[]=MENU_LEVEL_ATTY, \
	levelStrY[]=MENU_LEVEL_STRY, \
	levelCMod[]=MENU_LEVEL_CMOD;

struct MENUNAV *menuNav[LEVELS]=
{
	levelMore,
	levelRoot,
	levelHelp,
	levelShow,
	levelDump,
	levelAttr,
	levelStrg,
	levelAttH,
	levelStrH,
	levelAttY,
	levelStrY,
	levelCMod
};

int Menu_GetKey(int prompt)
{
	int key=tolower(getchar());

	switch(prompt)
	{
		case 1:
			if(key > 0x20 && key < 0x7f)
				printf("%c", key);
		break;
		case 2:
		{
			int flush=key;

			if(key > 0x20 && key < 0x7f)
				printf("%c", key);
			while(flush != '\n')
				flush=getchar();
		}
		break;
	}
	return(key);
}

int Menu_Function(int *menuLevel)
{
	int  ix=0, function=0, command=Menu_GetKey(1);

	while( command != menuNav[*menuLevel][ix].Command \
		&& menuNav[*menuLevel][ix].Command != EOL )
			ix++ ;
	function=menuNav[*menuLevel][ix].Function;
	*menuLevel=menuNav[*menuLevel][ix].NewLevel;

	return(function);
}

int Menu_Paging(int *menuLevel)
{
	if(*menuLevel != LV_MORE)
		lConsole=0;
	else
		printf("\n");
	do
		printf("%s", bConsole[lConsole++]);
	while((lConsole < eConsole) && (lConsole % CONSOLE_HEIGHT));

	return(!(lConsole % CONSOLE_HEIGHT));
}

int Menu_Display(int *menuLevel)
{
	int more=Menu_Paging(menuLevel), function=FN_NOOP;

	if(more)
	{
		if(*menuLevel != LV_MORE)
		{
			menuNav[LV_MORE][1].NewLevel=*menuLevel;
			*menuLevel=LV_MORE;
		}
	}
	else
	{
		if(*menuLevel == LV_MORE)
			*menuLevel=menuNav[LV_MORE][1].NewLevel;
		cflush();
	}

	printf("%7s | %66s :", menuLabel[*menuLevel], menuPrompt[*menuLevel]);

	function=Menu_Function(menuLevel);

	return(function);
}

static __inline__ unsigned long long rdtsc (void)
{
	unsigned hi, lo;
	
	__asm__ volatile
	(
		"rdtsc;"
		:"=a" (lo),
		 "=d" (hi)
	);
	return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
}


unsigned long long int Frequency()
{
	struct timezone tz;
	struct timeval tvstart, tvstop;
	unsigned long long int cycles[2]; // gotta be 64 bit
	unsigned int microseconds; // total time taken

	memset(&tz, 0, sizeof(tz));

	// get this function in cached memory
	gettimeofday(&tvstart, &tz);
	cycles[0] = rdtsc();
	gettimeofday(&tvstart, &tz);

	usleep(1000000);

	cycles[1] = rdtsc();
	gettimeofday(&tvstop, &tz);
	microseconds = ( (tvstop.tv_sec - tvstart.tv_sec) * 1000000) + (tvstop.tv_usec - tvstart.tv_usec);

	return( (cycles[1] - cycles[0]) / microseconds );
}

struct STRING *SMB_Dig_Strings(struct PACKED *packed, int ID)
{
	char  *buffer=NULL;
	size_t rbyte=0;

	if( getdelim(&buffer, &rbyte, STRING_DELIMITER, packed->File) > 1 )
	{
		struct STRING *pstr=malloc(sizeof(struct STRING));
		pstr->Buffer=buffer;
		pstr->ID=++ID;
		pstr->Link=SMB_Dig_Strings(packed, ID);
		return(pstr);
	}
	else
		return(NULL);
}

struct STRING *SMB_Read_Strings(struct PACKED *packed)
{
	return(packed->Length==0 ? SMB_Dig_Strings(packed, 0) : NULL);
}

void SMB_Dump_Strings(struct STRUCTINFO *smb)
{
	if(smb!=NULL)
	{
		struct STRING *pstr=smb->String;

		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("String#\t- Buffer -\n");

		while(pstr!=NULL)
		{
			cprintf("%3d\t[%s]\n", pstr->ID, pstr->Buffer);
			pstr=pstr->Link;
		};
	}
}

char *SMB_Find_String(struct STRUCTINFO *smb, int ID)
{
	struct STRING *pstr=NULL;

	if(smb!=NULL)
	{
		pstr=smb->String;
		while(pstr!=NULL && pstr->ID!=ID)
			pstr=pstr->Link;
	}
	return(pstr!=NULL ? pstr->Buffer : "");
}

int SMB_Read_Length(struct PACKED *packed)
{
	char    pathName[]="/sys/firmware/dmi/entries/999-99/file1234567890";

	sprintf(pathName, "/sys/firmware/dmi/entries/%d-%d/length", packed->Type, packed->Instance);
	if( (packed->File=fopen(pathName, "r")) != NULL )
	{
		fscanf(packed->File, "%zd\n", &packed->Length);
		fclose(packed->File);
		packed->Length-=sizeof(struct HEADER);
		return(0);
	}
	else
		return(errno);
}

int SMB_Open_Structure(struct PACKED *packed)
{
	char    pathName[]="/sys/firmware/dmi/entries/999-99/file1234567890";

	sprintf(pathName, "/sys/firmware/dmi/entries/%d-%d/raw", packed->Type, packed->Instance);
	if( (packed->File=fopen(pathName, "rb")) == NULL )
		return(errno);
	else
		return(0);
}

int SMB_Close_Structure(struct PACKED *packed)
{
	if(packed->File)
	{
		fclose(packed->File);
		packed->File=NULL;
	}
	return(errno);
}

struct STRUCTINFO *SMB_Read_Structure(struct PACKED *packed)
{
	struct STRUCTINFO *smb=NULL;

	if( (smb=calloc(1, sizeof(struct STRUCTINFO))) !=NULL )
	{
		unsigned int head=0;

		fread(&smb->Header, sizeof(struct HEADER), 1, packed->File);

		while(packed->Tape[head] != _EOT_)
		{
			unsigned long long poly=0;

			smb->Attrib=realloc(smb->Attrib, (head + 1) * sizeof(unsigned long long));
			fread(&poly, 0b0001 << packed->Tape[head], 1, packed->File);
			smb->Attrib[head]=poly;
			packed->Length-=0b0001 << packed->Tape[head];
			poly=0;
			head++ ;
		}
		smb->Dimension=head;
	}
	return(smb);
}

void SMB_Read_Extension(struct PACKED *packed, struct STRUCTINFO *smb)
{
	unsigned int head=0;

	while(smb!=NULL && packed->Tape[head]!=_EOT_)
	{
		unsigned long long poly=0;

		smb->Attrib=realloc(smb->Attrib, (smb->Dimension + head + 1) * sizeof(unsigned long long));
		fread(&poly, 0b0001 << packed->Tape[head], 1, packed->File);
		smb->Attrib[smb->Dimension + head]=poly;
		packed->Length-=0b0001 << packed->Tape[head];
		poly=0;
		head++ ;
	}
	smb->Dimension+=head;
}

void BIOS_Free_Structure(struct STRUCTINFO *smb)
{
	if(smb!=NULL)
	{
		struct STRING *Link=NULL, *pstr=NULL;

		pstr=smb->String;
		while(pstr!=NULL)
		{
			Link=pstr->Link;
			free(pstr);
			pstr=Link;
		};
		smb->String=NULL;
		if(smb->Attrib)
		{
			free(smb->Attrib);
			smb->Attrib=NULL;
		}
		free(smb);
		smb=NULL;
	}
}

void SMB_Dump_Attribs(struct STRUCTINFO *smb)
{
	if(smb!=NULL)
	{
		int ix=0;

		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("Attribute#\tHex\t\t\tDec\n");
		for(ix=0; ix < smb->Dimension; ix++)
			cprintf("%3d\t\t0x%-16llX\t( %20llu )\n", \
				ix, smb->Attrib[ix], smb->Attrib[ix]);
	}
}

struct BIOSINFO *BIOS_Read_Info(void)
{
	const int         tape[]=SMBIOS_BIOSINFO_PACKED;
	struct PACKED     packed={SMBIOS_BIOSINFO_TYPE, SMBIOS_BIOSINFO_INSTANCE, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct BIOSINFO*) smb);
}

struct BOARDINFO *BOARD_Read_Info(void)
{
	const int         tape[]=SMBIOS_BOARDINFO_PACKED,
	                  extens[]=SMBIOS_BOARDINFO_EXTENS;
	struct PACKED     packed={SMBIOS_BOARDINFO_TYPE, SMBIOS_BOARDINFO_INSTANCE, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		if(smb->Attrib[9] > 0)
		{
			packed.Tape=&extens[0];
			while(packed.Length > 0)
				SMB_Read_Extension(&packed, smb);
		}
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct BOARDINFO*) smb);
}

struct PROCINFO *PROC_Read_Info(void)
{
	const int         tape[]=SMBIOS_PROCINFO_PACKED,
	                  extens[]=SMBIOS_PROCINFO_EXTENS;
	struct PACKED     packed={SMBIOS_PROCINFO_TYPE, SMBIOS_PROCINFO_INSTANCE, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		if(packed.Length > 0)
		{
			packed.Tape=&extens[0];
			SMB_Read_Extension(&packed, smb);
		}
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct PROCINFO*) smb);
}

struct CACHEINFO *CACHE_Read_Info(int instance)
{
	const int         tape[]=SMBIOS_CACHEINFO_PACKED;
	struct PACKED     packed={SMBIOS_CACHEINFO_TYPE, instance, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct CACHEINFO*) smb);
}

struct MEMARRAY *MEM_Read_Array(void)
{
	const int         tape[]=SMBIOS_MEMARRAY_PACKED,
	                  extens[]=SMBIOS_MEMARRAY_EXTENS;
	struct PACKED     packed={SMBIOS_MEMARRAY_TYPE, SMBIOS_MEMARRAY_INSTANCE, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		if(packed.Length > 0)
		{
			packed.Tape=&extens[0];
			SMB_Read_Extension(&packed, smb);
		}
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct MEMARRAY*) smb);
}

struct MEMDEV *MEM_Read_Device(int instance)
{
	const int         tape[]=SMBIOS_MEMDEV_PACKED,
	                  extens[]=SMBIOS_MEMDEV_EXTENS;
	struct PACKED     packed={SMBIOS_MEMDEV_TYPE, instance, NULL, 0, &tape[0]};
	struct STRUCTINFO *smb=NULL;

	if(!SMB_Read_Length(&packed) && !SMB_Open_Structure(&packed))
	{
		smb=SMB_Read_Structure(&packed);
		if(packed.Length > 0)
		{
			packed.Tape=&extens[0];
			SMB_Read_Extension(&packed, smb);
		}
		smb->String=SMB_Read_Strings(&packed);
		SMB_Close_Structure(&packed);
	}
	return((struct MEMDEV*) smb);
}

struct MEMDEV **MEM_ReadAll_Devices(struct MEMARRAY *memArray)
{
	struct MEMDEV **memory=NULL;

	if(memArray!=NULL)
	{
		int stick=0;

		memory=malloc(sizeof(struct MEMDEV*) * memArray->Attrib->Number_Devices);
		for(stick=0; stick < memArray->Attrib->Number_Devices; stick++)
			memory[stick]=MEM_Read_Device(stick);
	}
	return(memory);
}

void MEM_FreeAll_Devices(struct MEMDEV **memory, struct MEMARRAY *memArray)
{
	if(memArray!=NULL)
	{
		int stick=0;

		for(stick=0; stick < memArray->Attrib->Number_Devices; stick++)
			BIOS_Free_Structure((struct STRUCTINFO*) memory[stick]);

		free(memory);
		memory=NULL;
	}
}

struct COREINFO *CORE_Read_Info(int threadCount)
{
	struct	COREINFO *core=NULL;

	if(threadCount)
	{
		core=malloc(sizeof(struct COREINFO) * threadCount);
		int cpu=0;
		for(cpu=0; core && cpu < threadCount; cpu++)
		{
			char    pathName[]="/dev/cpu/99/msr";
			sprintf(pathName, "/dev/cpu/%d/msr", cpu);
			int File=-1;
			if((File=open(pathName, O_RDONLY)) != -1)
			{
				ssize_t rbyte=0;
				rbyte=pread(File, &core[cpu].Platform, 8, MSR_PLATFORM_INFO);
				rbyte=pread(File, &core[cpu].aperf[0], 8, IA32_APERF);
				rbyte=pread(File, &core[cpu].mperf[0], 8, IA32_MPERF);
				rbyte=pread(File, &core[cpu].Perf, 8, IA32_PERF_STATUS);
				rbyte=pread(File, &core[cpu].Ctl, 8, IA32_PERF_CTL);
				rbyte=pread(File, &core[cpu].ClockMod, 8, IA32_CLOCK_MODULATION);
				rbyte=pread(File, &core[cpu].Therm, 8, IA32_THERM_STATUS);
				rbyte=pread(File, &core[cpu].Misc, 8, IA32_MISC_ENABLE);
				rbyte=pread(File, &core[cpu].Temp, 8, MSR_TEMPERATURE_TARGET);
				rbyte=pread(File, &core[cpu].Turbo, 8, MSR_TURBO_RATIO_LIMIT);
				close(File);
			}
		}
	}
	return(core);
}

void CORE_Update_Info(struct COREINFO *core, int threadCount)
{
	if(core!=NULL && threadCount)
	{
		int cpu=0;
		for(cpu=0; cpu < threadCount; cpu++)
		{
			char    pathName[]="/dev/cpu/99/msr";
			sprintf(pathName, "/dev/cpu/%d/msr", cpu);
			int File=-1;
			if((File=open(pathName, O_RDONLY)) != -1)
			{
				ssize_t rbyte=0;
				rbyte=pread(File, &core[cpu].aperf[1], 8, IA32_APERF);
				rbyte=pread(File, &core[cpu].mperf[1], 8, IA32_MPERF);
				rbyte=pread(File, &core[cpu].Perf, 8, IA32_PERF_STATUS);
				rbyte=pread(File, &core[cpu].Ctl, 8, IA32_PERF_CTL);
				rbyte=pread(File, &core[cpu].Therm, 8, IA32_THERM_STATUS);
				rbyte=pread(File, &core[cpu].Misc, 8, IA32_MISC_ENABLE);
				close(File);
			}
		}
	}
}

void CORE_Mod(struct COREINFO *core, int threadCount)
{
	if(core!=NULL && threadCount)
	{
		int cpu=0;
		for(cpu=0; cpu < threadCount; cpu++)
		{
			char    pathName[]="/dev/cpu/99/msr";
			sprintf(pathName, "/dev/cpu/%d/msr", cpu);
			int File=-1;
			if((File=open(pathName, O_RDWR)) != -1)
			{
				ssize_t rbyte=0;
				core[cpu].Misc.EIST=!core[cpu].Misc.EIST;
				core[cpu].Misc.NOT_TurboMode=!core[cpu].Misc.NOT_TurboMode;
				rbyte=pwrite(File, &core[cpu].Misc, 8, IA32_MISC_ENABLE);
				close(File);
			}
		}
	}
}

void CORE_Free_Info(struct COREINFO *core)
{
	if(core)
	{
		free(core);
		core=NULL;
	}
}

struct FEATURES *FEATURES_Read_Info()
{
	struct FEATURES *features=malloc(sizeof(struct FEATURES));
#ifdef __x86_64__
	__asm__ volatile
	(
		"movq	$0x1, %%rax;"
		"cpuid;"
		: "=a"	(features->Std.EAX),
		  "=b"	(features->Std.EBX),
		  "=c"	(features->Std.ECX),
		  "=d"	(features->Std.EDX)
	);
	__asm__ volatile
	(
		"movq	$0x4, %%rax;"
		"xorq	%%rcx, %%rcx;"
		"cpuid;"
		"shr	$26, %%rax;"
		"and	$0x3f, %%rax;"
		"add	$1, %%rax;"
		: "=a"	(features->ThreadCount)
	);
	__asm__ volatile
	(
		"movq	$0x80000000, %%rax;"
		"cpuid;"
		: "=a"	(features->LargestExtFunc)
	);
	if(features->LargestExtFunc >= 0x80000004 && features->LargestExtFunc <= 0x80000008)
	{
		__asm__ volatile
		(
			"movq	$0x80000001, %%rax;"
			"cpuid;"
			: "=c"	(features->Ext.ECX),
			  "=d"	(features->Ext.EDX)
		);

		struct
		{
			struct
			{
			unsigned char Chr[4];
			} EAX,
			EBX,
			ECX,
			EDX;
		} Brand;
		int ix=0, jx=0, px=0;
		for(ix=0; ix<3; ix++)
		{
			__asm__ volatile
			(
				"cpuid;"
				: "=a"	(Brand.EAX),
				  "=b"	(Brand.EBX),
				  "=c"	(Brand.ECX),
				  "=d"	(Brand.EDX)
				: "a"	(0x80000002 + ix)
			);
				for(jx=0; jx<4; jx++, px++)
					features->BrandString[px]=Brand.EAX.Chr[jx];
				for(jx=0; jx<4; jx++, px++)
					features->BrandString[px]=Brand.EBX.Chr[jx];
				for(jx=0; jx<4; jx++, px++)
					features->BrandString[px]=Brand.ECX.Chr[jx];
				for(jx=0; jx<4; jx++, px++)
					features->BrandString[px]=Brand.EDX.Chr[jx];
		}
	}
#endif
	return(features);
}

void FEATURES_Free_Info(struct FEATURES *features)
{
	if(features)
	{
		free(features);
		features=NULL;
	}
}

struct IMCINFO *IMC_Read_Info()
{
	struct	IMCINFO *imc=calloc(1, sizeof(struct IMCINFO));

	if(!iopl(3))
	{
		unsigned bus=0xff, dev=0x4, func=0;
		outl(PCI_CONFIG_ADDRESS(bus, 3, 0, 0x48), 0xcf8);
		int code=(inw(0xcfc + (0x48 & 2)) >> 8) & 0x7;
		imc->ChannelCount=(code == 7 ? 3 : code == 4 ? 1 : code == 2 ? 1 : code == 1 ? 1 : 2);
		imc->Channel=calloc(imc->ChannelCount, sizeof(struct CHANNEL));

		unsigned cha=0;
		for(cha=0; cha < imc->ChannelCount; cha++)
		{
			unsigned int MRs=0, RANK_TIMING_B=0, BANK_TIMING=0, REFRESH_TIMING=0;

			outl(PCI_CONFIG_ADDRESS(0xff, (dev + cha), func, 0x70), 0xcf8);
			MRs=inl(0xcfc);
			outl(PCI_CONFIG_ADDRESS(0xff, (dev + cha), func, 0x84), 0xcf8);
			RANK_TIMING_B=inl(0xcfc);
			outl(PCI_CONFIG_ADDRESS(0xff, (dev + cha), func, 0x88), 0xcf8);
			BANK_TIMING=inl(0xcfc);
			outl(PCI_CONFIG_ADDRESS(0xff, (dev + cha), func, 0x8c), 0xcf8);
			REFRESH_TIMING=inl(0xcfc);

			imc->Channel[cha].Timing.tCL  =((MRs >> 4) & 0x7) != 0 ? ((MRs >> 4) & 0x7) + 4 : 0;
			imc->Channel[cha].Timing.tRCD =(BANK_TIMING & 0x1E00) >> 9;
			imc->Channel[cha].Timing.tRP  =(BANK_TIMING & 0xF);
			imc->Channel[cha].Timing.tRAS =(BANK_TIMING & 0x1F0) >> 4;
			imc->Channel[cha].Timing.tRRD =(RANK_TIMING_B & 0x1c0) >> 6;
			imc->Channel[cha].Timing.tRFC =(REFRESH_TIMING & 0x1ff);
			imc->Channel[cha].Timing.tWR  =((MRs >> 9) & 0x7) != 0 ? ((MRs >> 9) & 0x7) + 4 : 0;
			imc->Channel[cha].Timing.tRTPr=(BANK_TIMING & 0x1E000) >> 13;
			imc->Channel[cha].Timing.tWTPr=(BANK_TIMING & 0x3E0000) >> 17;
			imc->Channel[cha].Timing.tFAW =(RANK_TIMING_B & 0x3f);
			imc->Channel[cha].Timing.B2B  =(RANK_TIMING_B & 0x1f0000) >> 16;
		}
		iopl(0);
	}
	return(imc);
}

void IMC_Free_Info(struct IMCINFO *imc)
{
	if(imc!=NULL)
	{
		if(imc->Channel!=NULL)
			free(imc->Channel);
		free(imc);
		imc = NULL;
	}
}


void IMC_Show_Info(struct IMCINFO *imc)
{
	if(imc!=NULL)
	{
		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("Channel   tCL   tRCD  tRP   tRAS  tRRD  tRFC  tWR   tRTPr tWTPr tFAW  B2B\n");

		unsigned cha=0;
		for(cha=0; cha < imc->ChannelCount; cha++)
			cprintf("   #%1i   |%4d%6d%6d%6d%6d%6d%6d%6d%6d%6d%6d\n", \
				cha, \
				imc->Channel[cha].Timing.tCL, \
				imc->Channel[cha].Timing.tRCD, \
				imc->Channel[cha].Timing.tRP, \
				imc->Channel[cha].Timing.tRAS, \
				imc->Channel[cha].Timing.tRRD, \
				imc->Channel[cha].Timing.tRFC, \
				imc->Channel[cha].Timing.tWR, \
				imc->Channel[cha].Timing.tRTPr, \
				imc->Channel[cha].Timing.tWTPr, \
				imc->Channel[cha].Timing.tFAW, \
				imc->Channel[cha].Timing.B2B);
	}
}

void CORE_Show_Temp(struct COREINFO *core, struct PROCINFO *proc, int threadCount)
{
	if(core!=NULL && threadCount)
	{
		unsigned long long bClk=(proc!=NULL) ? \
					proc->Attrib->Clock \
					: Frequency()/core[0].Platform.MaxNonTurboRatio;
		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("\tCore#\tRatio\tx Clock\t= Frequency\tTJMAX\t- DTS\t= Temperature\n");

		int cpu=0;
		for(cpu=0; cpu < threadCount; cpu++)
		{
			cprintf("\t%2d\t %2d\tx  %3lld\t= %4lld MHz\t %3d\t- %3d\t=  %3d°C\n", \
				cpu, \
				core[cpu].Perf.Ratio, \
				bClk, \
				core[cpu].Perf.Ratio * bClk, \
				core[cpu].Temp.Target, \
				core[cpu].Therm.DTS, \
				core[cpu].Temp.Target - core[cpu].Therm.DTS);
		}
	}
}

void CORE_Show_Info(struct COREINFO *core, struct PROCINFO *proc, int threadCount)
{
	if(core!=NULL && threadCount)
	{
		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("Core\tPlatform\t--- Perf ---\tTarget\t-- Turbo --\tSpeed  Turbo\n");
		cprintf(" #\tMin  Max\tA/M\tRatio\tRatio\t1C 2C 3C 4C\t-step  -mode\n");

		int cpu=0;
		for(cpu=0; cpu < threadCount; cpu++)
		{
			unsigned long long	d_aperf=core[cpu].aperf[1]-core[cpu].aperf[0], \
						d_mperf=core[cpu].mperf[1]-core[cpu].mperf[0];

			cprintf("%2d\t%3d  %3d\t%3d%%\t%3d\t%3d\t%2d %2d %2d %2d\t  %c     %c\n", \
				cpu, \
				core[cpu].Platform.MinimumRatio, \
				core[cpu].Platform.MaxNonTurboRatio, \
				(int) (100 * (float) d_aperf/d_mperf), \
				core[cpu].Perf.Ratio, \
				core[cpu].Ctl.Target, \
				core[cpu].Turbo.MaxRatio_1C, \
				core[cpu].Turbo.MaxRatio_2C, \
				core[cpu].Turbo.MaxRatio_3C, \
				core[cpu].Turbo.MaxRatio_4C, \
				core[cpu].Misc.EIST == 1 ? 'Y' : 'N', \
				core[cpu].Misc.NOT_TurboMode == 1 ? 'N' : 'Y');

			core[cpu].aperf[0]=core[cpu].aperf[1];
			core[cpu].mperf[0]=core[cpu].mperf[1];
		}
	}
}

void PROC_Show_Info(struct PROCINFO *proc, struct CACHEINFO *cache[])
{
	if(proc!=NULL && cache[0]!=NULL && cache[1]!=NULL && cache[2]!=NULL)
	{
		const float tension[0B1000]={0.0f, 5.0f, 3.3f, 0.0f, 2.9f, 0.0f, 0.0f, 0.0f};
		long deltaClock=proc->Attrib->CurrentSpeed - proc->Attrib->MaxSpeed;

		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("\t%s\n", SMB_Find_String((struct STRUCTINFO*) proc, proc->Attrib->Version));
		cprintf("\tBase clock  @ %lld MHz\n", proc->Attrib->Clock);
		cprintf("\t%sclocked @ %lld MHz\n", \
			deltaClock > 20 ? "Over" : deltaClock < -20 ? "Under" : "Factory ", \
			proc->Attrib->CurrentSpeed);
		cprintf("\t%lld/%lld cores enabled, %lld threads detected.\n", \
			proc->Attrib->CoreEnabled, \
			proc->Attrib->CoreCount, \
			proc->Attrib->ThreadCount);
		cprintf("\tSocket : %s\n", \
			SMB_Find_String((struct STRUCTINFO*) proc, proc->Attrib->Socket));
		cprintf("\tTension: %.1f V\n\t",
			proc->Attrib->Voltage.Mode ? proc->Attrib->Voltage.Tension / 10.0f \
						   : tension[proc->Attrib->Voltage.Tension & 0B0111]);

		int ix=0;
		for(ix=0; ix < 3; ix++)
			cprintf("%s [%4lld]%s", \
			SMB_Find_String((struct STRUCTINFO*) cache[ix], cache[ix]->Attrib->Socket), \
			cache[ix]->Attrib->Installed_Size, ix < 2 ? "     " : "");
		cprintf("\n");
	}
}

void FEATURES_Show_Info(struct FEATURES *features)
{
	if(features!=NULL)
	{
		const char  powered[2]={'N', 'Y'};

		cflush();
		if(features->LargestExtFunc >= 0x80000004 && features->LargestExtFunc <= 0x80000008)
		{
			cprintf("\n");
			cprintf("\n");
			cprintf("\tBrand [%s]", features->BrandString);
		}
		else
		{
			cprintf("\n");
			cprintf("\n");
			cprintf("\tProcessor Type [0x%-4x]  Brand ID [0x%-4x]", \
				features->Std.EAX.ProcType, \
				features->Std.EBX.Brand_ID);
		}
		cprintf("\n");
		cprintf("\n");
		cprintf("\t Family      Model    Stepping     Max# of\n");
		cprintf("\t  Code        No.        ID        Threads\n");
		cprintf("\t[%6d]   [%6d]   [%6d]   [%6d]\n\n", \
			features->Std.EAX.ExtFamily + features->Std.EAX.Family, \
			(features->Std.EAX.ExtModel << 4) + features->Std.EAX.Model, \
			features->Std.EAX.Stepping, \
			features->ThreadCount \
			);
		cprintf("\tVirtual Mode Extension                                    VME [%c]\n", \
			powered[features->Std.EDX.VME]);
		cprintf("\tDebugging Extension                                        DE [%c]\n", \
			powered[features->Std.EDX.DE]);
		cprintf("\tPage Size Extension                                       PSE [%c]\n", \
			powered[features->Std.EDX.PSE]);
		cprintf("\tTime Stamp Counter                                        TSC [%c]\n", \
			powered[features->Std.EDX.TSC]);
		cprintf("\tModel Specific Registers                                  MSR [%c]\n", \
			powered[features->Std.EDX.MSR]);
		cprintf("\tPhysical Address Extension                                PAE [%c]\n", \
			powered[features->Std.EDX.PAE]);
		cprintf("\tAdvanced Programmable Interrupt Controller               APIC [%c]\n", \
			powered[features->Std.EDX.APIC]);
		cprintf("\tMemory Type Range Registers                              MTRR [%c]\n", \
			powered[features->Std.EDX.MTRR]);
		cprintf("\tPage Global Enable                                        PGE [%c]\n", \
			powered[features->Std.EDX.PGE]);
		cprintf("\tMachine-Check Architecture                                MCA [%c]\n", \
			powered[features->Std.EDX.MCA]);
		cprintf("\tPage Attribute Table                                      PAT [%c]\n", \
			powered[features->Std.EDX.PAT]);
		cprintf("\t36-bit Page Size Extension                              PSE36 [%c]\n", \
			powered[features->Std.EDX.PSE36]);
		cprintf("\tProcessor Serial Number                                   PSN [%c]\n", \
			powered[features->Std.EDX.PSN]);
		cprintf("\tDebug Store                                                DS [%c]\n", \
			powered[features->Std.EDX.DS]);
		cprintf("\tAdvanced Configuration and Power Interface               ACPI [%c]\n", \
			powered[features->Std.EDX.ACPI]);
		cprintf("\tSelf-Snoop                                                 SS [%c]\n", \
			powered[features->Std.EDX.SS]);
		cprintf("\tHyper-Threading                                           HTT [%c]\n", \
			powered[features->Std.EDX.HTT]);
		cprintf("\tThermal Monitor                                  TM1 [%c]  TM2 [%c]\n", \
			powered[features->Std.EDX.TM1], \
			powered[features->Std.ECX.TM2] \
			);
		cprintf("\tPending Break Enable                                      PBE [%c]\n", \
			powered[features->Std.EDX.PBE]);
		cprintf("\t64-Bit Debug Store                                     DTES64 [%c]\n", \
			powered[features->Std.ECX.DTES64]);
		cprintf("\tCPL Qualified Debug Store                              DS-CPL [%c]\n", \
			powered[features->Std.ECX.DS_CPL]);
		cprintf("\tVirtual Machine Extensions                                VMX [%c]\n", \
			powered[features->Std.ECX.VMX]);
		cprintf("\tSafer Mode Extensions                                     SMX [%c]\n", \
			powered[features->Std.ECX.SMX]);
		cprintf("\tSpeedStep                                                EIST [%c]\n", \
			powered[features->Std.ECX.EIST]);
		cprintf("\tL1 Data Cache Context ID                              CNXT-ID [%c]\n",
			powered[features->Std.ECX.CNXT_ID]);
		cprintf("\tFused Multiply Add                                        FMA [%c]\n", \
			powered[features->Std.ECX.FMA]);
		cprintf("\txTPR Update Control                                      xTPR [%c]\n", \
			powered[features->Std.ECX.xTPR]);
		cprintf("\tPerfmon and Debug Capability                             PDCM [%c]\n", \
			powered[features->Std.ECX.PDCM]);
		cprintf("\tProcess Context Identifiers                              PCID [%c]\n", \
			powered[features->Std.ECX.PCID]);
		cprintf("\tDirect Cache Access                                       DCA [%c]\n", \
			powered[features->Std.ECX.DCA]);
		cprintf("\tExtended xAPIC Support                                 x2APIC [%c]\n", \
			powered[features->Std.ECX.x2APIC]);
		cprintf("\tTime Stamp Counter Deadline                      TSC-DEADLINE [%c]\n", \
			powered[features->Std.ECX.TSCDEAD]);
		cprintf("\tXSAVE/XSTOR States                                      XSAVE [%c]\n", \
			powered[features->Std.ECX.XSAVE]);
		cprintf("\tOS-Enabled Extended State Management                  OSXSAVE [%c]\n", \
			powered[features->Std.ECX.OSXSAVE]);
		cprintf("\tExecution Disable Bit Support (when PAE enabled)       XD-Bit [%c]\n", \
			powered[features->Ext.EDX.XD_Bit]);
		cprintf("\t1 GB Pages Support                                  1GB-PAGES [%c]\n", \
			powered[features->Ext.EDX.PG_1GB]);
		cprintf("\tInstruction set:   FPU [%c]  CX8 [%c]  SEP [%c]  CMOV [%c]  CLFSH [%c]\n", \
			powered[features->Std.EDX.FPU], \
			powered[features->Std.EDX.CX8], \
			powered[features->Std.EDX.SEP], \
			powered[features->Std.EDX.CMOV], \
			powered[features->Std.EDX.CLFSH] \
			);
		cprintf("\t                   MMX [%c] FXSR [%c]  SSE [%c]  SSE2 [%c]   SSE3 [%c]\n", \
			powered[features->Std.EDX.MMX], \
			powered[features->Std.EDX.FXSR], \
			powered[features->Std.EDX.SSE], \
			powered[features->Std.EDX.SSE2], \
			powered[features->Std.ECX.SSE3] \
			);
		cprintf("\t                   SSSE3 [%c]  SSE4.1 [%c]  SSE4.2 [%c]  PCLMULDQ[%c]\n", \
			powered[features->Std.ECX.SSSE3], \
			powered[features->Std.ECX.SSE41], \
			powered[features->Std.ECX.SSE42], \
			powered[features->Std.ECX.PCLMULDQ] \
			);
		cprintf("\t                   MONITOR [%c]  CX16 [%c]   MOVBE [%c]   POPCNT [%c]\n", \
			powered[features->Std.ECX.MONITOR], \
			powered[features->Std.ECX.CX16], \
			powered[features->Std.ECX.MOVBE], \
			powered[features->Std.ECX.POPCNT] \
			);
		cprintf("\t                   AES [%c]       AVX [%c]    F16C [%c]   RDRAND [%c]\n", \
			powered[features->Std.ECX.AES], \
			powered[features->Std.ECX.AVX], \
			powered[features->Std.ECX.F16C], \
			powered[features->Std.ECX.RDRAND] \
			);
		cprintf("\t                   LAHF/SAHF [%c]  SYSCALL [%c] RDTSCP [%c] IA64 [%c]\n", \
			powered[features->Ext.ECX.LAHFSAHF], \
			powered[features->Ext.EDX.SYSCALL], \
			powered[features->Ext.EDX.RDTSCP], \
			powered[features->Ext.EDX.IA64] \
			);
	}
}

void SYS_Show_Info(struct BIOSINFO *bios, \
		   struct BOARDINFO *board, \
		   struct MEMARRAY  *memArray, \
		   struct MEMDEV **memory )
{
	if(bios!=NULL && board!=NULL && memArray!=NULL && memory!=NULL)
	{
		unsigned long long totalMemSize=0;
		int                ix=0;

		cflush();
		cprintf("\n");
		cprintf("\n");
		cprintf("\tBoard: %s\n", \
			SMB_Find_String((struct STRUCTINFO*) board, board->Attrib->Product));
		cprintf("\t |- version %s\n", \
			SMB_Find_String((struct STRUCTINFO*) board, board->Attrib->Version));
		cprintf("\t |- manufactured by %s\n", \
			SMB_Find_String((struct STRUCTINFO*) board, board->Attrib->Manufacturer));
		cprintf("\n");
		cprintf("\tBIOS: %s\n", \
			SMB_Find_String((struct STRUCTINFO*) bios, bios->Attrib->Vendor));
		cprintf("\t |- version %s\n", \
			SMB_Find_String((struct STRUCTINFO*) bios, bios->Attrib->Version));
		cprintf("\t |- released date %s\n", \
			SMB_Find_String((struct STRUCTINFO*) bios, bios->Attrib->Release_Date));
		cprintf("\t |- revision %lld.%lld\n", \
			bios->Attrib->Major_Release, bios->Attrib->Minor_Release);
		cprintf("\t |- ROM Size: %lld KB at 0x%04llX\n", \
			64 * (1 + bios->Attrib->ROM_Size), bios->Attrib->Address);
		for(ix=0; ix < memArray->Attrib->Number_Devices; ix++)
			totalMemSize+=memory[ix]->Attrib->Size;
		cprintf("\n");
		cprintf("\tRAM: %lld/%lld MB\n", totalMemSize, memArray->Attrib->Maximum_Capacity/1024);
		for(ix=0; ix < memArray->Attrib->Number_Devices; ix++)
			cprintf("\t |- %s:%s  %lld MB @ %lld MHz\n", \
			    SMB_Find_String((struct STRUCTINFO*) memory[ix], memory[ix]->Attrib->Socket), \
			    SMB_Find_String((struct STRUCTINFO*) memory[ix], memory[ix]->Attrib->Bank), \
			    memory[ix]->Attrib->Size, memory[ix]->Attrib->Speed);
	}
}

void HELP_Show()
{
	int         uid=geteuid(), iopl3=iopl(3), iopl0=iopl(0),
	            dmiLoaded=0, msrLoaded=0,
		    cpuidReady=0;
	char        VendorID[12+1]={0};

	// Inspired from src/linux-3.5/kernel/module.c
	char          buffer[256], modName[64]={0};
	const char   *pathName="/proc/modules";
	FILE         *fProc=fopen(pathName, "r");

	while(!feof(fProc))
		if( (fgets(buffer, 256, fProc) > 0) && (sscanf(buffer, "%s", modName) > 0) )
		{
			if(!strcmp(modName, "dmi_sysfs")) dmiLoaded=1;
			if(!strcmp(modName, "msr")) msrLoaded=1;
		}
	if(fProc) fclose(fProc);

#ifdef __x86_64__
	__asm__ volatile
	(
		"pushf;"
		"pop	%%rax;"
		"movq	%%rax, %%rcx;"
		"xorq	$0x200000, %%rax;"
		"push	%%rax;"
		"popf;"
		"pushf;"
		"pop	%%rax;"
		"xorq	%%rcx, %%rax;"
		: "=a"	(cpuidReady)
	);
	if(cpuidReady)
	{
		int EBX=0, EDX=0, ECX=0;
		__asm__ volatile
		(
			"xorq	%%rax, %%rax;"
			"cpuid;"
			: "=b"	(EBX),
			  "=d"	(EDX),
			  "=c"	(ECX)
		);
		VendorID[0]=EBX; VendorID[1]=(EBX >> 8); VendorID[2]= (EBX >> 16); VendorID[3]= (EBX >> 24);
		VendorID[4]=EDX; VendorID[5]=(EDX >> 8); VendorID[6]= (EDX >> 16); VendorID[7]= (EDX >> 24);
		VendorID[8]=ECX; VendorID[9]=(ECX >> 8); VendorID[10]=(ECX >> 16); VendorID[11]=(ECX >> 24);
	}
#endif /* __x86_64__ */

	cflush();
	cprintf("\n");
	cprintf("\n");
	cprintf("\n");
	cprintf("\t%s\n", version);
	cprintf("\n");
	cprintf("\n");
	cprintf("\tRoot Access Granted     ...\t[%2s]\n", !uid ? "OK":"NO");
	cprintf("\n");
	cprintf("\tI/O Privilege Level 3   ...\t[%2s]\n", !iopl3 ? "OK":"NO");
	cprintf("\n");
	cprintf("\tI/O Privilege Level 0   ...\t[%2s]\n", !iopl0 ? "OK":"NO");
	cprintf("\n");
	cprintf("\t/proc/modules Readable  ...\t[%2s]", fProc ? "OK":"NO");
	cprintf("\n");
	cprintf("\t     |--- dmi-sysfs.ko  ...\t[%2s]", dmiLoaded ? "OK":"NO");
	cprintf("\n");
	cprintf("\t     |--- msr.ko        ...\t[%2s]\n", msrLoaded ? "OK":"NO");
	cprintf("\n");
	cprintf("\tCPUID Available         ...\t[%2s]\n", cpuidReady ? "OK":"NO");
	cprintf("\n");
	cprintf("\tProcessor\t\t[%12s]\n", VendorID);
}


int main(int argc, char *argv[])
{
	int    ix=0, menuLevel=1, function=FN_HELP;

	struct termios oldt, newt;
	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	newt.c_cc[VTIME] = 0;
	newt.c_cc[VMIN] = 1;
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );

	struct SMBIOS_TREE Smb=
	{
		BIOS_Read_Info(),
		BOARD_Read_Info(),
		PROC_Read_Info(),
		{CACHE_Read_Info(0), CACHE_Read_Info(1), CACHE_Read_Info(2)},
		MEM_Read_Array(),
		MEM_ReadAll_Devices(Smb.MemArray)
	};

	struct HAL_TREE Hal=
	{
		FEATURES_Read_Info(),
		CORE_Read_Info((Smb.Proc!=NULL) ? Smb.Proc->Attrib->ThreadCount : Hal.Features->ThreadCount),
		IMC_Read_Info()
	};

	if(Smb.MemArray!=NULL)
		for(ix=0; ix < Smb.MemArray->Attrib->Number_Devices; ix++)
		{
			menuPrompt[LV_ATTY][23+ix*6]='[';
			menuPrompt[LV_ATTY][24+ix*6]='1'+ix;
			menuPrompt[LV_ATTY][25+ix*6]=']';
			menuNav[LV_ATTY][3+ix].Command='1'+ix;
			menuNav[LV_ATTY][3+ix].Function=FN_ATTD+ix;
			menuPrompt[LV_STRY][23+ix*6]='[';
			menuPrompt[LV_STRY][24+ix*6]='1'+ix;
			menuPrompt[LV_STRY][25+ix*6]=']';
			menuNav[LV_STRY][3+ix].Command='1'+ix;
			menuNav[LV_STRY][3+ix].Function=FN_STRD+ix;
		}

	while(function != FN_EXIT)
	{
		switch(function)
		{
			case FN_BACK:	cflush();
			break;
			case FN_HELP:	HELP_Show();
			break;
			case FN_SHWC:	CORE_Update_Info(Hal.Core, \
							(Smb.Proc!=NULL) ? \
							Smb.Proc->Attrib->ThreadCount \
							: Hal.Features->ThreadCount);
					CORE_Show_Temp(Hal.Core, Smb.Proc, \
							(Smb.Proc!=NULL) ? \
							Smb.Proc->Attrib->ThreadCount \
							: Hal.Features->ThreadCount);
			break;
			case FN_CMDF:	CORE_Update_Info(Hal.Core, \
							(Smb.Proc!=NULL) ? \
							Smb.Proc->Attrib->ThreadCount \
							: Hal.Features->ThreadCount);
					CORE_Show_Info(Hal.Core, Smb.Proc, \
							(Smb.Proc!=NULL) ? \
							Smb.Proc->Attrib->ThreadCount \
							: Hal.Features->ThreadCount);
			break;
			case FN_CMDT:	IMC_Show_Info(Hal.Imc);
			break;
			case FN_CMOD:	CORE_Mod(Hal.Core, \
						(Smb.Proc!=NULL) ? \
						Smb.Proc->Attrib->ThreadCount \
						: Hal.Features->ThreadCount);
			break;
			case FN_SHWP:	PROC_Show_Info(Smb.Proc, Smb.Cache);
			break;
			case FN_SHWF:	FEATURES_Show_Info(Hal.Features);
			break;
			case FN_SHWS:	SYS_Show_Info(Smb.Bios, Smb.Board, Smb.MemArray, Smb.Memory);
			break;
			case FN_ATTB:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Bios);
			break;
			case FN_ATTM:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Board);
			break;
			case FN_ATTP:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Proc);
			break;
			case FN_ATL1:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Cache[0]);
			break;
			case FN_ATL2:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Cache[1]);
			break;
			case FN_ATL3:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Cache[2]);
			break;
			case FN_ATTY:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.MemArray);
			break;
			case FN_ATTD+0:
			case FN_ATTD+1:
			case FN_ATTD+2:
			case FN_ATTD+3:
			case FN_ATTD+4:
			case FN_ATTD+5:
			case FN_ATTD+6:
			case FN_ATTD+7:	SMB_Dump_Attribs((struct STRUCTINFO*) Smb.Memory[function-FN_ATTD]);
			break;
			case FN_STRB:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Bios);
			break;
			case FN_STRM:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Board);
			break;
			case FN_STRP:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Proc);
			break;
			case FN_STL1:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Cache[0]);
			break;
			case FN_STL2:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Cache[1]);
			break;
			case FN_STL3:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Cache[2]);
			break;
			case FN_STRY:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.MemArray);
			break;
			case FN_STRD+0:
			case FN_STRD+1:
			case FN_STRD+2:
			case FN_STRD+3:
			case FN_STRD+4:
			case FN_STRD+5:
			case FN_STRD+6:
			case FN_STRD+7:	SMB_Dump_Strings((struct STRUCTINFO*) Smb.Memory[function-FN_STRD]);
		}
		function=Menu_Display(&menuLevel);
	};

	IMC_Free_Info(Hal.Imc);
	FEATURES_Free_Info(Hal.Features);
	CORE_Free_Info(Hal.Core);
	MEM_FreeAll_Devices(Smb.Memory, Smb.MemArray);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.MemArray);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Cache[2]);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Cache[1]);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Cache[0]);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Proc);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Board);
	BIOS_Free_Structure((struct STRUCTINFO*) Smb.Bios);

	tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
	printf("\n");

	return(function);
}
