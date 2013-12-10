/*** 3-bit counter ***/
module counter3(clk,out);
	input clk;					// Clock Signal
	input rst;					// Reset Signal
	output [2:0] out;			// Four-bit Output
	reg [2:0] out;

	initial out = 0;

endmodule

module d_ff(clk, d, q, _q);
	input clk;
	input d;
	output q, _q;

endmodule

