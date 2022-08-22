// Half duplex style
// Don't care about Port B

// IOCON.BANK = 1 (i.e. non-interlacing)
// Register Addresses
#define MCP23S17_IODIR_A      0x00
#define MCP23S17_POL_A        0x01
#define MCP23S17_GPINTEN_A    0x02
#define MCP23S17_DEFVAL_A     0x03
#define MCP23S17_INTCON_A     0x04

#define MCP23S17_GPPU_A       0x06
#define MCP23S17_INTF_A       0x07
#define MCP23S17_INTCAP_A     0x08
#define MCP23S17_GPIO_A       0x09
#define MCP23S17_OLAT_A       0x0A

// Default Bank 0 Address scheme
#define MCP23S17_BK0_IODIR_A     0x00
#define MCP23S17_BK0_IPOL_A      0x02
#define MCP23S17_BK0_GPINTEN_A   0x04
#define MCP23S17_BK0_DEFVAL_A    0x06
#define MCP23S17_BK0_INTCON_A    0x08
#define MCP23S17_BK0_IOCON_A     0x0A
#define MCP23S17_BK0_GPPU_A      0x0C
#define MCP23S17_BK0_INTF_A      0x0E
#define MCP23S17_BK0_INTCAP_A    0x10
#define MCP23S17_BK0_GPIO_A      0x12
#define MCP23S17_BK0_OLAT_A      0x14

// Address of IOCON while Bank bit is Cleared (default)
#define MCP23S17_IOCR_BANK0   0x0A
// Address of IOCON while Bank bit is Set
#define MCP23S17_IOCR_BANK1   0x05

//  IOCR bit masks (details datasheet P20)
#define MCP23S17_IOCR_BANK    0x80    // Controls how the registers are addressed.
#define MCP23S17_IOCR_MIRROR  0x40    // INT Pins Mirror bit.
#define MCP23S17_IOCR_SEQOP   0x20    // Sequential Operation mode bit.
#define MCP23S17_IOCR_DISSLW  0x10    // Slew Rate control bit for SDA output.
#define MCP23S17_IOCR_HAEN    0x08    // Hardware Address Enable bit (MCP23S17 only).
#define MCP23S17_IOCR_ODR     0x04    // Configures the INT pin as an open-drain output.
#define MCP23S17_IOCR_INTPOL  0x02    // This bit sets the polarity of the INT output pin.
#define MCP23S17_IOCR_NI      0x01    // Not implemented.

#define MCP23S17_WR_OPCODE    0x40    // Device Op code for Write
#define MCP23S17_RD_OPCODE    0x41    // Device Op code for Read

#define MCP23S17_RD_DUMMY     0x00    // Dummy data during read
