`timescale 1ns / 1ps
module Ctrl_Subsystem(
	input [31:0] Instr,
	input ZE, NG, CY, OV ,
	output reg [4:0] AddrA, AddrB, AddrC,
	output reg [3:0] ALUOp,
	output reg WrC, WrPC, WrCR, WrIR,
	output reg Mem_ALU, PC_RA, IR_RB,
	output reg ALU_PC, ZE_SE, Sin_Sout,
	output reg MemRd,MemWr,
	output reg MemLength,
	output reg MemEnable,
	input MemRdy,
	output reg [2:0] Status, // 0:p_reset, 1:fetch, 2:execute, 3:memop
	input Clk, Reset
    );

	localparam p_reset = 2'b00;
	localparam fetch   = 2'b01;
	localparam execute = 2'b10;
	localparam memop   = 2'b11;
	
	localparam Ctrl_delay= 0.5; //500 ps
	localparam Reset_delay= 0.5; //500 ps
	localparam Dec_delay= 3; //3 ns
	localparam MemRd_delay= 2.5; //2500 ps
	localparam MemRd_pulse= MemRd_delay + 3 ; //5500 ps
	localparam MemWr_delay= 2.5; //2500 ps
	localparam MemWr_pulse= MemWr_delay + 3 ; //5500 ps

//
//Write your code below
//
    reg [0:0] TmpMemWr = 0;
    reg [0:0] TmpRS_RB = 0;
    reg [0:0] MemOp = 0;
    reg [0:0] tmpWrC = 0;
    reg [1:0] State = 0;

//wait(Clk or Reset);

always @(posedge Clk)
begin
    if (State == p_reset)
    begin
        Status <= #Ctrl_delay fetch;
        State <= #Ctrl_delay fetch;
    end
    
    else if(State == fetch)
    begin
        Status <= #Ctrl_delay execute;
        State <= #Ctrl_delay execute;
    end
    
    else if(State == execute)
    begin
        if (Instr[31:26] == 33 || Instr[31:26] == 34)
        begin
            Status <= #Ctrl_delay memop;
            State <= #Ctrl_delay memop;
        end
        
        else
        begin
            Status <= #Ctrl_delay fetch;
            State <= #Ctrl_delay fetch;
        end
    end

    else //if (State == memop)
    begin
        Status <= #Ctrl_delay fetch;
        State <= #Ctrl_delay fetch;
    end
end

always @(posedge Reset)
begin
    State  <= #Reset_delay p_reset ;
    Status <= #Reset_delay p_reset ;
end

always @(State)
begin
    if (State == fetch)
    begin
        // disable write signals from previous cycle
        WrCR     <= #Ctrl_delay 1'b0;
        WrC      <= #Ctrl_delay 1'b0;

        // fetch instruction
        ALU_PC   <= #Ctrl_delay  1'b1;
        MemLength<= #Ctrl_delay  1'b1;
        MemEnable<= #Ctrl_delay  1'b1;
        MemRd    <= #MemRd_delay 1'b1;
        MemRd    <= #MemRd_pulse 1'b0;
        Sin_Sout <= #Ctrl_delay  1'b0;

        // increment PC 
        PC_RA    <= #Ctrl_delay 1'b0;
        ALUOp    <= #Ctrl_delay 4'b1110;
        WrIR     <= #Ctrl_delay 1'b1;
        WrPC     <= #Ctrl_delay 1'b1;
    end

    else if (State == execute)
    begin
        // disable signals from fetch cycle
        WrIR     <= #Ctrl_delay 1'b0;
        WrPC     <= #Ctrl_delay 1'b0;
        MemEnable<= #Ctrl_delay 1'b0;
    end

    else if (State == memop)
    begin
        //Instr[31:26] is the opCode!
        if (Instr[31:26] == 24)     //xor
        begin
            WrC      <= #Ctrl_delay 1;
            TmpMemWr <= 0;
        end

        else if (Instr[31:26] == 17) //adi
        begin
            WrC      <= #Ctrl_delay 1;   
            TmpMemWr <= 0;
        end

        else if (Instr[31:26] == 33) //ldw
        begin
            WrC      <= #Ctrl_delay 1;   
            TmpMemWr <= 0;
        end

        else //if (Instr[31:26] == 34) //stb
        begin
            WrC      <= #Ctrl_delay 0;   
            TmpMemWr <= 1;
        end


        if (TmpMemWr == 1'b0)
        begin
          MemRd     <= #MemRd_delay 1'b1;
          MemRd     <= #MemRd_pulse 1'b0;
          Mem_ALU   <= #Ctrl_delay 1'b0;
        end

        else
        begin
          MemWr     <= #MemWr_delay 1'b1;
          MemWr     <= #MemWr_pulse 1'b0;
          Sin_Sout  <= #Ctrl_delay 1'b1;   
        end
    end

    else //if (State == p_reset)
    begin
          ALUOp  <= 4'b0000;
          MemRd  <= 1'b0;    
          MemWr  <= 1'b0; 
          MemEnable <= 1'b0;
          MemLength <= 1'b0;
    end
end

always @(Instr)
begin
    // decode registers
    AddrA   <= #Dec_delay Instr[20:16];

    if (Instr[31:26] == 24)       //xor
    begin
        TmpRS_RB = 1;
        // decode control signals
        PC_RA   <= #Ctrl_delay 1;
        ZE_SE   <= #Ctrl_delay 0;
        IR_RB   <= #Ctrl_delay 1;
        ALUOp   <= #Ctrl_delay 4'b0110;
        WrPC    <= #Ctrl_delay 0;
        WrCR    <= #Ctrl_delay 1;
        MemOp   <= 0;
        tmpWrC  <= 1;
    end

    else if (Instr[31:26] == 17)  //adi
    begin
        TmpRS_RB = 1;
        // decode control signals
        PC_RA   <= #Ctrl_delay 1;
        ZE_SE   <= #Ctrl_delay 1;
        IR_RB   <= #Ctrl_delay 0;
        ALUOp   <= #Ctrl_delay 4'b0001;
        WrPC    <= #Ctrl_delay 0;
        WrCR    <= #Ctrl_delay 1;
        MemOp   <= 0;
        tmpWrC  <= 1;
    end

    else if (Instr[31:26] == 33)  //ldw
    begin
        TmpRS_RB = 0;
        // decode control signals
        PC_RA   <= #Ctrl_delay 1;
        ZE_SE   <= #Ctrl_delay 1;
        IR_RB   <= #Ctrl_delay 0;
        ALUOp   <= #Ctrl_delay 4'b0001;
        WrPC    <= #Ctrl_delay 0;
        WrCR    <= #Ctrl_delay 0;
        MemOp   <= 1;
        tmpWrC  <= 1;
    end

    else //if (Instr[31:26] == 34) //stb
    begin
        TmpRS_RB = 0;
        // decode control signals
        PC_RA   <=  #Ctrl_delay 1;
        ZE_SE   <=  #Ctrl_delay 1;
        IR_RB   <=  #Ctrl_delay 0;
        ALUOp   <=  #Ctrl_delay 4'b0001;
        WrPC    <=  #Ctrl_delay 0;
        WrCR    <=  #Ctrl_delay 0;
        MemOp   <= 1;
        tmpWrC  <= 0;
    end

    if (MemOp == 0)
    begin
        WrC     <= #Ctrl_delay tmpWrC;
        Mem_ALU <= #Ctrl_delay 1;
    end

    if (TmpRS_RB == 0)
    begin
      AddrB <= #Dec_delay Instr[25:21];
    end
    
    else
    begin
      AddrB <= #Dec_delay Instr[15:11];
    end
    
    AddrC   <= #Dec_delay Instr[25:21];

end

always @(posedge MemRdy)
begin
    if (MemRdy == 1)
    begin
        if (State == memop)
        begin
            if (TmpMemWr == 1)
            begin
                Sin_Sout <= #Ctrl_delay 0 ;
            end
        end
    end
end

endmodule
