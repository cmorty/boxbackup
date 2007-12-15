// --------------------------------------------------------------------------
//
// File
//		Name:    BoxPortsAndFiles.h
//		Purpose: Central list of which tcp/ip ports and hardcoded file locations
//		Created: 2003/08/20
//
// --------------------------------------------------------------------------

#ifndef BOXPORTSANDFILES__H
#define BOXPORTSANDFILES__H

#define BOX_PORT_BASE		2200


// Backup store daemon
#define BOX_PORT_BBSTORED			(BOX_PORT_BASE+1)

// directory within the RAIDFILE root for the backup store daemon
#define BOX_RAIDFILE_ROOT_BBSTORED		"backup"

// configuration file paths
#ifdef WIN32
	// no default config file path, use these macros to call
	// GetDefaultConfigFilePath() instead.

	#define BOX_GET_DEFAULT_BBACKUPD_CONFIG_FILE \
		GetDefaultConfigFilePath("bbackupd.conf").c_str()
	#define BOX_GET_DEFAULT_RAIDFILE_CONFIG_FILE \
		GetDefaultConfigFilePath("raidfile.conf").c_str()
	#define BOX_GET_DEFAULT_BBSTORED_CONFIG_FILE \
		GetDefaultConfigFilePath("bbstored.conf").c_str()
#else
#define BOX_FILE_BBACKUPD_DEFAULT_CONFIG	"/etc/box/bbackupd.conf"
#define BOX_FILE_RAIDFILE_DEFAULT_CONFIG	"/etc/box/raidfile.conf"
#define BOX_FILE_BBSTORED_DEFAULT_CONFIG 	"/etc/box/bbstored.conf"
#endif

#endif // BOXPORTSANDFILES__H
