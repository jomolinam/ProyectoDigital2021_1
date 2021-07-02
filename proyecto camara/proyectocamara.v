module proyectocamara#(
parameter AW = 15, DW = 3
)
(
input VSYNC,
input HREF,
input P_clk,
input [7:0] D,
input car_stop,
output X_clk,
output reset,
output PWDN,
output wire RED,
output wire GREEN,
output wire BLUE
);

wire regwrite;
wire [DW-1:0] data_w;
wire [AW-1:0] addr_w;
wire read;
wire [DW-1:0] data_r;
wire [AW-1:0] addr_r;
reg clk_r;


driver_cam driver_cam(
.car_stop(car_stop),
.VSYNC(VSYNC),
.HREF(HREF),
.D(D),
.P_clk(P_clk),
.regwrite(regwrite),
.data_w(data_w),
.addr(addr_w),
.read(read)
);

buffer_ram buffer_ram(
.clk_w(P_clk),
.addr_in(addr_w), 
.data_in(data_w), 
.regwrite(regwrite),
.addr_out(addr_r), 
.data_out(data_r) 
);

colors colors (
.read(read),
.data(data_r),
.P_clk(P_clk),
.addr(addr_r),
.RED_out(RED),
.GREEN_out(GREEN),
.BLUE_out(BLUE)
);


endmodule
