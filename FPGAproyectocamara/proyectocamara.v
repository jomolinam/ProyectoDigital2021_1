module proyectocamara#(
parameter AW = 15, DW = 3
)
(
input VSYNC,
input HREF,
input P,
input [7:0] D,
input car_stop,
input clk,
input rst,
output X_clk,
output wire RED_L,
output wire GREEN_L,
output wire BLUE_L,
output wire RED_P,
output wire GREEN_P,
output wire BLUE_P
);

wire regwrite;
wire [DW-1:0] data_w;
wire [AW-1:0] addr_w;
wire read;
wire [DW-1:0] data_r;
wire [AW-1:0] addr_r;
wire clk_r;
wire RED;
wire GREEN;
wire BLUE;

//wire image_s;
//wire [DW-1:0] data_2;
//wire [AW-1:0] addr_2;

//assign X_clk=clk_r; 


clk_wiz_0 divisordefrecuencia(
.clk_in1(clk),
.clk_out1(X_clk)
);

driver_cam driver_cam(
.car_stop(car_stop),
.VSYNC(VSYNC),
.HREF(HREF),
.D(D),
.P(P),
.rst(rst),
.regwrite(regwrite),
.data_w(data_w),
.addr(addr_w),
.read(read)
);

buffer_ram buffer_ram(
.clk_w(P),
.addr_in(addr_w), 
.data_in(data_w), 
.regwrite(regwrite),
.addr_out(addr_r), 
.rst(rst),
.data_out(data_r)
);

colors colors (
.read(read),
.data(data_r),
.P(P),
.rst(rst),
.addr(addr_r),
.RED_out(RED),
.GREEN_out(GREEN),
.BLUE_out(BLUE)
);


assign RED_L = RED;
assign GREEN_L = GREEN;
assign BLUE_L = BLUE;
assign RED_P = RED;
assign GREEN_P = GREEN;
assign BLUE_P = BLUE;

endmodule