Instructions

Introduction
Create two files: pci.h and pci.c. These will eventually link into virtio.h and virtio.c, but for this lab, we will only be enumerating the PCI bus, configuring bridges, configuring devices, and setting BARs for future access.

ECAM Structure
struct pci_ecam {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command_reg;
    uint16_t status_reg;
    uint8_t revision_id;
    uint8_t prog_if;
    union {
        uint16_t class_code;
        struct {
            uint8_t class_subcode;
            uint8_t class_basecode;
        };
    };
    uint8_t cacheline_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    union {
        struct {
            uint32_t bar[6];
            uint32_t cardbus_cis_pointer;
            uint16_t sub_vendor_id;
            uint16_t sub_device_id;
            uint32_t expansion_rom_addr;
            uint8_t capes_pointer;
            uint8_t reserved0[3];
            uint32_t reserved1;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint8_t min_gnt;
            uint8_t max_lat;
        } type0;
        struct {
            uint32_t bar[2];
            uint8_t primary_bus_no;
            uint8_t secondary_bus_no;
            uint8_t subordinate_bus_no;
            uint8_t secondary_latency_timer;
            uint8_t io_base;
            uint8_t io_limit;
            uint16_t secondary_status;
            uint16_t memory_base;
            uint16_t memory_limit;
            uint16_t prefetch_memory_base;
            uint16_t prefetch_memory_limit;
            uint32_t prefetch_base_upper;
            uint32_t prefetch_limit_upper;
            uint16_t io_base_upper;
            uint16_t io_limit_upper;
            uint8_t capes_pointer;
            uint8_t reserved0[3];
            uint32_t expansion_rom_addr;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint16_t bridge_control;
        } type1;
        struct {
            uint32_t reserved0[9];
            uint8_t capes_pointer;
            uint8_t reserved1[3];
            uint32_t reserved2;
            uint8_t interrupt_line;
            uint8_t interrupt_pin;
            uint8_t reserved3[2];
        } common;
    };
};

struct pci_cape {
    uint8_t id;
    uint8_t next;
};
The structures above come from the PCI manual.
There are a lot of fields in the ECAM, but some are only relevant to a type 0 (device) or type 1 (bridge).

You will not see the devices behind a bridge until the bridge is configured to forward MMIO requests at that point.
PCI Initialization
Create and export a function prototyped below.

void pci_init();
This function will be called from main.c when you uncomment USE_PCI in config.h.

Enumerating the PCI Bus
The first thing you need to do is enumerate the PCI bus. Recall that the ECAM starts at the MMIO address 0x3000_0000. From here, the address encodes the bus/slot/device numbers as follows.

