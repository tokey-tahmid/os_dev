I need to implement virtIO in my os development code. And the instructions are below:

You will be creating the virtio subsystem by first creating virtio.h and virtio.c. You will not actually configure the devices, but instead, you will configure the VirtIO system, including allocating memory for the descriptor table, driver ring, and device ring. The VirtIO devices will be found on the PCI bus via the vendor ID 0x1AF4. When you see this vendor ID, you know how to handle the initialization of the device. To find where the configuration is attached, you will need to look at the capabilities list. Recall that this is a linked list. The capability ID we are looking for is "vendor specific", which is 0x09. When you find this capability, it will tell you which BAR to look at and an offset from the memory range. Most virtio devices use the same BAR with different offsets to map the device registers. When you find a vendor-specific capability for a VirtIO device, it will have the following structure.

struct virtio_pci_cap {
   u8 cap_vndr;   
   u8 cap_next;   
   u8 cap_len;    
   u8 cfg_type;   
   u8 bar;        
   u8 padding[3]; 
   u32 offset;    
   u32 length; 
};

Configuration Type (cfg_type)
You will know what configuration you are looking at by reading the cfg_type field. This field will be 1, 2, 3, 4, or 5, but we only really care about types 1, 2, and 3.

Common Configuration (type 1)
The type 1 configuration is located in the BAR address space specified in the capability plus the offset. When you dereference this memory address, you will find the VirtioPciCommonCfg struct in virtio.h.

When you select a queue via the queue_select field, the rest of the fields will be updated to reflect that queue. The valid queues you can put into queue_select is 0 through num_queues - 1.

The queue_notify_off field is used with the type 2 (notify) register to notify this specific queue.

The queue_desc field is a physical memory address which points to the top of the descriptor table allocated for this queue. The number of descriptors in the table is defined by queue_size, which can be negotiated.

The queue_driver field is a physical memory address which points to the top of the driver ring allocated for this queue. The number of elements in the ring is defined by queue_size.

The queue_device field is a physical memory address which points to the top of the device ring allocate for this queue. The number of elements in the ring is defined by queue_size.

None of the queues are active until queue_enable is assigned 1.
These tables/rings may span more than one page, so make sure you allocate contiguous physical memory.
Notify Configuration (type 2)
This is a slightly confusing configuration since we get information from two places: an additional field in the capability and the BAR + offset.

A notify register is how we tell the device to "go and do" what we asked it to do. The capability structure adds an additional field called notify_off_multiplier.

struct virtio_pci_notify_cap {
   struct virtio_pci_cap cap;
   u32 notify_off_multiplier;
};
#define BAR_NOTIFY_CAP(offset, queue_notify_off, notify_off_multiplier) \
                      ((offset) + (queue_notify_off) * (notify_off_multiplier))
We need a multiplier since we don’t notify the device directly. Instead, we notify the queue we want action upon. Recall that we can have more than one queue that we configure in type 1.

The actual memory address we write to notify a queue is given by the cap portion of this structure. In there, you will find a BAR and offset. When you go to the address in the BAR + offset, you are now at the top of the structure. When you then add the multiplier, you are looking at a specific queue.

The queue_notify_off comes from the type 1 configuration. It is based on a specific queue, so you need to make sure queue_select contains the queue you’re questioning.

Interrupt Service Routine (type 3)
The interrupt service routine notifies us that this particular device caused an interrupt. Recall that there are only four PCI interrupts, so the devices share them. When we receive an interrupt, we have to see who caused it by looking at this register.

This register has the following structure.

struct virtio_pci_isr_cap {
    union {
       struct {
          unsigned queue_interrupt: 1;
          unsigned device_cfg_interrupt: 1;
          unsigned reserved: 30;
       };
       unsigned int isr_cap;
    };
};
If we read queue_interrupt, and it is 1, that means the device did indeed interrupt and is responding to something we put in the queue. If we read device_cfg_interrupt, and it is 1, that means the device has changed its configuration, and we should reinitialize the device. If it’s none of the above, the interrupt was not caused by this particular device.

Strategy
You need to store pointers for the BAR address + offset. We do not want to enumerate the capabilities any time we need something, so we store the addresses in pointers. Recall that pointers are virtual memory addresses, and the BARs store physical memory addresses.

PCI Configuration Initialization
The PCI configuration (type 1) contains the pointers to physical addresses you need to point the device towards for the descriptor table, driver ring, and device ring.

Interface for interacting with PCIDevice:

PCIDevice *pci_find_saved_device(uint16_t vendor_id, uint16_t device_id);

PCIDevice *pci_save_device(PCIDevice device);

PCIDevice *pci_get_nth_saved_device(uint16_t n);

PCIDevice *pci_find_device_by_irq(uint8_t irq);

uint64_t pci_count_saved_devices(void);
uint64_t pci_count_irq_listeners(uint8_t irq);
struct pci_cape* pci_get_capability(PCIDevice *dev, uint8_t type, uint8_t nth);
struct VirtioCapability *pci_get_virtio_capability(PCIDevice *device, uint8_t virtio_cap_type);
struct VirtioPciCommonCfg *pci_get_virtio_common_config(PCIDevice *device);
struct VirtioPciNotifyCfg *pci_get_virtio_notify_config(PCIDevice *device);
struct VirtioPciISRStatus *pci_get_virtio_isr_status(PCIDevice *device);

so essentially, we go through all the PCIDevices using pci_count_saved_devices as the max number of devices to iterate through, and the pci_get_nth_saved_device to get the ith PCIDevice
then we populate this structure:
typedef struct VirtioDevice {
    struct PCIDevice *pcidev;
    volatile VirtioPciCommonCfg *common_cfg;
    volatile VirtioPciNotifyCap *notify_cap;
    volatile VirtioPciIsrCap *isr;
    volatile VirtioDescriptor *desc;
    volatile VirtioDriverRing *driver;
    volatile VirtioDeviceRing *device;

    void *priv;
    struct List *jobs;

    uint16_t desc_idx;
    uint16_t driver_idx;
    uint16_t device_idx;
    bool     ready;
} VirtioDevice;

and save it into the vector
struct PCIDevice *pcidev;
volatile VirtioPciCommonCfg *common_cfg;
volatile VirtioPciNotifyCap *notify_cap;

volatile VirtioPciIsrCap *isr;

these fields can all be gotten by calling things like pci_get_virtio_common_config, pci_get_virtio_notify_config, pci_get_virtio_isr_status on the PCIDevice that gets stored in the pcidev field

volatile VirtioDescriptor *desc;

volatile VirtioDriverRing *driver;
volatile VirtioDeviceRing *device;

this stuff we allocate and store copies of pointers to the virtual memory

Firstly, I need to write the virtio_init in virtio.c, which goes through and creates all the VirtioDevice structures from the PCIDevices and sets that field.