`timescale 1ns / 1ps
module Processor(
output  [23:0] MemAddr  , // memory address bus
input [31:0] fromMemData  , // data bus from memory 
output  [31:0] toMemData, // data bus to memory 
output  MemLength, // memory operand length
output  MemRd, // memory read control signal
output  MemWr, // memory write control signal
output  MemEnable, // memory enable signal
input MemRdy, // memory completion signal
output  [2:0] Status, // processor status signal 0:p_reset, 1:fetch, 2:execute, 3:memop
input Reset , // reset signal
input Clk // clock signal
    );

//
//Write your code below
//

wire [31:0] Instr;

wire [0:0] ZE;
wire [0:0] NG;
wire [0:0] CY;
wire [0:0] OV;

wire [4:0] AddrA;
wire [4:0] AddrB;
wire [4:0] AddrC;

wire [3:0] AluOp;

wire [0:0] WrC;
wire [0:0] WrPC;
wire [0:0] WrCR;
wire [0:0] WrIR;

wire [0:0] MemAlu;
wire [0:0] PC_RA;
wire [0:0] IR_RB;

wire [0:0] AluPc;
wire [0:0] ZE_SE;
wire [0:0] Sin_Sout;

Data_Subsystem dataSubSystem(
    MemAddr,
    fromMemData,
    toMemData,
    Instr,  
    ZE,
    NG,
    CY,
    OV,
    AddrA,
    AddrB,
    AddrC,
    AluOp,
    WrC,
    WrPC,
    WrCR,
    WrIR,
    MemAlu,
    PC_RA,
    IR_RB,
    AluPc,
    ZE_SE,
    Sin_Sout,
    Clk,
    Reset
    );

Ctrl_Subsytem  ctrlSubSystem(
    Instr,
    ZE,
    NG,
    CY,
    OV,
    AddrA,
    AddrB,
    AddrC,
    AluOp,
    WrC,
    WrPC,
    WrCR,
    WrIR,
    MemAlu,
    PC_RA,
    IR_RB,
    AluPc,
    ZE_SE,
    Sin_Sout,
    MemRd,
    MemWr,
    MemLength,
    MemEnable,
    MemRdy,
    Status,
    Clk,
    Reset
    );

endmodule
