module colors#(
parameter AW = 15
)
(
input read,
input [2:0] data,
input P,
input rst,
output [AW-1:0] addr,
output reg RED_out,
output reg GREEN_out,
output reg BLUE_out
);

reg [AW-1:0]addr_o = 0;
reg [AW-1:0] RED = 0;
reg [AW-1:0] GREEN = 0;
reg [AW-1:0] BLUE = 0;
assign addr = addr_o;


always @(posedge P)begin
	if(read == 1)begin
		if(addr_o < 15'd25343)begin
			RED = RED + data[2];
			GREEN = GREEN + data[1];
			BLUE = BLUE + data[0];
			addr_o = addr_o + 1;
		end
			if(addr_o == 15'd25343)begin
			if(RED > GREEN && RED > BLUE)begin
				RED_out = 1;
				GREEN_out = 0;
				BLUE_out = 0;
			end else if(GREEN > RED && GREEN > BLUE)begin
				RED_out = 0;
				GREEN_out = 1;
				BLUE_out = 0;
			end else if(BLUE > RED && BLUE > GREEN)begin
				RED_out = 0;
				GREEN_out = 0;
				BLUE_out = 1;
			end else begin
				RED_out = 0;
				GREEN_out = 0;
				BLUE_out = 0;
			end
			addr_o = 0;
		end
	end else begin
	addr_o = 0;
	end
end
//initial begin 
//    data = 0;
//  end
endmodule