0011 [00000111]  [01101] [  011  ]  [   0000   ]  [0000 00] 00
     [ Bus #  ]  [Dev #] [Func # ]  [Ext Reg # ]  [ Reg # ]
BITS:  27:20      19:15    14:12        11:8         7:2
The bits above have the following meanings.

Memory Address (A)	Size (bits)	Meaning
A[(20 + n - 1):20]

1 to 8

Bus number

A[19:15]

5

Device number

A[14:12]

3

Function number

A[11:8]

4

Extended register number

A[7:2]

6

Register number

A[1:0]

2

Used to generate byte enables

The ECAM header starts as follows.

pci ecam header

The first field you need to check is the Vendor ID. If the device is not connected, this field will be 0xFFFF. If a device isn’t connected, you cannot configure it, so you can move to the next slot or bus.

When a device is connected (VendorID != 0xFFFF), you need to determine if it is a bridge (type 1 device) or a device (type 0 device). The configuration is different depending on this.

The 1 or 0 comes from the Header Type field (uint8_t header_type). You must configure the bridges, otherwise, the devices behind the bridges will not respond to MMIO requests.

Bridges (Type-1)
Configure your bridges to ONLY forward the memory addresses required for the devices behind the bridges. Recall you can do this by setting the memory base and limit’s upper 16 bits. Also, recall that you need to set the primary bus number to the bus that the bridge is connected to, and then set the secondary and subordinate bus numbers to the next bus available.

Make sure that no two bridges have the same secondary bus number.
Below shows what happens if you do not enumerate the bridges first. Notice that no devices are present in the "info pci" system because none of the bridges are forwarding MMIO.

(qemu) info pci
  Bus  0, device   0, function 0:
    Host bridge: PCI device 1b36:0008
      PCI subsystem 1af4:1100
      id ""
  Bus  0, device   1, function 0:
    PCI bridge: PCI device 1b36:000c
      IRQ 0, pin A
      BUS 0.
      secondary bus 0.
      subordinate bus 0.
      IO range [0xf000, 0x0fff]
      memory range [0xfff00000, 0x000fffff]
      prefetchable memory range [0xfff00000, 0x000fffff]
      BAR0: 32 bit memory at 0xffffffffffffffff [0x00000ffe].
      id "bridge1"
  Bus  0, device   2, function 0:
    PCI bridge: PCI device 1b36:000c
      IRQ 0, pin A
      BUS 0.
      secondary bus 0.
      subordinate bus 0.
      IO range [0xf000, 0x0fff]
      memory range [0xfff00000, 0x000fffff]
      prefetchable memory range [0xfff00000, 0x000fffff]
      BAR0: 32 bit memory at 0xffffffffffffffff [0x00000ffe].
      id "bridge2"
  Bus  0, device   3, function 0:
    PCI bridge: PCI device 1b36:000c
      IRQ 0, pin A
      BUS 0.
      secondary bus 0.
      subordinate bus 0.
      IO range [0xf000, 0x0fff]
      memory range [0xfff00000, 0x000fffff]
      prefetchable memory range [0xfff00000, 0x000fffff]
      BAR0: 32 bit memory at 0xffffffffffffffff [0x00000ffe].
      id "bridge3"
  Bus  0, device   4, function 0:
    PCI bridge: PCI device 1b36:000c
      IRQ 0, pin A
      BUS 0.
      secondary bus 0.
      subordinate bus 0.
      IO range [0xf000, 0x0fff]
      memory range [0xfff00000, 0x000fffff]
      prefetchable memory range [0xfff00000, 0x000fffff]
      BAR0: 32 bit memory at 0xffffffffffffffff [0x00000ffe].
      id "bridge4"
(qemu)
After enumerating the bridge, we can see the PCI devices behind it.

(qemu) info pci
  Bus  0, device   0, function 0:
    Host bridge: PCI device 1b36:0008
      PCI subsystem 1af4:1100
      id ""
  Bus  0, device   1, function 0:
    PCI bridge: PCI device 1b36:000c
      IRQ 0, pin A
      BUS 0.
      secondary bus 1.
      subordinate bus 1.
      IO range [0xf000, 0x0fff]
      memory range [0x41000000, 0x41ffffff]
      prefetchable memory range [0x41000000, 0x41ffffff]
      BAR0: 32 bit memory at 0x00000000 [0x00000fff].
      id "bridge1"
  Bus  1, device   0, function 0:
    Class 0255: PCI device 1af4:1044
      PCI subsystem 1af4:1100
      IRQ 0, pin A
      BAR1: 32 bit memory at 0x41000000 [0x41000fff].
      BAR4: 64 bit prefetchable memory at 0x41010000 [0x41013fff].
      id "rng"
  Bus  1, device   1, function 0:
    Keyboard: PCI device 1af4:1052
      PCI subsystem 1af4:1100
      IRQ 0, pin A
      BAR1: 32 bit memory at 0x41040000 [0x41040fff].
      BAR4: 64 bit prefetchable memory at 0x41050000 [0x41053fff].
      id "keyboard"
  Bus  1, device   2, function 0:
    Class 2432: PCI device 1af4:1052
      PCI subsystem 1af4:1100
      IRQ 0, pin A
      BAR1: 32 bit memory at 0x41080000 [0x41080fff].
      BAR4: 64 bit prefetchable memory at 0x41090000 [0x41093fff].
      id "tablet"
Now you can see that Bus 1 is readable and contains the keyboard, tablet, and random number generator (aka entropy device).

You need to make sure the BAR addresses do not overlap. However, you also need to make sure the memory address you place in the BAR is within the base + limit of the bridges the device is behind.
Do NOT set the bridge BARs. You will not be configuring bridges beyond the memory limits and bus numbers.
Devices (Type-0)
Configure all type 0 devices and set the BARs. Recall that you need to determine the address space needed by writing -1 into a BAR. Also, remember there is a difference between 32-bit and 64-bit BARs.

Make sure that the memory addresses you assign in the BARs do NOT overlap either with different BARs in the same device or different devices.
PCI IRQs
Recall that PCI IRQs are assigned by the bus and slot number.

IRQ#=32+(bus+slot)mod4
So, IRQs 32, 33, 34, and 35 are assigned to the PCI system.

You will need to enable all of these with the PLIC. Recall that after a PCI device is finished with its work, whether successfully or an error, it will send an IRQ to let you know.

Create a handoff function prototyped below.

void pci_dispatch_irq(int irq_num);
This function will look through the devices and see who caused the interrupt. Recall that since there are only four IRQ numbers, PCI devices must share them. So, to determine who caused it, you need to look at the device itself, which we will do when we look at VirtIO.

This function needs to be called from the PLIC, which contains the following code.

void plic_handle_irq(int hart)
{
    int irq = plic_claim(hart);

    switch (irq) {
        // PCI devices 32-35
        case PLIC_PCI_INTA: [[fallthrough]]
        case PLIC_PCI_INTB: [[fallthrough]]
        case PLIC_PCI_INTC: [[fallthrough]]
        case PLIC_PCI_INTD:
#ifdef USE_PCI
            pci_dispatch_irq(irq);
#endif
            break;
    }

    plic_complete(hart, irq);
}
This function allows us to add other IRQs, but this is only called in response to supervisor interrupts. The SBI configures the UART to interrupt in machine mode, so it bypasses this function and is instead, handled by the SBI.