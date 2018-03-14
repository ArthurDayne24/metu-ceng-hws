`timescale 1ns / 1ps
module Computer(
input Reset, 
input Clk
);

wire [2:0] Status; // processor status signal 0:p_reset, 1:fetch, 2:execute, 3:memop

//
//Write your code below
//

wire [23:0] memAddr;

wire [31:0] fromMemData;
wire [31:0] toMemData;

wire [0:0] memLength;
wire [0:0] memRd;
wire [0:0] memWr;
wire [0:0] memEnable;
wire [0:0] memRdy;

Memory memory(
    memAddr,
    memLength,
    memRd,
    memWr,
    memEnable,
    memRdy,
    toMemData,
    fromMemData
    );

Processor processor(
    memAddr,
    fromMemData,
    toMemData,
    memLength,
    memRd,
    memWr,
    memEnable,
    memRdy,
    Status,
    Reset,
    Clk
    );

endmodule
