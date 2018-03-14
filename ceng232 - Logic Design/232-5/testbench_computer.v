`timescale 1ns / 1ps

module testbench_computer(
    );
	 
reg Clk;	 
reg Reset;
initial Clk =0;
always #20 Clk = ~Clk;

Computer COMPUTER_INSTANCE (Reset,Clk);

initial 
begin
	$display("Starting Testbench");
	Reset =1 ;
	
	#19;
	Reset=0;
	
	#630;
	$finish;
end
endmodule
