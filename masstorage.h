#if !defined(__MASSTORAGE_H__)
#define __MASSTORAGE_H__

#include <inttypes.h>
#include <avr/pgmspace.h>
#include "avrpins.h"
#include "max3421e.h"
#include "usbhost.h"
#include "usb_ch9.h"
#include "Usb.h"
#include <WProgram.h>

#include "..\DebugTools\printhex.h"
#include "..\DebugTools\hexdump.h"
#include "..\DebugTools\message.h"

#include "confdescparser.h"

//#define bmREQ_CDCOUT        USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE
//#define bmREQ_CDCIN         USB_SETUP_DEVICE_TO_HOST|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_INTERFACE 

// Mass Storage Subclass Constants
#define MASS_SUBCLASS_SCSI_NOT_REPORTED		0x00	// De facto use	
#define MASS_SUBCLASS_RBC					0x01	
#define MASS_SUBCLASS_ATAPI					0x02	// MMC-5 (ATAPI)
#define MASS_SUBCLASS_OBSOLETE1				0x03	// Was QIC-157
#define MASS_SUBCLASS_UFI					0x04	// Specifies how to interface Floppy Disk Drives to USB
#define MASS_SUBCLASS_OBSOLETE2				0x05	// Was SFF-8070i
#define MASS_SUBCLASS_SCSI					0x06	// SCSI Transparent Command Set
#define MASS_SUBCLASS_LSDFS					0x07	// Specifies how host has to negotiate access before trying SCSI
#define MASS_SUBCLASS_IEEE1667				0x08	

// Mass Storage Class Protocols
#define MASS_PROTO_CBI						0x00	// CBI (with command completion interrupt)	
#define MASS_PROTO_CBI_NO_INT				0x01	// CBI (without command completion interrupt)	
#define MASS_PROTO_OBSOLETE					0x02	
#define MASS_PROTO_BBB						0x50	// Bulk Only Transport	
#define MASS_PROTO_UAS						0x62		

// Request Codes
#define MASS_REQ_ADSC						0x00		
#define MASS_REQ_GET						0xFC		
#define MASS_REQ_PUT						0xFD		
#define MASS_REQ_GET_MAX_LUN				0xFE		
#define MASS_REQ_BOMSR						0xFF	// Bulk-Only Mass Storage Reset		

#define MASS_CBW_SIGNATURE					0x43425355
#define MASS_CBS_SIGNATURE					0x53425355

struct CommandBlockWrapper
{
	uint32_t	dCBWSignature;
	uint32_t	dCBWTag;
	uint32_t	dCBWDataTransferLength;
	uint8_t		bmCBWFlags;

	struct
	{
		uint8_t	bCBWLUN			: 4; 
		uint8_t	bReserved1		: 4;
	};
	struct
	{
		uint8_t	bCBWCBLength	: 4;
		uint8_t bReserved2		: 4;
	};

	uint8_t		CBWCB[16];
};

struct CommandStatusWrapper
{
	uint32_t	dCSWSignature;
	uint32_t	dCSWTag;
	uint32_t	dCSWDataResidue;
	uint8_t		bCSWStatus;
};

#define MASS_MAX_ENDPOINTS		3

class BulkOnly : public USBDeviceConfig, public UsbConfigXtracter
{
protected:
	static const uint8_t	epDataInIndex;			// DataIn endpoint index
	static const uint8_t	epDataOutIndex;			// DataOUT endpoint index
	static const uint8_t	epInterruptInIndex;		// InterruptIN  endpoint index

	USB			*pUsb;
//	CDCAsyncOper	*pAsync;
	uint8_t		bAddress;
	uint8_t		bConfNum;				// configuration number
	uint8_t		bIface;					// interface value
	uint8_t		bNumEP;					// total number of EP in the configuration
	uint32_t	qNextPollTime;			// next poll time
	bool		bPollEnable;			// poll enable flag

	EpInfo		epInfo[MASS_MAX_ENDPOINTS];

	void PrintEndpointDescriptor(const USB_ENDPOINT_DESCRIPTOR* ep_ptr);

public:
	BulkOnly(USB *p);

	uint8_t Reset();
	uint8_t GetMaxLun(uint8_t *max_lun);

	uint8_t ResetRecovery();

	// USBDeviceConfig implementation
	virtual uint8_t Init(uint8_t parent, uint8_t port, bool lowspeed);
	virtual uint8_t Release();
	virtual uint8_t Poll();
	virtual uint8_t GetAddress() { return bAddress; };

	// UsbConfigXtracter implementation
	virtual void EndpointXtract(uint8_t conf, uint8_t iface, uint8_t alt, uint8_t proto, const USB_ENDPOINT_DESCRIPTOR *ep);
};

#endif // __MASSTORAGE_H__